// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

class CanvasEx : public scene {
public:
    CanvasEx(game& game);
    ~CanvasEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;
    void on_mouse_wheel(mouse::wheel_event const& ev) override;

private:
    void canvas_ring();
    void canvas_ray_cast();
    void canvas_gradient();
    void canvas_path2d();
    void canvas_fancy_lines();
    void canvas_text();
    void canvas_clip();

    canvas          _canvas;
    canvas_renderer _renderer {_canvas};

    point_f _mp;
    f32     _value {45};

    font_family _font {""};

    i32                               _mode {0};
    std::vector<void (CanvasEx::*)()> _modes;
};
