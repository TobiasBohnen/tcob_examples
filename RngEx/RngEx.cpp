// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "RngEx.hpp"

RngEx::RngEx(game& game)
    : scene {game}
    , _form0 {std::make_shared<rng_form>(&get_window())}
{
}

RngEx::~RngEx() = default;

void RngEx::on_start()
{
    get_root_node()->Entity = _form0;
}

void RngEx::on_draw_to(render_target& target)
{
}

void RngEx::on_update(milliseconds deltaTime)
{
}

void RngEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::R: {
        auto _ = get_window().copy_to_image().save("screen01.webp");
    } break;
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    default:
        break;
    }
}
