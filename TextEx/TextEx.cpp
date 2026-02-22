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
        text0->Text   = "{EFFECT:1}The quick brown fox{EFFECT:0} jumps over {EFFECT:2}the lazy dog{EFFECT:0}.\n"
                        "{EFFECT:3}PACK MY BOX{EFFECT:0} with {EFFECT:4}five dozen liquor jugs{EFFECT:0}!\n"
                        "{EFFECT:5}Sphinx of black quartz{EFFECT:0}, judge my vow.\n"
                        "{EFFECT:6}How vexingly quick{EFFECT:0} daft zebras jump!\n"
                        "{EFFECT:7}Jackdaws love{EFFECT:0} my {EFFECT:8}big sphinx{EFFECT:0} of quartz.\n"
                        "{EFFECT:9}The five boxing wizards jump quickly{EFFECT:0}.\n\n";
        text0->Style  = text::style {.Color = colors::White, .Alignment = {}, .KerningEnabled = false};
        text0->Bounds = {{420, 6}, {900, 1000}};
        auto& effects {text0->Effects};
        effects.create(1, duration, effect::typing {});
        effects.create(2, duration, effect::fade_in {3});
        effects.create(3, duration, effect::fade_out {2});
        effects.create(4, duration, effect::blink {.Color0 = colors::Orange, .Color1 = colors::Teal, .Frequency = 5.f});
        effects.create(5, duration, effect::shake {.Intensity = 3.f, .RNG = rng {12345}});
        effects.create(6, duration, effect::wave {.Height = 15.f, .Amplitude = 4.f});
        effects.create(7, duration, effect::size {.WidthStart = 1.0f, .WidthEnd = 0.0f, .HeightStart = 1.0f, .HeightEnd = 0.0f, .Anchor = {.Horizontal = horizontal_alignment::Centered, .Vertical = vertical_alignment::Middle}});
        effects.create(8, duration, effect::rotate {.Speed = 5.0f});
        effects.create(9, duration, effect::gradient {color_gradient {colors::Orange, colors::DarkGreen, colors::BlueViolet, colors::LightBlue}.colors()});
        effects.start_all(playback_mode::Looped);
    }

    {
        auto& text0 {_texts.emplace_back(std::make_unique<text>(font))};
        text0->Text   = "{EFFECT:1}Earthquake detected...{EFFECT:0}\n"
                        "{EFFECT:2}Ocean waves rolling in{EFFECT:0}\n\n"
                        "{EFFECT:3}{COLOR:Purple}Balloon inflating{EFFECT:0}\n\n"
                        "{EFFECT:4}{COLOR:Green}Spinning tornado{EFFECT:0}\n";
        text0->Bounds = {{30, 206}, {500, 1000}};
        auto& effects {text0->Effects};
        effects.create(1, duration, effect::typing {}, effect::shake {.Intensity = 3.f, .RNG = rng {12345}})->Interval = 100ms;
        effects.create(2, duration,
                       effect::blink {.Color0 = colors::Blue, .Color1 = colors::LightBlue, .Frequency = 5.f},
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
        std::vector<polygon> polys {polygonize_text(*font2, text, true)};

        auto& shapeOutline {_layer0.create_shape<poly_shape>()};
        shapeOutline.Color    = colors::Blue;
        shapeOutline.Material = material::Empty();
        shapeOutline.Polygons = polys;
        shapeOutline.Polygons.mutate([&](auto& polys) { polygons::offset(polys, 5, offset_join::Square); });

        shapeOutline.move_by({10, 650});

        auto& shapeText {_layer0.create_shape<poly_shape>()};
        shapeText.Color    = colors::Green;
        shapeText.Material = material::Empty();
        shapeText.Polygons = polys;
        assert(polygons::check_winding(*shapeText.Polygons));
        shapeText.move_by({10, 650});

        transform xform0;
        xform0.rotate_at(degree_f {45}, shapeText.Polygons->at(1).info().Centroid);
        shapeText.Polygons.mutate([&](auto& poly) { poly[1].apply_transform(xform0); });
        transform xform1;
        xform1.rotate_at(degree_f {45}, shapeText.Polygons->at(4).info().Centroid);
        shapeText.Polygons.mutate([&](auto& poly) { poly[4].apply_transform(xform1); });
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
    default:
        break;
    }
}

void TextEx::on_mouse_motion(mouse::motion_event const& ev)
{
}