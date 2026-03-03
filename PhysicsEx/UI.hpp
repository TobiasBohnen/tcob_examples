// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::ui;

////////////////////////////////////////////////////////////

// spawn
enum class left_button_mode : u8 {
    Circle,
    Box,
    Polygon,
    Capsule
};
// world
enum class middle_button_mode : u8 {
    Explosion,
    ReverseGravity
};
// modify
enum class right_button_mode : u8 {
    Remove,
    AngularImpulse,
    LinearImpulse
};

class physics_form : public form<dock_layout> {
public:
    physics_form(rect_i const& bounds);

    left_button_mode   LeftMode {left_button_mode::Box};
    middle_button_mode MiddleMode {middle_button_mode::Explosion};
    right_button_mode  RightMode {right_button_mode::Remove};

    bool DebugMode {false};

    signal<> CreateObstacles;
    signal<> ClearObjects;

private:
    void gen_styles();

    asset_owner_ptr<font_family> _font;
};
