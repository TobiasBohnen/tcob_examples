﻿// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ControllerEx.hpp"
#include <cstring>
#include <iomanip>

static char const* fontTff {
#include "../_common/trim.txt"
};

using namespace std::chrono_literals;

ControllerEx::ControllerEx(game& game)
    : scene(game)
{
}

ControllerEx::~ControllerEx() = default;

void ControllerEx::on_start()
{
    if (_input.get_controller_count() == 0) {
        get_game().pop_current_scene();
        return;
    }

    std::span<ubyte const> fontSpan {reinterpret_cast<ubyte const*>(fontTff), strlen(fontTff)};
    auto                   fontData {*io::z85_filter {}.from(fontSpan)};
    io::isstream           fontStream {fontData};
    [[maybe_unused]] auto  _ = _font->load(fontStream, 36);

    _text         = std::make_shared<text>(_font);
    _text->Text   = "1: High freq \n"
                    "2: Low freq \n"
                    "3: both \n";
    _text->Bounds = {{30, 60}, {450, 800}};

    _controllerDesc         = std::make_shared<text>(_font);
    _controllerDesc->Bounds = {{500, 60}, {750, 450}};
    _controllerDesc->Pivot  = {{0, 0}};

    std::stringstream stream;
    stream << "Controller count: " << _input.get_controller_count() << "\n";
    stream << "Name of controller 0:" << _input.get_controller(0)->get_name();
    _controllerDesc->Text = stream.str();

    _controllerDesc->Scale = {0.5f, 0.5f};

    _button         = std::make_shared<text>(_font);
    _button->Bounds = {{330.0, 120}, {450, 1050}};

    _laxisx         = std::make_shared<text>(_font);
    _laxisx->Bounds = {{30, 240}, {450, 1050}};

    _laxisy         = std::make_shared<text>(_font);
    _laxisy->Bounds = {{30, 270}, {450, 1050}};

    _raxisx         = std::make_shared<text>(_font);
    _raxisx->Bounds = {{30, 300}, {450, 1050}};

    _raxisy         = std::make_shared<text>(_font);
    _raxisy->Bounds = {{30, 330}, {450, 1050}};

    _axis         = std::make_shared<text>(_font);
    _axis->Bounds = {{30, 360}, {450, 1050}};
}

void ControllerEx::on_draw_to(render_target& target)
{
    _text->draw_to(target);
    _controllerDesc->draw_to(target);
    _button->draw_to(target);

    _laxisx->draw_to(target);
    _laxisy->draw_to(target);
    _raxisx->draw_to(target);
    _raxisy->draw_to(target);
    _axis->draw_to(target);
}

void ControllerEx::on_update(milliseconds deltaTime)
{
    _text->update(deltaTime);
    _controllerDesc->update(deltaTime);
    _button->update(deltaTime);

    _laxisx->update(deltaTime);
    _laxisy->update(deltaTime);
    _raxisx->update(deltaTime);
    _raxisy->update(deltaTime);
    _axis->update(deltaTime);
}

void ControllerEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();
    stream << " input mode:" << static_cast<i32>(locate_service<input::system>().CurrentInputMode());

    get_window().Title = "TestGame " + stream.str();
}

void ControllerEx::on_key_down(keyboard::event& ev)
{
    auto& input {locate_service<input::system>()};
    switch (ev.ScanCode) {
    case scan_code::D1:
        input.get_controller(0)->rumble(0, 0xFFFF, 1s);
        break;
    case scan_code::D2:
        input.get_controller(0)->rumble(0xFFFF, 0, 1s);
        break;
    case scan_code::D3:
        input.get_controller(0)->rumble(0xFFFF, 0xFFFF, 1s);
        break;
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    default:
        break;
    }
}

void ControllerEx::on_mouse_motion(mouse::motion_event& ev)
{
}
void ControllerEx::on_controller_axis_motion(controller::axis_event& ev)
{
    text* text = nullptr;
    switch (ev.Axis) {
    case controller::axis::LeftX:
        text = _laxisx.get();
        break;
    case controller::axis::LeftY:
        text = _laxisy.get();
        break;
    case controller::axis::RightX:
        text = _raxisx.get();
        break;
    case controller::axis::RightY:
        text = _raxisy.get();
        break;
    default:
        text = _axis.get();
        break;
    }
    text->Text = _input.get_controller(0)->get_axis_name(ev.Axis) + " Value: " + std::to_string(ev.RelativeValue);
}

void ControllerEx::on_controller_button_down(controller::button_event& ev)
{
    _button->Text = "button down: " + _input.get_controller(0)->get_button_name(ev.Button);
}

void ControllerEx::on_controller_button_up(controller::button_event& ev)
{
    _button->Text = "";
}
