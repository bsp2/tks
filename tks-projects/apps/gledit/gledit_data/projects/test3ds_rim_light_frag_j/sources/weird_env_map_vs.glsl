
in vec3 lightpos;
in mat4 transform_inv;
out vec4 color;

float dot = dot(lightpos, a_normal);
if(dot < 0)
  dot = 0;
color = vec4(dot, dot, dot, 1);

float fx  = -19.5;
float fy  =   5.0;
float fm  = mod(fx, fy);
if(fm < 0.0)
  color = vec4(1,0,0,1);

vec3 t = normalize(a_position.xyz);
t = (transform_inv * vec4(t,1)).xyz;
v_uv = t.xy * t.z * 0.5;



