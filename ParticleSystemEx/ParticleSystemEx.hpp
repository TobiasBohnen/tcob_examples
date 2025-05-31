// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"

class ParticleSystemEx : public scene {
public:
    ParticleSystemEx(game& game);
    ~ParticleSystemEx();

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_button_down(mouse::button_event const& ev) override;
    void on_mouse_button_up(mouse::button_event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;
    void on_mouse_wheel(mouse::wheel_event const& ev) override;

private:
    void load_emitter(quad_particle_emitter& emi);

    quad_particle_system  _system0;
    point_particle_system _system1;

    basic_cam          _cam;
    bool               _reverse {false};
    std::vector<color> _colors;
};
