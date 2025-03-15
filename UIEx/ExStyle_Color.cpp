#include "ExStyle_Color.hpp"

using namespace std::chrono_literals;

auto create_color_styles(assets::group const& resGrp) -> style_collection
{
    color_theme normal;
    normal.Background     = colors::LightBlue;
    normal.Border         = colors::Black;
    normal.DropShadow     = color {0, 0, 0, 128};
    normal.Text           = colors::Black;
    normal.BarLower       = colors::LightGreen;
    normal.BarHigher      = colors::DarkGreen;
    normal.Caret          = colors::Black;
    normal.Item           = colors::LightGray;
    normal.NavArrowInc    = colors::White;
    normal.NavArrowDec    = colors::White;
    normal.NavArrow       = colors::Blue;
    normal.TextShadow     = colors::White;
    normal.TextDecoration = colors::Red;
    normal.Thumb          = colors::LightGreen;
    normal.Tick           = colors::Black;
    normal.Container      = colors::LightGray;

    color_theme hover {normal};
    hover.Background     = colors::LightGray;
    hover.Border         = colors::Gray;
    hover.Item           = colors::LightGreen;
    hover.Thumb          = colors::DarkKhaki;
    hover.NavArrowInc    = colors::Red;
    hover.NavArrowDec    = colors::Green;
    hover.Tick           = colors::White;
    hover.TextDecoration = colors::Blue;

    color_theme hoverCheck {hover};
    hoverCheck.Tick = colors::Green;

    color_theme focus {normal};
    focus.Border = colors::IndianRed;

    color_theme check {normal};
    check.Tick = colors::Green;

    color_theme focusHover {focus};
    focusHover.Background = colors::DarkKhaki;

    color_theme focusCheck {focus};
    focusCheck.Background = colors::Orange;
    focusCheck.Tick       = colors::Green;

    color_theme focusHoverCheck {focusHover};
    focusHoverCheck.Tick = colors::Green;

    color_theme active {normal};
    active.Background  = colors::LightCyan;
    active.Border      = colors::DarkKhaki;
    active.Text        = colors::White;
    active.Tick        = colors::White;
    active.Item        = colors::FireBrick;
    active.Thumb       = colors::Black;
    active.NavArrowInc = colors::Green;
    active.NavArrowDec = colors::Red;

    color_theme activeCheck {active};
    activeCheck.Tick = colors::Green;

    color_theme disabled {normal};
    disabled.Background = colors::Gray;

    using namespace tcob::literals;

    linear_gradient           grad0;
    std::array<color_stop, 2> colors0 {{{0.f, colors::Orange}, {1.f, colors::Teal}}};
    grad0.Colors = color_gradient {colors0};
    grad0.Angle  = degree_f {90};

    radial_gradient           grad1;
    std::array<color_stop, 2> colors1 {{{0.f, colors::Orange}, {1.f, colors::Teal}}};
    grad1.Colors = color_gradient {colors1};

    style_collection retValue;
    {
        auto style {retValue.create<button>("button", {})};
        style->Border.Type         = line_type::Double;
        style->Border.Size         = 3_px;
        style->Border.Radius       = 5_px;
        style->Text.Style          = {false, font::weight::Normal};
        style->Text.Font           = resGrp.get<font_family>("Poppins");
        style->Text.Size           = 50_pct;
        style->Text.Shadow.OffsetX = 0_px;
        style->Text.Shadow.OffsetY = 1_px;
        style->Text.Alignment      = {horizontal_alignment::Centered, vertical_alignment::Middle};

        // style->Text.Decoration.Line.LineThrough = true;
        // style->Text.Decoration.Style            = line_type::Solid;
        // style->Text.Decoration.Size             = {3_px};

        style->Margin  = {5_px};
        style->Padding = {2_px};

        auto disabledStyle {retValue.create<button>("button", {.Disabled = true})};

        auto hoverStyle {retValue.create<button>("button", {.Hover = true})};
        *hoverStyle = *style;
        //   hoverStyle->Text.Transform       = text_transform::Capitalize;
        //   hoverStyle->Text.Decoration.Size = {10_px};

        auto focusStyle {retValue.create<button>("button", {.Focus = true})};
        *focusStyle = *style;

        auto focusHoverStyle {retValue.create<button>("button", {.Focus = true, .Hover = true})};
        *focusHoverStyle = *focusStyle;

        auto activeStyle {retValue.create<button>("button", {.Focus = true, .Active = true})};
        *activeStyle        = *style;
        activeStyle->Margin = {5_px, 5_px, 10_px, 0_px};

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
        focus.apply(focusStyle);
        focusHover.apply(focusHoverStyle);
        active.apply(activeStyle);
    }
    {
        auto style {retValue.create<label>("label", {})};
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = resGrp.get<font_family>("Poppins");
        style->Text.Size      = 50_pct;
        style->Text.Alignment = {horizontal_alignment::Left, vertical_alignment::Middle};

        normal.apply(style);
    }
    {
        auto style {retValue.create<checkbox>("checkbox", {})};
        style->Border.Type       = line_type::Dashed;
        style->Border.Dash       = std::vector {20_pct, 10_pct, 10_pct, 5_pct};
        style->Border.DashOffset = 100;
        style->Border.Size       = 3_px;
        style->Border.Radius     = 5_px;
        style->Margin            = {5_px};
        style->Padding           = {2_px};
        style->Tick.Type         = tick_type::Triangle;
        style->Tick.Size         = 90_pct;

        auto disabledStyle {retValue.create<checkbox>("checkbox", {.Disabled = true})};

        auto hoverStyle {retValue.create<checkbox>("checkbox", {.Hover = true})};
        *hoverStyle           = *style;
        hoverStyle->Tick.Type = tick_type::Rect;

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
        hoverStyle->Border.DashOffset = 0;
        //  hoverStyle->Border.Dash = std::vector {15_pct, 15_pct};
    }
    {
        auto style {retValue.create<radio_button>("radio_button", {})};
        style->Border.Type   = line_type::Dotted;
        style->Border.Size   = 3_px;
        style->Border.Radius = 5_px;
        style->Margin        = {5_px};
        style->Padding       = {5_px};
        style->Tick.Type     = tick_type::Triangle;
        style->Tick.Size     = 80_pct;

        auto disabledStyle {retValue.create<radio_button>("radio_button", {.Disabled = true})};

        auto hoverStyle {retValue.create<radio_button>("radio_button", {.Hover = true})};
        *hoverStyle           = *style;
        hoverStyle->Tick.Type = tick_type::Circle;

        auto focusStyle {retValue.create<radio_button>("radio_button", {.Focus = true})};
        *focusStyle = *style;

        auto focusHoverStyle {retValue.create<radio_button>("radio_button", {.Focus = true, .Hover = true})};
        *focusHoverStyle           = *focusStyle;
        focusHoverStyle->Tick.Type = tick_type::Circle;

        auto activeStyle {retValue.create<radio_button>("radio_button", {.Focus = true, .Active = true})};
        *activeStyle = *style;

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
        focus.apply(focusStyle);
        focusHover.apply(focusHoverStyle);
        active.apply(activeStyle);
    }
    {
        auto style {retValue.create<toggle>("toggle", {})};
        style->Border.Size   = 3_px;
        style->Border.Radius = 25_px;
        style->Margin        = {5_px};
        style->Padding       = {2_px};
        style->Delay         = 500ms;
        style->Tick.Type     = tick_type::Square;
        style->Tick.Size     = 100_pct;

        auto checkedStyle {retValue.create<toggle>("toggle", {.Checked = true})};
        *checkedStyle           = *style;
        checkedStyle->Tick.Type = tick_type::Disc;
        checkedStyle->Tick.Size = 100_pct;

        auto disabledStyle {retValue.create<toggle>("toggle", {.Disabled = true})};

        auto hoverStyle {retValue.create<toggle>("toggle", {.Hover = true})};
        *hoverStyle = *style;

        auto hoverCheckedStyle {retValue.create<toggle>("toggle", {.Hover = true, .Checked = true})};
        *hoverCheckedStyle      = *hoverStyle;
        hoverCheckedStyle->Tick = checkedStyle->Tick;

        auto focusStyle {retValue.create<toggle>("toggle", {.Focus = true})};
        *focusStyle = *style;

        auto focusCheckedStyle {retValue.create<toggle>("toggle", {.Focus = true, .Checked = true})};
        *focusCheckedStyle      = *focusStyle;
        focusCheckedStyle->Tick = checkedStyle->Tick;

        auto focusHoverStyle {retValue.create<toggle>("toggle", {.Focus = true, .Hover = true})};
        *focusHoverStyle = *focusStyle;

        auto focusHoverCheckedStyle {retValue.create<toggle>("toggle", {.Focus = true, .Hover = true, .Checked = true})};
        *focusHoverCheckedStyle      = *focusHoverStyle;
        focusHoverCheckedStyle->Tick = checkedStyle->Tick;

        auto activeStyle {retValue.create<toggle>("toggle", {.Focus = true, .Active = true})};
        *activeStyle = *style;

        auto activeCheckedStyle {retValue.create<toggle>("toggle", {.Focus = true, .Active = true, .Checked = true})};
        *activeCheckedStyle      = *activeStyle;
        activeCheckedStyle->Tick = checkedStyle->Tick;

        normal.apply(style);
        check.apply(checkedStyle);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
        hoverCheck.apply(hoverCheckedStyle);
        focus.apply(focusStyle);
        focusCheck.apply(focusCheckedStyle);
        focusHover.apply(focusHoverStyle);
        focusHoverCheck.apply(focusHoverCheckedStyle);
        active.apply(activeStyle);
        activeCheck.apply(activeCheckedStyle);
    }
    {
        auto style {retValue.create<slider>("slider", {})};
        style->Margin            = {5_px};
        style->Padding           = {2_px, 15_px};
        style->ThumbClass        = "slider_thumb";
        style->Bar.Type          = bar_type::Continuous;
        style->Bar.Size          = 50_pct;
        style->Bar.Delay         = 250ms;
        style->Bar.Border.Size   = 3_px;
        style->Bar.Border.Radius = 5_px;

        auto disabledStyle {retValue.create<slider>("slider", {.Disabled = true})};

        normal.apply(style);
        disabled.apply(disabledStyle);
    }
    {
        auto style {retValue.create<progress_bar>("progress_bar", {})};

        style->Border.Size   = 3_px;
        style->Border.Radius = 5_px;
        style->Margin        = {5_px};
        style->Padding       = {5_px, 5_px};
        style->Bar.Type      = bar_type::Continuous;
        style->Bar.Border    = style->Border;
        style->Bar.Delay     = 250ms;

        auto disabledStyle {retValue.create<progress_bar>("progress_bar", {.Disabled = true})};

        normal.apply(style);
        disabled.apply(disabledStyle);
    }
    {
        auto style {retValue.create<spinner>("spinner", {})};
        style->Border.Size         = 3_px;
        style->Border.Radius       = 5_px;
        style->Text.Style          = {false, font::weight::Normal};
        style->Text.Font           = resGrp.get<font_family>("Poppins");
        style->Text.Size           = 50_pct;
        style->Text.Shadow.OffsetX = 0_px;
        style->Text.Shadow.OffsetY = 1_px;
        style->Text.Alignment      = {horizontal_alignment::Left, vertical_alignment::Middle};
        style->Margin              = {5_px};
        style->Padding             = {5_px};
        style->NavArrowClass       = "nav_arrows";

        auto disabledStyle {retValue.create<spinner>("spinner", {.Disabled = true})};

        auto hoverStyle {retValue.create<spinner>("spinner", {.Hover = true})};
        *hoverStyle = *style;

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
    }
    {
        auto style {retValue.create<cycle_button>("cycle_button", {})};
        style->Border.Size   = 3_px;
        style->Border.Radius = 5_px;
        style->Margin        = {5_px};
        style->Padding       = {5_px};

        auto disabledStyle {retValue.create<cycle_button>("cycle_button", {.Disabled = true})};

        auto hoverStyle {retValue.create<cycle_button>("cycle_button", {.Hover = true})};
        *hoverStyle = *style;

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
    }
    {
        auto style {retValue.create<image_box>("image_box", {})};
        style->Border.Size      = 3_px;
        style->Border.Radius    = 5_px;
        style->Margin           = {5_px};
        style->Padding          = {5_px};
        style->DropShadow.Color = color {0, 0, 0, 128};
        style->Alignment        = alignments {horizontal_alignment::Right, vertical_alignment::Bottom};

        auto hoverStyle {retValue.create<image_box>("image_box", {.Hover = true})};
        *hoverStyle = *style;

        auto disabledStyle {retValue.create<image_box>("image_box", {.Disabled = true})};

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
    }
    {
        auto style {retValue.create<panel>("panel", {})};
        style->Border.Size                = 3_px;
        style->Border.Radius              = 5_px;
        style->Margin                     = {5_px};
        style->Padding                    = {5_px};
        style->DropShadow.Color           = color {0, 0, 0, 128};
        style->VScrollBar.ThumbClass      = "scrollbar_thumb";
        style->VScrollBar.Bar.Type        = bar_type::Continuous;
        style->VScrollBar.Bar.Size        = 30_px;
        style->VScrollBar.Bar.Border.Size = 3_px;
        style->VScrollBar.Bar.Delay       = 250ms;
        style->HScrollBar                 = style->VScrollBar;

        auto hoverStyle {retValue.create<panel>("panel", {.Hover = true})};
        *hoverStyle = *style;

        auto disabledStyle {retValue.create<panel>("panel", {.Disabled = true})};

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
    }
    {
        auto style {retValue.create<tooltip>("tooltip", {})};
        style->Border.Size   = 3_px;
        style->Border.Radius = 5_px;

        normal.apply(style);
    }
    {
        auto style {retValue.create<list_box>("list_box", {})};
        style->Border.Size                = 3_px;
        style->Border.Radius              = 5_px;
        style->Margin                     = {5_px};
        style->Padding                    = {5_px};
        style->DropShadow.Color           = color {0, 0, 0, 128};
        style->ItemHeight                 = 20_pct;
        style->ItemClass                  = "list_items";
        style->VScrollBar.ThumbClass      = "scrollbar_thumb";
        style->VScrollBar.Bar.Type        = bar_type::Continuous;
        style->VScrollBar.Bar.Size        = 20_pct;
        style->VScrollBar.Bar.Border.Size = 3_px;
        style->VScrollBar.Bar.Delay       = 250ms;

        auto hoverStyle {retValue.create<list_box>("list_box", {.Hover = true})};
        *hoverStyle = *style;

        auto disabledStyle {retValue.create<list_box>("list_box", {.Disabled = true})};

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
    }
    {
        auto style {retValue.create<drop_down_list>("drop_down_list", {})};
        style->Border.Size                = 3_px;
        style->Border.Radius              = 5_px;
        style->Margin                     = {5_px};
        style->Padding                    = {5_px};
        style->DropShadow.Color           = color {0, 0, 0, 128};
        style->Text.Style                 = {false, font::weight::Normal};
        style->Text.Font                  = resGrp.get<font_family>("Poppins");
        style->Text.Size                  = 50_pct;
        style->Text.Shadow.OffsetX        = 0_px;
        style->Text.Shadow.OffsetY        = 1_px;
        style->Text.Alignment             = {horizontal_alignment::Centered, vertical_alignment::Middle};
        style->ItemHeight                 = 150_pct;
        style->ItemClass                  = "list_items";
        style->VScrollBar.ThumbClass      = "scrollbar_thumb";
        style->VScrollBar.Bar.Type        = bar_type::Continuous;
        style->VScrollBar.Bar.Size        = 20_pct;
        style->VScrollBar.Bar.Border.Size = 3_px;
        style->VScrollBar.Bar.Delay       = 250ms;

        auto hoverStyle {retValue.create<drop_down_list>("drop_down_list", {.Hover = true})};
        *hoverStyle = *style;

        auto disabledStyle {retValue.create<drop_down_list>("drop_down_list", {.Disabled = true})};

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
    }
    {
        auto style {retValue.create<grid_view>("grid_view", {})};
        style->Border.Size                = 3_px;
        style->Border.Radius              = 5_px;
        style->Margin                     = {5_px};
        style->Padding                    = {5_px};
        style->DropShadow.Color           = color {0, 0, 0, 128};
        style->RowHeight                  = 20_pct;
        style->HeaderItemClass            = "header_items";
        style->RowItemClass               = "row_items";
        style->VScrollBar.ThumbClass      = "scrollbar_thumb";
        style->VScrollBar.Bar.Type        = bar_type::Continuous;
        style->VScrollBar.Bar.Size        = 5_pct;
        style->VScrollBar.Bar.Border.Size = 3_px;
        style->VScrollBar.Bar.Delay       = 250ms;

        auto hoverStyle {retValue.create<grid_view>("grid_view", {.Hover = true})};
        *hoverStyle = *style;

        auto disabledStyle {retValue.create<grid_view>("grid_view", {.Disabled = true})};

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
    }
    {
        auto style {retValue.create<tab_container>("tab_container", {})};
        style->Border.Size      = 3_px;
        style->Border.Radius    = 5_px;
        style->Margin           = {5_px};
        style->Padding          = {5_px};
        style->DropShadow.Color = color {0, 0, 0, 128};
        style->TabBarSize       = 20_pct;
        style->TabItemClass     = "tab_items";
        style->TabBarPosition   = position::Top;

        auto hoverStyle {retValue.create<tab_container>("tab_container", {.Hover = true})};
        *hoverStyle = *style;

        auto disabledStyle {retValue.create<tab_container>("tab_container", {.Disabled = true})};

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);

        auto hoverStyleX {retValue.create<tab_container>("tab_container", {.Hover = true}, {{"hover", "SPanel0"}})};
        *hoverStyleX            = *hoverStyle;
        hoverStyleX->Background = colors::Orange;
        auto hoverStyleY {retValue.create<tab_container>("tab_container", {.Hover = true}, {{"hover", "SPanel1"}})};
        *hoverStyleY            = *hoverStyle;
        hoverStyleY->Background = colors::Red;
        auto hoverStyleZ {retValue.create<tab_container>("tab_container", {.Hover = true}, {{"hover", "SPanel2"}})};
        *hoverStyleZ            = *hoverStyle;
        hoverStyleZ->Background = colors::LavenderBlush;
    }
    {
        auto style {retValue.create<accordion>("accordion", {})};
        style->Border.Size      = 3_px;
        style->Border.Radius    = 5_px;
        style->Margin           = {5_px};
        style->Padding          = {5_px};
        style->DropShadow.Color = color {0, 0, 0, 128};
        style->SectionBarHeight = 10_pct;
        style->SectionItemClass = "section_items";

        auto hoverStyle {retValue.create<accordion>("accordion", {.Hover = true})};
        *hoverStyle = *style;

        auto disabledStyle {retValue.create<accordion>("accordion", {.Disabled = true})};

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);

        auto hoverStyleX {retValue.create<accordion>("accordion", {.Hover = true}, {{"active", "SPanel0"}})};
        *hoverStyleX            = *hoverStyle;
        hoverStyleX->Background = colors::Orange;
        auto hoverStyleY {retValue.create<accordion>("accordion", {.Hover = true}, {{"active", "SPanel1"}})};
        *hoverStyleY            = *hoverStyle;
        hoverStyleY->Background = colors::Red;
        auto hoverStyleZ {retValue.create<accordion>("accordion", {.Hover = true}, {{"active", "SPanel2"}})};
        *hoverStyleZ            = *hoverStyle;
        hoverStyleZ->Background = colors::LavenderBlush;
    }
    {
        auto style {retValue.create<text_box>("text_box", {})};
        style->Border.Type         = line_type::Solid;
        style->Border.Size         = 3_px;
        style->Border.Radius       = 5_px;
        style->Text.Style          = {false, font::weight::Normal};
        style->Text.Font           = resGrp.get<font_family>("Poppins");
        style->Text.Size           = 50_pct;
        style->Text.Shadow.OffsetX = 0_px;
        style->Text.Shadow.OffsetY = 1_px;
        style->Text.Alignment      = {horizontal_alignment::Left, vertical_alignment::Middle};
        style->Margin              = {5_px};
        style->Padding             = {2_px};
        style->Caret.BlinkRate     = 500ms;

        auto disabledStyle {retValue.create<text_box>("text_box", {.Disabled = true})};

        auto hoverStyle {retValue.create<text_box>("text_box", {.Hover = true})};
        *hoverStyle = *style;

        normal.apply(style);
        disabled.apply(disabledStyle);
        hover.apply(hoverStyle);
    }
    // Display
    {
        auto style {retValue.create<terminal>("terminal", {})};
        style->FlashDuration   = 100ms;
        style->Text.Font       = resGrp.get<font_family>("DejaVuSansMono");
        style->Text.Style      = {.IsItalic = false, .Weight = font::weight::Bold};
        style->Text.Size       = 24_px;
        style->Caret.BlinkRate = 500ms;
        style->Caret.Color     = colors::Red;
    }
    {
        auto style {retValue.create<dot_matrix_display>("dot_matrix_display", {})};
        rng  r;
        /*
        for (isize i {0}; i < 256; ++i) {
            style->Colors[i] = color::FromHSLA({.Hue = degree_f {360.f * (i / 255.f)}, .Saturation = 0.8f, .X = 0.5f});
        }
            */
        style->Colors[0] = colors::Cyan;
        style->Colors[1] = colors::Violet;
        style->Colors[2] = colors::White;
        style->Colors[3] = colors::Black;
        style->Colors[4] = colors::Chartreuse;
        style->Colors[5] = colors::OrangeRed;
        style->Colors[6] = colors::Yellow;

        style->Type       = dot_matrix_display::dot_type::Disc;
        style->Background = colors::Gray;

        auto hoverStyle {retValue.create<dot_matrix_display>("dot_matrix_display", {.Hover = true})};
        for (isize i {0}; i < style->Colors.size(); ++i) {
            hoverStyle->Colors[i] = style->Colors[style->Colors.size() - i - 1];
        }
        hoverStyle->Type = dot_matrix_display::dot_type::Disc;
    }
    {
        auto style {retValue.create<seven_segment_display>("seven_segment_display", {})};
        style->Size          = 5_pct;
        style->Background    = colors::DarkGreen;
        style->ActiveColor   = colors::Red;
        style->InactiveColor = colors::Black;

        auto hoverStyle {retValue.create<seven_segment_display>("seven_segment_display", {.Hover = true})};
        hoverStyle->Size          = 8_pct;
        hoverStyle->ActiveColor   = colors::Blue;
        hoverStyle->InactiveColor = colors::White;
    }

    // THUMBS
    {
        auto style {retValue.create<thumb_style>("slider_thumb", {}, {})};
        style->Thumb.Type          = thumb_type::Rect;
        style->Thumb.LongSide      = 25_pct;
        style->Thumb.ShortSide     = 80_pct;
        style->Thumb.Border.Size   = 3_px;
        style->Thumb.Border.Radius = 5_px;

        auto hoverStyle {retValue.create<thumb_style>("slider_thumb", {.Hover = true})};
        hoverStyle->Thumb = style->Thumb;

        auto activeStyle {retValue.create<thumb_style>("slider_thumb", {.Active = true})};
        activeStyle->Thumb = style->Thumb;

        normal.apply(style);
        hover.apply(hoverStyle);
        active.apply(activeStyle);
    }
    {
        auto style {retValue.create<thumb_style>("scrollbar_thumb", {}, {})};
        style->Thumb.Type        = thumb_type::Rect;
        style->Thumb.LongSide    = 25_pct;
        style->Thumb.ShortSide   = 80_pct;
        style->Thumb.Border.Size = 3_px;

        auto hoverStyle {retValue.create<thumb_style>("scrollbar_thumb", {.Hover = true})};
        hoverStyle->Thumb = style->Thumb;

        auto activeStyle {retValue.create<thumb_style>("scrollbar_thumb", {.Active = true})};
        activeStyle->Thumb = style->Thumb;

        normal.apply(style);
        hover.apply(hoverStyle);
        active.apply(activeStyle);
    }
    // NAV ARROWS
    {
        auto style {retValue.create<nav_arrows_style>("nav_arrows", {}, {})};
        style->NavArrow.Size.Height   = {0.75f, length::type::Relative};
        style->NavArrow.Size.Width    = {0.25f, length::type::Relative};
        style->NavArrow.Border.Size   = 3_px;
        style->NavArrow.Border.Radius = 0_px;

        auto hoverStyle {retValue.create<nav_arrows_style>("nav_arrows", {.Hover = true})};
        hoverStyle->NavArrow = style->NavArrow;

        auto activeStyle {retValue.create<nav_arrows_style>("nav_arrows", {.Active = true})};
        activeStyle->NavArrow = style->NavArrow;

        normal.apply(style);
        hover.apply(hoverStyle);
        active.apply(activeStyle);
    }
    // TAB ITEMS
    {
        auto style {retValue.create<item_style>("tab_items", {}, {})};
        style->Item.Padding        = {5_px};
        style->Item.Text.Style     = {false, font::weight::Normal};
        style->Item.Text.Font      = resGrp.get<font_family>("Poppins");
        style->Item.Text.Size      = 25_pct;
        style->Item.Text.AutoSize  = auto_size_mode::Always;
        style->Item.Text.Alignment = {horizontal_alignment::Centered, vertical_alignment::Middle};
        style->Item.Border.Size    = 3_px;

        auto hoverStyle {retValue.create<item_style>("tab_items", {.Hover = true})};
        hoverStyle->Item = style->Item;

        auto activeStyle {retValue.create<item_style>("tab_items", {.Active = true})};
        activeStyle->Item = style->Item;

        normal.apply(style);
        hover.apply(hoverStyle);
        active.apply(activeStyle);
    }
    // SECTION ITEMS
    {
        auto style {retValue.create<item_style>("section_items", {}, {})};
        style->Item.Padding        = {1_px};
        style->Item.Text.Style     = {false, font::weight::Normal};
        style->Item.Text.Font      = resGrp.get<font_family>("Poppins");
        style->Item.Text.Size      = 50_pct;
        style->Item.Text.Alignment = {horizontal_alignment::Left, vertical_alignment::Middle};
        style->Item.Border.Size    = 3_px;

        auto hoverStyle {retValue.create<item_style>("section_items", {.Hover = true})};
        hoverStyle->Item = style->Item;

        auto activeStyle {retValue.create<item_style>("section_items", {.Active = true})};
        activeStyle->Item = style->Item;

        normal.apply(style);
        hover.apply(hoverStyle);
        active.apply(activeStyle);
    }
    // LIST ITEMS
    {
        auto style {retValue.create<item_style>("list_items", {}, {})};
        style->Item.Padding        = {5_px};
        style->Item.Text.Style     = {false, font::weight::Normal};
        style->Item.Text.Font      = resGrp.get<font_family>("Poppins");
        style->Item.Text.Size      = 50_pct;
        style->Item.Text.Alignment = {horizontal_alignment::Left, vertical_alignment::Middle};
        style->Item.Border.Size    = 3_px;

        auto hoverStyle {retValue.create<item_style>("list_items", {.Hover = true})};
        hoverStyle->Item = style->Item;

        auto activeStyle {retValue.create<item_style>("list_items", {.Active = true})};
        activeStyle->Item = style->Item;

        normal.apply(style);
        hover.apply(hoverStyle);
        active.apply(activeStyle);
    }
    {
        auto style {retValue.create<item_style>("items", {}, {})};
        style->Item.Padding        = {5_px};
        style->Item.Text.Style     = {false, font::weight::Normal};
        style->Item.Text.Font      = resGrp.get<font_family>("Poppins");
        style->Item.Text.Size      = 50_pct;
        style->Item.Text.Alignment = {horizontal_alignment::Left, vertical_alignment::Middle};
        style->Item.Border.Size    = 3_px;

        normal.apply(style);
    }
    // GRID HEADER ITEMS
    {
        auto style {retValue.create<item_style>("header_items", {}, {})};
        style->Item.Padding        = {5_px};
        style->Item.Text.Style     = {false, font::weight::Bold};
        style->Item.Text.Font      = resGrp.get<font_family>("Poppins");
        style->Item.Text.AutoSize  = auto_size_mode::OnlyShrink;
        style->Item.Text.Size      = 15_px;
        style->Item.Text.Alignment = {horizontal_alignment::Centered, vertical_alignment::Middle};
        style->Item.Border.Size    = 5_px;

        auto hoverStyle {retValue.create<item_style>("header_items", {.Hover = true})};
        hoverStyle->Item = style->Item;

        auto activeStyle {retValue.create<item_style>("header_items", {.Active = true})};
        activeStyle->Item = style->Item;

        normal.apply(style);
        style->Item.Background = colors::LightGreen;

        hover.apply(hoverStyle);
        hoverStyle->Item.Background = colors::Orange;

        active.apply(activeStyle);
        activeStyle->Item.Background = colors::BlanchedAlmond;
    }
    // GRID ROW ITEMS
    {
        auto style {retValue.create<item_style>("row_items", {}, {})};
        style->Item.Padding        = {10_px};
        style->Item.Text.Style     = {false, font::weight::Normal};
        style->Item.Text.Font      = resGrp.get<font_family>("Poppins");
        style->Item.Text.AutoSize  = auto_size_mode::OnlyShrink;
        style->Item.Text.Size      = 20_px;
        style->Item.Text.Color     = normal.Text;
        style->Item.Text.Alignment = {horizontal_alignment::Left, vertical_alignment::Middle};
        style->Item.Border.Size    = 2_px;

        auto hoverStyle {retValue.create<item_style>("row_items", {.Hover = true})};
        hoverStyle->Item = style->Item;

        auto activeStyle {retValue.create<item_style>("row_items", {.Active = true})};
        activeStyle->Item = style->Item;

        normal.apply(style);
        style->Item.Background = colors::DarkGreen;

        hover.apply(hoverStyle);
        hoverStyle->Item.Background = colors::LightGreen;

        active.apply(activeStyle);
        activeStyle->Item.Border.Background = colors::SeaGreen;
        activeStyle->Item.Background        = colors::SeaGreen;
        activeStyle->Item.Padding           = {10_px, 10_px, 20_px, 0_px};
    }

    return retValue;
}

////////////////////////////////////////////////////////////

void color_theme::apply(std::shared_ptr<button::style> const& style) const
{
    style->Background            = Background;
    style->Border.Background     = Border;
    style->DropShadow.Color      = DropShadow;
    style->Text.Color            = Text;
    style->Text.Decoration.Color = TextDecoration;
    style->Text.Shadow.Color     = TextShadow;
}

void color_theme::apply(std::shared_ptr<checkbox::style> const& style) const
{
    style->Background        = Background;
    style->Border.Background = Border;
    style->DropShadow.Color  = DropShadow;
    style->Tick.Foreground   = Tick;
}

void color_theme::apply(std::shared_ptr<cycle_button::style> const& style) const
{
    style->Background        = Background;
    style->Border.Background = Border;
    style->DropShadow.Color  = DropShadow;
}

void color_theme::apply(std::shared_ptr<drop_down_list::style> const& style) const
{
    style->Background                      = Background;
    style->Border.Background               = Border;
    style->DropShadow.Color                = DropShadow;
    style->VScrollBar.Bar.HigherBackground = BarHigher;
    style->VScrollBar.Bar.LowerBackground  = BarLower;
}

void color_theme::apply(std::shared_ptr<grid_view::style> const& style) const
{
    style->Background                      = Background;
    style->Border.Background               = Border;
    style->DropShadow.Color                = DropShadow;
    style->VScrollBar.Bar.HigherBackground = BarHigher;
    style->VScrollBar.Bar.LowerBackground  = BarLower;
}

void color_theme::apply(std::shared_ptr<image_box::style> const& style) const
{
    style->Background        = Background;
    style->Border.Background = Border;
    style->DropShadow.Color  = DropShadow;
}

void color_theme::apply(std::shared_ptr<label::style> const& style) const
{
    style->Text.Color            = Text;
    style->Text.Decoration.Color = TextDecoration;
    style->Text.Shadow.Color     = TextShadow;
}

void color_theme::apply(std::shared_ptr<list_box::style> const& style) const
{
    style->Background                      = Background;
    style->Border.Background               = Border;
    style->DropShadow.Color                = DropShadow;
    style->VScrollBar.Bar.HigherBackground = BarHigher;
    style->VScrollBar.Bar.LowerBackground  = BarLower;
}

void color_theme::apply(std::shared_ptr<progress_bar::style> const& style) const
{
    style->Background           = Background;
    style->Border.Background    = Border;
    style->DropShadow.Color     = DropShadow;
    style->Bar.HigherBackground = BarHigher;
    style->Bar.LowerBackground  = BarLower;
}

void color_theme::apply(std::shared_ptr<radio_button::style> const& style) const
{
    style->Background        = Background;
    style->Border.Background = Border;
    style->DropShadow.Color  = DropShadow;
    style->Tick.Foreground   = Tick;
}

void color_theme::apply(std::shared_ptr<slider::style> const& style) const
{
    style->Bar.HigherBackground = BarHigher;
    style->Bar.LowerBackground  = BarLower;
}

void color_theme::apply(std::shared_ptr<spinner::style> const& style) const
{
    style->Background            = Background;
    style->Border.Background     = Border;
    style->DropShadow.Color      = DropShadow;
    style->Text.Color            = Text;
    style->Text.Decoration.Color = TextDecoration;
    style->Text.Shadow.Color     = TextShadow;
}

void color_theme::apply(std::shared_ptr<text_box::style> const& style) const
{
    style->Background            = Background;
    style->Border.Background     = Border;
    style->DropShadow.Color      = DropShadow;
    style->Text.Color            = Text;
    style->Text.Decoration.Color = TextDecoration;
    style->Text.Shadow.Color     = TextShadow;
    style->Caret.Color           = Caret;
}

void color_theme::apply(std::shared_ptr<toggle::style> const& style) const
{
    style->Background        = Background;
    style->Border.Background = Border;
    style->DropShadow.Color  = DropShadow;
    style->Tick.Foreground   = Tick;
}

void color_theme::apply(std::shared_ptr<panel::style> const& style) const
{
    style->Background                      = Container;
    style->Border.Background               = Border;
    style->DropShadow.Color                = DropShadow;
    style->VScrollBar.Bar.HigherBackground = BarHigher;
    style->VScrollBar.Bar.LowerBackground  = BarLower;
    style->HScrollBar.Bar.HigherBackground = BarHigher;
    style->HScrollBar.Bar.LowerBackground  = BarLower;
}

void color_theme::apply(std::shared_ptr<tab_container::style> const& style) const
{
    style->Background        = Container;
    style->Border.Background = Border;
    style->DropShadow.Color  = DropShadow;
}

void color_theme::apply(std::shared_ptr<accordion::style> const& style) const
{
    style->Background        = Container;
    style->Border.Background = Border;
    style->DropShadow.Color  = DropShadow;
}

void color_theme::apply(std::shared_ptr<thumb_style> const& style) const
{
    style->Thumb.Background        = Thumb;
    style->Thumb.Border.Background = Border;
}

void color_theme::apply(std::shared_ptr<item_style> const& style) const
{
    style->Item.Background            = Item;
    style->Item.Border.Background     = Border;
    style->Item.Text.Color            = Text;
    style->Item.Text.Decoration.Color = TextDecoration;
    style->Item.Text.Shadow.Color     = TextShadow;
}

void color_theme::apply(std::shared_ptr<nav_arrows_style> const& style) const
{
    style->NavArrow.Foreground        = NavArrow;
    style->NavArrow.UpBackground      = NavArrowInc;
    style->NavArrow.DownBackground    = NavArrowDec;
    style->NavArrow.Border.Background = Border;
}
