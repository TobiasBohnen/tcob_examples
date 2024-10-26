// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "SoundFontEx.hpp"

SoundFontEx::SoundFontEx(game& game)
    : scene {game}
    , _form0 {std::make_shared<piano_form>(&get_window())}
{
}

SoundFontEx::~SoundFontEx() = default;

seconds const full {2s}; // 120 bpm
seconds const half {full / 2};
seconds const quarter {half / 2};
seconds const eighth {quarter / 2};

void SoundFontEx::on_start()
{
    auto& resMgr {get_game().get_library()};
    auto* resGrp {resMgr.get_group("res")};

    _soundFont              = resGrp->get<sound_font>("font0");
    get_root_node()->Entity = _form0;

    auto mouseDown {[&](auto&& btn, midi_note baseNote) {
        btn->MouseDown.connect([&, baseNote]() {
            auto const note {static_cast<midi_note>(static_cast<u8>(baseNote) + (_form0->Octave->Value() + 1) * 12)};
            play_note(quarter, 0, note);
        });
    }};
    mouseDown(_form0->C, midi_note::CNeg1);
    mouseDown(_form0->CSharp, midi_note::CSharpNeg1);
    mouseDown(_form0->D, midi_note::DNeg1);
    mouseDown(_form0->DSharp, midi_note::DSharpNeg1);
    mouseDown(_form0->E, midi_note::ENeg1);
    mouseDown(_form0->F, midi_note::FNeg1);
    mouseDown(_form0->FSharp, midi_note::FSharpNeg1);
    mouseDown(_form0->G, midi_note::GNeg1);
    mouseDown(_form0->GSharp, midi_note::GSharpNeg1);
    mouseDown(_form0->A, midi_note::ANeg1);
    mouseDown(_form0->ASharp, midi_note::ASharpNeg1);
    mouseDown(_form0->B, midi_note::BNeg1);
}

void SoundFontEx::on_draw_to(render_target& target)
{
}

void SoundFontEx::on_update(milliseconds deltaTime)
{
}

void SoundFontEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::R: {
        auto _ = get_window().copy_to_image().save("screen01.webp");
    } break;
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;

    default:
        break;
    }
}

void SoundFontEx::play_note(milliseconds dur, i32 preset, midi_note note)
{
    sound_font_commands commands;
    commands.start_new_section(dur);
    commands.add<note_on_command>(preset, note, 1.0f);
    commands.start_new_section(half);
    commands.add<note_off_all_command>();

    _sound = _soundFont->create_sound(commands);
    _sound.play();
}
