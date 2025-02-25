﻿// Copyright (c) 2025 Tobias Bohnen
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
    window().ClearColor = colors::Black;

    using namespace tcob::literals;

    _lightingSystem0.Bounds = {point_f::Zero, size_f {1600, 900}};

    _lightSource0        = _lightingSystem0.create_light_source();
    _lightSource0->Range = 100;
    _lightSource0->Color = {255, 255, 0, 128};

    auto const createSC {[&](std::vector<point_f> const& points) {
        auto shape0 {_layer0.create_shape<gfx::poly_shape>()};
        shape0->Material = _material;
        shape0->Color    = colors::Blue;
        shape0->Polygons = {{.Outline = points}};

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
    createSC({{1300.0f, 100.0f}, {1350.0f, 150.0f}, {1380.0f, 200.0f}, {1350.0f, 250.0f}, {1300.0f, 200.0f}, {1270.0f, 150.0f}});
    createSC({{1150.0f, 300.0f}, {1180.0f, 350.0f}, {1230.0f, 370.0f}, {1270.0f, 330.0f}, {1240.0f, 280.0f}, {1180.0f, 270.0f}});
    createSC({{1100.0f, 600.0f}, {1150.0f, 620.0f}, {1200.0f, 610.0f}, {1180.0f, 560.0f}, {1130.0f, 540.0f}, {1080.0f, 580.0f}});
    createSC({{500.0f, 700.0f}, {550.0f, 720.0f}, {580.0f, 770.0f}, {540.0f, 800.0f}, {490.0f, 780.0f}, {470.0f, 730.0f}});
    createSC({{1400.0f, 500.0f}, {1450.0f, 550.0f}, {1500.0f, 530.0f}, {1480.0f, 480.0f}, {1430.0f, 460.0f}, {1380.0f, 480.0f}});
    createSC({{300.0f, 650.0f}, {350.0f, 670.0f}, {400.0f, 660.0f}, {380.0f, 610.0f}, {330.0f, 590.0f}, {290.0f, 630.0f}});
    createSC({{1300.0f, 700.0f}, {1320.0f, 750.0f}, {1360.0f, 740.0f}, {1380.0f, 700.0f}, {1350.0f, 660.0f}, {1310.0f, 670.0f}});
    createSC({{1450.0f, 200.0f}, {1500.0f, 230.0f}, {1530.0f, 280.0f}, {1490.0f, 320.0f}, {1440.0f, 290.0f}, {1410.0f, 240.0f}});
    createSC({{900.0f, 700.0f}, {920.0f, 750.0f}, {980.0f, 760.0f}, {1020.0f, 720.0f}, {980.0f, 680.0f}, {920.0f, 690.0f}});
    createSC({{100.0f, 800.0f}, {140.0f, 840.0f}, {180.0f, 830.0f}, {170.0f, 780.0f}, {130.0f, 770.0f}, {90.0f, 790.0f}});
    createSC({{600.0f, 650.0f}, {620.0f, 700.0f}, {670.0f, 720.0f}, {700.0f, 680.0f}, {670.0f, 630.0f}, {620.0f, 640.0f}});
    createSC({{1550.0f, 400.0f}, {1580.0f, 450.0f}, {1590.0f, 420.0f}, {1570.0f, 370.0f}, {1530.0f, 360.0f}, {1500.0f, 390.0f}});
    createSC({{1200.0f, 800.0f}, {1230.0f, 850.0f}, {1280.0f, 840.0f}, {1300.0f, 790.0f}, {1250.0f, 760.0f}, {1210.0f, 780.0f}});
    createSC({{400.0f, 550.0f}, {430.0f, 590.0f}, {470.0f, 580.0f}, {480.0f, 530.0f}, {440.0f, 510.0f}, {410.0f, 530.0f}});
}

void LightingSystemEx::on_draw_to(render_target& target)
{
    _layer0.draw_to(target);
    _lightingSystem0.draw_to(target);
}

void LightingSystemEx::on_update(milliseconds deltaTime)
{
    _lightingSystem0.update(deltaTime);
    _layer0.update(deltaTime);
}

void LightingSystemEx::on_fixed_update(milliseconds /* deltaTime */)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().stats()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void LightingSystemEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
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
            _lightSourceSec0->Color    = _lightSource0->Color();
            _lightSourceSec0->Position = _lightSource0->Position() + point_f {0, 15};
            _lightSourceSec0->Falloff  = _lightSource0->Falloff();
            _lightSourceSec0->Range    = _lightSource0->Range();

            _lightSourceSec1           = _lightingSystem0.create_light_source();
            _lightSourceSec1->Color    = _lightSource0->Color();
            _lightSourceSec1->Position = _lightSource0->Position() - point_f {0, 15};
            _lightSourceSec1->Falloff  = _lightSource0->Falloff();
            _lightSourceSec1->Range    = _lightSource0->Range();
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
            _lightSource0->StartAngle = degree_f {10};
            _lightSource0->EndAngle   = degree_f {170};
        }

    } break;
    default:
        break;
    }

    _cam.on_key_down(ev);
}

void LightingSystemEx::on_mouse_button_down(mouse::button_event const& ev)
{
    _cam.on_mouse_button_down(ev);
}

void LightingSystemEx::on_mouse_button_up(mouse::button_event const& ev)
{
    _cam.on_mouse_button_up(ev);
}

void LightingSystemEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _cam.on_mouse_motion(ev);
    _lightSource0->Position = (window().camera()).convert_screen_to_world(ev.Position);
    if (_lightSourceSec0) {
        _lightSourceSec0->Position = _lightSource0->Position() + point_f {0, 10};
        _lightSourceSec1->Position = _lightSource0->Position() - point_f {0, 10};
    }
}

void LightingSystemEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _cam.on_mouse_wheel(ev);
}
