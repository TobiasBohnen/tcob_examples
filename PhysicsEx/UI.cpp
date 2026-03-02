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
    auto& mainPanelLayout {mainPanel.create_layout<grid_layout>(size_i {40, 40})};

    // spawn
    auto addRadioButton = [&](rect_i bounds, rect_i labelBounds, string const& name, string const& labelText, auto&& func) -> radio_button& {
        auto& rb {mainPanelLayout.create_widget<radio_button>(bounds, name)};
        rb.Checked.Changed.connect(func);
        auto& lbl {mainPanelLayout.create_widget<label>(labelBounds, std::string {name} + "lbl")};
        lbl.Label = labelText;
        lbl.For   = &rb;
        return rb;
    };
    auto& rbSpawnBox {addRadioButton({0, 0, 4, 2}, {4, 0, 6, 2}, "rbSpawnBox", "box", [&]() { SpawnObject = spawn_object::Box; })};
    rbSpawnBox.Checked = true;
    auto& rbSpawnCircle {addRadioButton({0, 2, 4, 2}, {4, 2, 6, 2}, "rbSpawnCircle", "circle", [&]() { SpawnObject = spawn_object::Circle; })};
    auto& rbSpawnPolygon {addRadioButton({0, 4, 4, 2}, {4, 4, 6, 2}, "rbSpawnPolygon", "polygon", [&]() { SpawnObject = spawn_object::Polygon; })};
    auto& rbSpawnCapsule {addRadioButton({0, 6, 4, 2}, {4, 6, 6, 2}, "rbSpawnCapsule", "capsule", [&]() { SpawnObject = spawn_object::Capsule; })};
    auto& rbSpawnExplosion {addRadioButton({0, 8, 4, 2}, {4, 8, 6, 2}, "rbSpawnExplosion", "explosion", [&]() { SpawnObject = spawn_object::Explosion; })};

    // debug
    auto& tglDbg {mainPanelLayout.create_widget<toggle>({24, 35, 10, 2}, "tglDbg")};
    tglDbg.Checked.Changed.connect([&](bool val) { DebugMode = val; });
    auto& lblDbg {mainPanelLayout.create_widget<label>({34, 35, 6, 2}, "lblDbg")};
    lblDbg.Label = "debug";
    lblDbg.For   = &tglDbg;

    // gravity
    auto& btnGravity {mainPanelLayout.create_widget<button>({0, 14, 12, 2}, "btnGravity")};
    btnGravity.Click.connect([&]() { Gravity = *Gravity * -1; });
    btnGravity.Label = "reverse gravity";

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
