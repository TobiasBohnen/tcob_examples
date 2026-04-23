// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"

////////////////////////////////////////////////////////////

class sim_manager;

struct prey {
    point_f Position {};
    point_f Velocity {};
    bool    Alive {true};
};

struct prey_data {
    prey             Actor {};
    gfx::rect_shape* Shape {nullptr};
    fsm              Behavior {};

    sim_manager* Manager {};

    void move(milliseconds dt);

    void on_wander_enter() const;
    void on_wander_update(milliseconds dt);

    auto on_wander_flee_condition() const -> bool;

    void on_flee_enter() const;
    void on_flee_update(milliseconds dt);

    auto on_flee_wander_condition() const -> bool;
};

struct hunter {
    point_f Position {};
    point_f Velocity {};
    bool    Alive {true};

    point_f                         Home {};
    std::weak_ptr<struct prey_data> Target {};
    i32                             CaughtCount {0};
};

struct hunter_data {
    hunter             Actor {};
    gfx::rect_shape*   Shape {nullptr};
    gfx::circle_shape* RangeShape {nullptr};
    fsm                Behavior {};

    sim_manager* Manager {};

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

////////////////////////////////////////////////////////////

class sim_manager {
public:
    sim_manager(size_f const& winSize);

    auto spawn_hunter(point_f const& pos) -> std::shared_ptr<hunter_data>;

    auto spawn_prey(point_f const& pos) -> std::shared_ptr<prey_data>;

    auto find_nearest_prey(point_f const& origin, f32 range) const -> std::shared_ptr<prey_data>;

    void update(milliseconds dt);

    auto bounds() const -> size_f const&;
    auto hunter() const -> std::shared_ptr<hunter_data>;
    auto prey_list() const -> std::vector<std::shared_ptr<prey_data>> const&;

private:
    size_f                                  _windowSize {};
    std::shared_ptr<hunter_data>            _hunter {nullptr};
    std::vector<std::shared_ptr<prey_data>> _preyList {};
};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

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

    size_f                       _windowSize {};
    gfx::shape_batch             _batch {};
    std::shared_ptr<sim_manager> _sim {nullptr};

    rng _rng {};
};
