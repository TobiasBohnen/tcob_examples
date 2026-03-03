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

physics_form::physics_form(rect_i const& bounds)
    : form {{"physics", bounds}}
    , _font {"trim", "trim"}
{
    font_family::SingleFont(*_font.ptr(), std::as_bytes(std::span {trim_ttf}));
    gen_styles();

    auto& mainPanel {create_container<panel>(dock_style::Fill, "main")};
    auto& mainPanelLayout {mainPanel.create_layout<grid_layout>(size_i {45, 40})};

    auto addRadioButton {[&](auto&& layout, rect_i const& bounds, rect_i const& labelBounds, string const& labelText, auto&& func) -> radio_button& {
        auto& rb {layout.template create_widget<radio_button>(bounds, labelText + "rb")};
        rb.Checked.Changed.connect(func);
        auto& lbl {layout.template create_widget<label>(labelBounds, labelText + "lbl")};
        lbl.Label = labelText;
        lbl.For   = &rb;
        return rb;
    }};

    auto createPanel {[&](rect_i labelRect, rect_i panelRect, string const& labelText, string const& panelName) -> grid_layout& {
        auto& lbl {mainPanelLayout.create_widget<label>(labelRect, labelText + "lbl")};
        lbl.Label = labelText;
        auto& pnl {mainPanelLayout.create_widget<panel>(panelRect, panelName)};
        auto& layout {pnl.create_layout<grid_layout>(size_i {10, 10})};
        return layout;
    }};

    // left
    auto& leftPanelLayout {createPanel({0, 0, 15, 2}, {0, 2, 15, 10}, "left mouse button", "left")};
    auto& rbBox {addRadioButton(leftPanelLayout, {0, 0, 3, 2}, {3, 0, 6, 2}, "box", [&]() { LeftMode = left_button_mode::Box; })};
    rbBox.Checked = true;
    addRadioButton(leftPanelLayout, {0, 2, 3, 2}, {3, 2, 6, 2}, "circle", [&]() { LeftMode = left_button_mode::Circle; });
    addRadioButton(leftPanelLayout, {0, 4, 3, 2}, {3, 4, 6, 2}, "polygon", [&]() { LeftMode = left_button_mode::Polygon; });
    addRadioButton(leftPanelLayout, {0, 6, 3, 2}, {3, 6, 6, 2}, "capsule", [&]() { LeftMode = left_button_mode::Capsule; });

    // middle
    auto& middlePanelLayout {createPanel({15, 0, 15, 2}, {15, 2, 15, 10}, "middle mouse button", "middle")};
    auto& rbExplosion {addRadioButton(middlePanelLayout, {0, 0, 3, 2}, {3, 0, 6, 2}, "explosion", [&]() { MiddleMode = middle_button_mode::Explosion; })};
    rbExplosion.Checked = true;
    addRadioButton(middlePanelLayout, {0, 2, 3, 2}, {3, 2, 6, 2}, "reverse gravity", [&]() { MiddleMode = middle_button_mode::ReverseGravity; });

    // right
    auto& rightPanelLayout {createPanel({30, 0, 15, 2}, {30, 2, 15, 10}, "right mouse button", "right")};
    auto& rbRemove {addRadioButton(rightPanelLayout, {0, 0, 3, 2}, {3, 0, 6, 2}, "remove", [&]() { RightMode = right_button_mode::Remove; })};
    rbRemove.Checked = true;
    addRadioButton(rightPanelLayout, {0, 2, 3, 2}, {3, 2, 6, 2}, "angular imp.", [&]() { RightMode = right_button_mode::AngularImpulse; });
    addRadioButton(rightPanelLayout, {0, 4, 3, 2}, {3, 4, 6, 2}, "linear imp.", [&]() { RightMode = right_button_mode::LinearImpulse; });

    // debug
    auto& tglDbg {mainPanelLayout.create_widget<toggle>({29, 35, 10, 2}, "tglDbg")};
    tglDbg.Checked.Changed.connect([&](bool val) { DebugMode = val; });
    auto& lblDbg {mainPanelLayout.create_widget<label>({39, 35, 6, 2}, "lblDbg")};
    lblDbg.Label = "debug";
    lblDbg.For   = &tglDbg;

    // obstacles
    auto& btnObstacles {mainPanelLayout.create_widget<button>({0, 17, 12, 2}, "btnObstacles")};
    btnObstacles.Click.connect([&]() { CreateObstacles(); });
    btnObstacles.Label = "create obstacles";

    // clear
    auto& btnClear {mainPanelLayout.create_widget<button>({0, 20, 12, 2}, "btnClear")};
    btnClear.Click.connect([&]() { ClearObjects(); });
    btnClear.Label = "clear objects";
}

void physics_form::gen_styles()
{
    style_collection styles;

    {
        auto style {styles.create<panel>("panel", {})};
        style->Border.Size      = 3_px;
        style->Border.Radius    = 5_px;
        style->Margin           = {5_px};
        style->Padding          = {5_px};
        style->DropShadow.Color = color {0, 0, 0, 128};

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
    }
    {
        auto style {styles.create<button>("button", {})};
        style->Border.Type    = border_type::Solid;
        style->Border.Size    = 3_px;
        style->Border.Radius  = 5_px;
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

        auto hoverStyle {styles.create<button>("button", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Background        = colors::LightGray;
        hoverStyle->Border.Background = colors::Red;
        hoverStyle->Text.Color        = colors::Black;

        auto activeStyle {styles.create<button>("button", {.Active = true})};
        *activeStyle = *style;

        activeStyle->Background        = colors::Black;
        activeStyle->Border.Background = colors::Red;
        activeStyle->Text.Color        = colors::LightGray;
    }
    {
        auto style {styles.create<radio_button>("radio_button", {})};
        style->Border.Size      = 3_px;
        style->Margin           = {5_px};
        style->Padding          = {5_px};
        style->DropShadow.Color = color {0, 0, 0, 128};
        style->Tick.Type        = tick_type::Disc;
        style->Tick.Size        = 100_pct;

        style->Background        = colors::LightGray;
        style->Tick.Foreground   = colors::Black;
        style->Border.Background = colors::Black;

        auto hoverStyle {styles.create<radio_button>("radio_button", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Background        = colors::LightGray;
        hoverStyle->Border.Background = colors::Red;
    }
    {
        auto style {styles.create<toggle>("toggle", {})};
        style->Border.Size      = 3_px;
        style->Border.Radius    = 15_px;
        style->Margin           = {5_px};
        style->Padding          = {5_px};
        style->DropShadow.Color = color {0, 0, 0, 128};
        style->Tick.Type        = tick_type::Disc;
        style->Tick.Size        = 100_pct;

        style->Background        = colors::LightGray;
        style->Tick.Foreground   = colors::Black;
        style->Border.Background = colors::Black;

        auto hoverStyle {styles.create<toggle>("toggle", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Background        = colors::LightGray;
        hoverStyle->Border.Background = colors::Red;
    }
    {
        auto style {styles.create<label>("label", {})};
        style->Border.Type    = border_type::Solid;
        style->Border.Size    = 2_px;
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

    Styles = styles;
}
