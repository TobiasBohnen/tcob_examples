// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"

class TweenEx : public scene {
public:
    TweenEx(game& game);
    ~TweenEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;

private:
    shape_batch                              _layer1;
    std::vector<std::shared_ptr<tween_base>> _tweens;
    asset_owner_ptr<material>                _material;
};
