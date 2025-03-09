// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::ui;

////////////////////////////////////////////////////////////

class piano_form : public form {
public:
    piano_form(window* window);

    std::shared_ptr<button> C;
    std::shared_ptr<button> CSharp;
    std::shared_ptr<button> D;
    std::shared_ptr<button> DSharp;
    std::shared_ptr<button> E;
    std::shared_ptr<button> F;
    std::shared_ptr<button> FSharp;
    std::shared_ptr<button> G;
    std::shared_ptr<button> GSharp;
    std::shared_ptr<button> A;
    std::shared_ptr<button> ASharp;
    std::shared_ptr<button> B;

    std::shared_ptr<slider> Octave;

private:
    void gen_styles();

    assets::owning_asset_ptr<font_family> _font;
    assets::owning_asset_ptr<texture>     _tex;
};
