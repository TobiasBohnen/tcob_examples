// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "SoundFontEx.hpp"

using namespace std::chrono_literals;
using namespace tcob::literals;

SoundFontEx::SoundFontEx(game& game)
    : scene {game}
{
}

SoundFontEx::~SoundFontEx() = default;

void SoundFontEx::on_start()
{
    auto& resMgr {get_game().get_library()};
    auto* resGrp {resMgr.get_group("res")};

    _soundFont0 = resGrp->get<sound_font>("font0");
    //  _midi0                 = std::make_unique<music>();
    //  _midi0->DecoderContext = _soundFont0;
}

void SoundFontEx::on_draw_to(render_target& target)
{
}

void SoundFontEx::on_update(milliseconds deltaTime)
{
}

void SoundFontEx::on_key_down(keyboard::event const& ev)
{
    auto addNote = [](sound_font_commands& commands, auto&& dur, auto&& preset, auto&& note) {
        commands.start_new_section(dur);
        commands.add<note_on_command>(preset, note, 1.0f);
        commands.start_new_section(0.0s);
        commands.add<note_off_all_command>();
    };
    seconds const full {2s};
    seconds const half {full / 2};
    seconds const quarter {half / 2};
    seconds const eighth {quarter / 2};

    switch (ev.ScanCode) { // NOLINT
    case scan_code::R: {
        auto _ = get_window().copy_to_image().save("screen01.webp");
    } break;
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    case scan_code::A: {
        sound_font_commands commands;
        addNote(commands, quarter, 0, midi_note::A4);
        _sound = _soundFont0->create_sound(commands);
        _sound.play();
    } break;

    case scan_code::F: {
        sound_font_commands commands;

        for (i32 preset {0}; preset < std::min(4, _soundFont0->get_preset_count()); ++preset) {
            addNote(commands, quarter, preset, midi_note::C4);
            addNote(commands, quarter, preset, midi_note::E4);
            addNote(commands, quarter, preset, midi_note::G4);
            addNote(commands, quarter, preset, midi_note::A4);
            addNote(commands, half, preset, midi_note::G4);
            addNote(commands, eighth, preset, midi_note::F4);
            addNote(commands, eighth, preset, midi_note::E4);
            addNote(commands, quarter, preset, midi_note::D4);
            addNote(commands, half, preset, midi_note::C4);
        }
        _sound = _soundFont0->create_sound(commands);
        _sound.play();

        auto buffer {_soundFont0->create_buffer(commands)};
        auto _ = buffer.save("midiX.opus");
        _      = buffer.save("midiX.ogg");
        _      = buffer.save("midiX.wav");
    } break;
    default:
        break;
    }
}
