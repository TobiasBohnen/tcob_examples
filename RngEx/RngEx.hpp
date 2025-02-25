// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

#include "UI.hpp"

class RngEx : public scene {
public:
    RngEx(game& game);
    ~RngEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;

private:
    std::shared_ptr<rng_form> _form0;
};
