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

    _soundFont0             = resGrp->get<sound_font>("font0");
    get_root_node()->Entity = _form0;

    _form0->C->Click.connect([&]() { play_note(quarter, 0, midi_note::C4); });
    _form0->CSharp->Click.connect([&]() { play_note(quarter, 0, midi_note::CSharp4); });
    _form0->D->Click.connect([&]() { play_note(quarter, 0, midi_note::D4); });
    _form0->DSharp->Click.connect([&]() { play_note(quarter, 0, midi_note::DSharp4); });
    _form0->E->Click.connect([&]() { play_note(quarter, 0, midi_note::E4); });
    _form0->F->Click.connect([&]() { play_note(quarter, 0, midi_note::F4); });
    _form0->FSharp->Click.connect([&]() { play_note(quarter, 0, midi_note::FSharp4); });
    _form0->G->Click.connect([&]() { play_note(quarter, 0, midi_note::G4); });
    _form0->GSharp->Click.connect([&]() { play_note(quarter, 0, midi_note::GSharp4); });
    _form0->A->Click.connect([&]() { play_note(quarter, 0, midi_note::A4); });
    _form0->ASharp->Click.connect([&]() { play_note(quarter, 0, midi_note::ASharp4); });
    _form0->B->Click.connect([&]() { play_note(quarter, 0, midi_note::B4); });
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
    commands.start_new_section(0.0s);
    commands.add<note_off_all_command>();

    _sound = _soundFont0->create_sound(commands);
    _sound.play();
}
