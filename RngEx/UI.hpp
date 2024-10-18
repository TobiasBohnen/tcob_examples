// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::gfx::ui;

////////////////////////////////////////////////////////////

class rng_form : public form {
public:
    rng_form(window* window);

private:
    void gen_styles();

    void draw_dice(auto&& dice);

    std::shared_ptr<canvas_widget>        _canvas;
    assets::manual_asset_ptr<font_family> _font;
};

////////////////////////////////////////////////////////////

inline void rng_form::draw_dice(auto&& dice)
{
    auto* font {_font->get_font({}, 24).get_ptr()};

    auto const bounds {_canvas->get_content_bounds()};
    _canvas->clear();

    _canvas->begin_path();
    _canvas->rect(bounds.as_moved_to(point_f::Zero));
    _canvas->set_fill_style(colors::LightGray);
    _canvas->fill();

    u64 const seed      = static_cast<u64>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    i32       rollCount = 100'000;

    auto               rolls {dice(rollCount, seed)};
    std::map<i32, i32> hist;
    for (int n = 0; n < rollCount; ++n) { ++hist[rolls[n]]; }
    i32 max {0};
    for (auto p : hist) { max = std::max(p.second, max); }

    f32 const xoff {bounds.width() / hist.size()};
    f32 const barOffset {1.2f};
    i32 const blockCount {50};
    f32 const barHeight {(bounds.height() - font->get_info().LineHeight * 2.2f) / blockCount / barOffset};

    point_f pos {0, 0};

    for (auto p : hist) {
        _canvas->set_fill_style(colors::Black);
        _canvas->set_font(font);
        _canvas->draw_textbox({pos + point_f {xoff / 2, 0}, size_f {50, 50}}, std::to_string(p.first));
        _canvas->draw_textbox({pos + point_f {xoff / 4, font->get_info().LineHeight}, size_f {100, 100}}, std::to_string(p.second));

        i32 const count {static_cast<i32>(std::round(static_cast<f32>(p.second) / max * blockCount))};
        f32 const totalBarHeight {barHeight * count * barOffset};
        for (i32 i {0}; i < count; ++i) {
            _canvas->begin_path();
            _canvas->rect({{pos.X + xoff / 10, bounds.height() - (barHeight * i * barOffset)}, {xoff / 5 * 4, barHeight}});
            _canvas->set_fill_style(colors::DimGray);
            _canvas->fill();
        }

        pos += point_f {xoff, 0};
    }
}
