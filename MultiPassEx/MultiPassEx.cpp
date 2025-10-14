// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "MultiPassEx.hpp"

#include "../_common/Image.hpp"

#include <iomanip>

MultiPassEx::MultiPassEx(game& game)
    : scene {game}
{
    auto createPass {[&](auto const& tex, color col) {
        auto& pass {_mat0->create_pass()};
        pass.Texture = tex;
        pass.Color   = col;
    }};

    createPass(_tex0, colors::White);
    createPass(_tex1, colors::Red);
    createPass(_tex2, colors::Green);
    createPass(_tex3, colors::Blue);
    createPass(_tex4, colors::Orange);

    image img0 {image::Create({64, 64}, image::format::RGBA, smile)};
    auto  imgSize {img0.info().Size};

    _tex0->resize(imgSize, 1, texture::format::RGBA8);
    _tex0->update_data(img0.data(), 0);

    size_i const partSize {24, 24};

    _tex1->resize(imgSize, 1, texture::format::RGBA8);
    _tex1->update_data({0, 0}, partSize, img0.data({{40, 40}, partSize}).data(), 0);

    _tex2->resize(imgSize, 1, texture::format::RGBA8);
    _tex2->update_data({40, 40}, partSize, img0.data({{0, 0}, partSize}).data(), 0);

    _tex3->resize(imgSize, 1, texture::format::RGBA8);
    _tex3->update_data({0, 40}, partSize, img0.data({{40, 0}, partSize}).data(), 0);

    _tex4->resize(imgSize, 1, texture::format::RGBA8);
    _tex4->update_data({40, 0}, partSize, img0.data({{0, 40}, partSize}).data(), 0);
}

MultiPassEx::~MultiPassEx() = default;

void MultiPassEx::on_start()
{
    auto& sprite0    = _layer1.create_shape<gfx::rect_shape>();
    sprite0.Material = _mat0;
    sprite0.Bounds   = {{400, 300}, {512, 512}};
}

void MultiPassEx::on_draw_to(render_target& target)
{
    _layer1.draw_to(target);
}

void MultiPassEx::on_update(milliseconds deltaTime)
{
    _layer1.update(deltaTime);
}

void MultiPassEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void MultiPassEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void MultiPassEx::on_mouse_motion(mouse::motion_event const& ev)
{
}