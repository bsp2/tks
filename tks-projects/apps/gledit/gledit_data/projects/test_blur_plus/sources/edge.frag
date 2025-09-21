param float threshold_min = 0.1 min=0 max=1
param float threshold_max = 0.8 min=0 max=1
param float scale = 0.25 min=0 max=1
param bool enable = true
param bool scharr = true
param bool show_lum = true
param bool show_lum_both = true

out vec4 o_color;

[global]
float lum_from_rgb(vec3 rgb) {
  /* return dot(rgb, vec3(0.333, 0.333, 0.333)); */
   return rgb.y;
}

[main]
vec2 uv = g_NormCoord*0.5+0.5;
vec2 texSz = vec2(textureSize(u_tex, 0/*lod*/));
vec2 uvStep = vec2(1,1) / texSz;
//uv += uvStep*0.0;
vec2 uvPix = uv * texSz;
vec2 uvFrac = fract(uvPix);

// (todo) separate

vec4 cIn[9];

cIn[0] = texture2D(u_tex, uv+vec2(-uvStep.x, uvStep.y));
cIn[1] = texture2D(u_tex, uv+vec2(0,         uvStep.y));
cIn[2] = texture2D(u_tex, uv+vec2(uvStep.x,  uvStep.y));

cIn[3] = texture2D(u_tex, uv+vec2(-uvStep.x, 0));
cIn[4] = texture2D(u_tex, uv+vec2(0,         0));
cIn[5] = texture2D(u_tex, uv+vec2(uvStep.x,  0));

cIn[6] = texture2D(u_tex, uv+vec2(-uvStep.x, -uvStep.y));
cIn[7] = texture2D(u_tex, uv+vec2(0,         -uvStep.y));
cIn[8] = texture2D(u_tex, uv+vec2(uvStep.x,  -uvStep.y));

vec4 cblur = 
   (cIn[0] + cIn[1] + cIn[2] +
    cIn[3] + cIn[4] + cIn[5] +
    cIn[6] + cIn[7] + cIn[8]
    ) * (1.0 / 9.0);

mat3 lin;
lin[0][0] = lum_from_rgb(cIn[0].rgb);
lin[1][0] = lum_from_rgb(cIn[1].rgb);
lin[2][0] = lum_from_rgb(cIn[2].rgb);

lin[0][1] = lum_from_rgb(cIn[3].rgb);
lin[1][1] = lum_from_rgb(cIn[4].rgb);
lin[2][1] = lum_from_rgb(cIn[5].rgb);

lin[0][2] = lum_from_rgb(cIn[6].rgb);
lin[1][2] = lum_from_rgb(cIn[7].rgb);
lin[2][2] = lum_from_rgb(cIn[8].rgb);

lin *= 0.005;

mat3 mgx;
mat3 mgy;

if($(SCHARR))
{
   mgx = matrixCompMult(mat3(vec3(47, 162, 47), vec3(0,0,0), vec3(-47,-162,-47)), lin);
   mgy = matrixCompMult(mat3(vec3(47, 0, -47), vec3(162,0,-162), vec3(47,0,-47)), lin);
   /* /\* mat3 mgy = matrixCompMult(mat3(vec3(47, 162, 47), vec3(0,0,0), vec3(-47,-162,-47)), lin); *\/ */
}
else
{
   /* mat3 mgx = matrixCompMult(mat3(vec3(-1, -2, 1), vec3(0,0,0), vec3(1,2,-1)), lin); */
   /* mat3 mgy = matrixCompMult(mat3(vec3(1, 0, -1), vec3(2,0,-2), vec3(1,0,-1)), lin); */

   mgx = matrixCompMult(mat3(vec3(-1, -2, 1), vec3(0,0,0), vec3(1,2,-1)), lin);
   mgy = matrixCompMult(mat3(vec3(1, 0, -1), vec3(2,0,-2), vec3(1,0,-1)), lin);
}

mat3 m;
vec4 c = vec4(0);
vec4 cx = vec4(0);
vec4 cy = vec4(0);
float lum;
float lumx;
float lumy;

// m = mgx * mgy;

if($(SHOW_LUM_BOTH))
{
   m = mgx * mgy;
   lum =
      m[0][0] + m[1][0] + m[2][0] +
      m[0][1] + m[1][1] + m[2][1] +
      m[0][2] + m[1][2] + m[2][2] ;

   if(!$(SCHARR))
      lum *= 2000.0;
   lum  = (lum * 1.0) + 0.075;
}


m = mgx;
lumx =
  m[0][0] + m[1][0] + m[2][0] +
  m[0][1] + m[1][1] + m[2][1] +
  m[0][2] + m[1][2] + m[2][2] ;

if(!$(SCHARR))
  lumx *= 2000.0;

/* lum = (lum * 1.0) + 0.5; */
lumx  = (lumx * 1.0) + 0.075;
//lum = smoothstep(0.0, 1.0, lum);



if(lumx < 0.0)
{
   cx = cIn[4] + (cIn[1] - cIn[4]) * -lumx;
   cx = vec4(-lumx,0,0,1);
}
else if(lumx > 0.0)
{
   cx = cIn[4] + (cIn[7] - cIn[4]) * lumx;
   cx = vec4(0,lumx*1.0,0,1);
   // cx = vec4(0,0,1,1);
}

m = mgy;
lumy =
  m[0][0] + m[1][0] + m[2][0] +
  m[0][1] + m[1][1] + m[2][1] +
  m[0][2] + m[1][2] + m[2][2] ;

if(!$(SCHARR))
  lumy *= 2000.0;

/* lum = (lum * 1.0) + 0.5; */
 lumy  = (lumy * 1.0) + 0.075;

if(lumy < 0.0)
{
   cy = vec4(-lumy,0,0,1);
   cy = cIn[4] + (cIn[3] - cIn[4]) * -lumy;
}
else if(lumy > 0.0)
{
    // cy = vec4(0,lum*1.0,0,1);
    cy = cIn[4] + (cIn[5] - cIn[4]) * lumy;
}

// lum = 1.0 - lum;
// c = cIn[4] + (cblur - cIn[4]) * lum;


c = cx;

//c = vec4(1,1,1,1);


/* lum3 = smoothstep($(THRESHOLD_MIN), $(THRESHOLD_MAX), lum3); */

if($(ENABLE))
{
   if($(SHOW_LUM))
   {
      if($(SHOW_LUM_BOTH))
      {
         o_color = vec4(lum,lum,lum,1);
      }
      else
      {
         // lumx = 0.5*lumx + 0.5;
         // lumy = 0.5*lumy + 0.5;
         o_color = vec4(lumx,lumy,0,1);
      }
   }
   else
      o_color = vec4(c);
}
else
{
   o_color = cIn[4];
}

