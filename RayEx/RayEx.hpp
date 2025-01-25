// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

class RayEx : public scene {
public:
    RayEx(game& game);
    ~RayEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;
    void on_mouse_button_down(mouse::button_event const& ev) override;
    void on_mouse_wheel(mouse::wheel_event const& ev) override;

private:
    shape_batch       _batch;
    linear_tween<f32> _tween0 {seconds {10}, {50, 650}};
    linear_tween<f32> _tween1 {seconds {10}, {90, 180}};

    point_f _center;
    f32     _rotation {45};

    i32 _mode {0};

    bool _dirty {true};
};
