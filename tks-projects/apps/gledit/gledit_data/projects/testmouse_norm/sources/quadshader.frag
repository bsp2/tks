in vec2 mouse;

out vec4 o_color; 

vec2 m = mouse;
float l = 1.0 - length(g_NormCoord.xy); 
l = smoothstep(0.0, abs(m.x)*0.5+0.015, l); 
l = pow(l, 1.0 + 20.0 * abs(m.y)); 
o_color = vec4(l,l,l,l); 
