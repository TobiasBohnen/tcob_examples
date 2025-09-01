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

    signal<string const> Play;

private:
    void gen_styles();

    asset_owner_ptr<font_family> _font;
};

////////////////////////////////////////////////////////////
