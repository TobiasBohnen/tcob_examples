// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "HtmlEx.hpp"
#include <iomanip>

using namespace std::chrono_literals;

static char const* main_html {
#include "./html/main.html"
};
static char const* home_html {
#include "./html/home.html"
};
static char const* about_html {
#include "./html/about.html"
};
static char const* services_html {
#include "./html/services.html"
};
static char const* contact_html {
#include "./html/contact.html"
};
static char const* style_css {
#include "./html/style.css"
};

HtmlEx::HtmlEx(game& game)
    : scene(game)
{
}

HtmlEx::~HtmlEx() = default;

void HtmlEx::on_start()
{
    auto& resMgr {parent().library()};
    auto* resGrp {resMgr.get_group("res")};

    auto const winSize {window().Size()};

    _navHtml = std::make_shared<html::document>(
        html::document::config {.AssetGroup      = resGrp,
                                .Fonts           = resGrp->get<font_family>("Poppins"),
                                .DefaultFontSize = 24,
                                .Window          = &window()});

    _navHtml->AnchorClick.connect([&](auto&& ev) {
        if (ev == "#home") {
            _contentHtml->from_string(home_html, style_css);
        } else if (ev == "#about") {
            _contentHtml->from_string(about_html, style_css);
        } else if (ev == "#services") {
            _contentHtml->from_string(services_html, style_css);
        } else if (ev == "#contact") {
            _contentHtml->from_string(contact_html, style_css);
        }
    });
    _navHtml->Bounds = {point_f::Zero, {winSize.Width * 1.f, winSize.Height / 5.f}};
    _navHtml->from_string(main_html, style_css);

    _contentHtml = std::make_shared<html::document>(
        html::document::config {.AssetGroup      = resGrp,
                                .Fonts           = resGrp->get<font_family>("Poppins"),
                                .DefaultFontSize = 24,
                                .Window          = &window()});
    _contentHtml->Bounds = {{0, winSize.Height / 5.f}, {winSize.Width * 1.f, winSize.Height / 5.f * 4.f}};
    _contentHtml->from_string(home_html, style_css);

    root_node()->create_child()->Entity = _navHtml;
    root_node()->create_child()->Entity = _contentHtml;
}

void HtmlEx::on_draw_to(render_target& target)
{
}

void HtmlEx::on_update(milliseconds deltaTime)
{
}

void HtmlEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().stats()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void HtmlEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    case scan_code::D1:
        break;
    default:
        break;
    }
}
