// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
#include "UI.hpp"

class MusicEx : public scene {
public:
    MusicEx(game& game);
    ~MusicEx();

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;

private:
    std::shared_ptr<crtl_form> _form0;

    music _music0;
};
