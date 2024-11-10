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

    seconds duration {4s};

    {
        auto& text0 {_texts.emplace_back(std::make_unique<text>(font))};
        text0->Text   = "normal text \n"
                        "{COLOR:Red}c{COLOR:Blue}o{COLOR:Yellow}l{COLOR:LightBlue}o{COLOR:Cyan}r{COLOR:Orange}e{COLOR:Blue}d {COLOR:RebeccaPurple}t{COLOR:Green}e{COLOR:Indigo}x{COLOR:LimeGreen}t{COLOR:White}\n"
                        "{ALPHA:1}t{ALPHA:0.9}r{ALPHA:0.8}a{ALPHA:0.7}n{ALPHA:0.6}s{ALPHA:0.5}p{ALPHA:0.4}a{ALPHA:0.3}r{ALPHA:0.2}e{ALPHA:0.1}n{ALPHA:1}t te{ALPHA:0.5}xt";
        text0->Bounds = {{30, 6}, {500, 500}};
    }

    {
        auto& text0 {_texts.emplace_back(std::make_unique<text>(font))};
        text0->Text  = "{EFFECT:1} Typing\n"
                       "{EFFECT:2} FadeIn\n"
                       "{EFFECT:3} FadeOut\n"
                       "{EFFECT:4} Blink\n"
                       "{EFFECT:9} Gradient\n"
                       "{EFFECT:7} Shrink\n"
                       "{EFFECT:8} Rotate\n"
                       "{EFFECT:5} Shake\n"
                       "{EFFECT:6} WaveWave\n\n";
        text0->Style = text::style {.Color = colors::White, .Alignment = {}, .KerningEnabled = false};

        text0->Bounds = {{420, 6}, {500, 1000}};

        auto& effects {text0->Effects};
        effects.create(1, duration, effect::typing {});
        effects.create(2, duration, effect::fade_in {3});
        effects.create(3, duration, effect::fade_out {2});
        effects.create(4, duration, effect::blink {colors::Orange, colors::Teal, 5.f});
        effects.create(5, duration, effect::shake {3.f, rng {12345}})->Interval = 25ms;
        effects.create(6, duration, effect::wave {30.f, 4.f});
        effects.create(7, duration, effect::size {1.0f, 0.0f, 1.0f, 0.0f, {horizontal_alignment::Centered, vertical_alignment::Middle}});
        effects.create(8, duration, effect::rotate {1.0f});
        effects.create(9, duration, effect::gradient {color_gradient {colors::Orange, colors::LightBlue}.get_colors()});

        effects.start_all(playback_mode::Looped);
    }

    {
        auto& text0 {_texts.emplace_back(std::make_unique<text>(font))};
        text0->Text = "{EFFECT:1} TypingShake\n"
                      "{EFFECT:2} BlinkWave\n\n"
                      "{EFFECT:3} GrowWave\n\n"
                      "{EFFECT:4} RotateWave\n";

        text0->Bounds = {{30, 206}, {500, 1000}};

        auto& effects {text0->Effects};
        effects.create(1, duration, effect::typing {}, effect::shake {.Intensity = 3.f, .RNG = rng {12345}})->Interval = 100ms;
        effects.create(2, duration,
                       effect::blink {.Color0 = colors::Red, .Color1 = colors::Green, .Frequency = 5.f},
                       effect::wave {.Height = 15.f, .Amplitude = 10.f});
        effects.create(3, duration,
                       effect::size {.WidthStart = 0.0f, .WidthEnd = 1.0f, .HeightStart = 0.0f, .HeightEnd = 1.0f, .Anchor = {.Horizontal = horizontal_alignment::Left, .Vertical = vertical_alignment::Middle}},
                       effect::wave {.Height = 15.f, .Amplitude = 10.f});
        effects.create(4, duration, effect::rotate {2.0f}, effect::wave {.Height = 30.f, .Amplitude = 10.f});

        effects.start_all(playback_mode::Looped);
    }

    {
        // Vertex text

        auto                 font2 {fontFam->get_font({.IsItalic = false, .Weight = font::weight::ExtraBold}, 128)};
        string               text {"Vertex"};
        std::vector<polygon> polys {font2->polygonize_text(text, true)};

        auto shapeOutline {std::make_shared<poly_shape>()};
        shapeOutline->Color    = colors::Blue;
        shapeOutline->Material = material::Empty();
        shapeOutline->Polygons = polys;
        polygons::offset(*shapeOutline->Polygons, 5, offset_join::Square);
        shapeOutline->move_by({10, 650});
        _layer0.add_shape(shapeOutline);

        auto shapeText {std::make_shared<poly_shape>()};
        shapeText->Color    = colors::Green;
        shapeText->Material = material::Empty();
        shapeText->Polygons = polys;
        assert(polygons::check_winding(shapeText->Polygons()));
        shapeText->move_by({10, 650});
        _layer0.add_shape(shapeText);

        transform xform0;
        xform0.rotate_at(degree_f {45}, shapeText->Polygons->at(1).get_info().Centroid);
        (*shapeText->Polygons).at(1).apply_transform(xform0);
        transform xform1;
        xform1.rotate_at(degree_f {45}, shapeText->Polygons->at(4).get_info().Centroid);
        (*shapeText->Polygons).at(4).apply_transform(xform1);
    }
}

void TextEx::on_draw_to(render_target& target)
{
    target.clear(color::FromRGBA(0x2a2a2aff));

    for (auto& text : _texts) {
        text->draw_to(target);
    }

    _layer0.draw_to(target);
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
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    get_window().Title = "TestGame " + stream.str();
}

void TextEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    default:
        break;
    }
}

void TextEx::on_mouse_motion(mouse::motion_event const& ev)
{
}