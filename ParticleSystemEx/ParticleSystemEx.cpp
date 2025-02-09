﻿// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ParticleSystemEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

ParticleSystemEx::ParticleSystemEx(game& game)
    : scene(game)
    , _particleSystem0 {true, 50000}
    , _particleSystem1 {true, 50000}
{
    auto const colors {color_gradient {{0.0f, colors::Red},
                                       {0.14f, colors::Orange},
                                       {0.28f, colors::Yellow},
                                       {0.42f, colors::Green},
                                       {0.57f, colors::Blue},
                                       {0.71f, colors::Indigo},
                                       {0.85f, colors::Violet},
                                       {1.0f, colors::Red}}
                           .colors()};
    _colors = {colors.begin(), colors.end()};
}

ParticleSystemEx::~ParticleSystemEx() = default;

void ParticleSystemEx::on_start()
{
    window().ClearColor = colors::LightSteelBlue;

    using namespace tcob::literals;

    auto& resMgr {parent().library()};
    auto* resGrp {resMgr.get_group("res")};

    {
        _particleSystem0.Material = resGrp->get<material>("QuadParticleMat");
        auto emi0 {_particleSystem0.create_emitter()};
        emi0->Settings.Template = {
            .Speed     = std::minmax(30.f, 50.f),
            .Direction = std::minmax(0_deg, 180_deg),

            // .LinearDamping          = std::minmax(0.1f, 0.5f),
            .LinearAcceleration     = std::minmax(1.f, 3.f),
            .TangentialAcceleration = std::minmax(-40.f, 40.f),

            .Gravity = {{0, 10.f}, {0, 10.f}},

            .Texture      = "snowflake",
            .Colors       = {colors::Black},
            .Transparency = std::minmax(0.0f, 0.5f),

            .Lifetime = std::minmax(5000ms, 25000ms),

            .Scale = std::minmax(0.75f, 1.5f),
            .Size  = {10, 10},

            .Spin     = std::minmax(-150_deg, 150_deg),
            .Rotation = std::minmax(-15_deg, 15_deg),
        };
        emi0->Settings.SpawnArea = {450, 450, 5, 5};
        emi0->Settings.SpawnRate = 1000;
        emi0->Settings.Explosion = true;

        _particleSystem0.start();
    }
    {
        _particleSystem1.Material            = resGrp->get<material>("PointParticleMat");
        _particleSystem1.Material->PointSize = 10;

        auto emi0 {_particleSystem1.create_emitter()};
        emi0->Settings.Template = {
            .Speed     = std::minmax(3.f, 5.f),
            .Direction = std::minmax(0_deg, 180_deg),

            .LinearAcceleration = std::minmax(5.f, 10.f),

            .Texture      = "snowflake",
            .Colors       = _colors,
            .Transparency = std::minmax(0.0f, 0.0f),

            .Lifetime = std::minmax(5000ms, 25000ms),
        };
        emi0->Settings.SpawnArea = {1200, 450, 120, 75};
        emi0->Settings.SpawnRate = 100;

        _particleSystem1.start();
    }
}

void ParticleSystemEx::on_draw_to(render_target& target)
{
    _particleSystem0.draw_to(target);
    _particleSystem1.draw_to(target);
}

void ParticleSystemEx::on_update(milliseconds deltaTime)
{
    _particleSystem0.update(deltaTime);
    _particleSystem1.update(deltaTime);
}

void ParticleSystemEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().stats()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();
    stream << "| particle count:" << (_particleSystem0.particle_count() + _particleSystem1.particle_count());

    window().Title = "TestGame " + stream.str();
}

void ParticleSystemEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    case scan_code::D1:
        _particleSystem0.restart();
        _particleSystem1.restart();
        break;
    case scan_code::D2:
        if (_particleSystem0.is_running()) {
            _particleSystem0.stop();
        } else {
            _particleSystem0.start();
        }
        break;
    case scan_code::D3:
        if (_particleSystem1.is_running()) {
            _particleSystem1.stop();
        } else {
            _particleSystem1.start();
        }
        break;
    default:
        break;
    }

    _cam.on_key_down(ev);
}

void ParticleSystemEx::on_mouse_button_down(mouse::button_event const& ev)
{
    _cam.on_mouse_button_down(ev);
}

void ParticleSystemEx::on_mouse_button_up(mouse::button_event const& ev)
{
    _cam.on_mouse_button_up(ev);
}

void ParticleSystemEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _cam.on_mouse_motion(ev);
}

void ParticleSystemEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _cam.on_mouse_wheel(ev);
}

void ParticleSystemEx::load_emitter(quad_particle_emitter& emi)
{
    data::config::object obj;
    obj["emi"]                    = emi.Settings;
    obj["emi"]["spawn_area"]["x"] = 1200;
    obj["emi"]["lifetime"]        = 3000;
    auto emi1 {std::make_shared<quad_particle_emitter>()};
    emi1->Settings = obj["emi"].as<quad_particle_emitter::settings>();
    _particleSystem0.add_emitter(emi1);
}
