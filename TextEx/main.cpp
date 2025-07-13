// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "TextEx.hpp"

#include <tcob/tcob.hpp>

#if defined(__EMSCRIPTEN__)
auto main(void) -> int
{
    tcob::data::object config;

    tcob::gfx::video_config video;
    video.FrameLimit           = 100;
    video.FullScreen           = false;
    video.UseDesktopResolution = false;
    video.VSync                = false;
    video.Resolution           = tcob::size_i {800, 600};
    video.RenderSystem         = "OPENGLES30";
    config["video"]            = video;

    tcob::game game {{.Name           = "TextEx",
                      .LogFile        = "stdout",
                      .ConfigDefaults = config,
                      .WorkerThreads  = 8}};
#else
auto main(int /* argc */, char* argv[]) -> int
{
    tcob::game game {{.Name    = "TextEx",
                      .LogFile = "stdout"}};
#endif

    auto& resMgr {game.library()};
    auto& resGrp {resMgr.create_or_get_group("text")};
    resGrp.mount("./textex.zip");
    resMgr.load_all_groups();

    game.push_scene<TextEx>();
    game.start();
    return 0;
}
