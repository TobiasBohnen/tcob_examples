// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PathfindingEx.hpp"
#include <algorithm>
#include <iomanip>

using namespace std::chrono_literals;

auto static create_tileset() -> std::unordered_map<tile_index_t, orthogonal_tile>
{
    std::unordered_map<tile_index_t, orthogonal_tile> retValue;

    auto const gradLow {color_gradient {{0.0f, colors::White}, {0.10f, colors::LightGreen}, {1.0f, colors::Green}}.colors()};
    auto const gradHigh {color_gradient {{0.0f, colors::White}, {0.10f, colors::LightSalmon}, {1.0f, colors::Red}}.colors()};
    for (i32 i {0}; i < 256; ++i) {
        if (i > 127) {
            retValue[i + 1] = {.Color = gradHigh[(i - 128) * 2]};
        } else {
            retValue[i + 1] = {.Color = gradLow[(127 - i) * 2]};
        }
    }

    return retValue;
}

PathfindingEx::PathfindingEx(game& game)
    : scene(game)
    , _tileMapOrtho {tileset<orthogonal_tile> {create_tileset()}}
{
    window().ClearColor = colors::DarkRed;
}

PathfindingEx::~PathfindingEx() = default;

void PathfindingEx::on_start()
{
    f32              min {2000}, max {0};
    perlin_noise     noise {20};
    std::vector<f32> values;
    for (i32 x {0}; x < GRID_SIZE.Width; ++x) {
        for (i32 y {0}; y < GRID_SIZE.Height; ++y) {
            point_f pos {static_cast<f32>(x), static_cast<f32>(y)};
            pos /= GRID_SIZE.Width;
            f32 const f {1 - noise(pos)};
            min = std::min(min, f);
            max = std::max(max, f);
            values.push_back(f);
        }
    }
    for (u32 i {0}; i < values.size(); ++i) {
        tile_index_t const val {static_cast<tile_index_t>(std::clamp((values[i] - min) / (max - min) * 256 + 1, 1.0f, 256.0f))};

        _tiles[i] = val;
    }

    f32 const size {(*window().Size).Height / static_cast<f32>(GRID_SIZE.Height)};
    _tileSize          = {size, size};
    _tileMapOrtho.Grid = {.TileSize = _tileSize};

    tcob::gfx::tilemap_layer layer {.Tiles = _tiles};
    _tileMapOrtho.add_layer(layer);
    _tileMapOrtho.Material = material::Empty();
}

void PathfindingEx::on_draw_to(render_target& target)
{
    _tileMapOrtho.draw_to(target);

    auto const size {*target.Size};

    _canvas.begin_frame(size, 1);

    _canvas.set_edge_antialias(false);

    // path
    _canvas.begin_path();
    _canvas.set_fill_style(colors::Blue);
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
    _renderer.set_bounds({point_f::Zero, size_f {*window().Size}});
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
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    auto const pos {window().camera().convert_screen_to_world(locate_service<input::system>().mouse()->get_position())};
    stream << " | " << pos.X << ":" << pos.Y;
    window().Title = "TestGame " + stream.str();
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
        _path = _pathfinder.find_path(_costs, _tiles.size(), _start, _end);
    } else {
        _path.clear();
    }
}
