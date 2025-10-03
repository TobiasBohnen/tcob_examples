// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "TileMapEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

TileMapEx::TileMapEx(game& game)
    : scene(game)
    , _tileMapOrtho {orthogonal_tilemap::set {
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
      }}
    , _tileMapIso {isometric_tilemap::set {
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
      }}
    , _tileMapIsoStaggered {isometric_tilemap::set {
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
      }}
    , _tileMapHexPointy {hexagonal_tilemap::set {
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
      }}
    , _tileMapHexFlat {hexagonal_tilemap::set {
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
      }}
{
    window().ClearColor = colors::Cornsilk;
}

TileMapEx::~TileMapEx() = default;

i32 const tmHeight {40}, tmWidth {20};

void TileMapEx::on_start()
{
    auto* resGrp {library().get_group("res")};

    grid<tile_index_t> tiles {size_i {tmWidth, tmHeight}};
    for (i32 x {0}; x < tiles.width(); x++) {
        for (i32 y {0}; y < tiles.height(); y++) {
            tiles[x, y] = _rand(1, 12);
        }
    }

    ////////////////////////////////////////////////////////////
    {
        _tileMapOrtho.Material = resGrp->get<material>("ortho");
        _tileMapOrtho.Grid     = {.TileSize = {64, 64}};

        _layer        = &_tileMapOrtho.create_layer();
        _layer->Tiles = tiles;
    }
    ////////////////////////////////////////////////////////////
    {
        _tileMapIso.Material = resGrp->get<material>("iso");
        _tileMapIso.Grid     = {.TileSize = {128, 64}, .Staggered = false};

        _tileMapIso.Position = {6800, 30};

        auto* layer  = &_tileMapIso.create_layer();
        layer->Tiles = tiles;
    }
    ////////////////////////////////////////////////////////////
    {
        _tileMapIsoStaggered.Material = resGrp->get<material>("iso");
        _tileMapIsoStaggered.Grid     = {.TileSize = {128, 64}, .Staggered = true};

        _tileMapIsoStaggered.Position = {1500, 0};

        auto* layer  = &_tileMapIsoStaggered.create_layer();
        layer->Tiles = tiles;
    }
    ////////////////////////////////////////////////////////////
    {
        _tileMapHexPointy.Material = resGrp->get<material>("hex");
        _tileMapHexPointy.Grid     = {.TileSize = {64, 64}, .Top = hexagonal_top::Pointy};

        _tileMapHexPointy.Position = {0, 2800};

        auto* layer  = &_tileMapHexPointy.create_layer();
        layer->Tiles = tiles;
    }

    ////////////////////////////////////////////////////////////
    {
        _tileMapHexFlat.Material = resGrp->get<material>("hexflat");
        _tileMapHexFlat.Grid     = {.TileSize = {64, 64}, .Top = hexagonal_top::Flat};

        _tileMapHexFlat.Position = {2000, 2800};

        auto* layer  = &_tileMapHexFlat.create_layer();
        layer->Tiles = tiles;
    }

    ////////////////////////////////////////////////////////////
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
    if (_tween) {
        _tween->update(deltaTime);
    }

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
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    auto const pos {window().camera().convert_screen_to_world(locate_service<input::system>().mouse().get_position())};
    stream << " | " << pos.X << ":" << pos.Y;
    window().Title = "TestGame " + stream.str();
}

void TileMapEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    case scan_code::D1: {
        _layer->Tiles.mutate([&](auto& tiles) {
            for (i32 x {0}; x < tiles.width(); x++) {
                for (i32 y {0}; y < tiles.height(); y++) {
                    tiles[x, y] = _rand(1, 12);
                }
            }
        });
    } break;
    case scan_code::D2:
        _tileMapOrtho.clear();
        break;
    case scan_code::D3: {
        grid<tile_index_t> tiles {size_i {2, 5}, 1};
        _layer         = &_tileMapOrtho.create_layer();
        _layer->Tiles  = tiles;
        _layer->Offset = {1, 1};
    } break;
    case scan_code::D4: {
        grid<tile_index_t> tiles {size_i {2, 5}, 2};
        auto*              layer {&_tileMapIso.create_layer()};
        _layer->Tiles  = tiles;
        _layer->Offset = {4, 0};
        _layer->Tiles.mutate([&](auto& tiles) {
            tiles[0, 20] = 5;
        });
    } break;
    case scan_code::D5: {
        frame_animation ani {};
        ani.Frames = {{"stone1", 250ms}, {"dirt1", 250ms}, {"grass1", 250ms}, {"stone3", 250ms}, {"grass4", 250ms}};
        _tween     = std::make_shared<frame_animation_tween>(ani.duration(), ani);
        _tween->Value.Changed.connect([this](auto const& str) {
            std::unordered_map<string, tile_index_t> static map {{"stone1", 1}, {"stone2", 2}, {"stone3", 3}, {"stone4", 4}, {"dirt1", 5}, {"dirt2", 6}, {"dirt3", 7}, {"dirt4", 8}, {"grass1", 9}, {"grass2", 10}, {"grass3", 11}, {"grass4", 12}};

            for (i32 i {0}; i < 100; ++i) {
                i32 const x {_rand(0, tmWidth - 1)};
                i32 const y {_rand(0, tmHeight - 1)};
                _layer->Tiles.mutate([&](auto& tiles) {
                    tiles[x, y] = map[str];
                });
            }
        });
        _tween->start(playback_mode::Looped);
    } break;

    case scan_code::R: _tileMapOrtho.clear(); break;
    default:           break;
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
