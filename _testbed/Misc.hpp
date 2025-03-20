// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <tcob/tcob.hpp>
using namespace tcob;
using namespace tcob::gfx;
using namespace tcob::assets;
using namespace tcob::scripting;
using namespace tcob::physics;
using namespace tcob::audio;
using namespace tcob::data;
using namespace tcob::input;

////////////////////////////////////////////////////////////

class MiscScene : public scene {
public:
    MiscScene(game& game);
    ~MiscScene() override;

protected:
    void on_start() override;
    void on_finish() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_motion(mouse::motion_event const& ev) override;

    void on_mouse_wheel(mouse::wheel_event const& ev) override;

private:
    shape_batch                      _layer0;
    std::shared_ptr<gfx::poly_shape> _shape0;

    std::shared_ptr<static_shape_batch> _layer1;
    assets::asset_ptr<font>             _font;
    std::shared_ptr<text>               _text;
    std::shared_ptr<gfx::rect_shape>    _aniTexSprite;
    tween_queue                         _rvc;

    assets::asset_ptr<sound> _sound_mp3;
    assets::asset_ptr<sound> _sound_opus;
    assets::asset_ptr<sound> _sound_wav;
    assets::asset_ptr<sound> _sound_ogg;
    assets::asset_ptr<sound> _sound_flac;
    assets::asset_ptr<sound> _sound_it;

    std::shared_ptr<sound> _sound_speech0;

    assets::asset_ptr<music> _music0;

    timer                        _timer;
    std::unique_ptr<lua::script> _soundScript;

    uniform_buffer _uniBuf {sizeof(vec4) + sizeof(f32)};
    uniform_buffer _uniPoints {sizeof(uvec2)};
};
