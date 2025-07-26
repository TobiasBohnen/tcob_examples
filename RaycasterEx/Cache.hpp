// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "../_common/Common.hpp"

////////////////////////////////////////////////////////////

constexpr i32 floorTexture {8};
constexpr i32 ceilingTexture {9};

class cache_base {
public:
    virtual ~cache_base() = default;

    auto virtual screen_size() const -> size_i = 0;
    auto virtual tex_size() const -> size_i    = 0;
    auto virtual tex_bpp() const -> i32 { return 3; };

    auto virtual texture(i32 idx) -> u8* = 0;
    auto virtual screen(i32 idx) -> u32* = 0;

    void load()
    {
        auto const loadTex {[&](i32 tex, string const& path) {
            auto img {image::Load(path).value()};
            assert(img.info().bytes_per_pixel() == 4);

            resize_bilinear filter;
            filter.NewSize = tex_size();
            img            = filter(img);

            isize imgIdx {0};

            for (isize idx {0}; idx < filter.NewSize.Width * filter.NewSize.Height * tex_bpp();) {
                texture(tex)[idx++] = img.ptr()[imgIdx++];
                texture(tex)[idx++] = img.ptr()[imgIdx++];
                texture(tex)[idx++] = img.ptr()[imgIdx++];
                imgIdx++;
            }
        }};
        loadTex(0, "res/wall0.png");
        loadTex(1, "res/wall1.png");
        loadTex(2, "res/wall2.png");
        loadTex(3, "res/wall3.png");
        loadTex(4, "res/wall4.png");
        loadTex(5, "res/wall5.png");
        loadTex(6, "res/wall6.png");
        loadTex(7, "res/wall7.png");

        loadTex(floorTexture, "res/floor.png");
        loadTex(ceilingTexture, "res/ceiling.png");
    }
};

template <size_i S, size_i T>
class cache : public cache_base {
public:
    auto screen_size() const -> size_i override { return S; }
    auto tex_size() const -> size_i override { return T; }

    auto texture(i32 idx) -> u8* override { return _textures[idx].data(); }
    auto screen(i32 idx) -> u32* override { return _screen.data() + idx; }

private:
    std::array<u32, S.Width * S.Height> _screen;

    using tex = std::array<u8, T.Width * T.Height * 3>;
    std::array<tex, 10> _textures {};
};
