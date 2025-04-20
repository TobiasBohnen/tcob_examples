// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PhysicsEx.hpp"

////////////////////////////////////////////////////////////

PhysicsEx::PhysicsEx(game& game)
    : scene(game)
{
    _world.Gravity = {0, 50.f};
}

PhysicsEx::~PhysicsEx() = default;

void PhysicsEx::on_start()
{
    // obstacles
    rng r;
    _obstacles = _world.create_body({}, {});
    for (i32 i {0}; i < 25; i++) {
        create_obstacle({r(0.f, 66.f), r(2.f, 35.f), r(1.f, 5.f), r(1.f, 5.f)});
    }

    create_edge({0, 45}, {70, 45});
}

void PhysicsEx::on_draw_to(render_target& target)
{
    _layer1.draw_to(target);
}

void PhysicsEx::on_update(milliseconds deltaTime)
{
    for (auto it {_objects.begin()}; it < _objects.end(); ++it) {
        std::visit(
            overloaded {
                [&](std::shared_ptr<gfx::rect_shape> const& rect) {
                    auto const bounds {rect->Bounds->as_centered_at(it->Body->Transform().Center * 12)};
                    if (rect->Bounds != bounds) {
                        rect->Bounds   = bounds;
                        rect->Rotation = it->Body->Transform().Angle;

                        if (rect->Bounds->center().Y > 1000) {
                            _layer1.remove_shape(*rect);
                            _world.remove_body(*it->Body);
                            it = _objects.erase(it);
                        }
                    }
                },
                [&](std::shared_ptr<gfx::circle_shape> const& circle) {
                    if (circle->Center != it->Body->Transform().Center * 12) {
                        circle->Center   = it->Body->Transform().Center * 12;
                        circle->Rotation = it->Body->Transform().Angle;

                        if (circle->Center->Y > 1000) {
                            _layer1.remove_shape(*circle);
                            _world.remove_body(*it->Body);
                            it = _objects.erase(it);
                        }
                    }
                }},
            it->Sprite);
    }

    _layer1.update(deltaTime);
}

void PhysicsEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().stats()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();

    _world.update(deltaTime);

    if (_forceOn) {
        for (auto& obj : _objects) {
            obj.Body->apply_force_to_center({10.f, -300.f});
        }
    }
}

void PhysicsEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::D1:
        _forceOn = !_forceOn;
        break;
    case scan_code::D2:
        for (auto& obj : _objects) {
            obj.Body->apply_linear_impulse_to_center({10.f, -300.f});
        }
        break;
    case scan_code::D3:
        for (auto& obj : _objects) {
            obj.Body->apply_torque(500);
        }
        break;
    case scan_code::D4:
        for (auto& obj : _objects) {
            obj.Body->apply_angular_impulse(25);
        }
        break;
    case scan_code::G: {
        for (auto& obj : _objects) {
            obj.Body->wake_up();
        }

        _world.Gravity = -_world.Gravity;
    } break;
    case scan_code::C:
        _colorizeContact = !_colorizeContact;
        break;
    case scan_code::J: {
        if (_objects.size() >= 2) {

            distance_joint::settings def;
            def.BodyA        = _objects[_objects.size() - 2].Body.get();
            def.BodyB        = _objects[_objects.size() - 1].Body.get();
            def.LocalAnchorA = def.BodyA->local_center();
            def.LocalAnchorB = def.BodyB->local_center();
            def.Length       = def.BodyA->center().distance_to(def.BodyB->center());

            _world.create_joint<distance_joint>(def);
        }
    } break;
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void PhysicsEx::on_mouse_button_up(mouse::button_event const& ev)
{
    point_f pos {ev.Position};
    pos /= point_f {12.f, 12.f};

    if (ev.Button == mouse::button::Left) {
        create_box({pos});
    } else if (ev.Button == mouse::button::Right) {
        create_circle({pos});
    } else {
        _world.explode({.Position = pos, .Radius = 120, .Falloff = 10, .ImpulsePerLength = 60});
    }
}

void PhysicsEx::create_box(point_f pos)
{
    Box2DExObject obj {};
    rect_f        rect {pos, {3, 3}};
    obj.Body = _world.create_body({.Center = rect.center()}, {.Type = body_type::Dynamic});

    physics::rect_shape::settings shape;
    shape.Material.Friction = 0.3f;
    shape.Density           = 0.5f;
    shape.Extents           = {point_f::Zero, rect.Size};

    obj.Body->create_shape<physics::rect_shape>(shape);

    auto rectShape {_layer1.create_shape<gfx::rect_shape>()};
    rectShape->Material = _mat;
    rectShape->Bounds   = rect * 12;
    rectShape->Color    = colors::Red;
    obj.Sprite          = rectShape;

    _objects.push_back(obj);
}

void PhysicsEx::create_circle(point_f pos)
{
    Box2DExObject obj {};
    rect_f        rect {pos, {3, 3}};
    obj.Body = _world.create_body({.Center = rect.center()}, {.Type = body_type::Dynamic});

    physics::circle_shape::settings shape;
    shape.Material.Friction = 0.3f;
    shape.Density           = 0.75f;
    shape.Radius            = rect.width() / 2;

    obj.Body->create_shape<physics::circle_shape>(shape);

    auto circleShape {_layer1.create_shape<gfx::circle_shape>()};
    circleShape->Segments = 18;
    circleShape->Material = _mat;
    circleShape->Radius   = rect.width() / 2 * 12;
    circleShape->Color    = colors::Yellow;
    obj.Sprite            = circleShape;

    _objects.push_back(obj);
}

void PhysicsEx::create_obstacle(rect_f const& rect)
{
    physics::rect_shape::settings shape;
    shape.Extents              = {rect.center(), rect.Size};
    shape.Material.Restitution = 0.5f;

    _obstacles->create_shape<physics::rect_shape>(shape);

    auto spr {_layer1.create_shape<gfx::rect_shape>()};
    spr->Material = _mat;
    spr->Bounds   = rect * 12;
    spr->Color    = colors::Green;
}

void PhysicsEx::create_edge(point_f pos0, point_f pos1)
{
    segment_shape::settings shape;
    shape.Point0 = pos0;
    shape.Point1 = pos1;

    _obstacles->create_shape<segment_shape>(shape);

    auto spr {_layer1.create_shape<gfx::rect_shape>()};
    spr->Material = _mat;
    spr->Bounds   = rect_f::FromLTRB(pos0.X, pos0.Y, pos1.X, pos1.Y + 5) * 12;
    spr->Color    = colors::Blue;
}
