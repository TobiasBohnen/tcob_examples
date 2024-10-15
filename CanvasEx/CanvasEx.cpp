// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "CanvasEx.hpp"
#include "tcob/core/Color.hpp"
#include <iomanip>
#include <iostream>

CanvasEx::CanvasEx(game& game)
    : scene {game}
{
}

CanvasEx::~CanvasEx() = default;

void CanvasEx::on_fixed_update(milliseconds /* deltaTime */)
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
}

void CanvasEx::on_update(milliseconds /* deltaTime */)
{
    //  canvas_ray_cast();
    canvas_gradient();
}

void CanvasEx::on_draw_to(render_target& target)
{
    _renderer.set_layer(0);
    _renderer.set_bounds({point_f::Zero, size_f {get_window().Size()}});
    _renderer.render_to_target(target);
}

void CanvasEx::on_key_down(keyboard::event const& ev)
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

void CanvasEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _center = point_f {ev.Position};
}

void CanvasEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _rotation += (ev.Scroll.Y * 10);
}

void CanvasEx::canvas_ray_cast()
{
    _canvas.begin_frame(get_window().Size(), 1);

    std::vector<ray::result> points;
    f32                      size {75};
    rect_f                   rect {{_center.X - (size / 2), _center.Y - (size / 2)}, {size - 10, size - 20}};

    transform xform;
    xform.rotate_at(_rotation, rect.get_center());

    _canvas.set_stroke_style(colors::Red);
    _canvas.set_stroke_width(5);

    auto castRay {
        [&](auto&& ray) {
            auto points0 {ray.intersect_circle(_center, size)};
            points.insert(points.end(), points0.begin(), points0.end());
            auto points1 {ray.intersect_rect(rect, xform)};
            points.insert(points.end(), points1.begin(), points1.end());

            _canvas.begin_path();
            _canvas.move_to(ray.get_point(0));
            _canvas.line_to(ray.get_point(1000));
            _canvas.stroke();
        }};
    castRay(ray {{.Origin = point_f {0, 0}, .Direction = degree_f {135}}});
    castRay(ray {{.Origin = point_f {600, 0}, .Direction = degree_f {225}}});
    castRay(ray {{.Origin = point_f {300, 0}, .Direction = degree_f {180}}});
    castRay(ray {{.Origin = point_f {0, 300}, .Direction = degree_f {90}}});

    // draw circle
    _canvas.set_stroke_style(colors::Green);
    _canvas.set_stroke_width(5);
    _canvas.begin_path();
    _canvas.circle(_center, size);
    _canvas.stroke();

    // draw rect
    _canvas.set_stroke_style(colors::Blue);
    _canvas.set_stroke_width(5);
    _canvas.set_transform(xform);
    _canvas.begin_path();
    _canvas.rect(rect);
    _canvas.stroke();
    _canvas.reset_transform();

    // draw intersections
    _canvas.set_stroke_width(2);
    _canvas.set_stroke_style(colors::Yellow);
    _canvas.begin_path();
    for (auto p : points) { _canvas.circle(p.Point, 2); }
    _canvas.stroke();

    _canvas.end_frame();
}

void CanvasEx::canvas_gradient()
{
    _canvas.begin_frame(get_window().Size(), 1);

    std::vector<color_stop> static colorStops0 {{0, colors::Red}, {0.25, colors::Gold}, {0.75, colors::Green}, {1, colors::White}};
    color_gradient static colorGradient0 {colorStops0};
    auto LinearGradient0 = _canvas.create_linear_gradient({0, 0}, {0, 200}, colorGradient0);

    std::vector<color_stop> static colorStops1 {{0, colors::Green}, {0.25, colors::Blue}, {0.75, colors::Orange}, {1, colors::SlateBlue}};
    color_gradient static colorGradient1 {colorStops1};
    auto LinearGradient1 = _canvas.create_linear_gradient({0, 0}, {0, 200}, colorGradient1);

    auto static BoxGradient    = _canvas.create_box_gradient({550, 80, 100, 100}, 8, 75, colorGradient0);
    auto static RadialGradient = _canvas.create_radial_gradient(rect_f {450, 0, 250, 250}.get_center(), 0, 125, colorGradient0);

    _canvas.set_fill_style(LinearGradient0);
    _canvas.begin_path();
    _canvas.rect({{0, 0}, {200, 200}});
    _canvas.fill();

    _canvas.set_global_alpha(0.5f);
    _canvas.set_fill_style(LinearGradient1);
    _canvas.begin_path();
    _canvas.rect({{250, 0}, {200, 200}});
    _canvas.fill();

    _canvas.end_frame();
}
