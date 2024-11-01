// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PathfindingEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

PathfindingEx::PathfindingEx(game& game)
    : scene(game)
    , _tileMapOrtho {{
          {1, {.Color = colors::White}},
          {2, {.Color = colors::LightGray}},
          {ai::astar_pathfinding::IMPASSABLE_COST, {.Color = colors::Black}},
      }}
{
    get_window().ClearColor = colors::DarkRed;
}

PathfindingEx::~PathfindingEx() = default;

void PathfindingEx::on_start()
{
    rng _rand;
    for (i32 i {0}; i < 50; ++i) {
        i32 const x0 {_rand(0, GRID_SIZE.Width - 1)};
        i32 const y0 {_rand(0, GRID_SIZE.Height - 1)};
        _tiles[{x0, y0}] = 2;

        i32 const x1 {_rand(0, GRID_SIZE.Width - 1)};
        i32 const y1 {_rand(0, GRID_SIZE.Height - 1)};
        _tiles[{x1, y1}] = ai::astar_pathfinding::IMPASSABLE_COST;
    }

    f32 const size {get_window().Size().Height / static_cast<f32>(GRID_SIZE.Height)};
    _tileSize          = {size, size};
    _tileMapOrtho.Grid = {.TileSize = _tileSize};

    tcob::gfx::tilemap_layer layer {_tiles};
    _tileMapOrtho.add_layer(layer);
    _tileMapOrtho.Material = material::Empty();
}

void PathfindingEx::on_draw_to(render_target& target)
{
    _tileMapOrtho.draw_to(target);

    auto const size {target.Size()};

    _canvas.begin_frame(size, 1);
    // path
    _canvas.begin_path();
    _canvas.set_fill_style(colors::Gray);
    for (auto const& point : _path) {
        _canvas.rect({point_f {point * point_f {_tileSize.Width, _tileSize.Height}}, _tileSize});
    }
    _canvas.fill();

    // start/end
    auto drawRect {[&](point_i pos, color col) {
        _canvas.begin_path();
        _canvas.set_fill_style(col);
        _canvas.rect({point_f {pos * point_f {_tileSize.Width, _tileSize.Height}}, _tileSize});
        _canvas.fill();
    }};
    if (_start != INVALID) { drawRect(_start, colors::Green); }
    if (_end != INVALID) { drawRect(_end, colors::Red); }

    _canvas.end_frame();

    _renderer.set_layer(0);
    _renderer.set_bounds({point_f::Zero, size_f {get_window().Size()}});
    _renderer.render_to_target(target);
}

void PathfindingEx::on_update(milliseconds deltaTime)
{
    _tileMapOrtho.update(deltaTime);
}

void PathfindingEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    stream << " | " << get_window().get_camera().convert_screen_to_world(input::system::GetMousePosition());
    get_window().Title = "TestGame " + stream.str();
}

void PathfindingEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    default: break;
    }
}

void PathfindingEx::on_mouse_button_down(mouse::button_event const& ev)
{
    if (ev.Button == mouse::button::Left) {
        _start = point_i {static_cast<i32>(ev.Position.X / _tileSize.Width), static_cast<i32>(ev.Position.Y / _tileSize.Height)};
        if (_start.X >= GRID_SIZE.Width || _start.Y >= GRID_SIZE.Height) {
            _start = INVALID;
        }
    } else if (ev.Button == mouse::button::Right) {
        _end = point_i {static_cast<i32>(ev.Position.X / _tileSize.Width), static_cast<i32>(ev.Position.Y / _tileSize.Height)};
        if (_end.X >= GRID_SIZE.Width || _end.Y >= GRID_SIZE.Height) {
            _end = INVALID;
        }
    }

    if (_start != INVALID && _end != INVALID) {
        _path = _pathfinder->find_path(_tiles, _start, _end);
    } else {
        _path.clear();
    }
}
