// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PointCloudEx.hpp"

size_i numPoints {320, 240};
f32    pointSize {5.0f};

PointCloudEx::PointCloudEx(game& game)
    : scene {game}
    , _quadtree {{{-100, -100}, {320 * 5.f + 500, 240 * 5.f + 500}}}
{
}

PointCloudEx::~PointCloudEx() = default;

void PointCloudEx::on_start()
{
    _cloud0 = std::make_shared<point_cloud>(numPoints.Width * numPoints.Height);
    i32 i {0};
    for (f32 y = 0; y < pointSize * numPoints.Height; y += pointSize) {
        for (f32 x = 0; x < pointSize * numPoints.Width; x += pointSize) {
            auto& v     = _cloud0->create_point();
            v.Position  = {(x + (pointSize / 2)) + _rand(0, 150), (y + (pointSize / 2)) + _rand(0, 150)};
            v.Color     = {255, 255, 255, 255};
            v.TexCoords = {x / (pointSize * numPoints.Width),
                           y / (pointSize * numPoints.Height),
                           0};
            if (_quadtree.get_bounds().contains(v.Position)) {
                _quadtree.add({i++, &v});
            }
        }
    }

    _cloud0->Material            = material::Empty();
    _cloud0->Material->Color     = colors::Blue;
    _cloud0->Material->PointSize = pointSize;
}

void PointCloudEx::on_draw_to(render_target& target)
{
    _cloud0->draw_to(target);
}

void PointCloudEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    get_window().Title = "TestGame " + stream.str();
}

void PointCloudEx::on_update(milliseconds deltaTime)
{
    _cloud0->update(deltaTime);
}

void PointCloudEx::on_key_down(keyboard::event const& ev)
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
    rect_f const queryRect {_cam.screen_to_world(ev.Position) - point_f {25.f, 25.f}, {50.f, 50.f}};

    auto points {_quadtree.query(queryRect.as_intersection_with(_quadtree.get_bounds()))};
    for (auto& p : points) {
        auto const newPos {p.Vertex->Position + point_f {static_cast<f32>(ev.RelativeMotion.X) * 10, static_cast<f32>(ev.RelativeMotion.Y) * 10}};
        if (_quadtree.get_bounds().contains(rect_f {newPos, size_f::One})) {
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
