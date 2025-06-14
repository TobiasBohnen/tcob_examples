// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "Misc.hpp"

#include <tcob/tcob.hpp>

auto main(int argc, char* argv[]) -> int
{
    tcob::game game {{.Name    = "TestGame",
                      .LogFile = "stdout"}};
    game.push_scene<MiscScene>();
    game.start();
    return 0;
}
