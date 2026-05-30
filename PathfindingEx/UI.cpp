// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include "UI.hpp"

#include "../_common/Trim.hpp"

using namespace tcob::literals;

pathfinding_form::pathfinding_form(rect_i const& bounds)
    : form {{"pathfinding", bounds}}
    , _font {"trim", "trim"}
{
    font_family::SingleFont(*_font.ptr(), std::as_bytes(std::span {trim_ttf}));
    gen_styles();

    auto& mainPanel {create_container<glass>(dock_style::Fill, "main")};
    auto& layout {mainPanel.create_layout<grid_layout>(size_i {40, 40})};

    // map cycle button
    auto& lblMapTitle {layout.create_widget<label>({0, 0, 10, 3}, "lblMapTitle")};
    lblMapTitle.Label = "Map";

    auto& cbMap {layout.create_widget<cycle_button>({0, 3, 10, 5}, "cbMap")};
    cbMap.Items.mutate([](auto& items) {
        items.push_back({"Maze"});
        items.push_back({"Open"});
        items.push_back({"Boxes"});
        items.push_back({"Terrain"});
    });
    cbMap.SelectedItemIndex = 0;
    cbMap.SelectedItemIndex.Changed.connect([this, &cbMap]() {
        _mapMode = static_cast<map_mode>(*cbMap.SelectedItemIndex);
        OnMapChanged();
    });

    // algo cycle button
    auto& lblAlgoTitle {layout.create_widget<label>({12, 0, 10, 3}, "lblAlgoTitle")};
    lblAlgoTitle.Label = "Algorithm";

    auto& cbAlgo {layout.create_widget<cycle_button>({12, 3, 10, 5}, "cbAlgo")};
    cbAlgo.Items.mutate([](auto& items) {
        items.push_back({"A*"});
        items.push_back({"Bidir A*"});
        items.push_back({"Theta*"});
        items.push_back({"LPA*"});
        items.push_back({"D* Lite"});
        items.push_back({"MinTurns"});
        items.push_back({"FlowField"});
    });
    cbAlgo.SelectedItemIndex = 2; // ThetaStar default
    cbAlgo.SelectedItemIndex.Changed.connect([this, &cbAlgo] {
        _algoMode = static_cast<algo_mode>(*cbAlgo.SelectedItemIndex);
        OnAlgoChanged();
    });

    // stats
    _lblTime         = &layout.create_widget<label>({0, 30, 10, 3}, "lblTime");
    _lblNodes        = &layout.create_widget<label>({12, 30, 10, 3}, "lblNodes");
    _lblTime->Label  = "Time: -";
    _lblNodes->Label = "Nodes: -";

    // action buttons
    auto makeBtn {[&](rect_i r, string const& lbl, auto fn) -> button& {
        auto& btn {layout.create_widget<button>(r, string {lbl} + "Btn")};
        btn.Label = lbl;
        btn.Click.connect([fn](auto&&) { fn(); });
        return btn;
    }};

    makeBtn({0, 12, 10, 2}, "Regenerate", [this] { OnRegenerate(); });
    makeBtn({12, 12, 10, 2}, "Smooth Path", [this] { OnSmooth(); });
    makeBtn({0, 15, 10, 2}, "Toggle Wall", [this] { OnToggleWall(); });
    makeBtn({12, 15, 10, 2}, "D* Move", [this] { OnDStarMove(); });
}

auto pathfinding_form::map_mode_name() const -> string
{
    switch (_mapMode) {
    case map_mode::Maze:    return "Maze";
    case map_mode::Open:    return "Open";
    case map_mode::Boxes:   return "Boxes";
    case map_mode::Terrain: return "Terrain";
    }
    return "";
}

auto pathfinding_form::algo_mode_name() const -> string
{
    switch (_algoMode) {
    case algo_mode::AStar:      return "A*";
    case algo_mode::BidirAStar: return "Bidir A*";
    case algo_mode::ThetaStar:  return "Theta*";
    case algo_mode::LPA:        return "LPA*";
    case algo_mode::DStarLite:  return "D* Lite";
    case algo_mode::MinTurns:   return "MinTurns";
    case algo_mode::FlowField:  return "FlowField";
    }
    return "";
}

auto pathfinding_form::is_terrain_map() const -> bool
{
    return _mapMode == map_mode::Terrain;
}

void pathfinding_form::cycle_map()
{
    switch (_mapMode) {
    case map_mode::Maze:    _mapMode = map_mode::Open; break;
    case map_mode::Open:    _mapMode = map_mode::Boxes; break;
    case map_mode::Boxes:   _mapMode = map_mode::Terrain; break;
    case map_mode::Terrain: _mapMode = map_mode::Maze; break;
    }
    update_labels();
}

void pathfinding_form::cycle_algo()
{
    switch (_algoMode) {
    case algo_mode::AStar:      _algoMode = algo_mode::BidirAStar; break;
    case algo_mode::BidirAStar: _algoMode = algo_mode::ThetaStar; break;
    case algo_mode::ThetaStar:  _algoMode = algo_mode::LPA; break;
    case algo_mode::LPA:        _algoMode = algo_mode::DStarLite; break;
    case algo_mode::DStarLite:  _algoMode = algo_mode::MinTurns; break;
    case algo_mode::MinTurns:   _algoMode = algo_mode::FlowField; break;
    case algo_mode::FlowField:  _algoMode = algo_mode::AStar; break;
    }
    update_labels();
}

void pathfinding_form::set_time(f64 ms)
{
    _lblTime->Label = std::format("{:.3f}ms", ms);
}

void pathfinding_form::set_nodes(usize count)
{
    _lblNodes->Label = std::to_string(count);
}

void pathfinding_form::update_labels()
{
    _lblMap->Label  = map_mode_name();
    _lblAlgo->Label = algo_mode_name();
}

void pathfinding_form::gen_styles()
{
    style_collection styles;

    auto makeBase {[&]<typename T>(string const& name) {
        auto style {styles.create<T>(name, {})};
        style->Border.Type       = border_type::Solid;
        style->Border.Size       = 2_px;
        style->Border.Radius     = 4_px;
        style->Text.Font         = _font;
        style->Text.Size         = 40_pct;
        style->Text.Alignment    = {horizontal_alignment::Center, vertical_alignment::Middle};
        style->Text.AutoSize     = auto_size_mode::OnlyShrink;
        style->Margin            = {2_px};
        style->Padding           = {2_px};
        style->Background        = colors::DarkGray;
        style->Border.Background = colors::Gray;
        style->Text.Color        = colors::White;
        return style;
    }};

    {
        auto style {makeBase.template operator()<button>("button")};
        auto hover {styles.create<button>("button", {.Hover = true})};
        *hover                   = *style;
        hover->Border.Background = colors::LightBlue;
    }
    {
        makeBase.template operator()<label>("label");
    }

    {
        using namespace std::chrono_literals;
        auto style {styles.create<cycle_button>("cycle_button", {})};
        style->Border.Size          = 2_px;
        style->Border.Radius        = 4_px;
        style->Margin               = {2_px};
        style->Padding              = {4_px};
        style->Bar.Size             = 50_pct;
        style->Bar.Border.Size      = 1_px;
        style->Bar.Border.Radius    = 4_px;
        style->Bar.Delay            = 300ms;
        style->GapRatio             = 0.5f;
        style->Background           = colors::DarkGray;
        style->Border.Background    = colors::Black;
        style->Bar.LowerBackground  = colors::LightBlue;
        style->Bar.HigherBackground = colors::RoyalBlue;

        auto hover {styles.create<cycle_button>("cycle_button", {.Hover = true})};
        *hover                   = *style;
        hover->GapRatio          = 0.5f;
        hover->Border.Background = colors::LightBlue;
    }

    {
        auto style {styles.create<item_style>("items", {}, {})};
        style->Item.Padding           = {5_px};
        style->Item.Text.Style        = {false, font::weight::Bold};
        style->Item.Text.Font         = _font;
        style->Item.Text.AutoSize     = auto_size_mode::OnlyShrink;
        style->Item.Text.Size         = 15_px;
        style->Item.Text.Alignment    = {horizontal_alignment::Center, vertical_alignment::Middle};
        style->Item.Text.Color        = colors::Black;
        style->Item.Background        = colors::DarkGray;
        style->Item.Border.Size       = 1_px;
        style->Item.Border.Background = colors::Black;
    }

    Styles = styles;
}
