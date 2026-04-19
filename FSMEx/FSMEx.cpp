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

constexpr uid STATE_IDLE {1};
constexpr uid STATE_HUNT {2};
constexpr uid STATE_RETURN {3};

void FSMEx::on_start()
{
    _windowSize = size_f {*window().Size};

    // spawn prey
    for (i32 i {0}; i < NUM_PREY; ++i) {
        _prey.push_back({
            .Position = {
                static_cast<f32>((std::rand() % static_cast<i32>(_windowSize.Width)) + 50),
                static_cast<f32>((std::rand() % static_cast<i32>(_windowSize.Height)) + 50)},
            .Velocity = {static_cast<f32>(std::rand() % 200) - 100.0f, static_cast<f32>(std::rand() % 200) - 100.0f},
        });
    }

    _fsm.add_state({
        .ID          = STATE_IDLE,
        .OnEnter     = [](user_object& data) { data.get<hunter_data>()->Target = std::nullopt; },
        .OnTick      = [this](user_object& data, milliseconds dt) {
            auto*     h   {data.get<hunter_data>()};
            f32 const dts {static_cast<f32>(dt.count()) / 1000.0f};
            h->Position   += h->Velocity * (dts * 0.2f);
            h->Position.X  = std::clamp(h->Position.X, 0.0f, _windowSize.Width);
            h->Position.Y  = std::clamp(h->Position.Y, 0.0f, _windowSize.Height); },
        .Transitions = {{
            .TargetStateID = STATE_HUNT,
            .Condition     = [this](user_object const& data) -> bool {
                auto const* h {data.get<hunter_data>()};
                return std::ranges::any_of(_prey, [&](auto const& p) { return p.Alive && p.Position.distance_to(h->Position) < DETECTION_RANGE; });
            },
            .OnTransition = [this](user_object& data) {
                auto* h {data.get<hunter_data>()};
                for (usize i {0}; i < _prey.size(); ++i) {
                    if (_prey[i].Alive && _prey[i].Position.distance_to(h->Position) < DETECTION_RANGE) {
                        h->Target = i;
                        return;
                    }
                } },
        }},
    });

    _fsm.add_state({
        .ID          = STATE_HUNT,
        .OnTick      = [this](user_object& data, milliseconds dt) {
            auto* h {data.get<hunter_data>()};
            if (!h->Target) { return; }
            f32 const     dts {static_cast<f32>(dt.count()) / 1000.0f};
            point_f const dir {point_f {_prey[*h->Target].Position - h->Position}.as_normalized()};
            h->Position += dir * (HUNTER_SPEED * dts); },
        .Transitions = {{
            .TargetStateID = STATE_RETURN,
            .Condition     = [this](user_object const& data) -> bool {
                auto const* h {data.get<hunter_data>()};
                if (!h->Target) { return true; }

                auto const& p {_prey[*h->Target]};
                if (!p.Alive) { return true; }

                f64 const dist {p.Position.distance_to(h->Position)};
                if (dist < CATCH_RANGE) { return true; }
                if (dist > DETECTION_RANGE * 2.0f) { return true; }

                return false;
            },
            .OnTransition = [this](user_object& data) {    
                auto* h {data.get<hunter_data>()};
                if (!h->Target) { return; }
                auto& p {_prey[*h->Target]};

                if (p.Position.distance_to(h->Position) < CATCH_RANGE) {
                    p.Alive = false;
                    h->CaughtCount++;
                }
                h->Target = std::nullopt; },
        }},
    });

    _fsm.add_state({
        .ID     = STATE_RETURN,
        .OnTick = [](user_object& data, milliseconds dt) {
            auto*         h {data.get<hunter_data>()};
            f32 const     dts {static_cast<f32>(dt.count()) / 1000.0f};
            point_f const dir {point_f {h->Home - h->Position}.as_normalized()};
            h->Position += dir * (HUNTER_SPEED * 0.5f * dts);
        },
        .Transitions = {{
            .TargetStateID = STATE_IDLE,
            .Condition     = [](user_object const& data) -> bool {
                auto const* h {data.get<hunter_data>()};
                return h->Home.distance_to(h->Position) < 10.0;
            },
        }},
    });

    hunter_data hd;
    hd.Position = {_windowSize.Width / 2.0f, _windowSize.Height / 2.0f};
    hd.Home     = {_windowSize.Width / 2.0f, _windowSize.Height / 2.0f};
    hd.Velocity = {80.0f, 60.0f};
    _fsm.start(STATE_IDLE, user_object {hd});

    // create prey shapes
    for (auto const& p : _prey) {
        auto& shape {_batch.create_shape<gfx::rect_shape>()};
        shape.Bounds   = {{p.Position.X - 10.0f, p.Position.Y - 10.0f}, {20.0f, 20.0f}};
        shape.Color    = colors::CornflowerBlue;
        shape.Material = material::Empty();
        _preyShapes.push_back(&shape);
    }

    // create hunter shape
    auto& hunterShape {_batch.create_shape<gfx::rect_shape>()};
    hunterShape.Bounds   = {{hd.Position.X - 15.0f, hd.Position.Y - 15.0f}, {30.0f, 30.0f}};
    hunterShape.Color    = colors::Orange;
    hunterShape.Material = material::Empty();
    _hunterShape         = &hunterShape;

    // create range indicator
    auto& rangeShape {_batch.create_shape<gfx::circle_shape>()};
    rangeShape.Center   = hunterShape.Bounds->center();
    rangeShape.Radius   = DETECTION_RANGE;
    rangeShape.Color    = color {255, 165, 0, 30};
    rangeShape.Material = material::Empty();
    _rangeShape         = &rangeShape;
}

void FSMEx::on_fixed_update(milliseconds /* deltaTime */)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    window().Title = std::format("FSMEx | FPS avg:{:.2f} | State:{} Caught:{}",
                                 stats.average_FPS(),
                                 _fsm.current_state(),
                                 _fsm.data<hunter_data>() ? _fsm.data<hunter_data>()->CaughtCount : 0);
}

void FSMEx::on_update(milliseconds deltaTime)
{
    f32 const dts {static_cast<f32>(deltaTime.count()) / 1000.0f};

    for (usize i {0}; i < _prey.size(); ++i) {
        auto& p {_prey[i]};
        if (!p.Alive) {
            _preyShapes[i]->Color = colors::Transparent; // hide
            continue;
        }
        p.Position += p.Velocity * dts;
        if (p.Position.X < 10.0f || p.Position.X > _windowSize.Width - 10.0f) { p.Velocity.X = -p.Velocity.X; }
        if (p.Position.Y < 10.0f || p.Position.Y > _windowSize.Height - 10.0f) { p.Velocity.Y = -p.Velocity.Y; }
        _preyShapes[i]->Bounds = {{p.Position.X - 10.0f, p.Position.Y - 10.0f}, {20.0f, 20.0f}};
    }

    _fsm.tick(deltaTime);

    if (auto const* h {_fsm.data<hunter_data>()}) {
        _hunterShape->Bounds = {{h->Position.X - 15.0f, h->Position.Y - 15.0f}, {30.0f, 30.0f}};
        switch (_fsm.current_state()) {
        case STATE_HUNT:
            _hunterShape->Color = colors::Red;
            break;
        case STATE_IDLE:
            _hunterShape->Color = colors::Green;
            break;
        case STATE_RETURN:
            _hunterShape->Color = colors::Orange;
            break;
        }
        _rangeShape->Center = h->Position;
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
