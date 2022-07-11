#version 330 core

in vec2 texture_coords;

uniform sampler2D fb_texture;

out vec4 FragColor;

void main()
{
    FragColor = vec4(texture(fb_texture, texture_coords).rgb, 1.0f);
    //FragColor = vec4(vec2(texture_coords), 1.0f, 1.0f);
}