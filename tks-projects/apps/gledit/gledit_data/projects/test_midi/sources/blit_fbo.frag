// test

out vec4 o_color;

o_color = texture2D(u_tex1, g_NormCoord.st*0.5+0.5);// + vec4(g_NormCoord.st,0,0);
//o_color = vec4(1,0,0,1);


