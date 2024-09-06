// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "SoundGeneratorEx.hpp"

#include <iomanip>

#include "../_common/Trim.hpp"

////////////////////////////////////////////////////////////

using namespace tcob::literals;
using namespace std::chrono_literals;

generator_form::generator_form(window* window)
    : form {"generator", window}
    , _font {"trim", "trim"}
{
    font_family::SingleFont(*_font.get_obj(), trim_ttf);
    gen_styles();

    auto mainPanel {create_container<glass>(dock_style::Fill, "main")};
    auto mainPanelLayout {mainPanel->create_layout<grid_layout>(size_i {40, 40})};

    GenPickupCoin        = mainPanelLayout->create_widget<button>({1, 1, 4, 2}, "GenPickupCoin");
    GenPickupCoin->Label = "Pickup Coin";
    GenLaserShot         = mainPanelLayout->create_widget<button>({1, 3, 4, 2}, "GenLaserShot");
    GenLaserShot->Label  = "Laser Shot";
    GenExplosion         = mainPanelLayout->create_widget<button>({1, 5, 4, 2}, "GenExplosion");
    GenExplosion->Label  = "Explosion";
    GenPowerup           = mainPanelLayout->create_widget<button>({1, 7, 4, 2}, "GenPowerup");
    GenPowerup->Label    = "Powerup";
    GenHitHurt           = mainPanelLayout->create_widget<button>({1, 9, 4, 2}, "GenHitHurt");
    GenHitHurt->Label    = "Hit/Hurt";
    GenJump              = mainPanelLayout->create_widget<button>({1, 11, 4, 2}, "GenJump");
    GenJump->Label       = "Jump";
    GenBlipSelect        = mainPanelLayout->create_widget<button>({1, 13, 4, 2}, "GenBlipSelect");
    GenBlipSelect->Label = "Blip Select";
    GenRandom            = mainPanelLayout->create_widget<button>({1, 15, 4, 2}, "GenRandom");
    GenRandom->Label     = "Random";

    auto genSlider {[&](rect_i const& bounds, string const& name, i32 min, i32 max) {
        auto retValue {mainPanelLayout->create_widget<slider>(bounds, name)};
        retValue->Min = min;
        retValue->Max = max;
        auto lblText {mainPanelLayout->create_widget<label>({bounds.X - 4, bounds.Y, bounds.Width, bounds.Height}, "lbl" + name)};
        lblText->Label = name;
        auto lblValue {mainPanelLayout->create_widget<label>({bounds.X + 4, bounds.Y, bounds.Width / 2, bounds.Height}, "lbl" + name + "Val")};
        lblValue->Label = "0";
        retValue->Value.Changed.connect([ptr = lblValue.get()](auto val) {
            std::string str {std::to_string(val / 100.f)};
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            str.erase(str.find_last_not_of('.') + 1, std::string::npos);
            ptr->Label = str;
        });
        retValue->MouseUp.connect([this]() {
            NewWave();
        });
        return retValue;
    }};

    _valAttackTime   = genSlider({10, 18, 4, 2}, "Attack Time", 0, 100);
    _valSustainTime  = genSlider({10, 20, 4, 2}, "Sustain Time", 0, 100);
    _valSustainPunch = genSlider({10, 22, 4, 2}, "Sustain Punch", 0, 100);
    _valDecayTime    = genSlider({10, 24, 4, 2}, "Decay Time", 0, 100);

    _valStartFrequency = genSlider({10, 27, 4, 2}, "Start Frequency", 0, 100);
    _valMinFrequency   = genSlider({10, 29, 4, 2}, "Min Frequency", 0, 100);
    _valSlide          = genSlider({10, 31, 4, 2}, "Slide", -100, 100);
    _valDeltaSlide     = genSlider({10, 33, 4, 2}, "Delta Slide", -100, 100);
    _valVibratoDepth   = genSlider({10, 35, 4, 2}, "Vibrato Depth", 0, 100);
    _valVibratoSpeed   = genSlider({10, 37, 4, 2}, "Vibrato Speed", 0, 100);

    _valChangeAmount = genSlider({21, 18, 4, 2}, "Change Amount", -100, 100);
    _valChangeSpeed  = genSlider({21, 20, 4, 2}, "Change Speed", 0, 100);
    _valSquareDuty   = genSlider({21, 23, 4, 2}, "Square Duty", 0, 100);
    _valDutySweep    = genSlider({21, 25, 4, 2}, "Duty Sweep", -100, 100);
    _valRepeatSpeed  = genSlider({21, 28, 4, 2}, "Repeat Speed", 0, 100);
    _valPhaserOffset = genSlider({21, 31, 4, 2}, "Phaser Offset", -100, 100);
    _valPhaserSweep  = genSlider({21, 33, 4, 2}, "Phaser Sweep", -100, 100);

    _valLowPassFilterCutoff       = genSlider({32, 18, 4, 2}, "LPF Cutoff", 0, 100);
    _valLowPassFilterCutoffSweep  = genSlider({32, 20, 4, 2}, "LPF Cutoff Sweep", -100, 100);
    _valLowPassFilterResonance    = genSlider({32, 22, 4, 2}, "LPF Resonance", 0, 100);
    _valHighPassFilterCutoff      = genSlider({32, 24, 4, 2}, "HPF Cutoff", 0, 100);
    _valHighPassFilterCutoffSweep = genSlider({32, 26, 4, 2}, "HPF Cutoff Sweep", -100, 100);

    _valWaveType = mainPanelLayout->create_widget<drop_down_list>({1, 18, 4, 2}, "Wave Type");
    _valWaveType->add_item("Square");
    _valWaveType->add_item("Sawtooth");
    _valWaveType->add_item("Sine");
    _valWaveType->add_item("Noise");
    _valWaveType->add_item("Triangle");
    _valWaveType->SelectedItemIndex.Changed.connect([&]() { NewWave(); });

    Play          = mainPanelLayout->create_widget<button>({1, 29, 4, 2}, "Play");
    Play->Label   = "Play";
    Mutate        = mainPanelLayout->create_widget<button>({1, 31, 4, 2}, "Mutate");
    Mutate->Label = "Mutate";
    Load          = mainPanelLayout->create_widget<button>({1, 33, 4, 2}, "Load");
    Load->Label   = "Load";
    Save          = mainPanelLayout->create_widget<button>({1, 35, 4, 2}, "Save");
    Save->Label   = "Save";
    Export        = mainPanelLayout->create_widget<button>({1, 37, 4, 2}, "Export");
    Export->Label = "Export";

    Canvas = mainPanelLayout->create_widget<canvas_widget>({6, 1, 33, 16}, "Canvas");
}

void generator_form::gen_styles()
{
    style_collection styles;
    {
        auto style {styles.create<button>("button", {})};
        style->Border.Type    = element::border::type::Solid;
        style->Border.Size    = 3_px;
        style->Border.Radius  = 5_px;
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 50_pct;
        style->Text.Alignment = {horizontal_alignment::Centered, vertical_alignment::Top};
        style->Text.AutoSize  = element::text::auto_size_mode::OnlyShrink;
        style->Margin         = {2_px};
        style->Padding        = {2_px};

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
        style->Text.Color        = colors::Black;

        auto hoverStyle {styles.create<button>("button", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Background        = colors::Black;
        hoverStyle->Border.Background = colors::LightGray;
        hoverStyle->Text.Color        = colors::White;
    }
    {
        auto style {styles.create<label>("label", {})};
        style->Border.Type    = element::border::type::Solid;
        style->Border.Size    = 3_px;
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 40_pct;
        style->Text.Alignment = {horizontal_alignment::Centered, vertical_alignment::Middle};
        style->Text.AutoSize  = element::text::auto_size_mode::OnlyShrink;
        style->Margin         = {2_px};
        style->Padding        = {2_px};

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
        style->Text.Color        = colors::Black;
    }
    {
        auto style {styles.create<slider>("slider", {})};
        style->Margin            = {5_px};
        style->Padding           = {2_px};
        style->ThumbClass        = "slider_thumb";
        style->Bar.Type          = element::bar::type::Continuous;
        style->Bar.Size          = 95_pct;
        style->Bar.Delay         = 200ms;
        style->Bar.Border.Size   = 3_px;
        style->Bar.Border.Radius = 5_px;

        style->Bar.HigherBackground = colors::LightGray;
        style->Bar.LowerBackground  = colors::LightGray;
    }
    {
        auto style {styles.create<thumb_style>("slider_thumb", {}, {})};
        style->Thumb.Type          = element::thumb::type::Rect;
        style->Thumb.LongSide      = 25_pct;
        style->Thumb.ShortSide     = 80_pct;
        style->Thumb.Border.Size   = 3_px;
        style->Thumb.Border.Radius = 5_px;

        style->Thumb.Background        = colors::LightGray;
        style->Thumb.Border.Background = colors::Black;

        auto hoverStyle {styles.create<thumb_style>("slider_thumb", {.Hover = true})};
        hoverStyle->Thumb = style->Thumb;

        hoverStyle->Thumb.Background        = colors::Black;
        hoverStyle->Thumb.Border.Background = colors::LightGray;
    }
    {
        auto style {styles.create<drop_down_list>("drop_down_list", {})};
        style->Border.Size    = 3_px;
        style->Border.Radius  = 5_px;
        style->Margin         = {2_px};
        style->Padding        = {5_px};
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 50_pct;
        style->Text.Alignment = {horizontal_alignment::Centered, vertical_alignment::Middle};
        style->Text.AutoSize  = element::text::auto_size_mode::OnlyShrink;
        style->ItemHeight     = 100_pct;
        style->ItemClass      = "list_items";

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
        style->Text.Color        = colors::Black;
    }
    {
        auto style {styles.create<item_style>("list_items", {}, {})};
        style->Item.Padding        = {2_px};
        style->Item.Text.Style     = {false, font::weight::Normal};
        style->Item.Text.Font      = _font;
        style->Item.Text.Size      = 50_pct;
        style->Item.Text.Alignment = {horizontal_alignment::Left, vertical_alignment::Middle};
        style->Item.Text.AutoSize  = element::text::auto_size_mode::OnlyShrink;
        style->Item.Border.Size    = 3_px;

        style->Item.Background        = colors::LightGray;
        style->Item.Border.Background = colors::Black;
        style->Item.Text.Color        = colors::Black;

        auto hoverStyle {styles.create<item_style>("list_items", {.Hover = true})};
        hoverStyle->Item = style->Item;

        hoverStyle->Item.Background        = colors::Black;
        hoverStyle->Item.Border.Background = colors::Black;
        hoverStyle->Item.Text.Color        = colors::White;

        auto activeStyle {styles.create<item_style>("list_items", {.Active = true})};
        activeStyle->Item = style->Item;

        activeStyle->Item.Background        = colors::LightBlue;
        activeStyle->Item.Border.Background = colors::Black;
        activeStyle->Item.Text.Color        = colors::Black;
    }
    {
        auto style {styles.create<nav_arrows_style>("nav_arrows", {}, {})};
        style->NavArrow.Size.Height   = {0.75f, length::type::Relative};
        style->NavArrow.Size.Width    = {0.15f, length::type::Relative};
        style->NavArrow.Border.Size   = 3_px;
        style->NavArrow.Border.Radius = 0_px;

        style->NavArrow.Foreground        = colors::Black;
        style->NavArrow.IncBackground     = colors::LightGray;
        style->NavArrow.DecBackground     = colors::LightGray;
        style->NavArrow.Border.Background = colors::Black;

        auto hoverStyle {styles.create<nav_arrows_style>("nav_arrows", {.Hover = true})};
        hoverStyle->NavArrow = style->NavArrow;

        hoverStyle->NavArrow.Foreground        = colors::LightGray;
        hoverStyle->NavArrow.IncBackground     = colors::Black;
        hoverStyle->NavArrow.DecBackground     = colors::Black;
        hoverStyle->NavArrow.Border.Background = colors::LightGray;
    }
    Styles = styles;
}

void generator_form::set_values(sound_wave const& wave)
{
    switch (wave.WaveType) {
    case sound_wave::type::Square: _valWaveType->select_item("Square"); break;
    case sound_wave::type::Sawtooth: _valWaveType->select_item("Sawtooth"); break;
    case sound_wave::type::Sine: _valWaveType->select_item("Sine"); break;
    case sound_wave::type::Noise: _valWaveType->select_item("Noise"); break;
    case sound_wave::type::Triangle: _valWaveType->select_item("Triangle"); break;
    }

    _valAttackTime->Value   = wave.AttackTime * 100;
    _valSustainTime->Value  = wave.SustainTime * 100;
    _valSustainPunch->Value = wave.SustainPunch * 100;
    _valDecayTime->Value    = wave.DecayTime * 100;

    _valStartFrequency->Value = wave.StartFrequency * 100;
    _valMinFrequency->Value   = wave.MinFrequency * 100;
    _valSlide->Value          = wave.Slide * 100;
    _valDeltaSlide->Value     = wave.DeltaSlide * 100;
    _valVibratoDepth->Value   = wave.VibratoDepth * 100;
    _valVibratoSpeed->Value   = wave.VibratoSpeed * 100;

    _valChangeAmount->Value = wave.ChangeAmount * 100;
    _valChangeSpeed->Value  = wave.ChangeSpeed * 100;
    _valSquareDuty->Value   = wave.SquareDuty * 100;
    _valDutySweep->Value    = wave.DutySweep * 100;
    _valRepeatSpeed->Value  = wave.RepeatSpeed * 100;
    _valPhaserOffset->Value = wave.PhaserOffset * 100;
    _valPhaserSweep->Value  = wave.PhaserSweep * 100;

    _valLowPassFilterCutoff->Value       = wave.LowPassFilterCutoff * 100;
    _valLowPassFilterCutoffSweep->Value  = wave.LowPassFilterCutoffSweep * 100;
    _valLowPassFilterResonance->Value    = wave.LowPassFilterResonance * 100;
    _valHighPassFilterCutoff->Value      = wave.HighPassFilterCutoff * 100;
    _valHighPassFilterCutoffSweep->Value = wave.HighPassFilterCutoffSweep * 100;

    NewWave();
}

void generator_form::get_values(sound_wave& wave)
{
    auto const waveType {_valWaveType->get_selected_item()};
    if (waveType == "Square") {
        wave.WaveType = sound_wave::type::Square;
    } else if (waveType == "Sawtooth") {
        wave.WaveType = sound_wave::type::Sawtooth;
    } else if (waveType == "Sine") {
        wave.WaveType = sound_wave::type::Sine;
    } else if (waveType == "Noise") {
        wave.WaveType = sound_wave::type::Noise;
    } else if (waveType == "Triangle") {
        wave.WaveType = sound_wave::type::Triangle;
    }

    wave.AttackTime   = _valAttackTime->Value / 100.f;
    wave.SustainTime  = _valSustainTime->Value / 100.f;
    wave.SustainPunch = _valSustainPunch->Value / 100.f;
    wave.DecayTime    = _valDecayTime->Value / 100.f;

    wave.StartFrequency = _valStartFrequency->Value / 100.f;
    wave.MinFrequency   = _valMinFrequency->Value / 100.f;
    wave.Slide          = _valSlide->Value / 100.f;
    wave.DeltaSlide     = _valDeltaSlide->Value / 100.f;
    wave.VibratoDepth   = _valVibratoDepth->Value / 100.f;
    wave.VibratoSpeed   = _valVibratoSpeed->Value / 100.f;

    wave.ChangeAmount = _valChangeAmount->Value / 100.f;
    wave.ChangeSpeed  = _valChangeSpeed->Value / 100.f;
    wave.SquareDuty   = _valSquareDuty->Value / 100.f;
    wave.DutySweep    = _valDutySweep->Value / 100.f;
    wave.RepeatSpeed  = _valRepeatSpeed->Value / 100.f;
    wave.PhaserOffset = _valPhaserOffset->Value / 100.f;
    wave.PhaserSweep  = _valPhaserSweep->Value / 100.f;

    wave.LowPassFilterCutoff       = _valLowPassFilterCutoff->Value / 100.f;
    wave.LowPassFilterCutoffSweep  = _valLowPassFilterResonance->Value / 100.f;
    wave.LowPassFilterResonance    = _valLowPassFilterResonance->Value / 100.f;
    wave.HighPassFilterCutoff      = _valHighPassFilterCutoff->Value / 100.f;
    wave.HighPassFilterCutoffSweep = _valHighPassFilterCutoffSweep->Value / 100.f;
}

////////////////////////////////////////////////////////////

SoundGeneratorEx::SoundGeneratorEx(game& game)
    : scene(game)
    , _form0 {std::make_shared<generator_form>(&get_window())}
{
}

SoundGeneratorEx::~SoundGeneratorEx() = default;

void SoundGeneratorEx::on_start()
{
    get_root_node()->Entity = _form0;
    _form0->NewWave.connect([&] { _waveDirty = true; });
    _form0->GenPickupCoin->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_pickup_coin()); });
    _form0->GenLaserShot->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_laser_shoot()); });
    _form0->GenExplosion->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_explosion()); });
    _form0->GenPowerup->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_powerup()); });
    _form0->GenHitHurt->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_hit_hurt()); });
    _form0->GenJump->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_jump()); });
    _form0->GenBlipSelect->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_blip_select()); });
    _form0->GenRandom->MouseDown.connect([&]() { _form0->set_values(_gen1.generate_random()); });
    _form0->Play->MouseDown.connect([&]() { play_wave(); });
    _form0->Mutate->MouseDown.connect([&]() { _form0->set_values(_gen1.mutate_wave(_wave1)); });
    _form0->Load->MouseDown.connect([&]() {
        config::object loadFile;
        if (loadFile.load("sound_wave0.ini") == load_status::Ok) {
            sound_wave::Deserialize(_wave1, loadFile["wave"]);
            _form0->set_values(_wave1);
        }
    });
    _form0->Save->MouseDown.connect([&]() {
        config::object saveFile;
        sound_wave::Serialize(_wave1, saveFile["wave"]);
        saveFile.save("sound_wave0.ini");
    });
    _form0->Export->MouseDown.connect([&]() {
        auto _ = _audioData.save("sound0.wav");
    });
    _form0->fixed_update(0ms);
    draw_wave();
}

void SoundGeneratorEx::on_draw_to(render_target& target)
{
}

void SoundGeneratorEx::on_update(milliseconds deltaTime)
{
    if (_waveDirty) {
        _form0->get_values(_wave1);
        create_data();
        play_wave();
        draw_wave();
    }
}

void SoundGeneratorEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().get_stats()};
    stream << "avg FPS:" << stats.get_average_FPS();
    stream << " best FPS:" << stats.get_best_FPS();
    stream << " worst FPS:" << stats.get_worst_FPS();

    get_window().Title = "TestGame " + stream.str();
}

void SoundGeneratorEx::on_key_down(keyboard::event& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    default:
        break;
    }
}

void SoundGeneratorEx::on_mouse_motion(mouse::motion_event& ev)
{
}

void SoundGeneratorEx::create_data()
{
    _audioData = _gen1.create_buffer(_wave1);
}

void SoundGeneratorEx::play_wave()
{
    _sound1 = _gen1.create_sound(_wave1);
    _sound1.play();
}

void SoundGeneratorEx::draw_wave()
{
    auto& canvas {_form0->Canvas};

    rect_f const crect {0, 0, canvas->Bounds->Width, canvas->Bounds->Height};

    canvas->clear();

    // Background
    color c {colors::FloralWhite};
    canvas->set_fill_style(c);
    canvas->begin_path();
    canvas->rect(crect);
    canvas->fill();

    // center line
    canvas->set_stroke_width(1);
    canvas->set_stroke_style(colors::Black);
    std::array<point_f, 2> line {{{crect.X, crect.Height / 2.f}, {crect.right(), crect.Height / 2.f}}};
    canvas->begin_path();
    canvas->move_to(line[0]);
    for (u32 i {1}; i < line.size(); ++i) {
        canvas->line_to(line[i]);
    }
    canvas->stroke();

    // waveform
    auto audioData {_audioData.get_data()};
    if (!audioData.empty()) {
        f32       currentSample {0.0f};
        f32 const sampleSize {crect.Width * 2.0f};
        f32 const sampleIncrement {audioData.size() / sampleSize};
        f32 const sampleScale {crect.Height};

        std::vector<point_f> points;
        points.reserve(sampleSize + 1);
        points.emplace_back(crect.X, crect.Height / 2.f);

        for (i32 i {1}; i < sampleSize; i++) {
            f32 const sample {std::clamp<f32>(audioData[static_cast<u64>(currentSample)] * sampleScale, -crect.Height / 2.f, crect.Height / 2.f)};
            points.emplace_back(crect.X + i / 2.0f, (crect.Y + crect.Height / 2.0f) - sample);
            currentSample += sampleIncrement;
        }

        canvas->set_stroke_width(2);
        canvas->set_stroke_style(colors::DarkBlue);

        canvas->begin_path();
        canvas->move_to(points[0]);
        for (u32 i {1}; i < points.size(); ++i) {
            canvas->line_to(points[i]);
        }
        canvas->stroke();
    }

    //  canvas->end_frame();

    _waveDirty = false;
}
