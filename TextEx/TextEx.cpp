// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "TextEx.hpp"

using namespace std::chrono_literals;

TextEx::TextEx(game& game)
    : scene {game}
{
}

TextEx::~TextEx() = default;

void TextEx::on_start()
{
    auto* resGrp {library().get_group("text")};
    auto  fontFam {resGrp->get<font_family>("Poppins")};

    seconds duration {4s};

    {
        auto& text0 {_texts.emplace_back(std::make_unique<text>(fontFam))};
        text0->Text   = "normal text \n"
                        "{COLOR:Red}c{COLOR:Blue}o{COLOR:Yellow}l{COLOR:LightBlue}o{COLOR:Cyan}r{COLOR:Orange}e{COLOR:Blue}d {COLOR:RebeccaPurple}t{COLOR:Green}e{COLOR:Indigo}x{COLOR:LimeGreen}t{COLOR:White}\n"
                        "{ALPHA:1}t{ALPHA:0.9}r{ALPHA:0.8}a{ALPHA:0.7}n{ALPHA:0.6}s{ALPHA:0.5}p{ALPHA:0.4}a{ALPHA:0.3}r{ALPHA:0.2}e{ALPHA:0.1}n{ALPHA:1}t te{ALPHA:0.5}xt";
        text0->Style  = text::style {.Color = colors::White, .Alignment = {}, .KerningEnabled = false, .FontStyle = {}, .FontSize = 32};
        text0->Bounds = {{30, 6}, {500, 500}};
    }

    {
        auto& text0 {_texts.emplace_back(std::make_unique<text>(fontFam))};
        text0->Text   = "{EFFECT:1}The quick brown fox{EFFECT:0} jumps over {EFFECT:2}the lazy dog{EFFECT:0}.\n"
                        "{EFFECT:3}PACK MY BOX{EFFECT:0} with {EFFECT:4}five dozen liquor jugs{EFFECT:0}!\n"
                        "{EFFECT:5}Sphinx of black quartz{EFFECT:0}, judge my vow.\n"
                        "{EFFECT:6}How vexingly quick{EFFECT:0} daft zebras jump!\n"
                        "{EFFECT:7}Jackdaws love{EFFECT:0} my {EFFECT:8}big sphinx{EFFECT:0} of quartz.\n"
                        "{EFFECT:9}The five boxing wizards jump quickly{EFFECT:0}.\n"
                        "{EFFECT:10}Bright vixens jump{EFFECT:0}; dozy fowl quack.\n"
                        "{EFFECT:11}Waltz, nymph{EFFECT:0}, for quick jigs vex Bud.\n\n";
        text0->Style  = text::style {.Color = colors::White, .Alignment = {}, .KerningEnabled = false, .FontStyle = {}, .FontSize = 40};
        text0->Bounds = {{420, 6}, {900, 1200}};

        text0->create_effect(1, duration, effect::typing {});
        text0->create_effect(2, duration, effect::fade_in {3});
        text0->create_effect(3, duration, effect::fade_out {2});
        text0->create_effect(4, duration, effect::blink {.Color0 = colors::Orange, .Color1 = colors::Teal, .Frequency = 5.f});
        text0->create_effect(5, duration, effect::shake {.Intensity = 3.f, .RNG = rng {12345}});
        text0->create_effect(6, duration, effect::wave {.Height = 15.f, .Amplitude = 4.f});
        text0->create_effect(7, duration, effect::size {.WidthStart = 1.0f, .WidthEnd = 0.0f, .HeightStart = 1.0f, .HeightEnd = 0.0f, .Anchor = {.Horizontal = horizontal_alignment::Center, .Vertical = vertical_alignment::Middle}});
        text0->create_effect(8, duration, effect::rotate {.Speed = 5.0f});
        text0->create_effect(9, duration, effect::gradient {color_gradient {colors::White, colors::Green, colors::Black, colors::Orange}.colors()});
        text0->create_effect(10, duration, effect::bounce {.Height = 24.f});
        text0->create_effect(11, duration, effect::jitter {.Intensity = 2.f, .RNG = rng {67890}});
    }

    {
        auto& text0 {_texts.emplace_back(std::make_unique<text>(fontFam))};
        text0->Text   = "{EFFECT:1}Earthquake detected...{EFFECT:0}\n"
                        "{EFFECT:2}Ocean waves rolling in{EFFECT:0}\n\n"
                        "{EFFECT:3}{COLOR:Purple}Balloon inflating{EFFECT:0}\n\n"
                        "{EFFECT:4}{COLOR:Green}Spinning tornado{EFFECT:0}\n";
        text0->Style  = text::style {.Color = colors::White, .Alignment = {}, .KerningEnabled = false, .FontStyle = {}, .FontSize = 32};
        text0->Bounds = {{30, 206}, {500, 1000}};

        text0->create_effect(1, duration, effect::typing {}, effect::shake {.Intensity = 3.f, .RNG = rng {12345}})->Interval = 100ms;
        text0->create_effect(2, duration,
                             effect::blink {.Color0 = colors::Blue, .Color1 = colors::LightBlue, .Frequency = 5.f},
                             effect::wave {.Height = 15.f, .Amplitude = 10.f});
        text0->create_effect(3, duration,
                             effect::size {.WidthStart = 0.0f, .WidthEnd = 1.0f, .HeightStart = 0.0f, .HeightEnd = 1.0f, .Anchor = {.Horizontal = horizontal_alignment::Left, .Vertical = vertical_alignment::Middle}},
                             effect::wave {.Height = 15.f, .Amplitude = 10.f});
        text0->create_effect(4, duration, effect::rotate {2.0f}, effect::wave {.Height = 30.f, .Amplitude = 10.f});
    }
}

void TextEx::on_draw_to(render_target& target, transform const& xform)
{
    target.clear(color::FromRGBA(0x2a2a2aff));

    for (auto& text : _texts) {
        text->draw_to(target, xform);
    }

    _layer0.draw_to(target, xform);
}

void TextEx::on_update(milliseconds deltaTime)
{
    for (auto& text : _texts) {
        text->update(deltaTime);
    }

    _layer0.update(deltaTime);
}

void TextEx::on_fixed_update(milliseconds deltaTime)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);
}

void TextEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    case scan_code::S:
        for (auto& text : _texts) {
            text->start(playback_mode::Looped);
        }
        break;
    default:
        break;
    }
}

void TextEx::on_mouse_motion(mouse::motion_event const& ev)
{
}