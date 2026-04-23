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
    void setup_prey();
    void setup_hunter();

    struct prey {
        point_f              Position {};
        point_f              Velocity {};
        bool                 Alive {true};
        gfx::rect_shape*     Shape {nullptr};
        std::shared_ptr<fsm> Behavior {};
    };

    struct hunter {
        point_f             Position {};
        point_f             Home {};
        point_f             Velocity {};
        std::weak_ptr<prey> Target {};
        gfx::rect_shape*    Shape {nullptr};
        gfx::circle_shape*  RangeShape {nullptr};
        i32                 CaughtCount {0};
        fsm                 Behavior {};
    };

    static constexpr f32 DETECTION_RANGE {200.0f};
    static constexpr f32 CATCH_RANGE {20.0f};
    static constexpr f32 HUNTER_SPEED {250.0f};
    static constexpr i32 NUM_PREY {12};

    gfx::shape_batch _batch {};

    std::shared_ptr<hunter>            _hunter;
    std::vector<std::shared_ptr<prey>> _prey {};

    size_f _windowSize {};
    rng    _rng;
};
