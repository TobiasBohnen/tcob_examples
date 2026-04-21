// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "FSMEx.hpp"

FSMEx::FSMEx(game& game)
    : scene {game}
{
}

FSMEx::~FSMEx() = default;

constexpr uid                                STATE_IDLE {1};
constexpr uid                                STATE_HUNT {2};
constexpr uid                                STATE_RETURN {3};
constexpr uid                                STATE_DEAD {4};
static std::unordered_map<uid, string> const STATES {
    {INVALID_ID, "invalid"},
    {STATE_IDLE, "idle"},
    {STATE_HUNT, "hunting"},
    {STATE_RETURN, "returning"},
    {STATE_DEAD, "dead"},
};

constexpr uid PREY_WANDER {10};
constexpr uid PREY_FLEE {11};

constexpr f32 PREY_FLEE_SPEED {150.0f};

void FSMEx::on_start()
{
    _windowSize = size_f {*window().Size};

    // 1. Setup Prey
    for (i32 i {0}; i < NUM_PREY; ++i) {
        prey_data& p {_prey.emplace_back()};
        p.Position = {_rng(20.0f, _windowSize.Width - 20.0f), _rng(20.0f, _windowSize.Height - 20.0f)};
        p.Velocity = {_rng(50.f, 100.f) * (_rng(0, 1) ? 1.0f : -1.0f),
                      _rng(50.f, 100.f) * (_rng(0, 1) ? 1.0f : -1.0f)};

        auto& shape {_batch.create_shape<gfx::rect_shape>()};
        shape.Bounds   = {{p.Position.X - 10.0f, p.Position.Y - 10.0f}, {20.0f, 20.0f}};
        shape.Color    = colors::CornflowerBlue;
        shape.Material = material::Empty();
        _preyShapes.push_back(&shape);

        p.Behavior = std::make_shared<fsm>();

        // Individual Prey FSM
        // prey behaviors
        auto const preyWanderCondition {[this, i](user_object const&) -> bool {
            return _hunter && _prey[i].Position.distance_to(_hunter->Position) < DETECTION_RANGE;
        }};

        auto const preyFleeUpdate {[this, i](user_object&, milliseconds) {
            if (!_hunter) { return; }
            point_f const fleeDir {point_f {_prey[i].Position - _hunter->Position}.as_normalized()};
            _prey[i].Velocity = fleeDir * PREY_FLEE_SPEED;
        }};

        auto const preyFleeCondition {[this, i](user_object const&) -> bool {
            return !_hunter || _prey[i].Position.distance_to(_hunter->Position) > DETECTION_RANGE * 1.5f;
        }};

        p.Behavior->add_state({
            .ID          = PREY_WANDER,
            .OnEnter     = [this, i](user_object&) { _preyShapes[i]->Color = colors::CornflowerBlue; },
            .Transitions = {{.TargetStateID = PREY_FLEE, .Condition = preyWanderCondition}},
        });

        p.Behavior->add_state({
            .ID          = PREY_FLEE,
            .OnEnter     = [this, i](user_object&) { _preyShapes[i]->Color = colors::White; },
            .OnUpdate    = preyFleeUpdate,
            .Transitions = {{.TargetStateID = PREY_WANDER, .Condition = preyFleeCondition}},
        });

        p.Behavior->start(PREY_WANDER, user_object {});
    }

    _hunter           = std::make_shared<hunter_data>();
    _hunter->Position = {_windowSize.Width / 2.0f, _windowSize.Height / 2.0f};
    _hunter->Home     = {_windowSize.Width / 2.0f, _windowSize.Height / 2.0f};
    _hunter->Velocity = {120.0f, 100.0f};

    // 2. Setup Hunter Shapes
    auto& hunterShape {_batch.create_shape<gfx::rect_shape>()};
    hunterShape.Bounds   = {{_hunter->Position.X - 15.0f, _hunter->Position.Y - 15.0f}, {30.0f, 30.0f}};
    hunterShape.Color    = colors::Orange;
    hunterShape.Material = material::Empty();
    _hunterShape         = &hunterShape;

    auto& rangeShape {_batch.create_shape<gfx::circle_shape>()};
    rangeShape.Center   = hunterShape.Bounds->center();
    rangeShape.Radius   = DETECTION_RANGE;
    rangeShape.Color    = color {255, 165, 0, 30};
    rangeShape.Material = material::Empty();
    _rangeShape         = &rangeShape;

    // 3. Setup Hunter FSM
    auto const h {[](user_object& data) -> hunter_data& { return *data.get<hunter_data>(); }};
    auto const ch {[](user_object const& data) -> hunter_data const& { return *data.get<hunter_data>(); }};
    auto const dts {[](milliseconds dt) -> f32 { return static_cast<f32>(dt.count()) / 1000.0f; }};

    auto const idleEnter {[this, h](user_object& data) {
        _hunterShape->Color = colors::Green;
        h(data).Target      = nullptr;
    }};

    auto const idleUpdate {[this, h](user_object& data, milliseconds) {
        auto&     hd {h(data)};
        f32 const elapsed {static_cast<f32>(_hunterBehavior.time_in_state().count()) / 1000.0f};
        hd.Position.X = hd.Home.X + (std::cos(elapsed * 1.5f) * 150.0f);
        hd.Position.Y = hd.Home.Y + (std::sin(elapsed * 1.5f) * 150.0f);
    }};

    auto const idleCondition {[this, ch](user_object const& data) -> bool {
        auto const& hd {ch(data)};
        return std::ranges::any_of(_prey, [&](auto const& p) {
            return p.Alive && p.Position.distance_to(hd.Position) < DETECTION_RANGE;
        });
    }};

    auto const idleTransition {[this, h](user_object& data) {
        auto& hd {h(data)};
        for (auto& p : _prey) {
            if (p.Alive && p.Position.distance_to(hd.Position) < DETECTION_RANGE) {
                hd.Target = &p;
                return;
            }
        }
    }};

    _hunterBehavior.add_state({
        .ID          = STATE_IDLE,
        .OnEnter     = idleEnter,
        .OnUpdate    = idleUpdate,
        .Transitions = {{.TargetStateID = STATE_HUNT, .Condition = idleCondition, .OnTransition = idleTransition}},
    });

    auto const huntUpdate {[this, h, dts](user_object& data, milliseconds dt) {
        auto& hd {h(data)};
        if (!hd.Target) { return; }
        point_f const dir {point_f {hd.Target->Position - hd.Position}.as_normalized()};
        hd.Position += dir * (HUNTER_SPEED * dts(dt));
    }};

    auto const huntCondition {[this, ch](user_object const& data) -> bool {
        auto const& hd {ch(data)};
        if (!hd.Target || !hd.Target->Alive) { return true; }
        f64 const dist {hd.Target->Position.distance_to(hd.Position)};
        return dist < CATCH_RANGE
            || dist > DETECTION_RANGE * 2.0f
            || _hunterBehavior.time_in_state() > seconds {3};
    }};

    auto const huntTransition {[this, h](user_object& data) {
        auto& hd {h(data)};
        if (hd.Target && hd.Target->Position.distance_to(hd.Position) < CATCH_RANGE) {
            hd.Target->Alive = false;
            hd.CaughtCount++;
        }
        hd.Target = nullptr;
    }};

    _hunterBehavior.add_state({
        .ID          = STATE_HUNT,
        .OnEnter     = [this](user_object&) { _hunterShape->Color = colors::Red; },
        .OnUpdate    = huntUpdate,
        .Transitions = {{.TargetStateID = STATE_RETURN, .Condition = huntCondition, .OnTransition = huntTransition}},
    });

    auto const returnUpdate {[this, h, dts](user_object& data, milliseconds dt) {
        auto&         hd {h(data)};
        point_f const circleStart {hd.Home.X + 150.0f, hd.Home.Y};
        point_f const dir {point_f {circleStart - hd.Position}.as_normalized()};
        hd.Position += dir * (HUNTER_SPEED * 0.75f * dts(dt));
    }};

    auto const returnCondition {[ch](user_object const& data) -> bool {
        auto const&   hd {ch(data)};
        point_f const circleStart {hd.Home.X + 150.0f, hd.Home.Y};
        return hd.Position.distance_to(circleStart) < 5.0f;
    }};

    _hunterBehavior.add_state({
        .ID          = STATE_RETURN,
        .OnEnter     = [this](user_object&) { _hunterShape->Color = colors::Yellow; },
        .OnUpdate    = returnUpdate,
        .Transitions = {{.TargetStateID = STATE_IDLE, .Condition = returnCondition}},
    });

    auto const deadEnter {[this](user_object&) {
        _hunterShape->Color = colors::Black;
        _rangeShape->Color  = colors::Transparent;
    }};

    _hunterBehavior.add_state({
        .ID      = STATE_DEAD,
        .OnEnter = deadEnter,
    });

    auto const deadCondition {[](user_object const& data) -> bool {
        return data.get<hunter_data>()->CaughtCount == NUM_PREY;
    }};

    _hunterBehavior.add_global_transition({.TargetStateID = STATE_DEAD,
                                           .Condition     = deadCondition});

    _hunterBehavior.start(STATE_IDLE, user_object {_hunter});
}

void FSMEx::on_fixed_update(milliseconds /* deltaTime */)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    window().Title = std::format("FSMEx | FPS avg:{:.2f} | State: {} Caught: {} / {}",
                                 stats.average_FPS(),
                                 STATES.at(_hunterBehavior.current_state()),
                                 _hunter ? _hunter->CaughtCount : 0,
                                 NUM_PREY);
}

void FSMEx::on_update(milliseconds deltaTime)
{
    f32 const dts {static_cast<f32>(deltaTime.count()) / 1000.0f};

    for (usize i {0}; i < _prey.size(); ++i) {
        auto& p {_prey[i]};
        if (!p.Alive) {
            _preyShapes[i]->Color = colors::Transparent;
            continue;
        }

        p.Behavior->update(deltaTime);

        p.Position += p.Velocity * dts;

        if (p.Position.X < 10.0f) {
            p.Position.X = 10.0f;
            p.Velocity.X *= -1;
        } else if (p.Position.X > _windowSize.Width - 10.0f) {
            p.Position.X = _windowSize.Width - 10.0f;
            p.Velocity.X *= -1;
        }

        if (p.Position.Y < 10.0f) {
            p.Position.Y = 10.0f;
            p.Velocity.Y *= -1;
        } else if (p.Position.Y > _windowSize.Height - 10.0f) {
            p.Position.Y = _windowSize.Height - 10.0f;
            p.Velocity.Y *= -1;
        }

        _preyShapes[i]->Bounds = {{p.Position.X - 10.0f, p.Position.Y - 10.0f}, {20.0f, 20.0f}};
    }

    _hunterBehavior.update(deltaTime);

    if (auto* h {_hunterBehavior.data<hunter_data>()}) {
        h->Position.X = std::clamp(h->Position.X, 15.0f, _windowSize.Width - 15.0f);
        h->Position.Y = std::clamp(h->Position.Y, 15.0f, _windowSize.Height - 15.0f);

        _hunterShape->Bounds = {{h->Position.X - 15.0f, h->Position.Y - 15.0f}, {30.0f, 30.0f}};
        _rangeShape->Center  = h->Position;
    }

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
