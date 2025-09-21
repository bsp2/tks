out vec4 o_color;

float l = 1.0 - length(g_NormCoord.xy);
l = smoothstep(0.0, (sin(u_time*1.5)*0.5+0.5)*0.3+0.015, l);
l = pow(l, 3.0);
o_color = vec4(l,l,l,l);
