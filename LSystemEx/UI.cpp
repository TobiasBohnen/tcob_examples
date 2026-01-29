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

lsystem_form::lsystem_form(rect_i const& bounds)
    : form {{"lsystem", bounds}}
    , _font {"trim", "trim"}
{
    font_family::SingleFont(*_font.ptr(), trim_ttf);
    gen_styles();

    auto& mainPanel {create_container<glass>(dock_style::Fill, "main")};
    auto& mainPanelLayout {mainPanel.create_layout<grid_layout>(size_i {40, 40})};

    create_rule(mainPanelLayout);
    create_settings(mainPanelLayout);

    auto& btnStart {mainPanelLayout.create_widget<button>({1, 35, 12, 2}, "btnStart")};
    btnStart.Label = "Start";
    btnStart.Click.connect([this] { Start(); });
}

void lsystem_form::create_rule(grid_layout& layout)
{
    auto& txbVariable {layout.create_widget<text_box>({1, 1, 4, 3}, "txbVariable")};
    txbVariable.MaxLength  = 1;
    txbVariable.Selectable = true;

    auto& txbRule {layout.create_widget<text_box>({5, 1, 21, 3}, "txbRule")};
    txbRule.Selectable = true;

    auto& spnProp {layout.create_widget<spinner>({26, 1, 6, 3}, "spnProp")};
    spnProp.Min   = 0;
    spnProp.Max   = 100;
    spnProp.Value = 100;

    auto& btnAdd {layout.create_widget<button>({34, 1, 4, 3}, "btnAdd")};
    btnAdd.Label = "+";

    _grdRules                   = &layout.create_widget<grid_view>({1, 4, 38, 20}, "grdRules");
    _grdRules->Header           = {{"Variable"}, {"Rule"}, {"Probability"}};
    _grdRules->HeaderSelectable = false;
    _grdRules->SelectMode       = grid_select_mode::Row;
    _grdRules->SelectedCellIndex.Changed.connect([&] {
        if (_grdRules->SelectedCellIndex->Y < 1) { return; }
        txbVariable.Text = _grdRules->get_cell({0, _grdRules->SelectedCellIndex->Y}).Text;
        txbRule.Text     = _grdRules->get_cell({1, _grdRules->SelectedCellIndex->Y}).Text;
        spnProp.Value    = *helper::to_number<i32>(_grdRules->get_cell({2, _grdRules->SelectedCellIndex->Y}).Text);
    });

    grid<item> grid {{3, 2}};
    grid.assign({0, 0}, std::vector<item> {{"F"}, {"FF"}, {"100"}});
    grid.assign({0, 1}, std::vector<item> {{"X"}, {"F[+X]F[-X]+X"}, {"100"}});
    _grdRules->Grid = grid;

    _grdRules->KeyDown.connect([&](keyboard_event const& ev) {
        if (ev.Event.KeyCode == key_code::DEL) {
            if (_grdRules->SelectedCellIndex->Y < 1) { return; }
            _grdRules->Grid.mutate([&](auto& grid) {
                grid.erase_row(_grdRules->SelectedCellIndex->Y - 1);
            });
            btnAdd.enable();
        }
    });

    btnAdd.Click.connect([&](widget_event const& ev) {
        if (_grdRules->Grid->height() >= 5 || txbVariable.Text->empty() || txbRule.Text->empty()) { return; }

        _grdRules->Grid.mutate([&](auto& grid) {
            for (i32 i {0}; i < grid.height(); ++i) {
                auto const row {grid.row(i)};
                if (row[0].Text == txbVariable.Text && row[2].Text == std::to_string(spnProp.Value)) { // remove duplicate row
                    grid.erase_row(i);
                    break;
                }
            }

            grid.append_row({{txbVariable.Text}, {txbRule.Text}, {std::to_string(spnProp.Value)}});
        });

        if (_grdRules->Grid->height() == 5) {
            ev.Sender->disable();
        }
    });
}

void lsystem_form::create_settings(grid_layout& layout)
{
    auto& lblAxiom {layout.create_widget<label>({1, 26, 8, 2}, "lblAxiom")};
    lblAxiom.Label  = "Axiom:";
    _txbAxiom       = &layout.create_widget<text_box>({9, 26, 8, 2}, "txbAxiom");
    _txbAxiom->Text = "X";

    auto& lblIterations {layout.create_widget<label>({1, 28, 8, 2}, "lblIterations")};
    lblIterations.Label   = "Iterations:";
    _spnIterations        = &layout.create_widget<spinner>({9, 28, 8, 2}, "spnIterations");
    _spnIterations->Step  = 1;
    _spnIterations->Min   = 1;
    _spnIterations->Value = 5;

    auto& lblAngle {layout.create_widget<label>({21, 28, 8, 2}, "lblAngle")};
    lblAngle.Label   = "Angle:";
    _spnAngle        = &layout.create_widget<spinner>({29, 28, 8, 2}, "spnAngle");
    _spnAngle->Step  = 1;
    _spnAngle->Min   = 0;
    _spnAngle->Max   = 359;
    _spnAngle->Value = 20;

    auto& lblLine {layout.create_widget<label>({1, 30, 8, 2}, "lblLine")};
    lblLine.Label   = "Line:";
    _spnLine        = &layout.create_widget<spinner>({9, 30, 8, 2}, "spnLine");
    _spnLine->Step  = 1;
    _spnLine->Min   = 5;
    _spnLine->Max   = 20;
    _spnLine->Value = 12;

    auto& lblStroke {layout.create_widget<label>({21, 30, 8, 2}, "lblStroke")};
    lblStroke.Label   = "Stroke:";
    _spnStroke        = &layout.create_widget<spinner>({29, 30, 8, 2}, "spnStroke");
    _spnStroke->Step  = 1;
    _spnStroke->Min   = 1;
    _spnStroke->Max   = 10;
    _spnStroke->Value = 5;
}

auto lsystem_form::get_settings() const -> settings
{
    settings retValue;
    l_system system {};
    for (i32 i {0}; i < _grdRules->Grid->height(); ++i) {
        system.add_rule(
            _grdRules->get_cell({0, i + 1}).Text[0],
            {.Replacement = _grdRules->get_cell({1, i + 1}).Text,
             .Probability = *helper::to_number<f32>(_grdRules->get_cell({2, i + 1}).Text) / 100.f});
    }
    retValue.String = system.generate(*_txbAxiom->Text, _spnIterations->Value);

    retValue.TurningAngle = degree_f {static_cast<f32>(_spnAngle->Value)};

    retValue.LineLength  = _spnLine->Value;
    retValue.StrokeWidth = _spnStroke->Value;

    return retValue;
}

void lsystem_form::gen_styles()
{
    style_collection styles;

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

        auto disabledStyle {styles.create<button>("button", {.Disabled = true})};
        *disabledStyle = *style;

        disabledStyle->Background        = colors::LightGray;
        disabledStyle->Border.Background = colors::LightGray;
        disabledStyle->Text.Color        = colors::LightGray;
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
    {
        auto style {styles.create<text_box>("text_box", {})};
        style->Border.Type    = border_type::Solid;
        style->Border.Size    = 3_px;
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 50_pct;
        style->Text.Alignment = {horizontal_alignment::Left, vertical_alignment::Top};
        style->Text.AutoSize  = auto_size_mode::OnlyShrink;
        style->Caret.Color    = colors::Black;
        style->Margin         = {2_px};
        style->Padding        = {2_px};

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
        style->Text.Color        = colors::Black;

        auto hoverStyle {styles.create<text_box>("text_box", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Background        = colors::LightGray;
        hoverStyle->Border.Background = colors::Red;
        hoverStyle->Text.Color        = colors::Black;
    }
    {
        auto style {styles.create<grid_view>("grid_view", {})};
        style->Border.Size       = 3_px;
        style->Border.Radius     = 5_px;
        style->Border.Background = colors::Black;
        style->Margin            = {5_px};
        style->Padding           = {5_px};
        style->MaxVisibleRows    = 7;
        style->RowItemClasses    = {"row_items"};
        style->HeaderItemClass   = "header_items";
        style->AutoSizeColumns   = false;
    }
    {
        auto style {styles.create<item_style>("header_items", {}, {})};
        style->Item.Padding           = {5_px};
        style->Item.Text.Style        = {false, font::weight::Bold};
        style->Item.Text.Font         = _font;
        style->Item.Text.AutoSize     = auto_size_mode::OnlyShrink;
        style->Item.Text.Size         = 15_px;
        style->Item.Text.Alignment    = {horizontal_alignment::Centered, vertical_alignment::Middle};
        style->Item.Text.Color        = colors::Black;
        style->Item.Background        = colors::DarkGray;
        style->Item.Border.Size       = 1_px;
        style->Item.Border.Background = colors::Black;
    }
    {
        auto style {styles.create<item_style>("row_items", {}, {})};
        style->Item.Padding        = {10_px};
        style->Item.Text.Style     = {false, font::weight::Normal};
        style->Item.Text.Font      = _font;
        style->Item.Text.AutoSize  = auto_size_mode::OnlyShrink;
        style->Item.Text.Size      = 20_px;
        style->Item.Text.Alignment = {horizontal_alignment::Left, vertical_alignment::Middle};
        style->Item.Border.Size    = 2_px;

        style->Item.Background        = colors::LightGray;
        style->Item.Border.Background = colors::Black;
        style->Item.Text.Color        = colors::Black;

        auto activeStyle {styles.create<item_style>("row_items", {.Active = true})};
        *activeStyle = *style;

        activeStyle->Item.Background        = colors::DarkGray;
        activeStyle->Item.Border.Background = colors::Black;
        activeStyle->Item.Text.Color        = colors::Black;

        auto hoverStyle {styles.create<item_style>("row_items", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Item.Background        = colors::LightGray;
        hoverStyle->Item.Border.Background = colors::Red;
        hoverStyle->Item.Text.Color        = colors::Black;
    }
    {
        auto style {styles.create<spinner>("spinner", {})};
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
    }
    {
        auto style {styles.create<nav_arrows_style>("nav_arrows", {}, {})};
        style->NavArrow.Size.Height   = {0.75f, length::type::Relative};
        style->NavArrow.Size.Width    = {0.25f, length::type::Relative};
        style->NavArrow.Border.Size   = 3_px;
        style->NavArrow.Border.Radius = 3_px;

        style->NavArrow.Foreground     = colors::Black;
        style->NavArrow.UpBackground   = colors::DarkGray;
        style->NavArrow.DownBackground = colors::DarkGray;

        auto hoverStyle {styles.create<nav_arrows_style>("nav_arrows", {.Hover = true})};
        hoverStyle->NavArrow                   = style->NavArrow;
        hoverStyle->NavArrow.Border.Background = colors::Red;

        auto activeStyle {styles.create<nav_arrows_style>("nav_arrows", {.Active = true})};
        activeStyle->NavArrow            = style->NavArrow;
        activeStyle->NavArrow.Foreground = colors::Red;
    }

    Styles = styles;
}
