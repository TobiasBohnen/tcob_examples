// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

#include "UI.hpp"

class LSystemEx : public scene {
public:
    LSystemEx(game& game);
    ~LSystemEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;

private:
    void step();

    canvas          _canvas;
    canvas_renderer _renderer {_canvas};

    string       _lstring;
    usize        _lstringpos {0};
    milliseconds _countdown {};

    std::shared_ptr<lsystem_form> _mainForm;

    struct state {
        point_f  Position;
        degree_f Angle;
        f32      LineLength {};
        degree_f TurningAngle {};
        f32      StrokeWidth {};
        bool     SwapTurn {};
    };

    state              _currentState;
    std::vector<state> _stateStack;
};
