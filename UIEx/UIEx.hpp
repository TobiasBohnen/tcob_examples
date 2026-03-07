// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::ui;

class UIEx : public scene {
public:
    UIEx(game& game);
    ~UIEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target, transform const& xform) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;

private:
    i32 _switch {0};

    i32                                                                             _mode {0};
    std::vector<std::shared_ptr<form_base> (*)(gfx::window&, assets::group const&)> _modes;
};
