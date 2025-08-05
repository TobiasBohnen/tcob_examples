// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "AniTexEx.hpp"

#include <iomanip>
#include <iostream>

#include "../_common/Ani.hpp"

AniTexEx::AniTexEx(game& game)
    : scene {game}
{
}

AniTexEx::~AniTexEx() = default;

void AniTexEx::on_fixed_update(milliseconds /* deltaTime */)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void AniTexEx::on_start()
{
    /*
        std::vector<image_frame>  frames;
        easing::quad_bezier_curve curv;
        curv.StartPoint   = {196, 40};
        curv.ControlPoint = {0, 256};
        curv.EndPoint     = {196, 216};

        for (i32 i {0}; i < 1500; ++i) {
            image img {image::CreateEmpty({256, 256}, image::format::RGBA)};
            img.fill({point_i {curv(i / 1500.)}, size_i {25, 25}}, colors::Red);
            frames.push_back(image_frame {img, milliseconds {5}});
        }
        std::ignore = save_animation("test-ani.png", frames);
    */
    auto createShape {[&](auto&& tex, auto&& mat, auto&& img, auto&& ext, auto&& bounds) {
        std::ignore  = tex->load(std::make_shared<io::isstream>(img), ext);
        mat->Texture = tex;
        tex->start(true);

        auto shape0 {_layer0.create_shape<gfx::rect_shape>()};
        shape0->Bounds   = bounds;
        shape0->Material = mat;
    }};

    createShape(_texGif, _matGif, ani_gif, ".gif", rect_f {10, 10, 500, 500});
    createShape(_texWebp, _matWebp, ani_webp, ".webp", rect_f {600, 10, 500, 500});
    createShape(_texPng, _matPng, ani_png, ".png", rect_f {10, 550, 500, 500});
    createShape(_texTheora, _matTheora, ani_theora, ".ogg", rect_f {600, 550, 500, 500});
}

void AniTexEx::on_update(milliseconds deltaTime)
{
    _texGif->update(deltaTime);
    _texWebp->update(deltaTime);
    _texPng->update(deltaTime);
    _texTheora->update(deltaTime);

    _layer0.update(deltaTime);
}

void AniTexEx::on_draw_to(render_target& target)
{
    _layer0.draw_to(target);
}

void AniTexEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::R: {
        _texGif->restart();
        _texWebp->restart();
        _texPng->restart();
        _texTheora->restart();
    } break;
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void AniTexEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _cam.on_mouse_motion(ev);
}

void AniTexEx::on_mouse_button_down(mouse::button_event const& ev)
{
    _cam.on_mouse_button_down(ev);
}

void AniTexEx::on_mouse_button_up(mouse::button_event const& ev)
{
    _cam.on_mouse_button_up(ev);
}

void AniTexEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _cam.on_mouse_wheel(ev);
}
