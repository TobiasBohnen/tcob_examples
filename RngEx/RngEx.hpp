// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

#include <iomanip>
#include <iostream>

// TODO: add ui
class RngEx : public scene {
public:
    RngEx(game& game);
    ~RngEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;

private:
    void draw_dice(auto&& dice)
    {
        auto               rolls {dice.roll_n(100'000)};
        std::map<i32, i32> hist;
        for (int n = 0; n < 100'000; ++n) {
            ++hist[rolls[n]];
        }
        for (auto p : hist) {
            std::cout << std::setfill('0') << std::setw(2) << p.first << ' ' << std::string(p.second / 250, '*') << '\n';
        }
    }

    void draw_rng(auto&& rng, f32 scale = 1.0)
    {
        std::map<f32, i32> hist;
        for (int n = 0; n < 10'000; ++n) {
            ++hist[static_cast<i32>(rng() * scale)];
        }
        for (auto p : hist) {
            if (p.second / 100 > 0) {
                std::cout << std::setfill('0') << std::setw(2) << p.first << ' ' << std::string(p.second / 100, '*') << '\n';
            }
        }
    }
};
