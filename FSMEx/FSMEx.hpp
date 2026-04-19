// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

class FSMEx : public scene {
public:
    explicit FSMEx(game& game);
    ~FSMEx() override;

protected:
    void on_start() override;
    void on_fixed_update(milliseconds deltaTime) override;
    void on_update(milliseconds deltaTime) override;
    void on_draw_to(render_target& target, transform const& xform) override;
    void on_key_down(keyboard::event const& ev) override;

private:
    struct prey_data {
        point_f Position {};
        point_f Velocity {};
        bool    Alive {true};
    };

    struct hunter_data {
        point_f              Position {};
        point_f              Home {};
        point_f              Velocity {};
        std::optional<usize> Target {};
        i32                  CaughtCount {0};
    };

    static constexpr f32 DETECTION_RANGE {200.0f};
    static constexpr f32 CATCH_RANGE {20.0f};
    static constexpr f32 HUNTER_SPEED {180.0f};
    static constexpr i32 NUM_PREY {16};

    fsm                    _fsm {};
    gfx::shape_batch       _batch {};
    std::vector<prey_data> _prey {};

    std::vector<gfx::rect_shape*> _preyShapes {};
    gfx::rect_shape*              _hunterShape {nullptr};
    gfx::circle_shape*            _rangeShape {nullptr};

    size_f _windowSize {};
};
