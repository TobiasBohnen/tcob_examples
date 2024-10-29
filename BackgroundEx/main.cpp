// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "BackgroundEx.hpp"

#include <tcob/tcob.hpp>

#if defined(__EMSCRIPTEN__)
auto main(void) -> int
{
    tcob::data::config::object config;

    tcob::gfx::video_config video;
    video.FrameLimit           = 100;
    video.FullScreen           = false;
    video.UseDesktopResolution = false;
    video.VSync                = false;
    video.Resolution           = tcob::size_i {800, 600};
    video.RenderSystem         = "OPENGLES30";
    config["video"]            = video;

    tcob::game game {{.Path           = ".",
                      .Name           = "BackgroundEx",
                      .LogFile        = "stdout",
                      .ConfigDefaults = config,
                      .WorkerThreads  = 8}};
#else
auto main(int /* argc */, char* argv[]) -> int
{
    tcob::game game {{.Path    = argv[0],
                      .Name    = "BackgroundEx",
                      .LogFile = "stdout"}};
#endif

    auto& resMgr {game.get_library()};
    auto& resGrp {resMgr.create_or_get_group("res")};
    resGrp.mount("./backgroundex.zip");
    resMgr.load_all_groups();

    game.push_scene<BackgroundEx>();
    game.start();
    return 0;
}
