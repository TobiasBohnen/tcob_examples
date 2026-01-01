// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::ui;

////////////////////////////////////////////////////////////

class piano_form : public form<dock_layout> {
public:
    piano_form(window& window);

    button* C {nullptr};
    button* CSharp {nullptr};
    button* D {nullptr};
    button* DSharp {nullptr};
    button* E {nullptr};
    button* F {nullptr};
    button* FSharp {nullptr};
    button* G {nullptr};
    button* GSharp {nullptr};
    button* A {nullptr};
    button* ASharp {nullptr};
    button* B {nullptr};

    slider* Octave {nullptr};

private:
    void gen_styles();

    asset_owner_ptr<font_family> _font;
    asset_owner_ptr<texture>     _tex;
};
