// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::ui;

auto create_form0(window* wnd, assets::group const& resGrp) -> std::shared_ptr<form>;
auto create_form1(window* wnd) -> std::shared_ptr<form>;

auto create_form_displays(window* wnd) -> std::shared_ptr<form>;
auto create_form_tabcontainer(window* wnd, assets::group const& resGrp) -> std::shared_ptr<form>;
auto create_form_accordion(window* wnd, assets::group const& resGrp) -> std::shared_ptr<form>;
auto create_form_colorpicker(window* wnd) -> std::shared_ptr<form>;