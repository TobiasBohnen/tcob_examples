// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "BackgroundEx.hpp"

using namespace std::chrono_literals;

BackgroundEx::BackgroundEx(game& game)
    : scene {game}
{
}

BackgroundEx::~BackgroundEx() = default;

void BackgroundEx::on_start()
{
    auto& resMgr {library()};
    auto& resGrp {resMgr.create_or_get_group("res")};

    _material->first_pass().Color = colors::Gray;

    _back0               = std::make_shared<parallax_background>();
    _back0->Material     = resGrp.get<material>("parallax");
    _back0->TextureScale = {6, 6};

    auto& back {_back0->create_layer()};
    back.TextureRegion = "back";
    back.ScrollScale   = {0.50f, 0.0f};

    auto& mid {_back0->create_layer()};
    mid.TextureRegion = "middle";
    mid.ScrollScale   = {1.00f, 0.0f};

    auto& front {_back0->create_layer()};
    front.TextureRegion = "front";
    front.ScrollScale   = {4.00f, 0.0f};
    front.Offset        = {0.25, 0.0f};

    _cam.LimitBounds = {point_f::Zero, size_f {_back0->Material->first_pass().Texture->info().Size.Width * _back0->TextureScale.Width * 10, 1}};
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
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);
}

void BackgroundEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
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
