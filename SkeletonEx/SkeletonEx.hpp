// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

class SkeletonEx final : public tcob::scene {
public:
    explicit SkeletonEx(tcob::game& game);
    ~SkeletonEx() override;

protected:
    void on_fixed_update(tcob::milliseconds deltaTime) override;
    void on_start() override;
    void on_update(tcob::milliseconds deltaTime) override;
    void on_draw_to(tcob::gfx::render_target& target, tcob::transform const& xform) override;
    void on_key_down(tcob::input::keyboard::event const& ev) override;
    void on_mouse_motion(tcob::input::mouse::motion_event const& ev) override;
    void on_mouse_button_down(tcob::input::mouse::button_event const& ev) override;
    void on_mouse_wheel(tcob::input::mouse::wheel_event const& ev) override;

private:
    void create_shapes(isize boneCount);
    void update_shapes(std::vector<tcob::transform> const& pose);

    tcob::gfx::shape_batch                               _layer0 {};
    tcob::asset_ptr<tcob::gfx::material>                 _material {};
    std::vector<tcob::gfx::rect_shape*>                  _slots {};
    std::shared_ptr<tcob::gfx::skeletal_animation_tween> _anim {};
    bool                                                 _paused {false};
};
