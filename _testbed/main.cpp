// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "Misc.hpp"

#include <tcob/tcob.hpp>

auto main(int argc, char* argv[]) -> int
{
    tcob::game game {{.Path = argv[0],
                      .Name = "TestGame"}};
    game.push_scene<MiscScene>();
    game.start();
    return 0;
}
