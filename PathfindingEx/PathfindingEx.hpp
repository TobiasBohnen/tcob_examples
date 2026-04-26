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
    bool            _canvasDirty {true};
    canvas_renderer _renderer {_canvas};

    point_i              _start {INVALID};
    point_i              _end {INVALID};
    std::vector<point_i> _path;

    ai::astar_pathfinding _pathfinder {true, astar_pathfinding::heuristic::Chebyshev, 1};

    grid<tile_index_t> _tiles {GRID_SIZE, 1};

    struct grid_view {
        auto get_cost(point_i from, point_i to) const -> u64
        {
            if ((*Parent)[to] == 256) { return ai::astar_pathfinding::IMPASSABLE_COST; }
            if (from.X != to.X && from.Y != to.Y) {
                if ((*Parent)[{from.X, to.Y}] == 256 || (*Parent)[{to.X, from.Y}] == 256) {
                    return ai::astar_pathfinding::IMPASSABLE_COST;
                }
            }
            u64 wall_penalty {0};
            for (auto const& dir : std::array<point_i, 4> {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}}) {
                point_i const n {to.X + dir.X, to.Y + dir.Y};
                if ((*Parent)[n] == 256) { ++wall_penalty; }
            }
            return 2 + wall_penalty;
        }
        grid<tile_index_t>* Parent;
    };
    grid_view _costs {&_tiles};
};
