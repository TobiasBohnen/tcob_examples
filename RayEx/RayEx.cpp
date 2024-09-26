﻿// Copyright (c) 2024 Tobias Bohnen
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
}

void RayEx::on_update(milliseconds deltaTime)
{
    if (!_dirty) { return; }
    _dirty = false;

    _batch.clear();

    // draw shape
    auto shape {_batch.create_shape<gfx::poly_shape>()};
    shape->Color    = colors::Blue;
    shape->Material = _emptyMat;
    auto const [x, y] {_center - point_f {50, 0}};
    shape->Polygon  = {{x, y}, {40 + x, 20 + y}, {40 + x, 60 + y}, {x, 90 + y}, {90 + x, 90 + y}, {120 + x, 60 + y}, {120 + x, 20 + y}, {90 + x, y}};
    shape->Holes    = {{{60 + x, 20 + y}, {60 + x, 60 + y}, {100 + x, 60 + y}, {100 + x, 20 + y}}};
    shape->Rotation = _rotation;

    _batch.update(deltaTime);

    std::vector<ray::result> points;
    auto                     castRay {[&points, batch = &_batch, mat = _emptyMat](point_f origin, degree_f deg) {
        ray  ray {origin, deg};
        auto points0 {batch->intersect(ray)};
        if (points0.size() > 0) {
            for (auto const& v : points0) { points.insert(points.end(), v.second.begin(), v.second.end()); }

            auto       rayShape {batch->create_shape<gfx::rect_shape>()};
            auto const height {static_cast<f32>(ray.get_point(0).distance_to(ray.get_point(1000)))};
            rayShape->Bounds      = {{origin.X - 2, origin.Y}, {4, height}};
            rayShape->Pivot       = rayShape->Bounds->top_left();
            rayShape->Rotation    = deg - degree_f {180};
            rayShape->Color       = colors::Red;
            rayShape->Material    = mat;
            rayShape->RayCastMask = 0;
        }
    }};
    castRay({0, 0}, {135});
    castRay({600, 0}, {225});
    castRay({300, 0}, {180});
    castRay({0, 300}, {90});

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

void RayEx::on_key_down(keyboard::event& ev)
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

void RayEx::on_mouse_motion(mouse::motion_event& ev)
{
    _center = (*get_window().Camera).convert_screen_to_world(ev.Position);
    _dirty  = true;
}

void RayEx::on_mouse_wheel(mouse::wheel_event& ev)
{
    _rotation += (ev.Scroll.Y * 10);
    _dirty = true;
}
