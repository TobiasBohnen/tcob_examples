// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

// based on: https://lodev.org/cgtutor/raycasting.html Copyright (c) 2004-2019, Lode Vandevenne
// textures from: https://opengameart.org/content/zortch-unused-textures

#pragma once
#include "../_common/Common.hpp"
#include "Cache.hpp"

using namespace std::chrono_literals;
using namespace tcob::literals;

////////////////////////////////////////////////////////////

class RaycasterEx : public scene {
public:
    RaycasterEx(game& game);
    ~RaycasterEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_fixed_update(milliseconds deltaTime) override;
    void on_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;

private:
    void draw();
    void cast(i32 x, i32 w, i32 h);

    auto move(milliseconds deltaTime) -> bool;

    point_d _pos {5, 5};      // x and y start position
    point_d _dir {-1, 0};     // initial direction vector
    point_d _plane {0, 0.66}; // the 2d raycaster version of camera plane

    asset_owner_ptr<material> _material;
    asset_owner_ptr<texture>  _texture;
    quad_renderer             _renderer {buffer_usage_hint::StaticDraw};

    std::unique_ptr<cache_base> _cache;

    bool _update {true};
};
