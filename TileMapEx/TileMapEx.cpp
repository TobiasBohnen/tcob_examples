// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "TileMapEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

TileMapEx::TileMapEx(game& game)
    : scene(game)
    , _tileMapOrtho(tileset {{
          {1, {"stone1"}},
          {2, {"stone2"}},
          {3, {"stone3"}},
          {4, {"stone4"}},
          {5, {"dirt1"}},
          {6, {"dirt2"}},
          {7, {"dirt3"}},
          {8, {"dirt4"}},
          {9, {"grass1"}},
          {10, {"grass2"}},
          {11, {"grass3"}},
          {12, {"grass4"}},
      }})
    , _tileMapIso {tileset {{
          {1, {"stone1"}},
          {2, {"stone2"}},
          {3, {"stone3"}},
          {4, {"stone4"}},
          {5, {"dirt1"}},
          {6, {"dirt2"}},
          {7, {"dirt3"}},
          {8, {"dirt4"}},
          {9, {"grass1"}},
          {10, {"grass2"}},
          {11, {"grass3"}},
          {12, {"grass4"}},
      }}}
{
}

TileMapEx::~TileMapEx() = default;

u32 const tmHeight {40}, tmWidth {20};

void TileMapEx::on_start()
{
    auto* resGrp {get_game().get_library().get_group("res")};

    rng rnd;

    std::array<u16, tmWidth * tmHeight> tiles1 {};
    for (u32 i {0}; i < tmWidth * tmHeight; i++) {
        tiles1[i] = rnd(1, 12);
    }

    ////////////////////////////////////////////////////////////
    {
        _tileMapOrtho.Material = resGrp->get<material>("ortho");
        _tileMapOrtho.Grid     = {.TileSize = {65, 65}};

        tcob::gfx::tilemap_layer layer;
        layer.Tiles.insert(layer.Tiles.end(), tiles1.begin(), tiles1.end());
        layer.Size = {tmWidth, tmHeight};
        _tileMapOrtho.add_layer(layer);
    }
    ////////////////////////////////////////////////////////////
    {
        _tileMapIso.Material = resGrp->get<material>("iso");
        _tileMapIso.Grid     = {.TileSize = {110, 128}, .SurfaceCenter = {0.5f, 0.25f}};

        _tileMapIso.Position = {2050, 300};

        tcob::gfx::tilemap_layer layer;
        layer.Tiles.insert(layer.Tiles.end(), tiles1.begin(), tiles1.end());
        layer.Size = {tmWidth, tmHeight};
        _tileMapIso.add_layer(layer);
    }
}

void TileMapEx::on_draw_to(render_target& target)
{
    _tileMapOrtho.draw_to(target);
    _tileMapIso.draw_to(target);
}

void TileMapEx::on_update(milliseconds deltaTime)
{
    _tileMapOrtho.update(deltaTime);
    _tileMapIso.update(deltaTime);
}

void TileMapEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    get_window().Title = "TestGame " + stream.str();
}

void TileMapEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    case scan_code::D1:
        for (i32 i = 0; i < tmHeight; i++) {
            _tileMapOrtho.change_tile(0, {0, i}, 2);
        }
        break;
    case scan_code::D2:
        for (i32 i = 1; i <= 12; i++) {
            _tileMapOrtho.change_tileset(i, {"stone1"});
        }

        break;
    case scan_code::D3: {
        tcob::gfx::tilemap_layer layer;
        for (int i = 0; i < 10; i++) {
            layer.Tiles.push_back(1);
        }
        layer.Size = {2, 5};
        _tileMapOrtho.add_layer(layer);
    } break;
    case scan_code::D4: {
        tcob::gfx::tilemap_layer layer;
        for (int i = 0; i < 10; i++) {
            layer.Tiles.push_back(2);
        }
        layer.Size = {2, 5};
        _tileMapOrtho.add_layer(layer, {4, 0});
    } break;
    case scan_code::R:
        _tileMapOrtho.clear();
        break;
    default:
        break;
    }

    _cam.on_key_down(ev);
}

void TileMapEx::on_mouse_button_down(mouse::button_event const& ev)
{
    _cam.on_mouse_button_down(ev);
}

void TileMapEx::on_mouse_button_up(mouse::button_event const& ev)
{
    _cam.on_mouse_button_up(ev);
}

void TileMapEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _cam.on_mouse_motion(ev);
}

void TileMapEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _cam.on_mouse_wheel(ev);
}
