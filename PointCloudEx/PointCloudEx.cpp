// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PointCloudEx.hpp"

PointCloudEx::PointCloudEx(game& game)
    : scene {game}
{
}

PointCloudEx::~PointCloudEx() = default;

size_i numPoints {320, 240};
f32    pointSize {5.0f};

void PointCloudEx::on_start()
{
    _cloud0 = std::make_shared<point_cloud>(numPoints.Width * numPoints.Height);

    for (f32 y = 0; y < pointSize * numPoints.Height; y += pointSize) {
        for (f32 x = 0; x < pointSize * numPoints.Width; x += pointSize) {
            auto& v     = _cloud0->create_point();
            v.Position  = {(x + (pointSize / 2)) + _rand(0, 150), (y + (pointSize / 2)) + _rand(0, 150)};
            v.Color     = {255, 255, 255, 255};
            v.TexCoords = {x / (pointSize * numPoints.Width),
                           y / (pointSize * numPoints.Height),
                           0};
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

    auto& p {_cloud0->get_point_at(_rand(0, numPoints.Width * numPoints.Height - 1))};
    p.Position.X += _rand(0, 10);
    p.Position.Y += _rand(0, 10);
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
}

void PointCloudEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _cam.on_mouse_wheel(ev);
}
