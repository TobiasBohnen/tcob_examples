// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

class SkeletonEx final : public scene {
public:
    explicit SkeletonEx(game& game);
    ~SkeletonEx() override;

protected:
    void on_fixed_update(milliseconds deltaTime) override;
    void on_start() override;
    void on_update(milliseconds deltaTime) override;
    void on_draw_to(gfx::render_target& target, transform const& xform) override;
    void on_key_down(input::keyboard::event const& ev) override;
    void on_mouse_motion(input::mouse::motion_event const& ev) override;
    void on_mouse_button_down(input::mouse::button_event const& ev) override;
    void on_mouse_wheel(input::mouse::wheel_event const& ev) override;

private:
    void create_shapes(isize boneCount);
    void update_shapes(skeletal_animation::poses const& poses);

    gfx::shape_batch                               _layer0 {};
    asset_owner_ptr<gfx::material>                 _material {};
    std::vector<gfx::rect_shape*>                  _slots {};
    std::shared_ptr<gfx::skeletal_animation_tween> _anim {};
    bool                                           _paused {false};
};
