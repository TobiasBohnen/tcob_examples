// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "TileMapEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

TileMapEx::TileMapEx(game& game)
    : scene(game)
    , _tileMapOrtho {{{
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
    , _tileMapIso {{{
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
    , _tileMapIsoStaggered {{{
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
    , _tileMapHexPointy {{{
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
    , _tileMapHexFlat {{{
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

    std::array<u64, tmWidth * tmHeight> tiles1 {};
    for (u32 i {0}; i < tmWidth * tmHeight; i++) {
        tiles1[i] = rnd(1, 12);
    }

    ////////////////////////////////////////////////////////////
    {
        _tileMapOrtho.Material = resGrp->get<material>("ortho");
        _tileMapOrtho.Grid     = {.TileSize = {64, 64}};

        tcob::gfx::tilemap_layer layer;
        layer.Tiles = tiles1;
        layer.Size  = {tmWidth, tmHeight};
        _layerID    = _tileMapOrtho.add_layer(layer);
    }
    ////////////////////////////////////////////////////////////
    {
        _tileMapIso.Material = resGrp->get<material>("iso");
        _tileMapIso.Grid     = {.TileSize = {128, 64}, .Staggered = false};

        _tileMapIso.Position = {6800, 30};

        tcob::gfx::tilemap_layer layer;
        layer.Tiles = tiles1;
        layer.Size  = {tmWidth, tmHeight};
        _tileMapIso.add_layer(layer);
    }
    ////////////////////////////////////////////////////////////
    {
        _tileMapIsoStaggered.Material = resGrp->get<material>("iso");
        _tileMapIsoStaggered.Grid     = {.TileSize = {128, 64}, .Staggered = true};

        _tileMapIsoStaggered.Position = {1500, 0};

        tcob::gfx::tilemap_layer layer;
        layer.Tiles = tiles1;
        layer.Size  = {tmWidth, tmHeight};
        _tileMapIsoStaggered.add_layer(layer);
    }
    ////////////////////////////////////////////////////////////
    {
        _tileMapHexPointy.Material = resGrp->get<material>("hex");
        _tileMapHexPointy.Grid     = {.TileSize = {64, 64}, .Top = hex_top::Pointy};

        _tileMapHexPointy.Position = {0, 2800};

        tcob::gfx::tilemap_layer layer;
        layer.Tiles = tiles1;
        layer.Size  = {tmWidth, tmHeight};
        _tileMapHexPointy.add_layer(layer);
    }

    ////////////////////////////////////////////////////////////
    {
        _tileMapHexFlat.Material = resGrp->get<material>("hexflat");
        _tileMapHexFlat.Grid     = {.TileSize = {64, 64}, .Top = hex_top::Flat};

        _tileMapHexFlat.Position = {2000, 2800};

        tcob::gfx::tilemap_layer layer;
        layer.Tiles = tiles1;
        layer.Size  = {tmWidth, tmHeight};
        _tileMapHexFlat.add_layer(layer);
    }

    ////////////////////////////////////////////////////////////
    frame_animation ani {};
    ani.Frames = {{"stone1", 1s}, {"dirt1", 1s}, {"grass1", 1s}, {"stone3", 1s}, {"grass4", 1s}};

    _tween = std::make_shared<frame_animation_tween>(ani.get_duration(), ani);

    _tween->Value.Changed.connect([this](auto const& str) {
        std::unordered_map<string, tile_index_t> static map {{"stone1", 1}, {"stone2", 2}, {"stone3", 3}, {"stone4", 4}, {"dirt1", 5}, {"dirt2", 6}, {"dirt3", 7}, {"dirt4", 8}, {"grass1", 9}, {"grass2", 10}, {"grass3", 11}, {"grass4", 12}};
        _tileMapOrtho.set_tile_index(_layerID, {0, 0}, map[str]);
    });
    _tween->start(playback_mode::Looped);
}

void TileMapEx::on_draw_to(render_target& target)
{
    _tileMapOrtho.draw_to(target);
    _tileMapIso.draw_to(target);
    _tileMapIsoStaggered.draw_to(target);
    _tileMapHexPointy.draw_to(target);
    _tileMapHexFlat.draw_to(target);
}

void TileMapEx::on_update(milliseconds deltaTime)
{
    _tween->update(deltaTime);

    _tileMapOrtho.update(deltaTime);
    _tileMapIso.update(deltaTime);
    _tileMapIsoStaggered.update(deltaTime);
    _tileMapHexPointy.update(deltaTime);
    _tileMapHexFlat.update(deltaTime);
}

void TileMapEx::on_fixed_update(milliseconds deltaTime)
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

void TileMapEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    case scan_code::D1:
        for (i32 i = 0; i < tmHeight; i++) {
            _tileMapOrtho.set_tile_index(_layerID, {0, i}, 2);
        }
        break;
    case scan_code::D2:
        for (i32 i = 1; i <= 12; i++) {
            _tileMapOrtho.change_tileset(i, {"stone1"});
        }

        break;
    case scan_code::D3: {
        std::array<tile_index_t, 10> tiles {};
        for (int i = 0; i < 10; i++) {
            tiles[i] = 1;
        }
        tcob::gfx::tilemap_layer layer;
        layer.Tiles = tiles;
        layer.Size  = {2, 5};
        _tileMapOrtho.add_layer(layer);
    } break;
    case scan_code::D4: {
        std::array<tile_index_t, 10> tiles {};
        for (int i = 0; i < 10; i++) {
            tiles[i] = 2;
        }
        tcob::gfx::tilemap_layer layer;
        layer.Tiles  = tiles;
        layer.Size   = {2, 5};
        layer.Offset = {4, 0};
        _tileMapOrtho.add_layer(layer);
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
