// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "Misc.hpp"

#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

using namespace std::chrono_literals;
using namespace tcob::literals;

MiscScene::MiscScene(game& game)
    : scene(game)
{
}

MiscScene::~MiscScene() = default;

void MiscScene::on_start()
{
    auto& resMgr {library()};

    lua::script script;
    if (io::exists("bootstrap.lua")) {
        auto result {script.run_file<std::unordered_map<std::string, std::vector<std::string>>>("bootstrap.lua")};
        if (result.has_value()) {
            for (auto& [playlistName, sources] : result.value()) {
                auto& playlist {resMgr.create_or_get_group(playlistName)};
                for (auto& source : sources) {
                    playlist.mount(source);
                }
            }
        }
    }

    resMgr.load_all_groups();
    window().load_icon("res/tex/testing.webp");

    auto* resGrp {resMgr.get_group("res")};

    auto defaultCursor {resGrp->get<cursor>("default")};
    window().Cursor           = defaultCursor;
    defaultCursor->ActiveMode = "cursor1";

    rng                                      rnd;
    std::vector<std::shared_ptr<gfx::shape>> sprites;

    _layer1 = std::make_shared<static_shape_batch>(sprites);

    _font = resGrp->get<font>("defaultFont");

    _text         = std::make_shared<text>(_font);
    _text->Text   = "SÖ5";
    // text.text("ö±\nad to");
    _text->Bounds = {{800, 600}, {400, 60}};
    _text->Style  = {.Color = colors::White, .Alignment = {horizontal_alignment::Left, vertical_alignment::Top}, .KerningEnabled = true};

    /*
        _aniTexSprite           = _layer0.create_shape<gfx::rect_shape>();
        _aniTexSprite->Bounds   = {{450, 0}, {320, 240}};
        _aniTexSprite->Material = resGrp->get<material>("aniSpriteMat");

        auto sprite1      = _layer0.create_shape<gfx::rect_shape>();
        sprite1->Bounds   = {point_f::Zero, {320, 240}};
        sprite1->Material = resGrp->get<material>("uniforms-buffer-test");
        _uniBuf.bind_base(1);
        std::array<f32, 5> bufData {1, 0, 0, 1, 0.5f};
        _uniBuf.update<f32>(bufData, 0);
    */

    _audioPlaylist.add("mp3", resGrp->get<sound>("mp3-test").ptr());
    _audioPlaylist.add("wav", resGrp->get<sound>("wav-test").ptr());
    _audioPlaylist.add("ogg", resGrp->get<sound>("ogg-test").ptr());
    _audioPlaylist.add("flac", resGrp->get<sound>("flac-test").ptr());
    _audioPlaylist.add("it", resGrp->get<sound>("it-test").ptr());
    _audioPlaylist.add("opus", resGrp->get<sound>("opus-test").ptr());

    _music0 = resGrp->get<music>("test");

    _shape0           = _layer0.create_shape<gfx::poly_shape>();
    _shape0->Color    = colors::Blue;
    _shape0->Material = material::Empty();
    _shape0->Polygons = {path2d::Parse(
                             "M 150 100 l 50 50 l 0 100 l -50 50 l 300 0 l 0 -200 z" // outline
                             "M 250 150 l 50 0 l 0 50 l -50 0 z"                     // hole
                             )
                             ->polygonize()};
    _shape0->Rotation = degree_f {0};
}

void MiscScene::on_finish()
{
    _timer.stop();
    _music0->stop();
}

void MiscScene::on_draw_to(render_target& target)
{
    /*

    _text.draw_to(target);
   _poly.render_to_target(target);
      */
    _layer0.draw_to(target);
    _layer1->draw_to(target);
    //
}

void MiscScene::on_update(milliseconds deltaTime)
{
    /*
    _text.update(deltaTime);
    _rvc.update(deltaTime);

    */
    _layer0.update(deltaTime);

    asset_ptr<animated_texture> aniTex = library().get_group("res")->get<texture>("test-ani");
    aniTex->update(deltaTime);
}

void MiscScene::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().stats()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();
    if (_music0.is_ready()) {
        stream << "|" << _music0->duration().count() / 1000;
        stream << "|" << _music0->playback_position().count() / 1000;
    }

    window().Title = "TestGame " + stream.str();
}

void MiscScene::on_key_down(keyboard::event const& ev)
{
    float moveFactor {10};
    auto& camera {window().camera()};
    auto& resMgr {library()};
    auto* resGrp {resMgr.get_group("res")};

    if (ev.ScanCode == scan_code::D2) {
    } else if (ev.ScanCode == scan_code::D3) {
        if (!resMgr.is_loading_complete()) {
            return;
        }
        resMgr.unload_all_groups();
        resMgr.load_all_groups();

        window().Cursor->ActiveMode = "cursor1";
        _text->force_reshape();

    } else if (ev.ScanCode == scan_code::D5) {
        asset_ptr<animated_texture> aniTex = resMgr.get_group("res")->get<texture>("test-ani");
        aniTex->start(true);
    } else if (ev.ScanCode == scan_code::D6) {
        std::array<f32, 5> bufData {0, 1, 0, 1, 1};
        _uniBuf.update<f32>(bufData, 0);
        _aniTexSprite->rotate_by(degree_f {45});
    } else if (ev.ScanCode == scan_code::D7) {
        asset_ptr<animated_texture> aniTex = resMgr.get_group("res")->get<texture>("test-ani");
        aniTex->toggle_pause();
    } else if (ev.ScanCode == scan_code::A) {
        camera.move_by({-moveFactor, 0});
    } else if (ev.ScanCode == scan_code::D) {
        camera.move_by({moveFactor, 0});
    } else if (ev.ScanCode == scan_code::S) {
        camera.move_by({0.0f, moveFactor});
    } else if (ev.ScanCode == scan_code::W) {
        camera.move_by({0.0f, -moveFactor});
    } else if (ev.ScanCode == scan_code::Q) {
        camera.zoom_by({1.25f, 1.25f});
    } else if (ev.ScanCode == scan_code::E) {
        camera.zoom_by({0.8f, 0.8f});
    } else if (ev.ScanCode == scan_code::C) {
        window().Cursor = nullptr;
    } else if (ev.ScanCode == scan_code::KP_1) {
        _audioPlaylist.queue("wav");
        _audioPlaylist.queue("mp3");
        _audioPlaylist.queue("ogg");
        _audioPlaylist.queue("flac");
        _audioPlaylist.queue("it");
        _audioPlaylist.queue("opus");
    } else if (ev.ScanCode == scan_code::KP_2) {
        _audioPlaylist.play("wav");
        _audioPlaylist.play("mp3");
        _audioPlaylist.play("ogg");
        _audioPlaylist.play("flac");
        _audioPlaylist.play("it");
        _audioPlaylist.play("opus");
    } else if (ev.ScanCode == scan_code::KP_7) {
        _sound_speech0 = speech_generator {}.create_sound("1 2 3 4 5 6 7 8 9 0");
        _sound_speech0->play();
    } else if (ev.ScanCode == scan_code::K) {
        _music0->play();
    } else if (ev.ScanCode == scan_code::M) {
        static std::vector<animated_image_encoder::frame> frames;

        if (frames.size() < 5) {
            auto img {window().copy_to_image()};
            frames.push_back({.Image = img, .TimeStamp = milliseconds {frames.size() * 250}});
        }

        if (frames.size() == 5) {
            io::ofstream of {"test_ani1.webp"};
            locate_service<animated_image_encoder::factory>().create(".webp")->encode(frames, of);
            frames.clear();
        }
        // window().copy_to_image().save("screen1abc.png");
    } else if (ev.ScanCode == scan_code::BACKSPACE) {
        parent().pop_current_scene();
    }
}

void MiscScene::on_mouse_motion(mouse::motion_event const& ev)
{
}

void MiscScene::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _shape0->Rotation += degree_f {15.f};
}
