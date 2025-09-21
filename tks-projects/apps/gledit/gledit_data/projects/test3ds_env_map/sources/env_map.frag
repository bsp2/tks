in vec4 in_color; 
in vec3 in_normal; 
in vec2 in_uv; 
 
out vec4 out_color; 
 
/* out_color = in_color * 0.5 + 0.5;  */
/* out_color = vec4(1,0,0,1); */ 
/* in_color *= 0.75; */
/* in_color += 0.25;  */
/* in_color = vec4(1,1,1,1), */

out_color = in_color + texture2D(u_tex, in_uv); 

