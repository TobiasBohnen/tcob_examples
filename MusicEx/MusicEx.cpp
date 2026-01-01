// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "MusicEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

MusicEx::MusicEx(game& game)
    : scene {game}
    , _form0 {std::make_shared<crtl_form>(window())}
{
}

MusicEx::~MusicEx() = default;

void MusicEx::on_start()
{
    auto& resMgr {library()};
    auto& resGrp {resMgr.create_or_get_group("res")};
    root_node().Entity = _form0;
    _form0->Play.connect([&](auto const& val) {
        std::ignore = _music0.open(val);
        _music0.play();
    });
}

void MusicEx::on_draw_to(render_target& target)
{
}

void MusicEx::on_update(milliseconds deltaTime)
{
}

void MusicEx::on_fixed_update(milliseconds /* deltaTime */)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void MusicEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}
