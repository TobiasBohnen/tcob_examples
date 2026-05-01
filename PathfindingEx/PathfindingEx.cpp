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
    if (c == 0) { return pathfinding::IMPASSABLE_COST; }
    return 200 / c;
}

void PathfindingEx::generate_maze()
{
    static constexpr i32 CELL {7};
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
    random::shuffle                         shuffle;

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
}

void PathfindingEx::generate_open()
{
    _tiles.fill(1);

    rng                                  rng;
    random::prng_xoroshiro_128_plus_plus posRng;

    for (i32 i {0}; i < 500; ++i) {
        i32 const x {static_cast<i32>(posRng(0, GRID_SIZE.Width - 1))};
        i32 const y {static_cast<i32>(posRng(0, GRID_SIZE.Height - 1))};
        _tiles[{x, y}] = 256;
    }

    for (i32 y {0}; y < 5; ++y) {
        for (i32 x {0}; x < 5; ++x) {
            _tiles[{x, y}] = 1;
        }
    }
    for (i32 y {GRID_SIZE.Height - 5}; y < GRID_SIZE.Height; ++y) {
        for (i32 x {GRID_SIZE.Width - 5}; x < GRID_SIZE.Width; ++x) {
            _tiles[{x, y}] = 1;
        }
    }
}

void PathfindingEx::compute_clearance()
{
    static constexpr std::array<point_i, 4> FLOOD_DIRS {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};
    std::queue<point_i>                     bfsQueue;
    _clearance.fill(std::numeric_limits<tile_index_t>::max());

    for (i32 y {0}; y < GRID_SIZE.Height; ++y) {
        for (i32 x {0}; x < GRID_SIZE.Width; ++x) {
            if (_tiles[{x, y}] == 256) {
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
}

void PathfindingEx::on_start()
{
    generate_maze();
    compute_clearance();

    f32 const size {(*window().Size).Height / static_cast<f32>(GRID_SIZE.Height)};
    _tileSize    = {size, size};
    _canvasDirty = true;
}

void PathfindingEx::run_pathfinding()
{
    if (_start == INVALID || _end == INVALID) {
        _path.clear();
        _canvasDirty = true;
        return;
    }

    stopwatch sw {stopwatch::StartNew()};
    switch (_algoMode) {
    case algo_mode::AStar:
        _path = _astar.find_path(_costs, GRID_SIZE, _start, _end);
        break;
    case algo_mode::BidirAStar:
        _path = _bidir.find_path(_costs, GRID_SIZE, _start, _end);
        break;
    case algo_mode::ThetaStar:
        _path = _thetastar.find_path(_costs, GRID_SIZE, _start, _end);
        break;
    case algo_mode::LPA:
        if (!_lpaInitialized) {
            _lpa.initialize(_costs, GRID_SIZE, _start, _end);
            _lpaInitialized = true;
        }
        _path = _lpa.path();
        break;
    case algo_mode::DStarLite:
        if (!_dstarInitialized) {
            _dstar.initialize(_costs, GRID_SIZE, _start, _end);
            _dstarInitialized = true;
        }
        _path = _dstar.path();
        break;
    case algo_mode::MinTurn:
        _path = _minturn.find_path(_costs, GRID_SIZE, _start, _end);
        break;
    }
    _lastMs      = sw.elapsed_milliseconds();
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

        // passages with clearance color
        for (i32 y {0}; y < GRID_SIZE.Height; ++y) {
            for (i32 x {0}; x < GRID_SIZE.Width; ++x) {
                if (_tiles[{x, y}] != 1) { continue; }
                tile_index_t const c {_clearance[{x, y}]};
                color              col;
                switch (c) {
                case 1:  col = colors::Red; break;
                case 2:  col = colors::Orange; break;
                default: col = colors::Yellow; break;
                }
                _canvas.begin_path();
                _canvas.set_fill_style(col);
                _canvas.rect({point_f {static_cast<f32>(x), static_cast<f32>(y)} * point_f {_tileSize.Width, _tileSize.Height}, _tileSize});
                _canvas.fill();
            }
        }

        auto toScreen {[&](point_i p) {
            return point_f {p} * point_f {_tileSize.Width, _tileSize.Height};
        }};

        // agent position for D* Lite
        if (_algoMode == algo_mode::DStarLite && _dstarInitialized) {
            point_i const pos {_dstar.position()};
            if (pos != INVALID) {
                _canvas.begin_path();
                _canvas.set_fill_style(colors::Cyan);
                _canvas.rect({toScreen(pos), _tileSize});
                _canvas.fill();
            }
        }

        // path
        if (!_path.empty()) {
            _canvas.begin_path();
            _canvas.set_stroke_style(colors::Blue);
            _canvas.set_stroke_width(2);
            _canvas.move_to(toScreen(_algoMode == algo_mode::DStarLite && _dstarInitialized
                                         ? _dstar.position()
                                         : _start));
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
            _canvas.rect({toScreen(pos) - point_f {_tileSize.Width / 2, _tileSize.Height / 2}, _tileSize});
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

    string_view mapName;
    switch (_mapMode) {
    case map_mode::Maze: mapName = "Maze"; break;
    case map_mode::Open: mapName = "Open"; break;
    }

    string_view algoName;
    switch (_algoMode) {
    case algo_mode::AStar:      algoName = "A*"; break;
    case algo_mode::BidirAStar: algoName = "Bidir A*"; break;
    case algo_mode::ThetaStar:  algoName = "Theta*"; break;
    case algo_mode::LPA:        algoName = "LPA*"; break;
    case algo_mode::DStarLite:  algoName = "D* Lite"; break;
    case algo_mode::MinTurn:    algoName = "MinTurn"; break;
    }

    window().Title = std::format("TestGame | FPS avg:{:.2f} | map:{} | algo:{} | path:{:.3f}ms | nodes:{} | x:{} y:{} ",
                                 stats.average_FPS(), mapName, algoName,
                                 _lastMs, _path.size(), mouse.X, mouse.Y);
}

void PathfindingEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    case scan_code::M:
        _mapMode = (_mapMode == map_mode::Maze) ? map_mode::Open : map_mode::Maze;
        [[fallthrough]];
    case scan_code::R:
        _start = INVALID;
        _end   = INVALID;
        _path.clear();
        _lpaInitialized   = false;
        _dstarInitialized = false;
        if (_mapMode == map_mode::Open) {
            generate_open();
        } else {
            generate_maze();
        }
        compute_clearance();
        _canvasDirty = true;
        break;
    case scan_code::A:
        _lpaInitialized   = false;
        _dstarInitialized = false;
        switch (_algoMode) {
        case algo_mode::AStar:      _algoMode = algo_mode::BidirAStar; break;
        case algo_mode::BidirAStar: _algoMode = algo_mode::ThetaStar; break;
        case algo_mode::ThetaStar:  _algoMode = algo_mode::LPA; break;
        case algo_mode::LPA:        _algoMode = algo_mode::DStarLite; break;
        case algo_mode::DStarLite:  _algoMode = algo_mode::MinTurn; break;
        case algo_mode::MinTurn:    _algoMode = algo_mode::AStar; break;
        }
        run_pathfinding();
        break;
    case scan_code::U:
        // toggle a tile near center as dynamic obstacle, update stateful algos
        {
            static constexpr point_i CENTER {GRID_SIZE.Width / 2, GRID_SIZE.Height / 2};
            _tiles[CENTER] = (_tiles[CENTER] == 256) ? 1 : 256;
            compute_clearance();
            stopwatch sw {stopwatch::StartNew()};
            if (_algoMode == algo_mode::LPA && _lpaInitialized) {
                _lpa.update(_costs, CENTER);
                _path = _lpa.path();
            } else if (_algoMode == algo_mode::DStarLite && _dstarInitialized) {
                _dstar.update(_costs, CENTER);
                _path = _dstar.path();
            }
            _lastMs      = sw.elapsed_milliseconds();
            _canvasDirty = true;
        }
        break;
    case scan_code::SPACE:
        // advance D* Lite agent one step
        if (_algoMode == algo_mode::DStarLite && _dstarInitialized) {
            stopwatch sw {stopwatch::StartNew()};
            _dstar.move(_costs);
            _path        = _dstar.path();
            _lastMs      = sw.elapsed_milliseconds();
            _canvasDirty = true;
        }
        break;
    default: break;
    }
}

void PathfindingEx::on_mouse_button_down(mouse::button_event const& ev)
{
    if (ev.Button == mouse::button::Left) {
        point_i const p {static_cast<i32>(ev.Position.X / _tileSize.Width), static_cast<i32>(ev.Position.Y / _tileSize.Height)};
        if (p.X < GRID_SIZE.Width && p.Y < GRID_SIZE.Height && _tiles[p] != 256) {
            _start            = p;
            _lpaInitialized   = false;
            _dstarInitialized = false;
        }
    } else if (ev.Button == mouse::button::Right) {
        point_i const p {static_cast<i32>(ev.Position.X / _tileSize.Width), static_cast<i32>(ev.Position.Y / _tileSize.Height)};
        if (p.X < GRID_SIZE.Width && p.Y < GRID_SIZE.Height && _tiles[p] != 256) {
            _end              = p;
            _lpaInitialized   = false;
            _dstarInitialized = false;
        }
    }

    run_pathfinding();
}
