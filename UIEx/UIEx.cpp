// Copyright (c) 2025 Tobias Bohnen
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
    auto* resGrp {library().get_group("ui")};
    auto  defaultCursor {resGrp->get<cursor>("default")};
    window().Cursor           = defaultCursor;
    defaultCursor->ActiveMode = "default";

    // _form0 = create_form0(&window(), *resGrp);
    // _form0 = create_form1(&window());
    _form0 = create_form_displays(&window());
    // _form0 = create_form_tabcontainer(&window(), *resGrp);
    //_form0 = create_form_accordion(&window(), *resGrp);
    //_form0 = create_form_terminal(&window());

    _form0->Styles = create_color_styles(*resGrp);
    // _form0->Bounds = rect_f {{300, 450}, size_f {*window().Size * 2}};
    //_form0->Scale  = {0.5f, 0.5f};

    _switch             = false;
    root_node()->Entity = _form0;
}

void UIEx::on_draw_to(render_target& target)
{
}

void UIEx::on_update(milliseconds /* deltaTime */)
{
}

void UIEx::on_fixed_update(milliseconds deltaTime)
{
    scene::on_fixed_update(deltaTime);

    std::stringstream stream;
    stream << std::fixed << std::setprecision(0) << std::setfill('0');
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    auto const pos {locate_service<input::system>().mouse()->get_position()};
    stream << " | " << pos.X << ":" << pos.Y;
    window().Title = "TestGame " + stream.str();
}

void UIEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    case scan_code::F: {
        {
            data::object obj;
            _form0->submit(obj);
            obj.save("form0.json");
        }
        {
            data::object obj;
            std::dynamic_pointer_cast<panel>(_form0->find_widget_by_name("Panel0"))->submit(obj);
            obj.save("Panel0.json");
        }
    } break;
    case scan_code::S: {
        auto img {window().copy_to_image()};
        auto _ = img.save("screen0.png");
    } break;
    case scan_code::R: {
        locate_service<gfx::render_system>().statistics().reset();
    } break;
    case scan_code::G: {
        _form0->Shader = library().get_group("ui")->get<shader>("sepia");
    } break;
    case scan_code::V: {
        window().VSync = !window().VSync;
    } break;
    case scan_code::T: {
        _switch = !_switch;
        if (_switch) {
            _form0->Styles = create_skinned_styles(*library().get_group("ui"));
        } else {
            _form0->Styles = create_color_styles(*library().get_group("ui"));
        }
    } break;
    default:
        break;
    }
}

void UIEx::on_mouse_motion(mouse::motion_event const& /* ev */)
{
}
