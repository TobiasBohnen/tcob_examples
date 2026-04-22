#include "FSMEx.hpp"

FSMEx::FSMEx(game& game)
    : scene {game}
{
}

FSMEx::~FSMEx() = default;

constexpr uid STATE_IDLE {1};
constexpr uid STATE_HUNT {2};
constexpr uid STATE_RETURN {3};
constexpr uid STATE_DEAD {4};

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

    auto const dts {[](milliseconds dt) -> f32 {
        return static_cast<f32>(dt.count()) / 1000.0f;
    }};

    // prey accessors
    auto const p {[](user_object& data) -> prey& { return *data.get<prey>(); }};
    auto const cp {[](user_object const& data) -> prey const& { return *data.get<prey>(); }};

    // prey shared behaviors
    auto const preyMove {[this, p](user_object& data, milliseconds dt) {
        prey&     pr {p(data)};
        f32 const dts {static_cast<f32>(dt.count()) / 1000.0f};

        pr.Position += pr.Velocity * dts;

        if (pr.Position.X < 10.0f) {
            pr.Position.X = 10.0f;
            pr.Velocity.X *= -1;
        } else if (pr.Position.X > _windowSize.Width - 10.0f) {
            pr.Position.X = _windowSize.Width - 10.0f;
            pr.Velocity.X *= -1;
        }

        if (pr.Position.Y < 10.0f) {
            pr.Position.Y = 10.0f;
            pr.Velocity.Y *= -1;
        } else if (pr.Position.Y > _windowSize.Height - 10.0f) {
            pr.Position.Y = _windowSize.Height - 10.0f;
            pr.Velocity.Y *= -1;
        }

        pr.Shape->Bounds = {{pr.Position.X - 10.0f, pr.Position.Y - 10.0f}, {20.0f, 20.0f}};
    }};

    auto const preyWanderCondition {[this, cp](user_object const& data) -> bool {
        return _hunter && cp(data).Position.distance_to(_hunter->Position) < DETECTION_RANGE;
    }};

    auto const preyFleeUpdate {[this, p, preyMove](user_object& data, milliseconds dt) {
        if (!_hunter) { return; }
        prey&         pr {p(data)};
        point_f const fleeDir {point_f {pr.Position - _hunter->Position}.as_normalized()};
        pr.Velocity = fleeDir * PREY_FLEE_SPEED;
        preyMove(data, dt);
    }};

    auto const preyFleeCondition {[this, cp](user_object const& data) -> bool {
        return !_hunter || cp(data).Position.distance_to(_hunter->Position) > DETECTION_RANGE * 1.5f;
    }};

    // 1. setup prey
    for (i32 i {0}; i < NUM_PREY; ++i) {
        auto pr {_prey.emplace_back(std::make_shared<prey>())};
        pr->Position = {_rng(20.0f, _windowSize.Width - 20.0f),
                        _rng(20.0f, _windowSize.Height - 20.0f)};
        pr->Velocity = {_rng(50.f, 100.f) * (_rng(0, 1) ? 1.0f : -1.0f),
                        _rng(50.f, 100.f) * (_rng(0, 1) ? 1.0f : -1.0f)};

        auto& shape {_batch.create_shape<gfx::rect_shape>()};
        shape.Bounds   = {{pr->Position.X - 10.0f, pr->Position.Y - 10.0f}, {20.0f, 20.0f}};
        shape.Color    = colors::CornflowerBlue;
        shape.Material = material::Empty();
        pr->Shape      = &shape;

        pr->Behavior = std::make_shared<fsm>();

        pr->Behavior->add_state({
            .ID          = PREY_WANDER,
            .OnEnter     = [p](user_object& data) { p(data).Shape->Color = colors::CornflowerBlue; },
            .OnUpdate    = preyMove,
            .Transitions = {{.TargetStateID = PREY_FLEE, .Condition = preyWanderCondition}},
        });

        pr->Behavior->add_state({
            .ID          = PREY_FLEE,
            .OnEnter     = [p](user_object& data) { p(data).Shape->Color = colors::White; },
            .OnUpdate    = preyFleeUpdate,
            .Transitions = {{.TargetStateID = PREY_WANDER, .Condition = preyFleeCondition}},
        });

        pr->Behavior->start(PREY_WANDER, user_object {pr});
    }

    // 2. setup hunter
    _hunter           = std::make_shared<hunter>();
    _hunter->Position = {_windowSize.Width / 2.0f, _windowSize.Height / 2.0f};
    _hunter->Home     = {_windowSize.Width / 2.0f, _windowSize.Height / 2.0f};
    _hunter->Velocity = {120.0f, 100.0f};

    auto& hunterShape {_batch.create_shape<gfx::rect_shape>()};
    hunterShape.Bounds   = {{_hunter->Position.X - 15.0f, _hunter->Position.Y - 15.0f}, {30.0f, 30.0f}};
    hunterShape.Color    = colors::Orange;
    hunterShape.Material = material::Empty();
    _hunter->Shape       = &hunterShape;

    auto& rangeShape {_batch.create_shape<gfx::circle_shape>()};
    rangeShape.Center   = hunterShape.Bounds->center();
    rangeShape.Radius   = DETECTION_RANGE;
    rangeShape.Color    = color {255, 165, 0, 30};
    rangeShape.Material = material::Empty();
    _rangeShape         = &rangeShape;

    // hunter accessors
    auto const h {[](user_object& data) -> hunter& { return *data.get<hunter>(); }};
    auto const ch {[](user_object const& data) -> hunter const& { return *data.get<hunter>(); }};

    // hunter shared sync
    auto const hunterSync {[this, h](user_object& data, milliseconds) {
        auto& hd {h(data)};
        hd.Position.X       = std::clamp(hd.Position.X, 15.0f, _windowSize.Width - 15.0f);
        hd.Position.Y       = std::clamp(hd.Position.Y, 15.0f, _windowSize.Height - 15.0f);
        hd.Shape->Bounds    = {{hd.Position.X - 15.0f, hd.Position.Y - 15.0f}, {30.0f, 30.0f}};
        _rangeShape->Center = hd.Position;
    }};

    // 3. setup hunter FSM
    auto const idleEnter {[h](user_object& data) {
        auto& hd {h(data)};
        hd.Shape->Color = colors::Green;
        hd.Target.reset();
    }};

    auto const idleUpdate {[this, h, hunterSync](user_object& data, milliseconds dt) {
        auto&     hd {h(data)};
        f32 const elapsed {static_cast<f32>(hd.Behavior.time_in_state().count()) / 1000.0f};
        hd.Position.X = hd.Home.X + (std::cos(elapsed * 1.5f) * 150.0f);
        hd.Position.Y = hd.Home.Y + (std::sin(elapsed * 1.5f) * 150.0f);
        hunterSync(data, dt);
    }};

    auto const idleHuntCondition {[this, ch](user_object const& data) -> bool {
        auto const& hd {ch(data)};
        return std::ranges::any_of(_prey, [&](auto const& p) {
            return p->Alive && p->Position.distance_to(hd.Position) < DETECTION_RANGE;
        });
    }};

    auto const idleHuntTransition {[this, h](user_object& data) {
        auto& hd {h(data)};
        for (auto& p : _prey) {
            if (p->Alive && p->Position.distance_to(hd.Position) < DETECTION_RANGE) {
                hd.Target = p;
                return;
            }
        }
    }};

    _hunter->Behavior.add_state({
        .ID          = STATE_IDLE,
        .OnEnter     = idleEnter,
        .OnUpdate    = idleUpdate,
        .Transitions = {{
            .TargetStateID = STATE_HUNT,
            .Condition     = idleHuntCondition,
            .OnTransition  = idleHuntTransition,
        }},
    });

    auto const huntUpdate {[this, h, dts, hunterSync](user_object& data, milliseconds dt) {
        auto& hd {h(data)};
        if (!hd.Target.expired()) {
            point_f const dir {point_f {hd.Target.lock()->Position - hd.Position}.as_normalized()};
            hd.Position += dir * (HUNTER_SPEED * dts(dt));
        }
        hunterSync(data, dt);
    }};

    auto const huntReturnCondition {[this, ch](user_object const& data) -> bool {
        auto const& hd {ch(data)};
        if (hd.Target.expired()) { return true; }
        auto const target {hd.Target.lock()};
        if (!target->Alive) { return true; }
        f64 const dist {target->Position.distance_to(hd.Position)};
        return dist < CATCH_RANGE
            || dist > DETECTION_RANGE * 2.0f
            || hd.Behavior.time_in_state() > seconds {3};
    }};

    auto const huntReturnTransition {[h](user_object& data) {
        auto& hd {h(data)};
        if (hd.Target.expired()) { return; }
        auto target {hd.Target.lock()};
        if (target->Position.distance_to(hd.Position) < CATCH_RANGE) {
            target->Alive = false;
            target->Shape->hide();
            hd.CaughtCount++;
        }
        hd.Target.reset();
    }};

    _hunter->Behavior.add_state({
        .ID          = STATE_HUNT,
        .OnEnter     = [h](user_object& data) { h(data).Shape->Color = colors::Red; },
        .OnUpdate    = huntUpdate,
        .Transitions = {{
            .TargetStateID = STATE_RETURN,
            .Condition     = huntReturnCondition,
            .OnTransition  = huntReturnTransition,
        }},
    });

    auto const returnUpdate {[this, h, dts, hunterSync](user_object& data, milliseconds dt) {
        auto&         hd {h(data)};
        point_f const circleStart {hd.Home.X + 150.0f, hd.Home.Y};
        point_f const dir {point_f {circleStart - hd.Position}.as_normalized()};
        hd.Position += dir * (HUNTER_SPEED * 0.75f * dts(dt));
        hunterSync(data, dt);
    }};

    auto const returnIdleCondition {[ch](user_object const& data) -> bool {
        auto const&   hd {ch(data)};
        point_f const circleStart {hd.Home.X + 150.0f, hd.Home.Y};
        return hd.Position.distance_to(circleStart) < 5.0f;
    }};

    _hunter->Behavior.add_state({
        .ID          = STATE_RETURN,
        .OnEnter     = [h](user_object& data) { h(data).Shape->Color = colors::Yellow; },
        .OnUpdate    = returnUpdate,
        .Transitions = {{.TargetStateID = STATE_IDLE, .Condition = returnIdleCondition}},
    });

    _hunter->Behavior.add_state({
        .ID      = STATE_DEAD,
        .OnEnter = [this, h](user_object& data) {
            h(data).Shape->Color = colors::Black;
            _rangeShape->Color   = colors::Transparent;
        },
    });

    _hunter->Behavior.add_global_transition({
        .TargetStateID = STATE_DEAD,
        .Condition     = [](user_object const& data) -> bool {
            return data.get<hunter>()->CaughtCount == NUM_PREY;
        },
    });

    _hunter->Behavior.start(STATE_IDLE, user_object {_hunter});
}

void FSMEx::on_fixed_update(milliseconds /* deltaTime */)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    window().Title = std::format("FSMEx | FPS avg:{:.2f} | State: {} Caught: {} / {}",
                                 stats.average_FPS(),
                                 STATES.at(_hunter->Behavior.current_state()),
                                 _hunter->CaughtCount,
                                 NUM_PREY);
}

void FSMEx::on_update(milliseconds deltaTime)
{
    for (auto& p : _prey) {
        if (!p->Alive) { continue; }
        p->Behavior->update(deltaTime);
    }

    _hunter->Behavior.update(deltaTime);
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
