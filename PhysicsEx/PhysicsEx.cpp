// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PhysicsEx.hpp"

#include <numbers>

#include "../_common/Trim.hpp"

using namespace std::literals;

////////////////////////////////////////////////////////////

PhysicsEx::PhysicsEx(game& game)
    : scene {game}
{
    font_family::SingleFont(_font, std::as_bytes(std::span {trim_ttf}));
    _debugDraw = std::make_shared<B2DDebugDraw>(_font.get_font({}, 10).ptr());
}

PhysicsEx::~PhysicsEx() = default;

void PhysicsEx::on_start()
{
    auto const windowSize {*window().Size};
    i32 const  uiWidth {static_cast<i32>(windowSize.Height / 2)};

    rect_i const menuBounds {windowSize.Width - uiWidth, 0, uiWidth, windowSize.Height};
    _mainForm      = std::make_shared<physics_form>(menuBounds);
    _world.Gravity = _mainForm->Gravity;

    _mainForm->Gravity.Changed.connect([&](point_f val) {
        for (auto& obj : _objects) { obj.Body->wake_up(); }
        _world.Gravity = val;
    });
    _mainForm->CreateObstacles.connect([&]() { create_obstacles(); });
    _mainForm->ClearObjects.connect([&]() {
        for (auto& obj : _objects) {
            _objectLayer.remove_shape(*get_shape(obj.Shape));
            _world.remove_body(*obj.Body);
        }
        _objects.clear();
        _selectedObject = nullptr;
    });
    root_node().Entity = _mainForm;

    create_obstacles();
}

void PhysicsEx::on_draw_to(render_target& target, transform const& xform)
{
    _objectLayer.draw_to(target, xform);
    _obstacleLayer.draw_to(target, xform);

    if (_mainForm->DebugMode) {
        _debugDraw->draw(_world, 0.5f, target);
    }
}

void PhysicsEx::on_update(milliseconds deltaTime)
{
    // Hold-to-spawn: fire immediately on first tick, then every 500ms.
    if (_mouseDown) {
        _spawnTimer -= deltaTime;
        if (_spawnTimer <= 0ms) {
            _spawnTimer = 500ms;
            switch (_mainForm->SpawnObject) {
            case spawn_object::Circle:    create_circle(_mousePos); break;
            case spawn_object::Box:       create_box(_mousePos); break;
            case spawn_object::Polygon:   create_polygon(_mousePos); break;
            case spawn_object::Capsule:   create_capsule(_mousePos); break;
            case spawn_object::Explosion: _world.explode({.Position = _mousePos, .Radius = 120, .Falloff = 10, .ImpulsePerLength = 60}); break;
            }
        }
    }

    // Sync gfx to physics; cull objects that have fallen off-screen.
    for (auto it {_objects.begin()}; it != _objects.end();) {
        auto const& tf {*it->Body->Transform};

        auto const [shapeY, shape] {std::visit(
            overloaded {
                [&](gfx::rect_shape* rect) -> std::pair<f32, gfx::shape*> {
                    auto const bounds {rect->Bounds->as_centered_at(tf.Center * physicsWorldSize)};
                    if (rect->Bounds != bounds) {
                        rect->Bounds   = bounds;
                        rect->Rotation = tf.Angle;
                    }
                    return {rect->Bounds->center().Y, rect};
                },
                [&](gfx::circle_shape* circle) -> std::pair<f32, gfx::shape*> {
                    point_f const center {tf.Center * physicsWorldSize};
                    if (circle->Center != center) {
                        circle->Center   = center;
                        circle->Rotation = tf.Angle;
                    }
                    return {circle->Center->Y, circle};
                },
                [&](gfx::poly_shape* poly) -> std::pair<f32, gfx::shape*> {
                    point_f const center {tf.Center * physicsWorldSize};
                    point_f const delta {center - it->LastCenter};
                    if (delta != point_f::Zero) {
                        poly->move_by(delta);
                        poly->Rotation = tf.Angle;
                        it->LastCenter = center;
                    }
                    return {it->LastCenter.Y, poly};
                },
            },
            it->Shape)};
        shape->Color = it->DefaultColor;
        if (shapeY > 1000) {
            _objectLayer.remove_shape(*shape);
            _world.remove_body(*it->Body);
            it              = _objects.erase(it);
            _selectedObject = nullptr;
        } else {
            ++it;
        }
    }

    if (_selectedObject) {
        get_shape(_selectedObject->Shape)->Color = colors::WhiteSmoke;
    }

    _objectLayer.update(deltaTime);
    _obstacleLayer.update(deltaTime);
}

void PhysicsEx::on_fixed_update(milliseconds deltaTime)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);

    _world.update(deltaTime);
    hit_test();
}

void PhysicsEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE: parent().pop_current_scene(); break;
    default:                   break;
    }
}

void PhysicsEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _mousePos = point_f {ev.Position} / physicsWorldSize;
}

void PhysicsEx::on_mouse_button_down(mouse::button_event const& ev)
{
    if (!_selectedObject) {
        _spawnTimer = 0ms;
        _mouseDown  = true;
    }
}

void PhysicsEx::on_mouse_button_up(mouse::button_event const& ev)
{
    _mouseDown = false;
}

auto PhysicsEx::get_shape(gfx_shape const& shape) const -> gfx::shape*
{
    return std::visit(
        overloaded {
            [&](gfx::rect_shape* v) -> gfx::shape* { return v; },
            [&](gfx::circle_shape* v) -> gfx::shape* { return v; },
            [&](gfx::poly_shape* v) -> gfx::shape* { return v; },
        },
        shape);
}

void PhysicsEx::hit_test()
{
    _selectedObject = nullptr;
    for (auto& obj : _objects) {
        if (obj.Body->test_point(_mousePos)) {
            _selectedObject = &obj;
            break;
        }
    }
}

void PhysicsEx::create_box(point_f pos)
{
    rect_f const rect {pos, {3, 3}};

    auto& body {_world.create_body({.Center = rect.center()}, {.Type = body_type::Dynamic})};

    physics::rect_shape::settings shape;
    shape.Shape.Material.Friction = 0.75f;
    shape.Shape.Density           = 0.5f;
    shape.Extents                 = {point_f::Zero, rect.Size};
    body.create_shape<physics::rect_shape>(shape);

    auto& gfx {_objectLayer.create_shape<gfx::rect_shape>()};
    gfx.Material = material::Empty();
    gfx.Bounds   = rect * physicsWorldSize;
    gfx.Color    = colors::Red;

    _objects.emplace_back(&body, &gfx, colors::Red);
}

void PhysicsEx::create_circle(point_f pos)
{
    rect_f const rect {pos, {3, 3}};

    auto& body {_world.create_body({.Center = rect.center()}, {.Type = body_type::Dynamic})};

    physics::circle_shape::settings shape;
    shape.Shape.Material.Friction = 0.3f;
    shape.Shape.Density           = 0.75f;
    shape.Radius                  = rect.width() / 2;
    body.create_shape<physics::circle_shape>(shape);

    auto& gfx {_objectLayer.create_shape<gfx::circle_shape>()};
    gfx.Segments = 18;
    gfx.Material = material::Empty();
    gfx.Radius   = rect.width() / 2 * physicsWorldSize;
    gfx.Color    = colors::Yellow;

    _objects.emplace_back(&body, &gfx, colors::Yellow);
}

void PhysicsEx::create_polygon(point_f pos)
{
    i32 const sides {_rng(3, 8)};
    f32 const radius {1.5f};
    f32 const step {TAU_F / sides};

    std::vector<point_f> verts;
    verts.reserve(sides);
    for (i32 i {0}; i < sides; ++i) {
        f32 const a {step * static_cast<f32>(i)};
        verts.emplace_back(radius * std::cos(a), radius * std::sin(a));
    }

    auto& body {_world.create_body({.Center = pos}, {.Type = body_type::Dynamic})};

    physics::polygon_shape::settings phys;
    phys.Shape.Material.Friction    = 0.4f;
    phys.Shape.Material.Restitution = 0.2f;
    phys.Shape.Density              = 0.6f;
    phys.Vertices                   = verts;
    body.create_shape<physics::polygon_shape>(phys);

    point_f const gfxOrigin {pos * physicsWorldSize};
    polygon       gfxPoly;
    for (auto const& v : verts) {
        gfxPoly.Outline.push_back(gfxOrigin + v * physicsWorldSize);
    }

    auto& gfx {_objectLayer.create_shape<gfx::poly_shape>()};
    gfx.Material = material::Empty();
    gfx.Color    = colors::Cyan;
    gfx.Polygons.mutate([&](auto& polys) { polys.push_back(gfxPoly); });

    _objects.emplace_back(&body, &gfx, colors::Cyan, gfxOrigin);
}

void PhysicsEx::create_capsule(point_f pos)
{
    constexpr f32 radius {0.8f};
    constexpr f32 halfLen {1.25f};
    constexpr i32 segs {8};
    constexpr f32 pi {std::numbers::pi_v<f32>};

    auto& body {_world.create_body({.Center = pos}, {.Type = body_type::Dynamic})};

    physics::capsule_shape::settings phys;
    phys.Shape.Material.Friction = 0.3f;
    phys.Shape.Density           = 0.65f;
    phys.Radius                  = radius;
    phys.Center0                 = {0.f, -halfLen};
    phys.Center1                 = {0.f, halfLen};
    body.create_shape<physics::capsule_shape>(phys);

    point_f const gfxOrigin {pos * physicsWorldSize};
    f32 const     gr {radius * physicsWorldSize};
    f32 const     gl {halfLen * physicsWorldSize};

    polygon gfxPoly;
    for (i32 i {0}; i <= segs; ++i) {
        f32 const a {pi + (pi * static_cast<f32>(i) / segs)};
        gfxPoly.Outline.push_back(gfxOrigin + point_f {gr * std::cos(a), -gl + (gr * std::sin(a))});
    }
    for (i32 i {0}; i <= segs; ++i) {
        f32 const a {pi * static_cast<f32>(i) / segs};
        gfxPoly.Outline.push_back(gfxOrigin + point_f {gr * std::cos(a), gl + (gr * std::sin(a))});
    }

    auto& gfx {_objectLayer.create_shape<gfx::poly_shape>()};
    gfx.Material = material::Empty();
    gfx.Color    = colors::Magenta;
    gfx.Polygons.mutate([&](auto& polys) { polys.push_back(gfxPoly); });

    _objects.emplace_back(&body, &gfx, colors::Magenta, gfxOrigin);
}

void PhysicsEx::create_obstacles()
{
    // Remove existing obstacle body and all its gfx shapes before recreating.
    if (_obstacles.Body) {
        for (auto* spr : _obstacles.Sprites) {
            _obstacleLayer.remove_shape(*spr);
        }
        _world.remove_body(*_obstacles.Body);
        _obstacles.Sprites.clear();
    }

    _obstacles.Body = &_world.create_body({}, {});

    for (i32 i {0}; i < 25; i++) {
        create_obstacle({_rng(0.f, 66.f), _rng(2.f, 35.f), _rng(1.f, 5.f), _rng(1.f, 5.f)});
    }

    create_edge({5, 45}, {75, 45});
}

void PhysicsEx::create_obstacle(rect_f const& rect)
{
    physics::rect_shape::settings shape;
    shape.Extents                    = {rect.center(), rect.Size};
    shape.Shape.Material.Restitution = 0.9f;
    _obstacles.Body->create_shape<physics::rect_shape>(shape);

    auto& gfx {_obstacleLayer.create_shape<gfx::rect_shape>()};
    gfx.Material = material::Empty();
    gfx.Bounds   = rect * physicsWorldSize;
    gfx.Color    = colors::Green;
    _obstacles.Sprites.push_back(&gfx);
}

void PhysicsEx::create_edge(point_f pos0, point_f pos1)
{
    segment_shape::settings shape;
    shape.Point1                     = pos0;
    shape.Point2                     = pos1;
    shape.Shape.Material.Restitution = 0.1f;
    _obstacles.Body->create_shape<segment_shape>(shape);

    auto& gfx {_obstacleLayer.create_shape<gfx::rect_shape>()};
    gfx.Material = material::Empty();
    gfx.Bounds   = rect_f::FromLTRB(pos0.X, pos0.Y, pos1.X, pos1.Y + 5) * physicsWorldSize;
    gfx.Color    = colors::Blue;
    _obstacles.Sprites.push_back(&gfx);
}
