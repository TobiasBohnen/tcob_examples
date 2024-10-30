// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::gfx::ui;

////////////////////////////////////////////////////////////

class crtl_form : public form {
public:
    crtl_form(window* window);

    std::shared_ptr<label> Controller;

private:
    void gen_styles();

    void on_controller_axis_motion(input::controller::axis_event const& ev) override;
    void on_controller_button_down(input::controller::button_event const& ev) override;
    void on_controller_button_up(input::controller::button_event const& ev) override;

    assets::manual_asset_ptr<font_family> _font;

    std::shared_ptr<slider> _laxisx;
    std::shared_ptr<slider> _laxisy;
    std::shared_ptr<slider> _raxisx;
    std::shared_ptr<slider> _raxisy;
    std::shared_ptr<slider> _laxis;
    std::shared_ptr<slider> _raxis;

    std::shared_ptr<label> _button;

    input::system& _input {locate_service<input::system>()};
};

////////////////////////////////////////////////////////////
