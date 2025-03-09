// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::ui;

////////////////////////////////////////////////////////////

class generator_form : public form {
public:
    generator_form(window* window);

    std::shared_ptr<button> GenPickupCoin;
    std::shared_ptr<button> GenLaserShot;
    std::shared_ptr<button> GenExplosion;
    std::shared_ptr<button> GenPowerup;
    std::shared_ptr<button> GenHitHurt;
    std::shared_ptr<button> GenJump;
    std::shared_ptr<button> GenBlipSelect;
    std::shared_ptr<button> GenRandom;

    std::shared_ptr<button> Play;
    std::shared_ptr<button> Mutate;

    std::shared_ptr<button> Load;
    std::shared_ptr<button> Save;
    std::shared_ptr<button> Export;

    std::shared_ptr<button> Exit;

    std::shared_ptr<canvas_widget> Canvas;

    signal<> NewWave;

    void set_values(sound_wave const& wave);
    void get_values(sound_wave& wave);

private:
    void gen_styles();

    std::shared_ptr<drop_down_list> _valWaveType;

    // Wave envelope parameters
    std::shared_ptr<slider> _valAttackTime;   // 0 to 1
    std::shared_ptr<slider> _valSustainTime;  // 0 to 1
    std::shared_ptr<slider> _valSustainPunch; // 0 to 1
    std::shared_ptr<slider> _valDecayTime;    // 0 to 1

    // Frequency parameters
    std::shared_ptr<slider> _valStartFrequency; // 0 to 1
    std::shared_ptr<slider> _valMinFrequency;   // 0 to 1
    std::shared_ptr<slider> _valSlide;          // -1 to 1
    std::shared_ptr<slider> _valDeltaSlide;     // -1 to 1
    std::shared_ptr<slider> _valVibratoDepth;   // 0 to 1
    std::shared_ptr<slider> _valVibratoSpeed;   // 0 to 1

    // Tone change parameters
    std::shared_ptr<slider> _valChangeAmount; // -1 to 1
    std::shared_ptr<slider> _valChangeSpeed;  // 0 to 1

    // Square wave parameters
    std::shared_ptr<slider> _valSquareDuty; // 0 to 1
    std::shared_ptr<slider> _valDutySweep;  // -1 to 1

    // Repeat parameters
    std::shared_ptr<slider> _valRepeatSpeed; // 0 to 1

    // Phaser parameters
    std::shared_ptr<slider> _valPhaserOffset; // -1 to 1
    std::shared_ptr<slider> _valPhaserSweep;  // -1 to 1

    // Filter parameters
    std::shared_ptr<slider> _valLowPassFilterCutoff;       // 0 to 1
    std::shared_ptr<slider> _valLowPassFilterCutoffSweep;  // -1 to 1
    std::shared_ptr<slider> _valLowPassFilterResonance;    // 0 to 1
    std::shared_ptr<slider> _valHighPassFilterCutoff;      // 0 to 1
    std::shared_ptr<slider> _valHighPassFilterCutoffSweep; // -1 to 1

    assets::owning_asset_ptr<font_family> _font;
};