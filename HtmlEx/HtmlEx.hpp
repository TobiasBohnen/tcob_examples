// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"

class HtmlEx : public scene {
public:
    HtmlEx(game& game);
    ~HtmlEx();

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;

private:
    std::shared_ptr<html::document> _navHtml;
    std::shared_ptr<html::document> _contentHtml;
};
