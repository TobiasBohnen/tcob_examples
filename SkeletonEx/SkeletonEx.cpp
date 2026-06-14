// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "SkeletonEx.hpp"

using namespace tcob;
using namespace tcob::gfx;
using namespace tcob::input;

SkeletonEx::SkeletonEx(game& game)
    : scene {game}
{
}

SkeletonEx::~SkeletonEx() = default;

void SkeletonEx::on_fixed_update(milliseconds /* deltaTime */)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y);
}

void SkeletonEx::on_start()
{
    std::vector<skeletal_animation::bone> const bones {
        {.Name = "root"},
        {.Name = "torso", .Parent = 0},
        {.Name = "neck", .Parent = 1},
        {.Name = "head", .Parent = 2},
        {.Name = "upper_l", .Parent = 1},
        {.Name = "lower_l", .Parent = 4},
        {.Name = "hand_l", .Parent = 5},
        {.Name = "upper_r", .Parent = 1},
        {.Name = "lower_r", .Parent = 7},
        {.Name = "hand_r", .Parent = 8},
        {.Name = "thigh_l", .Parent = 0},
        {.Name = "shin_l", .Parent = 10},
        {.Name = "foot_l", .Parent = 11},
        {.Name = "thigh_r", .Parent = 0},
        {.Name = "shin_r", .Parent = 13},
        {.Name = "foot_r", .Parent = 14},
    };

    auto key {[](milliseconds t, point_f tr, degree_f ro, size_f sc = {1, 1}) {
        return keyframe {.Timestamp = t, .Translation = tr, .Rotation = radian_f {ro}, .Scale = sc};
    }};

    using ms = milliseconds;

    std::map<i32, std::vector<keyframe>> tracks;

    tracks[0] = {
        // root
        key(ms {0}, {400, 300}, degree_f {0.0f}),
        key(ms {300}, {410, 290}, degree_f {6.0f}),
        key(ms {600}, {420, 300}, degree_f {0.0f}),
        key(ms {900}, {410, 308}, degree_f {-6.0f}),
        key(ms {1200}, {400, 300}, degree_f {0.0f}),
        key(ms {1500}, {390, 290}, degree_f {6.0f}),
        key(ms {1800}, {380, 300}, degree_f {0.0f}),
        key(ms {2100}, {390, 308}, degree_f {-6.0f}),
        key(ms {2400}, {400, 300}, degree_f {0.0f}),
    };
    tracks[1] = {
        // torso
        key(ms {0}, {0, -70}, degree_f {-12.0f}),
        key(ms {600}, {0, -70}, degree_f {17.0f}),
        key(ms {1200}, {0, -70}, degree_f {-12.0f}),
        key(ms {1800}, {0, -70}, degree_f {17.0f}),
        key(ms {2400}, {0, -70}, degree_f {-12.0f}),
    };
    tracks[2] = {
        // neck
        key(ms {0}, {0, -20}, degree_f {0}),
        key(ms {2400}, {0, -20}, degree_f {0}),
    };
    tracks[3] = {
        // head
        key(ms {0}, {0, -30}, degree_f {9.0f}),
        key(ms {400}, {0, -28}, degree_f {-9.0f}),
        key(ms {800}, {0, -30}, degree_f {9.0f}),
        key(ms {1200}, {0, -28}, degree_f {-9.0f}),
        key(ms {1600}, {0, -30}, degree_f {9.0f}),
        key(ms {2000}, {0, -28}, degree_f {-9.0f}),
        key(ms {2400}, {0, -30}, degree_f {9.0f}),
    };
    tracks[4] = {
        // upper_l
        key(ms {0}, {-22, -50}, degree_f {-69.0f}),
        key(ms {400}, {-22, -50}, degree_f {-17.0f}),
        key(ms {800}, {-22, -50}, degree_f {-80.0f}),
        key(ms {1200}, {-22, -50}, degree_f {11.0f}),
        key(ms {1600}, {-22, -50}, degree_f {-69.0f}),
        key(ms {2000}, {-22, -50}, degree_f {-17.0f}),
        key(ms {2400}, {-22, -50}, degree_f {-69.0f}),
    };
    tracks[5] = {
        // lower_l
        key(ms {0}, {0, -28}, degree_f {11.0f}),
        key(ms {400}, {0, -28}, degree_f {69.0f}),
        key(ms {800}, {0, -28}, degree_f {6.0f}),
        key(ms {1200}, {0, -28}, degree_f {80.0f}),
        key(ms {1600}, {0, -28}, degree_f {11.0f}),
        key(ms {2000}, {0, -28}, degree_f {69.0f}),
        key(ms {2400}, {0, -28}, degree_f {11.0f}),
    };
    tracks[6] = {
        // hand_l
        key(ms {0}, {0, -20}, degree_f {-23.0f}),
        key(ms {600}, {0, -20}, degree_f {34.0f}),
        key(ms {1200}, {0, -20}, degree_f {-34.0f}),
        key(ms {1800}, {0, -20}, degree_f {23.0f}),
        key(ms {2400}, {0, -20}, degree_f {-23.0f}),
    };
    tracks[7] = {
        // upper_r
        key(ms {0}, {22, -50}, degree_f {11.0f}),
        key(ms {400}, {22, -50}, degree_f {-80.0f}),
        key(ms {800}, {22, -50}, degree_f {-17.0f}),
        key(ms {1200}, {22, -50}, degree_f {-69.0f}),
        key(ms {1600}, {22, -50}, degree_f {11.0f}),
        key(ms {2000}, {22, -50}, degree_f {-80.0f}),
        key(ms {2400}, {22, -50}, degree_f {11.0f}),
    };
    tracks[8] = {
        // lower_r
        key(ms {0}, {0, -28}, degree_f {80.0f}),
        key(ms {400}, {0, -28}, degree_f {6.0f}),
        key(ms {800}, {0, -28}, degree_f {69.0f}),
        key(ms {1200}, {0, -28}, degree_f {11.0f}),
        key(ms {1600}, {0, -28}, degree_f {80.0f}),
        key(ms {2000}, {0, -28}, degree_f {6.0f}),
        key(ms {2400}, {0, -28}, degree_f {80.0f}),
    };
    tracks[9] = {
        // hand_r
        key(ms {0}, {0, -20}, degree_f {34.0f}),
        key(ms {600}, {0, -20}, degree_f {-23.0f}),
        key(ms {1200}, {0, -20}, degree_f {23.0f}),
        key(ms {1800}, {0, -20}, degree_f {-34.0f}),
        key(ms {2400}, {0, -20}, degree_f {34.0f}),
    };
    tracks[10] = {
        // thigh_l
        key(ms {0}, {-15, 0}, degree_f {0.0f}),
        key(ms {300}, {-15, 0}, degree_f {-46.0f}),
        key(ms {600}, {-15, 0}, degree_f {6.0f}),
        key(ms {1200}, {-15, 0}, degree_f {-34.0f}),
        key(ms {1800}, {-15, 0}, degree_f {6.0f}),
        key(ms {2400}, {-15, 0}, degree_f {0.0f}),
    };
    tracks[11] = {
        // shin_l
        key(ms {0}, {0, 28}, degree_f {6.0f}),
        key(ms {300}, {0, 28}, degree_f {80.0f}),
        key(ms {600}, {0, 28}, degree_f {6.0f}),
        key(ms {1200}, {0, 28}, degree_f {69.0f}),
        key(ms {1800}, {0, 28}, degree_f {6.0f}),
        key(ms {2400}, {0, 28}, degree_f {6.0f}),
    };
    tracks[12] = {
        // foot_l
        key(ms {0}, {0, 24}, degree_f {0.0f}),
        key(ms {300}, {0, 24}, degree_f {-29.0f}),
        key(ms {600}, {0, 24}, degree_f {11.0f}),
        key(ms {1200}, {0, 24}, degree_f {-17.0f}),
        key(ms {2400}, {0, 24}, degree_f {0.0f}),
    };
    tracks[13] = {
        // thigh_r
        key(ms {0}, {15, 0}, degree_f {6.0f}),
        key(ms {600}, {15, 0}, degree_f {-34.0f}),
        key(ms {1200}, {15, 0}, degree_f {0.0f}),
        key(ms {1500}, {15, 0}, degree_f {-46.0f}),
        key(ms {1800}, {15, 0}, degree_f {6.0f}),
        key(ms {2400}, {15, 0}, degree_f {6.0f}),
    };
    tracks[14] = {
        // shin_r
        key(ms {0}, {0, 28}, degree_f {6.0f}),
        key(ms {600}, {0, 28}, degree_f {69.0f}),
        key(ms {1200}, {0, 28}, degree_f {6.0f}),
        key(ms {1500}, {0, 28}, degree_f {80.0f}),
        key(ms {1800}, {0, 28}, degree_f {6.0f}),
        key(ms {2400}, {0, 28}, degree_f {6.0f}),
    };
    tracks[15] = {
        // foot_r
        key(ms {0}, {0, 24}, degree_f {11.0f}),
        key(ms {600}, {0, 24}, degree_f {-17.0f}),
        key(ms {1200}, {0, 24}, degree_f {0.0f}),
        key(ms {1500}, {0, 24}, degree_f {-29.0f}),
        key(ms {2400}, {0, 24}, degree_f {11.0f}),
    };

    skeletal_animation const ani {bones, milliseconds {2400}, tracks};
    create_shapes(ani.bone_count());

    _anim = std::make_shared<skeletal_animation_tween>(ani.duration(), ani);
    _anim->Value.Changed.connect([&](std::vector<transform> const& pose) {
        update_shapes(pose);
    });
    _anim->start(playback_mode::Looped);
}

void SkeletonEx::on_update(milliseconds deltaTime)
{
    _anim->update(deltaTime);
    _layer0.update(deltaTime);
}

void SkeletonEx::on_draw_to(render_target& target, transform const& xform)
{
    _layer0.draw_to(target, xform);
}

void SkeletonEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::SPACE:
        _paused = !_paused;
        _paused ? _anim->pause() : _anim->resume();
        break;
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void SkeletonEx::on_mouse_motion(mouse::motion_event const& /* ev */)
{
}

void SkeletonEx::on_mouse_button_down(mouse::button_event const& /* ev */)
{
}

void SkeletonEx::on_mouse_wheel(mouse::wheel_event const& /* ev */)
{
}

void SkeletonEx::create_shapes(isize boneCount)
{
    struct bone_visual {
        f32   w {};
        f32   h {};
        color col;
    };
    static constexpr std::array<bone_visual, 16> visuals {{
        {20, 20, colors::White},        // root
        {28, 50, colors::Coral},        // torso
        {16, 16, colors::LightCyan},    // neck
        {28, 28, colors::LightCyan},    // head
        {12, 36, colors::Orange},       // upper_l
        {10, 30, colors::Orange},       // lower_l
        {14, 14, colors::Yellow},       // hand_l
        {12, 36, colors::SkyBlue},      // upper_r
        {10, 30, colors::SkyBlue},      // lower_r
        {14, 14, colors::LightBlue},    // hand_r
        {14, 38, colors::HotPink},      // thigh_l
        {12, 34, colors::HotPink},      // shin_l
        {18, 12, colors::Pink},         // foot_l
        {14, 38, colors::MediumPurple}, // thigh_r
        {12, 34, colors::MediumPurple}, // shin_r
        {18, 12, colors::Plum},         // foot_r
    }};

    _material->create_pass();
    _layer0.clear();
    _slots.clear();

    for (isize i {0}; i < boneCount; ++i) {
        auto& shape {_layer0.create_shape<gfx::rect_shape>()};
        shape.Material = _material;
        shape.Color    = visuals[i].col;
        shape.Bounds   = {0, 0, visuals[i].w, visuals[i].h};
        _slots.push_back(&shape);
    }
}

void SkeletonEx::update_shapes(std::vector<transform> const& pose)
{
    for (isize i {0}; i < std::ssize(_slots); ++i) {
        point_f const pos {pose[i].transform_point({0, 0})};
        _slots[i]->Bounds = {pos.X, pos.Y, _slots[i]->Bounds->width(), _slots[i]->Bounds->height()};
    }
}
