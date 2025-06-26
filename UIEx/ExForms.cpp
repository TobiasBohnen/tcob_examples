#include "ExForms.hpp"

using namespace tcob::literals;
using namespace std::chrono_literals;

////////////////////////////////////////////////////////////

auto create_form0(window* wnd, assets::group const& resGrp) -> std::shared_ptr<form_base>
{
    // TODO: remove shared_ptr self capture
    //
    auto retValue {std::make_shared<form<dock_layout>>(form_init {"form0", wnd->bounds()})};

    auto  tooltip0 {retValue->create_tooltip<tooltip>("tooltip")};
    auto& tooltipLayout {tooltip0->get_layout<static_layout>()};
    tooltip0->Bounds = {0, 0, 170, 50};
    auto tooltipLabel0 {tooltipLayout.create_widget<label>({5, 5, 160, 40}, "TTLabel0")};
    tooltip0->Popup.connect([tooltipLabel0](auto const& event) { tooltipLabel0->Label = event.Widget->name(); });

    auto panel0 {retValue->create_container<panel>(dock_style::Fill, "Panel0")};
    panel0->Flex = {.Width = 100_pct, .Height = 100_pct};
    auto& panel0Layout {panel0->get_layout<static_layout>()};
    panel0->TabStop       = {.Enabled = false};
    panel0->ScrollEnabled = true;

    auto label0 {panel0Layout.create_widget<label>({0, 520, 200, 40}, "Label0")};
    auto button0 {panel0Layout.create_widget<button>({0, 0, 200, 100}, "Button0")};
    button0->Label = "In ä Hürri";
    // button0->Icon = {.Texture = resGrp.get<texture>("anim"),                     .Region  = "l1"};
    button0->Click.connect([btn = button0.get(), resGrp = &resGrp]() {
        btn->start_animation(*resGrp->get<frame_animation>("anim"), playback_mode::Looped);
    });

    label0->For = button0;

    auto spinner0 {panel0Layout.create_widget<spinner>({0, 150, 120, 100}, "Spinner0")};
    spinner0->Min   = 0;
    spinner0->Step  = 25;
    spinner0->Max   = 100;
    spinner0->Value = 0;

    auto cycleButton0 {panel0Layout.create_widget<cycle_button>({0, 260, 120, 100}, "CycleButton0")};
    cycleButton0->Items.mutate([&](auto& items) {
        items.push_back({.Text = "abc", .Icon = {.Texture = resGrp.get<texture>("anim"), .Region = "l1"}});
        items.push_back({"def"});
        items.push_back({"ghi"});
    });
    cycleButton0->Click.connect([btn = cycleButton0.get(), resGrp = &resGrp]() {
        btn->start_animation(*resGrp->get<frame_animation>("anim"), playback_mode::Looped);
    });

    auto progressBar0 {panel0Layout.create_widget<progress_bar>({130, 150, 75, 200}, "ProgressBar0")};
    progressBar0->Min   = 0;
    progressBar0->Max   = 100;
    progressBar0->Value = 50;

    auto progressBar1 {panel0Layout.create_widget<progress_bar>({50, 370, 200, 75}, "ProgressBar1")};
    progressBar1->Min   = 0;
    progressBar1->Max   = 100;
    progressBar1->Value = 50;

    auto  sliderPanel {panel0Layout.create_widget<panel>({490, 390, 350, 550}, "SliderPanel")};
    auto& sliderPanelLayout {sliderPanel->get_layout<static_layout>()};

    auto sliderLabel0 {sliderPanelLayout.create_widget<label>({200, 10, 150, 50}, "Label2")};
    sliderLabel0->TabStop = {.Enabled = false};

    auto slider0 {sliderPanelLayout.create_widget<slider>({0, 0, 250, 100}, "Slider0")};
    slider0->Min   = 0;
    slider0->Max   = 255;
    slider0->Value = 50;
    slider0->Step  = 5;
    slider0->Value.Changed.connect([sliderLabel0, slider0](auto val) {
        sliderLabel0->Label = std::to_string(val);
        slider0->Alpha      = val / 255.f;
    });

    auto rangeSlider0 {sliderPanelLayout.create_widget<range_slider>({0, 110, 250, 100}, "RangeSlider0")};
    rangeSlider0->Min    = 0;
    rangeSlider0->Max    = 100;
    rangeSlider0->Values = {50, 75};
    rangeSlider0->Step   = 5;
    rangeSlider0->Values.Changed.connect([sliderLabel0](auto val) {
        sliderLabel0->Label = std::to_string(val.first) + ":" + std::to_string(val.second);
    });
    rangeSlider0->MinRange = 10;
    rangeSlider0->MaxRange = 50;

    auto slider1 {sliderPanelLayout.create_widget<slider>({0, 220, 75, 250}, "Slider1")};
    slider1->Min   = 0;
    slider1->Max   = 100;
    slider1->Value = 100;
    slider1->Step  = 5;
    slider1->Value.Changed.connect([sliderLabel0](auto val) {
        sliderLabel0->Label = std::to_string(val);
    });

    auto rangeSlider1 {sliderPanelLayout.create_widget<range_slider>({100, 220, 75, 250}, "RangeSlider1")};
    rangeSlider1->Min    = 0;
    rangeSlider1->Max    = 100;
    rangeSlider1->Values = {50, 75};
    rangeSlider1->Step   = 5;
    rangeSlider1->Values.Changed.connect([sliderLabel0](auto val) {
        sliderLabel0->Label = std::to_string(val.first) + ":" + std::to_string(val.second);
    });
    rangeSlider1->MinRange = 10;
    rangeSlider1->MaxRange = 50;

    auto textBox0 {panel0Layout.create_widget<text_box>({890, 350, 125, 50}, "TextBox0")};
    textBox0->MaxLength = 9;
    textBox0->Submit.connect([label0, textBox0](auto const&) { label0->Label = "submitted: " + *textBox0->Text; });

    auto toggle0 {panel0Layout.create_widget<toggle>({890, 500, 150, 75}, "Toggle0")};
    toggle0->Checked = true;

    auto gridPanel {panel0Layout.create_widget<panel>({230, 20, 250, 250}, "SPanel0")};
    gridPanel->TabStop = {.Enabled = false};
    auto& gridLayout {gridPanel->create_layout<grid_layout>(size_i {4, 3})};
    auto  createGridWidget {
        [&](rect_i loc, std::string const& name) {
            auto b {gridLayout.create_widget<button>(loc, "gridB" + name, true)};
            b->Label   = name;
            b->Tooltip = tooltip0;
        }};
    createGridWidget({0, 0, 2, 1}, "0");
    createGridWidget({2, 0, 2, 1}, "1");
    createGridWidget({0, 1, 4, 1}, "2");
    createGridWidget({0, 2, 1, 1}, "3");
    createGridWidget({1, 2, 1, 1}, "4");
    createGridWidget({2, 2, 1, 1}, "5");
    createGridWidget({3, 2, 1, 1}, "6");

    auto boxVPanel0 {panel0Layout.create_widget<panel>({490, 20, 250, 250}, "SPanel1")};
    boxVPanel0->TabStop = {.Enabled = false};
    auto& boxVLayout0 {boxVPanel0->create_layout<vertical_layout>()};
    for (i32 i {0}; i < 4; i++) {
        auto bb {boxVLayout0.create_widget<button>("Button" + std::to_string(i))};
        bb->Label = std::to_string(i);
    }

    auto boxHPanel1 {panel0Layout.create_widget<panel>({230, 280, 250, 100}, "SPanel2")};
    boxHPanel1->TabStop = {.Enabled = false};
    auto& boxHLayout1 {boxHPanel1->create_layout<horizontal_layout>()};
    for (i32 i {0}; i < 4; i++) {
        auto chk {boxHLayout1.create_widget<checkbox>("Check" + std::to_string(i))};
        chk->Checked.Changed.connect([label0, chk](bool value) { label0->Label = (value ? "checked: " : "unchecked: ") + chk->name(); });
    }

    auto boxHPanel2 {panel0Layout.create_widget<panel>({490, 280, 250, 100}, "SPanel3")};
    boxHPanel2->TabStop = {.Enabled = false};
    auto& boxHLayout2 {boxHPanel2->create_layout<horizontal_layout>()};
    for (i32 i {0}; i < 4; i++) {
        auto rb {boxHLayout2.create_widget<radio_button>("Radio" + std::to_string(i))};
        rb->Checked.Changed.connect([label0, rb](bool value) { label0->Label = (value ? "checked: " : "unchecked: ") + rb->name(); });
        rb->Cursor = "cursor2";
    }
    retValue->CursorChanged.connect([wnd, form = retValue.get()](string const& cursor) {
        if (wnd->SystemCursorEnabled) { return; }
        if (!wnd->Cursor) { return; }
        wnd->Cursor->ActiveMode = cursor;

        size_f off {wnd->Cursor->bounds().Size};
        form->TooltipOffset = point_f {off.Width, off.Height};
    });

    auto masonryPanel0 {panel0Layout.create_widget<panel>({230, 390, 250, 550}, "SPanel4")};
    masonryPanel0->TabStop = {.Enabled = false};
    masonryPanel0->FocusGained.connect([](widget_event ev) {
        ev.Sender->Bounds = {ev.Sender->Bounds->Position, {800, ev.Sender->Bounds->height()}};
    });
    masonryPanel0->FocusLost.connect([](widget_event ev) {
        ev.Sender->Bounds = {ev.Sender->Bounds->Position, {250, ev.Sender->Bounds->height()}};
    });
    auto& masonryLayout0 {masonryPanel0->create_layout<masonry_layout>(6)};
    for (i32 i {0}; i < 16; i++) {
        auto fb {masonryLayout0.create_widget<button>("FButton" + std::to_string(i))};
        if (i % 3 == 0) {
            fb->Flex = {.Width = 100_pct, .Height = 40_pct};
        } else if (i % 2 == 0) {
            fb->Flex = {.Width = 100_pct, .Height = 30_pct};
        } else {
            fb->Flex = {.Width = 100_pct, .Height = 20_pct};
        }

        fb->Label = std::to_string(i);
    }
    masonryPanel0->ZOrder = 100;

    auto dropDownList0 {panel0Layout.create_widget<drop_down_list>({1000, 50, 150, 50}, "DropDownList0")};
    dropDownList0->Items.mutate([&](auto& items) {
        for (i32 i {0}; i < 4; ++i) {
            items.push_back({"item " + std::to_string(i)});
        }
        items.push_back({.Text = "abc", .Icon = {.Texture = resGrp.get<texture>("anim"), .Region = "l1"}});
    });
    dropDownList0->SelectedItemIndex.Changed.connect([label0, dropDownList0](isize value) { label0->Label = "selected: " + std::to_string(value); });
    dropDownList0->HoveredItemIndex.Changed.connect([label0, dropDownList0](isize value) { label0->Label = "hovered: " + std::to_string(value); });
    dropDownList0->ZOrder = 1;
    dropDownList0->start_animation(*resGrp.get<frame_animation>("anim"), playback_mode::Looped);

    auto              listbox0 {panel0Layout.create_widget<list_box>({1200, 80, 150, 300}, "Listbox0")};
    std::vector<item> listboxtItems;
    listboxtItems.push_back({.Text = "abc", .Icon = {.Texture = resGrp.get<texture>("anim"), .Region = "l1"}});
    for (i32 i {0}; i < 40; ++i) {
        listboxtItems.push_back({.Text = "item " + std::to_string(i)});
    }
    listbox0->Items = listboxtItems;

    listbox0->SelectedItemIndex.Changed.connect([label0, listbox0](isize value) { label0->Label = "selected: " + std::to_string(value); });
    listbox0->HoveredItemIndex.Changed.connect([label0, listbox0](isize value) { label0->Label = "hovered: " + std::to_string(value); });
    listbox0->start_animation(*resGrp.get<frame_animation>("anim"), playback_mode::Looped);

    auto imgBox0 {panel0Layout.create_widget<image_box>({750, 20, 150, 200}, "ImageBox0")};
    imgBox0->Image = {.Texture = resGrp.get<texture>("anim"),
                      .Region  = "l1"};
    // imgBox0->start_animation(*resGrp.get<frame_animation>("anim"), playback_mode::AlternatedLooped);

    auto gridView0 {panel0Layout.create_widget<grid_view>({1080, 400, 450, 400}, "GridView0")};
    gridView0->Header = {{"Last"}, {"First"}, {"Age"}, {"City"}};
    gridView0->Grid.mutate([&](auto& grid) {
        grid.resize({4, 5});

        std::array<item, 4> items;
        items[0].Text = "Smith";
        items[0].Icon = {.Texture = resGrp.get<texture>("anim"), .Region = "l1"};
        items[1].Text = "John";
        items[2].Text = "28";
        items[3].Text = "New York";
        grid.assign({0, 0}, items);

        grid.assign({0, 1}, {{"Johnson"}, {"Emily"}, {"35"}, {"Los Angeles"}});
        grid.assign({0, 2}, {{"Brown"}, {"Michael"}, {"22"}, {"Chicago"}});
        grid.assign({0, 3}, {{"Davis"}, {"Sarah"}, {"40"}, {"Houston"}});
        grid.assign({0, 4}, {{"Patel"}, {"Raj"}, {"32"}, {"San Francisco"}});
    });

    gridView0->HeaderSelectable = true;
    gridView0->SelectMode       = grid_view::select_mode::Row;
    gridView0->SelectedCellIndex.Changed.connect([label0, gridView0]() {
        if (*gridView0->SelectedCellIndex == point_i {-1, -1}) { return; }
        label0->Label = std::format("grid: {}", gridView0->get_cell(gridView0->SelectedCellIndex).Text);
    });
    gridView0->start_animation(*resGrp.get<frame_animation>("anim"), playback_mode::Looped);

    button0->Click.connect([=, &resGrp]() {
        progressBar0->Value = progressBar0->Value == 100
            ? 0
            : progressBar0->Value + 10;
        progressBar1->Value = progressBar1->Value == 100
            ? 0
            : progressBar1->Value + 10;
        listbox0->Items.mutate([&](auto& items) {
            items.push_back({.Text = std::to_string(progressBar0->Value), .Icon = {.Texture = resGrp.get<texture>("blue_boxCheckmark")}, .UserData = {}});
        });

        dropDownList0->Items.mutate([&](auto& items) {
            items.push_back({std::to_string(progressBar0->Value)});
        });

        gridView0->Grid.mutate([&](auto& grid) {
            grid.resize({grid.width(), grid.height() + 1});
            grid.assign({0, grid.height() - 1}, {{"XXX"}, {"XX"}, {std::to_string(progressBar0->Value * 10)}, {"XXXXX"}});
        });
    });

    locate_service<input::system>().InputMode.Changed.connect([retValue, wnd, &resGrp](input::mode mode) {
        if (mode == input::mode::Controller) {
            retValue->find_widget_by_name("gridB0")->focus();
            wnd->Cursor              = nullptr;
            wnd->SystemCursorEnabled = false;
        } else {
            auto defaultCursor {resGrp.get<cursor>("default")};
            wnd->Cursor               = defaultCursor;
            defaultCursor->ActiveMode = "default";
        }
    });

    // nav map
    nav_map nm;
    nm["gridB0"]     = {.Left = "Slider0", .Right = "gridB1", .Down = "gridB2"};
    nm["gridB1"]     = {.Left = "gridB0", .Down = "gridB2"};
    nm["gridB2"]     = {.Up = "gridB0", .Down = "gridB3"};
    nm["gridB3"]     = {.Up = "gridB2", .Right = "gridB4"};
    nm["gridB4"]     = {.Left = "gridB3", .Up = "gridB2", .Right = "gridB5"};
    nm["gridB5"]     = {.Left = "gridB4", .Up = "gridB2", .Right = "gridB6"};
    nm["gridB6"]     = {.Left = "gridB5", .Up = "gridB2", .Down = "Check0"};
    nm["Check0"]     = {.Up = "gridB6", .Right = "Check1"};
    nm["Check1"]     = {.Left = "Check0", .Right = "Check2"};
    nm["Check2"]     = {.Left = "Check1", .Right = "Check3"};
    nm["Check3"]     = {.Left = "Check2"};
    retValue->NavMap = nm;

    for (auto* c : retValue->all_widgets()) {
        c->TransitionDuration = 250ms;
    }
    return retValue;
}

////////////////////////////////////////////////////////////

auto create_form1(window* wnd) -> std::shared_ptr<form_base>
{
    auto retValue {std::make_shared<form<static_layout>>(form_init {"form1", wnd->bounds()})};

    {
        auto panel0 {retValue->create_container<panel>(rect_f {0, 0, 400, 400}, "Panel0")};
        panel0->Flex          = {100_pct, 100_pct};
        panel0->ScrollEnabled = true;
        panel0->Movable       = true;
        auto& panel0Layout {panel0->get_layout<static_layout>()};

        auto panel1 {panel0Layout.create_widget<panel>({50, 30, 400, 400}, "Panel1")};
        panel1->ScrollEnabled = true;
        auto& panel1Layout {panel1->get_layout<static_layout>()};

        auto panel2 {panel1Layout.create_widget<panel>({50, 30, 400, 400}, "Panel2")};
        panel2->ScrollEnabled = true;
        auto& panel2Layout {panel2->get_layout<static_layout>()};

        auto button0 {panel2Layout.create_widget<button>({-10, -10, 30, 30}, "Button0")};
        auto button1 {panel2Layout.create_widget<button>({350, 350, 80, 50}, "Button1")};
    }

    {
        auto panel0 {retValue->create_container<panel>(rect_f {800, 0, 400, 400}, "Panel3")};
        panel0->Flex          = {.Width = 100_pct, .Height = 100_pct};
        panel0->ScrollEnabled = true;
        panel0->Movable       = true;
        auto& panel0Layout {panel0->get_layout<static_layout>()};
    }

    return retValue;
}

auto create_form_terminal(window* wnd) -> std::shared_ptr<form<dock_layout>>
{
    auto retValue {std::make_shared<form<dock_layout>>(form_init {"form2", wnd->bounds()})};

    auto panel0 {retValue->create_container<panel>(dock_style::Fill, "Panel0")};
    panel0->Flex = {.Width = 100_pct, .Height = 100_pct};
    auto& panel0Layout {panel0->get_layout<static_layout>()};
    panel0->TabStop = {.Enabled = false};

    {
        size_i const termSize {80, 24};
        auto         terminal0 {panel0Layout.create_widget<terminal>({5, 5, termSize.Width * 14.f, termSize.Height * 28.f}, "Term1")};
        terminal0->Size = termSize;
        terminal0->clear();

        terminal0->add_str("\033[106;34mC\033[105;33mo\033[104;32ml\033[103;31mo\033[102;31mr\033[0m Te\033[31;40mst ");
        terminal0->color_set(color::FromString("#008f11"), colors::White);
        terminal0->add_str("\nThis is line 2");
        terminal0->add_str({79, 23}, "x");
        terminal0->move({20, 10});
        terminal0->add_str("st\033[4CTe");
        terminal0->move({40, 10});
        terminal0->add_str("st\033[4CTe");

        terminal0->curs_set(true);
        terminal0->mouse_set(true);
        terminal0->echo(false);

        auto button0 {panel0Layout.create_widget<button>({1150, 600, 200, 100}, "Button0")};
        button0->Label = "Button0";
        auto button1 {panel0Layout.create_widget<button>({1150, 750, 200, 100}, "Button1")};
        button1->Label = "Button1";
        auto label0 {panel0Layout.create_widget<label>({1150, 500, 400, 100}, "Label0")};

        button0->Click.connect([terminal0, label0](auto&& ex) {
            terminal0->rectangle({terminal0->get_xy(), {10, 5}});
        });
        button1->Click.connect([terminal0](auto const&) {
            {
                io::ifstream str {"term.dump"};
                auto         _ = terminal0->restore(str);
            }
            terminal0->flash();
            locate_service<render_system>().stats().reset();
        });
        terminal0->Submit.connect([label0, ptr = terminal0.get()]() {
            label0->Label = ptr->get_str({ptr->get_xy().X, ptr->get_xy().Y - 1});
            ptr->move({0, ptr->get_xy().Y + 1});
        });
    }

    return retValue;
}

auto create_form_displays(window* wnd) -> std::shared_ptr<form<dock_layout>>
{
    auto retValue {std::make_shared<form<dock_layout>>(form_init {.Name = "form2", .Bounds = wnd->bounds()})};

    retValue->Bounds = rect_f {0, 0, (*wnd->Size).Width * 0.8f, static_cast<f32>((*wnd->Size).Height)};

    {
        auto panel0 {retValue->create_container<panel>(dock_style::Top, "Panel0")};
        panel0->Flex = {.Width = 100_pct, .Height = 25_pct};
        auto& panel0Layout {panel0->create_layout<dock_layout>()};

        auto canvas {panel0Layout.create_widget<canvas_widget>(dock_style::Fill, "Canvas1")};
        canvas->Bounds.Changed.connect([canvas = canvas.get()](rect_f const& bounds) {
            size_f s {bounds.Size / 3};
            canvas->set_fill_style(colors::Blue);
            canvas->begin_path();
            canvas->rect({0, 0, s.Width, s.Height});
            canvas->fill();

            canvas->set_fill_style(colors::Red);
            canvas->begin_path();
            canvas->rect({s.Width / 2, s.Height / 2, s.Width, s.Height});
            canvas->fill();

            canvas->set_fill_style(colors::Green);
            canvas->begin_path();
            canvas->rect({s.Width, s.Height, s.Width, s.Height});
            canvas->fill();
        });
    }
    {
        auto panel0 {retValue->create_container<panel>(dock_style::Top, "Panel0")};
        panel0->Flex = {.Width = 100_pct, .Height = 25_pct};
        auto& panel0Layout {panel0->create_layout<dock_layout>()};

        auto colorPicker00 {panel0Layout.create_widget<color_picker>(dock_style::Fill, "CP1")};
        colorPicker00->SelectedColor.Changed.connect([wnd](auto val) { wnd->ClearColor = val; });
    }
    {
        auto panel0 {retValue->create_container<panel>(dock_style::Top, "Panel0")};
        panel0->Flex = {.Width = 100_pct, .Height = 25_pct};
        auto& panel0Layout {panel0->create_layout<dock_layout>()};

        auto     dotMatrix {panel0Layout.create_widget<dot_matrix_display>(dock_style::Fill, "DM1")};
        rng      rand;
        grid<u8> dots {{50, 10}};
        string   str {"00000000000036666666666660000000000003666666666666"
                      "00200020002036555555555560020002000203655555555556"
                      "00020212020036566666666460002021202003656666666646"
                      "00020212020036566666666460002021202003656666666646"
                      "00020212020036566666666460002021202003656666666646"
                      "00020212020036566666666460002021202003656666666646"
                      "00020212020036566666666460002021202003656666666646"
                      "00020212020036566666666460002021202003656666666646"
                      "00002010200036566444666460000201020003656644466646"
                      "00000000000036666666666660000000000003666666666666"};
        isize    i {0};
        for (auto c : str) { dots[i++] = (c - '0'); }
        // for (i32 i {0}; i < dots.size(); ++i) { dots[i] = (i / 64 * 4); }
        //  for (i32 i {0}; i < dots.capacity(); ++i) { dots.push_back(rand(0, 255)); }

        dotMatrix->Dots               = dots;
        dotMatrix->TransitionDuration = 500ms;
    }

    {
        auto panel0 {retValue->create_container<panel>(dock_style::Top, "Panel0")};
        panel0->Flex = {.Width = 100_pct, .Height = 25_pct};
        auto& panel0Layout {panel0->create_layout<grid_layout>(size_i {200, 200})};

        auto lcdDisplay0 {panel0Layout.create_widget<seven_segment_display>({0, 0, 100, 100}, "LCD0")};
        lcdDisplay0->draw_text("0123456789 -=\"',");
        lcdDisplay0->TransitionDuration = 500ms;
        auto lcdDisplay1 {panel0Layout.create_widget<seven_segment_display>({100, 0, 100, 100}, "LCD1")};
        // lcdDisplay1->draw_text("ABCDEFGHIJLOPSUZ");
        lcdDisplay1->draw_segments(std::array<seven_segment_display::segment, 1> {{{.A = true, .B = true, .C = true, .D = true, .E = true, .F = true, .G = false}}});
        lcdDisplay1->TransitionDuration = 500ms;
    }

    return retValue;
}

auto create_form_tabcontainer(window* wnd, assets::group const& resGrp) -> std::shared_ptr<form<dock_layout>>
{
    auto retValue {std::make_shared<form<dock_layout>>(form_init {.Name = "form3", .Bounds = wnd->bounds()})};

    auto createTabs {[](std::shared_ptr<tab_container> const& tabContainer0) {
        auto boxVPanel0 {tabContainer0->create_tab<panel>("SPanel0")};
        boxVPanel0->TabStop = {.Enabled = false};
        auto& boxVLayout0 {boxVPanel0->create_layout<vertical_layout>()};
        for (i32 i {0}; i < 4; i++) {
            auto bb {boxVLayout0.create_widget<button>("Button" + std::to_string(i))};
            bb->Label = std::to_string(i);
        }

        auto gridPanel {tabContainer0->create_tab<panel>("SPanel1")};
        gridPanel->TabStop = {.Enabled = false};
        auto& gridLayout {gridPanel->create_layout<grid_layout>(size_i {4, 3})};

        auto createGridWidget {[&](rect_i loc, std::string const& name) {
            auto b {gridLayout.create_widget<button>(loc, "gridB" + name, true)};
            b->Label = name;
        }};
        createGridWidget({0, 0, 2, 1}, "0");
        createGridWidget({2, 0, 2, 1}, "1");
        createGridWidget({0, 1, 4, 1}, "2");
        createGridWidget({0, 2, 1, 1}, "3");
        createGridWidget({1, 2, 1, 1}, "4");
        createGridWidget({2, 2, 1, 1}, "5");
        createGridWidget({3, 2, 1, 1}, "6");

        auto boxHPanel1 {tabContainer0->create_tab<panel>("SPanel2")};
        boxHPanel1->TabStop = {.Enabled = false};
        auto& boxHLayout1 {boxHPanel1->create_layout<horizontal_layout>()};
        for (i32 i {0}; i < 4; i++) {
            boxHLayout1.create_widget<checkbox>("Check" + std::to_string(i));
        }
    }};

    {
        auto tabContainer0 {retValue->create_container<tab_container>(dock_style::Left, "TabContainer0")};
        tabContainer0->Flex = {.Width = 50_pct, .Height = 100_pct};
        createTabs(tabContainer0);
    }

    {
        auto tabContainer0 {retValue->create_container<tab_container>(dock_style::Left, "TabContainer1")};
        tabContainer0->Flex = {.Width = 50_pct, .Height = 100_pct};

        auto tabContainer1 {tabContainer0->create_tab<tab_container>("TabContainer11", {.Text = "abc", .Icon = {.Texture = resGrp.get<texture>("anim"), .Region = "l1"}})};
        createTabs(tabContainer1);

        auto tabContainer2 {tabContainer0->create_tab<tab_container>("TabContainer12")};
        createTabs(tabContainer2);

        auto tabContainer3 {tabContainer0->create_tab<tab_container>("TabContainer13")};
        createTabs(tabContainer3);

        tabContainer0->start_animation(*resGrp.get<frame_animation>("anim"), playback_mode::Looped);
    }

    for (auto* c : retValue->all_widgets()) {
        c->TransitionDuration = 500ms;
    }
    return retValue;
}

auto create_form_accordion(window* wnd, assets::group const& resGrp) -> std::shared_ptr<form<dock_layout>>
{
    auto retValue {std::make_shared<form<dock_layout>>(form_init {.Name = "form4", .Bounds = wnd->bounds()})};

    auto const createSections {[](std::shared_ptr<accordion> const& accordion0) {
        auto boxVPanel0 {accordion0->create_section<panel>("SPanel0")};
        boxVPanel0->TabStop = {.Enabled = false};
        auto& boxVLayout0 {boxVPanel0->create_layout<vertical_layout>()};
        for (i32 i {0}; i < 4; i++) {
            auto bb {boxVLayout0.create_widget<button>("Button" + std::to_string(i))};
            bb->Label = std::to_string(i);
        }

        auto gridPanel {accordion0->create_section<panel>("SPanel1")};
        gridPanel->TabStop = {.Enabled = false};
        auto& gridLayout {gridPanel->create_layout<grid_layout>(size_i {4, 3})};

        auto const createGridWidget {[&](rect_i loc, std::string const& name) {
            auto b {gridLayout.create_widget<button>(loc, "gridB" + name, true)};
            b->Label = name;
        }};
        createGridWidget({0, 0, 2, 1}, "0");
        createGridWidget({2, 0, 2, 1}, "1");
        createGridWidget({0, 1, 4, 1}, "2");
        createGridWidget({0, 2, 1, 1}, "3");
        createGridWidget({1, 2, 1, 1}, "4");
        createGridWidget({2, 2, 1, 1}, "5");
        createGridWidget({3, 2, 1, 1}, "6");

        auto boxHPanel1 {accordion0->create_section<panel>("SPanel2")};
        boxHPanel1->TabStop = {.Enabled = false};
        auto& boxHLayout1 {boxHPanel1->create_layout<horizontal_layout>()};
        for (i32 i {0}; i < 4; i++) {
            boxHLayout1.create_widget<checkbox>("Check" + std::to_string(i));
        }
    }};

    {
        auto accordion0 {retValue->create_container<accordion>(dock_style::Left, "Accordion0")};
        accordion0->Flex = {.Width = 30_pct, .Height = 100_pct};
        createSections(accordion0);
    }

    {
        auto panel0 {retValue->create_container<panel>(dock_style::Left, "Panel0")};
        panel0->Flex          = {.Width = 30_pct, .Height = 100_pct};
        panel0->ScrollEnabled = true;
        auto& panel0Layout {panel0->get_layout<static_layout>()};

        auto accordion0 {panel0Layout.create_widget<accordion>({0, 0, 500, 500}, "Accordion0")};
        createSections(accordion0);
    }

    {
        auto accordion0 {retValue->create_container<accordion>(dock_style::Left, "Accordion0")};
        accordion0->Flex                  = {.Width = 30_pct, .Height = 100_pct};
        accordion0->MaximizeActiveSection = true;

        auto accordion1 {accordion0->create_section<accordion>("Accordion1", {.Text = "abc", .Icon = {.Texture = resGrp.get<texture>("anim"), .Region = "l1"}})};
        createSections(accordion1);

        auto accordion2 {accordion0->create_section<accordion>("Accordion2")};
        createSections(accordion2);

        auto accordion3 {accordion0->create_section<accordion>("Accordion3")};
        createSections(accordion3);

        accordion0->start_animation(*resGrp.get<frame_animation>("anim"), playback_mode::Looped);
    }

    for (auto* c : retValue->all_widgets()) {
        c->TransitionDuration = 500ms;
    }
    return retValue;
}
