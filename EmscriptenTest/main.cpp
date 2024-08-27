// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "emscripten/console.h"
#include <tcob/tcob.hpp>

using namespace tcob;

class my_scene : public scene {
public:
    my_scene(game& game)
        : scene {game}
    {
        auto& resMgr {game.get_library()};
        auto& resGrp {resMgr.create_or_get_group("res")};
        resGrp.mount("./emtest.zip");
        resMgr.load_all_groups();
        _music0 = resGrp.get<audio::music>("test");
        _music0->play();

        rng                                      rnd;
        std::vector<std::shared_ptr<gfx::shape>> sprites;
        for (i32 i {0}; i < 500; i++) {
            f32 x {rnd(0.0f, 1200.f)};
            f32 y {rnd(0.0f, 1200.f)};
            f32 r {rnd(0.0f, 20.f)};
            i32 seg {rnd(4, 9)};
            i32 rot {rnd(0, 360)};

            if (i % 2 == 0) {
                auto shape0 {std::make_shared<gfx::circle_shape>()};
                shape0->Material = _mat;
                shape0->Color    = colors::Red;
                shape0->Center   = {x, y};
                shape0->Radius   = r;
                shape0->Segments = seg;
                shape0->Rotation = rot;
                sprites.push_back(shape0);
            } else {
                auto shape0 {std::make_shared<gfx::rect_shape>()};
                shape0->Material = _mat;
                shape0->Color    = colors::Blue;
                shape0->Bounds   = {x, y, r, r};
                shape0->Rotation = rot;
                sprites.push_back(shape0);
            }
        }
        _layer1 = std::make_shared<gfx::static_shape_batch>(sprites);
        logger::Info("ok");
    }

    void on_draw_to(gfx::render_target& target) override
    {
        _layer1->draw_to(target);
    }

    void on_update(milliseconds deltaTime) override
    {
        _layer1->update(deltaTime);
    }

private:
    std::shared_ptr<gfx::static_shape_batch> _layer1;
    assets::manual_asset_ptr<gfx::material>  _mat;
    assets::asset_ptr<audio::music>          _music0;
};

auto main() -> int
{
    data::config::object config;

    data::video_config video;
    video.FrameLimit           = 60;
    video.FullScreen           = false;
    video.UseDesktopResolution = false;
    video.VSync                = false;
    video.Resolution           = size_i {800, 600};
    video.RenderSystem         = "OPENGLES30";
    config["video"]            = video;

    game game {{.Path           = ".",
                .Name           = "EMEX",
                .LogFile        = "stdout",
                .ConfigDefaults = config,
                .WorkerThreads  = 0}};

    game.push_scene<my_scene>();
    game.start();
    return 0;
}
