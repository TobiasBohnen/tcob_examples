// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "RaycasterEx.hpp"

#include <algorithm>

using gfx_cache = cache<{800, 600}, {256, 256}>;

RaycasterEx::RaycasterEx(game& game)
    : scene {game}
    , _cache {std::make_unique<gfx_cache>()}
{
    _material->Texture = _texture;
    _renderer.set_material(_material.ptr());

    _texture->create(_cache->screen_size(), 1, texture::format::RGBA8);
    _texture->Filtering = texture::filtering::Linear;
}

RaycasterEx::~RaycasterEx() = default;

constexpr i32 mapWidth {24};
constexpr i32 mapHeight {24};

static_grid<u8, mapWidth, mapHeight> worldMap {
    {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 7},
    {4, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
    {4, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
    {4, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 7},
    {4, 0, 4, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 0, 7, 7, 7, 7, 7},
    {4, 0, 5, 0, 0, 0, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 7, 0, 0, 0, 7, 7, 7, 1},
    {4, 0, 6, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 0, 0, 0, 8},
    {4, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 1},
    {4, 0, 8, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 0, 0, 0, 8},
    {4, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 0, 0, 7, 7, 7, 1},
    {4, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 1},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
    {6, 6, 6, 6, 6, 6, 0, 6, 6, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 6, 0, 6, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 2, 0, 0, 5, 0, 0, 2, 0, 0, 0, 2},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2},
    {4, 0, 6, 0, 6, 0, 0, 0, 0, 4, 6, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 2},
    {4, 0, 0, 5, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2},
    {4, 0, 6, 0, 6, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 5, 0, 0, 2, 0, 0, 0, 2},
    {4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 0, 6, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2},
    {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3}};

void RaycasterEx::on_start()
{
    auto& resMgr {library()};
    auto* resGrp {resMgr.get_group("res")};

    _cache->load();
}

void RaycasterEx::on_draw_to(render_target& target)
{
    size_i const size {*target.Size};

    quad q {};
    geometry::set_color(q, colors::White);
    geometry::set_position(q, {0, 0, static_cast<f32>(size.Width), static_cast<f32>(size.Height)});
    geometry::set_texcoords(q, {.UVRect = render_texture::GetTexcoords(), .Level = 0});
    _renderer.set_geometry(q);

    _renderer.render_to_target(target);
}

void RaycasterEx::on_fixed_update(milliseconds deltaTime)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);
    auto const& stats {locate_service<gfx::render_system>().stats()};
    stream << "avg FPS:" << stats.average_FPS();
    stream << " best FPS:" << stats.best_FPS();
    stream << " worst FPS:" << stats.worst_FPS();

    window().Title = "TestGame " + stream.str();
}

void RaycasterEx::on_update(milliseconds deltaTime)
{
    _update = move(deltaTime) || _update;

    if (_update) {
        draw();
        _update = false;
        _texture->update_data(_cache->screen(0), 0);
    }
}

void RaycasterEx::draw()
{
    auto const [w, h] {_texture->info().Size};
    locate_service<task_manager>().run_parallel(
        [&](par_task const& ctx) {
            for (isize x {ctx.Start}; x < ctx.End; x++) {
                cast(static_cast<i32>(x), w, h);
            }
        },
        w);
}

void RaycasterEx::cast(i32 x, i32 w, i32 h)
{
    auto const [texWidth, texHeight] {_cache->tex_size()};
    auto const texBpp {_cache->tex_bpp()};

    // WALL CASTING
    // calculate ray position and direction
    f64 const     cameraX {(2.0 * x / w) - 1.0}; // x-coordinate in camera space
    point_d const rayDir {_dir + (_plane * cameraX)};

    // which box of the map we're in
    point_i map {static_cast<point_i>(_pos)};

    // length of ray from one x or y-side to next x or y-side
    point_d const deltaDist {(rayDir.X == 0) ? 1e30 : std::abs(1 / rayDir.X), (rayDir.Y == 0) ? 1e30 : std::abs(1 / rayDir.Y)};

    // what direction to step in x or y-direction (either +1 or -1)
    point_i step {};

    bool side {false}; // was a NS or a EW wall hit?

    // length of ray from current position to next x or y-side
    point_d sideDist {};

    // calculate step and initial sideDist
    if (rayDir.X < 0) {
        step.X     = -1;
        sideDist.X = (_pos.X - map.X) * deltaDist.X;
    } else {
        step.X     = 1;
        sideDist.X = (map.X + 1.0 - _pos.X) * deltaDist.X;
    }
    if (rayDir.Y < 0) {
        step.Y     = -1;
        sideDist.Y = (_pos.Y - map.Y) * deltaDist.Y;
    } else {
        step.Y     = 1;
        sideDist.Y = (map.Y + 1.0 - _pos.Y) * deltaDist.Y;
    }

    // perform DDA
    for (;;) {
        // jump to next map square, either in x-direction, or in y-direction
        if (sideDist.X < sideDist.Y) {
            sideDist.X += deltaDist.X;
            map.X += step.X;
            side = false;
        } else {
            sideDist.Y += deltaDist.Y;
            map.Y += step.Y;
            side = true;
        }
        // Check if ray has hit a wall
        if (worldMap[map] > 0) {
            break;
        }
    }

    // Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
    f64 const perpWallDist {!side ? sideDist.X - deltaDist.X : sideDist.Y - deltaDist.Y};

    // Calculate height of line to draw on screen
    i32 const lineHeight {static_cast<i32>(h / perpWallDist)};

    // calculate lowest and highest pixel to fill in current stripe
    i32 const drawStart {std::max((-lineHeight / 2) + (h / 2), 0)};
    i32       drawEnd {std::min((lineHeight / 2) + (h / 2), h - 1)};

    // calculate value of wallX
    f64 wallX {!side ? _pos.Y + (perpWallDist * rayDir.Y) : _pos.X + (perpWallDist * rayDir.X)}; // where exactly the wall was hit
    wallX -= std::floor(wallX);

    {                                                                                            // texturing calculations
        i32 const   texNum {worldMap[map] - 1};                                                  // 1 subtracted from it so that texture 0 can be used!
        auto const* tex {_cache->texture(texNum)};

        // x coordinate on the texture
        i32 texX {static_cast<i32>(wallX * static_cast<f64>(texWidth))};
        if ((!side && rayDir.X > 0) || (side && rayDir.Y < 0)) {
            texX = texWidth - texX - 1;
        }

        // How much to increase the texture coordinate per screen pixel
        f64 const texStep {1.0 * texHeight / lineHeight};
        // Starting texture coordinate
        f64       texPos {(drawStart - h / 2 + lineHeight / 2) * texStep};
        for (i32 y {drawStart}; y < drawEnd; y++) {
            // Cast the texture coordinate to integer, and mask with (cache::TexSize.Height - 1) in case of overflow
            i32 const texY {static_cast<i32>(texPos) & (texHeight - 1)};
            texPos += texStep;
            _cache->copy(x + ((h - y - 1) * w), tex, (texX + (texY * texWidth)) * texBpp);
        }
    }

    // FLOOR CASTING (vertical version, directly after drawing the vertical wall stripe for the current x)
    point_d floorWall {}; // x, y position of the floor texel at the bottom of the wall

    // 4 different wall directions possible
    if (!side && rayDir.X > 0) {
        floorWall.X = map.X;
        floorWall.Y = map.Y + wallX;
    } else if (!side && rayDir.X < 0) {
        floorWall.X = map.X + 1.0;
        floorWall.Y = map.Y + wallX;
    } else if (side && rayDir.Y > 0) {
        floorWall.X = map.X + wallX;
        floorWall.Y = map.Y;
    } else {
        floorWall.X = map.X + wallX;
        floorWall.Y = map.Y + 1.0;
    }

    // draw the floor from drawEnd to the bottom of the screen
    for (i32 y {drawEnd}; y < h; y++) {
        f64 const currentDist {h / (2.0 * y - h)};
        f64 const weight {std::min(currentDist / perpWallDist, 1.0)};

        point_d const currentFloor {(weight * floorWall.X) + ((1.0 - weight) * _pos.X), (weight * floorWall.Y) + ((1.0 - weight) * _pos.Y)};

        assert(h - y - 1 >= 0);
        // floor
        auto const* floorTex {_cache->texture(floorTexture)};
        i32 const   floorTexX {static_cast<i32>(currentFloor.X * texWidth) % texWidth};
        i32 const   floorTexY {static_cast<i32>(currentFloor.Y * texHeight) % texHeight};
        _cache->copy(x + ((h - y - 1) * w), floorTex, (floorTexX + (floorTexY * texWidth)) * texBpp);

        // ceil
        auto const* ceilTex {_cache->texture(ceilingTexture)};
        i32 const   ceilTexX {static_cast<i32>(currentFloor.X * texWidth) % texWidth};
        i32 const   ceilTexY {static_cast<i32>(currentFloor.Y * texHeight) % texHeight};
        _cache->copy(x + (y * w), ceilTex, (ceilTexX + (ceilTexY * texWidth)) * texBpp);
    }
}

auto RaycasterEx::move(milliseconds deltaTime) -> bool
{
    bool retValue {false};

    f64 const moveSpeed {deltaTime.count() / 1000 * 4.0}; // the constant value is in squares/second
    f64 const rotSpeed {deltaTime.count() / 1000 * 3.0};  // the constant value is in radians/second

    auto const is_position_clear {[&](point_d pos) -> bool {
        i32 const tileX {static_cast<i32>(pos.X)};
        i32 const tileY {static_cast<i32>(pos.Y)};

        if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapHeight) {
            return false;
        }

        return (worldMap[tileX, tileY] == 0);
    }};

    auto const keyboard {locate_service<input::system>().keyboard()};
    f64 const  margin {0.4}; // extra margin for checking

    // Forward/Backward
    i32 forwardDir {0};
    if (keyboard.is_key_down(scan_code::W) || keyboard.is_key_down(scan_code::UP)) { forwardDir += 1; }
    if (keyboard.is_key_down(scan_code::S) || keyboard.is_key_down(scan_code::DOWN)) { forwardDir -= 1; }

    // Move Forward/Backward
    if (forwardDir != 0) {
        f64 const     moveAmount {forwardDir * moveSpeed};
        point_d const newPos {_pos + (_dir * moveAmount)};
        point_d const checkPos {_pos + (_dir * (moveSpeed + margin) * forwardDir)};

        if (is_position_clear(checkPos)) {
            _pos     = newPos;
            retValue = true;
        } else {
            if (is_position_clear({_pos.X, checkPos.Y})) {
                _pos.Y   = newPos.Y;
                retValue = true;
            } else if (is_position_clear({checkPos.X, _pos.Y})) {
                _pos.X   = newPos.X;
                retValue = true;
            }
        }
    }

    // Strafe Left/Right
    i32 strafeDir {0};
    if (keyboard.is_key_down(scan_code::D)) { strafeDir -= 1; }
    if (keyboard.is_key_down(scan_code::A)) { strafeDir += 1; }

    // Strafe Left/Right (perpendicular to _dir)
    if (strafeDir != 0) {
        // Perpendicular vector to (_dir.X, _dir.Y) is (-_dir.Y, _dir.X)
        point_d const strafe {_dir.as_perpendicular()};

        f64 const     moveAmount {strafeDir * moveSpeed};
        point_d const newPos {_pos + (strafe * moveAmount)};
        point_d const checkPos {_pos + (strafe * (moveSpeed + margin) * strafeDir)};

        if (is_position_clear(checkPos)) {
            _pos     = newPos;
            retValue = true;
        } else {
            if (is_position_clear({_pos.X, checkPos.Y})) {
                _pos.Y   = newPos.Y;
                retValue = true;
            } else if (is_position_clear({checkPos.X, _pos.Y})) {
                _pos.X   = newPos.X;
                retValue = true;
            }
        }
    }

    i32 rotateDir {0};
    if (keyboard.is_key_down(scan_code::LEFT)) { rotateDir += 1; }
    if (keyboard.is_key_down(scan_code::RIGHT)) { rotateDir -= 1; }

    // rotate
    if (rotateDir != 0) {
        f64 const rotateAmount {rotateDir * rotSpeed};

        // both camera direction and camera plane must be rotated
        f64 const old_dirX {_dir.X};
        _dir.X = _dir.X * std::cos(rotateAmount) - _dir.Y * std::sin(rotateAmount);
        _dir.Y = old_dirX * std::sin(rotateAmount) + _dir.Y * std::cos(rotateAmount);
        f64 const oldPlaneX {_plane.X};
        _plane.X = _plane.X * std::cos(rotateAmount) - _plane.Y * std::sin(rotateAmount);
        _plane.Y = oldPlaneX * std::sin(rotateAmount) + _plane.Y * std::cos(rotateAmount);
        retValue = true;
    }

    return retValue;
}

void RaycasterEx::on_key_down(keyboard::event const& ev)
{
    switch (ev.ScanCode) { // NOLINT
    case scan_code::R: {
        auto _ = window().copy_to_image().save("screen01.webp");
    } break;
    case scan_code::BACKSPACE:
        parent().pop_current_scene();
        break;
    default:

        break;
    }
}
