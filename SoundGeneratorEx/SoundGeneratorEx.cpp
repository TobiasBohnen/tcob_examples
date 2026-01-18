// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "SoundGeneratorEx.hpp"

using namespace tcob::literals;
using namespace std::chrono_literals;

////////////////////////////////////////////////////////////

SoundGeneratorEx::SoundGeneratorEx(game& game)
    : scene {game}
    , _form0 {std::make_shared<generator_form>(window())}
{
}

SoundGeneratorEx::~SoundGeneratorEx() = default;

void SoundGeneratorEx::on_start()
{
    root_node().Entity = _form0;
    _form0->NewWave.connect([&] {
        if (_waveState == wave_state::Clean) {
            _waveState = wave_state::Dirty;
        }
    });
    _form0->GenPickupCoin->MouseButtonDown.connect([&] { _form0->set_values(_gen1.generate_pickup_coin(clock::now().time_since_epoch().count())); });
    _form0->GenLaserShot->MouseButtonDown.connect([&] { _form0->set_values(_gen1.generate_laser_shoot(clock::now().time_since_epoch().count())); });
    _form0->GenExplosion->MouseButtonDown.connect([&] { _form0->set_values(_gen1.generate_explosion(clock::now().time_since_epoch().count())); });
    _form0->GenPowerup->MouseButtonDown.connect([&] { _form0->set_values(_gen1.generate_powerup(clock::now().time_since_epoch().count())); });
    _form0->GenHitHurt->MouseButtonDown.connect([&] { _form0->set_values(_gen1.generate_hit_hurt(clock::now().time_since_epoch().count())); });
    _form0->GenJump->MouseButtonDown.connect([&] { _form0->set_values(_gen1.generate_jump(clock::now().time_since_epoch().count())); });
    _form0->GenBlipSelect->MouseButtonDown.connect([&] { _form0->set_values(_gen1.generate_blip_select(clock::now().time_since_epoch().count())); });
    _form0->GenRandom->MouseButtonDown.connect([&] { _form0->set_values(_gen1.generate_random(clock::now().time_since_epoch().count())); });
    _form0->Play->MouseButtonDown.connect([&] { create_data(); play_wave(); });
    _form0->Mutate->MouseButtonDown.connect([&] { _form0->set_values(_gen1.mutate_wave(clock::now().time_since_epoch().count(), _wave1)); });
    _form0->FromClipboard->MouseButtonDown.connect([&] {
        object loadFile;
        if (loadFile.parse(locate_service<input::system>().clipboard().get_text(), ".ini")) {
            if (loadFile.try_get(_wave1, "wave")) {
                _form0->set_values(_wave1);
            }
        }
    });
    _form0->ToClipboard->MouseButtonDown.connect([&] {
        object saveFile;
        saveFile["wave"] = _wave1;
        string str {saveFile.str()};
        locate_service<input::system>().clipboard().set_text(str.substr(1, str.size() - 2));
    });
    _form0->Export->MouseButtonDown.connect([&] {
        auto const fileName {[]() {
            for (i32 i {0};; ++i) {
                auto const name {std::format("sound{:02}.wav", i)};
                if (!io::exists(name)) { return name; }
            }
        }()};
        auto       _ = _audioData.save(fileName);
    });
    _form0->Exit->MouseButtonDown.connect([&] {
        parent().pop_current_scene();
    });
    _form0->update(0ms);
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
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);

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
        parent().pop_current_scene();
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
    _sound1 = std::make_shared<sound>(_audioData);
    _sound1->play();
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
    auto audioData {_audioData.data()};
    if (audioData.empty()) { return; }

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

    canvas->set_stroke_width(1);
    canvas->set_stroke_style(colors::DarkBlue);

    canvas->begin_path();
    canvas->move_to(points[0]);
    for (u32 i {1}; i < points.size(); ++i) { canvas->line_to(points[i]); }
    canvas->stroke();
}
