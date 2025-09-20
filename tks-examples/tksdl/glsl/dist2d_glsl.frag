#version 120
////#extension GL_EXT_gpu_shader4 : enable
////// (note) need gpu_shader4 because of integer bitwise &. also see <http://developer.download.nvidia.com/opengl/specs/GL_EXT_gpu_shader4.txt>

const int NUM_ATTRACTORS = 8;

uniform float viewport_sx;
uniform float viewport_sy;
uniform float attractors[2 * NUM_ATTRACTORS];

// out vec4 fragColor;

float mathSmoothStepf(float _a, float _b, float _s) {
   // this one works different from the GLSL smoothstep() function since it does not clip
   float t = _s * _s * (3.0 - 2.0 * _s);
   return (_a * (1.0 - t)) + (_b * t);
}

void main()
{
   float cx = (1.0 - gl_FragCoord.x/viewport_sx);
   float cy = (1.0 - gl_FragCoord.y/viewport_sy);

   int i = 0;
   float daccum = 0.0;
   while(i < (2*NUM_ATTRACTORS))
   {
      float dx = (cx - attractors[i]);
      float dy = (cy - attractors[i+1]);
      float d = 1.41 * sqrt(dx*dx + dy*dy);
      d = mathSmoothStepf(0.0, 1.0, d);
      daccum = (d+daccum);
      i = i + 2;
   }
   // int di = (int(daccum * 155.0) & 31) < 15 ? 255 : 0;
   float dif = mod(daccum * 155.0, 31.0) < 15.0 ? 255.0 : 0.0;
   float df = dif * (1.0 / 255.0);

   gl_FragColor = vec4(df, df, df, 1.0);
}
