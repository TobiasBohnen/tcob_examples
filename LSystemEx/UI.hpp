// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::ui;

////////////////////////////////////////////////////////////
struct settings {
    string   String;
    f32      LineLength {};
    degree_f TurningAngle {};
    f32      StrokeWidth {};
};

class lsystem_form : public form<dock_layout> {
public:
    lsystem_form(rect_i const& bounds);

    signal<> Start;
    auto     get_settings() const -> settings;

private:
    void gen_styles();
    void create_rule(grid_layout& layout);
    void create_settings(grid_layout& layout);

    text_box*  _txbAxiom {nullptr};
    spinner*   _spnIterations {nullptr};
    spinner*   _spnAngle {nullptr};
    spinner*   _spnLine {nullptr};
    spinner*   _spnStroke {nullptr};
    grid_view* _grdRules {nullptr};

    asset_owner_ptr<font_family> _font;
};
