// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"

class ControllerEx : public scene {
public:
    ControllerEx(game& game);
    ~ControllerEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;

    void on_controller_axis_motion(controller::axis_event const& ev) override;
    void on_controller_button_down(controller::button_event const& ev) override;
    void on_controller_button_up(controller::button_event const& ev) override;

private:
    input::system& _input {locate_service<input::system>()};

    assets::manual_asset_ptr<font> _font;

    std::shared_ptr<text> _text;
    std::shared_ptr<text> _controllerDesc;
    std::shared_ptr<text> _button;

    std::shared_ptr<text> _axis;
    std::shared_ptr<text> _laxisx;
    std::shared_ptr<text> _laxisy;
    std::shared_ptr<text> _raxisx;
    std::shared_ptr<text> _raxisy;
};
