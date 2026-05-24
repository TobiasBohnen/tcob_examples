// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "CameraEx.hpp"

CameraEx::CameraEx(game& game)
    : scene {game}
{
}

CameraEx::~CameraEx() = default;

void CameraEx::on_start()
{
    for (i32 y {0}; y < 8; ++y) {
        for (i32 x {0}; x < 8; ++x) {
            auto& shape    = _layer0.create_shape<gfx::rect_shape>();
            shape.Bounds   = {static_cast<f32>(x) * 96.0f,
                              static_cast<f32>(y) * 96.0f, 80.0f, 80.0f};
            shape.Material = _material;
            shape.Color    = colors::DarkGreen;
        }
    }

    _controller = std::make_unique<gfx::camera_controller>(window().camera());
}

void CameraEx::on_key_down(keyboard::event const& ev)
{
    auto& cam {window().camera()};

    switch (ev.ScanCode) { // NOLINT
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;

    case scan_code::Q:     cam.zoom_by({0.9f, 0.9f}); break;
    case scan_code::E:     cam.zoom_by({1.1f, 1.1f}); break;

    case scan_code::LEFT:  cam.rotate_by(degree_f {-5.0f}); break;
    case scan_code::RIGHT: cam.rotate_by(degree_f {5.0f}); break;

    case scan_code::SPACE:
        _controller->add_trauma(0.6f);
        break;

    case scan_code::P:
        _controller->pan({
            {.Position = {384.0f, 0.0f}, .TimeToArrive = milliseconds {2000}},
            {.Position = {384.0f, 672.0f}, .TimeToArrive = milliseconds {3000}},
            {.Position = {0.0f, 336.0f}, .TimeToArrive = milliseconds {2000}},
            {.Position = {336.0f, 336.0f}, .TimeToArrive = milliseconds {1000}},
        });
        break;

    case scan_code::R:
        cam.reset();
        break;

    default:
        break;
    }
}

void CameraEx::on_mouse_button_down(mouse::button_event const& ev)
{
    _controller->add_trauma(0.4f);
}

void CameraEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    f32 const factor {ev.Scroll.Y > 0 ? 1.1f : 0.9f};
    window().camera().zoom_by({factor, factor});
}

void CameraEx::on_update(milliseconds deltaTime)
{
    _layer0.update(deltaTime);

    _controller->update(deltaTime);
}

void CameraEx::on_draw_to(render_target& target, transform const& xform)
{
    _layer0.draw_to(target, xform);
}

void CameraEx::on_mouse_motion(mouse::motion_event const& ev)
{
}
