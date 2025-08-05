// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "StencilEx.hpp"

#include <iomanip>
#include <iostream>

StencilEx::StencilEx(game& game)
    : scene {game}
{
}

StencilEx::~StencilEx() = default;

void StencilEx::on_fixed_update(milliseconds /* deltaTime */)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void StencilEx::on_start()
{
    _tween.Function.Min = -250;
    _tween.Function.Max = 250;
    _tween.start(playback_mode::AlternatedLooped);
    create_shapes();
}

void StencilEx::on_update(milliseconds deltaTime)
{
    _tween.update(deltaTime);
    update_shapes();
    _layer0.update(deltaTime);
}

void StencilEx::on_draw_to(render_target& target)
{
    _layer0.draw_to(target);
}

void StencilEx::on_key_down(keyboard::event const& ev)
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

void StencilEx::on_mouse_motion(mouse::motion_event const& ev)
{
}

void StencilEx::on_mouse_button_down(mouse::button_event const& ev)
{
}

void StencilEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
}

void StencilEx::create_shapes()
{
    _layer0.clear();

    _back           = _layer0.create_shape<gfx::rect_shape>();
    _back->Material = _mat1;
    _back->Color    = colors::Blue;
    _back->Bounds   = {0, _tween.Value, 500, 300};

    _wall                        = _layer0.create_shape<gfx::poly_shape>();
    _wall->Material              = _mat0;
    _wall->Material->StencilFunc = stencil_func::Always;
    _wall->Material->StencilOp   = stencil_op::Replace;
    _wall->Material->StencilRef  = 1;
    _wall->Color                 = colors::Green;
    _wall->Polygons              = {{
                     .Outline = {{350, 60}, {420, 200}, {580, 200}, {460, 280}, {500, 420}, {350, 340}, {200, 420}, {240, 280}, {120, 200}, {280, 200}},
                     .Holes   = {},
    }};

    _front                        = _layer0.create_shape<gfx::rect_shape>();
    _front->Material              = _mat2;
    _front->Material->StencilFunc = stencil_func::Equal;
    _front->Material->StencilOp   = stencil_op::Keep;
    _front->Material->StencilRef  = 1;
    _front->Color                 = colors::Red;
    _front->Bounds                = {0, _tween.Value, 500, 300};
}

void StencilEx::update_shapes()
{
    _back->Bounds  = {{50, _tween.Value}, _back->Bounds->Size};
    _front->Bounds = *_back->Bounds;
}
