// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "LightingSystemEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

LightingSystemEx::LightingSystemEx(game& game)
    : scene(game)
{
}

LightingSystemEx::~LightingSystemEx() = default;

void LightingSystemEx::on_start()
{
    get_window().ClearColor = colors::Black;

    using namespace tcob::literals;

    _layer1 = std::make_shared<shape_batch>();

    _lightingSystem0.Bounds = {point_f::Zero, size_f {get_window().Size()}};

    _lightSource0        = _lightingSystem0.create_light_source();
    _lightSource0->Range = 100;
    _lightSource0->Color = {255, 255, 0, 128};

    auto const createSC {[&](std::vector<point_f> const& points) {
        auto shape0 {_layer1->create_shape<gfx::poly_shape>()};
        shape0->Material = _material;
        shape0->Color    = colors::Blue;
        shape0->Polygon  = points;

        auto sc0 {_lightingSystem0.create_shadow_caster()};
        sc0->Polygon = points;
        sc0->Hit.connect([=](auto const& ev) {
            if (ev.Source->Falloff && ev.Source->Range()) {
                if (ev.Distance < *ev.Source->Range() / 2) {
                    shape0->Color = colors::Green;
                }
            } else {
                shape0->Color = colors::Red;
            }
        });
    }};

    createSC({{100.0f, 50.0f}, {125.0f, 80.0f}, {125.0f, 125.0f}, {100.0f, 200.0f}, {200.0f, 200.0f}, {250.0f, 150.0f}, {250.0f, 100.0f}, {200.0f, 50.0f}});
    createSC({{300.0f, 250.0f}, {300.0f, 400.0f}, {400.0f, 400.0f}, {450.0f, 350.0f}, {450.0f, 300.0f}, {400.0f, 250.0f}});
    createSC({{437.0f, 411.0f}, {409.0f, 433.0f}, {424.0f, 479.0f}, {456.0f, 488.0f}, {478.0f, 455.0f}, {467.0f, 413.0f}});
    createSC({{500.0f, 450.0f}, {510.0f, 600.0f}, {620.0f, 620.0f}, {670.0f, 560.0f}, {640.0f, 480.0f}, {560.0f, 450.0f}});
    createSC({{750.0f, 200.0f}, {760.0f, 320.0f}, {850.0f, 350.0f}, {880.0f, 290.0f}, {840.0f, 210.0f}, {780.0f, 190.0f}});
    createSC({{900.0f, 400.0f}, {920.0f, 550.0f}, {1020.0f, 570.0f}, {1080.0f, 490.0f}, {1040.0f, 420.0f}, {960.0f, 390.0f}});
    createSC({{600.0f, 150.0f}, {620.0f, 200.0f}, {680.0f, 220.0f}, {700.0f, 180.0f}, {680.0f, 150.0f}, {610.0f, 130.0f}});
    createSC({{200.0f, 300.0f}, {250.0f, 330.0f}, {270.0f, 400.0f}, {230.0f, 450.0f}, {180.0f, 420.0f}, {200.0f, 350.0f}});
    createSC({{50.0f, 600.0f}, {100.0f, 620.0f}, {150.0f, 590.0f}, {140.0f, 550.0f}, {90.0f, 530.0f}, {60.0f, 580.0f}});
    createSC({{300.0f, 700.0f}, {350.0f, 750.0f}, {400.0f, 740.0f}, {420.0f, 700.0f}, {380.0f, 680.0f}, {320.0f, 690.0f}});
}

void LightingSystemEx::on_draw_to(render_target& target)
{
    _layer1->draw_to(target);
    _lightingSystem0.draw_to(target);
}

void LightingSystemEx::on_update(milliseconds deltaTime)
{
    _layer1->update(deltaTime);
    _lightingSystem0.update(deltaTime);
}

void LightingSystemEx::on_fixed_update(milliseconds /* deltaTime */)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    get_window().Title = "TestGame " + stream.str();
}

void LightingSystemEx::on_key_down(keyboard::event& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    case scan_code::D1: {
        blend_funcs funcs;
        _lightingSystem0.set_blend_funcs(funcs);
    } break;
    case scan_code::D2: {
        blend_funcs funcs;
        funcs.SourceAlphaBlendFunc = funcs.SourceColorBlendFunc = blend_func::SrcAlpha;
        funcs.DestinationAlphaBlendFunc = funcs.DestinationColorBlendFunc = blend_func::One;
        _lightingSystem0.set_blend_funcs(funcs);
    } break;
    case scan_code::L: {
        static rng rand;
        auto       lightSource1  = _lightingSystem0.create_light_source();
        lightSource1->Color      = _lightSource0->Color();
        lightSource1->Position   = _lightSource0->Position();
        lightSource1->Range      = _lightSource0->Range();
        lightSource1->Falloff    = _lightSource0->Falloff();
        lightSource1->StartAngle = _lightSource0->StartAngle();
        lightSource1->EndAngle   = _lightSource0->EndAngle();
    } break;
    case scan_code::K: {
        if (!_lightSourceSec0) {
            _lightSourceSec0           = _lightingSystem0.create_light_source();
            _lightSourceSec0->Color    = {255, 255, 255, 128};
            _lightSourceSec1           = _lightingSystem0.create_light_source();
            _lightSourceSec1->Color    = {255, 255, 255, 128};
            _lightSourceSec0->Position = _lightSource0->Position() + point_f {0, 10};
            _lightSourceSec1->Position = _lightSource0->Position() - point_f {0, 10};
        } else {
            _lightingSystem0.remove_light_source(*_lightSourceSec0);
            _lightSourceSec0 = nullptr;
            _lightingSystem0.remove_light_source(*_lightSourceSec1);
            _lightSourceSec1 = nullptr;
        }
    } break;
    case scan_code::F: {
        _lightSource0->Falloff = !_lightSource0->Falloff;
    } break;
    case scan_code::R: {
        if (_lightSource0->Range()) {
            if (*_lightSource0->Range == 500) {
                _lightSource0->Range = std::nullopt;
            } else {
                _lightSource0->Range = *_lightSource0->Range() + 100;
            }
        } else {
            _lightSource0->Range = 100;
        }

    } break;
    case scan_code::T: {
        if (_lightSource0->StartAngle()) {
            if (_lightSource0->StartAngle->Value >= 80) {
                _lightSource0->StartAngle = std::nullopt;
                _lightSource0->EndAngle   = std::nullopt;
            } else {
                _lightSource0->StartAngle = *_lightSource0->StartAngle() + degree_f(10);
                _lightSource0->EndAngle   = *_lightSource0->EndAngle() - degree_f(10);
            }
        } else {
            _lightSource0->StartAngle = 10;
            _lightSource0->EndAngle   = 170;
        }

    } break;
    default:
        break;
    }

    _cam.on_key_down(ev);
}

void LightingSystemEx::on_mouse_button_down(mouse::button_event& ev)
{
    _cam.on_mouse_button_down(ev);
}

void LightingSystemEx::on_mouse_button_up(mouse::button_event& ev)
{
    _cam.on_mouse_button_up(ev);
}

void LightingSystemEx::on_mouse_motion(mouse::motion_event& ev)
{
    _cam.on_mouse_motion(ev);
    _lightSource0->Position = (*get_window().Camera).convert_screen_to_world(ev.Position);
    if (_lightSourceSec0) {
        _lightSourceSec0->Position = _lightSource0->Position() + point_f {0, 10};
        _lightSourceSec1->Position = _lightSource0->Position() - point_f {0, 10};
    }
}

void LightingSystemEx::on_mouse_wheel(mouse::wheel_event& ev)
{
    _cam.on_mouse_wheel(ev);
}
