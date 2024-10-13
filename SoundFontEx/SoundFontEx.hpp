// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

// TODO: add ui
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
    assets::asset_ptr<sound_font> _soundFont0;
    sound                         _sound;
};
