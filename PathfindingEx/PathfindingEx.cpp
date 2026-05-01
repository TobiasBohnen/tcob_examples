// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include "PathfindingEx.hpp"

#include <algorithm>

PathfindingEx::PathfindingEx(game& game)
    : scene {game}
{
    window().ClearColor = colors::Black;
}

PathfindingEx::~PathfindingEx() = default;

auto PathfindingEx::grid_view::get_cost(point_i from, point_i to) const -> u64
{
    if ((*Wall)[to]) { return ai::pathfinding::IMPASSABLE_COST; }
    if (from.X != to.X && from.Y != to.Y) {
        if ((*Wall)[{from.X, to.Y}] || (*Wall)[{to.X, from.Y}]) {
            return ai::pathfinding::IMPASSABLE_COST;
        }
    }
    if (Terrain != nullptr) { return (*Terrain)[to]; }
    return 1;
}

void PathfindingEx::generate_maze()
{
    static constexpr i32 CELL {7};
    static constexpr i32 WALL {1};
    static constexpr i32 STEP {CELL + WALL};
    static constexpr i32 CW {(GRID_SIZE.Width - WALL) / STEP};
    static constexpr i32 CH {(GRID_SIZE.Height - WALL) / STEP};

    _walls.fill(true);

    auto carve {[&](i32 cx, i32 cy) {
        i32 const tx {(cx * STEP) + WALL};
        i32 const ty {(cy * STEP) + WALL};
        for (i32 dy {0}; dy < CELL; ++dy) {
            for (i32 dx {0}; dx < CELL; ++dx) {
                _walls[{tx + dx, ty + dy}] = false;
            }
        }
    }};

    auto carve_between {[&](point_i a, point_i b) {
        i32 const tx {(std::min(a.X, b.X) * STEP) + WALL + (a.X != b.X ? CELL : 0)};
        i32 const ty {(std::min(a.Y, b.Y) * STEP) + WALL + (a.Y != b.Y ? CELL : 0)};
        if (a.X != b.X) {
            for (i32 dy {0}; dy < CELL; ++dy) {
                _walls[{tx, ty + dy}] = false;
            }
        } else {
            for (i32 dx {0}; dx < CELL; ++dx) {
                _walls[{tx + dx, ty}] = false;
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

    _gridView = {&_walls, nullptr};
}

void PathfindingEx::generate_open()
{
    _walls.fill(false);

    rng rng;

    for (i32 i {0}; i < 500; ++i) {
        i32 const x {static_cast<i32>(rng(0, GRID_SIZE.Width - 1))};
        i32 const y {static_cast<i32>(rng(0, GRID_SIZE.Height - 1))};
        _walls[{x, y}] = true;
    }

    for (i32 y {0}; y < 5; ++y) {
        for (i32 x {0}; x < 5; ++x) {
            _walls[{x, y}] = false;
        }
    }
    for (i32 y {GRID_SIZE.Height - 5}; y < GRID_SIZE.Height; ++y) {
        for (i32 x {GRID_SIZE.Width - 5}; x < GRID_SIZE.Width; ++x) {
            _walls[{x, y}] = false;
        }
    }

    _gridView = {&_walls, nullptr};
}

void PathfindingEx::generate_boxes()
{
    _walls.fill(false);
    rng rng;

    for (i32 i {0}; i < 8; ++i) {
        i32 const w {static_cast<i32>(rng(10, 20))};
        i32 const h {static_cast<i32>(rng(10, 20))};
        i32 const xStart {static_cast<i32>(rng(2, GRID_SIZE.Width - w - 2))};
        i32 const yStart {static_cast<i32>(rng(2, GRID_SIZE.Height - h - 2))};

        for (i32 y {yStart}; y < yStart + h; ++y) {
            for (i32 x {xStart}; x < xStart + w; ++x) {
                bool const isEdge {x == xStart || x == xStart + w - 1 || y == yStart || y == yStart + h - 1};
                if (isEdge) { _walls[{x, y}] = true; }
            }
        }
    }

    auto const clearArea {[&](i32 sx, i32 sy) {
        for (i32 y {sy}; y < sy + 5; ++y) {
            for (i32 x {sx}; x < sx + 5; ++x) {
                _walls[{x, y}] = false;
            }
        }
    }};

    clearArea(0, 0);
    clearArea(GRID_SIZE.Width - 5, GRID_SIZE.Height - 5);

    _gridView = {&_walls, nullptr};
}

constexpr i32 FOREST_COST {2};
constexpr i32 WATER_COST {8};

void PathfindingEx::generate_terrain()
{
    _walls.fill(false);
    _costs.fill(1);

    // border walls
    for (i32 x {0}; x < GRID_SIZE.Width; ++x) {
        _walls[{x, 0}]                    = true;
        _walls[{x, GRID_SIZE.Height - 1}] = true;
    }
    for (i32 y {0}; y < GRID_SIZE.Height; ++y) {
        _walls[{0, y}]                   = true;
        _walls[{GRID_SIZE.Width - 1, y}] = true;
    }

    rng rng;

    // water regions
    for (i32 i {0}; i < 6; ++i) {
        i32 const cx {static_cast<i32>(rng(20, GRID_SIZE.Width - 20))};
        i32 const cy {static_cast<i32>(rng(20, GRID_SIZE.Height - 20))};
        i32 const r {static_cast<i32>(rng(15, 35))};
        for (i32 y {cy - r}; y <= cy + r; ++y) {
            for (i32 x {cx - r}; x <= cx + r; ++x) {
                if (x < 1 || x >= GRID_SIZE.Width - 1 || y < 1 || y >= GRID_SIZE.Height - 1) { continue; }
                if (((x - cx) * (x - cx)) + ((y - cy) * (y - cy)) <= (r * r)) {
                    _costs[{x, y}] = WATER_COST;
                }
            }
        }
    }

    // forest regions
    for (i32 i {0}; i < 8; ++i) {
        i32 const cx {static_cast<i32>(rng(15, GRID_SIZE.Width - 15))};
        i32 const cy {static_cast<i32>(rng(15, GRID_SIZE.Height - 15))};
        i32 const r {static_cast<i32>(rng(10, 25))};
        for (i32 y {cy - r}; y <= cy + r; ++y) {
            for (i32 x {cx - r}; x <= cx + r; ++x) {
                if (x < 1 || x >= GRID_SIZE.Width - 1 || y < 1 || y >= GRID_SIZE.Height - 1) { continue; }
                if (((x - cx) * (x - cx)) + ((y - cy) * (y - cy)) <= (r * r)) {
                    if (_costs[{x, y}] == 1) {
                        _costs[{x, y}] = FOREST_COST;
                    }
                }
            }
        }
    }

    // mountain walls
    for (i32 i {0}; i < 4; ++i) {
        i32 const cx {static_cast<i32>(rng(20, GRID_SIZE.Width - 20))};
        i32 const cy {static_cast<i32>(rng(20, GRID_SIZE.Height - 20))};
        i32 const r {static_cast<i32>(rng(8, 15))};
        for (i32 y {cy - r}; y <= cy + r; ++y) {
            for (i32 x {cx - r}; x <= cx + r; ++x) {
                if (x < 1 || x >= GRID_SIZE.Width - 1 || y < 1 || y >= GRID_SIZE.Height - 1) { continue; }
                if (((x - cx) * (x - cx)) + ((y - cy) * (y - cy)) <= (r * r)) {
                    _walls[{x, y}] = true;
                }
            }
        }
    }

    // ensure corners clear
    for (i32 y {1}; y < 6; ++y) {
        for (i32 x {1}; x < 6; ++x) {
            _walls[{x, y}] = false;
            _costs[{x, y}] = 1;
        }
    }
    for (i32 y {GRID_SIZE.Height - 6}; y < GRID_SIZE.Height - 1; ++y) {
        for (i32 x {GRID_SIZE.Width - 6}; x < GRID_SIZE.Width - 1; ++x) {
            _walls[{x, y}] = false;
            _costs[{x, y}] = 1;
        }
    }

    _gridView = {&_walls, &_costs};
}

void PathfindingEx::on_start()
{
    generate_maze();

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
        _path = _astar.find_path(_gridView, GRID_SIZE, _start, _end);
        break;
    case algo_mode::BidirAStar:
        _path = _bidir.find_path(_gridView, GRID_SIZE, _start, _end);
        break;
    case algo_mode::ThetaStar:
        _path = _thetastar.find_path(_gridView, GRID_SIZE, _start, _end);
        break;
    case algo_mode::LPA:
        if (!_lpaInitialized) {
            _lpa.initialize(_gridView, GRID_SIZE, _start, _end);
            _lpaInitialized = true;
        }
        _path = _lpa.path();
        break;
    case algo_mode::DStarLite:
        if (!_dstarInitialized) {
            _dstar.initialize(_gridView, GRID_SIZE, _start, _end);
            _dstarInitialized = true;
        }
        _path = _dstar.path();
        break;
    case algo_mode::MinTurns:
        _path = _minturns.find_path(_gridView, GRID_SIZE, _start, _end);
        break;
    case algo_mode::FlowField:
        if (!_flowfieldInitialized) {
            _flowfield.build(_gridView, GRID_SIZE, _end);
            _flowfieldInitialized = true;
        }
        _path = _flowfield.path(_start);
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

        auto toScreen {[&](point_i p) -> point_f {
            return {std::floor(static_cast<f32>(p.X) * _tileSize.Width),
                    std::floor(static_cast<f32>(p.Y) * _tileSize.Height)};
        }};

        auto tileRect {[&](point_i p) -> rect_f {
            point_f const pos {toScreen(p)};
            point_f const next {toScreen({p.X + 1, p.Y + 1})};
            return {pos, size_f {next.X - pos.X, next.Y - pos.Y}};
        }};

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
                if (!_walls[{x, y}]) {
                    _canvas.rect(tileRect({x, y}));
                }
            }
        }
        _canvas.fill();

        // terrain costs
        if (_mapMode == map_mode::Terrain) {
            for (i32 y {0}; y < GRID_SIZE.Height; ++y) {
                for (i32 x {0}; x < GRID_SIZE.Width; ++x) {
                    if (_walls[{x, y}]) { continue; }
                    u64 const cost {_costs[{x, y}]};
                    if (cost == 1) { continue; }
                    color col;
                    switch (cost) {
                    case FOREST_COST: col = colors::DarkGreen; break;
                    case WATER_COST:  col = colors::DarkBlue; break;
                    default:          col = colors::Gray; break;
                    }
                    _canvas.begin_path();
                    _canvas.set_fill_style(col);
                    _canvas.rect(tileRect({x, y}));
                    _canvas.fill();
                }
            }
        }

        // flow field visualization
        if (_algoMode == algo_mode::FlowField && _flowfieldInitialized) {
            f32 const scale {_tileSize.Width * 2.5f};
            for (i32 y {0}; y < GRID_SIZE.Height; y += 4) {
                for (i32 x {0}; x < GRID_SIZE.Width; x += 4) {
                    if (_walls[{x, y}]) { continue; }
                    point_i const dir {_flowfield.direction({x, y})};
                    if (dir == pathfinding::INVALID_DIR) { continue; }

                    rect_f const  tr {tileRect({x, y})};
                    point_f const center {tr.Position + point_f {tr.Size.Width / 2, tr.Size.Height / 2}};
                    point_f const fd {static_cast<f32>(dir.X), static_cast<f32>(dir.Y)};
                    f32 const     len {std::sqrt((fd.X * fd.X) + (fd.Y * fd.Y))};
                    point_f const fn {fd.X / len, fd.Y / len};
                    point_f const perp {fn.as_perpendicular()};

                    point_f const tip {center + point_f {fn.X * scale, fn.Y * scale}};
                    point_f const left {center + point_f {perp.X * scale * 0.4f, perp.Y * scale * 0.4f}};
                    point_f const right {center - point_f {perp.X * scale * 0.4f, perp.Y * scale * 0.4f}};

                    _canvas.begin_path();
                    _canvas.set_fill_style(colors::Black);
                    _canvas.triangle(left, right, tip);
                    _canvas.fill();
                }
            }
        }

        // agent position for D* Lite
        if (_algoMode == algo_mode::DStarLite && _dstarInitialized) {
            point_i const pos {_dstar.position()};
            if (pos != INVALID) {
                _canvas.begin_path();
                _canvas.set_fill_style(colors::Cyan);
                _canvas.rect(tileRect(pos));
                _canvas.fill();
            }
        }

        // path
        if (!_path.empty()) {
            point_i const pathStart {_algoMode == algo_mode::DStarLite && _dstarInitialized
                                         ? _dstar.position()
                                         : _start};
            _canvas.begin_path();
            _canvas.set_stroke_style(colors::Blue);
            _canvas.set_stroke_width(2);
            _canvas.move_to(toScreen(pathStart) + point_f {_tileSize.Width / 2, _tileSize.Height / 2});
            for (auto const& p : _path) {
                _canvas.line_to(toScreen(p) + point_f {_tileSize.Width / 2, _tileSize.Height / 2});
            }
            _canvas.line_to(toScreen(_end) + point_f {_tileSize.Width / 2, _tileSize.Height / 2});
            _canvas.stroke();
        }

        // start/end
        auto drawRect {[&](point_i pos, color col) {
            _canvas.begin_path();
            _canvas.set_fill_style(col);
            _canvas.rect(tileRect(pos));
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
    case map_mode::Maze:    mapName = "Maze"; break;
    case map_mode::Open:    mapName = "Open"; break;
    case map_mode::Boxes:   mapName = "Boxes"; break;
    case map_mode::Terrain: mapName = "Terrain"; break;
    }

    string_view algoName;
    switch (_algoMode) {
    case algo_mode::AStar:      algoName = "A*"; break;
    case algo_mode::BidirAStar: algoName = "Bidir A*"; break;
    case algo_mode::ThetaStar:  algoName = "Theta*"; break;
    case algo_mode::LPA:        algoName = "LPA*"; break;
    case algo_mode::DStarLite:  algoName = "D* Lite"; break;
    case algo_mode::MinTurns:   algoName = "MinTurns"; break;
    case algo_mode::FlowField:  algoName = "FlowField"; break;
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
        switch (_mapMode) {
        case map_mode::Maze:    _mapMode = map_mode::Open; break;
        case map_mode::Open:    _mapMode = map_mode::Boxes; break;
        case map_mode::Boxes:   _mapMode = map_mode::Terrain; break;
        case map_mode::Terrain: _mapMode = map_mode::Maze; break;
        }
        [[fallthrough]];
    case scan_code::R:
        _start = INVALID;
        _end   = INVALID;
        _path.clear();
        _lpaInitialized       = false;
        _dstarInitialized     = false;
        _flowfieldInitialized = false;
        switch (_mapMode) {
        case map_mode::Maze:    generate_maze(); break;
        case map_mode::Open:    generate_open(); break;
        case map_mode::Boxes:   generate_boxes(); break;
        case map_mode::Terrain: generate_terrain(); break;
        }
        _canvasDirty = true;
        break;
    case scan_code::A:
        _lpaInitialized       = false;
        _dstarInitialized     = false;
        _flowfieldInitialized = false;
        switch (_algoMode) {
        case algo_mode::AStar:      _algoMode = algo_mode::BidirAStar; break;
        case algo_mode::BidirAStar: _algoMode = algo_mode::ThetaStar; break;
        case algo_mode::ThetaStar:  _algoMode = algo_mode::LPA; break;
        case algo_mode::LPA:        _algoMode = algo_mode::DStarLite; break;
        case algo_mode::DStarLite:  _algoMode = algo_mode::MinTurns; break;
        case algo_mode::MinTurns:   _algoMode = algo_mode::FlowField; break;
        case algo_mode::FlowField:  _algoMode = algo_mode::AStar; break;
        }
        run_pathfinding();
        break;
    case scan_code::U: {
        static constexpr point_i CENTER {GRID_SIZE.Width / 2, GRID_SIZE.Height / 2};
        _walls[CENTER]        = !_walls[CENTER];
        _flowfieldInitialized = false;
        stopwatch sw {stopwatch::StartNew()};
        if (_algoMode == algo_mode::LPA && _lpaInitialized) {
            _lpa.update(_gridView, CENTER);
            _path = _lpa.path();
        } else if (_algoMode == algo_mode::DStarLite && _dstarInitialized) {
            _dstar.update(_gridView, CENTER);
            _path = _dstar.path();
        } else if (_algoMode == algo_mode::FlowField && _end != INVALID) {
            _flowfield.build(_gridView, GRID_SIZE, _end);
            _flowfieldInitialized = true;
            _path                 = _flowfield.path(_start);
        }
        _lastMs      = sw.elapsed_milliseconds();
        _canvasDirty = true;
    } break;
    case scan_code::SPACE:
        if (_algoMode == algo_mode::DStarLite && _dstarInitialized) {
            stopwatch sw {stopwatch::StartNew()};
            _dstar.move(_gridView);
            _path        = _dstar.path();
            _lastMs      = sw.elapsed_milliseconds();
            _canvasDirty = true;
        }
        break;
    case scan_code::S:
        _path        = pathfinding::smooth_path(_gridView, GRID_SIZE, _path);
        _canvasDirty = true;
        break;
    default: break;
    }
}

void PathfindingEx::on_mouse_button_down(mouse::button_event const& ev)
{
    if (ev.Button == mouse::button::Left) {
        point_i const p {static_cast<i32>(ev.Position.X / _tileSize.Width), static_cast<i32>(ev.Position.Y / _tileSize.Height)};
        if (p.X < GRID_SIZE.Width && p.Y < GRID_SIZE.Height && !_walls[p]) {
            _start            = p;
            _lpaInitialized   = false;
            _dstarInitialized = false;
        }
    } else if (ev.Button == mouse::button::Right) {
        point_i const p {static_cast<i32>(ev.Position.X / _tileSize.Width), static_cast<i32>(ev.Position.Y / _tileSize.Height)};
        if (p.X < GRID_SIZE.Width && p.Y < GRID_SIZE.Height && !_walls[p]) {
            _end                  = p;
            _lpaInitialized       = false;
            _dstarInitialized     = false;
            _flowfieldInitialized = false;
        }
    }

    run_pathfinding();
}
