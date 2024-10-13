// Copyright (c) 2024 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "RngEx.hpp"

RngEx::RngEx(game& game)
    : scene {game}
{
}

RngEx::~RngEx() = default;

void RngEx::on_start()
{
    u64 const seed = static_cast<u64>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    using namespace tcob::random;
    std::cout << "----xoroshiro_128_plus" << '\n';
    draw_dice(dice<20, xoroshiro_128_plus> {seed});
    std::cout << "----xoroshiro_128_plus_plus" << '\n';
    draw_dice(dice<20, xoroshiro_128_plus_plus> {seed});
    std::cout << "----xoroshiro_128_star_star" << '\n';
    draw_dice(dice<20, xoroshiro_128_star_star> {seed});
    std::cout << "----xoshiro_256_plus" << '\n';
    draw_dice(dice<20, xoshiro_256_plus> {seed});
    std::cout << "----xoshiro_256_plus_plus" << '\n';
    draw_dice(dice<20, xoshiro_256_plus_plus> {seed});
    std::cout << "----xoshiro_256_star_star" << '\n';
    draw_dice(dice<20, xoshiro_256_star_star> {seed});
    std::cout << "----split_mix_64" << '\n';
    draw_dice(dice<20, split_mix_64> {seed});
    std::cout << "----xorshift_64" << '\n';
    draw_dice(dice<20, xorshift_64> {seed});
    std::cout << "----xorshift_64_star" << '\n';
    draw_dice(dice<20, xorshift_64_star> {seed});
    std::cout << "----split_mix_32" << '\n';
    draw_dice(dice<20, split_mix_32> {static_cast<u32>(seed)});
    std::cout << "----game_rand" << '\n';
    draw_dice(dice<20, game_rand> {static_cast<u32>(seed)});
    std::cout << "----well_512_a" << '\n';
    draw_dice(dice<20, well_512_a> {static_cast<u32>(seed)});

    random_number_generator<xorshift_64, normal_distribution> rng0 {static_cast<u32>(seed), 5.f, 3.f};
    std::cout << "----normal " << '\n';
    draw_rng(rng0);

    random_number_generator<xorshift_64, poisson_distribution> rng1 {static_cast<u32>(seed), 15.f};
    std::cout << "----poisson " << '\n';
    draw_rng(rng1);

    random_number_generator<xorshift_64, triangular_distribution> rng2 {static_cast<u32>(seed), 0.f, 30.f, 25.f};
    std::cout << "----triangular " << '\n';
    draw_rng(rng2);

    random_number_generator<xorshift_64, uniform_distribution<f32>> rng3a {static_cast<u32>(seed), 5.f, 7.f};
    std::cout << "----uniform f32 " << '\n';
    draw_rng(rng3a, 10);
    random_number_generator<xorshift_64, uniform_distribution<f64>> rng3b {static_cast<u32>(seed), 5., 7.};
    std::cout << "----uniform f64 " << '\n';
    draw_rng(rng3b, 10);
    random_number_generator<xorshift_64, uniform_distribution<i32>> rng3c {static_cast<u32>(seed), 5, 7};
    std::cout << "----uniform i32 " << '\n';
    draw_rng(rng3c);
    random_number_generator<xorshift_64, uniform_distribution<i64>> rng3d {static_cast<u32>(seed), 5, 7};
    std::cout << "----uniform i64 " << '\n';
    draw_rng(rng3d);

    random_number_generator<xorshift_64, weibull_distribution> rng4 {static_cast<u32>(seed), 1.f, 10.f};
    std::cout << "----weibull " << '\n';
    draw_rng(rng4);

    random_number_generator<xorshift_64, pareto_distribution> rng5 {static_cast<u32>(seed), 2.f, 10.f};
    std::cout << "----pareto " << '\n';
    draw_rng(rng5);

    random_number_generator<xorshift_64, exponential_distribution> rng6 {static_cast<u32>(seed), 0.5f};
    std::cout << "----exp " << '\n';
    draw_rng(rng6);

    random_number_generator<xorshift_64, discrete_distribution> rng7a {static_cast<u32>(seed), std::array {1., 1., 2., 2., 1.}};
    std::cout << "----discrete int " << '\n';
    draw_rng(rng7a);
    random_number_generator<xorshift_64, discrete_distribution> rng7b {static_cast<u32>(seed), std::array {0.2, 0.2, 0.1, 0.1, 0.2}};
    std::cout << "----discrete float " << '\n';
    draw_rng(rng7b);

    random_number_generator<xorshift_64, cauchy_distribution> rng8 {static_cast<u32>(seed), -2.f, 1.f};
    std::cout << "----cauchy " << '\n';
    draw_rng(rng8);

    random_number_generator<xorshift_64, bernoulli_distribution> rng9 {static_cast<u32>(seed), 0.75f};
    std::cout << "----bernoulli" << '\n';
    draw_rng(rng9);

    random_number_generator<xorshift_64, bag_distribution> rng10 {static_cast<u32>(seed), 1, 20, 5};
    std::cout << "----bag " << '\n';
    draw_rng(rng10);

    random_number_generator<xorshift_64, piecewise_constant_distribution> rng11 {static_cast<u32>(seed), std::array {0., 1., 10., 15.}, std::array {1., 0., 1.}};
    std::cout << "----pc" << '\n';
    draw_rng(rng11);

    random_number_generator<xorshift_64, binomial_distribution> rng12 {static_cast<u32>(seed), 4, 0.5};
    std::cout << "----binomial" << '\n';
    draw_rng(rng12);

    random_number_generator<xorshift_64, negative_binomial_distribution> rng13 {static_cast<u32>(seed), 5, 0.75};
    std::cout << "----neg binomial" << '\n';
    draw_rng(rng13);

    random_number_generator<xorshift_64, gamma_distribution> rng14 {static_cast<u32>(seed), 1., 2.};
    std::cout << "----gamma" << '\n';
    draw_rng(rng14);

    random_number_generator<xorshift_64, beta_distribution> rng15 {static_cast<u32>(seed), 2., 2.};
    std::cout << "----beta" << '\n';
    draw_rng(rng15, 10);

    random_number_generator<xorshift_64, lognormal_distribution> rng16 {static_cast<u32>(seed), 1.6, 0.25};
    std::cout << "----log normal" << '\n';
    draw_rng(rng16);
}

void RngEx::on_draw_to(render_target& target)
{
}

void RngEx::on_update(milliseconds deltaTime)
{
}

void RngEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::R: {
        auto _ = get_window().copy_to_image().save("screen01.webp");
    } break;
    case scan_code::BACKSPACE:
        get_game().pop_current_scene();
        break;
    default:
        break;
    }
}
