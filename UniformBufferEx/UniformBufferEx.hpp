// Copyright (c) 2026 Tobias Bohnen
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#pragma once
#include "../_common/Common.hpp"

namespace {

string_view VERT_SRC = R"(
#version 450 core

layout(location = 0)in vec2 vertInPos;
layout(location = 1)in vec4 vertInColor;
layout(location = 2)in vec3 vertInTexCoords;

layout(std140, binding = 0)uniform Globals
{
	mat4 camera;
	mat4 model;
	uvec2 view_size;
	ivec2 mouse_pos;
	float time;
	bool debug; 
} global;

layout(std140, binding = 1)uniform Material
{
	vec4 color;
	float point_size;
} material;

layout(location = 0)out VS_OUT
{
	vec4 color;
	vec3 tex_coords;
} vs_out;

void main()
{
    vec4 pos = global.camera * global.model * vec4(vertInPos.xy, 0.0, 1.0);
    gl_Position = vec4(((pos.x / global.view_size.x) * 2.0 - 1.0), (1.0 - 2.0 * (pos.y / global.view_size.y)), 0.0, 1.0);
    vs_out.color = vertInColor;
    vs_out.tex_coords = vertInTexCoords;
}
)";

string_view FRAG0_SRC = R"(
#version 450 core

layout(std140, binding = 1) uniform MaterialBlock {
    vec4  color;
    float pointSize;
};

layout(binding = 2) uniform PassData {
    vec4 u_tint;
};

layout(binding = 0)uniform sampler2DArray texture0;

layout(location = 0)in VS_OUT
{
   vec4 color;
   vec3 tex_coords;
} fs_in;

out vec4 frag_color;

void main()
{
    vec4 tex   = texture(texture0, fs_in.tex_coords);
    frag_color = tex * color * fs_in.color * vec4(u_tint.rgb, 1.0);
}
)";

} // namespace

class UniformBufferEx : public scene {
public:
    explicit UniformBufferEx(game& game);
    ~UniformBufferEx() override;

protected:
    void on_start() override;
    void on_draw_to(render_target& target, transform const& xform) override;
    void on_update(milliseconds deltaTime) override;
    void on_fixed_update(milliseconds deltaTime) override;
    void on_key_down(keyboard::event const& ev) override;

private:
    gfx::shape_batch _layer1;

    asset_owner_ptr<texture>        _tex0 {};
    asset_owner_ptr<material>       _mat0 {material::Empty()};
    asset_owner_ptr<shader>         _shader0 {"", VERT_SRC, FRAG0_SRC};
    asset_owner_ptr<uniform_buffer> _ubo0 {"", sizeof(f32) * 4};

    milliseconds _elapsed {0};
};
