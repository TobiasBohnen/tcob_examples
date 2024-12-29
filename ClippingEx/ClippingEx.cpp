// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ClippingEx.hpp"

#include <iomanip>
#include <iostream>

ClippingEx::ClippingEx(game& game)
    : scene {game}
{
}

ClippingEx::~ClippingEx() = default;

void ClippingEx::on_fixed_update(milliseconds /* deltaTime */)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().stats()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void ClippingEx::on_start()
{
    create_shapes();
}

void ClippingEx::on_update(milliseconds deltaTime)
{
    _layer0.update(deltaTime);
}

void ClippingEx::on_draw_to(render_target& target)
{
    _layer0.draw_to(target);
}

void ClippingEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::R: {
        auto _ = window().copy_to_image().save("screen01.webp");
    } break;
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void ClippingEx::on_mouse_motion(mouse::motion_event const& ev)
{
}

void ClippingEx::on_mouse_button_down(mouse::button_event const& ev)
{
    static i32 toggle {0};
    create_shapes();
    if (toggle < 4) {
        _polyShape->clip(*_cutShape, static_cast<clip_mode>(toggle));
        _layer0.remove_shape(*_cutShape);
        _layer0.remove_shape(*_polyShape);
        std::array colors {colors::Red, colors::Blue, colors::Green, colors::Yellow, colors::Brown, colors::DodgerBlue};
        i32        i {0};
        for (auto const& poly : _polyShape->Polygons()) {
            auto shape      = _layer0.create_shape<gfx::poly_shape>();
            shape->Material = material::Empty();
            shape->Color    = colors[i++ % colors.size()];
            shape->Polygons = {poly};
        }
    }
    toggle = (toggle + 1) % 5;
}

void ClippingEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
}

void ClippingEx::create_shapes()
{
    _layer0.clear();

    auto& resMgr {parent().library()};
    auto* resGrp {resMgr.get_group("res")};

    _polyShape           = _layer0.create_shape<gfx::poly_shape>();
    _polyShape->Material = material::Empty();
    _polyShape->Color    = colors::Blue;
    _polyShape->Polygons = {
        {.Outline = {{10, 10}, {200, 100}, {200, 300}, {10, 450}, {450, 450}, {600, 300}, {600, 100}, {450, 10}},
         .Holes   = {{{300, 100}, {500, 100}, {500, 300}, {300, 300}}}}};

    _cutShape           = _layer0.create_shape<gfx::poly_shape>();
    _cutShape->Material = material::Empty();
    _cutShape->Color    = colors::Red;
    _cutShape->Polygons = {
        {.Outline = {{60, 50}, {60, 320}, {650, 320}, {620, 50}},
         .Holes   = {{{70, 60}, {610, 60}, {610, 310}, {70, 310}}}}};
}
