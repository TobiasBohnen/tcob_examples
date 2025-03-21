// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ImageFilterEx.hpp"

#include "../_common/Image.hpp"

#include <iomanip>
#include <memory>

ImageFilterEx::ImageFilterEx(game& game)
    : scene(game)
{
    _mat0->Texture = _tex0;
    image img0 {image::Create({64, 64}, image::format::RGBA, smile)};
    auto  imgSize {img0.info().Size};
    _tex0->create(imgSize, 7, texture::format::RGBA8);

    _tex0->update_data(img0.data(), 0);
    _tex0->add_region("normal", {{0, 0, 1, 1}, 0});

    {
        edge_detect_filter filter;
        filter.IncludeAlpha = true;
        image filterImg {filter(img0)};
        _tex0->update_data(filterImg.data(), 1);
        _tex0->add_region("edgedetect", {{0, 0, 1, 1}, 1});
    }

    {
        blur_filter filter;
        image       filterImg {filter(img0)};
        _tex0->update_data(filterImg.data(), 2);
        _tex0->add_region("blur", {{0, 0, 1, 1}, 2});
    }

    {
        emboss_filter filter;
        image         filterImg {filter(img0)};
        _tex0->update_data(filterImg.data(), 3);
        _tex0->add_region("emboss", {{0, 0, 1, 1}, 3});
    }

    {
        edge_enhance_filter filter;
        filter.IncludeAlpha = true;
        image filterImg {filter(img0)};
        _tex0->update_data(filterImg.data(), 4);
        _tex0->add_region("edgeenhance", {{0, 0, 1, 1}, 4});
    }

    {
        motion_blur_filter filter;
        image              filterImg {filter(img0)};
        _tex0->update_data(filterImg.data(), 5);
        _tex0->add_region("motionblur", {{0, 0, 1, 1}, 5});
    }

    {
        sharpen_filter filter;
        image          filterImg {filter(img0)};
        _tex0->update_data(filterImg.data(), 6);
        _tex0->add_region("sharpen", {{0, 0, 1, 1}, 6});
    }
}

ImageFilterEx::~ImageFilterEx() = default;

void ImageFilterEx::on_start()
{
    auto sprite0           = _layer1.create_shape<gfx::rect_shape>();
    sprite0->Material      = _mat0;
    sprite0->TextureRegion = "normal";
    sprite0->Bounds        = {{400, 300}, {128, 128}};

    //////////
    auto sprite1           = _layer1.create_shape<gfx::rect_shape>();
    sprite1->Material      = _mat0;
    sprite1->TextureRegion = "blur";
    sprite1->Bounds        = {{0.f, 0.f}, {128, 128}};

    //////////
    auto sprite2           = _layer1.create_shape<gfx::rect_shape>();
    sprite2->Material      = _mat0;
    sprite2->TextureRegion = "edgedetect";
    sprite2->Bounds        = {{0.f, 200.f}, {128, 128}};

    //////////
    auto sprite3           = _layer1.create_shape<gfx::rect_shape>();
    sprite3->Material      = _mat0;
    sprite3->TextureRegion = "emboss";
    sprite3->Bounds        = {{0.f, 400.f}, {128, 128}};

    //////////
    auto sprite4           = _layer1.create_shape<gfx::rect_shape>();
    sprite4->Material      = _mat0;
    sprite4->TextureRegion = "edgeenhance";
    sprite4->Bounds        = {{600.f, 0.f}, {128, 128}};

    //////////
    auto sprite5           = _layer1.create_shape<gfx::rect_shape>();
    sprite5->Material      = _mat0;
    sprite5->TextureRegion = "motionblur";
    sprite5->Bounds        = {{600.f, 200.f}, {128, 128}};

    //////////
    auto sprite6           = _layer1.create_shape<gfx::rect_shape>();
    sprite6->Material      = _mat0;
    sprite6->TextureRegion = "sharpen";
    sprite6->Bounds        = {{600.f, 400.f}, {128, 128}};
}

void ImageFilterEx::on_draw_to(render_target& target)
{
    _layer1.draw_to(target);
}

void ImageFilterEx::on_update(milliseconds deltaTime)
{
    _layer1.update(deltaTime);
}

void ImageFilterEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().stats()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void ImageFilterEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) {
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:
        break;
    }
}

void ImageFilterEx::on_mouse_motion(mouse::motion_event const& ev)
{
}