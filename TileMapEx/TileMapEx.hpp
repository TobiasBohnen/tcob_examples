// Copyright (c) 2024 Tobias Bohnen
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
    ortho_tilemap _tileMapOrtho;
    iso_tilemap   _tileMapIso;
    iso_tilemap   _tileMapIsoStaggered;
    hex_tilemap   _tileMapHexPointy;
    hex_tilemap   _tileMapHexFlat;

    uid _layerID {INVALID_ID};

    basic_cam _cam;

    std::shared_ptr<frame_animation_tween> _tween;
};
