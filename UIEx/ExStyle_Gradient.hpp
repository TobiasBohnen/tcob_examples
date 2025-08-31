// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <tcob/tcob.hpp>
using namespace tcob;
using namespace tcob::gfx;
using namespace tcob::ui;
using namespace tcob::input;

struct gradient_theme {
    paint Background {colors::Transparent};
    paint Container {colors::Transparent};
    color DropShadow {colors::Transparent};
    paint BarLower {colors::Transparent};
    paint BarHigher {colors::Transparent};
    paint Border {colors::Transparent};
    color Caret {colors::Transparent};
    paint Item {colors::Transparent};
    paint NavArrowInc {colors::Transparent};
    paint NavArrowDec {colors::Transparent};
    paint NavArrow {colors::Transparent};
    color Text {colors::Transparent};
    color TextShadow {colors::Transparent};
    color TextDecoration {colors::Transparent};
    paint Thumb {colors::Transparent};
    paint Tick {colors::Transparent};

    void apply(std::shared_ptr<button::style> const& style) const;
    void apply(std::shared_ptr<checkbox::style> const& style) const;
    void apply(std::shared_ptr<cycle_button::style> const& style) const;
    void apply(std::shared_ptr<drop_down_list::style> const& style) const;
    void apply(std::shared_ptr<grid_view::style> const& style) const;
    void apply(std::shared_ptr<image_box::style> const& style) const;
    void apply(std::shared_ptr<label::style> const& style) const;
    void apply(std::shared_ptr<list_box::style> const& style) const;
    void apply(std::shared_ptr<progress_bar::style> const& style) const;
    void apply(std::shared_ptr<radio_button::style> const& style) const;
    void apply(std::shared_ptr<slider::style> const& style) const;
    void apply(std::shared_ptr<spinner::style> const& style) const;
    void apply(std::shared_ptr<text_box::style> const& style) const;
    void apply(std::shared_ptr<toggle::style> const& style) const;

    void apply(std::shared_ptr<panel::style> const& style) const;
    void apply(std::shared_ptr<tab_container::style> const& style) const;
    void apply(std::shared_ptr<accordion::style> const& style) const;

    void apply(std::shared_ptr<charts::chart::style> const& style) const;

    void apply(std::shared_ptr<thumb_style> const& style) const;
    void apply(std::shared_ptr<item_style> const& style) const;
    void apply(std::shared_ptr<nav_arrows_style> const& style) const;
};

auto create_gradient_styles(assets::group const& resGrp) -> style_collection;
