// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "UI.hpp"

#include "../_common/Trim.hpp"

using namespace tcob::literals;
using namespace std::chrono_literals;

////////////////////////////////////////////////////////////

generator_form::generator_form(window& window)
    : form {{"generator", window.bounds()}}
    , _font {"trim", "trim"}
{
    font_family::SingleFont(*_font.ptr(), trim_ttf);
    gen_styles();

    auto  mainPanel {create_container<glass>(dock_style::Fill, "main")};
    auto& mainPanelLayout {mainPanel->create_layout<grid_layout>(size_i {40, 40})};

    GenPickupCoin        = mainPanelLayout.create_widget<button>({1, 1, 4, 2}, "GenPickupCoin");
    GenPickupCoin->Label = "Pickup Coin";
    GenLaserShot         = mainPanelLayout.create_widget<button>({1, 3, 4, 2}, "GenLaserShot");
    GenLaserShot->Label  = "Laser Shot";
    GenExplosion         = mainPanelLayout.create_widget<button>({1, 5, 4, 2}, "GenExplosion");
    GenExplosion->Label  = "Explosion";
    GenPowerup           = mainPanelLayout.create_widget<button>({1, 7, 4, 2}, "GenPowerup");
    GenPowerup->Label    = "Powerup";
    GenHitHurt           = mainPanelLayout.create_widget<button>({1, 9, 4, 2}, "GenHitHurt");
    GenHitHurt->Label    = "Hit/Hurt";
    GenJump              = mainPanelLayout.create_widget<button>({1, 11, 4, 2}, "GenJump");
    GenJump->Label       = "Jump";
    GenBlipSelect        = mainPanelLayout.create_widget<button>({1, 13, 4, 2}, "GenBlipSelect");
    GenBlipSelect->Label = "Blip Select";
    GenRandom            = mainPanelLayout.create_widget<button>({1, 15, 4, 2}, "GenRandom");
    GenRandom->Label     = "Random";

    auto genSlider {[&](rect_i const& bounds, string const& name, f32 min, f32 max) {
        auto retValue {mainPanelLayout.create_widget<slider>(bounds, name)};
        retValue->Min  = min;
        retValue->Max  = max;
        retValue->Step = 0.01f;
        auto lblText {mainPanelLayout.create_widget<label>({bounds.left() - 4, bounds.top(), bounds.width(), bounds.height()}, "lbl" + name)};
        lblText->Label = name;
        auto lblValue {mainPanelLayout.create_widget<label>({bounds.left() + 4, bounds.top(), bounds.width() / 2, bounds.height()}, "lbl" + name + "Val")};
        lblValue->Label = "0";
        retValue->Value.Changed.connect([this, lbl = lblValue.get()](auto val) {
            std::string str {std::to_string(std::ceil(val * 100) / 100)};
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            str.erase(str.find_last_not_of('.') + 1, std::string::npos);
            lbl->Label = str;
            NewWave();
        });
        return retValue;
    }};

    _valAttackTime   = genSlider({10, 18, 4, 2}, "Attack Time", 0, 1.0f);
    _valSustainTime  = genSlider({10, 20, 4, 2}, "Sustain Time", 0, 1.0f);
    _valSustainPunch = genSlider({10, 22, 4, 2}, "Sustain Punch", 0, 1.0f);
    _valDecayTime    = genSlider({10, 24, 4, 2}, "Decay Time", 0, 1.0f);

    _valStartFrequency = genSlider({10, 27, 4, 2}, "Start Frequency", 0, 1.0f);
    _valMinFrequency   = genSlider({10, 29, 4, 2}, "Min Frequency", 0, 1.0f);
    _valMinFrequency->Value.Changed.connect([this] { _valStartFrequency->Min = *_valMinFrequency->Value; });

    _valSlide        = genSlider({10, 31, 4, 2}, "Slide", -1.0f, 1.0f);
    _valDeltaSlide   = genSlider({10, 33, 4, 2}, "Delta Slide", -1.0f, 1.0f);
    _valVibratoDepth = genSlider({10, 35, 4, 2}, "Vibrato Depth", 0, 1.0f);
    _valVibratoSpeed = genSlider({10, 37, 4, 2}, "Vibrato Speed", 0, 1.0f);

    _valChangeAmount = genSlider({21, 18, 4, 2}, "Change Amount", -1.0f, 1.0f);
    _valChangeSpeed  = genSlider({21, 20, 4, 2}, "Change Speed", 0, 1.0f);
    _valSquareDuty   = genSlider({21, 23, 4, 2}, "Square Duty", 0, 1.0f);
    _valDutySweep    = genSlider({21, 25, 4, 2}, "Duty Sweep", -1.0f, 1.0f);
    _valRepeatSpeed  = genSlider({21, 28, 4, 2}, "Repeat Speed", 0, 1.0f);
    _valPhaserOffset = genSlider({21, 31, 4, 2}, "Phaser Offset", -1.0f, 1.0f);
    _valPhaserSweep  = genSlider({21, 33, 4, 2}, "Phaser Sweep", -1.0f, 1.0f);

    _valLowPassFilterCutoff       = genSlider({32, 18, 4, 2}, "LPF Cutoff", 0, 1.0f);
    _valLowPassFilterCutoffSweep  = genSlider({32, 20, 4, 2}, "LPF Cutoff Sweep", -1.0f, 1.0f);
    _valLowPassFilterResonance    = genSlider({32, 22, 4, 2}, "LPF Resonance", 0, 1.0f);
    _valHighPassFilterCutoff      = genSlider({32, 24, 4, 2}, "HPF Cutoff", 0, 1.0f);
    _valHighPassFilterCutoffSweep = genSlider({32, 26, 4, 2}, "HPF Cutoff Sweep", -1.0f, 1.0f);

    _valWaveType = mainPanelLayout.create_widget<drop_down_list>({32, 30, 4, 2}, "Wave Type");
    _valWaveType->Items.mutate([](auto& items) {
        items.push_back({"Square", {}, sound_wave::type::Square});
        items.push_back({"Sawtooth", {}, sound_wave::type::Sawtooth});
        items.push_back({"Sine", {}, sound_wave::type::Sine});
        items.push_back({"Triangle", {}, sound_wave::type::Triangle});
        items.push_back({"Noise", {}, sound_wave::type::Noise});
    });
    _valWaveType->SelectedItemIndex = 0;
    _valWaveType->SelectedItemIndex.Changed.connect([&] { NewWave(); });
    auto lblText {mainPanelLayout.create_widget<label>({32 - 4, 30, 4, 2}, "lblWave Type")};
    lblText->Label = "Wave Type";

    Play          = mainPanelLayout.create_widget<button>({1, 25, 4, 2}, "Play");
    Play->Label   = "Play";
    Mutate        = mainPanelLayout.create_widget<button>({1, 27, 4, 2}, "Mutate");
    Mutate->Label = "Mutate";

    Load          = mainPanelLayout.create_widget<button>({1, 30, 4, 2}, "Load");
    Load->Label   = "Load";
    Save          = mainPanelLayout.create_widget<button>({1, 32, 4, 2}, "Save");
    Save->Label   = "Save";
    Export        = mainPanelLayout.create_widget<button>({1, 34, 4, 2}, "Export");
    Export->Label = "Export";

    Exit        = mainPanelLayout.create_widget<button>({1, 37, 4, 2}, "Exit");
    Exit->Label = "Exit";

    Canvas = mainPanelLayout.create_widget<canvas_widget>({6, 1, 33, 16}, "Canvas");
}

void generator_form::gen_styles()
{
    style_collection styles;
    {
        auto style {styles.create<button>("button", {})};
        style->Border.Type    = border_type::Solid;
        style->Border.Size    = 3_px;
        style->Border.Radius  = 5_px;
        style->Text.Style     = {false, font::weight::Normal};
        style->Text.Font      = _font;
        style->Text.Size      = 50_pct;
        style->Text.Alignment = {horizontal_alignment::Centered, vertical_alignment::Top};
        style->Text.AutoSize  = auto_size_mode::OnlyShrink;
        style->Margin         = {2_px};
        style->Padding        = {2_px};

        style->Background        = colors::LightGray;
        style->Border.Background = colors::Black;
        style->Text.Color        = colors::Black;

        auto activeStyle {styles.create<button>("button", {.Active = true, .Hover = true})};
        *activeStyle = *style;

        activeStyle->Background        = colors::Black;
        activeStyle->Border.Background = colors::LightGray;
        activeStyle->Text.Color        = colors::White;

        auto hoverStyle {styles.create<button>("button", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Background        = colors::LightGray;
        hoverStyle->Border.Background = colors::DarkGray;
        hoverStyle->Text.Color        = colors::White;
    }
    {
        auto style {styles.create<label>("label", {})};
        style->Border.Type    = border_type::Solid;
        style->Border.Size    = 3_px;
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
        auto style {styles.create<slider>("slider", {})};
        style->Margin            = {5_px};
        style->Padding           = {2_px};
        style->ThumbClass        = "slider_thumb";
        style->Bar.Size          = 95_pct;
        style->Bar.Delay         = 0ms;
        style->Bar.Border.Size   = 3_px;
        style->Bar.Border.Radius = 5_px;

        style->Bar.HigherBackground = colors::LightGray;
        style->Bar.LowerBackground  = colors::LightGray;
    }
    {
        auto style {styles.create<thumb_style>("slider_thumb", {}, {})};
        style->Thumb.Type          = thumb_type::Rect;
        style->Thumb.LongSide      = 25_pct;
        style->Thumb.ShortSide     = 80_pct;
        style->Thumb.Border.Size   = 3_px;
        style->Thumb.Border.Radius = 5_px;

        style->Thumb.Background        = colors::LightGray;
        style->Thumb.Border.Background = colors::Black;

        auto activeStyle {styles.create<thumb_style>("slider_thumb", {.Active = true})};
        *activeStyle = *style;

        activeStyle->Thumb.Background        = colors::Black;
        activeStyle->Thumb.Border.Background = colors::LightGray;

        auto hoverStyle {styles.create<thumb_style>("slider_thumb", {.Hover = true})};
        *hoverStyle = *style;

        hoverStyle->Thumb.Background        = colors::LightGray;
        hoverStyle->Thumb.Border.Background = colors::DarkGray;
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
        style->Text.AutoSize  = auto_size_mode::OnlyShrink;
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
        style->Item.Text.AutoSize  = auto_size_mode::OnlyShrink;
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
        style->NavArrow.UpBackground      = colors::LightGray;
        style->NavArrow.DownBackground    = colors::LightGray;
        style->NavArrow.Border.Background = colors::Black;

        auto hoverStyle {styles.create<nav_arrows_style>("nav_arrows", {.Hover = true})};
        hoverStyle->NavArrow = style->NavArrow;

        hoverStyle->NavArrow.Foreground        = colors::LightGray;
        hoverStyle->NavArrow.UpBackground      = colors::Black;
        hoverStyle->NavArrow.DownBackground    = colors::Black;
        hoverStyle->NavArrow.Border.Background = colors::LightGray;
    }
    Styles = styles;
}

void generator_form::set_values(sound_wave const& wave)
{
    switch (wave.WaveType) {
    case sound_wave::type::Square:   _valWaveType->select_item("Square"); break;
    case sound_wave::type::Sawtooth: _valWaveType->select_item("Sawtooth"); break;
    case sound_wave::type::Sine:     _valWaveType->select_item("Sine"); break;
    case sound_wave::type::Noise:    _valWaveType->select_item("Noise"); break;
    case sound_wave::type::Triangle: _valWaveType->select_item("Triangle"); break;
    }

    _valAttackTime->Value   = wave.AttackTime;
    _valSustainTime->Value  = wave.SustainTime;
    _valSustainPunch->Value = wave.SustainPunch;
    _valDecayTime->Value    = wave.DecayTime;

    _valStartFrequency->Value = wave.StartFrequency;
    _valMinFrequency->Value   = wave.MinFrequency;
    _valSlide->Value          = wave.Slide;
    _valDeltaSlide->Value     = wave.DeltaSlide;
    _valVibratoDepth->Value   = wave.VibratoDepth;
    _valVibratoSpeed->Value   = wave.VibratoSpeed;

    _valChangeAmount->Value = wave.ChangeAmount;
    _valChangeSpeed->Value  = wave.ChangeSpeed;
    _valSquareDuty->Value   = wave.SquareDuty;
    _valDutySweep->Value    = wave.DutySweep;
    _valRepeatSpeed->Value  = wave.RepeatSpeed;
    _valPhaserOffset->Value = wave.PhaserOffset;
    _valPhaserSweep->Value  = wave.PhaserSweep;

    _valLowPassFilterCutoff->Value       = wave.LowPassFilterCutoff;
    _valLowPassFilterCutoffSweep->Value  = wave.LowPassFilterCutoffSweep;
    _valLowPassFilterResonance->Value    = wave.LowPassFilterResonance;
    _valHighPassFilterCutoff->Value      = wave.HighPassFilterCutoff;
    _valHighPassFilterCutoffSweep->Value = wave.HighPassFilterCutoffSweep;
}

void generator_form::get_values(sound_wave& wave)
{
    auto const waveType {_valWaveType->selected_item()};
    wave.WaveType = std::any_cast<sound_wave::type>(waveType.UserData);

    wave.AttackTime   = _valAttackTime->Value;
    wave.SustainTime  = _valSustainTime->Value;
    wave.SustainPunch = _valSustainPunch->Value;
    wave.DecayTime    = _valDecayTime->Value;

    wave.StartFrequency = _valStartFrequency->Value;
    wave.MinFrequency   = _valMinFrequency->Value;
    wave.Slide          = _valSlide->Value;
    wave.DeltaSlide     = _valDeltaSlide->Value;
    wave.VibratoDepth   = _valVibratoDepth->Value;
    wave.VibratoSpeed   = _valVibratoSpeed->Value;

    wave.ChangeAmount = _valChangeAmount->Value;
    wave.ChangeSpeed  = _valChangeSpeed->Value;
    wave.SquareDuty   = _valSquareDuty->Value;
    wave.DutySweep    = _valDutySweep->Value;
    wave.RepeatSpeed  = _valRepeatSpeed->Value;
    wave.PhaserOffset = _valPhaserOffset->Value;
    wave.PhaserSweep  = _valPhaserSweep->Value;

    wave.LowPassFilterCutoff       = _valLowPassFilterCutoff->Value;
    wave.LowPassFilterCutoffSweep  = _valLowPassFilterCutoffSweep->Value;
    wave.LowPassFilterResonance    = _valLowPassFilterResonance->Value;
    wave.HighPassFilterCutoff      = _valHighPassFilterCutoff->Value;
    wave.HighPassFilterCutoffSweep = _valHighPassFilterCutoffSweep->Value;
}
