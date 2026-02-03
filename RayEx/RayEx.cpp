// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "RayEx.hpp"

#include "Meshes.hpp"

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

constexpr i32 NUM_MODES {6};

void RayEx::on_update(milliseconds deltaTime)
{
    _tween0.update(deltaTime);
    _tween1.update(deltaTime);

    if (!_dirty) { return; }
    _dirty = false;

    _batch.clear();

    // draw shape
    switch (_mode) {
    case 5: {
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
        io::isstream               str {bytes};
        auto                       _ = meshShape.load(str, ".obj");
        meshShape.Material           = material::Empty();
        meshShape.Color              = colors::Blue;
        meshShape.Rotation           = degree_f {_rotation};
        meshShape.move_by(_center - point_i {100, 100});
    } break;
    case 4: {
        auto&                      meshShape {_batch.create_shape<gfx::mesh_shape>()};
        std::span<std::byte const> bytes {reinterpret_cast<std::byte const*>(ply_mesh.data()), ply_mesh.size()};
        io::isstream               str {bytes};
        auto                       _ = meshShape.load(str, ".ply");
        meshShape.Material           = material::Empty();
        meshShape.Color              = colors::White;
        meshShape.Rotation           = degree_f {_rotation};
        meshShape.move_by(_center - point_i {100, 100});
    } break;
    case 0: {
        auto&        meshShape {_batch.create_shape<gfx::mesh_shape>()};
        io::isstream str {std::as_bytes(std::span {ply_mesh_binary})};
        auto         _     = meshShape.load(str, ".ply");
        meshShape.Material = material::Empty();
        meshShape.Color    = colors::White;
        meshShape.Rotation = degree_f {_rotation};
        meshShape.move_by(_center - point_i {100, 100});
    } break;
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
    _mode  = (_mode + 1) % NUM_MODES;
    _dirty = true;
}

void RayEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _rotation += static_cast<f32>(ev.Scroll.Y * 10);
    _dirty = true;
}
