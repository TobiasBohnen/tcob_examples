// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <tcob/tcob.hpp>
using namespace tcob;
using namespace tcob::gfx;
using namespace tcob::assets;
using namespace tcob::tweening;
using namespace tcob::input;

class TextEx : public scene {
public:
    TextEx(game& game);
    ~TextEx();

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event& ev) override;
    void on_mouse_motion(mouse::motion_event& ev) override;

private:
    std::vector<std::unique_ptr<text>> _texts;
};
