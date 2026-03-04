// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "UniformBufferEx.hpp"
#include "../_common/Image.hpp"

UniformBufferEx::UniformBufferEx(game& game)
    : scene {game}
{
    {
        image img0 {image::Create({64, 64}, image::format::RGBA, smile)};
        auto  imgSize {img0.info().Size};
        _tex0->resize(imgSize, 1, texture::format::RGBA8);
        _tex0->update_data(img0.data(), 0);

        f32 constexpr speed {8.0f};
        _ubo0->update(speed, 0);

        auto& pass         = _mat->create_pass();
        pass.Texture       = _tex0;
        pass.Shader        = _shader0;
        pass.Color         = colors::White;
        pass.UniformBuffer = _ubo0;
        pass.BlendFuncs    = blend_funcs {};
    }

    {
        std::array<f32, 2> constexpr vignette {0.35f, 0.15f};
        _ubo1->update(std::span<f32 const> {vignette}, 0);

        auto& pass         = _mat->create_pass();
        pass.Shader        = _shader1;
        pass.Color         = colors::White;
        pass.UniformBuffer = _ubo1;
        pass.BlendFuncs    = blend_funcs {
            blend_func::SrcAlpha,
            blend_func::OneMinusSrcAlpha,
            blend_func::One,
            blend_func::OneMinusSrcAlpha};
        pass.BlendEquation = blend_equation::Add;
    }
}

UniformBufferEx::~UniformBufferEx() = default;

void UniformBufferEx::on_start()
{
    auto& sprite0    = _layer1.create_shape<gfx::rect_shape>();
    sprite0.Material = _mat;
    auto const winSize {*window().Size};
    sprite0.Bounds = {{(winSize.Width - 256) / 2.f, (winSize.Height - 256) / 2.f}, {256, 256}};
}

void UniformBufferEx::on_draw_to(render_target& target, transform const& xform)
{
    _layer1.draw_to(target, xform);
}

void UniformBufferEx::on_update(milliseconds deltaTime)
{
    _layer1.update(deltaTime);
}

void UniformBufferEx::on_fixed_update(milliseconds deltaTime)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);
}

void UniformBufferEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}
