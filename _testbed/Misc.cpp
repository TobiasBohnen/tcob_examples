// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "Misc.hpp"

#include <iomanip>
#include <iostream>

using namespace std::chrono_literals;
using namespace tcob::literals;

MiscScene::MiscScene(game& game)
    : scene(game)
{
}

MiscScene::~MiscScene() = default;

void MiscScene::on_start()
{
    auto& resMgr {library()};
    resMgr.load_all_groups();
}

void MiscScene::on_finish()
{
}

void MiscScene::on_draw_to(render_target& target)
{
}

void MiscScene::on_update(milliseconds deltaTime)
{
}

void MiscScene::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().stats()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void MiscScene::on_key_down(keyboard::event const& ev)
{
    auto& camera {window().camera()};
    auto& resMgr {library()};
    auto* resGrp {resMgr.get_group("res")};

    if (ev.ScanCode == scan_code::D2) {

    } else if (ev.ScanCode == scan_code::BACKSPACE) {
        parent().pop_current_scene();
    }
}

void MiscScene::on_mouse_motion(mouse::motion_event const& /* ev */)
{
}

void MiscScene::on_mouse_wheel(mouse::wheel_event const& ev)
{
}
