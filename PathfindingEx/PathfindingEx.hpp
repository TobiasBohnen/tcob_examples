// Copyright (c) 2026 Tobias Bohnen
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
    void on_draw_to(render_target& target, transform const& xform) override;
    void on_fixed_update(milliseconds deltaTime) override;
    void on_key_down(keyboard::event const& ev) override;
    void on_mouse_button_down(mouse::button_event const& ev) override;

private:
    size_f          _tileSize;
    canvas          _canvas;
    canvas_renderer _renderer {_canvas};
    bool            _canvasDirty {false};

    point_i              _start {INVALID};
    point_i              _end {INVALID};
    std::vector<point_i> _path;

    ai::astar_pathfinding _pathfinder {true};

    grid<tile_index_t> _tiles {GRID_SIZE, 1};
    grid<tile_index_t> _clearance {GRID_SIZE, 0};

    struct grid_view {
        auto                get_cost(point_i from, point_i to) const -> u64;
        grid<tile_index_t>* Parent;
        grid<tile_index_t>* Clearance;
    };
    grid_view _costs {&_tiles, &_clearance};
};
