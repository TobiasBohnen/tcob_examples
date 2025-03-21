// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ControllerEx.hpp"

#include "../_common/Trim.hpp"

#include <iomanip>

using namespace std::chrono_literals;

ControllerEx::ControllerEx(game& game)
    : scene(game)
    , _form0 {std::make_shared<crtl_form>(window())}
{
}

ControllerEx::~ControllerEx() = default;

void ControllerEx::on_start()
{
    if (locate_service<input::system>().controllers().empty()) { std::terminate(); }
    _form0->Controller->Label = locate_service<input::system>().first_controller().name();
    root_node()->Entity       = _form0;
}

void ControllerEx::on_draw_to(render_target& target)
{
}

void ControllerEx::on_update(milliseconds deltaTime)
{
}

void ControllerEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().stats()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();
    stream << " input mode:" << static_cast<i32>(locate_service<input::system>().InputMode());

    window().Title = "ControllerEx " + stream.str();
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
