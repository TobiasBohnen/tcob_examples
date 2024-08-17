// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PhysicsEx.hpp"
#include <iomanip>
#include <iostream>

////////////////////////////////////////////////////////////

PhysicsEx::PhysicsEx(game& game)
    : scene(game)
{
    _world.Gravity = {0, 50.f};
}

PhysicsEx::~PhysicsEx() = default;

void PhysicsEx::on_start()
{
    auto* resGrp {get_game().get_library().get_group("res")};

    _boxMat    = resGrp->get<material>("mat-solid");
    _circleMat = resGrp->get<material>("mat-circle");

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
        if (it->Sprite->Center != it->Body->Transform().Center * 12) {
            it->Sprite->Center   = it->Body->Transform().Center * 12;
            it->Sprite->Rotation = it->Body->Transform().Angle;
            std::cout << it->Sprite->Center() << ":" << it->Sprite->Rotation() << "\n";
            if (it->Sprite->Center->Y > 1000) {
                _layer1.remove_shape(*it->Sprite);
                _world.remove_body(*it->Body);
                it = _objects.erase(it);
            }
        }
    }

    _layer1.update(deltaTime);
}

void PhysicsEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    get_window().Title = "TestGame " + stream.str();

    _world.update(deltaTime);

    if (_forceOn) {
        for (auto& obj : _objects) {
            obj.Body->apply_force_to_center({10.f, -300.f});
        }
    }
}

void PhysicsEx::on_key_down(keyboard::event& ev)
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
            def.BodyA        = _objects[_objects.size() - 2].Body;
            def.BodyB        = _objects[_objects.size() - 1].Body;
            def.LocalAnchorA = def.BodyA->get_local_center();
            def.LocalAnchorB = def.BodyB->get_local_center();
            def.Length       = def.BodyA->get_center().distance_to(def.BodyB->get_center());

            _world.create_joint<distance_joint>(def);
        }
    } break;
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    default:
        break;
    }
}

void PhysicsEx::on_mouse_button_up(mouse::button_event& ev)
{
    point_f pos {ev.Position};
    pos /= point_f {12.f, 12.f};

    if (ev.Button == mouse::button::Left) {
        create_box({pos});
    } else if (ev.Button == mouse::button::Right) {
        create_circle({pos});
    } else {
        _world.explode(pos, 120, 60);
    }
}

void PhysicsEx::create_box(point_f pos)
{
    Box2DExObject obj {};
    rect_f        rect {pos, {3, 3}};
    obj.Body = _world.create_body({.Center = rect.get_center()}, {.Type = body_type::Dynamic});

    physics::rect_shape::settings shape;
    shape.Friction = 0.3f;
    shape.Density  = 0.5f;
    shape.Extents  = {point_f::Zero, rect.get_size()};

    obj.Body->create_shape<physics::rect_shape>(shape);

    obj.Sprite           = _layer1.create_shape<gfx::rect_shape>();
    obj.Sprite->Material = _boxMat;
    obj.Sprite->Bounds   = rect * 12;
    obj.Sprite->Color    = colors::Red;

    _objects.push_back(obj);
}

void PhysicsEx::create_circle(point_f pos)
{
    Box2DExObject obj {};
    rect_f        rect {pos, {3, 3}};
    obj.Body = _world.create_body({.Center = rect.get_center()}, {.Type = body_type::Dynamic});

    physics::circle_shape::settings shape;
    shape.Friction = 0.3f;
    shape.Density  = 0.75f;
    shape.Radius   = rect.get_size().Width / 2;

    obj.Body->create_shape<physics::circle_shape>(shape);

    obj.Sprite           = _layer1.create_shape<gfx::rect_shape>();
    obj.Sprite->Material = _circleMat;
    obj.Sprite->Bounds   = rect * 12;
    obj.Sprite->Color    = colors::Yellow;

    _objects.push_back(obj);
}

void PhysicsEx::create_obstacle(rect_f const& rect)
{
    physics::rect_shape::settings shape;
    shape.Extents = {rect.get_center(), rect.get_size()};

    _obstacles->create_shape<physics::rect_shape>(shape);

    auto spr {_layer1.create_shape<gfx::rect_shape>()};
    spr->Material = _boxMat;
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
    spr->Material = _boxMat;
    spr->Bounds   = rect_f::FromLTRB(pos0.X, pos0.Y, pos1.X, pos1.Y + 5) * 12;
    spr->Color    = colors::Blue;
}

/*
void PhysicsEx::on_begin_contact(contact_event const& ev)
{
    if (!_colorizeContact) {
        return;
    }

    auto& bodyA {ev.FixtureA->get_body()};
    auto& bodyB {ev.FixtureB->get_body()};
    if (*_obstacles == bodyA || *_obstacles == bodyB) {
        return;
    }

    for (auto& obj : _objects) {
        if (*obj.Body == bodyA) {
            obj.Sprite->Color = colors::Beige;
        }
        if (*obj.Body == bodyB) {
            obj.Sprite->Color = colors::Aquamarine;
        }
    }
}

void PhysicsEx::on_end_contact(contact_event const& ev)
{
    if (!_colorizeContact) {
        return;
    }

    auto& bodyA {ev.FixtureA->get_body()};
    auto& bodyB {ev.FixtureB->get_body()};
    if (*_obstacles == bodyA || *_obstacles == bodyB) {
        return;
    }

    for (auto& obj : _objects) {
        if (*obj.Body == bodyA) {
            obj.Sprite->Color = colors::Black;
        }
        if (*obj.Body == bodyB) {
            obj.Sprite->Color = colors::Black;
        }
    }
}
*/
