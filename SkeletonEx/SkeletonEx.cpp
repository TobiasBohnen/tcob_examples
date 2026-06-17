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
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} | {}",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y, _anim->playback_progress());
}

void SkeletonEx::on_start()
{
    auto key {[](milliseconds dur, point_f tr, degree_f ro, size_f sc = {1, 1}) {
        return skeletal_animation::frame {.Duration = dur, .Translation = tr, .Rotation = radian_f {ro}, .Scale = sc};
    }};

    using ms = milliseconds;

    std::vector<skeletal_animation::bone> const bones {
        {.Name = "root", .Track = {
                             key(ms {300}, {400, 300}, degree_f {0}),
                             key(ms {300}, {410, 290}, degree_f {6}),
                             key(ms {300}, {420, 300}, degree_f {0}),
                             key(ms {300}, {410, 308}, degree_f {-6}),
                             key(ms {300}, {400, 300}, degree_f {0}),
                             key(ms {300}, {390, 290}, degree_f {6}),
                             key(ms {300}, {380, 300}, degree_f {0}),
                             key(ms {300}, {390, 308}, degree_f {-6}),
                         }},
        {.Name = "torso", .Parent = "root", .Track = {
                                                key(ms {600}, {0, -70}, degree_f {-12}),
                                                key(ms {600}, {0, -70}, degree_f {17}),
                                                key(ms {600}, {0, -70}, degree_f {-12}),
                                                key(ms {600}, {0, -70}, degree_f {17}),
                                            }},
        {.Name = "neck", .Parent = "torso", .Track = {
                                                key(ms {600}, {0, -20}, degree_f {0}),
                                                key(ms {600}, {0, -20}, degree_f {45}),
                                                key(ms {600}, {0, -20}, degree_f {0}),
                                                key(ms {600}, {0, -20}, degree_f {-75}),
                                            }},
        {.Name = "head", .Parent = "neck", .Track = {
                                               key(ms {400}, {0, -30}, degree_f {9}),
                                               key(ms {400}, {0, -28}, degree_f {-9}),
                                               key(ms {400}, {0, -30}, degree_f {9}),
                                               key(ms {400}, {0, -28}, degree_f {-9}),
                                               key(ms {400}, {0, -30}, degree_f {9}),
                                               key(ms {400}, {0, -28}, degree_f {-9}),
                                           }},
        {.Name = "upper_l", .Parent = "torso", .Track = {
                                                   key(ms {400}, {-22, -50}, degree_f {-69}),
                                                   key(ms {400}, {-22, -50}, degree_f {-17}),
                                                   key(ms {400}, {-22, -50}, degree_f {-80}),
                                                   key(ms {400}, {-22, -50}, degree_f {11}),
                                                   key(ms {400}, {-22, -50}, degree_f {-69}),
                                                   key(ms {400}, {-22, -50}, degree_f {-17}),
                                               }},
        {.Name = "lower_l", .Parent = "upper_l", .Track = {
                                                     key(ms {400}, {0, -28}, degree_f {11}),
                                                     key(ms {400}, {0, -28}, degree_f {69}),
                                                     key(ms {400}, {0, -28}, degree_f {6}),
                                                     key(ms {400}, {0, -28}, degree_f {80}),
                                                     key(ms {400}, {0, -28}, degree_f {11}),
                                                     key(ms {400}, {0, -28}, degree_f {69}),
                                                 }},
        {.Name = "hand_l", .Parent = "lower_l", .Track = {
                                                    key(ms {600}, {0, -20}, degree_f {-23}),
                                                    key(ms {600}, {0, -20}, degree_f {34}),
                                                    key(ms {600}, {0, -20}, degree_f {-34}),
                                                    key(ms {600}, {0, -20}, degree_f {23}),
                                                }},
        {.Name = "upper_r", .Parent = "torso", .Track = {
                                                   key(ms {400}, {22, -50}, degree_f {11}),
                                                   key(ms {400}, {22, -50}, degree_f {-80}),
                                                   key(ms {400}, {22, -50}, degree_f {-17}),
                                                   key(ms {400}, {22, -50}, degree_f {-69}),
                                                   key(ms {400}, {22, -50}, degree_f {11}),
                                                   key(ms {400}, {22, -50}, degree_f {-80}),
                                               }},
        {.Name = "lower_r", .Parent = "upper_r", .Track = {
                                                     key(ms {400}, {0, -28}, degree_f {80}),
                                                     key(ms {400}, {0, -28}, degree_f {6}),
                                                     key(ms {400}, {0, -28}, degree_f {69}),
                                                     key(ms {400}, {0, -28}, degree_f {11}),
                                                     key(ms {400}, {0, -28}, degree_f {80}),
                                                     key(ms {400}, {0, -28}, degree_f {6}),
                                                 }},
        {.Name = "hand_r", .Parent = "lower_r", .Track = {
                                                    key(ms {600}, {0, -20}, degree_f {34}),
                                                    key(ms {600}, {0, -20}, degree_f {-23}),
                                                    key(ms {600}, {0, -20}, degree_f {23}),
                                                    key(ms {600}, {0, -20}, degree_f {-34}),
                                                }},
        {.Name = "thigh_l", .Parent = "root", .Track = {
                                                  key(ms {300}, {-15, 0}, degree_f {0}),
                                                  key(ms {300}, {-15, 0}, degree_f {-46}),
                                                  key(ms {600}, {-15, 0}, degree_f {6}),
                                                  key(ms {600}, {-15, 0}, degree_f {-34}),
                                                  key(ms {600}, {-15, 0}, degree_f {6}),
                                              }},
        {.Name = "shin_l", .Parent = "thigh_l", .Track = {
                                                    key(ms {300}, {0, 28}, degree_f {6}),
                                                    key(ms {300}, {0, 28}, degree_f {80}),
                                                    key(ms {600}, {0, 28}, degree_f {6}),
                                                    key(ms {600}, {0, 28}, degree_f {69}),
                                                    key(ms {600}, {0, 28}, degree_f {6}),
                                                }},
        {.Name = "foot_l", .Parent = "shin_l", .Track = {
                                                   key(ms {300}, {0, 24}, degree_f {0}),
                                                   key(ms {300}, {0, 24}, degree_f {-29}),
                                                   key(ms {600}, {0, 24}, degree_f {11}),
                                                   key(ms {600}, {0, 24}, degree_f {-17}),
                                                   key(ms {600}, {0, 24}, degree_f {0}),
                                               }},
        {.Name = "thigh_r", .Parent = "root", .Track = {
                                                  key(ms {600}, {15, 0}, degree_f {6}),
                                                  key(ms {600}, {15, 0}, degree_f {-34}),
                                                  key(ms {300}, {15, 0}, degree_f {0}),
                                                  key(ms {300}, {15, 0}, degree_f {-46}),
                                                  key(ms {600}, {15, 0}, degree_f {6}),
                                              }},
        {.Name = "shin_r", .Parent = "thigh_r", .Track = {
                                                    key(ms {600}, {0, 28}, degree_f {6}),
                                                    key(ms {600}, {0, 28}, degree_f {69}),
                                                    key(ms {300}, {0, 28}, degree_f {6}),
                                                    key(ms {300}, {0, 28}, degree_f {80}),
                                                    key(ms {600}, {0, 28}, degree_f {6}),
                                                }},
        {.Name = "foot_r", .Parent = "shin_r", .Track = {
                                                   key(ms {600}, {0, 24}, degree_f {11}),
                                                   key(ms {600}, {0, 24}, degree_f {-17}),
                                                   key(ms {300}, {0, 24}, degree_f {0}),
                                                   key(ms {300}, {0, 24}, degree_f {-29}),
                                                   key(ms {600}, {0, 24}, degree_f {11}),
                                               }},
    };

    skeletal_animation const ani {bones};
    create_shapes(ani.bone_count());

    _anim = std::make_shared<skeletal_animation_tween>(ani.duration(), ani);
    _anim->Value.Changed.connect([&](skeletal_animation::pose const& pose) {
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
