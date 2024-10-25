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
    canvas_ray_cast();
    // canvas_gradient();
    // canvas_path2d();
    // canvas_fancy_lines();
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

    auto linearGradient0 = _canvas.create_linear_gradient(
        {0, 0}, {0, 200},
        color_gradient {{0, colors::Red}, {0.25, colors::Gold}, {0.75, colors::Green}, {1, colors::White}});

    _canvas.set_fill_style(linearGradient0);
    _canvas.begin_path();
    _canvas.rect({{0, 0}, {200, 200}});
    _canvas.fill();

    auto linearGradient1 = _canvas.create_linear_gradient(
        {250, 0}, {450, 0},
        color_gradient {{0, colors::Green}, {0.25, colors::Blue}, {0.75, colors::Orange}, {1, colors::SlateBlue}});

    _canvas.set_global_alpha(0.5f);
    _canvas.set_fill_style(linearGradient1);
    _canvas.begin_path();
    _canvas.rect({{250, 0}, {200, 200}});
    _canvas.fill();

    auto linearGradient2 = _canvas.create_linear_gradient(
        {500, 0}, {0, 700},
        color_gradient {colors::Green, colors::Green});

    _canvas.set_global_alpha(1.0f);
    _canvas.set_fill_style(linearGradient2);
    _canvas.begin_path();
    _canvas.rect({{500, 0}, {200, 200}});
    _canvas.fill();

    _canvas.end_frame();
}

void CanvasEx::canvas_path2d()
{
    _canvas.begin_frame(get_window().Size(), 1);
    _canvas.set_fill_style(colors::Green);
    _canvas.path_2d(*path2d::Parse("M 80 80 A 45 45, 0, 0, 0, 125 125 L 125 80 Z"));
    _canvas.fill();

    _canvas.set_fill_style(colors::Red);
    _canvas.path_2d(*path2d::Parse("M 230 80 A 45 45, 0, 1, 0, 275 125 L 275 80 Z"));
    _canvas.fill();

    _canvas.set_fill_style(colors::Purple);
    _canvas.path_2d(*path2d::Parse("M 80 230 A 45 45, 0, 0, 1, 125 275 L 125 230 Z"));
    _canvas.fill();

    _canvas.set_fill_style(colors::Blue);
    _canvas.path_2d(*path2d::Parse("M 230 230 A 45 45, 0, 1, 1, 275 275 L 275 230 Z"));
    _canvas.fill();

    _canvas.end_frame();
}

void CanvasEx::canvas_fancy_lines()
{
    _canvas.begin_frame(get_window().Size(), 1);

    _canvas.set_stroke_style(colors::Red);
    _canvas.set_stroke_width(10);
    _canvas.begin_path();

    _canvas.move_to({0, 100});
    _canvas.dotted_line_to({250, 50}, 5, 5);
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.move_to({350, 50});
    _canvas.dashed_line_to({400, 250}, 5);
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.dashed_circle({400, 250}, 50, 20);
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.move_to({200, 550});
    _canvas.dashed_quad_bezier_to({50, 650}, {350, 850}, 20);
    _canvas.stroke();

    _canvas.set_stroke_style(colors::Green);
    _canvas.set_stroke_width(5);
    _canvas.begin_path();
    _canvas.move_to({350, 750});
    _canvas.wavy_line_to({850, 350}, 10, 0.5f);
    _canvas.stroke();

    _canvas.end_frame();
}
