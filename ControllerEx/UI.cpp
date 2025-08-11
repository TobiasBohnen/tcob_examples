// Copyright (c) 2025 Tobias Bohnen
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
    : form {{"generator", window.bounds()}}
    , _font {"trim", "trim"}
{
    font_family::SingleFont(*_font.ptr(), trim_ttf);
    gen_styles();

    auto  mainPanel {create_container<glass>(dock_style::Fill, "main")};
    auto& mainPanelLayout {mainPanel->create_layout<grid_layout>(size_i {40, 40})};

    Controller = mainPanelLayout.create_widget<label>({13, 1, 10, 2}, "lbl");

    // pressed button
    _button = mainPanelLayout.create_widget<label>({1, 5, 10, 4}, "lblButton");

    // rumble
    auto btnRumble0 {mainPanelLayout.create_widget<button>({1, 12, 10, 4}, "btnRumble0")};
    btnRumble0->Click += [this] { _input.first_controller().rumble(0, 0xFFFF, 1s); };
    btnRumble0->Label = "High Freq";

    auto btnRumble1 {mainPanelLayout.create_widget<button>({1, 17, 10, 4}, "btnRumble1")};
    btnRumble1->Click += [this] { _input.first_controller().rumble(0xFFFF, 0, 1s); };
    btnRumble1->Label = "Low Freq";

    auto btnRumble2 {mainPanelLayout.create_widget<button>({1, 22, 10, 4}, "btnRumble2")};
    btnRumble2->Click += [this] { _input.first_controller().rumble(0xFFFF, 0xFFFF, 1s); };
    btnRumble2->Label = "Both";

    // axes
    auto createSlider {[&](std::shared_ptr<slider>& ptr, rect_i const& bounds, rect_i const& lblBounds, i16 min, i16 max, controller::axis axis) {
        string const name {_input.first_controller().get_axis_name(axis)};
        ptr      = mainPanelLayout.create_widget<slider>(bounds, name);
        ptr->Min = min;
        ptr->Max = max;
        ptr->disable();

        auto lbl {mainPanelLayout.create_widget<label>(lblBounds, "lbl" + name)};
        lbl->Label = name;
    }};

    createSlider(_laxisx, {15, 7, 8, 4}, {15, 5, 8, 2}, -32768, 32767, controller::axis::LeftX);
    createSlider(_laxisy, {18, 14, 2, 8}, {15, 12, 8, 2}, -32768, 32767, controller::axis::LeftY);

    createSlider(_raxisx, {25, 7, 8, 4}, {25, 5, 8, 2}, -32768, 32767, controller::axis::RightX);
    createSlider(_raxisy, {28, 14, 2, 8}, {25, 12, 8, 2}, -32768, 32767, controller::axis::RightY);

    createSlider(_laxis, {15, 25, 8, 4}, {15, 23, 8, 2}, 0, 32767, controller::axis::LeftTrigger);
    createSlider(_raxis, {25, 25, 8, 4}, {25, 23, 8, 2}, 0, 32767, controller::axis::RightTrigger);
}

void crtl_form::on_controller_axis_motion(input::controller::axis_event const& ev)
{
    switch (ev.Axis) {
    case controller::axis::LeftX:        _laxisx->Value = ev.Value; break;
    case controller::axis::LeftY:        _laxisy->Value = -ev.Value; break;
    case controller::axis::RightX:       _raxisx->Value = ev.Value; break;
    case controller::axis::RightY:       _raxisy->Value = -ev.Value; break;
    case controller::axis::LeftTrigger:  _laxis->Value = ev.Value; break;
    case controller::axis::RightTrigger: _raxis->Value = ev.Value; break;
    case controller::axis::Invalid:      break;
    }
}

void crtl_form::on_controller_button_down(input::controller::button_event const& ev)
{
    _button->Label = _input.first_controller().get_button_name(ev.Button);
}

void crtl_form::on_controller_button_up(input::controller::button_event const& ev)
{
    _button->Label = "";
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
        style->Text.AutoSize  = auto_size_mode::Always;
        style->Margin         = {2_px};
        style->Padding        = {2_px};

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
        style->Text.Color        = colors::Black;
    }
    {
        auto style {styles.create<slider>("slider", {})};
        style->Margin             = {5_px};
        style->Padding            = {2_px};
        style->ThumbClass         = "slider_thumb";
        style->Bar.Size           = 75_pct;
        style->Bar.MotionDuration = 0ms;
        style->Bar.Border.Size    = 3_px;
        style->Bar.Border.Radius  = 5_px;

        style->Bar.HigherBackground  = colors::LightGray;
        style->Bar.LowerBackground   = colors::LightGray;
        style->Bar.Border.Background = colors::Black;
    }
    {
        auto style {styles.create<thumb_style>("slider_thumb", {}, {})};
        style->Thumb.Type          = thumb_type::Rect;
        style->Thumb.LongSide      = 10_pct;
        style->Thumb.ShortSide     = 100_pct;
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
