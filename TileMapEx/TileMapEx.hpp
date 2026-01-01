// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"

class TileMapEx : public scene {
public:
    TileMapEx(game& game);
    ~TileMapEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_button_down(mouse::button_event const& ev) override;
    void on_mouse_button_up(mouse::button_event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;
    void on_mouse_wheel(mouse::wheel_event const& ev) override;

private:
    rng _rand;

    orthogonal_tilemap _tileMapOrtho;
    isometric_tilemap  _tileMapIso;
    isometric_tilemap  _tileMapIsoStaggered;
    hexagonal_tilemap  _tileMapHexPointy;
    hexagonal_tilemap  _tileMapHexFlat;

    tilemap_layer* _layer {nullptr};

    basic_cam _cam;

    std::shared_ptr<frame_animation_tween> _tween;
};
