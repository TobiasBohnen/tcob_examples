// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "SoundGeneratorEx.hpp"

#include <iomanip>

using namespace tcob::literals;
using namespace std::chrono_literals;

////////////////////////////////////////////////////////////

SoundGeneratorEx::SoundGeneratorEx(game& game)
    : scene(game)
    , _form0 {std::make_shared<generator_form>(&get_window())}
{
}

SoundGeneratorEx::~SoundGeneratorEx() = default;

void SoundGeneratorEx::on_start()
{
    get_root_node()->Entity = _form0;
    _form0->NewWave.connect([&] {
        if (_waveState == wave_state::Clean) {
            _waveState = wave_state::Dirty;
        }
    });
    _form0->GenPickupCoin->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_pickup_coin()); });
    _form0->GenLaserShot->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_laser_shoot()); });
    _form0->GenExplosion->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_explosion()); });
    _form0->GenPowerup->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_powerup()); });
    _form0->GenHitHurt->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_hit_hurt()); });
    _form0->GenJump->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_jump()); });
    _form0->GenBlipSelect->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_blip_select()); });
    _form0->GenRandom->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_random()); });
    _form0->Play->MouseDown.connect([&]() { create_data(); play_wave(); });
    _form0->Mutate->MouseDown.connect([&]() { _form0->set_values(_gen1.mutate_wave(_wave1)); });
    _form0->Load->MouseDown.connect([&]() {
        config::object loadFile;
        if (loadFile.load("sound_wave0.ini") == load_status::Ok) {
            Deserialize(_wave1, loadFile["wave"]);
            _form0->set_values(_wave1);
        }
    });
    _form0->Save->MouseDown.connect([&]() {
        config::object saveFile;
        Serialize(_wave1, saveFile["wave"]);

        saveFile.save("sound_wave0.ini");
    });
    _form0->Export->MouseDown.connect([&]() {
        string name {};
        i32    i {0};
        do {
            name = "sound" + std::to_string(i++) + ".wav";
        } while (io::is_file(name));
        auto _ = _audioData.save(name);
    });
    _form0->Exit->MouseDown.connect([&]() {
        get_game().pop_current_scene();
    });
    _form0->fixed_update(0ms);
    draw_wave();
}

void SoundGeneratorEx::on_draw_to(render_target& target)
{
}

void SoundGeneratorEx::on_update(milliseconds deltaTime)
{
}

void SoundGeneratorEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    get_window().Title = "SoundGeneratorEx " + stream.str();

    if (_waveState == wave_state::Dirty) {
        _waveState = wave_state::Generating;

        _form0->get_values(_wave1);
        _wave1.sanitize();

        locate_service<task_manager>().run_async<void>([&] {
            create_data();
            _waveState = wave_state::Ready;
        });
    } else if (_waveState == wave_state::Ready) {
        play_wave();
        draw_wave();
        _waveState = wave_state::Clean;
    }
}

void SoundGeneratorEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    default:
        break;
    }
}

void SoundGeneratorEx::on_mouse_motion(mouse::motion_event const& ev)
{
}

void SoundGeneratorEx::create_data()
{
    _audioData = _gen1.create_buffer(_wave1);
}

void SoundGeneratorEx::play_wave()
{
    _sound1 = sound {_audioData};
    _sound1.play();
}

void SoundGeneratorEx::draw_wave()
{
    auto& canvas {_form0->Canvas};

    rect_f const crect {0, 0, canvas->Bounds->width(), canvas->Bounds->height()};

    canvas->clear();

    // Background
    color c {colors::FloralWhite};
    canvas->set_fill_style(c);
    canvas->begin_path();
    canvas->rect(crect);
    canvas->fill();

    // center line
    canvas->set_stroke_width(1);
    canvas->set_stroke_style(colors::Black);
    std::array<point_f, 2> line {{{crect.left(), crect.height() / 2.f}, {crect.right(), crect.height() / 2.f}}};
    canvas->begin_path();
    canvas->move_to(line[0]);
    for (u32 i {1}; i < line.size(); ++i) {
        canvas->line_to(line[i]);
    }
    canvas->stroke();

    // waveform
    auto audioData {_audioData.get_data()};
    if (!audioData.empty()) {
        f32       currentSample {0.0f};
        f32 const sampleSize {crect.width()};
        f32 const sampleIncrement {audioData.size() / sampleSize};
        f32 const sampleScale {crect.height()};

        std::vector<point_f> points;
        points.reserve(sampleSize);
        points.emplace_back(crect.left(), crect.height() / 2.f);

        for (i32 i {1}; i < sampleSize; i++) {
            f32 const sample {std::clamp<f32>(audioData[static_cast<u64>(currentSample)] * sampleScale, -crect.height() / 2.f, crect.height() / 2.f)};
            points.emplace_back(crect.left() + i, (crect.top() + crect.height() / 2.0f) - sample);
            currentSample += sampleIncrement;
        }

        canvas->set_stroke_width(2);
        canvas->set_stroke_style(colors::DarkBlue);

        canvas->begin_path();
        canvas->move_to(points[0]);
        for (u32 i {1}; i < points.size(); ++i) {
            canvas->line_to(points[i]);
        }
        canvas->stroke();
    }
}
