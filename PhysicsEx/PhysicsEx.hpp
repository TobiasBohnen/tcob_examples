// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
#include "B2DebugDraw.hpp"
#include "UI.hpp"

////////////////////////////////////////////////////////////

using gfx_shape = std::variant<gfx::rect_shape*, gfx::circle_shape*, gfx::poly_shape*>;

class PhysicsEx : public scene {
public:
    PhysicsEx(game& game);
    ~PhysicsEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target, transform const& xform) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;
    void on_mouse_button_down(mouse::button_event const& ev) override;
    void on_mouse_button_up(mouse::button_event const& ev) override;

private:
    struct object {
        physics::body* Body {nullptr};
        gfx_shape      Shape;

        color DefaultColor;

        point_f LastCenter;
    };

    struct obstacles {
        physics::body*           Body {nullptr};
        std::vector<gfx::shape*> Sprites;
    };

    auto get_shape(gfx_shape const& shape) const -> gfx::shape*;
    void hit_test();

    void create_box(point_f pos);
    void create_circle(point_f pos);
    void create_polygon(point_f pos);
    void create_capsule(point_f pos);

    void create_obstacles();
    void create_obstacle(rect_f const& rect);
    void create_edge(point_f pos0, point_f pos1);

    world     _world {};
    obstacles _obstacles {};

    std::deque<object> _objects {};
    object*            _selectedObject {};

    shape_batch _objectLayer {};
    shape_batch _obstacleLayer {};

    font_family                   _font {""};
    std::shared_ptr<B2DDebugDraw> _debugDraw {};

    milliseconds _spawnTimer {0};
    bool         _mouseDown {false};
    point_f      _mousePos;

    std::shared_ptr<physics_form> _mainForm;

    rng _rng;
};
