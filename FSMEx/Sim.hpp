// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"

constexpr i32 NUM_PREY {12};

////////////////////////////////////////////////////////////

class sim_manager;

struct prey {
    point_f Position {};
    point_f Velocity {};
    bool    Alive {true};

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

    point_f                    Home {};
    std::weak_ptr<struct prey> Target {};

    i32 CaughtCount {0};

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

    auto spawn_hunter(point_f pos) -> std::shared_ptr<hunter>;
    auto spawn_prey(point_f pos, point_f vel) -> std::shared_ptr<prey>;

    auto find_nearest_prey(point_f origin, f32 range) const -> std::shared_ptr<prey>;

    void update(milliseconds dt);
    void draw_to(render_target& target, transform const& xform);

    auto get_bounds() const -> size_f;
    auto get_hunter() const -> std::shared_ptr<hunter>;
    auto get_prey_list() const -> std::vector<std::shared_ptr<prey>> const&;

    auto get_state_name(uid id) const -> string;

private:
    size_f                             _windowSize {};
    std::shared_ptr<hunter>            _hunter {nullptr};
    std::vector<std::shared_ptr<prey>> _preyList {};

    gfx::shape_batch _batch {};
};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////