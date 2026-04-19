// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "UIEx.hpp"

#include "ExForms.hpp"

#include "ExStyle_Color.hpp"
#include "ExStyle_Gradient.hpp"
#include "ExStyle_Skin.hpp"

using namespace std::chrono_literals;

UIEx::UIEx(game& game)
    : scene {game}
{
    _modes = {
        &create_form0,
        &create_form1,
        &add_node_graph,
        &create_form_terminal,
        &create_form_charting,
        &create_form_displays,
        &create_form_tabcontainer,
        &create_form_accordion,
    };
}

UIEx::~UIEx() = default;

void UIEx::on_start()
{
    auto* resGrp {library().get_group("ui")};
    auto  defaultCursor {resGrp->get<cursor>("default")};
    window().Cursor           = defaultCursor;
    defaultCursor->ActiveMode = "default";

    auto form0 {add_node_graph(window(), *resGrp)};
    form0->Styles = create_color_styles(*resGrp);
    form0->Cursor = window().Cursor;
    // _form0->Bounds = rect_f {{300, 450}, size_f {*window().Size * 2}};
    // root_node().Scale = {0.5f, 0.5f};

    root_node().Entity = form0;
}

void UIEx::on_draw_to(render_target& target, transform const& xform)
{
}

void UIEx::on_update(milliseconds /* deltaTime */)
{
}

void UIEx::on_fixed_update(milliseconds deltaTime)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);
}

void UIEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    case scan_code::S: {
        auto img {window().copy_to_image()};
        auto _ = img.save("screen0.png");
    } break;
    case scan_code::R: {
        locate_service<gfx::render_system>().statistics().reset();
    } break;
    case scan_code::G: {
        std::dynamic_pointer_cast<form_base>(*root_node().Entity)->Shader = library().get_group("ui")->get<shader>("sepia");
    } break;
    case scan_code::V: {
        window().VSync = !window().VSync;
        locate_service<gfx::render_system>().statistics().reset();
    } break;
    case scan_code::T: {
        auto form0 {std::dynamic_pointer_cast<form_base>(*root_node().Entity)};
        switch (_switch) {
        case 0: form0->Styles = create_skinned_styles(*library().get_group("ui")); break;
        case 1: form0->Styles = create_gradient_styles(*library().get_group("ui")); break;
        case 2: form0->Styles = create_color_styles(*library().get_group("ui")); break;
        }
        _switch = (_switch + 1) % 3;
    } break;
    case scan_code::M: {
        _mode = (_mode + 1) % _modes.size();
        auto* resGrp {library().get_group("ui")};
        auto  form0 {_modes[_mode](window(), *resGrp)};
        form0->Styles      = create_color_styles(*resGrp);
        form0->Cursor      = window().Cursor;
        root_node().Entity = form0;
        _switch            = 0;
    } break;
    default:
        break;
    }
}

void UIEx::on_mouse_motion(mouse::motion_event const& /* ev */)
{
}
