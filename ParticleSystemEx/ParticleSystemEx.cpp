// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ParticleSystemEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

ParticleSystemEx::ParticleSystemEx(game& game)
    : scene(game)
    , _particleSystem0 {true}
    , _particleSystem1 {true}
{
    _colors = color_gradient {{0.0f, colors::Red},
                              {0.14f, colors::Orange},
                              {0.28f, colors::Yellow},
                              {0.42f, colors::Green},
                              {0.57f, colors::Blue},
                              {0.71f, colors::Indigo},
                              {0.85f, colors::Violet},
                              {1.0f, colors::Red}}
                  .colors();
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
            .Scale = std::minmax(0.75f, 1.5f),
            .Size  = {10, 10},

            .Spin     = std::minmax(-150_deg, 150_deg),
            .Rotation = std::minmax(-15, +15),

            .Acceleration = std::minmax(15.f, 35.f),
            .Speed        = std::minmax(30.f, 50.f),
            .Direction    = std::minmax(-25_deg, 25_deg),

            .Texture      = "snowflake",
            .Color        = colors::Red,
            .Transparency = std::minmax(1.0f, 1.0f),

            .Lifetime = std::minmax(5000ms, 25000ms),
        };
        emi0->Settings.SpawnArea = {450, 450, 30, 50};
        emi0->Settings.SpawnRate = 2000;

        _particleSystem0.ParticleUpdate.connect([&](auto const& pev) {
            auto& p {pev.Particle};
            if (!p.UserData.has_value()) { p.UserData = 0; }
            auto const phase {std::any_cast<i32>(p.UserData)};

            f32 const parLife {static_cast<f32>(p.RemainingLife / p.StartingLife)};
            if (phase < 4) { p.Color = _colors[static_cast<u8>(255.f * (1 - parLife))]; }

            auto const dist {p.Bounds.center().distance_to({460, 200})};
            if (phase < 5 && dist < 10) {
                auto const corr {36_deg * (10 - dist) * (pev.DeltaTime.count() / 1000)};
                p.Direction += p.Direction > 0 ? corr : -corr;
                p.Color    = colors::Black;
                p.UserData = 4;
            } else if (phase == 4 && dist > 100) {
                p.Color     = colors::Gray;
                p.Direction = degree_f {0};
                p.UserData  = 5;
            }

            switch (phase) {
            case 0:
                if (parLife <= 0.75f) {
                    p.Direction /= 4;
                    p.UserData = 1;
                }
                break;
            case 1:
                if (parLife <= 0.50f) {
                    p.UserData  = 2;
                    p.Direction = degree_f {0};
                    p.Acceleration *= -5.5f;
                }
                break;
            case 2:
                if (parLife <= 0.25f) {
                    p.Color.A = static_cast<u8>(255.f * (parLife / 0.25f));
                }
                break;
            }
        });

        _particleSystem0.start();
    }
    {
        _particleSystem1.Material            = resGrp->get<material>("PointParticleMat");
        _particleSystem1.Material->PointSize = 10;

        auto emi0 {_particleSystem1.create_emitter()};
        emi0->Settings.Template = {
            .Acceleration = std::minmax(15.f, 35.f),
            .Speed        = std::minmax(30.f, 50.f),
            .Direction    = std::minmax(-25_deg, 25_deg),

            .Texture      = "snowflake",
            .Color        = colors::Red,
            .Transparency = std::minmax(1.0f, 1.0f),

            .Lifetime = std::minmax(5000ms, 25000ms),
        };
        emi0->Settings.SpawnArea = {1200, 450, 30, 50};
        emi0->Settings.SpawnRate = 2000;

        _particleSystem1.ParticleUpdate.connect([&](auto const& pev) {
            auto& p {pev.Particle};
            if (!p.UserData.has_value()) { p.UserData = 0; }
            auto const phase {std::any_cast<i32>(p.UserData)};

            f32 const parLife {static_cast<f32>(p.RemainingLife / p.StartingLife)};
            if (phase < 4) { p.Color = _colors[static_cast<u8>(255.f * (1 - parLife))]; }

            auto const dist {p.Position.distance_to({1210, 200})};
            if (phase < 5 && dist < 10) {
                auto const corr {36_deg * (10 - dist) * (pev.DeltaTime.count() / 1000)};
                p.Direction += p.Direction > 0 ? corr : -corr;
                p.Color    = colors::Black;
                p.UserData = 4;
            } else if (phase == 4 && dist > 100) {
                p.Color     = colors::Gray;
                p.Direction = degree_f {0};
                p.UserData  = 5;
            }

            switch (phase) {
            case 0:
                if (parLife <= 0.75f) {
                    p.Direction /= 4;
                    p.UserData = 1;
                }
                break;
            case 1:
                if (parLife <= 0.50f) {
                    p.UserData  = 2;
                    p.Direction = degree_f {0};
                    p.Acceleration *= -5.5f;
                }
                break;
            case 2:
                if (parLife <= 0.25f) {
                    p.Color.A = static_cast<u8>(255.f * (parLife / 0.25f));
                }
                break;
            }
        });

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
