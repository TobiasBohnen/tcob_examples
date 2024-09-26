// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

class RayEx : public scene {
public:
    RayEx(game& game);
    ~RayEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event& ev) override;
    void on_mouse_motion(mouse::motion_event& ev) override;
    void on_mouse_wheel(mouse::wheel_event& ev) override;

private:
    shape_batch                        _batch;
    assets::manual_asset_ptr<material> _emptyMat;

    point_f _center;
    i32     _rotation {45};

    bool _dirty {true};
};
