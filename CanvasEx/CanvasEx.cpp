// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "CanvasEx.hpp"
#include "tcob/core/Color.hpp"
#include <iomanip>
#include <iostream>

#include "../_common/Trim.hpp"

CanvasEx::CanvasEx(game& game)
    : scene {game}
{
    font_family::SingleFont(_font, trim_ttf);

    _modes = {
        &CanvasEx::canvas_gradient,
        &CanvasEx::canvas_clip,
        &CanvasEx::canvas_text,
        &CanvasEx::canvas_ring,
        &CanvasEx::canvas_ray_cast,
        &CanvasEx::canvas_path2d,
        &CanvasEx::canvas_fancy_lines,
    };
}

CanvasEx::~CanvasEx() = default;

void CanvasEx::on_fixed_update(milliseconds /* deltaTime */)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str() + "| press 'm' to cycle example";
}

void CanvasEx::on_start()
{
}

void CanvasEx::on_update(milliseconds /* deltaTime */)
{
    (this->*_modes[_mode])();
}

void CanvasEx::on_draw_to(render_target& target)
{
    _renderer.add_layer(0);
    _renderer.set_bounds({point_f::Zero, size_f {*window().Size}});
    _renderer.render_to_target(target);
}

void CanvasEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::M: {
        _mode  = (_mode + 1) % _modes.size();
        _value = 45;
    } break;
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void CanvasEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _mp = point_f {ev.Position};
}

void CanvasEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _value += (ev.Scroll.Y * 5);
}

void CanvasEx::canvas_ring()
{
    _canvas.begin_frame(window().Size, 1);

    f32 outerRadius {200};
    f32 innerRadius {_value};
    i32 segments {10};
    f32 gapSize {5};

    f32 const ogap {static_cast<f32>(2 * std::asin(gapSize / (2 * outerRadius)))};
    f32 const igap {static_cast<f32>(2 * std::asin(gapSize / (2 * innerRadius)))};

    for (isize i {0}; i < segments; i++) {
        radian_f const startAngle {radian_f {(i * TAU_F) / segments}};
        radian_f const outerEndAngle {static_cast<f32>((((i + 1) * TAU_F) / segments) - ogap)};
        radian_f const innerEndAngle {static_cast<f32>((((i + 1) * TAU_F) / segments) - igap)};

        _canvas.begin_path();
        _canvas.arc(_mp, outerRadius, startAngle, outerEndAngle, winding::CW);
        _canvas.arc(_mp, innerRadius, innerEndAngle, startAngle, winding::CCW); // NOLINT(readability-suspicious-call-argument)
        _canvas.close_path();

        // Fill the segment
        _canvas.set_fill_style(color::FromHSLA({.Hue = degree_f {i * 360.f / segments}, .Saturation = 0.8f, .X = 0.5f}));
        _canvas.fill();

        // Draw outline
        _canvas.set_stroke_style(colors::Black);
        _canvas.set_stroke_width(2);
        _canvas.stroke();
    }

    _canvas.end_frame();
}

void CanvasEx::canvas_ray_cast()
{
    _canvas.begin_frame(window().Size, 1);

    std::vector<ray::result> points;
    f32                      size {75};
    rect_f                   rect {{_mp.X - (size / 2), _mp.Y - (size / 2)}, {size - 10, size - 20}};

    transform xform;
    xform.rotate_at(degree_f {_value}, rect.center());

    _canvas.set_stroke_style(colors::Red);
    _canvas.set_stroke_width(5);

    auto castRay {
        [&](auto&& ray) {
            auto points0 {ray.intersect_circle(_mp, size)};
            points.insert(points.end(), points0.begin(), points0.end());
            auto points1 {ray.intersect_rect(rect, xform)};
            points.insert(points.end(), points1.begin(), points1.end());

            _canvas.begin_path();
            _canvas.move_to(ray.get_point(0));
            _canvas.line_to(ray.get_point(1000));
            _canvas.stroke();
        }};
    castRay(ray {{0, 0}, degree_f {135}});
    castRay(ray {{600, 0}, degree_f {225}});
    castRay(ray {{300, 0}, degree_f {180}});
    castRay(ray {{0, 300}, degree_f {90}});

    // draw circle
    _canvas.set_stroke_style(colors::Green);
    _canvas.set_stroke_width(5);
    _canvas.begin_path();
    _canvas.circle(_mp, size);
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
    _canvas.begin_frame(window().Size, 1);

    auto linearGradient0 {_canvas.create_linear_gradient(
        {0, 0}, {0, 200},
        color_gradient {{0, colors::Red}, {0.25, colors::Gold}, {0.75, colors::Green}, {1, colors::White}})};

    _canvas.set_fill_style(linearGradient0);
    _canvas.begin_path();
    _canvas.rect({{0, 0}, {200, 200}});
    _canvas.fill();

    auto linearGradient1 {_canvas.create_linear_gradient(
        {250, 0}, {450, 0},
        color_gradient {{0, colors::Green}, {0.25, colors::Blue}, {0.75, colors::Orange}, {1, colors::SlateBlue}})};

    _canvas.set_global_alpha(0.5f);
    _canvas.set_fill_style(linearGradient1);
    _canvas.begin_path();
    _canvas.rect({{250, 0}, {200, 200}});
    _canvas.fill();

    auto linearGradient2 {_canvas.create_linear_gradient(
        {500, 0}, {0, 700},
        color_gradient {colors::Green, colors::Green})};

    _canvas.set_global_alpha(1.0f);
    _canvas.set_fill_style(linearGradient2);
    _canvas.begin_path();
    _canvas.rect({{500, 0}, {200, 200}});
    _canvas.fill();

    auto conicalGradient {_canvas.create_conic_gradient(
        {350, 350},
        color_gradient {
            {0, colors::Green},
            {0.2499, colors::Green},
            {0.25, colors::Red},
            {0.4999, colors::Red},
            {0.50, colors::Blue},
            {0.7499, colors::Blue},
            {0.75, colors::Orange},
            {1, colors::Orange},
        })};

    _canvas.set_fill_style(conicalGradient);
    _canvas.begin_path();
    _canvas.circle({350, 350}, 100);
    _canvas.fill();

    _canvas.end_frame();
}

void CanvasEx::canvas_path2d()
{
    _canvas.begin_frame(window().Size, 1);
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
    _canvas.begin_frame(window().Size, 1);

    _canvas.set_fill_style(colors::Green);
    _canvas.set_stroke_style(colors::Black);
    _canvas.set_stroke_width(8);

    _canvas.set_line_dash(std::array {_value, _value / 2});
    _canvas.set_line_cap(line_cap::Round);
    _canvas.set_dash_offset(_value);

    _canvas.begin_path();
    _canvas.move_to({350, 50});
    _canvas.line_to({400, 250});
    _canvas.fill();
    _canvas.stroke();
    /*
   _canvas.begin_path();
    _canvas.circle(_mp, _value);
    _canvas.clip();
*/

    _canvas.begin_path();
    _canvas.rounded_rect({20, 30, 300, 100}, 50);
    _canvas.fill();
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.rect({20, 150, 300, 100});
    _canvas.fill();
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.circle({600, 250}, 85);
    _canvas.fill();
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.ellipse({700, 250}, 50, 85);
    _canvas.fill();
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.triangle({800, 20}, {650, 120}, {850, 110});
    _canvas.fill();
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.arc({800, 600}, 150, degree_f {25}, degree_f {280}, winding::CW);
    _canvas.fill();
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.move_to({200, 550});
    _canvas.quad_bezier_to({50, 650}, {350, 850});
    _canvas.fill();
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.move_to({350, 450});
    _canvas.cubic_bezier_to({600, 500}, {600, 550}, {500, 750});
    _canvas.fill();
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.move_to({1280, 190});
    _canvas.arc_to({1280, 430}, {1010, 430}, 430);
    _canvas.line_to({1210, 430});
    _canvas.fill();
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.set_fill_style(colors::Red);
    _canvas.circle({1280, 190}, 25);
    _canvas.fill();
    _canvas.begin_path();
    _canvas.set_fill_style(colors::Blue);
    _canvas.circle({1280, 430}, 25);
    _canvas.fill();
    _canvas.begin_path();
    _canvas.set_fill_style(colors::Orange);
    _canvas.circle({1010, 430}, 25);
    _canvas.fill();
    _canvas.begin_path();
    _canvas.set_fill_style(colors::Black);
    _canvas.circle({1210, 430}, 25);
    _canvas.fill();

    /*
    _canvas.set_stroke_style(colors::Green);
    _canvas.set_stroke_width(15);
    _canvas.begin_path();
    _canvas.move_to({350, 750});
    _canvas.wavy_line_to(_mp, 10, 0.5f);
    _canvas.stroke();

    _canvas.set_stroke_style(colors::Black);
    _canvas.begin_path();
    _canvas.move_to({350, 750});
    _canvas.line_to(_mp);
    //_canvas.line_to({850, 350});
    _canvas.stroke();
*/

    _canvas.end_frame();
}

void CanvasEx::canvas_text()
{
    _canvas.begin_frame(window().Size, 1);

    _canvas.set_font(_font.get_font({}, 128).ptr());

    _canvas.set_fill_style(colors::White);
    _canvas.fill_text({10, 300, 500, 500}, "Lorem ipsum dolor sit amet");

    _canvas.set_fill_style(colors::Black);
    _canvas.draw_text({10, 400, 3000, 1000}, "Lorem ipsum dolor sit amet");

    _canvas.set_stroke_style(colors::Black);
    _canvas.set_stroke_width(2);
    _canvas.stroke_text({10, 500, 500, 500}, "Lorem ipsum dolor sit amet");

    _canvas.end_frame();
}

void CanvasEx::canvas_clip()
{
    _canvas.begin_frame(window().Size, 1);

    _canvas.set_fill_style(colors::Green);
    _canvas.set_stroke_style(colors::BurlyWood);
    _canvas.set_stroke_width(6);

    _canvas.begin_path();
    _canvas.rect({30, 30, 300, 400});
    _canvas.stroke();

    _canvas.begin_path();
    _canvas.circle({30, 30}, 150);
    _canvas.clip();

    _canvas.begin_path();
    _canvas.rect({30, 30, 300, 400});
    _canvas.fill();

    _canvas.begin_path();
    _canvas.circle({330, 430}, 150);
    _canvas.clip();

    _canvas.begin_path();
    _canvas.rect({30, 30, 300, 400});
    _canvas.fill();

    _canvas.set_font(_font.get_font({}, 48).ptr());
    _canvas.set_fill_style(colors::Black);
    _canvas.draw_text({10, 350, 3000, 1000}, "Lorem ipsum dolor sit amet");

    _canvas.begin_path();
    _canvas.rect({10, 350, 1000, 100});
    _canvas.stroke();

    _canvas.end_frame();
}
