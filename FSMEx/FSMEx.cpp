// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "FSMEx.hpp"

constexpr uid STATE_IDLE {1};
constexpr uid STATE_HUNT {2};
constexpr uid STATE_RETURN {3};
constexpr uid STATE_DEAD {4};

constexpr f32 DETECTION_RANGE {200.0f};
constexpr f32 CATCH_RANGE {20.0f};
constexpr f32 HUNTER_SPEED {250.0f};
constexpr f32 PREY_FLEE_SPEED {150.0f};
constexpr i32 NUM_PREY {12};

static std::unordered_map<uid, string> const STATES {
    {INVALID_ID, "invalid"},
    {STATE_IDLE, "idle"},
    {STATE_HUNT, "hunting"},
    {STATE_RETURN, "returning"},
    {STATE_DEAD, "dead"},
};

constexpr uid PREY_WANDER {10};
constexpr uid PREY_FLEE {11};

static auto dts(milliseconds dt) -> f32
{
    return static_cast<f32>(dt.count()) / 1000.0f;
}

// prey_data member functions

void prey_data::move(milliseconds dt)
{
    Actor.Position += Actor.Velocity * dts(dt);

    if (Actor.Position.X < 10.0f) {
        Actor.Position.X = 10.0f;
        Actor.Velocity.X *= -1;
    } else if (Actor.Position.X > WindowSize.Width - 10.0f) {
        Actor.Position.X = WindowSize.Width - 10.0f;
        Actor.Velocity.X *= -1;
    }
    if (Actor.Position.Y < 10.0f) {
        Actor.Position.Y = 10.0f;
        Actor.Velocity.Y *= -1;
    } else if (Actor.Position.Y > WindowSize.Height - 10.0f) {
        Actor.Position.Y = WindowSize.Height - 10.0f;
        Actor.Velocity.Y *= -1;
    }
    Shape->Bounds = {{Actor.Position.X - 10.0f, Actor.Position.Y - 10.0f}, {20.0f, 20.0f}};
}

void prey_data::on_wander_enter() const
{
    Shape->Color = colors::CornflowerBlue;
}

void prey_data::on_wander_update(milliseconds dt)
{
    move(dt);
}

auto prey_data::on_wander_condition() const -> bool
{
    return Actor.Position.distance_to(Hunter->Actor.Position) < DETECTION_RANGE;
}

void prey_data::on_flee_enter() const
{
    Shape->Color = colors::White;
}

void prey_data::on_flee_update(milliseconds dt)
{
    point_f const fleeDir {point_f {Actor.Position - Hunter->Actor.Position}.as_normalized()};
    Actor.Velocity = fleeDir * PREY_FLEE_SPEED;
    move(dt);
}

auto prey_data::on_flee_condition() const -> bool
{
    return Actor.Position.distance_to(Hunter->Actor.Position) > DETECTION_RANGE * 1.5f;
}

// hunter_data member functions

void hunter_data::sync()
{
    Actor.Position.X   = std::clamp(Actor.Position.X, 15.0f, WindowSize.Width - 15.0f);
    Actor.Position.Y   = std::clamp(Actor.Position.Y, 15.0f, WindowSize.Height - 15.0f);
    Shape->Bounds      = {{Actor.Position.X - 15.0f, Actor.Position.Y - 15.0f}, {30.0f, 30.0f}};
    RangeShape->Center = Actor.Position;
}

void hunter_data::on_idle_enter()
{
    Shape->Color = colors::Green;
    Actor.Target.reset();
}

void hunter_data::on_idle_update(milliseconds dt)
{
    f32 const elapsed {static_cast<f32>(Behavior.time_in_state().count()) / 1000.0f};
    Actor.Position.X = Actor.Home.X + (std::cos(elapsed * 1.5f) * 150.0f);
    Actor.Position.Y = Actor.Home.Y + (std::sin(elapsed * 1.5f) * 150.0f);
    sync();
}

auto hunter_data::on_idle_hunt_condition() const -> bool
{
    return std::ranges::any_of(*Prey, [&](auto const& pd) {
        return pd->Actor.Alive && pd->Actor.Position.distance_to(Actor.Position) < DETECTION_RANGE;
    });
}

void hunter_data::on_idle_hunt_transition()
{
    for (auto const& p : *Prey) {
        if (p->Actor.Alive && p->Actor.Position.distance_to(Actor.Position) < DETECTION_RANGE) {
            Actor.Target = p;
            return;
        }
    }
}

void hunter_data::on_hunt_enter() const
{
    Shape->Color = colors::Red;
}

void hunter_data::on_hunt_update(milliseconds dt)
{
    if (!Actor.Target.expired()) {
        point_f const dir {point_f {Actor.Target.lock()->Actor.Position - Actor.Position}.as_normalized()};
        Actor.Position += dir * (HUNTER_SPEED * dts(dt));
    }
    sync();
}

auto hunter_data::on_hunt_return_condition() const -> bool
{
    if (Actor.Target.expired()) { return true; }
    auto const target {Actor.Target.lock()};
    if (!target->Actor.Alive) { return true; }
    f64 const dist {target->Actor.Position.distance_to(Actor.Position)};
    return dist < CATCH_RANGE
        || dist > DETECTION_RANGE * 2.0f
        || Behavior.time_in_state() > seconds {3};
}

void hunter_data::on_hunt_return_transition()
{
    if (Actor.Target.expired()) { return; }
    auto target {Actor.Target.lock()};
    if (target->Actor.Position.distance_to(Actor.Position) < CATCH_RANGE) {
        target->Actor.Alive = false;
        target->Shape->hide();
        Actor.CaughtCount++;
    }
    Actor.Target.reset();
}

void hunter_data::on_return_enter() const
{
    Shape->Color = colors::Yellow;
}

void hunter_data::on_return_update(milliseconds dt)
{
    point_f const circleStart {Actor.Home.X + 150.0f, Actor.Home.Y};
    point_f const dir {point_f {circleStart - Actor.Position}.as_normalized()};
    Actor.Position += dir * (HUNTER_SPEED * 0.75f * dts(dt));
    sync();
}

auto hunter_data::on_return_idle_condition() const -> bool
{
    point_f const circleStart {Actor.Home.X + 150.0f, Actor.Home.Y};
    return Actor.Position.distance_to(circleStart) < 5.0f;
}

void hunter_data::on_dead_enter() const
{
    Shape->Color      = colors::Black;
    RangeShape->Color = colors::Transparent;
}

auto hunter_data::on_dead_condition() const -> bool
{
    return Actor.CaughtCount == NUM_PREY;
}

// FSMEx

FSMEx::FSMEx(game& game)
    : scene {game}
{
}

FSMEx::~FSMEx() = default;

void FSMEx::on_start()
{
    _windowSize = size_f {*window().Size};
    setup_hunter();
    setup_prey();
}

void FSMEx::setup_prey()
{
    for (i32 i {0}; i < NUM_PREY; ++i) {
        auto pd {_preyData.emplace_back(std::make_shared<prey_data>())};
        pd->Actor.Position = {_rng(20.0f, _windowSize.Width - 20.0f),
                              _rng(20.0f, _windowSize.Height - 20.0f)};
        pd->Actor.Velocity = {_rng(50.f, 100.f) * (_rng(0, 1) ? 1.0f : -1.0f),
                              _rng(50.f, 100.f) * (_rng(0, 1) ? 1.0f : -1.0f)};
        pd->WindowSize     = _windowSize;
        pd->Hunter         = _hunterData.get();

        auto& shape {_batch.create_shape<gfx::rect_shape>()};
        shape.Bounds   = {{pd->Actor.Position.X - 10.0f, pd->Actor.Position.Y - 10.0f}, {20.0f, 20.0f}};
        shape.Color    = colors::CornflowerBlue;
        shape.Material = material::Empty();
        pd->Shape      = &shape;

        pd->Behavior.add_state({
            .ID          = PREY_WANDER,
            .OnEnter     = [](user_object& data) { data.get<prey_data>()->on_wander_enter(); },
            .OnUpdate    = [](user_object& data, milliseconds dt) { data.get<prey_data>()->on_wander_update(dt); },
            .Transitions = {{
                .TargetStateID = PREY_FLEE,
                .Condition     = [](user_object const& data) { return data.get<prey_data>()->on_wander_condition(); },
            }},
        });
        pd->Behavior.add_state({
            .ID          = PREY_FLEE,
            .OnEnter     = [](user_object& data) { data.get<prey_data>()->on_flee_enter(); },
            .OnUpdate    = [](user_object& data, milliseconds dt) { data.get<prey_data>()->on_flee_update(dt); },
            .Transitions = {{
                .TargetStateID = PREY_WANDER,
                .Condition     = [](user_object const& data) { return data.get<prey_data>()->on_flee_condition(); },
            }},
        });
        pd->Behavior.start(PREY_WANDER, user_object {pd});
    }
}

void FSMEx::setup_hunter()
{
    _hunterData                 = std::make_shared<hunter_data>();
    _hunterData->Actor.Position = {_windowSize.Width / 2.0f, _windowSize.Height / 2.0f};
    _hunterData->Actor.Home     = {_windowSize.Width / 2.0f, _windowSize.Height / 2.0f};
    _hunterData->Actor.Velocity = {120.0f, 100.0f};
    _hunterData->WindowSize     = _windowSize;
    _hunterData->Prey           = &_preyData;

    auto& hunterShape {_batch.create_shape<gfx::rect_shape>()};
    hunterShape.Bounds   = {{_hunterData->Actor.Position.X - 15.0f, _hunterData->Actor.Position.Y - 15.0f}, {30.0f, 30.0f}};
    hunterShape.Color    = colors::Orange;
    hunterShape.Material = material::Empty();
    _hunterData->Shape   = &hunterShape;

    auto& rangeShape {_batch.create_shape<gfx::circle_shape>()};
    rangeShape.Center       = hunterShape.Bounds->center();
    rangeShape.Radius       = DETECTION_RANGE;
    rangeShape.Color        = color {255, 165, 0, 30};
    rangeShape.Material     = material::Empty();
    _hunterData->RangeShape = &rangeShape;

    _hunterData->Behavior.add_state({
        .ID          = STATE_IDLE,
        .OnEnter     = [](user_object& data) { data.get<hunter_data>()->on_idle_enter(); },
        .OnUpdate    = [](user_object& data, milliseconds dt) { data.get<hunter_data>()->on_idle_update(dt); },
        .Transitions = {{
            .TargetStateID = STATE_HUNT,
            .Condition     = [](user_object const& data) { return data.get<hunter_data>()->on_idle_hunt_condition(); },
            .OnTransition  = [](user_object& data) { data.get<hunter_data>()->on_idle_hunt_transition(); },
        }},
    });

    _hunterData->Behavior.add_state({
        .ID          = STATE_HUNT,
        .OnEnter     = [](user_object& data) { data.get<hunter_data>()->on_hunt_enter(); },
        .OnUpdate    = [](user_object& data, milliseconds dt) { data.get<hunter_data>()->on_hunt_update(dt); },
        .Transitions = {{
            .TargetStateID = STATE_RETURN,
            .Condition     = [](user_object const& data) { return data.get<hunter_data>()->on_hunt_return_condition(); },
            .OnTransition  = [](user_object& data) { data.get<hunter_data>()->on_hunt_return_transition(); },
        }},
    });

    _hunterData->Behavior.add_state({
        .ID          = STATE_RETURN,
        .OnEnter     = [](user_object& data) { data.get<hunter_data>()->on_return_enter(); },
        .OnUpdate    = [](user_object& data, milliseconds dt) { data.get<hunter_data>()->on_return_update(dt); },
        .Transitions = {{
            .TargetStateID = STATE_IDLE,
            .Condition     = [](user_object const& data) { return data.get<hunter_data>()->on_return_idle_condition(); },
        }},
    });

    _hunterData->Behavior.add_state({
        .ID      = STATE_DEAD,
        .OnEnter = [](user_object& data) { data.get<hunter_data>()->on_dead_enter(); },
    });

    _hunterData->Behavior.add_global_transition({
        .TargetStateID = STATE_DEAD,
        .Condition     = [](user_object const& data) { return data.get<hunter_data>()->on_dead_condition(); },
    });

    _hunterData->Behavior.start(STATE_IDLE, user_object {_hunterData});
}

void FSMEx::on_fixed_update(milliseconds /* deltaTime */)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    window().Title = std::format("FSMEx | FPS avg:{:.2f} | State: {} Caught: {} / {}",
                                 stats.average_FPS(),
                                 STATES.at(_hunterData->Behavior.current_state()),
                                 _hunterData->Actor.CaughtCount,
                                 NUM_PREY);
}

void FSMEx::on_update(milliseconds deltaTime)
{
    for (auto& p : _preyData) {
        if (!p->Actor.Alive) { continue; }
        p->Behavior.update(deltaTime);
    }

    _hunterData->Behavior.update(deltaTime);
    _batch.update(deltaTime);
}

void FSMEx::on_draw_to(render_target& target, transform const& xform)
{
    _batch.draw_to(target, xform);
}

void FSMEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}
