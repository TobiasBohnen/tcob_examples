// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "TextEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

TextEx::TextEx(game& game)
    : scene(game)
{
}

TextEx::~TextEx() = default;

void TextEx::on_start()
{
    auto* resGrp {get_game().get_library().get_group("text")};
    auto  fontFam {resGrp->get<font_family>("Poppins")};
    auto  font {fontFam->get_font({}, 32)};

    auto& text0 {_texts.emplace_back(std::make_unique<text>(font))};
    text0->Text   = "normal text \n"
                    "{COLOR:Red}c{COLOR:Blue}o{COLOR:Yellow}l{COLOR:LightBlue}o{COLOR:Cyan}r{COLOR:Orange}e{COLOR:Blue}d {COLOR:RebeccaPurple}t{COLOR:Green}e{COLOR:Indigo}x{COLOR:LimeGreen}t{COLOR:White}\n"
                    "{ALPHA:1}t{ALPHA:0.9}r{ALPHA:0.8}a{ALPHA:0.7}n{ALPHA:0.6}s{ALPHA:0.5}p{ALPHA:0.4}a{ALPHA:0.3}r{ALPHA:0.2}e{ALPHA:0.1}n{ALPHA:1}t te{ALPHA:0.5}xt";
    text0->Bounds = {{30, 6}, {500, 500}};

    auto& text4 {_texts.emplace_back(std::make_unique<text>(font))};
    text4->Text = "{EFFECT:1} Typing\n"
                  "{EFFECT:2} FadeIn\n"
                  "{EFFECT:3} FadeOut\n"
                  "{EFFECT:4} Blink\n"
                  "{EFFECT:5} Shake\n"
                  "{EFFECT:6} {COLOR:blue}Wave{COLOR:lightblue}Wave\n\n"
                  "{EFFECT:7} TypingShake\n"
                  "{EFFECT:8} BlinkWave\n";

    text4->Bounds = {{420, 6}, {500, 1000}};

    auto& effects {text4->Effects};
    effects.create<effect::typing>(1, 3s);
    effects.create<effect::fade_in>(2, 3s);
    effects.create<effect::fade_out>(3, 3s);
    effects.create<effect::blink>(4, 3s, colors::Orange, colors::Teal, 5.f);

    auto eff5 {effects.create<effect::shake>(5, 3s, 3.f, 1.f, rng {12345})};
    eff5->Interval = 25ms;

    effects.create<effect::wave>(6, 3s, 30.f, 4.f);

    auto eff7 {effects.create(7, 3s, effect::typing {}, effect::shake {3.f, 1.f, rng {12345}})};
    eff7->Interval = 100ms;

    effects.create(8, 3s, effect::blink {colors::Red, colors::Green, 5.f}, effect::wave {15.f, 10.f});

    effects.start_all(playback_style::Looped);
}

void TextEx::on_draw_to(render_target& target)
{
    target.clear(color::FromRGBA(0x2a2a2aff));

    for (auto& text : _texts) {
        text->draw_to(target);
    }
}

void TextEx::on_update(milliseconds deltaTime)
{
    for (auto& text : _texts) {
        text->update(deltaTime);
    }
}

void TextEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    get_window().Title = "TestGame " + stream.str();
}

void TextEx::on_key_down(keyboard::event& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    default:
        break;
    }
}

void TextEx::on_mouse_motion(mouse::motion_event& ev)
{
}