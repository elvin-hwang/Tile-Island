#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;

in vec2 texcoord;

layout(location = 0) out vec4 color;

vec2 distort(vec2 uv) 
{
	vec2 coord = uv.xy;
	coord = coord + (0.002 * cos(time));
    return coord;
}

vec4 color_shift(vec4 in_color) 
{
	vec4 color = in_color;
	color += vec4(0, 0, 0.3, 1);
	return color;
}

vec4 fade_color(vec4 in_color) 
{
	vec4 color = in_color;
	if (darken_screen_factor > 0)
		color -= darken_screen_factor * vec4(0.8, 0.8, 0.8, 0);

	return color;
}

void main()
{
	vec2 coord = distort(texcoord);

    vec4 in_color = texture(screen_texture, coord);
    color = color_shift(in_color);
    color = fade_color(color);
}