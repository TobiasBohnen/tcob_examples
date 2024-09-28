// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "RayEx.hpp"

#include <iomanip>
#include <iostream>

RayEx::RayEx(game& game)
    : scene {game}
{
}

RayEx::~RayEx() = default;

void RayEx::on_fixed_update(milliseconds /* deltaTime */)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    get_window().Title = "TestGame " + stream.str();
}

void RayEx::on_start()
{
    _tween0.start(playback_mode::AlternatedLooped);
    _tween0.Value.Changed.connect([&] { _dirty = true; });

    _tween1.start(playback_mode::AlternatedLooped);
    _tween1.Value.Changed.connect([&] { _dirty = true; });
}

void RayEx::on_update(milliseconds deltaTime)
{
    _tween0.update(deltaTime);
    _tween1.update(deltaTime);

    if (!_dirty) { return; }
    _dirty = false;

    _batch.clear();

    // draw shape
    switch (_mode) {
    case 0: {
        auto polyShape {_batch.create_shape<gfx::poly_shape>()};
        polyShape->Color    = colors::Blue;
        polyShape->Material = _emptyMat;
        auto const [x, y] {_center - point_f {150, 50}};
        polyShape->Polygon  = {{x, y}, {80 + x, 40 + y}, {80 + x, 120 + y}, {x, 180 + y}, {180 + x, 180 + y}, {240 + x, 120 + y}, {240 + x, 40 + y}, {180 + x, y}};
        polyShape->Holes    = {{{120 + x, 40 + y}, {120 + x, 120 + y}, {200 + x, 120 + y}, {200 + x, 40 + y}}};
        polyShape->Rotation = _rotation;
    } break;
    case 1: {
        auto circleShape {_batch.create_shape<gfx::circle_shape>()};
        circleShape->Color    = colors::Blue;
        circleShape->Material = _emptyMat;
        circleShape->Center   = _center;
        circleShape->Radius   = 100;
    } break;
    case 2: {
        auto rectShape {_batch.create_shape<gfx::rect_shape>()};
        rectShape->Color    = colors::Blue;
        rectShape->Material = _emptyMat;
        size_f const rectSize {250, 250};
        rectShape->Bounds   = {_center - point_f {rectSize.Width / 2, rectSize.Height / 2}, rectSize};
        rectShape->Rotation = _rotation;
    } break;
    }

    _batch.update(deltaTime);

    std::vector<ray::result> points;
    auto const               castRay {[&points, batch = &_batch, mat = _emptyMat](point_f origin, degree_f deg) {
        ray  ray {{.Origin = origin, .Direction = deg, .MaxDistance = 600}};
        auto points0 {batch->intersect(ray)};
        if (!points0.empty()) {
            for (auto const& v : points0) {
                points.insert(points.end(), v.second.begin(), v.second.end());
            }
        }

        auto       rayShape {batch->create_shape<gfx::rect_shape>()};
        auto const height {static_cast<f32>(ray.get_point(0).distance_to(ray.get_point(600)))};
        rayShape->Bounds      = {{origin.X - 2, origin.Y}, {4, height}};
        rayShape->Pivot       = rayShape->Bounds->top_left();
        rayShape->Rotation    = deg - degree_f {180};
        rayShape->Color       = colors::Red;
        rayShape->Material    = mat;
        rayShape->RayCastMask = 0;
    }};
    castRay({_tween0.Value, 50}, {135});
    castRay({1300 - _tween0.Value, 50}, {225});

    castRay({50, 50}, {_tween1.Value});
    castRay({50, 600}, {180 - _tween1.Value});

    castRay({1300, 50}, {90 + _tween1.Value});
    castRay({1300, 600}, {90 - _tween1.Value});

    // draw intersections
    for (auto p : points) {
        auto dotShape {_batch.create_shape<gfx::circle_shape>()};
        dotShape->Center   = p.Point;
        dotShape->Radius   = 10;
        dotShape->Color    = colors::Green;
        dotShape->Material = _emptyMat;
    }
    _batch.update(deltaTime);
}

void RayEx::on_draw_to(render_target& target)
{
    _batch.draw_to(target);
}

void RayEx::on_key_down(keyboard::event const& ev)
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

void RayEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _center = (*get_window().Camera).convert_screen_to_world(ev.Position);
    _dirty  = true;
}

void RayEx::on_mouse_button_down(mouse::button_event const& ev)
{
    _mode  = (_mode + 1) % 3;
    _dirty = true;
}

void RayEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _rotation += static_cast<f32>(ev.Scroll.Y * 10);
    _dirty = true;
}
