// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "FSMEx.hpp"

////////////////////////////////////////////////////////////

FSMEx::FSMEx(game& game)
    : scene {game}
{
}
FSMEx::~FSMEx() = default;

void FSMEx::on_start()
{
    _sim = std::make_shared<sim_manager>(size_f {*window().Size});

    setup_hunter();
    setup_prey();
}

void FSMEx::setup_prey()
{
    size_f const windowSize {size_f {*window().Size}};

    for (i32 i {0}; i < NUM_PREY; ++i) {
        point_f const pos {
            _rng(20.0f, windowSize.Width - 20.0f),
            _rng(20.0f, windowSize.Height - 20.0f)};
        point_f const vel {
            _rng(50.f, 100.f) * (_rng(0, 1) ? 1.0f : -1.0f),
            _rng(50.f, 100.f) * (_rng(0, 1) ? 1.0f : -1.0f)};

        auto const p {_sim->spawn_prey(pos, vel)};
    }
}

void FSMEx::setup_hunter()
{
    size_f const windowSize {size_f {*window().Size}};

    point_f const startPos {windowSize.Width / 2.0f, windowSize.Height / 2.0f};
    _sim->spawn_hunter(startPos);
}

void FSMEx::on_update(milliseconds deltaTime)
{
    _sim->update(deltaTime);
}

void FSMEx::on_fixed_update(milliseconds /* deltaTime */)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    window().Title = std::format("FSMEx | FPS avg:{:.2f} | State: {} Caught: {} / {}",
                                 stats.average_FPS(),
                                 _sim->get_state_name(_sim->get_hunter()->Behavior.current_state()),
                                 _sim->get_hunter()->CaughtCount,
                                 NUM_PREY);
}

void FSMEx::on_draw_to(render_target& target, transform const& xform)
{
    _sim->draw_to(target, xform);
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
