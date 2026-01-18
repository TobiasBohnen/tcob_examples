// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ControllerEx.hpp"

#include "../_common/Trim.hpp"

using namespace std::chrono_literals;

ControllerEx::ControllerEx(game& game)
    : scene {game}
{
}

ControllerEx::~ControllerEx() = default;

void ControllerEx::on_start()
{
    if (locate_service<input::system>().controllers().empty()) { std::terminate(); }
    _form0                    = std::make_shared<crtl_form>(window());
    _form0->Controller->Label = locate_service<input::system>().first_controller().name();
    root_node().Entity        = _form0;
}

void ControllerEx::on_draw_to(render_target& target)
{
}

void ControllerEx::on_update(milliseconds deltaTime)
{
}

void ControllerEx::on_fixed_update(milliseconds deltaTime)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);
}

void ControllerEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void ControllerEx::on_mouse_motion(mouse::motion_event const& ev)
{
}
