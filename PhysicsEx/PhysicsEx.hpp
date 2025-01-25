// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"

////////////////////////////////////////////////////////////

class PhysicsEx : public scene {
public:
    PhysicsEx(game& game);
    ~PhysicsEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_button_up(mouse::button_event const& ev) override;

private:
    struct Box2DExObject {
        std::variant<std::shared_ptr<gfx::rect_shape>, std::shared_ptr<gfx::circle_shape>> Sprite;
        std::shared_ptr<physics::body>                                                     Body;
    };

    void create_box(point_f pos);
    void create_edge(point_f pos0, point_f pos1);
    void create_circle(point_f pos);
    void create_obstacle(rect_f const& rect);

    physics::world _world {};

    std::shared_ptr<physics::body> _obstacles {};
    std::vector<Box2DExObject>     _objects {};

    assets::owning_asset_ptr<material> _mat;

    shape_batch _layer1;
    bool        _forceOn {false};
    bool        _colorizeContact {false};
};
