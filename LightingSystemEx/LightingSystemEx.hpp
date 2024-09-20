// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"

class LightingSystemEx : public scene {
public:
    LightingSystemEx(game& game);
    ~LightingSystemEx();

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event& ev) override;
    void on_mouse_button_down(mouse::button_event& ev) override;
    void on_mouse_button_up(mouse::button_event& ev) override;
    void on_mouse_motion(mouse::motion_event& ev) override;
    void on_mouse_wheel(mouse::wheel_event& ev) override;

private:
    lighting_system               _lightingSystem0;
    std::shared_ptr<light_source> _lightSource0;
    std::shared_ptr<light_source> _lightSourceSec0;
    std::shared_ptr<light_source> _lightSourceSec1;
    basic_cam                     _cam;

    assets::manual_asset_ptr<material>  _material;
    std::shared_ptr<static_shape_batch> _layer1;
};
