// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PointCloudEx.hpp"

size_i numPoints {512, 512};
size_i frameSize {256, 256};
f32    pointSize {5.0f};

PointCloudEx::PointCloudEx(game& game)
    : scene {game}
    , _quadtree {{{-100, -100}, {numPoints.Width * pointSize * 1.25f, numPoints.Height * pointSize * 1.25f}}}
{
    _canvas.begin_frame(frameSize, 1);

    // Background rectangle
    i32 const cellSize {32};
    for (i32 y {0}; y < frameSize.Height; y += cellSize) {
        for (i32 x {0}; x < frameSize.Width; x += cellSize) {
            _canvas.begin_path();
            _canvas.rect({static_cast<f32>(x), static_cast<f32>(y), cellSize, cellSize});
            _canvas.set_fill_style(((x / cellSize) + (y / cellSize)) % 2 == 0 ? colors::GoldenRod : colors::DarkTurquoise);
            _canvas.fill();
        }
    }

    // Outer circle with stroke
    _canvas.begin_path();
    _canvas.circle({40, 40}, 50);
    _canvas.set_stroke_style(colors::Black);
    _canvas.set_stroke_width(4);
    _canvas.stroke();

    // Inner circle with fill
    _canvas.begin_path();
    _canvas.circle({40, 40}, 30);
    _canvas.set_fill_style(colors::White);
    _canvas.fill();

    // Triangle
    _canvas.begin_path();
    _canvas.triangle({150, 50}, {200, 150}, {100, 150});
    _canvas.set_fill_style(colors::DarkRed);
    _canvas.fill();
    _canvas.set_stroke_style(colors::Black);
    _canvas.stroke();

    // Additional rectangle
    _canvas.begin_path();
    _canvas.rect({180, 180, 50, 50});
    _canvas.set_fill_style(colors::ForestGreen);
    _canvas.fill();
    _canvas.set_stroke_style(colors::Black);
    _canvas.stroke();

    // Line
    _canvas.begin_path();
    _canvas.move_to({0, 256});
    _canvas.line_to({256, 0});
    _canvas.set_stroke_style(colors::DarkSlateGray);
    _canvas.set_stroke_width(2);
    _canvas.stroke();

    // Small circle
    _canvas.begin_path();
    _canvas.circle({128, 128}, 20);
    _canvas.set_fill_style(colors::Blue);
    _canvas.fill();
    _canvas.set_stroke_style(colors::Black);
    _canvas.stroke();

    _canvas.end_frame();
}

PointCloudEx::~PointCloudEx() = default;

void PointCloudEx::on_start()
{
    _cloud0 = std::make_shared<point_cloud>(numPoints.Width * numPoints.Height);
    i32 i {0};
    for (f32 y {0}; y < pointSize * numPoints.Height; y += pointSize) {
        for (f32 x {0}; x < pointSize * numPoints.Width; x += pointSize) {
            auto& v {_cloud0->create_point()};
            v.Position  = {(x + (pointSize / 2)), (y + (pointSize / 2))};
            v.Color     = {255, 255, 255, 255};
            v.TexCoords = {.U     = x / (pointSize * numPoints.Width),
                           .V     = y / (pointSize * numPoints.Height),
                           .Level = 0};
            _quadtree.add({.ID = i++, .Vertex = &v});
        }
    }

    _cloud0->Material                         = _mat;
    _cloud0->Material->first_pass().Texture   = _canvas.get_texture(0);
    _cloud0->Material->first_pass().PointSize = pointSize;
}

void PointCloudEx::on_draw_to(render_target& target)
{
    _cloud0->draw_to(target);
}

void PointCloudEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void PointCloudEx::on_update(milliseconds deltaTime)
{
}

void PointCloudEx::on_key_down(keyboard::event const& ev)
{
    _cam.on_key_down(ev);
    switch (ev.ScanCode) { // NOLINT
    case scan_code::R: {
        auto _ = window().copy_to_image().save("screen01.webp");
    } break;
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void PointCloudEx::on_mouse_button_down(mouse::button_event const& ev)
{
    _cam.on_mouse_button_down(ev);
}

void PointCloudEx::on_mouse_button_up(mouse::button_event const& ev)
{
    _cam.on_mouse_button_up(ev);
}

void PointCloudEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _cam.on_mouse_motion(ev);
    if (locate_service<input::system>().mouse().is_button_down(mouse::button::Right)) { return; }

    rect_f const queryRect {_cam.screen_to_world(ev.Position) - point_f {25.f, 25.f}, {50.f, 50.f}};
    auto         points {_quadtree.query(queryRect.as_intersection_with(_quadtree.bounds()))};
    for (auto& p : points) {
        auto const dist {p.Vertex->Position.distance_to(queryRect.center())};
        if (dist > 25) { continue; }

        auto const newPos {p.Vertex->Position + point_f {static_cast<f32>(ev.RelativeMotion.X * (25 - dist + 1)), static_cast<f32>(ev.RelativeMotion.Y * (25 - dist + 1))}};
        if (_quadtree.contains(rect_f {newPos, size_f::One})) {
            auto oldVertex {*p.Vertex};
            p.Vertex->Position = newPos;
            _quadtree.replace({p.ID, &oldVertex}, p);
        }
    }
}

void PointCloudEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _cam.on_mouse_wheel(ev);
}
