// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "NoiseEx.hpp"

#include "../_common/Image.hpp"

#include "tcob/core/procgen/Noise.hpp"

template <typename Noise>
static auto GenerateNoiseImage(Noise const& noise, size_i size, i32 tiles) -> image
{
    std::vector<u8> data;
    data.reserve(static_cast<usize>(size.Width * size.Height * 4));

    for (i32 y {0}; y < size.Height; ++y) {
        for (i32 x {0}; x < size.Width; ++x) {
            // repeat the [0, 1) noise domain `tiles` times across the image
            f32 const u {static_cast<f32>((x * tiles) % size.Width) / static_cast<f32>(size.Width)};
            f32 const w {static_cast<f32>((y * tiles) % size.Height) / static_cast<f32>(size.Height)};

            f32 const  v {noise(point_f {u, w})};
            auto const c {static_cast<u8>(std::clamp(v, 0.0f, 1.0f) * 255.0f)};
            data.push_back(c);
            data.push_back(c);
            data.push_back(c);
            data.push_back(255);
        }
    }

    return image::Create(size, image::format::RGBA, data);
}

NoiseEx::NoiseEx(game& game)
    : scene {game}
{
    _mat0->first_pass().Texture = _tex0;

    size_i const imgSize {256, 256};
    _tex0->resize(imgSize, 6, texture::format::RGBA8);

    auto& regs {_tex0->regions()};

    {
        procgen::perlin_noise noise {{8, 8}, 1.0f};

        image img {GenerateNoiseImage(noise, imgSize, 1)};
        _tex0->update_data(img.data(), 0);
        regs["perlin"] = {{0, 0, 1, 1}, 0};

        image imgTiled {GenerateNoiseImage(noise, imgSize, 2)};
        _tex0->update_data(imgTiled.data(), 3);
        regs["perlin_tiled"] = {{0, 0, 1, 1}, 3};
    }

    {
        procgen::value_noise noise {{8, 8}, 1.0f};

        image img {GenerateNoiseImage(noise, imgSize, 1)};
        _tex0->update_data(img.data(), 1);
        regs["value"] = {{0, 0, 1, 1}, 1};

        image imgTiled {GenerateNoiseImage(noise, imgSize, 2)};
        _tex0->update_data(imgTiled.data(), 4);
        regs["value_tiled"] = {{0, 0, 1, 1}, 4};
    }

    {
        procgen::cellular_noise noise {64, 1.0f};

        image img {GenerateNoiseImage(noise, imgSize, 1)};
        _tex0->update_data(img.data(), 2);
        regs["cellular"] = {{0, 0, 1, 1}, 2};

        image imgTiled {GenerateNoiseImage(noise, imgSize, 2)};
        _tex0->update_data(imgTiled.data(), 5);
        regs["cellular_tiled"] = {{0, 0, 1, 1}, 5};
    }
}

NoiseEx::~NoiseEx() = default;

void NoiseEx::on_start()
{
    auto& sprite0         = _layer1.create_shape<gfx::rect_shape>();
    sprite0.Material      = _mat0;
    sprite0.TextureRegion = "perlin";
    sprite0.Bounds        = {{16.f, 16.f}, {240, 240}};

    //////////
    auto& sprite1         = _layer1.create_shape<gfx::rect_shape>();
    sprite1.Material      = _mat0;
    sprite1.TextureRegion = "value";
    sprite1.Bounds        = {{280.f, 16.f}, {240, 240}};

    //////////
    auto& sprite2         = _layer1.create_shape<gfx::rect_shape>();
    sprite2.Material      = _mat0;
    sprite2.TextureRegion = "cellular";
    sprite2.Bounds        = {{544.f, 16.f}, {240, 240}};

    //////////
    auto& sprite3         = _layer1.create_shape<gfx::rect_shape>();
    sprite3.Material      = _mat0;
    sprite3.TextureRegion = "perlin_tiled";
    sprite3.Bounds        = {{16.f, 280.f}, {240, 240}};

    //////////
    auto& sprite4         = _layer1.create_shape<gfx::rect_shape>();
    sprite4.Material      = _mat0;
    sprite4.TextureRegion = "value_tiled";
    sprite4.Bounds        = {{280.f, 280.f}, {240, 240}};

    //////////
    auto& sprite5         = _layer1.create_shape<gfx::rect_shape>();
    sprite5.Material      = _mat0;
    sprite5.TextureRegion = "cellular_tiled";
    sprite5.Bounds        = {{544.f, 280.f}, {240, 240}};
}

void NoiseEx::on_draw_to(render_target& target, transform const& xform)
{
    _layer1.draw_to(target, xform);
}

void NoiseEx::on_update(milliseconds deltaTime)
{
    _layer1.update(deltaTime);
}

void NoiseEx::on_fixed_update(milliseconds /* deltaTime */)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);
}

void NoiseEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void NoiseEx::on_mouse_motion(mouse::motion_event const& /* ev */)
{
}
