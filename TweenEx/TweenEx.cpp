// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "TweenEx.hpp"

TweenEx::TweenEx(game& game)
    : scene {game}
{
}

TweenEx::~TweenEx() = default;

milliseconds duration {5000};

void TweenEx::on_start()
{
    auto create_circle {[&](point_f position, color color, auto&& auto0) {
        auto& circle {_layer1.create_shape<gfx::circle_shape>()};
        circle.Material = _material;
        circle.Segments = 16;
        circle.Center   = position + point_f {30, 30};
        circle.Radius   = 30;
        circle.Color    = color;

        auto0->Value.Changed.connect([&circle](point_f point) { circle.Center = point + point_f {30, 30}; });

        _tweens.push_back(auto0);
    }};

    f32           endX {800 - 60};
    point_f       position {0, -65};
    point_f const posOffset {position.X, -position.Y};
    {
        position += posOffset;
        auto auto0 {make_shared_tween<linear_tween<point_f>>(duration, position, point_f {endX, position.Y})};
        create_circle(position, colors::Blue, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<smoothstep_tween<point_f>>(duration, position, point_f {endX, position.Y})};
        create_circle(position, colors::Red, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<smootherstep_tween<point_f>>(duration, position, point_f {endX, position.Y})};
        create_circle(position, colors::Yellow, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<sine_wave_tween<point_f>>(duration, position, point_f {endX, position.Y}, 0.5f, 0.f)};
        create_circle(position, colors::Orange, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<polynomial_tween<point_f>>(duration, position, point_f {endX, position.Y}, 0.75f)};
        create_circle(position, colors::LawnGreen, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<polynomial_tween<point_f>>(duration, position, point_f {endX, position.Y}, 2.f, tween_func::ease_mode::Out)};
        create_circle(position, colors::Brown, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<polynomial_tween<point_f>>(duration, position, point_f {endX, position.Y}, 2.f)};
        create_circle(position, colors::Green, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<polynomial_tween<point_f>>(duration, position, point_f {endX, position.Y}, 0.5f, tween_func::ease_mode::Out)};
        create_circle(position, colors::Orchid, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<square_wave_tween<point_f>>(duration, position, point_f {endX, position.Y}, 1.f, 0.f)};
        create_circle(position, colors::RebeccaPurple, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<bounce_tween<point_f>>(duration, position, point_f {endX, position.Y}, tween_func::ease_mode::In)};
        create_circle(position, colors::DarkRed, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<elastic_tween<point_f>>(duration, position, point_f {endX, position.Y})};
        create_circle(position, colors::GhostWhite, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<back_tween<point_f>>(duration, position, point_f {endX, position.Y})};
        create_circle(position, colors::LimeGreen, auto0);
    }

    {
        position += posOffset;
        auto auto0 {make_shared_tween<circular_tween<point_f>>(duration, position, point_f {endX, position.Y})};
        create_circle(position, colors::MediumTurquoise, auto0);
    }

    ////////////////////////////////////////////////////////////
    for (auto& tween : _tweens) {
        tween->start(playback_mode::Looped);
    }
}

void TweenEx::on_draw_to(render_target& target)
{
    _layer1.draw_to(target);
}

void TweenEx::on_update(milliseconds deltaTime)
{
    for (auto& tween : _tweens) {
        tween->update(deltaTime);
    }
    _layer1.update(deltaTime);
}

void TweenEx::on_fixed_update(milliseconds deltaTime)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);
}

void TweenEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::D1:
        for (auto& auto0 : _tweens) {
            auto0->Interval = duration / 2;
        }
        break;
    case scan_code::D2:
        for (auto& auto0 : _tweens) {
            auto0->Interval = duration / 5;
        }
        break;
    case scan_code::D3:
        for (auto& auto0 : _tweens) {
            auto0->Interval = duration / 10;
        }
        break;
    case scan_code::D4:
        for (auto& auto0 : _tweens) {
            auto0->Interval = duration / 50;
        }
        break;
    case scan_code::D5:
        for (auto& auto0 : _tweens) {
            auto0->Interval = duration / 100;
        }
        break;
    case scan_code::D6:
        for (auto& auto0 : _tweens) {
            auto0->Interval = milliseconds {0};
        }
        break;
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void TweenEx::on_mouse_motion(mouse::motion_event const& ev)
{
}
