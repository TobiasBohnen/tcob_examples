// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "RayEx.hpp"

string const obj_mesh {R"(
# Blender 5.0.1
# www.blender.org
o Cube
v 200.510468 119.321335 35.717751
v 138.740906 122.617752 35.717747
v 67.040382 116.998550 35.717751
v 83.633736 66.252197 35.717747
v 148.407623 29.316662 35.717739
v 175.891663 62.406006 35.717743
v 8.609720 127.638878 35.717751
v 10.029209 26.900352 35.717743
v 118.183525 41.500885 35.717743
v 114.085945 217.355652 35.717762
v 168.128174 196.295654 35.717762
v 69.729431 178.244202 35.717754
v 250.675354 164.594910 35.717758
v 257.261230 41.674732 35.717743
v 184.656586 9.586830 35.717739
v 97.837059 5.873154 35.717739
v 59.448414 16.569122 35.717739
s 0
f 3 8 4
f 2 12 3
f 2 11 10
f 5 9 16
f 14 13 1
f 3 7 8
f 2 1 11
f 2 10 12
f 9 4 17
f 4 8 17
f 17 16 9
f 16 15 5
f 14 6 15
f 6 5 15
f 1 6 14
f 13 11 1
f 12 7 3

)"};

RayEx::RayEx(game& game)
    : scene {game}
{
}

RayEx::~RayEx() = default;

void RayEx::on_fixed_update(milliseconds /* deltaTime */)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);
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
        auto& polyShape {_batch.create_shape<gfx::poly_shape>()};
        polyShape.Color    = colors::Blue;
        polyShape.Material = material::Empty();
        auto const [x, y] {_center - point_f {150, 50}};
        polygon poly {
            .Outline = {{x, y}, {80 + x, 40 + y}, {80 + x, 120 + y}, {x, 180 + y}, {180 + x, 180 + y}, {240 + x, 120 + y}, {240 + x, 40 + y}, {180 + x, y}},
            .Holes   = {{{200 + x, 40 + y}, {200 + x, 120 + y}, {120 + x, 120 + y}, {120 + x, 40 + y}}}};
        polyShape.Polygons = {poly};
        polyShape.Rotation = degree_f {_rotation};
    } break;
    case 1: {
        auto& circleShape {_batch.create_shape<gfx::circle_shape>()};
        circleShape.Color    = colors::Blue;
        circleShape.Material = material::Empty();
        circleShape.Center   = _center;
        circleShape.Radius   = 100;
    } break;
    case 2: {
        auto& rectShape {_batch.create_shape<gfx::rect_shape>()};
        rectShape.Color    = colors::Blue;
        rectShape.Material = material::Empty();
        size_f const rectSize {250, 250};
        rectShape.Bounds   = {_center - point_f {rectSize.Width / 2, rectSize.Height / 2}, rectSize};
        rectShape.Rotation = degree_f {_rotation};
    } break;
    case 3: {
        auto&                      meshShape {_batch.create_shape<gfx::mesh_shape>()};
        std::span<std::byte const> bytes {reinterpret_cast<std::byte const*>(obj_mesh.data()), obj_mesh.size()};

        io::isstream str {bytes};
        auto         _     = meshShape.load(str, ".obj");
        meshShape.Material = material::Empty();
        meshShape.Color    = colors::Blue;
        meshShape.Rotation = degree_f {_rotation};

        meshShape.move_by(_center - point_i {100, 100});
    }
    }

    _batch.update(deltaTime);

    std::vector<ray::result> points;
    auto const               castRay {[&points, batch = &_batch](point_f origin, degree_f deg) {
        ray  ray {origin, deg, 600};
        auto points0 {batch->intersect(ray)};
        if (!points0.empty()) {
            for (auto const& v : points0) {
                points.insert(points.end(), v.second.begin(), v.second.end());
            }
        }

        auto&      rayShape {batch->create_shape<gfx::rect_shape>()};
        auto const height {static_cast<f32>(ray.get_point(0).distance_to(ray.get_point(600)))};
        rayShape.Bounds        = {{origin.X - 2, origin.Y}, {4, height}};
        rayShape.Pivot         = rayShape.Bounds->top_left();
        rayShape.Rotation      = deg - degree_f {180};
        rayShape.Color         = colors::Red;
        rayShape.Material      = material::Empty();
        rayShape.IntersectMask = 0;
    }};
    castRay({_tween0.Value, 50}, degree_f {135});
    castRay({1300 - _tween0.Value, 50}, degree_f {225});

    castRay({50, 50}, degree_f {_tween1.Value});
    castRay({50, 600}, degree_f {180 - _tween1.Value});

    castRay({1300, 50}, degree_f {90 + _tween1.Value});
    castRay({1300, 600}, degree_f {90 - _tween1.Value});

    // draw intersections
    for (auto p : points) {
        auto& dotShape {_batch.create_shape<gfx::circle_shape>()};
        dotShape.Center   = p.Point;
        dotShape.Radius   = 10;
        dotShape.Color    = colors::Green;
        dotShape.Material = material::Empty();
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
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void RayEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _center = window().camera().convert_screen_to_world(ev.Position);
    _dirty  = true;
}

void RayEx::on_mouse_button_down(mouse::button_event const& ev)
{
    _mode  = (_mode + 1) % 4;
    _dirty = true;
}

void RayEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _rotation += static_cast<f32>(ev.Scroll.Y * 10);
    _dirty = true;
}
