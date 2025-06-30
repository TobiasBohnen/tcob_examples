// Copyright (c) 2025 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "../_common/Common.hpp"

class PathfindingEx : public scene {
    static constexpr point_i INVALID {-1, -1};
    static constexpr size_i  GRID_SIZE {256, 256};

public:
    PathfindingEx(game& game);
    ~PathfindingEx() override;

protected:
    void on_start() override;

    void on_draw_to(render_target& target) override;

    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;

    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_button_down(mouse::button_event const& ev) override;

private:
    orthogonal_tilemap _tileMapOrtho;
    size_f             _tileSize;

    canvas          _canvas;
    canvas_renderer _renderer {_canvas};

    point_i              _start {INVALID};
    point_i              _end {INVALID};
    std::vector<point_i> _path;

    ai::astar_pathfinding _pathfinder;
    grid<tile_index_t>    _tiles {GRID_SIZE, 1};

    struct grid_view {
        auto get_cost(point_i, point_i p) const -> u64
        {
            return std::pow((*Parent)[p], 3);
        }
        grid<tile_index_t>* Parent;
    };
    grid_view _costs {&_tiles};
};
