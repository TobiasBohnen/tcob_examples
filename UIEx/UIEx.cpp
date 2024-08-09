﻿// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "UIEx.hpp"

#include <iomanip>
#include <iostream>

#include "ExForms.hpp"

#include "ExStyle_Color.hpp"
#include "ExStyle_Skin.hpp"

using namespace std::chrono_literals;

UIEx::UIEx(game& game)
    : scene(game)
{
}

UIEx::~UIEx() = default;

void UIEx::on_start()
{
    auto* resGrp {get_game().get_library().get_group("ui")};
    auto  defaultCursor {resGrp->get<cursor>("default")};
    get_window().Cursor       = defaultCursor;
    defaultCursor->ActiveMode = "default";

    _form0         = create_form0(&get_window(), *resGrp);
    _form0->Styles = create_color_styles(*resGrp);
    // _form0->Bounds = rect_f {{300, 450}, size_f {get_window().Size() * 2}};
    //_form0->Scale  = {0.5f, 0.5f};

    _switch = false;
    get_root_node()->attach_entity(_form0);
}

void UIEx::on_draw_to(render_target& target)
{
    if (_form0) {
        _form0->draw_to(target);
    }
}

void UIEx::on_update(milliseconds /* deltaTime */)
{
}

void UIEx::on_fixed_update(milliseconds deltaTime)
{
    scene::on_fixed_update(deltaTime);

    std::stringstream stream;
    stream << std::fixed << std::setprecision(0) << std::setfill('0');
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    stream << " | " << input::system::GetMousePosition();
    get_window().Title = "TestGame " + stream.str();
}

void UIEx::on_key_down(keyboard::event& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    case scan_code::D: {
        if (_form0->find_widget_by_name("Panel0")->is_enabled()) {
            _form0->find_widget_by_name("Panel0")->disable();
        } else {
            _form0->find_widget_by_name("Panel0")->enable();
        }
    } break;
    case scan_code::F: {
        data::config::object obj;
        _form0->submit(obj);
        obj.save("form0.ini");
    } break;
    case scan_code::S: {
        auto img {get_window().copy_to_image()};
        auto _ = img.save("screen0.png");
    } break;
    case scan_code::R: {
        locate_service<gfx::render_system>().get_stats().reset();
    } break;
    case scan_code::V: {
        get_window().VSync = !get_window().VSync;
    } break;
    case scan_code::T: {
        _switch = !_switch;
        if (_switch) {
            _form0->Styles = create_skinned_styles(*get_game().get_library().get_group("ui"));
        } else {
            _form0->Styles = create_color_styles(*get_game().get_library().get_group("ui"));
        }
    } break;
    default:
        break;
    }
}

void UIEx::on_mouse_motion(mouse::motion_event& /* ev */)
{
}
