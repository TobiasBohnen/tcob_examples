// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ParticleSystemEx.hpp"

using namespace std::chrono_literals;

ParticleSystemEx::ParticleSystemEx(game& game)
    : scene {game}
    , _system0 {true, 50000}
{
    auto const colors {color_gradient {{0.0f, colors::Red},
                                       {0.14f, colors::Orange},
                                       {0.28f, colors::Yellow},
                                       {0.42f, colors::Green},
                                       {0.57f, colors::Blue},
                                       {0.71f, colors::Indigo},
                                       {0.85f, colors::Violet},
                                       {1.0f, colors::Red}}
                           .colors()};
    _colors = {colors.begin(), colors.end()};
}

ParticleSystemEx::~ParticleSystemEx() = default;

void ParticleSystemEx::on_start()
{
    window().ClearColor = colors::Black;
    using namespace tcob::literals;

    auto& resMgr {parent().library()};
    auto* resGrp {resMgr.get_group("res")};
    _system0.Material = resGrp->get<material>("QuadParticleMat");

    constexpr f32 row0y {150};
    constexpr f32 row1y {500};
    constexpr f32 col0x {150};
    constexpr f32 col1x {450};
    constexpr f32 col2x {750};
    constexpr f32 col3x {1050};
    constexpr f32 col4x {1350};

    // -------------------------------------------------------
    // ENGINE EXHAUST  @ col0, row0  — fires left (270)
    // -------------------------------------------------------
    auto& core {_system0.create_emitter()};
    core.Settings.Template = {
        .Speed         = std::minmax(100.f, 300.f),
        .Direction     = std::minmax(268_deg, 272_deg),
        .LinearDamping = std::minmax(0.04f, 0.08f),
        .TextureRegion = "2x2",
        .Colors        = {colors::White, color {100, 220, 255, 255}, color {20, 80, 220, 255}},
        .Transparency  = std::minmax(0.0f, 0.05f),
        .Lifetime      = std::minmax(100ms, 200ms),
        .Scale         = std::minmax(0.3f, 0.8f),
        .Size          = {2, 2},
    };
    core.Settings.SpawnArea = {col0x, row0y - 3, 2, 6};
    core.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 750};

    auto& dust {_system0.create_emitter()};
    dust.Settings.Template = {
        .Speed         = std::minmax(30.f, 125.f),
        .Direction     = std::minmax(262_deg, 278_deg),
        .LinearDamping = std::minmax(0.10f, 0.20f),
        .TextureRegion = "2x2",
        .Colors        = {color {100, 220, 255, 255}, color {20, 80, 220, 255}, color {5, 15, 80, 0}},
        .Transparency  = std::minmax(0.3f, 0.7f),
        .Lifetime      = std::minmax(600ms, 1600ms),
        .Scale         = std::minmax(0.2f, 0.6f),
        .Size          = {2, 2},
    };
    dust.Settings.SpawnArea = {col0x, row0y - 5, 4, 10};
    dust.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 1200};

    // -------------------------------------------------------
    // FIRE  @ col1, row0  — upward
    // -------------------------------------------------------
    auto& firePillar {_system0.create_emitter()};
    firePillar.Settings.Template = {
        .Speed              = std::minmax(40.f, 120.f),
        .Direction          = std::minmax(-10_deg, 10_deg),
        .LinearAcceleration = std::minmax(10.f, 30.f),
        .LinearDamping      = std::minmax(0.05f, 0.12f),
        .TextureRegion      = "2x2",
        .Colors             = {colors::White, colors::Yellow, colors::OrangeRed, colors::Red},
        .Transparency       = std::minmax(0.0f, 0.2f),
        .Lifetime           = std::minmax(400ms, 900ms),
        .Scale              = std::minmax(0.5f, 2.0f),
        .Size               = {2, 2},
        .Spin               = std::minmax(-20_deg, 20_deg),
    };
    firePillar.Settings.SpawnArea = {col1x - 12, row0y, 24, 4};
    firePillar.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 800};

    auto& fireEmbers {_system0.create_emitter()};
    fireEmbers.Settings.Template = {
        .Speed         = std::minmax(80.f, 200.f),
        .Direction     = std::minmax(-20_deg, 20_deg),
        .LinearDamping = std::minmax(0.15f, 0.3f),
        .Gravity       = {{-5.f, 0.f}, {5.f, 0.f}},
        .TextureRegion = "2x2",
        .Colors        = {colors::White, colors::Yellow, color {255, 80, 0, 255}},
        .Transparency  = std::minmax(0.2f, 0.5f),
        .Lifetime      = std::minmax(600ms, 1400ms),
        .Scale         = std::minmax(0.2f, 0.5f),
        .Size          = {2, 2},
    };
    fireEmbers.Settings.SpawnArea = {col1x - 10, row0y, 20, 4};
    fireEmbers.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 300};

    auto& fireSmoke {_system0.create_emitter()};
    fireSmoke.Settings.Template = {
        .Speed         = std::minmax(15.f, 40.f),
        .Direction     = std::minmax(-15_deg, 15_deg),
        .LinearDamping = std::minmax(0.02f, 0.06f),
        .Gravity       = {{-8.f, 0.f}, {8.f, 0.f}},
        .TextureRegion = "2x2",
        .Colors        = {color {80, 80, 80, 180}, color {50, 50, 50, 80}, color {30, 30, 30, 0}},
        .Transparency  = std::minmax(0.5f, 0.8f),
        .Lifetime      = std::minmax(1500ms, 3000ms),
        .Scale         = std::minmax(2.f, 6.f),
        .Size          = {2, 2},
    };
    fireSmoke.Settings.SpawnArea = {col1x - 8, row0y, 16, 4};
    fireSmoke.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 60};

    // -------------------------------------------------------
    // SPARKS  @ col2, row0  — omnidirectional
    // -------------------------------------------------------
    auto& sparks {_system0.create_emitter()};
    sparks.Settings.Template = {
        .Speed         = std::minmax(100.f, 350.f),
        .Direction     = std::minmax(0_deg, 360_deg),
        .LinearDamping = std::minmax(0.08f, 0.18f),
        .Gravity       = {{0.f, 30.f}, {0.f, 30.f}},
        .TextureRegion = "2x2",
        .Colors        = {colors::White, colors::Yellow, color {255, 160, 0, 255}},
        .Transparency  = std::minmax(0.0f, 0.1f),
        .Lifetime      = std::minmax(300ms, 900ms),
        .Scale         = std::minmax(0.2f, 0.5f),
        .Size          = {2, 2},
    };
    sparks.Settings.SpawnArea = {col2x, row0y, 4, 4};
    sparks.Settings.Pattern   = particle_emitter::emit_burst {.Count = 8, .Interval = 80ms, .Repeats = 100};

    // -------------------------------------------------------
    // MAGIC PORTAL  @ col3, row0  — inward spiral
    // -------------------------------------------------------
    auto& portalRing {_system0.create_emitter()};
    portalRing.Settings.Template = {
        .Speed              = std::minmax(60.f, 100.f),
        .Direction          = std::minmax(0_deg, 360_deg),
        .RadialAcceleration = std::minmax(-120.f, -80.f),
        .TextureRegion      = "2x2",
        .Colors             = {color {180, 0, 255, 255}, color {80, 0, 200, 255}, color {20, 0, 80, 0}},
        .Transparency       = std::minmax(0.1f, 0.4f),
        .Lifetime           = std::minmax(500ms, 1200ms),
        .Scale              = std::minmax(0.3f, 0.8f),
        .Size               = {2, 2},
        .Spin               = std::minmax(-180_deg, 180_deg),
    };
    portalRing.Settings.SpawnArea = {col3x - 60, row0y - 60, 120, 120};
    portalRing.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 600};

    auto& portalCore {_system0.create_emitter()};
    portalCore.Settings.Template = {
        .Speed         = std::minmax(5.f, 20.f),
        .Direction     = std::minmax(0_deg, 360_deg),
        .LinearDamping = std::minmax(0.1f, 0.2f),
        .TextureRegion = "2x2",
        .Colors        = {colors::White, color {220, 180, 255, 255}, color {140, 0, 255, 0}},
        .Transparency  = std::minmax(0.0f, 0.2f),
        .Lifetime      = std::minmax(200ms, 600ms),
        .Scale         = std::minmax(0.2f, 1.0f),
        .Size          = {2, 2},
    };
    portalCore.Settings.SpawnArea = {col3x - 8, row0y - 8, 16, 16};
    portalCore.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 400};

    // -------------------------------------------------------
    // LIGHTNING  @ col4, row0  — downward (180) with scatter
    // -------------------------------------------------------
    auto& lightning {_system0.create_emitter()};
    lightning.Settings.Template = {
        .Speed                  = std::minmax(200.f, 600.f),
        .Direction              = std::minmax(160_deg, 200_deg),
        .LinearDamping          = std::minmax(0.3f, 0.6f),
        .TangentialAcceleration = std::minmax(-200.f, 200.f),
        .TextureRegion          = "2x2",
        .Colors                 = {colors::White, color {200, 200, 255, 255}, color {100, 100, 255, 255}, color {40, 40, 180, 0}},
        .Transparency           = std::minmax(0.0f, 0.05f),
        .Lifetime               = std::minmax(40ms, 120ms),
        .Scale                  = std::minmax(0.2f, 0.6f),
        .Size                   = {2, 2},
    };
    lightning.Settings.SpawnArea = {col4x, row0y, 4, 4};
    lightning.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 1500};

    auto& lightningGlow {_system0.create_emitter()};
    lightningGlow.Settings.Template = {
        .Speed         = std::minmax(20.f, 80.f),
        .Direction     = std::minmax(0_deg, 360_deg),
        .LinearDamping = std::minmax(0.2f, 0.4f),
        .TextureRegion = "2x2",
        .Colors        = {color {180, 180, 255, 180}, color {100, 100, 255, 80}, color {40, 40, 180, 0}},
        .Transparency  = std::minmax(0.3f, 0.6f),
        .Lifetime      = std::minmax(60ms, 180ms),
        .Scale         = std::minmax(1.f, 3.f),
        .Size          = {2, 2},
    };
    lightningGlow.Settings.SpawnArea = {col4x, row0y, 4, 4};
    lightningGlow.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 300};

    // -------------------------------------------------------
    // SNOW  @ row1 spawn line  — downward (180)
    // -------------------------------------------------------
    auto& snow {_system0.create_emitter()};
    snow.Settings.Template = {
        .Speed         = std::minmax(20.f, 60.f),
        .Direction     = std::minmax(170_deg, 190_deg),
        .LinearDamping = std::minmax(0.01f, 0.03f),
        .Gravity       = {{-5.f, 8.f}, {5.f, 8.f}},
        .TextureRegion = "2x2",
        .Colors        = {colors::White, color {200, 220, 255, 255}, color {180, 200, 255, 120}},
        .Transparency  = std::minmax(0.1f, 0.4f),
        .Lifetime      = std::minmax(4000ms, 8000ms),
        .Scale         = std::minmax(0.3f, 1.2f),
        .Size          = {2, 2},
    };
    snow.Settings.SpawnArea = {col0x, row1y - 350, col4x - col0x + 100, 2};
    snow.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 150};

    // -------------------------------------------------------
    // WATERFALL  @ col1, row1  — downward (180)
    // -------------------------------------------------------
    auto& waterStream {_system0.create_emitter()};
    waterStream.Settings.Template = {
        .Speed              = std::minmax(80.f, 200.f),
        .Direction          = std::minmax(178_deg, 182_deg),
        .LinearAcceleration = std::minmax(20.f, 40.f),
        .LinearDamping      = std::minmax(0.01f, 0.03f),
        .TextureRegion      = "2x2",
        .Colors             = {color {200, 230, 255, 255}, color {150, 200, 255, 200}, color {100, 180, 255, 0}},
        .Transparency       = std::minmax(0.0f, 0.15f),
        .Lifetime           = std::minmax(800ms, 1400ms),
        .Scale              = std::minmax(0.3f, 0.8f),
        .Size               = {2, 2},
    };
    waterStream.Settings.SpawnArea = {col1x - 10, row1y - 200, 20, 4};
    waterStream.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 600};

    auto& waterMist {_system0.create_emitter()};
    waterMist.Settings.Template = {
        .Speed         = std::minmax(10.f, 50.f),
        .Direction     = std::minmax(80_deg, 280_deg),
        .LinearDamping = std::minmax(0.05f, 0.12f),
        .Gravity       = {{-3.f, -2.f}, {3.f, -2.f}},
        .TextureRegion = "2x2",
        .Colors        = {color {220, 240, 255, 120}, color {200, 230, 255, 60}, color {180, 220, 255, 0}},
        .Transparency  = std::minmax(0.5f, 0.8f),
        .Lifetime      = std::minmax(1000ms, 2500ms),
        .Scale         = std::minmax(1.f, 4.f),
        .Size          = {2, 2},
    };
    waterMist.Settings.SpawnArea = {col1x - 20, row1y + 100, 40, 10};
    waterMist.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 120};

    // -------------------------------------------------------
    // EXPLOSION  @ col2, row1  — omnidirectional burst
    // -------------------------------------------------------
    auto& explode {_system0.create_emitter()};
    explode.Settings.Template = {
        .Speed         = std::minmax(150.f, 500.f),
        .Direction     = std::minmax(0_deg, 360_deg),
        .LinearDamping = std::minmax(0.12f, 0.25f),
        .Gravity       = {{0.f, 20.f}, {0.f, 20.f}},
        .TextureRegion = "2x2",
        .Colors        = {colors::White, colors::Yellow, color {255, 80, 0, 255}, color {80, 0, 0, 0}},
        .Transparency  = std::minmax(0.0f, 0.1f),
        .Lifetime      = std::minmax(400ms, 1200ms),
        .Scale         = std::minmax(0.3f, 1.5f),
        .Size          = {2, 2},
    };
    explode.Settings.SpawnArea = {col2x, row1y, 6, 6};
    explode.Settings.Pattern   = particle_emitter::emit_burst {.Count = 400, .Interval = 2000ms, .Repeats = 100};

    auto& explodeDebris {_system0.create_emitter()};
    explodeDebris.Settings.Template = {
        .Speed         = std::minmax(50.f, 180.f),
        .Direction     = std::minmax(0_deg, 360_deg),
        .LinearDamping = std::minmax(0.06f, 0.14f),
        .Gravity       = {{0.f, 40.f}, {0.f, 40.f}},
        .TextureRegion = "2x2",
        .Colors        = {color {255, 120, 0, 255}, color {180, 60, 0, 255}, color {60, 20, 0, 0}},
        .Transparency  = std::minmax(0.1f, 0.3f),
        .Lifetime      = std::minmax(800ms, 2000ms),
        .Scale         = std::minmax(0.5f, 2.0f),
        .Size          = {2, 2},
    };
    explodeDebris.Settings.SpawnArea = {col2x, row1y, 6, 6};
    explodeDebris.Settings.Pattern   = particle_emitter::emit_burst {.Count = 80, .Interval = 2000ms, .Repeats = 100};

    // -------------------------------------------------------
    // SMOKE TRAIL  @ col3, row1  — upward drift
    // -------------------------------------------------------
    auto& smokeTrail {_system0.create_emitter()};
    smokeTrail.Settings.Template = {
        .Speed         = std::minmax(5.f, 20.f),
        .Direction     = std::minmax(-10_deg, 10_deg),
        .LinearDamping = std::minmax(0.01f, 0.04f),
        .Gravity       = {{5.f, -3.f}, {15.f, -3.f}},
        .TextureRegion = "2x2",
        .Colors        = {color {100, 100, 100, 200}, color {70, 70, 70, 100}, color {40, 40, 40, 0}},
        .Transparency  = std::minmax(0.4f, 0.7f),
        .Lifetime      = std::minmax(2000ms, 5000ms),
        .Scale         = std::minmax(3.f, 10.f),
        .Size          = {2, 2},
        .Spin          = std::minmax(-10_deg, 10_deg),
    };
    smokeTrail.Settings.SpawnArea = {col3x, row1y, 8, 8};
    smokeTrail.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 40};

    // -------------------------------------------------------
    // BUBBLES  @ col4, row1  — upward
    // -------------------------------------------------------
    auto& bubbles {_system0.create_emitter()};
    bubbles.Settings.Template = {
        .Speed         = std::minmax(15.f, 50.f),
        .Direction     = std::minmax(-5_deg, 5_deg),
        .LinearDamping = std::minmax(0.02f, 0.06f),
        .Gravity       = {{-4.f, -5.f}, {4.f, -5.f}},
        .TextureRegion = "2x2",
        .Colors        = {color {180, 230, 255, 200}, color {150, 210, 255, 100}, color {120, 200, 255, 0}},
        .Transparency  = std::minmax(0.2f, 0.5f),
        .Lifetime      = std::minmax(1500ms, 3500ms),
        .Scale         = std::minmax(0.5f, 2.5f),
        .Size          = {2, 2},
    };
    bubbles.Settings.SpawnArea = {col4x - 10, row1y, 20, 4};
    bubbles.Settings.Pattern   = particle_emitter::emit_linear {.Rate = 80};

    // -------------------------------------------------------
    // color map
    // -------------------------------------------------------
    using color_fn = std::function<color(f32)>;
    std::unordered_map<uid, color_fn> colorMap;

    colorMap[core.id()] = [](f32 age) {
        if (age < 0.5f) { return color::Lerp(colors::White, {100, 220, 255, 255}, age / 0.5f); }
        return color::Lerp({100, 220, 255, 255}, {20, 80, 220, 0}, (age - 0.5f) / 0.5f);
    };
    colorMap[dust.id()] = [](f32 age) {
        if (age < 0.15f) { return color::Lerp(colors::White, {100, 220, 255, 255}, age / 0.15f); }
        if (age < 0.5f) { return color::Lerp({100, 220, 255, 255}, {20, 80, 220, 255}, (age - 0.15f) / 0.35f); }
        return color::Lerp({20, 80, 220, 255}, {5, 15, 80, 0}, (age - 0.5f) / 0.5f);
    };
    colorMap[firePillar.id()] = [](f32 age) {
        if (age < 0.2f) { return color::Lerp(colors::White, colors::Yellow, age / 0.2f); }
        if (age < 0.6f) { return color::Lerp(colors::Yellow, colors::OrangeRed, (age - 0.2f) / 0.4f); }
        return color::Lerp(colors::OrangeRed, {80, 0, 0, 0}, (age - 0.6f) / 0.4f);
    };
    colorMap[fireEmbers.id()] = [](f32 age) {
        if (age < 0.7f) { return color::Lerp(colors::Yellow, {255, 80, 0, 255}, age / 0.7f); }
        return color::Lerp({255, 80, 0, 255}, {60, 0, 0, 0}, (age - 0.7f) / 0.3f);
    };
    colorMap[fireSmoke.id()] = [](f32 age) {
        return color::Lerp({70, 70, 70, 160}, {20, 20, 20, 0}, age);
    };
    colorMap[sparks.id()] = [](f32 age) {
        if (age < 0.3f) { return color::Lerp(colors::White, colors::Yellow, age / 0.3f); }
        if (age < 0.7f) { return color::Lerp(colors::Yellow, {255, 120, 0, 255}, (age - 0.3f) / 0.4f); }
        return color::Lerp({255, 120, 0, 255}, {80, 20, 0, 0}, (age - 0.7f) / 0.3f);
    };
    colorMap[portalRing.id()] = [](f32 age) {
        if (age < 0.6f) { return color::Lerp({180, 0, 255, 255}, {80, 0, 200, 255}, age / 0.6f); }
        return color::Lerp({80, 0, 200, 255}, {20, 0, 80, 0}, (age - 0.6f) / 0.4f);
    };
    colorMap[portalCore.id()] = [](f32 age) {
        if (age < 0.4f) { return color::Lerp(colors::White, {220, 180, 255, 255}, age / 0.4f); }
        return color::Lerp({220, 180, 255, 255}, {140, 0, 255, 0}, (age - 0.4f) / 0.6f);
    };
    colorMap[snow.id()] = [](f32 age) {
        if (age < 0.8f) { return color::Lerp(colors::White, {200, 220, 255, 255}, age / 0.8f); }
        return color::Lerp({200, 220, 255, 255}, {180, 200, 255, 0}, (age - 0.8f) / 0.2f);
    };
    colorMap[waterStream.id()] = [](f32 age) {
        if (age < 0.3f) { return color::Lerp({200, 230, 255, 255}, {150, 200, 255, 220}, age / 0.3f); }
        if (age < 0.7f) { return color::Lerp({150, 200, 255, 220}, {120, 180, 255, 120}, (age - 0.3f) / 0.4f); }
        return color::Lerp({120, 180, 255, 120}, {100, 160, 255, 0}, (age - 0.7f) / 0.3f);
    };
    colorMap[waterMist.id()] = [](f32 age) {
        return color::Lerp({220, 240, 255, 100}, {180, 220, 255, 0}, age);
    };
    colorMap[explode.id()] = [](f32 age) {
        if (age < 0.15f) { return color::Lerp(colors::White, colors::Yellow, age / 0.15f); }
        if (age < 0.4f) { return color::Lerp(colors::Yellow, {255, 80, 0, 255}, (age - 0.15f) / 0.25f); }
        return color::Lerp({255, 80, 0, 255}, {80, 0, 0, 0}, (age - 0.4f) / 0.6f);
    };
    colorMap[explodeDebris.id()] = [](f32 age) {
        if (age < 0.5f) { return color::Lerp({255, 120, 0, 255}, {180, 60, 0, 255}, age / 0.5f); }
        return color::Lerp({180, 60, 0, 255}, {60, 20, 0, 0}, (age - 0.5f) / 0.5f);
    };
    colorMap[smokeTrail.id()] = [](f32 age) {
        return color::Lerp({100, 100, 100, 180}, {40, 40, 40, 0}, age);
    };
    colorMap[lightning.id()] = [](f32 age) {
        if (age < 0.4f) { return color::Lerp(colors::White, {200, 200, 255, 255}, age / 0.4f); }
        return color::Lerp({200, 200, 255, 255}, {40, 40, 180, 0}, (age - 0.4f) / 0.6f);
    };
    colorMap[lightningGlow.id()] = [](f32 age) {
        return color::Lerp({180, 180, 255, 160}, {40, 40, 180, 0}, age);
    };
    colorMap[bubbles.id()] = [](f32 age) {
        if (age < 0.7f) { return color::Lerp({180, 230, 255, 180}, {150, 210, 255, 80}, age / 0.7f); }
        return color::Lerp({150, 210, 255, 80}, {120, 200, 255, 0}, (age - 0.7f) / 0.3f);
    };

    _system0.ParticleUpdate.connect([=](particle_event const& ev) {
        auto& p {ev.Particles};
        isize idx {ev.Index};

        auto it {colorMap.find(p.EmitterID[idx])};
        if (it == colorMap.end()) { return; }

        f32 const age {static_cast<f32>(1.0 - (p.RemainingLife[idx].count() / p.StartingLife[idx].count()))};
        p.Color[idx] = it->second(age);
    });

    _system0.start();
}

void ParticleSystemEx::on_draw_to(render_target& target, transform const& xform)
{
    _system0.draw_to(target, xform);
}

void ParticleSystemEx::on_update(milliseconds deltaTime)
{
    _system0.update(_reverse ? -deltaTime : deltaTime);
}

void ParticleSystemEx::on_fixed_update(milliseconds deltaTime)
{
    auto const& stats {locate_service<gfx::render_system>().statistics()};
    auto const& mouse {locate_service<input::system>().mouse().get_position()};
    window().Title = std::format("TestGame | FPS avg:{:.2f} best:{:.2f} worst:{:.2f} | x:{} y:{} | particles:{} ",
                                 stats.average_FPS(), stats.best_FPS(), stats.worst_FPS(),
                                 mouse.X, mouse.Y,
                                 _system0.particle_count());
}

void ParticleSystemEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    case scan_code::D1:
        _system0.restart();
        break;
    case scan_code::D2:
        if (_system0.is_running()) {
            _system0.stop();
        } else {
            _system0.start();
        }
        break;
    case scan_code::R:
        _reverse = !_reverse;
        break;
    default:
        break;
    }

    _cam.on_key_down(ev);
}

void ParticleSystemEx::on_mouse_button_down(mouse::button_event const& ev)
{
    _cam.on_mouse_button_down(ev);
}

void ParticleSystemEx::on_mouse_button_up(mouse::button_event const& ev)
{
    _cam.on_mouse_button_up(ev);
}

void ParticleSystemEx::on_mouse_motion(mouse::motion_event const& ev)
{
    _cam.on_mouse_motion(ev);
}

void ParticleSystemEx::on_mouse_wheel(mouse::wheel_event const& ev)
{
    _cam.on_mouse_wheel(ev);
}

void ParticleSystemEx::load_emitter(particle_emitter& emi)
{
    data::object obj;
    obj["emi"]                    = emi.Settings;
    obj["emi"]["spawn_area"]["x"] = 1200;
    obj["emi"]["lifetime"]        = 3000;
    auto& emi1 {_system0.create_emitter()};
    emi1.Settings = obj["emi"].as<particle_emitter::settings>();
}
