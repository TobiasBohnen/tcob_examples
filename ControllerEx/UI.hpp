// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::ui;

////////////////////////////////////////////////////////////

class crtl_form : public form<dock_layout> {
public:
    crtl_form(window& window);

    label* Controller;

private:
    void gen_styles();

    void on_controller_axis_motion(input::controller::axis_event const& ev) override;
    void on_controller_button_down(input::controller::button_event const& ev) override;
    void on_controller_button_up(input::controller::button_event const& ev) override;

    asset_owner_ptr<font_family> _font;

    slider* _laxisx {nullptr};
    slider* _laxisy {nullptr};
    slider* _raxisx {nullptr};
    slider* _raxisy {nullptr};
    slider* _laxis {nullptr};
    slider* _raxis {nullptr};

    label* _button {nullptr};

    input::system& _input {locate_service<input::system>()};
};

////////////////////////////////////////////////////////////
