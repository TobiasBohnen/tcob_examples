// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "UI.hpp"

#include "../_common/Trim.hpp"

#include <tuple>

using namespace tcob::literals;
using namespace std::chrono_literals;
using namespace tcob::random;

////////////////////////////////////////////////////////////

piano_form::piano_form(window& window, sound_font const& sf)
    : form {{.Name = "piano", .Bounds = window.bounds()}}
    , _font {"trim", "trim"}
{
    font_family::SingleFont(*_font.ptr(), std::as_bytes(std::span {trim_ttf}));
    gen_styles();

    auto& mainPanel {create_container<glass>(dock_style::Fill, "main")};
    auto& mainPanelLayout {mainPanel.create_layout<grid_layout>(size_i {24, 8})};

    // White
    std::array<std::tuple<button*&, i32, std::string>, 7> const whiteKeys {
        {{C, 0, "C"}, {D, 3, "D"}, {E, 6, "E"}, {F, 9, "F"}, {G, 12, "G"}, {A, 15, "A"}, {B, 18, "B"}}};

    for (auto const& [btn, posX, label] : whiteKeys) {
        btn         = &mainPanelLayout.create_widget<button>({posX, 1, 3, 6}, label);
        btn->Class  = "white-keys";
        btn->Label  = label;
        btn->ZOrder = 1;
    }

    // Black keys
    std::array<std::tuple<button*&, i32, std::string>, 5> const blackKeys {
        {{CSharp, 2, "C#"}, {DSharp, 5, "D#"}, {FSharp, 11, "F#"}, {GSharp, 14, "G#"}, {ASharp, 17, "A#"}}};

    for (auto const& [btn, posX, label] : blackKeys) {
        btn         = &mainPanelLayout.create_widget<button>({posX, 1, 2, 3}, label);
        btn->Class  = "black-keys";
        btn->Label  = label;
        btn->ZOrder = 10;
    }

    { // Preset
        Preset      = &mainPanelLayout.create_widget<slider>({2, 0, 5, 1}, "Preset");
        Preset->Min = 0;
        Preset->Max = sf.info().PresetCount - 1;

        auto& lbl {mainPanelLayout.create_widget<label>({0, 0, 2, 1}, "lblPreset")};
        Preset->Value.Changed.connect([sf = &sf, &lbl](auto val) {
            lbl.Label = sf->get_preset_name(val);
        });

        Preset->Value = 0;
        lbl.Label     = sf.get_preset_name(0);
    }
    { // Length
        Length      = &mainPanelLayout.create_widget<slider>({10, 0, 5, 1}, "Length");
        Length->Min = 0;
        Length->Max = 3;

        auto& lbl {mainPanelLayout.create_widget<label>({8, 0, 2, 1}, "lblLength")};
        Length->Value.Changed.connect([sf = &sf, &lbl](isize val) {
            switch (val) {
            case 0: lbl.Label = "full"; break;
            case 1: lbl.Label = "half"; break;
            case 2: lbl.Label = "quarter"; break;
            case 3: lbl.Label = "eighth"; break;
            }
        });

        Length->Value = 0;
        lbl.Label     = "full";
    }
    // Octave
    {
        Octave      = &mainPanelLayout.create_widget<slider>({18, 0, 5, 1}, "Octave");
        Octave->Min = 0;
        Octave->Max = 8;

        auto& lbl {mainPanelLayout.create_widget<label>({16, 0, 2, 1}, "lblOctave")};
        Octave->Value.Changed.connect([&lbl](auto val) {
            lbl.Label = std::to_string(static_cast<i32>(val));
        });
        Octave->Value = 4;
    }
}

void piano_form::gen_styles()
{
    style_collection styles;
    {
        auto style {styles.create<button>("white-keys", {})};
        style->Border.Type    = border_type::Solid;
        style->Border.Size    = 3_px;
        style->Border.Radius  = 5_px;
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 25_pct;
        style->Text.Alignment = {horizontal_alignment::Center, vertical_alignment::Bottom};
        style->Text.AutoSize  = auto_size_mode::OnlyShrink;
        style->Margin         = {2_px};
        style->Padding        = {2_px};

        style->Background        = colors::White;
        style->Border.Background = colors::Black;
        style->Text.Color        = colors::Black;

        auto activeStyle {styles.create<button>("white-keys", {.Active = true, .Hover = true})};
        *activeStyle = *style;

        activeStyle->Background        = colors::LightGray;
        activeStyle->Border.Background = colors::Black;
        activeStyle->Text.Color        = colors::Black;

        auto hoverStyle {styles.create<button>("white-keys", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Background        = colors::LightBlue;
        hoverStyle->Border.Background = colors::Black;
        hoverStyle->Text.Color        = colors::Black;
    }
    {
        auto style {styles.create<button>("black-keys", {})};
        style->Border.Type    = border_type::Solid;
        style->Border.Size    = 3_px;
        style->Border.Radius  = 5_px;
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 50_pct;
        style->Text.Alignment = {horizontal_alignment::Center, vertical_alignment::Middle};
        style->Text.AutoSize  = auto_size_mode::OnlyShrink;
        style->Margin         = {2_px};
        style->Padding        = {2_px};

        style->Background        = colors::Black;
        style->Border.Background = colors::White;
        style->Text.Color        = colors::White;

        auto activeStyle {styles.create<button>("black-keys", {.Active = true, .Hover = true})};
        *activeStyle = *style;

        activeStyle->Background        = colors::DarkGray;
        activeStyle->Border.Background = colors::White;
        activeStyle->Text.Color        = colors::White;

        auto hoverStyle {styles.create<button>("black-keys", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Background        = colors::DarkBlue;
        hoverStyle->Border.Background = colors::White;
        hoverStyle->Text.Color        = colors::White;
    }
    {
        auto style {styles.create<label>("label", {})};
        style->Border.Type    = border_type::Solid;
        style->Border.Size    = 3_px;
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 40_pct;
        style->Text.Alignment = {horizontal_alignment::Center, vertical_alignment::Middle};
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

        style->Bar.HigherBackground  = colors::LightGray;
        style->Bar.LowerBackground   = colors::LightGray;
        style->Bar.Border.Background = colors::Black;
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
        activeStyle->Thumb.Border.Background = colors::Black;

        auto hoverStyle {styles.create<thumb_style>("slider_thumb", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Thumb.Background        = colors::LightBlue;
        hoverStyle->Thumb.Border.Background = colors::Black;
    }

    Styles = styles;
}
