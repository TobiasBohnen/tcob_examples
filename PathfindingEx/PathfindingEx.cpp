// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PathfindingEx.hpp"

#include <algorithm>
#include <queue>

PathfindingEx::PathfindingEx(game& game)
    : scene {game}
{
    window().ClearColor = colors::Black;
}

PathfindingEx::~PathfindingEx() = default;

auto PathfindingEx::grid_view::get_cost(point_i from, point_i to) const -> u64
{
    if ((*Parent)[to] == 256) { return ai::pathfinding::IMPASSABLE_COST; }
    if (from.X != to.X && from.Y != to.Y) {
        if ((*Parent)[{from.X, to.Y}] == 256 || (*Parent)[{to.X, from.Y}] == 256) {
            return ai::pathfinding::IMPASSABLE_COST;
        }
    }
    tile_index_t const c {(*Clearance)[to]};
    if (c == 0) { return ai::pathfinding::IMPASSABLE_COST; }
    u64 const clearanceCost {200 / c};
    u64 const diagPenalty {from.X != to.X && from.Y != to.Y ? 50u : 0};
    return clearanceCost + diagPenalty;
}

void PathfindingEx::on_start()
{
    static constexpr i32 CELL {6};
    static constexpr i32 WALL {1};
    static constexpr i32 STEP {CELL + WALL};
    static constexpr i32 CW {(GRID_SIZE.Width - WALL) / STEP};
    static constexpr i32 CH {(GRID_SIZE.Height - WALL) / STEP};

    static constexpr tile_index_t WALL_TILE {256};
    static constexpr tile_index_t PASSAGE_TILE {1};
    _tiles.fill(WALL_TILE);

    auto carve {[&](i32 cx, i32 cy) {
        i32 const tx {(cx * STEP) + WALL};
        i32 const ty {(cy * STEP) + WALL};
        for (i32 dy {0}; dy < CELL; ++dy) {
            for (i32 dx {0}; dx < CELL; ++dx) {
                _tiles[{tx + dx, ty + dy}] = PASSAGE_TILE;
            }
        }
    }};

    auto carve_between {[&](point_i a, point_i b) {
        i32 const tx {(std::min(a.X, b.X) * STEP) + WALL + (a.X != b.X ? CELL : 0)};
        i32 const ty {(std::min(a.Y, b.Y) * STEP) + WALL + (a.Y != b.Y ? CELL : 0)};
        if (a.X != b.X) {
            for (i32 dy {0}; dy < CELL; ++dy) {
                _tiles[{tx, ty + dy}] = PASSAGE_TILE;
            }
        } else {
            for (i32 dx {0}; dx < CELL; ++dx) {
                _tiles[{tx + dx, ty}] = PASSAGE_TILE;
            }
        }
    }};

    static constexpr std::array<point_i, 4> DIRS {{{0, -1}, {0, 1}, {-1, 0}, {1, 0}}};
    random::shuffle                         shuffle {12345};

    grid<bool>           visited {{CW, CH}, false};
    std::vector<point_i> stack;

    point_i cur {0, 0};
    visited[cur] = true;
    carve(cur.X, cur.Y);
    stack.push_back(cur);

    while (!stack.empty()) {
        cur = stack.back();
        std::array<point_i, 4> dirs {DIRS};
        shuffle(dirs);
        bool pushed {false};
        for (auto const& d : dirs) {
            point_i const next {cur.X + d.X, cur.Y + d.Y};
            if (next.X < 0 || next.X >= CW || next.Y < 0 || next.Y >= CH) { continue; }
            if (visited[next]) { continue; }
            visited[next] = true;
            carve(next.X, next.Y);
            carve_between(cur, next);
            stack.push_back(next);
            pushed = true;
            break;
        }
        if (!pushed) { stack.pop_back(); }
    }

    static constexpr std::array<point_i, 4> FLOOD_DIRS {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};
    std::queue<point_i>                     bfsQueue;
    _clearance.fill(std::numeric_limits<tile_index_t>::max());

    for (i32 y {0}; y < GRID_SIZE.Height; ++y) {
        for (i32 x {0}; x < GRID_SIZE.Width; ++x) {
            if (_tiles[{x, y}] == WALL_TILE) {
                _clearance[{x, y}] = 0;
                bfsQueue.emplace(x, y);
            }
        }
    }

    while (!bfsQueue.empty()) {
        point_i const p {bfsQueue.front()};
        bfsQueue.pop();
        for (auto const& dir : FLOOD_DIRS) {
            point_i const n {p.X + dir.X, p.Y + dir.Y};
            if (n.X < 0 || n.X >= GRID_SIZE.Width || n.Y < 0 || n.Y >= GRID_SIZE.Height) { continue; }
            if (_clearance[n] == std::numeric_limits<tile_index_t>::max()) {
                _clearance[n] = _clearance[p] + 1;
                bfsQueue.push(n);
            }
        }
    }

    f32 const size {(*window().Size).Height / static_cast<f32>(GRID_SIZE.Height)};
    _tileSize    = {size, size};
    _canvasDirty = true;
}

void PathfindingEx::on_draw_to(render_target& target, transform const& xform)
{
    auto const size {*target.Size};
    if (_canvasDirty) {
        _canvas.begin_frame(size, 1);
        _canvas.set_edge_antialias(false);

        // background
        _canvas.begin_path();
        _canvas.set_fill_style(colors::Black);
        _canvas.rect({{0, 0}, size_f {size}});
        _canvas.fill();

        // passages
        _canvas.begin_path();
        _canvas.set_fill_style(colors::White);
        for (i32 y {0}; y < GRID_SIZE.Height; ++y) {
            for (i32 x {0}; x < GRID_SIZE.Width; ++x) {
                if (_tiles[{x, y}] == 1) {
                    _canvas.rect({point_f {static_cast<f32>(x), static_cast<f32>(y)} * point_f {_tileSize.Width, _tileSize.Height}, _tileSize});
                }
            }
        }
        _canvas.fill();
        auto toScreen {[&](point_i p) {
            return point_f {p} * point_f {_tileSize.Width, _tileSize.Height};
        }};

        // path
        if (!_path.empty()) {
            _canvas.begin_path();
            _canvas.set_stroke_style(colors::Blue);
            _canvas.set_stroke_width(2);
            _canvas.move_to(toScreen(_start));
            for (auto const& p : _path) {
                _canvas.line_to(toScreen(p));
            }
            _canvas.line_to(toScreen(_end));
            _canvas.stroke();
        }

        // start/end
        auto drawRect {[&](point_i pos, color col) {
            _canvas.begin_path();
            _canvas.set_fill_style(col);
            _canvas.rect({toScreen(pos), {3, 3}});
            _canvas.fill();
        }};
        if (_start != INVALID) { drawRect(_start, colors::Green); }
        if (_end != INVALID) { drawRect(_end, colors::Red); }

        _canvas.end_frame();
        _canvasDirty = false;
    }

    _renderer.queue_layer(0);
    _renderer.set_bounds({point_f::Zero, size_f {size}});
    _renderer.render_to_target(target, transform::Identity);
}

void PathfindingEx::on_fixed_update(milliseconds deltaTime)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);
}

void PathfindingEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default: break;
    }
}

void PathfindingEx::on_mouse_button_down(mouse::button_event const& ev)
{
    if (ev.Button == mouse::button::Left) {
        point_i const p {static_cast<i32>(ev.Position.X / _tileSize.Width), static_cast<i32>(ev.Position.Y / _tileSize.Height)};
        if (p.X < GRID_SIZE.Width && p.Y < GRID_SIZE.Height && _tiles[p] != 256) {
            _start = p;
        }
    } else if (ev.Button == mouse::button::Right) {
        point_i const p {static_cast<i32>(ev.Position.X / _tileSize.Width), static_cast<i32>(ev.Position.Y / _tileSize.Height)};
        if (p.X < GRID_SIZE.Width && p.Y < GRID_SIZE.Height && _tiles[p] != 256) {
            _end = p;
        }
    }

    if (_start != INVALID && _end != INVALID) {
        _path        = _pathfinder.find_path(_costs, GRID_SIZE, _start, _end);
        _canvasDirty = true;
    } else {
        _path.clear();
        _canvasDirty = true;
    }
}
