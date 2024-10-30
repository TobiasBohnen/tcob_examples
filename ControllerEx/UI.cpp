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

crtl_form::crtl_form(window* window)
    : form {"generator", window}
    , _font {"trim", "trim"}
{
    font_family::SingleFont(*_font.get_ptr(), trim_ttf);
    gen_styles();

    auto mainPanel {create_container<glass>(dock_style::Fill, "main")};
    auto mainPanelLayout {mainPanel->create_layout<grid_layout>(size_i {40, 40})};

    Controller = mainPanelLayout->create_widget<label>({13, 1, 10, 2}, "lbl");

    // pressed button
    _button = mainPanelLayout->create_widget<label>({1, 5, 10, 4}, "lblButton");

    // rumble
    auto btnRumble0 {mainPanelLayout->create_widget<button>({1, 12, 10, 4}, "btnRumble0")};
    btnRumble0->Click.connect([&]() { _input.get_controller(0)->rumble(0, 0xFFFF, 1s); });
    btnRumble0->Label = "High Freq";

    auto btnRumble1 {mainPanelLayout->create_widget<button>({1, 17, 10, 4}, "btnRumble1")};
    btnRumble1->Click.connect([&]() { _input.get_controller(0)->rumble(0xFFFF, 0, 1s); });
    btnRumble1->Label = "Low Freq";

    auto btnRumble2 {mainPanelLayout->create_widget<button>({1, 22, 10, 4}, "btnRumble2")};
    btnRumble2->Click.connect([&]() { _input.get_controller(0)->rumble(0xFFFF, 0xFFFF, 1s); });
    btnRumble2->Label = "Both";

    // axes
    auto createSlider {[&](std::shared_ptr<slider>& ptr, rect_i const& bounds, i16 min, controller::axis axis) {
        string const name {_input.get_controller(0)->get_axis_name(axis)};
        ptr      = mainPanelLayout->create_widget<slider>(bounds, name);
        ptr->Min = min;
        ptr->Max = std::numeric_limits<i16>::max();
        ptr->disable();

        auto lbl {mainPanelLayout->create_widget<label>({bounds.Position - point_i {0, 2}, bounds.Size - size_i {0, 2}}, "lbl" + name)};
        lbl->Label = name;
    }};

    createSlider(_laxisx, {15, 7, 8, 4}, -32768, controller::axis::LeftX);
    createSlider(_laxisy, {15, 14, 8, 4}, -32768, controller::axis::LeftY);
    createSlider(_raxisx, {25, 7, 8, 4}, -32768, controller::axis::RightX);
    createSlider(_raxisy, {25, 14, 8, 4}, -32768, controller::axis::RightY);
    createSlider(_laxis, {15, 21, 8, 4}, 0, controller::axis::TriggerLeft);
    createSlider(_raxis, {25, 21, 8, 4}, 0, controller::axis::TriggerRight);
}

void crtl_form::on_controller_axis_motion(input::controller::axis_event const& ev)
{
    slider* text {nullptr};

    switch (ev.Axis) {
    case controller::axis::LeftX: text = _laxisx.get(); break;
    case controller::axis::LeftY: text = _laxisy.get(); break;
    case controller::axis::RightX: text = _raxisx.get(); break;
    case controller::axis::RightY: text = _raxisy.get(); break;
    case controller::axis::TriggerLeft: text = _laxis.get(); break;
    case controller::axis::TriggerRight: text = _raxis.get(); break;
    case controller::axis::Invalid: break;
    }

    text->Value = ev.Value;
}

void crtl_form::on_controller_button_down(input::controller::button_event const& ev)
{
    _button->Label = _input.get_controller(0)->get_button_name(ev.Button);
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
        style->Text.AutoSize  = element::text::auto_size_mode::Always;
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
        style->Bar.Type          = element::bar::type::Continuous;
        style->Bar.Size          = 75_pct;
        style->Bar.Delay         = 0ms;
        style->Bar.Border.Size   = 3_px;
        style->Bar.Border.Radius = 5_px;

        style->Bar.HigherBackground  = colors::LightGray;
        style->Bar.LowerBackground   = colors::LightGray;
        style->Bar.Border.Background = colors::Black;
    }
    {
        auto style {styles.create<thumb_style>("slider_thumb", {}, {})};
        style->Thumb.Type          = element::thumb::type::Rect;
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
