// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

class MetaballEx : public scene {
public:
    MetaballEx(game& game);
    ~MetaballEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target, transform const& xform) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;

private:
    metaball_image _metaball;

    renderer                  _renderer {buffer_usage_hint::StaticDraw};
    asset_owner_ptr<material> _material;
    quad                      _quad;
    asset_owner_ptr<texture>  _texture;
};
