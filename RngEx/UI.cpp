// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "UI.hpp"

#include "../_common/Trim.hpp"

using namespace tcob::literals;
using namespace std::chrono_literals;
using namespace tcob::random;

////////////////////////////////////////////////////////////

rng_form::rng_form(window* window)
    : form {"generator", window}
    , _font {"trim", "trim"}
{
    u32 rollCount = 100'000;

    font_family::SingleFont(*_font.get_ptr(), trim_ttf);
    gen_styles();

    auto mainPanel {create_container<glass>(dock_style::Fill, "main")};
    auto mainPanelLayout {mainPanel->create_layout<grid_layout>(size_i {40, 40})};

    _canvas = {mainPanelLayout->create_widget<canvas_widget>({8, 1, 30, 38}, "uxCanvas")};

    using func = std::function<std::vector<i32>(i32, u64)>;
    auto rngEngine {mainPanelLayout->create_widget<list_box>({1, 6, 4, 25}, "uxRngEngine")};
    rngEngine->add_item({"SplitMix32", func {[](i32 rolls, u64 seed) { return dice<6, split_mix_32> {static_cast<u32>(seed)}.roll_n(rolls); }}});
    rngEngine->add_item({"SplitMix64", func {[](i32 rolls, u64 seed) { return dice<6, split_mix_64> {seed}.roll_n(rolls); }}});
    rngEngine->add_item({"GameRand", func {[](i32 rolls, u64 seed) { return dice<6, game_rand> {static_cast<u32>(seed)}.roll_n(rolls); }}});
    rngEngine->add_item({"Xorshift64", func {[](i32 rolls, u64 seed) { return dice<6, xorshift_64> {seed}.roll_n(rolls); }}});
    rngEngine->add_item({"Xorshift64*", func {[](i32 rolls, u64 seed) { return dice<6, xorshift_64_star> {seed}.roll_n(rolls); }}});
    rngEngine->add_item({"Xoroshiro128+", func {[](i32 rolls, u64 seed) { return dice<6, xoroshiro_128_plus> {seed}.roll_n(rolls); }}});
    rngEngine->add_item({"Xoroshiro128++", func {[](i32 rolls, u64 seed) { return dice<6, xoroshiro_128_plus_plus> {seed}.roll_n(rolls); }}});
    rngEngine->add_item({"Xoroshiro128**", func {[](i32 rolls, u64 seed) { return dice<6, xoroshiro_128_star_star> {seed}.roll_n(rolls); }}});
    rngEngine->add_item({"Xoshiro256+", func {[](i32 rolls, u64 seed) { return dice<6, xoshiro_256_plus> {seed}.roll_n(rolls); }}});
    rngEngine->add_item({"Xoshiro256++", func {[](i32 rolls, u64 seed) { return dice<6, xoshiro_256_plus_plus> {seed}.roll_n(rolls); }}});
    rngEngine->add_item({"Xoshiro256**", func {[](i32 rolls, u64 seed) { return dice<6, xoshiro_256_star_star> {seed}.roll_n(rolls); }}});
    rngEngine->add_item({"Well512a", func {[](i32 rolls, u64 seed) { return dice<6, well_512_a> {static_cast<u32>(seed)}.roll_n(rolls); }}});
    rngEngine->SelectedItemIndex = 0;

    auto rollDice {mainPanelLayout->create_widget<button>({1, 1, 4, 2}, "uxRollDice")};
    rollDice->Label = "Roll Dice";
    rollDice->Click.connect([this, rollCount, lsb = rngEngine.get()]() {
        auto const rng {lsb->get_selected_item()};
        draw_dice(std::any_cast<func>(rng.UserData));
    });
}

void rng_form::gen_styles()
{
    style_collection styles;
    {
        auto style {styles.create<button>("button", {})};
        style->Border.Type    = element::border::type::Solid;
        style->Border.Size    = 3_px;
        style->Border.Radius  = 5_px;
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 50_pct;
        style->Text.Alignment = {horizontal_alignment::Centered, vertical_alignment::Top};
        style->Text.AutoSize  = element::text::auto_size_mode::OnlyShrink;
        style->Margin         = {2_px};
        style->Padding        = {2_px};

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
        style->Text.Color        = colors::Black;

        auto activeStyle {styles.create<button>("button", {.Active = true, .Hover = true})};
        *activeStyle = *style;

        activeStyle->Background        = colors::Black;
        activeStyle->Border.Background = colors::LightGray;
        activeStyle->Text.Color        = colors::White;

        auto hoverStyle {styles.create<button>("button", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Background        = colors::LightGray;
        hoverStyle->Border.Background = colors::DarkGray;
        hoverStyle->Text.Color        = colors::White;
    }
    {
        auto style {styles.create<label>("label", {})};
        style->Border.Type    = element::border::type::Solid;
        style->Border.Size    = 3_px;
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 40_pct;
        style->Text.Alignment = {horizontal_alignment::Centered, vertical_alignment::Middle};
        style->Text.AutoSize  = element::text::auto_size_mode::OnlyShrink;
        style->Margin         = {2_px};
        style->Padding        = {2_px};

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
        style->Text.Color        = colors::Black;
    }
    {
        auto style {styles.create<list_box>("list_box", {})};
        style->Border.Size   = 3_px;
        style->Border.Radius = 5_px;
        style->Margin        = {2_px};
        style->Padding       = {5_px};
        style->ItemHeight    = 8_pct;
        style->ItemClass     = "list_items";

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
    }
    {
        auto style {styles.create<item_style>("list_items", {}, {})};
        style->Item.Padding        = {2_px};
        style->Item.Text.Style     = {false, font::weight::Normal};
        style->Item.Text.Font      = _font;
        style->Item.Text.Size      = 50_pct;
        style->Item.Text.Alignment = {horizontal_alignment::Left, vertical_alignment::Middle};
        style->Item.Text.AutoSize  = element::text::auto_size_mode::OnlyShrink;
        style->Item.Border.Size    = 3_px;

        style->Item.Background        = colors::LightGray;
        style->Item.Border.Background = colors::Black;
        style->Item.Text.Color        = colors::Black;

        auto hoverStyle {styles.create<item_style>("list_items", {.Hover = true})};
        hoverStyle->Item = style->Item;

        hoverStyle->Item.Background        = colors::Black;
        hoverStyle->Item.Border.Background = colors::Black;
        hoverStyle->Item.Text.Color        = colors::White;

        auto activeStyle {styles.create<item_style>("list_items", {.Active = true})};
        activeStyle->Item = style->Item;

        activeStyle->Item.Background        = colors::LightBlue;
        activeStyle->Item.Border.Background = colors::Black;
        activeStyle->Item.Text.Color        = colors::Black;
    }

    Styles = styles;
}
