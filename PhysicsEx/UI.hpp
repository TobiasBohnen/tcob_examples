// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::ui;

////////////////////////////////////////////////////////////

enum class spawn_object : u8 {
    Circle,
    Box,
    Polygon,
    Capsule,
    Explosion
};

class physics_form : public form<dock_layout> {
public:
    physics_form(rect_i const& bounds);

    spawn_object SpawnObject {spawn_object::Box};
    bool         DebugMode {false};

    prop<point_f> Gravity {{0, 50.f}};
    signal<>      CreateObstacles;
    signal<>      ClearObjects;

private:
    void gen_styles();

    asset_owner_ptr<font_family> _font;
};
