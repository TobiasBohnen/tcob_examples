// Copyright (c) 2026 Tobias Bohnen
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
    auto const& stats {locate_service<gfx::render_system>().statistics()};
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
        _polyShape->Color = {_polyShape->Color->R, _polyShape->Color->G, _polyShape->Color->B, 128};
        _cutShape->Color  = {_cutShape->Color->R, _cutShape->Color->G, _cutShape->Color->B, 128};

        std::array colors {colors::Red, colors::Blue, colors::Green, colors::Yellow};
        i32        i {0};
        for (auto const& poly : *_polyShape->Polygons) {
            auto& shape    = _layer0.create_shape<gfx::poly_shape>();
            shape.Material = material::Empty();
            shape.Color    = colors[i++ % colors.size()];
            shape.Polygons = {poly};
        }
        _layer0.bring_to_front(*_polyShape);
    }
    toggle = (toggle + 1) % 5;
}

void ClippingEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
}

void ClippingEx::create_shapes()
{
    _layer0.clear();

    auto& resMgr {library()};
    auto* resGrp {resMgr.get_group("res")};

    _polyShape           = &_layer0.create_shape<gfx::poly_shape>();
    _polyShape->Material = material::Empty();
    _polyShape->Color    = colors::Black;
    _polyShape->Polygons = {{
        .Outline = {{110, 110}, {300, 200}, {300, 400}, {110, 550}, {550, 550}, {700, 400}, {700, 200}, {550, 110}},
        .Holes   = {{{400, 200}, {600, 200}, {600, 400}, {400, 400}}},
    }};

    _cutShape           = &_layer0.create_shape<gfx::poly_shape>();
    _cutShape->Material = material::Empty();
    _cutShape->Color    = colors::White;

    _cutShape           = &_layer0.create_shape<gfx::poly_shape>();
    _cutShape->Material = material::Empty();
    _cutShape->Color    = colors::White;
    _cutShape->Polygons = {{
        .Outline = {{160, 150}, {160, 420}, {750, 420}, {720, 150}},
        .Holes   = {{{170, 160}, {710, 160}, {710, 410}, {170, 410}}},
    }};
}
