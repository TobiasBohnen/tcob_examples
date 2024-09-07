// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::gfx::ui;

#include "UI.hpp"

////////////////////////////////////////////////////////////

class SoundGeneratorEx : public scene {
public:
    SoundGeneratorEx(game& game);
    ~SoundGeneratorEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event& ev) override;
    void on_mouse_motion(mouse::motion_event& ev) override;

private:
    void create_data();
    void play_wave();
    void draw_wave();

    sound_generator _gen1;
    sound_wave      _wave1;
    buffer          _audioData;
    sound           _sound1;

    enum class wave_state {
        Dirty,
        Generating,
        Ready,
        Clean
    };
    std::atomic<wave_state> _waveState {wave_state::Clean};

    std::shared_ptr<generator_form> _form0;
};
