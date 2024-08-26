// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "emscripten/console.h"
#include <tcob/tcob.hpp>

using namespace tcob;

class my_scene : public tcob::scene {
public:
    my_scene(tcob::game& game)
        : tcob::scene {game}
    {
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

    void on_draw_to(tcob::gfx::render_target& target) override
    {
        _layer1->draw_to(target);
    }

    void on_update(tcob::milliseconds deltaTime) override
    {
        _layer1->update(deltaTime);
    }

private:
    std::shared_ptr<gfx::static_shape_batch> _layer1;
    assets::manual_asset_ptr<gfx::material>  _mat;
};

auto main() -> int
{
    tcob::game game {{.Path    = ".",
                      .Name    = "Empty",
                      .LogFile = "stdout"}};

    emscripten_console_log("push");
    game.push_scene<my_scene>();
    game.start();
    return 0;
}
