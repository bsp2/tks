in vec4 in_color;

out vec4 out_color;

//out_color = in_color * 0.5;
/* out_color = vec4(1,0,0,1); */
out_color = texture2D(u_tex, v_uv);
