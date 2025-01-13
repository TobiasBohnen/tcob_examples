// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

class AniTexEx : public scene {
public:
    AniTexEx(game& game);
    ~AniTexEx() override;

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
    shape_batch _layer0;

    owning_asset_ptr<animated_texture> _texGif;
    owning_asset_ptr<material>         _matGif;

    owning_asset_ptr<animated_texture> _texWebp;
    owning_asset_ptr<material>         _matWebp;

    ;

    basic_cam _cam;
};
