// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "MetaballEx.hpp"

constexpr i32    scale {2};
constexpr size_f msize {800, 600};

MetaballEx::MetaballEx(game& game)
    : scene {game}
    , _metaball {size_i {msize / scale},
                 color_gradient {{0, colors::DarkRed}, {0.25, colors::Red}, {0.75, colors::Orange}, {0.95, colors::Yellow}, {1, colors::Yellow}}}
{
    rng                         rng;
    std::vector<metaball::ball> balls;
    for (i32 i {0}; i < 10; ++i) {
        auto& ball {balls.emplace_back()};
        ball.Position.X = rng(0.0f, msize.Width / scale);
        ball.Position.Y = rng(0.0f, msize.Height / scale);
        ball.Radius     = rng(10.0f / scale, 70.0f / scale);
        ball.Velocity.X = rng(-100.0f / scale, 100.0f / scale);
        ball.Velocity.Y = rng(-100.0f / scale, 100.0f / scale);
    }
    _metaball.Balls = balls;

    _texture->resize(size_i {msize / scale}, 1, texture::format::RGBA8);
    _material->first_pass().Texture = _texture;

    geometry::set_color(_quad, colors::White);
    geometry::set_position(_quad, {point_f::Zero, msize});
    geometry::set_texcoords(_quad, {.UVRect = {0, 0, 1, 1}, .Level = 0});
    _renderer.set_geometry({.Vertices = _quad, .Indices = QuadIndicies, .Type = primitive_type::Triangles}, &_material->first_pass());
}

MetaballEx::~MetaballEx() = default;

void MetaballEx::on_fixed_update(milliseconds /* deltaTime */)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);
}

void MetaballEx::on_start()
{
}

void MetaballEx::on_update(milliseconds deltaTime)
{
    _metaball.update(deltaTime);
}

void MetaballEx::on_draw_to(render_target& target)
{
    _texture->update_data(_metaball.image(), 0);
    _renderer.render_to_target(target);
}

void MetaballEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void MetaballEx::on_mouse_motion(mouse::motion_event const& ev)
{
}

void MetaballEx::on_mouse_button_down(mouse::button_event const& ev)
{
}

void MetaballEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
}
