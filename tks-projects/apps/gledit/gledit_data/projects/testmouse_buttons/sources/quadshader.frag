in vec4 mousebuttons;

out vec4 o_color; 

float l = 1.0 - length(g_NormCoord.xy); 
l = smoothstep(0.0, 0.015, l); 
o_color = (vec4(((mousebuttons.xyz*0.25+0.25)+0.5), 1)*(mousebuttons.w*0.25+0.75)) * vec4(l,l,l,l); 
