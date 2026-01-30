// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "UI.hpp"

#include "../_common/Trim.hpp"

using namespace tcob::literals;
using namespace std::chrono_literals;
using namespace tcob::random;

////////////////////////////////////////////////////////////

crtl_form::crtl_form(window& window)
    : form {{"music", window.bounds()}}
    , _font {"trim", "trim"}
{
    font_family::SingleFont(*_font.ptr(), std::as_bytes(std::span {trim_ttf}));
    gen_styles();

    auto& mainPanel {create_container<glass>(dock_style::Fill, "main")};
    auto& mainPanelLayout {mainPanel.create_layout<grid_layout>(size_i {12, 20})};

    auto& btn0 {mainPanelLayout.create_widget<button>({1, 1, 2, 4}, "mp3")};
    btn0.Label = "mp3";
    btn0.Click.connect([&] { Play("res/music0.mp3"); });

    auto& btn1 {mainPanelLayout.create_widget<button>({3, 1, 2, 4}, "ogg")};
    btn1.Label = "ogg";
    btn1.Click.connect([&] { Play("res/music0.ogg"); });

    auto& btn2 {mainPanelLayout.create_widget<button>({5, 1, 2, 4}, "wav")};
    btn2.Label = "wav";
    btn2.Click.connect([&] { Play("res/music0.wav"); });

    auto& btn4 {mainPanelLayout.create_widget<button>({7, 1, 2, 4}, "flac")};
    btn4.Label = "flac";
    btn4.Click.connect([&] { Play("res/music0.flac"); });
}

void crtl_form::gen_styles()
{
    style_collection styles;
    {
        auto style {styles.create<button>("button", {})};
        style->Border.Type    = border_type::Solid;
        style->Border.Size    = 3_px;
        style->Border.Radius  = 5_px;
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 50_pct;
        style->Text.Alignment = {horizontal_alignment::Centered, vertical_alignment::Top};
        style->Text.AutoSize  = auto_size_mode::OnlyShrink;
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
        style->Border.Type    = border_type::Solid;
        style->Border.Size    = 3_px;
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 40_pct;
        style->Text.Alignment = {horizontal_alignment::Centered, vertical_alignment::Middle};
        style->Text.AutoSize  = auto_size_mode::OnlyShrink;
        style->Margin         = {2_px};
        style->Padding        = {2_px};

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
        style->Text.Color        = colors::Black;
    }
    {
        auto style {styles.create<slider>("slider", {})};
        style->Margin            = {5_px};
        style->Padding           = {2_px};
        style->ThumbClass        = "slider_thumb";
        style->Bar.Size          = 95_pct;
        style->Bar.Delay         = 0ms;
        style->Bar.Border.Size   = 3_px;
        style->Bar.Border.Radius = 5_px;

        style->Bar.HigherBackground = colors::LightGray;
        style->Bar.LowerBackground  = colors::LightGray;
    }
    {
        auto style {styles.create<thumb_style>("slider_thumb", {}, {})};
        style->Thumb.Type          = thumb_type::Rect;
        style->Thumb.LongSide      = 25_pct;
        style->Thumb.ShortSide     = 80_pct;
        style->Thumb.Border.Size   = 3_px;
        style->Thumb.Border.Radius = 5_px;

        style->Thumb.Background        = colors::LightGray;
        style->Thumb.Border.Background = colors::Black;

        auto activeStyle {styles.create<thumb_style>("slider_thumb", {.Active = true})};
        *activeStyle = *style;

        activeStyle->Thumb.Background        = colors::Black;
        activeStyle->Thumb.Border.Background = colors::LightGray;

        auto hoverStyle {styles.create<thumb_style>("slider_thumb", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Thumb.Background        = colors::LightGray;
        hoverStyle->Thumb.Border.Background = colors::DarkGray;
    }
    {
        auto style {styles.create<drop_down_list>("drop_down_list", {})};
        style->Border.Size    = 3_px;
        style->Border.Radius  = 5_px;
        style->Margin         = {2_px};
        style->Padding        = {5_px};
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 50_pct;
        style->Text.Alignment = {horizontal_alignment::Centered, vertical_alignment::Middle};
        style->Text.AutoSize  = auto_size_mode::OnlyShrink;
        style->ItemHeight     = 100_pct;
        style->ItemClass      = "list_items";

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
        style->Text.Color        = colors::Black;
    }
    {
        auto style {styles.create<item_style>("list_items", {}, {})};
        style->Item.Padding        = {2_px};
        style->Item.Text.Style     = {false, font::weight::Normal};
        style->Item.Text.Font      = _font;
        style->Item.Text.Size      = 50_pct;
        style->Item.Text.Alignment = {horizontal_alignment::Left, vertical_alignment::Middle};
        style->Item.Text.AutoSize  = auto_size_mode::OnlyShrink;
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
    {
        auto style {styles.create<nav_arrows_style>("nav_arrows", {}, {})};
        style->NavArrow.Size.Height   = {0.75f, length::type::Relative};
        style->NavArrow.Size.Width    = {0.15f, length::type::Relative};
        style->NavArrow.Border.Size   = 3_px;
        style->NavArrow.Border.Radius = 0_px;

        style->NavArrow.Foreground        = colors::Black;
        style->NavArrow.UpBackground      = colors::LightGray;
        style->NavArrow.DownBackground    = colors::LightGray;
        style->NavArrow.Border.Background = colors::Black;

        auto hoverStyle {styles.create<nav_arrows_style>("nav_arrows", {.Hover = true})};
        hoverStyle->NavArrow = style->NavArrow;

        hoverStyle->NavArrow.Foreground        = colors::LightGray;
        hoverStyle->NavArrow.UpBackground      = colors::Black;
        hoverStyle->NavArrow.DownBackground    = colors::Black;
        hoverStyle->NavArrow.Border.Background = colors::LightGray;
    }
    Styles = styles;
}
