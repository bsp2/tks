
in vec4 i_position;

out vec4 o_position;

vec4 c = texture2D(u_tex_hmap, vec2(i_position.x*0.5+0.5, 0.0));
c *= 0.7;
o_position = i_position + c;

