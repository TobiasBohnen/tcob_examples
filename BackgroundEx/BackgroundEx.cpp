// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "BackgroundEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

BackgroundEx::BackgroundEx(game& game)
    : scene(game)
{
}

BackgroundEx::~BackgroundEx() = default;

void BackgroundEx::on_start()
{
    auto& resMgr {get_game().get_library()};
    auto& resGrp {resMgr.create_or_get_group("res")};

    auto shape0      = _layer0.create_shape<gfx::rect_shape>();
    shape0->Bounds   = {60, 60, 200, 60};
    shape0->Material = _material;
    _material->Color = colors::Gray;

    _back0               = std::make_shared<parallax_background>();
    _back0->Material     = resGrp.get<material>("parallax");
    _back0->TextureScale = {2, 2};
    _back0->add_layer({"para2", 0.10f});
    _back0->add_layer({"para1", 0.50f});
    _back0->add_layer({"para0", 1});

    _cam.LimitBounds = {point_f::Zero, size_f {_back0->Material->Texture->get_size().Width * _back0->TextureScale.Width * 10, 1}};
}

void BackgroundEx::on_draw_to(render_target& target)
{
    _back0->draw_to(target);
    _layer0.draw_to(target);
}

void BackgroundEx::on_update(milliseconds deltaTime)
{
    _layer0.update(deltaTime);
}

void BackgroundEx::on_fixed_update(milliseconds /* deltaTime */)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    get_window().Title = "TestGame " + stream.str();
}

void BackgroundEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    default:
        break;
    }

    _cam.on_key_down(ev);
}

void BackgroundEx::on_mouse_button_down(mouse::button_event const& ev)
{
    _cam.on_mouse_button_down(ev);
}

void BackgroundEx::on_mouse_button_up(mouse::button_event const& ev)
{
    _cam.on_mouse_button_up(ev);
}

void BackgroundEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _cam.on_mouse_motion(ev);
}

void BackgroundEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    //  _cam.on_mouse_wheel(ev);
}
