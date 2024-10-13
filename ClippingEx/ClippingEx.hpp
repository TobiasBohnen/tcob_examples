// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

class ClippingEx : public scene {
public:
    ClippingEx(game& game);
    ~ClippingEx() override;

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

    shape_batch                            _layer0;
    std::shared_ptr<tcob::gfx::poly_shape> _polyShape;
    std::shared_ptr<tcob::gfx::poly_shape> _cutShape;
};
