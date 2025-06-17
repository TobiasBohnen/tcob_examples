// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

class PointCloudEx : public scene {
public:
    PointCloudEx(game& game);
    ~PointCloudEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_button_down(mouse::button_event const& ev) override;
    void on_mouse_button_up(mouse::button_event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;
    void on_mouse_wheel(mouse::wheel_event const& ev) override;

private:
    rng _rand;

    std::shared_ptr<point_cloud> _cloud0;
    asset_owner_ptr<material>    _mat;
    canvas                       _canvas;

    struct point_node {
        i32     ID {0};
        vertex* Vertex {};

        auto get_rect() const -> rect_f
        {
            return {Vertex->Position, size_f::One};
        }

        auto operator==(point_node const& other) const -> bool
        {
            return ID == other.ID;
        }
    };
    quadtree<point_node> _quadtree;

    basic_cam _cam;
};
