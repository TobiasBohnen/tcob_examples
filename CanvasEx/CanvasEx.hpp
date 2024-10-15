// Copyright (c) 2024 Tobias Bohnen
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
    void canvas_ray_cast();
    void canvas_gradient();

    canvas          _canvas;
    canvas_renderer _renderer {_canvas};

    point_f _center;
    f32     _rotation {45};
};
