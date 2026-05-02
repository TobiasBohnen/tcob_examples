// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include "PathfindingEx.hpp"

#include <algorithm>

static constexpr i32 FOREST_COST {2};
static constexpr i32 WATER_COST {8};

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

void PathfindingEx::on_start()
{
    auto const windowSize {*window().Size};
    _uiWidth = static_cast<f32>(windowSize.Width) / 5.0f;

    rect_i const formBounds {windowSize.Width - static_cast<i32>(_uiWidth), 0,
                             static_cast<i32>(_uiWidth), windowSize.Height};
    _form = std::make_shared<pathfinding_form>(formBounds);

    _form->OnRegenerate.connect([this] { generate_map(); run_pathfinding(); });
    _form->OnSmooth.connect([this] { do_smooth(); });
    _form->OnToggleWall.connect([this] { do_toggle_wall(); });
    _form->OnDStarMove.connect([this] { do_dstar_move(); });
    _form->OnMapChanged.connect([this] {
        generate_map();
        run_pathfinding();
    });
    _form->OnAlgoChanged.connect([this] {
        reset_stateful();
        run_pathfinding();
    });

    root_node().Entity = _form;

    f32 const tileSize {std::floor(static_cast<f32>(windowSize.Height) / static_cast<f32>(GRID_SIZE.Height))};
    _tileSize = {tileSize, tileSize};

    generate_map();
}

void PathfindingEx::generate_map()
{
    _start = INVALID;
    _end   = INVALID;
    _path.clear();
    reset_stateful();

    switch (_form->current_map_mode()) {
    case pathfinding_form::map_mode::Maze:    generate_maze(); break;
    case pathfinding_form::map_mode::Open:    generate_open(); break;
    case pathfinding_form::map_mode::Boxes:   generate_boxes(); break;
    case pathfinding_form::map_mode::Terrain: generate_terrain(); break;
    }
    _canvasDirty = true;
}

void PathfindingEx::reset_stateful()
{
    _lpaInitialized       = false;
    _dstarInitialized     = false;
    _flowfieldInitialized = false;
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
            for (i32 dy {0}; dy < CELL; ++dy) { _walls[{tx, ty + dy}] = false; }
        } else {
            for (i32 dx {0}; dx < CELL; ++dx) { _walls[{tx + dx, ty}] = false; }
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
        _walls[{static_cast<i32>(rng(0, GRID_SIZE.Width - 1)),
                static_cast<i32>(rng(0, GRID_SIZE.Height - 1))}] = true;
    }

    for (i32 y {0}; y < 5; ++y) {
        for (i32 x {0}; x < 5; ++x) { _walls[{x, y}] = false; }
    }
    for (i32 y {GRID_SIZE.Height - 5}; y < GRID_SIZE.Height; ++y) {
        for (i32 x {GRID_SIZE.Width - 5}; x < GRID_SIZE.Width; ++x) { _walls[{x, y}] = false; }
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
                if (x == xStart || x == xStart + w - 1 || y == yStart || y == yStart + h - 1) {
                    _walls[{x, y}] = true;
                }
            }
        }
    }

    auto clearArea {[&](i32 sx, i32 sy) {
        for (i32 y {sy}; y < sy + 5; ++y) {
            for (i32 x {sx}; x < sx + 5; ++x) { _walls[{x, y}] = false; }
        }
    }};
    clearArea(0, 0);
    clearArea(GRID_SIZE.Width - 5, GRID_SIZE.Height - 5);

    _gridView = {&_walls, nullptr};
}

void PathfindingEx::generate_terrain()
{
    _walls.fill(false);
    _costs.fill(1);

    for (i32 x {0}; x < GRID_SIZE.Width; ++x) {
        _walls[{x, 0}]                    = true;
        _walls[{x, GRID_SIZE.Height - 1}] = true;
    }
    for (i32 y {0}; y < GRID_SIZE.Height; ++y) {
        _walls[{0, y}]                   = true;
        _walls[{GRID_SIZE.Width - 1, y}] = true;
    }

    rng rng;

    auto fillCircle {[&](i32 cx, i32 cy, i32 r, auto fn) {
        for (i32 y {cy - r}; y <= cy + r; ++y) {
            for (i32 x {cx - r}; x <= cx + r; ++x) {
                if (x < 1 || x >= GRID_SIZE.Width - 1 || y < 1 || y >= GRID_SIZE.Height - 1) { continue; }
                if (((x - cx) * (x - cx)) + ((y - cy) * (y - cy)) <= (r * r)) { fn(x, y); }
            }
        }
    }};

    for (i32 i {0}; i < 6; ++i) {
        fillCircle(static_cast<i32>(rng(20, GRID_SIZE.Width - 20)),
                   static_cast<i32>(rng(20, GRID_SIZE.Height - 20)),
                   static_cast<i32>(rng(15, 35)),
                   [&](i32 x, i32 y) { _costs[{x, y}] = WATER_COST; });
    }
    for (i32 i {0}; i < 8; ++i) {
        fillCircle(static_cast<i32>(rng(15, GRID_SIZE.Width - 15)),
                   static_cast<i32>(rng(15, GRID_SIZE.Height - 15)),
                   static_cast<i32>(rng(10, 25)),
                   [&](i32 x, i32 y) { if (_costs[{x, y}] == 1) { _costs[{x, y}] = FOREST_COST; } });
    }
    for (i32 i {0}; i < 4; ++i) {
        fillCircle(static_cast<i32>(rng(20, GRID_SIZE.Width - 20)),
                   static_cast<i32>(rng(20, GRID_SIZE.Height - 20)),
                   static_cast<i32>(rng(8, 15)),
                   [&](i32 x, i32 y) { _walls[{x, y}] = true; });
    }

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

void PathfindingEx::run_pathfinding()
{
    if (_start == INVALID || _end == INVALID) {
        _path.clear();
        _canvasDirty = true;
        return;
    }

    stopwatch sw {stopwatch::StartNew()};
    switch (_form->current_algo_mode()) {
    case pathfinding_form::algo_mode::AStar:
        _path = _astar.find_path(_gridView, GRID_SIZE, _start, _end);
        break;
    case pathfinding_form::algo_mode::BidirAStar:
        _path = _bidir.find_path(_gridView, GRID_SIZE, _start, _end);
        break;
    case pathfinding_form::algo_mode::ThetaStar:
        _path = _thetastar.find_path(_gridView, GRID_SIZE, _start, _end);
        break;
    case pathfinding_form::algo_mode::LPA:
        if (!_lpaInitialized) {
            _lpa.initialize(_gridView, GRID_SIZE, _start, _end);
            _lpaInitialized = true;
        }
        _path = _lpa.path();
        break;
    case pathfinding_form::algo_mode::DStarLite:
        if (!_dstarInitialized) {
            _dstar.initialize(_gridView, GRID_SIZE, _start, _end);
            _dstarInitialized = true;
        }
        _path = _dstar.path();
        break;
    case pathfinding_form::algo_mode::MinTurns:
        _path = _minturns.find_path(_gridView, GRID_SIZE, _start, _end);
        break;
    case pathfinding_form::algo_mode::FlowField:
        if (!_flowfieldInitialized) {
            _flowfield.build(_gridView, GRID_SIZE, _end);
            _flowfieldInitialized = true;
        }
        _path = _flowfield.path(_start);
        break;
    }

    _lastMs = sw.elapsed_milliseconds();
    _form->set_time(_lastMs);
    _form->set_nodes(_path.size());
    _canvasDirty = true;
}

void PathfindingEx::do_smooth()
{
    _path = pathfinding::smooth_path(_gridView, GRID_SIZE, _path);
    _form->set_nodes(_path.size());
    _canvasDirty = true;
}

void PathfindingEx::do_toggle_wall()
{
    static constexpr point_i CENTER {GRID_SIZE.Width / 2, GRID_SIZE.Height / 2};
    _walls[CENTER]        = !_walls[CENTER];
    _flowfieldInitialized = false;

    stopwatch  sw {stopwatch::StartNew()};
    auto const algo {_form->current_algo_mode()};
    if (algo == pathfinding_form::algo_mode::LPA && _lpaInitialized) {
        _lpa.update(_gridView, CENTER);
        _path = _lpa.path();
    } else if (algo == pathfinding_form::algo_mode::DStarLite && _dstarInitialized) {
        _dstar.update(_gridView, CENTER);
        _path = _dstar.path();
    } else if (algo == pathfinding_form::algo_mode::FlowField && _end != INVALID) {
        _flowfield.build(_gridView, GRID_SIZE, _end);
        _flowfieldInitialized = true;
        _path                 = _flowfield.path(_start);
    }
    _lastMs = sw.elapsed_milliseconds();
    _form->set_time(_lastMs);
    _form->set_nodes(_path.size());
    _canvasDirty = true;
}

void PathfindingEx::do_dstar_move()
{
    if (_form->current_algo_mode() != pathfinding_form::algo_mode::DStarLite || !_dstarInitialized) { return; }
    stopwatch sw {stopwatch::StartNew()};
    _dstar.move(_gridView);
    _path   = _dstar.path();
    _lastMs = sw.elapsed_milliseconds();
    _form->set_time(_lastMs);
    _form->set_nodes(_path.size());
    _canvasDirty = true;
}

void PathfindingEx::on_fixed_update(milliseconds deltaTime)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} | map:{} | algo:{} ",
                                 stats.average_FPS(),
                                 _form->map_mode_name(),
                                 _form->algo_mode_name());
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

void PathfindingEx::on_draw_to(render_target& target, transform const& xform)
{
    size_i const size {static_cast<i32>(_tileSize.Width * GRID_SIZE.Width),
                       static_cast<i32>(_tileSize.Height * GRID_SIZE.Height)};
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
                if (!_walls[{x, y}]) { _canvas.rect(tileRect({x, y})); }
            }
        }
        _canvas.fill();

        // terrain
        if (_form->is_terrain_map()) {
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

        // flow field
        auto const algo {_form->current_algo_mode()};
        if (algo == pathfinding_form::algo_mode::FlowField && _flowfieldInitialized) {
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

        // D* Lite agent
        if (algo == pathfinding_form::algo_mode::DStarLite && _dstarInitialized) {
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
            point_i const pathStart {algo == pathfinding_form::algo_mode::DStarLite && _dstarInitialized
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

void PathfindingEx::on_mouse_button_down(mouse::button_event const& ev)
{
    // ignore clicks in the UI panel
    if (ev.Position.X >= static_cast<f32>((*window().Size).Width) - _uiWidth) { return; }

    point_i const p {static_cast<i32>(ev.Position.X / _tileSize.Width),
                     static_cast<i32>(ev.Position.Y / _tileSize.Height)};
    if (p.X >= GRID_SIZE.Width || p.Y >= GRID_SIZE.Height || _walls[p]) { return; }

    if (ev.Button == mouse::button::Left) {
        _start            = p;
        _lpaInitialized   = false;
        _dstarInitialized = false;
    } else if (ev.Button == mouse::button::Right) {
        _end = p;
        reset_stateful();
    }

    run_pathfinding();
}
