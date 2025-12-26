// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

class StencilEx : public scene {
public:
    StencilEx(game& game);
    ~StencilEx() override;

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
    void create_shapes();
    void update_shapes();

    shape_batch _layer0;

    gfx::poly_shape*          _star {nullptr};
    asset_owner_ptr<material> _mat0;

    gfx::rect_shape*          _back {nullptr};
    asset_owner_ptr<material> _mat1;

    gfx::rect_shape*          _front {nullptr};
    asset_owner_ptr<material> _mat2;

    sine_wave_tween<f32> _tween {seconds {5}};
};
