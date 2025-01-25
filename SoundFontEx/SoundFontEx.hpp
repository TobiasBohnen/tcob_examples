// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

#include "UI.hpp"

using namespace std::chrono_literals;
using namespace tcob::literals;

class SoundFontEx : public scene {
public:
    SoundFontEx(game& game);
    ~SoundFontEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;

private:
    void play_note(milliseconds dur, i32 preset, midi_note note);

    assets::asset_ptr<sound_font> _soundFont;

    std::shared_ptr<sound> _sound;

    std::shared_ptr<piano_form> _form0;
};
