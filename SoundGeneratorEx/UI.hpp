// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"
using namespace tcob::ui;

////////////////////////////////////////////////////////////

class generator_form : public form<dock_layout> {
public:
    generator_form(window& window);

    button* GenPickupCoin {nullptr};
    button* GenLaserShot {nullptr};
    button* GenExplosion {nullptr};
    button* GenPowerup {nullptr};
    button* GenHitHurt {nullptr};
    button* GenJump {nullptr};
    button* GenBlipSelect {nullptr};
    button* GenRandom {nullptr};

    button* Play {nullptr};
    button* Mutate {nullptr};

    button* Load {nullptr};
    button* Save {nullptr};
    button* Export {nullptr};

    button* Exit {nullptr};

    canvas_widget* Canvas {nullptr};

    signal<> NewWave;

    void set_values(sound_wave const& wave);
    void get_values(sound_wave& wave);

private:
    void gen_styles();

    drop_down_list* _valWaveType {nullptr};

    // Wave envelope parameters
    slider* _valAttackTime {nullptr};   // 0 to 1
    slider* _valSustainTime {nullptr};  // 0 to 1
    slider* _valSustainPunch {nullptr}; // 0 to 1
    slider* _valDecayTime {nullptr};    // 0 to 1

                                        // Frequency parameters
    slider* _valStartFrequency {nullptr}; // 0 to 1
    slider* _valMinFrequency {nullptr};   // 0 to 1
    slider* _valSlide {nullptr};          // -1 to 1
    slider* _valDeltaSlide {nullptr};     // -1 to 1
    slider* _valVibratoDepth {nullptr};   // 0 to 1
    slider* _valVibratoSpeed {nullptr};   // 0 to 1

                                          // Tone change parameters
    slider* _valChangeAmount {nullptr}; // -1 to 1
    slider* _valChangeSpeed {nullptr};  // 0 to 1

                                        // Square wave parameters
    slider* _valSquareDuty {nullptr}; // 0 to 1
    slider* _valDutySweep {nullptr};  // -1 to 1

                                      // Repeat parameters
    slider* _valRepeatSpeed {nullptr}; // 0 to 1

                                       // Phaser parameters
    slider* _valPhaserOffset {nullptr}; // -1 to 1
    slider* _valPhaserSweep {nullptr};  // -1 to 1

                                        // Filter parameters
    slider* _valLowPassFilterCutoff {nullptr};       // 0 to 1
    slider* _valLowPassFilterCutoffSweep {nullptr};  // -1 to 1
    slider* _valLowPassFilterResonance {nullptr};    // 0 to 1
    slider* _valHighPassFilterCutoff {nullptr};      // 0 to 1
    slider* _valHighPassFilterCutoffSweep {nullptr}; // -1 to 1

    asset_owner_ptr<font_family> _font;
};