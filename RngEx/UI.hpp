// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::ui;

////////////////////////////////////////////////////////////

class rng_form : public form<dock_layout> {
public:
    rng_form(window& window);

private:
    void gen_styles();

    void draw_dice();
    void draw_noise();

    std::shared_ptr<canvas_widget> _canvas;
    asset_owner_ptr<font_family>   _font;
    asset_owner_ptr<texture>       _tex;
};

////////////////////////////////////////////////////////////

inline void rng_form::draw_dice()
{
    auto* font {_font->get_font({}, 24).ptr()};

    auto const bounds {_canvas->content_bounds()};
    _canvas->clear();

    _canvas->begin_path();
    _canvas->rect({point_f::Zero, bounds.Size});
    _canvas->set_fill_style(colors::LightGray);
    _canvas->fill();

    u64 const seed      = static_cast<u64>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    i32       rollCount = 100'000;

    auto               dice {random::dice<6, random::split_mix_64> {seed}};
    auto               rolls {dice.roll_n(rollCount)};
    std::map<i32, i32> hist;
    for (int n = 0; n < rollCount; ++n) { ++hist[rolls[n]]; }
    i32 max {0};
    for (auto p : hist) { max = std::max(p.second, max); }

    f32 const xoff {bounds.width() / hist.size()};
    f32 const barOffset {1.2f};
    i32 const blockCount {50};
    f32 const barHeight {(bounds.height() - font->info().LineHeight * 2.2f) / blockCount / barOffset};

    point_f pos {0, 0};

    for (auto p : hist) {
        _canvas->set_fill_style(colors::Black);
        _canvas->set_font(font);
        _canvas->draw_text({pos + point_f {xoff / 2, 0}, size_f {50, 50}}, std::to_string(p.first));
        _canvas->draw_text({pos + point_f {xoff / 4, font->info().LineHeight}, size_f {100, 100}}, std::to_string(p.second));

        i32 const count {static_cast<i32>(std::round(static_cast<f32>(p.second) / max * blockCount))};
        f32 const totalBarHeight {barHeight * count * barOffset};
        for (i32 i {0}; i < count; ++i) {
            _canvas->begin_path();
            _canvas->rect({{pos.X + (xoff / 10), bounds.height() - (barHeight * i * barOffset)}, {xoff / 5 * 4, barHeight}});
            _canvas->set_fill_style(colors::DimGray);
            _canvas->fill();
        }

        pos += point_f {xoff, 0};
    }
}

inline void rng_form::draw_noise()
{
    auto const bounds {_canvas->content_bounds()};
    _canvas->clear();

    u64 const seed {static_cast<u64>(std::chrono::high_resolution_clock::now().time_since_epoch().count())};

    auto rng {random::rng_split_mix_64 {seed}};

    _tex = {"Noise", size_i {256, 256}, 1, texture::format::RGBA8};
    std::array<color, 256 * 256> data;
    for (auto& c : data) {
        c.R = c.G = c.B = rng(0, 255);
        c.A             = 255;
    }
    _tex->update_data(data.data(), 0, 256, 1);
    _tex->regions()["default"] = {.UVRect = {0, 0, 1, 1}, .Level = 0};

    _canvas->draw_image(_tex.ptr(), "default", bounds);
}
