// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ParticleSystemEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

ParticleSystemEx::ParticleSystemEx(game& game)
    : scene(game)
{
}

ParticleSystemEx::~ParticleSystemEx() = default;

void ParticleSystemEx::on_start()
{
    using namespace tcob::literals;

    auto& resMgr {get_game().get_library()};
    auto* resGrp {resMgr.get_group("res")};

    // _particleSystem0 = *resGrp->get_asset_ptr<particle_system>("system1");
    _particleSystem0->Material = resGrp->get<material>("particleMat");

    particle_template pt {
        .Acceleration = std::minmax(15.f, 15.f),
        .Direction    = std::minmax(175_deg, 185_deg),
        .Lifetime     = std::minmax(7500ms, 12500ms),
        .Scale        = std::minmax(0.75f, 1.5f),
        .Size         = {30, 30},
        .Speed        = std::minmax(30.f, 150.f),
        .Spin         = std::minmax(-150_deg, 150_deg),
        .Texture      = "snowflake",
        .Transparency = std::minmax(0.0f, 0.55f)};

    auto emi0 {_particleSystem0->create_emitter()};
    emi0->Template  = pt;
    emi0->SpawnArea = {450, 450, 600, 150};
    emi0->Lifetime  = 1000ms;
    emi0->IsLooping = true;
    emi0->SpawnRate = 500;

    _particleSystem0->ParticleUpdate.connect([](particle& p) {
        if (std::any_cast<i32>(p.UserData) == 0) {
            if (p.get_lifetime_ratio() <= 0.95f) {
                p.Direction = p.Direction - 180_deg;
                p.UserData  = 1;
                p.Color     = colors::Red;
            }
        } else if (std::any_cast<i32>(p.UserData) == 1) {
            if (p.get_lifetime_ratio() <= 0.75f) {
                p.Direction = p.Direction - 45_deg;
                p.UserData  = 2;
                p.Color     = colors::Green;
            }
        } else if (std::any_cast<i32>(p.UserData) == 2) {
            if (p.get_lifetime_ratio() <= 0.50f) {
                p.Direction = p.Direction + 45_deg;
                p.UserData  = 3;
                p.Color     = colors::Blue;
            }
        }
    });

    _particleSystem0->ParticleUpdate.connect([](particle& p) {
        p.Color.A = static_cast<u8>(255.f * p.get_lifetime_ratio());
    });
    _particleSystem0->Position = {0.1f, 0.1f};

    _particleSystem0->start();
}

void ParticleSystemEx::on_draw_to(render_target& target)
{
    _particleSystem0->draw_to(target);
}

void ParticleSystemEx::on_update(milliseconds deltaTime)
{
}

void ParticleSystemEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    get_window().Title = "TestGame " + stream.str();

    _particleSystem0->update(deltaTime);
}

void ParticleSystemEx::on_key_down(keyboard::event& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    case scan_code::D1:
        _particleSystem0->restart();
    default:
        break;
    }

    _cam.on_key_down(ev);
}

void ParticleSystemEx::on_mouse_button_down(mouse::button_event& ev)
{
    _cam.on_mouse_button_down(ev);
}

void ParticleSystemEx::on_mouse_button_up(mouse::button_event& ev)
{
    _cam.on_mouse_button_up(ev);
}

void ParticleSystemEx::on_mouse_motion(mouse::motion_event& ev)
{
    _cam.on_mouse_motion(ev);
}

void ParticleSystemEx::on_mouse_wheel(mouse::wheel_event& ev)
{
    _cam.on_mouse_wheel(ev);
}
