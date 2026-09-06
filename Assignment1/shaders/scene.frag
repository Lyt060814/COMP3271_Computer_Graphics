#version 330 core

in vec3 world_normal;

uniform vec3 u_color;
uniform bool u_lit;
uniform bool u_selected;

out vec4 fragment_color;

void main() {
    vec3 color = u_color;
    if (u_lit) {
        vec3 light_direction = normalize(vec3(0.6, 1.0, 0.8));
        float diffuse = max(dot(normalize(world_normal), light_direction), 0.0);
        color *= 0.25 + 0.75 * diffuse;
    }
    if (u_selected) {
        color = mix(color, vec3(1.0, 0.72, 0.12), 0.22);
    }
    fragment_color = vec4(color, 1.0);
}

