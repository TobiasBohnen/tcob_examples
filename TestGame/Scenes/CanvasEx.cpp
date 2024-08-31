﻿// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "CanvasEx.hpp"
#include "tcob/core/Color.hpp"
#include <iomanip>
#include <iostream>

using namespace std;

CanvasEx::CanvasEx(game& game)
    : scene {game}
    , _ninePatchTween(milliseconds {1500}, {0.0f, 1.0f, 1.0f, 0.0f})
{
    _material->Texture = _canvas.get_texture();
}

CanvasEx::~CanvasEx() = default;

void CanvasEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    get_window().Title = "TestGame " + stream.str();
}

void CanvasEx::on_start()
{
    prepare_canvas();
    _ninePatchTween.start(playback_mode::Looped);
}

void CanvasEx::on_update(milliseconds deltaTime)
{
    paint_to_canvas();
    _layer1.update(deltaTime);
    _ninePatchTween.update(deltaTime);
}

void CanvasEx::on_draw_to(render_target& target)
{
    _layer1.draw_to(target);
}

void CanvasEx::on_key_down(keyboard::event& ev)
{
    switch (ev.ScanCode) {
    case scan_code::R: {
        auto _ = get_window().copy_to_image().save("screen01.webp");
    } break;
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    case scan_code::X: {
        auto spr {std::static_pointer_cast<gfx::rect_shape>(_layer1.get_shape_at(0))};
        spr->Bounds = spr->Bounds->with_size({400, 200});
    } break;
    default:
        break;
    }
}

void CanvasEx::on_mouse_motion(mouse::motion_event& ev)
{
}

void CanvasEx::prepare_canvas()
{
    auto* resGrp {get_game().get_library().get_group("res")};

    Image        = resGrp->get<texture>("testing").get_obj();
    ImagePattern = _canvas.create_image_pattern({10, 550 - 256}, {128, 128}, 0, Image, 1);
    NP1          = resGrp->get<texture>("np1").get_obj();

    std::vector<color_stop> colorStops {{0, colors::Red}, {0.25, colors::Gold}, {0.75, colors::Green}, {1, colors::White}};
    color_gradient          colorGradient {colorStops};
    LinearGradient = _canvas.create_linear_gradient({0, 0}, {0, 200}, colorGradient);

    BoxGradient     = _canvas.create_box_gradient({550, 80, 100, 100}, 8, 75, colorGradient);
    RadialGradient0 = _canvas.create_radial_gradient(rect_f {450, 0, 250, 250}.get_center(), 0, 125, colorGradient);
    RadialGradient1 = _canvas.create_radial_gradient(rect_f {550, 480, 100, 100}.get_center(), 0, 125, {0.5f, 1.f}, colorGradient);

    auto sprite1 {_layer1.create_shape<gfx::rect_shape>()};
    sprite1->Material = _material;
    sprite1->Bounds   = {{0.f, 0.f}, {1000.f, 800.f}};
}

void CanvasEx::paint_to_canvas()
{
    _canvas.begin_frame({1000, 800}, 2);

    _canvas.set_fill_style(colors::White);
    _canvas.begin_path();
    _canvas.rect({150, 150, 500, 500});
    _canvas.fill();

    func::catmull_rom cm;
    cm.ControlPoints = {{150, 300},
                        {150, 150},
                        {350, 300},
                        {200, 300},
                        {250, 15}};

    std::vector<point_f> points;
    for (f32 f {0}; f < 1.0f; f += 0.01f) {
        points.push_back(cm(f));
    }
    points.push_back(points.front());

    _canvas.set_stroke_style(colors::Blue);
    _canvas.set_stroke_width(5);
    _canvas.stroke_lines(points);

    for (auto const& pt : cm.ControlPoints) {
        _canvas.begin_path();
        _canvas.set_fill_style(colors::Orange);
        _canvas.circle(pt, 10);
        _canvas.fill();
    }

    _canvas.end_frame();
}
