// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

#include <memory>

class CameraEx : public scene {
public:
    explicit CameraEx(game& game);
    ~CameraEx() override;

protected:
    void on_start() override;

    void on_update(milliseconds deltaTime) override;

    void on_draw_to(render_target&, transform const&) override;

    void on_key_down(keyboard::event const&) override;
    void on_mouse_motion(mouse::motion_event const&) override;
    void on_mouse_button_down(mouse::button_event const&) override;
    void on_mouse_wheel(mouse::wheel_event const&) override;

private:
    shape_batch                             _layer0;
    std::unique_ptr<gfx::camera_controller> _controller;
    asset_owner_ptr<material>               _material {material::Empty()};

    static constexpr f32 MoveStep {16.0f};
};
