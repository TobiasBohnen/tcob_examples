// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

// IWYU pragma: always_keep

#include <tcob/tcob.hpp>
using namespace tcob;
using namespace tcob::gfx;
using namespace tcob::assets;
using namespace tcob::tweening;
using namespace tcob::scripting;
using namespace tcob::physics;
using namespace tcob::audio;
using namespace tcob::data;
using namespace tcob::input;

////////////////////////////////////////////////////////////

class basic_cam {
public:
    void on_key_down(keyboard::event& ev) const
    {
        float moveFactor {14.f};
        auto& camera {*locate_service<render_system>().get_window().Camera};
        if (ev.ScanCode == scan_code::A) {
            camera.move_by({-moveFactor, 0});
        } else if (ev.ScanCode == scan_code::D) {
            camera.move_by({moveFactor, 0});
        } else if (ev.ScanCode == scan_code::S) {
            camera.move_by({0.0f, moveFactor});
        } else if (ev.ScanCode == scan_code::W) {
            camera.move_by({0.0f, -moveFactor});
        }
    }

    void on_mouse_button_down(mouse::button_event& ev)
    {
        _mouseDown = false;
        if (ev.Pressed && ev.Button == mouse::button::Left) {
            _mouseDown = true;
        }
    }

    void on_mouse_button_up(mouse::button_event& ev)
    {
        if (ev.Button == mouse::button::Left) {
            _mouseDown = false;
        }
    }

    void on_mouse_motion(mouse::motion_event& ev) const
    {
        if (_mouseDown) {
            auto& camera {*locate_service<render_system>().get_window().Camera};
            auto  zoom {camera.get_zoom()};
            camera.move_by(-(point_f {ev.RelativeMotion} / point_f {zoom.Width, zoom.Height}));
        }
    }

    void on_mouse_wheel(mouse::wheel_event& ev)
    {
        auto& camera {*locate_service<render_system>().get_window().Camera};
        _zoomStage = std::clamp(_zoomStage - ev.Scroll.Y, 0, 6);
        constexpr std::array<size_f, 7> zoomLevels {{{5.f, 5.f}, {3.f, 3.f}, {2.f, 2.f}, {1.f, 1.f}, {0.75f, 0.75f}, {0.5f, 0.5f}, {0.25f, 0.25f}}};
        camera.set_zoom(zoomLevels[_zoomStage]);
    }

private:
    bool _mouseDown {false};
    i32  _zoomStage {3};
};
