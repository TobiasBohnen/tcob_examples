// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PhysicsEx.hpp"

#include "../_common/Trim.hpp"

using namespace std::literals;

////////////////////////////////////////////////////////////

PhysicsEx::PhysicsEx(game& game)
    : scene {game}
    , _mat {material::Empty()}
{
    font_family::SingleFont(_font, std::as_bytes(std::span {trim_ttf}));
    _debugDraw = std::make_shared<B2DDebugDraw>(_font.get_font({}, 10).ptr());
}

PhysicsEx::~PhysicsEx() = default;

void PhysicsEx::on_start()
{
    auto&     win {window()};
    auto      windowSize {*win.Size};
    i32 const uiWidth {static_cast<i32>(windowSize.Height / 2)};

    rect_i const menuBounds {windowSize.Width - uiWidth, 0, uiWidth, windowSize.Height};
    _mainForm      = std::make_shared<physics_form>(menuBounds);
    _world.Gravity = _mainForm->Gravity;
    _mainForm->Gravity.Changed.connect([&](point_f val) {
        for (auto& obj : _objects) { obj.Body->wake_up(); }
        _world.Gravity = val;
    });
    _mainForm->CreateObstacles.connect([&]() { create_obstacles(); });
    root_node().Entity = _mainForm;

    create_obstacles();
}

void PhysicsEx::on_draw_to(render_target& target, transform const& xform)
{
    _layer1.draw_to(target, xform);

    if (_mainForm->DebugMode) {
        _debugDraw->draw(_world, 0.5f, target);
    }
}

void PhysicsEx::on_update(milliseconds deltaTime)
{
    if (_mouseDown) {
        _mouseDownTimer -= deltaTime;
        if (_mouseDownTimer <= 0ms) {
            _mouseDownTimer = 500ms;
            switch (_mainForm->SpawnObject) {
            case spawn_object::Circle:    create_circle(_mousePos); break;
            case spawn_object::Box:       create_box(_mousePos); break;
            case spawn_object::Explosion: _world.explode({.Position = _mousePos, .Radius = 120, .Falloff = 10, .ImpulsePerLength = 60}); break;
            }
        }
    }

    for (auto it {_objects.begin()}; it < _objects.end();) {
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
            },
            it->Sprite)};

        if (shapeY > 1000) {
            _layer1.remove_shape(*shape);
            _world.remove_body(*it->Body);
            it = _objects.erase(it);
        } else {
            ++it;
        }
    }
    _layer1.update(deltaTime);
}

void PhysicsEx::on_fixed_update(milliseconds deltaTime)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);

    _world.update(deltaTime);
}

void PhysicsEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void PhysicsEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _mousePos = point_f {ev.Position} / physicsWorldSize;
}

void PhysicsEx::on_mouse_button_down(mouse::button_event const& ev)
{
    _mouseDownTimer = 0ms;
    _mouseDown      = true;
}

void PhysicsEx::on_mouse_button_up(mouse::button_event const& ev)
{
    _mouseDown = false;
}

void PhysicsEx::create_box(point_f pos)
{
    object obj {};
    rect_f rect {pos, {3, 3}};
    obj.Body = &_world.create_body({.Center = rect.center()}, {.Type = body_type::Dynamic});

    physics::rect_shape::settings shape;
    shape.Shape.Material.Friction = 0.3f;
    shape.Shape.Density           = 0.5f;
    shape.Extents                 = {point_f::Zero, rect.Size};

    obj.Body->create_shape<physics::rect_shape>(shape);

    auto& rectShape {_layer1.create_shape<gfx::rect_shape>()};
    rectShape.Material = _mat;
    rectShape.Bounds   = rect * physicsWorldSize;
    rectShape.Color    = colors::Red;
    obj.Sprite         = &rectShape;

    _objects.push_back(obj);
}

void PhysicsEx::create_circle(point_f pos)
{
    object obj {};
    rect_f rect {pos, {3, 3}};
    obj.Body = &_world.create_body({.Center = rect.center()}, {.Type = body_type::Dynamic});

    physics::circle_shape::settings shape;
    shape.Shape.Material.Friction = 0.3f;
    shape.Shape.Density           = 0.75f;
    shape.Radius                  = rect.width() / 2;

    obj.Body->create_shape<physics::circle_shape>(shape);

    auto& circleShape {_layer1.create_shape<gfx::circle_shape>()};
    circleShape.Segments = 18;
    circleShape.Material = _mat;
    circleShape.Radius   = rect.width() / 2 * physicsWorldSize;
    circleShape.Color    = colors::Yellow;
    obj.Sprite           = &circleShape;

    _objects.push_back(obj);
}

void PhysicsEx::create_obstacles()
{
    if (_obstacles.Body) {
        for (auto* shape : _obstacles.Sprites) {
            _layer1.remove_shape(*shape);
        }
        _world.remove_body(*_obstacles.Body);
        _obstacles.Sprites.clear();
    }

    rng r;
    _obstacles.Body = &_world.create_body({}, {});
    for (i32 i {0}; i < 25; i++) {
        create_obstacle({r(0.f, 66.f), r(2.f, 35.f), r(1.f, 5.f), r(1.f, 5.f)});
    }

    create_edge({0, 45}, {70, 45});
}

void PhysicsEx::create_obstacle(rect_f const& rect)
{
    physics::rect_shape::settings shape;
    shape.Extents                    = {rect.center(), rect.Size};
    shape.Shape.Material.Restitution = 0.5f;

    _obstacles.Body->create_shape<physics::rect_shape>(shape);

    auto& spr {_layer1.create_shape<gfx::rect_shape>()};
    spr.Material = _mat;
    spr.Bounds   = rect * physicsWorldSize;
    spr.Color    = colors::Green;
    _obstacles.Sprites.push_back(&spr);
}

void PhysicsEx::create_edge(point_f pos0, point_f pos1)
{
    segment_shape::settings shape;
    shape.Point1 = pos0;
    shape.Point2 = pos1;

    _obstacles.Body->create_shape<segment_shape>(shape);

    auto& spr {_layer1.create_shape<gfx::rect_shape>()};
    spr.Material = _mat;
    spr.Bounds   = rect_f::FromLTRB(pos0.X, pos0.Y, pos1.X, pos1.Y + 5) * physicsWorldSize;
    spr.Color    = colors::Blue;
    _obstacles.Sprites.push_back(&spr);
}
