#include "ExForms.hpp"

using namespace tcob::literals;
using namespace std::chrono_literals;

////////////////////////////////////////////////////////////

i32 stackIdx {0};

auto create_form0(window& wnd, group const& resGrp) -> std::shared_ptr<form_base>
{
    auto bounds {wnd.bounds()};
    // bounds.Position += point_i {40, 40};
    auto retValue {std::make_shared<form<dock_layout>>(form_init {"form0", bounds})};

    auto  tooltip0 {retValue->create_tooltip<tooltip>("tooltip")};
    auto& tooltipLayout {tooltip0->get_layout<panel::default_layout>()};
    tooltip0->Bounds = {0, 0, 170, 50};
    auto& tooltipLabel0 {tooltipLayout.create_widget<label>({5, 5, 160, 40}, "TTLabel0")};
    tooltip0->Popup.connect([&tooltipLabel0](auto const& event) { tooltipLabel0.Label = event.Widget->name(); });

    auto& panel0 {retValue->create_container<panel>(dock_style::Fill, "Panel0")};
    panel0.Flex = {.Width = 100_pct, .Height = 100_pct};
    auto& panel0Layout {panel0.get_layout<panel::default_layout>()};
    panel0.TabStop       = {.Enabled = false};
    panel0.ScrollEnabled = true;

    auto& label0 {panel0Layout.create_widget<label>({0, 520, 200, 40}, "Label0")};
    auto& button0 {panel0Layout.create_widget<button>({0, 0, 200, 100}, "Button0")};
    button0.Label = "äöü";
    button0.Icon  = icon {.Texture = resGrp.get<texture>("anim"), .TextureRegion = "l1"};

    label0.For = &button0;

    auto& spinner0 {panel0Layout.create_widget<spinner>({0, 150, 120, 100}, "Spinner0")};
    spinner0.Min   = 0;
    spinner0.Step  = 25;
    spinner0.Max   = 100;
    spinner0.Value = 0;

    auto& cycleButton0 {panel0Layout.create_widget<cycle_button>({0, 260, 120, 100}, "CycleButton0")};
    cycleButton0.Items.mutate([&](auto& items) {
        items.push_back({"abc"});
        items.push_back({"def"});
        items.push_back({"ghi"});
        items.push_back({"jkl"});
        items.push_back({"mno"});
        items.push_back({"pqr"});
        items.push_back({"stu"});
        items.push_back({"vwx"});
        items.push_back({"yz"});
    });
    cycleButton0.SelectedItemIndex = 0;

    auto& progressBar0 {panel0Layout.create_widget<progress_bar>({130, 150, 75, 200}, "ProgressBar0")};
    progressBar0.Min   = 0;
    progressBar0.Max   = 100;
    progressBar0.Value = 50;

    auto& progressBar1 {panel0Layout.create_widget<progress_bar>({10, 370, 200, 75}, "ProgressBar1")};
    progressBar1.Min   = 0;
    progressBar1.Max   = 100;
    progressBar1.Value = 50;

    auto& sliderPanel {panel0Layout.create_widget<panel>({490, 390, 350, 550}, "SliderPanel")};
    auto& sliderPanelLayout {sliderPanel.get_layout<panel::default_layout>()};

    auto& sliderLabel0 {sliderPanelLayout.create_widget<label>({180, 220, 150, 50}, "Label2")};
    sliderLabel0.TabStop = {.Enabled = false};

    auto& slider0 {sliderPanelLayout.create_widget<slider>({0, 0, 250, 100}, "Slider0")};
    slider0.Min   = 0;
    slider0.Max   = 100;
    slider0.Value = 0;
    slider0.Step  = 1;

    auto& rangeSlider0 {sliderPanelLayout.create_widget<range_slider>({0, 110, 250, 100}, "RangeSlider0")};
    rangeSlider0.Min    = 0;
    rangeSlider0.Max    = 100;
    rangeSlider0.Values = {50.f, 75.f};
    rangeSlider0.Step   = 1;
    rangeSlider0.Values.Changed.connect([&sliderLabel0](auto val) {
        sliderLabel0.Label = std::format("{:.2f}:{:.2f}", val.first, val.second);
    });
    slider0.Value.Changed.connect([&sliderLabel0, &slider0, &rangeSlider0](auto val) {
        sliderLabel0.Label = std::format("{:.2f}", val);
        rangeSlider0.Alpha = slider0.Value / 100.f;
    });
    //  rangeSlider0.MinRange = 10;
    //  rangeSlider0.MaxRange = 50;

    auto& slider1 {sliderPanelLayout.create_widget<slider>({0, 220, 75, 250}, "Slider1")};
    slider1.Min   = 0;
    slider1.Max   = 100;
    slider1.Value = 100;
    slider1.Step  = 5;
    slider1.Value.Changed.connect([&sliderLabel0](auto val) {
        sliderLabel0.Label = std::format("{:.2f}", val);
    });

    auto& rangeSlider1 {sliderPanelLayout.create_widget<range_slider>({100, 220, 75, 250}, "RangeSlider1")};
    rangeSlider1.Min    = 0;
    rangeSlider1.Max    = 1;
    rangeSlider1.Values = {0.01f, 0.15f};
    rangeSlider1.Step   = 0.01f;
    rangeSlider1.Values.Changed.connect([&sliderLabel0](auto val) {
        sliderLabel0.Label = std::format("{:.2f}:{:.2f}", val.first, val.second);
    });
    rangeSlider1.MinRange = 0.05f;
    rangeSlider1.MaxRange = 0.5f;

    auto& treePanel {panel0Layout.create_widget<panel>({850, 250, 200, 300}, "TreePanel")};
    auto& treePanelLayout {treePanel.create_layout<tree_layout>()};
    treePanelLayout.create_widget<button>(0, "").Label = "I";
    treePanelLayout.create_widget<button>(1, "").Label = "I.I";
    treePanelLayout.create_widget<button>(0, "").Label = "II";
    treePanelLayout.create_widget<button>(1, "").Label = "II.I";
    treePanelLayout.create_widget<button>(2, "").Label = "II.I.I";
    treePanelLayout.create_widget<button>(1, "").Label = "II.II";

    auto& stackPanel {panel0Layout.create_widget<panel>({850, 560, 200, 300}, "StackPanel")};
    auto& stackPanelLayout {stackPanel.create_layout<stack_layout>()};
    auto  createStackPanel([&stackPanel, &stackPanelLayout](string const& lbl) -> panel& {
        auto& cp {stackPanelLayout.create_widget<panel>(lbl)};
        auto& cpLayout {cp.create_layout<grid_layout>(size_i {3, 3})};
        cpLayout.create_widget<label>({{0, 0}, {3, 1}}, lbl).Label = lbl;
        auto& btn {cpLayout.create_widget<button>({{0, 2}, {3, 1}}, "next")};
        btn.Label = "next";

        btn.Click.connect([&stackPanel, &stackPanelLayout] {
            stackIdx = (stackIdx + 1) % 5;
            stackPanelLayout.activate_widget(stackPanel.find_child_by_name(std::to_string(stackIdx)));
        });
        return cp;
    });
    auto& cp1 {createStackPanel("0")};
    auto& cp2 {createStackPanel("1")};
    auto& cp3 {createStackPanel("2")};
    auto& cp4 {createStackPanel("3")};
    auto& cp5 {createStackPanel("4")};
    stackPanelLayout.activate_widget(&cp1);

    auto& circlePanel {panel0Layout.create_widget<panel>({1050, 450, 200, 150}, "CirclePanel")};
    auto& circlePanelLayout {circlePanel.create_layout<circle_layout>()};
    auto  createCircleButton([&circlePanelLayout](string const& lbl) {
        auto& btn {circlePanelLayout.create_widget<button>(lbl)};
        btn.Flex  = {20_pct, 25_pct};
        btn.Label = lbl;
    });
    createCircleButton("1");
    createCircleButton("2");
    createCircleButton("3");
    createCircleButton("4");
    createCircleButton("5");
    createCircleButton("6");
    createCircleButton("7");
    createCircleButton("8");
    createCircleButton("9");
    createCircleButton("0");

    auto& textBox0 {panel0Layout.create_widget<text_box>({0, 650, 125, 50}, "TextBox0")};
    textBox0.MaxLength = 9;
    textBox0.Submit.connect([&label0, &textBox0](auto const&) { label0.Label = "submitted: " + *textBox0.Text; });
    textBox0.Selectable = true;

    auto& toggle0 {panel0Layout.create_widget<toggle>({0, 550, 150, 75}, "Toggle0")};
    toggle0.Checked = false;

    auto& gridPanel {panel0Layout.create_widget<panel>({230, 20, 250, 250}, "SPanel0")};
    gridPanel.TabStop = {.Enabled = false};
    auto& gridLayout {gridPanel.create_layout<grid_layout>(size_i {4, 3}, true)};
    auto  createGridWidget {
        [&](rect_i loc, std::string const& name) {
            auto& b {gridLayout.create_widget<button>(loc, "gridB" + name)};
            b.Label   = name;
            b.Tooltip = tooltip0;
        }};
    createGridWidget({0, 0, 2, 1}, "0");
    createGridWidget({2, 0, 2, 1}, "1");
    createGridWidget({0, 1, 4, 1}, "2");
    createGridWidget({0, 2, 1, 1}, "3");
    createGridWidget({1, 2, 1, 1}, "4");
    createGridWidget({2, 2, 1, 1}, "5");
    createGridWidget({3, 2, 1, 1}, "6");

    auto& boxVPanel0 {panel0Layout.create_widget<panel>({490, 20, 250, 250}, "SPanel1")};
    boxVPanel0.TabStop = {.Enabled = false};
    auto& boxVLayout0 {boxVPanel0.create_layout<vertical_layout>()};
    for (i32 i {0}; i < 4; i++) {
        auto& bb {boxVLayout0.create_widget<button>("Button" + std::to_string(i))};
        bb.Label = std::to_string(i);
    }

    auto& boxHPanel1 {panel0Layout.create_widget<panel>({230, 280, 250, 100}, "SPanel2")};
    boxHPanel1.TabStop = {.Enabled = false};
    auto& boxHLayout1 {boxHPanel1.create_layout<horizontal_layout>()};
    for (i32 i {0}; i < 4; i++) {
        auto& chk {boxHLayout1.create_widget<checkbox>("Check" + std::to_string(i))};
        chk.Checked.Changed.connect([&label0, &chk](bool value) { label0.Label = (value ? "checked: " : "unchecked: ") + chk.name(); });
    }

    auto& boxHPanel2 {panel0Layout.create_widget<panel>({490, 280, 250, 100}, "SPanel3")};
    boxHPanel2.TabStop = {.Enabled = false};
    auto& boxHLayout2 {boxHPanel2.create_layout<horizontal_layout>()};
    for (i32 i {0}; i < 4; i++) {
        auto& rb {boxHLayout2.create_widget<radio_button>("Radio" + std::to_string(i))};
        rb.Checked.Changed.connect([&label0, &rb](bool value) { label0.Label = (value ? "checked: " : "unchecked: ") + rb.name(); });
        rb.Cursor = cursor_mode::User1;
    }

    auto& masonryPanel0 {panel0Layout.create_widget<panel>({230, 390, 250, 550}, "SPanel4")};
    masonryPanel0.TabStop = {.Enabled = false};
    masonryPanel0.FocusGained.connect([](widget_event ev) {
        ev.Sender->Bounds = {ev.Sender->Bounds->Position, {800, ev.Sender->Bounds->height()}};
    });
    masonryPanel0.FocusLost.connect([](widget_event ev) {
        ev.Sender->Bounds = {ev.Sender->Bounds->Position, {250, ev.Sender->Bounds->height()}};
    });
    auto& masonryLayout0 {masonryPanel0.create_layout<masonry_layout>(6)};
    for (i32 i {0}; i < 16; i++) {
        auto& fb {masonryLayout0.create_widget<button>("FButton" + std::to_string(i))};
        if (i % 3 == 0) {
            fb.Flex = {.Width = 100_pct, .Height = 40_pct};
        } else if (i % 2 == 0) {
            fb.Flex = {.Width = 100_pct, .Height = 30_pct};
        } else {
            fb.Flex = {.Width = 100_pct, .Height = 20_pct};
        }

        fb.Label = std::to_string(i);
    }
    masonryPanel0.ZOrder = 100;

    auto& dropDownList0 {panel0Layout.create_widget<drop_down_list>({1000, 50, 150, 50}, "DropDownList0")};
    dropDownList0.Items.mutate([&](auto& items) {
        for (i32 i {0}; i < 4; ++i) {
            items.push_back({"item " + std::to_string(i)});
        }
        items.push_back({.Text = "abc", .Icon = {.Texture = resGrp.get<texture>("anim"), .TextureRegion = "l1"}});
    });
    dropDownList0.SelectedItemIndex.Changed.connect([&label0, &dropDownList0](isize value) { label0.Label = "selected: " + std::to_string(value); });
    dropDownList0.HoveredItemIndex.Changed.connect([&label0, &dropDownList0](isize value) { label0.Label = "hovered: " + std::to_string(value); });
    dropDownList0.ZOrder = 10000;

    auto&             listbox0 {panel0Layout.create_widget<list_box>({1200, 80, 150, 300}, "Listbox0")};
    std::vector<item> listboxtItems;
    listboxtItems.push_back({.Text = "abc", .Icon = {.Texture = resGrp.get<texture>("anim"), .TextureRegion = "l1"}});
    for (i32 i {0}; i < 40; ++i) {
        listboxtItems.push_back({.Text = "item " + std::to_string(i)});
    }
    listbox0.Items = listboxtItems;

    listbox0.SelectedItemIndex.Changed.connect([&label0, &listbox0](isize value) { label0.Label = "selected: " + std::to_string(value); });
    listbox0.HoveredItemIndex.Changed.connect([&label0, &listbox0](isize value) { label0.Label = "hovered: " + std::to_string(value); });

    auto& imgBox0 {panel0Layout.create_widget<image_box>({750, 20, 150, 200}, "ImageBox0")};
    imgBox0.Image     = {.Texture       = resGrp.get<texture>("anim"),
                         .TextureRegion = "l1"};
    imgBox0.Draggable = true;
    imgBox0.Dropped.connect([&label0, form = retValue.get()](auto&& ev) {
        label0.Label = std::format("drop:{}", ev.Target ? ev.Target->name() : "");
    });

    imgBox0.start_animation(*resGrp.get<frame_animation>("anim"), playback_mode::Looped);

    auto& gridView0 {panel0Layout.create_widget<grid_view>({1580, 450, 450, 400}, "GridView0")};
    gridView0.Header = {{"Last"}, {"First"}, {"Age"}, {"City"}};
    gridView0.Grid.mutate([&](auto& grid) {
        grid.resize({4, 5});

        std::array<item, 4> items;
        items[0].Text = "Smith";
        items[0].Icon = {.Texture = resGrp.get<texture>("anim"), .TextureRegion = "l1"};
        items[1].Text = "John";
        items[2].Text = "28";
        items[3].Text = "New York";
        grid.assign({0, 0}, items);

        grid.assign({0, 1}, {{"Johnson"}, {"Emily"}, {"35"}, {"Los Angeles"}});
        grid.assign({0, 2}, {{"Brown"}, {"Michael"}, {"22"}, {"Chicago"}});
        grid.assign({0, 3}, {{"Davis"}, {"Sarah"}, {"40"}, {"Houston"}});
        grid.assign({0, 4}, {{"Patel"}, {"Raj"}, {"32"}, {"San Francisco"}});
    });

    gridView0.HeaderSelectable = true;
    gridView0.SelectMode       = grid_select_mode::Row;
    gridView0.SelectedCellIndex.Changed.connect([&label0, &gridView0] {
        if (*gridView0.SelectedCellIndex == point_i {-1, -1}) { return; }
        label0.Label = std::format("grid: {}", gridView0.get_cell(gridView0.SelectedCellIndex).Text);
    });

    auto& treeView0 {panel0Layout.create_widget<tree_view>({1280, 450, 200, 400}, "TreeView0")};
    treeView0.Nodes = {
        {.Item = {.Text = "parent1"}, .Children = {
                                          {.Item = {.Text = "child1"}},
                                          {.Item = {.Text = "child2"}, .Children = {
                                                                           {.Item = {.Text = "grandchild1"}},
                                                                           {.Item = {.Text = "grandchild2"}},
                                                                       }},
                                          {.Item = {.Text = "child3"}},
                                      }},
        {.Item = {.Text = "parent2"}, .Children = {
                                          {.Item = {.Text = "child4"}},
                                          {.Item = {.Text = "child5"}},
                                      }},
        {.Item = {.Text = "parent3"}},
        {.Item = {.Text = "parent4"}},
        {.Item = {.Text = "parent5"}},
        {.Item = {.Text = "parent6"}},
    };

    button0.Click.connect([&] {
        progressBar0.Value = progressBar0.Value == 100
            ? 0
            : progressBar0.Value + 10;
        progressBar1.Value = progressBar1.Value == 100
            ? 0
            : progressBar1.Value + 10;
        listbox0.Items.mutate([&](auto& items) {
            items.push_back({.Text = std::to_string(progressBar0.Value), .Icon = {.Texture = resGrp.get<texture>("blue_boxCheckmark")}, .UserData = {}});
        });

        dropDownList0.Items.mutate([&](auto& items) {
            items.push_back({std::to_string(progressBar0.Value)});
        });

        gridView0.Grid.mutate([&](auto& grid) {
            grid.resize({grid.width(), grid.height() + 1});
            grid.assign({0, grid.height() - 1}, {{"XXX"}, {"XX"}, {std::to_string(progressBar0.Value * 10)}, {"XXXXX"}});
        });
    });

    locate_service<input::system>().InputMode.Changed.connect([&](input::mode mode) {
        if (mode == input::mode::Controller) {
            retValue->find_widget_by_name("gridB0")->focus();
            wnd.Cursor              = nullptr;
            wnd.SystemCursorEnabled = false;
        } else {
            auto defaultCursor {resGrp.get<cursor>("default")};
            wnd.Cursor                = defaultCursor;
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
        c->TransitionDuration = 750ms;
    }
    return retValue;
}

////////////////////////////////////////////////////////////

auto create_form1(window& wnd, assets::group const& resGrp) -> std::shared_ptr<form_base>
{
    auto retValue {std::make_shared<form<manual_layout>>(
        form_init {"form1", wnd.bounds()})};

    {
        auto& panel0 {retValue->create_container<panel>(rect_f {0, 0, 300, 300}, "Panel0")};
        panel0.Flex          = {100_pct, 100_pct};
        panel0.ScrollEnabled = true;
        panel0.Movable       = true;
        panel0.Resizable     = true;
        auto& panel0Layout {panel0.get_layout<panel::default_layout>()};

        auto& panel1 {panel0Layout.create_widget<panel>({50, 30, 300, 300}, "Panel0-1")};
        panel1.ScrollEnabled = true;
        panel1.Movable       = true;
        panel1.Resizable     = true;
        auto& panel1Layout {panel1.get_layout<panel::default_layout>()};

        auto& panel2 {panel1Layout.create_widget<panel>({50, 30, 300, 300}, "Panel0-2")};
        panel2.ScrollEnabled = true;
        panel2.Movable       = true;
        panel2.Resizable     = true;
        auto& panel2Layout {panel2.get_layout<panel::default_layout>()};

        auto& panel3 {panel2Layout.create_widget<panel>({50, 30, 300, 300}, "Panel0-3")};
        panel3.ScrollEnabled = true;
        panel3.Movable       = true;
        panel3.Resizable     = true;
        auto& panel3Layout {panel3.get_layout<panel::default_layout>()};

        auto& button0 {panel3Layout.create_widget<button>({-10, -10, 30, 30}, "Button0")};
        auto& button1 {panel3Layout.create_widget<button>({350, 350, 80, 50}, "Button1")};
    }

    {
        auto& panel0 {retValue->create_container<panel>(rect_f {800, 0, 300, 300}, "Panel3")};
        panel0.Flex          = {.Width = 100_pct, .Height = 100_pct};
        panel0.ScrollEnabled = true;
        panel0.Movable       = true;
        panel0.Resizable     = true;
        panel0.MinSize       = {150, 150};
        panel0.MaxSize       = {450, 450};
        auto& panel0Layout {panel0.get_layout<panel::default_layout>()};
    }

    {
        auto& panel0 {retValue->create_container<panel>(rect_f {800, 350, 300, 300}, "Panel4")};
        panel0.Flex          = {.Width = 100_pct, .Height = 100_pct};
        panel0.ScrollEnabled = true;
        panel0.Movable       = true;
        panel0.Resizable     = true;
        panel0.MinSize       = {50, 150};
        panel0.MaxSize       = {450, 450};
        auto& panel0Layout {panel0.get_layout<panel::default_layout>()};
    }

    {
        auto& panel0 {retValue->create_container<panel>(rect_f {0, 350, 300, 300}, "Panel5")};
        panel0.Flex          = {.Width = 100_pct, .Height = 100_pct};
        panel0.ScrollEnabled = true;
        panel0.Movable       = true;
        panel0.Resizable     = true;
        panel0.MinSize       = {50, 50};
        panel0.MaxSize       = {450, 450};
        auto& panel0Layout {panel0.get_layout<panel::default_layout>()};
    }

    return retValue;
}

auto create_form_terminal(window& wnd, assets::group const& resGrp) -> std::shared_ptr<form_base>
{
    auto retValue {std::make_shared<form<dock_layout>>(form_init {"form2", wnd.bounds()})};

    auto& panel0 {retValue->create_container<panel>(dock_style::Fill, "Panel0")};
    panel0.Flex = {.Width = 100_pct, .Height = 100_pct};
    auto& panel0Layout {panel0.get_layout<panel::default_layout>()};
    panel0.TabStop = {.Enabled = false};

    {
        size_i const termSize {80, 24};
        auto&        terminal0 {panel0Layout.create_widget<terminal>({5, 5, termSize.Width * 14.f, termSize.Height * 28.f}, "Term1")};
        terminal0.Size = termSize;
        terminal0.clear();

        terminal0.add_str("\033[106;34mC\033[105;33mo\033[104;32ml\033[103;31mo\033[102;31mr\033[0m Te\033[31;40mst ");
        terminal0.color_set(color::FromString("#008f11"), colors::White);
        terminal0.add_str("\nThis is line 2");
        terminal0.add_str({79, 23}, "x");
        terminal0.move({20, 10});
        terminal0.add_str("st\033[4CTe");
        terminal0.move({40, 10});
        terminal0.add_str("st\033[4CTe");

        terminal0.curs_set(true);
        terminal0.mouse_set(true);
        terminal0.echo(false);

        auto& button0 {panel0Layout.create_widget<button>({1150, 600, 200, 100}, "Button0")};
        button0.Label = "Button0";
        auto& button1 {panel0Layout.create_widget<button>({1150, 750, 200, 100}, "Button1")};
        button1.Label = "Button1";
        auto& label0 {panel0Layout.create_widget<label>({1150, 500, 400, 100}, "Label0")};

        button0.Click.connect([&terminal0, &label0](auto&& ex) {
            terminal0.rectangle({terminal0.get_xy(), {10, 5}});
        });
        button1.Click.connect([&terminal0](auto const&) {
            {
                io::ifstream str {"term.dump"};
                auto         _ = terminal0.restore(str);
            }
            terminal0.flash();
            locate_service<render_system>().statistics().reset();
        });
        terminal0.Submit.connect([&label0, &terminal0] {
            label0.Label = terminal0.get_str({terminal0.get_xy().X, terminal0.get_xy().Y - 1});
            terminal0.move({0, terminal0.get_xy().Y + 1});
        });
    }

    return retValue;
}

auto create_form_charting(window& wnd, assets::group const& resGrp) -> std::shared_ptr<form_base>
{
    using namespace tcob::ui::charts;

    auto retValue {std::make_shared<form<vertical_layout>>(form_init {"form5", wnd.bounds()})};

    {
        auto& panel0 {retValue->create_container<panel>("Panel0")};
        auto& panel0Layout {panel0.create_layout<box_layout>(size_i {3, 2})};

        auto& barChart0 {panel0Layout.create_widget<bar_chart>("Bar")};
        barChart0.Datasets.mutate([](auto& val) {
            val.push_back({.Name = "bar1", .Value = {0.1f, 0.4f, 0.3f, 0.1f}, .Color = colors::SlateBlue});
            val.push_back({.Name = "bar2", .Value = {0.2f, 0.1f, 0.4f, 0.5f}, .Color = colors::SeaGreen});
            val.push_back({.Name = "bar3", .Value = {0.5f, 0.4f, 0.2f, 0.1f}, .Color = colors::DarkSeaGreen});
        });
        barChart0.TransitionDuration = 250ms;
        barChart0.XAxis              = {.CustomLabels = {"Q1", "Q2", "Q3", "Q4"}};
        barChart0.YAxis              = {.Min = 0, .Max = 1, .SmallStep = 0.05f, .LargeStep = 0.20f};

        auto& lineChart0 {panel0Layout.create_widget<line_chart>("Line")};
        lineChart0.Datasets.mutate([](auto& val) {
            val.push_back({.Name = "line1", .Value = {0.1f, 0.4f, 0.8f, 0.1f, 0.8f, 0.1f, 0.8f}, .Color = colors::SlateBlue, .Marker = {.Type = marker::type::Disc, .Filled = true}});
            val.push_back({.Name = "line2", .Value = {0.2f, 0.75f, 0.75f, 0.15f, 0.15f, 0.85f, 0.97f}, .Color = colors::SeaGreen, .Marker = {.Type = marker::type::Disc, .Filled = false, .Color = colors::Blue, .OutlineColor = colors::Red}});
            val.push_back({.Name = "line3", .Value = {0.3f, 0.15f, 0.5f, 0.25f, 0.5f, 0.25f, 0.83f}, .Color = colors::DarkSeaGreen, .Marker = {.Type = marker::type::Triangle, .Filled = true}});
        });
        lineChart0.TransitionDuration = 250ms;
        lineChart0.XAxis              = {.Min = 1, .Max = 10, .SmallStep = 0.2f, .LargeStep = 1, .LabelPrecision = 2};
        lineChart0.YAxis              = {.Min = 0, .Max = 1, .SmallStep = 0.1f, .LargeStep = 0.5f};

        auto& pieChart0 {panel0Layout.create_widget<pie_chart>("Pie")};

        pieChart0.Datasets.mutate([](pie_chart::dataset_type& val) {
            val.push_back({.Name = "pie1", .Value = 4, .Color = colors::SlateBlue});
            val.push_back({.Name = "pie2", .Value = 6, .Color = colors::SeaGreen});
            val.push_back({.Name = "pie3", .Value = 2, .Color = colors::DarkSeaGreen});
            val.push_back({.Name = "pie4", .Value = 2, .Color = colors::LightSteelBlue});
        });
        pieChart0.TransitionDuration = 500ms;

        auto& legend0 {panel0Layout.create_widget<legend>("Legend")};
        legend0.For  = &barChart0;
        legend0.Flex = {100_pct, 50_pct};

        auto& legend1 {panel0Layout.create_widget<legend>("Legend")};
        legend1.For  = &lineChart0;
        legend1.Flex = {100_pct, 50_pct};

        auto& legend2 {panel0Layout.create_widget<legend>("Legend")};
        legend2.For  = &pieChart0;
        legend2.Flex = {100_pct, 50_pct};
    }

    {
        auto& panel1 {retValue->create_container<panel>("Panel1")};
        auto& panel1Layout {panel1.create_layout<horizontal_layout>()};

        auto& mekkoChart0 {panel1Layout.create_widget<marimekko_chart>("Mekko")};

        mekkoChart0.Datasets.mutate([](marimekko_chart::dataset_type& val) {
            val.push_back({.Name = "mekko1", .Value = {1, 1, 1, 1}, .Color = colors::SlateBlue});
            val.push_back({.Name = "mekko2", .Value = {5, 4, 6, 9}, .Color = colors::SeaGreen});
            val.push_back({.Name = "mekko3", .Value = {6, 2, 2, 6}, .Color = colors::DarkSeaGreen});
        });
        mekkoChart0.TransitionDuration = 250ms;
        mekkoChart0.XAxis              = {.CustomLabels = {"Q1", "Q2", "Q3", "Q4"}};

        auto& legend4 {panel1Layout.create_widget<legend>("Legend")};
        legend4.For  = &mekkoChart0;
        legend4.Flex = {50_pct, 100_pct};

        auto& scatterChart0 {panel1Layout.create_widget<scatter_chart>("Scatter")};
        scatterChart0.Datasets.mutate([](scatter_chart::dataset_type& val) {
            val.push_back({.Name = "scatter1", .Value = {{0, 0}, {2, 3}, {3, 6}}, .Color = colors::SlateBlue, .OutlineColor = colors::Red, .Marker = {.Type = marker::type::Disc, .Filled = true}});
            val.push_back({.Name = "scatter2", .Value = {{3, 0}, {4, 2}, {10, 12}, {4, 3}}, .Color = colors::SeaGreen, .Marker = {.Type = marker::type::Square, .Filled = true}});
            val.push_back({.Name = "scatter3", .Value = {{1, 5}, {9, 1}, {5, 1}, {3, 3}}, .Color = colors::Transparent, .Marker = {.Type = marker::type::Triangle, .Filled = true}});
        });
        scatterChart0.TransitionDuration = 250ms;
        scatterChart0.XAxis              = {.Min = 0, .Max = 10, .SmallStep = 1, .LargeStep = 2};
        scatterChart0.YAxis              = {.Min = 0, .Max = 12, .SmallStep = 1, .LargeStep = 3};

        auto& legend5 {panel1Layout.create_widget<legend>("Legend")};
        legend5.For  = &scatterChart0;
        legend5.Flex = {50_pct, 100_pct};

        auto& radarChart0 {panel1Layout.create_widget<radar_chart>("Radar")};
        radarChart0.Datasets.mutate([](radar_chart::dataset_type& val) {
            val.push_back({.Name = "radar1", .Value = {0.8f, 0.6f, 0.9f, 0.5f, 0.7f, 0.65f}, .Color = colors::SlateBlue, .Marker = {.Type = marker::type::Square, .Filled = true}});
            val.push_back({.Name = "radar2", .Value = {0.5f, 0.7f, 0.4f, 0.95f, 0.45f, 0.8f}, .Color = colors::SeaGreen, .Marker = {.Type = marker::type::Disc, .Filled = true}});
            val.push_back({.Name = "radar3", .Value = {0.3f, 0.9f, 0.6f, 0.7f, 0.2f, 0.55f}, .Color = colors::DarkSeaGreen, .Marker = {.Type = marker::type::Square, .Filled = false}});
            val.push_back({.Name = "radar4", .Value = {0.6f, 0.4f, 0.8f, 0.6f, 0.5f, 0.9f}, .Color = colors::LightSteelBlue, .Marker = {.Type = marker::type::Disc, .Filled = false}});
        });
        radarChart0.TransitionDuration = 250ms;
        radarChart0.YAxis              = {.Min = 0, .Max = 1};
        radarChart0.XAxis              = {.CustomLabels = {"A", "B", "C", "D", "E", "F"}};

        auto& legend6 {panel1Layout.create_widget<legend>("Legend")};
        legend6.For  = &radarChart0;
        legend6.Flex = {50_pct, 100_pct};
    }

    return retValue;
}

auto create_form_displays(window& wnd, assets::group const& resGrp) -> std::shared_ptr<form_base>
{
    auto retValue {std::make_shared<form<dock_layout>>(form_init {.Name = "form2", .Bounds = wnd.bounds()})};

    retValue->Bounds = rect_f {0, 0, (*wnd.Size).Width * 0.8f, static_cast<f32>((*wnd.Size).Height)};

    {
        auto& panel0 {retValue->create_container<panel>(dock_style::Top, "Panel0")};
        panel0.Flex = {.Width = 100_pct, .Height = 25_pct};
        auto& panel0Layout {panel0.create_layout<dock_layout>()};

        auto& canvas {panel0Layout.create_widget<canvas_widget>(dock_style::Fill, "Canvas1")};
        canvas.Bounds.Changed.connect([&canvas](rect_f const& bounds) {
            size_f s {bounds.Size / 3};
            canvas.set_fill_style(colors::Blue);
            canvas.begin_path();
            canvas.rect({0, 0, s.Width, s.Height});
            canvas.fill();

            canvas.set_fill_style(colors::Red);
            canvas.begin_path();
            canvas.rect({s.Width / 2, s.Height / 2, s.Width, s.Height});
            canvas.fill();

            canvas.set_fill_style(colors::Green);
            canvas.begin_path();
            canvas.rect({s.Width, s.Height, s.Width, s.Height});
            canvas.fill();
        });
    }
    {
        auto& panel0 {retValue->create_container<panel>(dock_style::Top, "Panel0")};
        panel0.Flex = {.Width = 100_pct, .Height = 25_pct};
        auto& panel0Layout {panel0.create_layout<dock_layout>()};

        auto& colorPicker00 {panel0Layout.create_widget<color_picker>(dock_style::Fill, "CP1")};
        colorPicker00.SelectedColor.Changed.connect([&wnd](auto val) { wnd.ClearColor = color::FromHSVA(val); });
        colorPicker00.TransitionDuration = 550ms;
    }
    {
        auto& panel0 {retValue->create_container<panel>(dock_style::Top, "Panel0")};
        panel0.Flex = {.Width = 100_pct, .Height = 30_pct};
        auto& panel0Layout {panel0.create_layout<dock_layout>()};

        auto&    dotMatrix {panel0Layout.create_widget<dot_matrix_display>(dock_style::Fill, "DM1")};
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

        dotMatrix.Dots               = dots;
        dotMatrix.TransitionDuration = 500ms;
    }

    {
        auto& panel0 {retValue->create_container<panel>(dock_style::Top, "Panel0")};
        panel0.Flex = {.Width = 100_pct, .Height = 20_pct};
        auto& panel0Layout {panel0.create_layout<grid_layout>(size_i {200, 200})};

        auto& lcdDisplay0 {panel0Layout.create_widget<seven_segment_display>({0, 0, 100, 100}, "LCD0")};
        lcdDisplay0.draw_text("0123456789 -=\"',");
        lcdDisplay0.TransitionDuration = 500ms;
        auto& lcdDisplay1 {panel0Layout.create_widget<seven_segment_display>({100, 0, 100, 100}, "LCD1")};
        // lcdDisplay1.draw_text("ABCDEFGHIJLOPSUZ");
        lcdDisplay1.draw_segments(std::array<seven_segment_display::segment, 1> {{{.A = true, .B = true, .C = true, .D = true, .E = true, .F = true, .G = false}}});
        lcdDisplay1.TransitionDuration = 500ms;
    }

    return retValue;
}

auto create_form_tabcontainer(window& wnd, assets::group const& resGrp) -> std::shared_ptr<form_base>
{
    auto retValue {std::make_shared<form<dock_layout>>(form_init {.Name = "form3", .Bounds = wnd.bounds()})};

    auto createTabs {[](tab_container& tabContainer0) {
        auto& boxVPanel0 {tabContainer0.create_tab<panel>("TAB0")};
        boxVPanel0.TabStop = {.Enabled = false};
        auto& boxVLayout0 {boxVPanel0.create_layout<vertical_layout>()};
        for (i32 i {0}; i < 4; i++) {
            auto& bb {boxVLayout0.create_widget<button>("Button" + std::to_string(i))};
            bb.Label = std::to_string(i);
        }

        auto& gridPanel {tabContainer0.create_tab<panel>("TAB1")};
        gridPanel.TabStop = {.Enabled = false};
        auto& gridLayout {gridPanel.create_layout<grid_layout>(size_i {4, 3}, true)};

        auto createGridWidget {[&](rect_i loc, std::string const& name) {
            auto& b {gridLayout.create_widget<button>(loc, "gridB" + name)};
            b.Label = name;
        }};
        createGridWidget({0, 0, 2, 1}, "0");
        createGridWidget({2, 0, 2, 1}, "1");
        createGridWidget({0, 1, 4, 1}, "2");
        createGridWidget({0, 2, 1, 1}, "3");
        createGridWidget({1, 2, 1, 1}, "4");
        createGridWidget({2, 2, 1, 1}, "5");
        createGridWidget({3, 2, 1, 1}, "6");

        auto& boxHPanel1 {tabContainer0.create_tab<panel>("TAB2")};
        boxHPanel1.TabStop = {.Enabled = false};
        auto& boxHLayout1 {boxHPanel1.create_layout<horizontal_layout>()};
        for (i32 i {0}; i < 4; i++) {
            boxHLayout1.create_widget<checkbox>("Check" + std::to_string(i));
        }
    }};

    {
        auto& tabContainer0 {retValue->create_container<tab_container>(dock_style::Left, "TabContainer0")};
        tabContainer0.Flex = {.Width = 50_pct, .Height = 100_pct};
        createTabs(tabContainer0);
    }

    {
        auto& tabContainer0 {retValue->create_container<tab_container>(dock_style::Left, "TabContainer1")};
        tabContainer0.Flex = {.Width = 50_pct, .Height = 100_pct};

        auto& tabContainer1 {tabContainer0.create_tab<tab_container>("TabContainer11", {.Text = "abc", .Icon = {.Texture = resGrp.get<texture>("anim"), .TextureRegion = "l1"}})};
        createTabs(tabContainer1);

        auto& tabContainer2 {tabContainer0.create_tab<tab_container>("TabContainer12", {.Text = "def"})};
        createTabs(tabContainer2);

        auto& tabContainer3 {tabContainer0.create_tab<tab_container>("TabContainer13", {.Text = "ghi"})};
        createTabs(tabContainer3);

        auto& tabContainer4 {tabContainer0.create_tab<tab_container>("TabContainer14", {.Text = "jkl"})};
        createTabs(tabContainer4);

        auto& tabContainer5 {tabContainer0.create_tab<tab_container>("TabContainer15", {.Text = "mno"})};
        createTabs(tabContainer5);

        auto& tabContainer6 {tabContainer0.create_tab<tab_container>("TabContainer16", {.Text = "pqr"})};
        createTabs(tabContainer6);
    }

    for (auto* c : retValue->all_widgets()) {
        c->TransitionDuration = 500ms;
    }
    return retValue;
}

auto create_form_accordion(window& wnd, assets::group const& resGrp) -> std::shared_ptr<form_base>
{
    auto bounds {wnd.bounds()};
    auto retValue {std::make_shared<form<dock_layout>>(form_init {"form4", bounds})};

    auto const createSections {[](accordion& accordion0) {
        auto& boxVPanel0 {accordion0.create_section<panel>("TAB0")};
        boxVPanel0.TabStop = {.Enabled = false};
        auto& boxVLayout0 {boxVPanel0.create_layout<vertical_layout>()};
        for (i32 i {0}; i < 4; i++) {
            auto& bb {boxVLayout0.create_widget<button>("Button" + std::to_string(i))};
            bb.Label = std::to_string(i);
        }

        auto& gridPanel {accordion0.create_section<panel>("TAB1")};
        gridPanel.TabStop = {.Enabled = false};
        auto& gridLayout {gridPanel.create_layout<grid_layout>(size_i {4, 3}, true)};

        auto const createGridWidget {[&](rect_i loc, std::string const& name) {
            auto& b {gridLayout.create_widget<button>(loc, "gridB" + name)};
            b.Label = name;
        }};
        createGridWidget({0, 0, 2, 1}, "0");
        createGridWidget({2, 0, 2, 1}, "1");
        createGridWidget({0, 1, 4, 1}, "2");
        createGridWidget({0, 2, 1, 1}, "3");
        createGridWidget({1, 2, 1, 1}, "4");
        createGridWidget({2, 2, 1, 1}, "5");
        createGridWidget({3, 2, 1, 1}, "6");

        auto& boxHPanel1 {accordion0.create_section<panel>("TAB2")};
        boxHPanel1.TabStop = {.Enabled = false};
        auto& boxHLayout1 {boxHPanel1.create_layout<horizontal_layout>()};
        for (i32 i {0}; i < 4; i++) {
            boxHLayout1.create_widget<checkbox>("Check" + std::to_string(i));
        }
    }};

    {
        auto& accordion0 {retValue->create_container<accordion>(dock_style::Left, "Accordion1")};
        accordion0.Flex = {.Width = 30_pct, .Height = 100_pct};
        createSections(accordion0);
    }

    {
        auto& panel0 {retValue->create_container<panel>(dock_style::Left, "Panel0")};
        panel0.Flex          = {.Width = 30_pct, .Height = 100_pct};
        panel0.ScrollEnabled = true;
        auto& panel0Layout {panel0.get_layout<panel::default_layout>()};

        auto& accordion0 {panel0Layout.create_widget<accordion>({0, 0, 500, 500}, "Accordion2")};
        createSections(accordion0);
    }

    {
        auto& accordion0 {retValue->create_container<accordion>(dock_style::Left, "Accordion3")};
        accordion0.Flex                  = {.Width = 30_pct, .Height = 100_pct};
        accordion0.MaximizeActiveSection = true;

        auto& accordion1 {accordion0.create_section<accordion>("Accordion31", {.Text = "abc", .Icon = {.Texture = resGrp.get<texture>("anim"), .TextureRegion = "l1"}})};
        createSections(accordion1);

        auto& accordion2 {accordion0.create_section<accordion>("Accordion32", {.Text = "def", .Icon = {.Texture = resGrp.get<texture>("anim"), .TextureRegion = "l1"}})};
        createSections(accordion2);

        auto& accordion3 {accordion0.create_section<accordion>("Accordion33", {.Text = "ghi", .Icon = {.Texture = resGrp.get<texture>("anim"), .TextureRegion = "l1"}})};
        createSections(accordion3);
    }

    for (auto* c : retValue->all_widgets()) {
        c->TransitionDuration = 500ms;
    }
    return retValue;
}

auto create_node_graph(window& wnd, assets::group const& resGrp) -> std::shared_ptr<form_base>
{
    constexpr u32 TYPE_FLOAT {1 << 0};
    constexpr u32 TYPE_INT {1 << 1};
    constexpr u32 TYPE_BOOL {1 << 2};
    constexpr u32 TYPE_COLOR {1 << 3};

    auto  bounds {wnd.bounds()};
    auto  retValue {std::make_shared<form<dock_layout>>(form_init {"form-ng", bounds})};
    auto& panel0 {retValue->create_container<panel>(dock_style::Fill, "Panel0")};
    panel0.Flex = {.Width = 100_pct, .Height = 100_pct};
    auto& panelLayout {panel0.create_layout<dock_layout>()};
    auto& lbl {panelLayout.create_widget<label>(dock_style::Top, "LBL1")};
    lbl.Flex = {.Width = 100_pct, .Height = 5_pct};
    auto& ng {panelLayout.create_widget<node_graph_view>(dock_style::Fill, "NG1")};

    // helpers
    auto const toFloat {[](node_value_types const& v) -> f32 {
        return std::visit(overloaded {
                              [](f32 val) { return val; },
                              [](i32 val) { return static_cast<f32>(val); },
                              [](bool val) { return val ? 1.0f : 0.0f; },
                              [](string const& val) {
                                  auto f {helper::to_number<f32>(val)};
                                  return f ? *f : 0.0f;
                              }},
                          v);
    }};
    auto const toInt {[](node_value_types const& v) -> i32 {
        return std::visit(overloaded {
                              [](i32 val) { return val; },
                              [](f32 val) { return static_cast<i32>(val); },
                              [](auto const&) { return 0; }},
                          v);
    }};
    auto const toBool {[](node_value_types const& v) -> bool {
        return std::visit(overloaded {
                              [](bool val) { return val; },
                              [](f32 val) { return val != 0.0f; },
                              [](i32 val) { return val != 0; },
                              [](auto const&) { return false; }},
                          v);
    }};

    auto const packColor {[](f32 r, f32 g, f32 b) -> string {
        return std::format("{:.4f},{:.4f},{:.4f}", r, g, b);
    }};
    auto const unpackColor {[](string const& s) -> std::array<f32, 3> {
        std::array<f32, 3> out {0.0f, 0.0f, 0.0f};
        usize              i {0};
        for (auto part : s | std::views::split(',')) {
            if (i >= 3) { break; }
            auto f {helper::to_number<f32>(string(part.begin(), part.end()))};
            out[i++] = f ? *f : 0.0f;
        }
        return out;
    }};

    auto const getFloat {[toFloat](auto const& in, usize i) -> f32 {
        return i < in.size() ? toFloat(in[i]) : 0.0f;
    }};
    auto const getBool {[toBool](auto const& in, usize i) -> bool {
        return i < in.size() ? toBool(in[i]) : false;
    }};
    auto const getColor {[unpackColor](auto const& in, usize i) -> std::array<f32, 3> {
        if (i >= in.size()) { return {0.0f, 0.0f, 0.0f}; }
        auto const* s {std::get_if<string>(&in[i])};
        return s ? unpackColor(*s) : std::array<f32, 3> {0.0f, 0.0f, 0.0f};
    }};
    auto const getString {[](auto const& in, usize i) -> string {
        if (i >= in.size()) { return {}; }
        auto const* s {std::get_if<string>(&in[i])};
        return s ? *s : string {};
    }};

    // --- source nodes ---

    node_def floatNode {
        .Title      = "Float",
        .Outputs    = {{.ID      = 1,
                        .Name    = "Value",
                        .Type    = TYPE_FLOAT,
                        .Compute = [](auto const& /*in*/, auto const& vals) -> node_value_types {
                         return std::get<f32>(vals[0]);
                        }}},
        .Parameters = {node_param_float {.Name = "Value", .Value = 1.0f, .Min = -10.0f, .Max = 10.0f, .Step = 0.1f}},
    };

    node_def intNode {
        .Title      = "Int",
        .Outputs    = {{.ID      = 1,
                        .Name    = "Value",
                        .Type    = TYPE_INT,
                        .Compute = [](auto const& /*in*/, auto const& vals) -> node_value_types {
                         return std::get<i32>(vals[0]);
                        }}},
        .Parameters = {node_param_int {.Name = "Value", .Value = 1, .Min = -100, .Max = 100, .Step = 1}},
    };

    node_def boolNode {
        .Title      = "Bool",
        .Outputs    = {{.ID      = 1,
                        .Name    = "Value",
                        .Type    = TYPE_BOOL,
                        .Compute = [](auto const& /*in*/, auto const& vals) -> node_value_types {
                         return std::get<bool>(vals[0]);
                        }}},
        .Parameters = {node_param_bool {.Name = "Value", .Value = true}},
    };

    node_def colorNode {
        .Title      = "Color",
        .Outputs    = {{.ID      = 1,
                        .Name    = "RGB",
                        .Type    = TYPE_COLOR,
                        .Compute = [packColor](auto const& /*in*/, auto const& vals) -> node_value_types {
                         return packColor(std::get<f32>(vals[0]), std::get<f32>(vals[1]), std::get<f32>(vals[2]));
                        }},
                       {.ID      = 2,
                        .Name    = "R",
                        .Type    = TYPE_FLOAT,
                        .Compute = [](auto const& /*in*/, auto const& vals) -> node_value_types {
                         return std::get<f32>(vals[0]);
                        }},
                       {.ID      = 3,
                        .Name    = "G",
                        .Type    = TYPE_FLOAT,
                        .Compute = [](auto const& /*in*/, auto const& vals) -> node_value_types {
                         return std::get<f32>(vals[1]);
                        }},
                       {.ID      = 4,
                        .Name    = "B",
                        .Type    = TYPE_FLOAT,
                        .Compute = [](auto const& /*in*/, auto const& vals) -> node_value_types {
                         return std::get<f32>(vals[2]);
                        }}},
        .Parameters = {
            node_param_float {.Name = "R", .Value = 1.0f, .Min = 0.0f, .Max = 1.0f, .Step = 0.01f},
            node_param_float {.Name = "G", .Value = 0.5f, .Min = 0.0f, .Max = 1.0f, .Step = 0.01f},
            node_param_float {.Name = "B", .Value = 0.2f, .Min = 0.0f, .Max = 1.0f, .Step = 0.01f},
        },
    };

    // --- math nodes ---

    node_def mathNode {
        .Title      = "Math",
        .Inputs     = {{.ID = 1, .Name = "A", .Type = TYPE_FLOAT},
                       {.ID = 2, .Name = "B", .Type = TYPE_FLOAT}},
        .Outputs    = {{.ID      = 3,
                        .Name    = "Result",
                        .Type    = TYPE_FLOAT,
                        .Compute = [getFloat, getString](auto const& in, auto const& vals) -> node_value_types {
                         f32 const    a {getFloat(in, 0)};
                         f32 const    b {getFloat(in, 1)};
                         string const op {std::get<string>(vals[0])};
                         if (op == "add") { return a + b; }
                         if (op == "subtract") { return a - b; }
                         if (op == "multiply") { return a * b; }
                         if (op == "divide") { return b != 0.0f ? a / b : 0.0f; }
                         return 0.0f;
                        }}},
        .Parameters = {node_param_string {.Name = "Op", .Value = "add", .Options = {"add", "subtract", "multiply", "divide"}}},
    };

    node_def clampNode {
        .Title      = "Clamp",
        .Inputs     = {{.ID = 1, .Name = "Value", .Type = TYPE_FLOAT}},
        .Outputs    = {{.ID      = 2,
                        .Name    = "Result",
                        .Type    = TYPE_FLOAT,
                        .Compute = [getFloat](auto const& in, auto const& vals) -> node_value_types {
                         f32 const v {getFloat(in, 0)};
                         f32 const mn {std::get<f32>(vals[0])};
                         f32 const mx {std::get<f32>(vals[1])};
                         return std::clamp(v, std::min(mn, mx), std::max(mn, mx));
                        }}},
        .Parameters = {
            node_param_float {.Name = "Min", .Value = 0.0f, .Min = -10.0f, .Max = 10.0f, .Step = 0.1f},
            node_param_float {.Name = "Max", .Value = 1.0f, .Min = -10.0f, .Max = 10.0f, .Step = 0.1f},
        },
    };

    node_def remapNode {
        .Title      = "Remap",
        .Inputs     = {{.ID = 1, .Name = "Value", .Type = TYPE_FLOAT}},
        .Outputs    = {{.ID      = 2,
                        .Name    = "Result",
                        .Type    = TYPE_FLOAT,
                        .Compute = [getFloat](auto const& in, auto const& vals) -> node_value_types {
                         f32 const v {getFloat(in, 0)};
                         f32 const inMn {std::get<f32>(vals[0])};
                         f32 const inMx {std::get<f32>(vals[1])};
                         f32 const ouMn {std::get<f32>(vals[2])};
                         f32 const ouMx {std::get<f32>(vals[3])};
                         if (inMx == inMn) { return ouMn; }
                         return ouMn + ((v - inMn) / (inMx - inMn)) * (ouMx - ouMn);
                        }}},
        .Parameters = {
            node_param_float {.Name = "In Min", .Value = 0.0f, .Min = -10.0f, .Max = 10.0f, .Step = 0.1f},
            node_param_float {.Name = "In Max", .Value = 1.0f, .Min = -10.0f, .Max = 10.0f, .Step = 0.1f},
            node_param_float {.Name = "Out Min", .Value = 0.0f, .Min = -10.0f, .Max = 10.0f, .Step = 0.1f},
            node_param_float {.Name = "Out Max", .Value = 2.0f, .Min = -10.0f, .Max = 10.0f, .Step = 0.1f},
        },
    };

    node_def quantizeNode {
        .Title      = "Quantize",
        .Inputs     = {{.ID = 1, .Name = "Value", .Type = TYPE_FLOAT | TYPE_INT}},
        .Outputs    = {{.ID      = 2,
                        .Name    = "Result",
                        .Type    = TYPE_FLOAT,
                        .Compute = [getFloat](auto const& in, auto const& vals) -> node_value_types {
                         f32 const v {getFloat(in, 0)};
                         i32 const steps {std::max(1, std::get<i32>(vals[0]))};
                         f32 const s {static_cast<f32>(steps)};
                         return std::round(v * s) / s;
                        }}},
        .Parameters = {node_param_int {.Name = "Steps", .Value = 4, .Min = 1, .Max = 32, .Step = 1}},
    };

    // --- color nodes ---

    node_def colorScaleNode {
        .Title   = "Color Scale",
        .Inputs  = {{.ID = 1, .Name = "Color", .Type = TYPE_COLOR},
                    {.ID = 2, .Name = "Factor", .Type = TYPE_FLOAT}},
        .Outputs = {{.ID      = 3,
                     .Name    = "Result",
                     .Type    = TYPE_COLOR,
                     .Compute = [getFloat, getColor, packColor](auto const& in, auto const& /*vals*/) -> node_value_types {
                         auto const rgb {getColor(in, 0)};
                         f32 const  f {getFloat(in, 1)};
                         return packColor(
                             std::clamp(rgb[0] * f, 0.0f, 1.0f),
                             std::clamp(rgb[1] * f, 0.0f, 1.0f),
                             std::clamp(rgb[2] * f, 0.0f, 1.0f));
                     }}},
    };

    node_def colorMixNode {
        .Title   = "Color Mix",
        .Inputs  = {{.ID = 1, .Name = "A", .Type = TYPE_COLOR},
                    {.ID = 2, .Name = "B", .Type = TYPE_COLOR},
                    {.ID = 3, .Name = "Weight", .Type = TYPE_FLOAT}},
        .Outputs = {{.ID      = 4,
                     .Name    = "Result",
                     .Type    = TYPE_COLOR,
                     .Compute = [getFloat, getColor, packColor](auto const& in, auto const& /*vals*/) -> node_value_types {
                         auto const a {getColor(in, 0)};
                         auto const b {getColor(in, 1)};
                         f32 const  t {std::clamp(getFloat(in, 2), 0.0f, 1.0f)};
                         return packColor(
                             a[0] + (b[0] - a[0]) * t,
                             a[1] + (b[1] - a[1]) * t,
                             a[2] + (b[2] - a[2]) * t);
                     }}},
    };

    node_def luminanceNode {
        .Title   = "Luminance",
        .Inputs  = {{.ID = 1, .Name = "Color", .Type = TYPE_COLOR}},
        .Outputs = {{.ID      = 2,
                     .Name    = "Value",
                     .Type    = TYPE_FLOAT,
                     .Compute = [getColor](auto const& in, auto const& /*vals*/) -> node_value_types {
                         auto const rgb {getColor(in, 0)};
                         return 0.2126f * rgb[0] + 0.7152f * rgb[1] + 0.0722f * rgb[2];
                     }}},
    };

    // --- logic nodes ---

    node_def compareNode {
        .Title      = "Compare",
        .Inputs     = {{.ID = 1, .Name = "A", .Type = TYPE_FLOAT},
                       {.ID = 2, .Name = "B", .Type = TYPE_FLOAT}},
        .Outputs    = {{.ID      = 3,
                        .Name    = "Result",
                        .Type    = TYPE_BOOL,
                        .Compute = [getFloat](auto const& in, auto const& vals) -> node_value_types {
                         f32 const    a {getFloat(in, 0)};
                         f32 const    b {getFloat(in, 1)};
                         string const op {std::get<string>(vals[0])};
                         if (op == ">") { return a > b; }
                         if (op == "<") { return a < b; }
                         if (op == ">=") { return a >= b; }
                         if (op == "<=") { return a <= b; }
                         if (op == "==") { return a == b; }
                         return false;
                        }}},
        .Parameters = {node_param_string {.Name = "Op", .Value = ">", .Options = {">", "<", ">=", "<=", "=="}}},
    };

    node_def colorGateNode {
        .Title   = "Color Gate",
        .Inputs  = {{.ID = 1, .Name = "Color", .Type = TYPE_COLOR},
                    {.ID = 2, .Name = "Enabled", .Type = TYPE_BOOL}},
        .Outputs = {{.ID      = 3,
                     .Name    = "Result",
                     .Type    = TYPE_COLOR,
                     .Compute = [getBool, getColor, packColor](auto const& in, auto const& /*vals*/) -> node_value_types {
                         auto const rgb {getColor(in, 0)};
                         return getBool(in, 1) ? packColor(rgb[0], rgb[1], rgb[2]) : packColor(0.0f, 0.0f, 0.0f);
                     }}},
    };

    // --- sink ---

    node_def outputNode {
        .Title  = "Output",
        .Inputs = {{.ID = 1, .Name = "Color", .Type = TYPE_COLOR},
                   {.ID = 2, .Name = "Luminance", .Type = TYPE_FLOAT}},
    };

    // layout
    uid const floatID {ng.create_node(floatNode, {0.02f, 0.05f})};
    uid const intID {ng.create_node(intNode, {0.02f, 0.25f})};
    uid const boolID {ng.create_node(boolNode, {0.02f, 0.50f})};
    uid const colorID {ng.create_node(colorNode, {0.02f, 0.68f})};
    uid const mathID {ng.create_node(mathNode, {0.28f, 0.05f})};
    uid const clampID {ng.create_node(clampNode, {0.28f, 0.28f})};
    uid const remapID {ng.create_node(remapNode, {0.28f, 0.48f})};
    uid const quantizeID {ng.create_node(quantizeNode, {0.28f, 0.75f})};
    uid const colorScaleID {ng.create_node(colorScaleNode, {0.55f, 0.10f})};
    uid const colorMixID {ng.create_node(colorMixNode, {0.55f, 0.38f})};
    uid const luminanceID {ng.create_node(luminanceNode, {0.55f, 0.68f})};
    uid const compareID {ng.create_node(compareNode, {0.55f, 0.85f})};
    uid const colorGateID {ng.create_node(colorGateNode, {0.78f, 0.38f})};
    uid const outputID {ng.create_node(outputNode, {0.90f, 0.38f})};

    // float -> math A, clamp, remap
    ng.create_connection(floatID, 1, mathID, 1);
    ng.create_connection(floatID, 1, clampID, 1);
    ng.create_connection(floatID, 1, remapID, 1);
    // int -> math B
    ng.create_connection(intID, 1, mathID, 2);
    // math result -> color scale factor, quantize
    ng.create_connection(mathID, 3, colorScaleID, 2);
    ng.create_connection(mathID, 3, quantizeID, 1);
    // color -> color scale, color mix A
    ng.create_connection(colorID, 1, colorScaleID, 1);
    ng.create_connection(colorID, 1, colorMixID, 1);
    // color scale -> color mix B
    ng.create_connection(colorScaleID, 3, colorMixID, 2);
    // clamp -> color mix weight
    ng.create_connection(clampID, 2, colorMixID, 3);
    // color mix -> gate, luminance
    ng.create_connection(colorMixID, 4, colorGateID, 1);
    ng.create_connection(colorMixID, 4, luminanceID, 1);
    // bool -> gate
    ng.create_connection(boolID, 1, colorGateID, 2);
    // gate -> output color
    ng.create_connection(colorGateID, 3, outputID, 1);
    // luminance -> compare A, output
    ng.create_connection(luminanceID, 2, compareID, 1);
    ng.create_connection(luminanceID, 2, outputID, 2);
    // remap -> compare B
    ng.create_connection(remapID, 2, compareID, 2);

    auto const eval {[lbl = &lbl, getColor, getFloat](auto const& in, auto const& /*vals*/) -> node_value_types {
        auto const rgb {getColor(in, 0)};
        f32 const  lum {getFloat(in, 1)};
        lbl->Label = std::format("RGB: ({:.2f}, {:.2f}, {:.2f})  Lum: {:.4f}", rgb[0], rgb[1], rgb[2], lum);
        return {};
    }};

    ng.GraphChanged.connect([ng = &ng, eval, outputID] {
        ng->evaluate(outputID, eval);
    });
    ng.evaluate(outputID, eval);

    return retValue;
}
