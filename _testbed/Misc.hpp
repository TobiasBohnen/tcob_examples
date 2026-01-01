// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <tcob/tcob.hpp>

using namespace tcob;
using namespace tcob::gfx;
using namespace tcob::assets;
using namespace tcob::scripting;
using namespace tcob::physics;
using namespace tcob::audio;
using namespace tcob::data;
using namespace tcob::input;

////////////////////////////////////////////////////////////

class MiscScene : public scene {
public:
    MiscScene(game& game);
    ~MiscScene() override;

protected:
    void on_start() override;
    void on_finish() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;

    void on_mouse_wheel(mouse::wheel_event const& ev) override;

private:
};
