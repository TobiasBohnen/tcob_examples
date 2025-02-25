// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"

class ImageFilterEx : public scene {
public:
    ImageFilterEx(game& game);
    ~ImageFilterEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;

private:
    shape_batch                _layer1;
    owning_asset_ptr<material> _mat0 {};
    owning_asset_ptr<texture>  _tex0 {};
};
