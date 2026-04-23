// FSMEx.hpp
#pragma once
#include "../_common/Common.hpp"

struct hunter_data;

struct prey {
    point_f Position {};
    point_f Velocity {};
    bool    Alive {true};
};

struct prey_data {
    gfx::rect_shape* Shape {nullptr};
    fsm              Behavior {};
    size_f           WindowSize {};
    hunter_data*     Hunter {nullptr};
    prey             Actor {};

    void on_wander_enter() const;
    void on_wander_update(milliseconds dt);

    auto on_wander_flee_condition() const -> bool;

    void on_flee_enter() const;
    void on_flee_update(milliseconds dt);

    auto on_flee_wander_condition() const -> bool;

    void move(milliseconds dt);
};

struct hunter {
    point_f                  Position {};
    point_f                  Home {};
    point_f                  Velocity {};
    std::weak_ptr<prey_data> Target {};
    i32                      CaughtCount {0};
};

struct hunter_data {
    gfx::rect_shape*                               Shape {nullptr};
    gfx::circle_shape*                             RangeShape {nullptr};
    fsm                                            Behavior {};
    size_f                                         WindowSize {};
    std::vector<std::shared_ptr<prey_data>> const* Prey {nullptr};
    hunter                                         Actor {};

    void sync();

    void on_idle_enter();
    void on_idle_update(milliseconds dt);

    auto on_idle_hunt_condition() const -> bool;
    void on_idle_hunt_transition();

    void on_hunt_enter() const;
    void on_hunt_update(milliseconds dt);

    auto on_hunt_return_condition() const -> bool;
    void on_hunt_return_transition();

    void on_return_enter() const;
    void on_return_update(milliseconds dt);
    auto on_return_idle_condition() const -> bool;

    void on_dead_enter() const;
    auto on_dead_condition() const -> bool;
};

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

    gfx::shape_batch                        _batch {};
    std::shared_ptr<hunter_data>            _hunterData {};
    std::vector<std::shared_ptr<prey_data>> _preyData {};
    size_f                                  _windowSize {};
    rng                                     _rng {};
};
