// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#pragma once
#include "../_common/Common.hpp"

using namespace tcob::ui;

class pathfinding_form : public form<dock_layout> {
public:
    pathfinding_form(rect_i const& bounds);

    signal<void, pathfinding_form> OnRegenerate;
    signal<void, pathfinding_form> OnSmooth;
    signal<void, pathfinding_form> OnToggleWall;
    signal<void, pathfinding_form> OnDStarMove;
    signal<void, pathfinding_form> OnMapChanged;
    signal<void, pathfinding_form> OnAlgoChanged;

    auto map_mode_name() const -> string;
    auto algo_mode_name() const -> string;
    auto is_terrain_map() const -> bool;

    void cycle_map();
    void cycle_algo();
    void set_time(f64 ms);
    void set_nodes(usize count);

    enum class map_mode : u8 {
        Maze,
        Open,
        Boxes,
        Terrain
    };
    enum class algo_mode : u8 {
        AStar,
        BidirAStar,
        ThetaStar,
        LPA,
        DStarLite,
        MinTurns,
        FlowField
    };

    auto current_map_mode() const -> map_mode { return _mapMode; }
    auto current_algo_mode() const -> algo_mode { return _algoMode; }

private:
    void gen_styles();
    void update_labels();

    map_mode  _mapMode {map_mode::Maze};
    algo_mode _algoMode {algo_mode::ThetaStar};

    label* _lblMap {nullptr};
    label* _lblAlgo {nullptr};
    label* _lblTime {nullptr};
    label* _lblNodes {nullptr};

    asset_owner_ptr<font_family> _font;
};
