
// $ g++ -o t.exe test_acos.cpp && ./t.exe
// x=-1.000000 => loc_acosf(x)=3.141593   acosf(x)=3.141593
// x=1.000000 => loc_acosf(x)=0.000000   acosf(x)=0.000000
// x=0.500000 => loc_acosf(x)=1.047180   acosf(x)=1.047198
// xxx sqrt(0.0f)=0.000000

#include <stdio.h>
#include <math.h>

#define MATH_PI      (3.1415926535f)
#define MATH_PI_2    (1.5707963267f)   // PI/2

// float loc_asinf(float x) {
//    float sq1mx = sqrtf(1.0f-x);
//    return ( sq1mx * (MATH_PI_2 + x*(-0.213300989f + x*(0.077980478f + x*-0.02164095f))) ) + MATH_PI_2;
// }

float loc_acosf(float x) {
   // // acos(x) =  pi/2 - asin(x)

   int bSign = (x < 0.0f);
   if(bSign)
      x = -x;

   float t = MATH_PI_2 + x*(-0.213300989f + x*(0.077980478f + x*-0.02164095f));
   float sq1mx = sqrtf(1.0f-x);
   printf("xxx sq1mx=%f t=%f\n", sq1mx, t);
   t = sq1mx * t;
   printf("xxx t=%f bSign=%d\n", t, bSign);

   t -= MATH_PI_2;
   printf("xxx tmin=%f\n", t);
   if(t < 0.0f)
   {
      if(bSign)
         t = -t;
   }
   else if(bSign)
      t = -t;
   printf("xxx t=%f\n", t);
   t += MATH_PI_2;
   printf("xxx ret=%f\n", t);

   return t;
}

void t(float x) {
   float ac = loc_acosf(x);
   float acRef = acosf(x);
   printf("x=%f => loc_acosf(x)=%f   acosf(x)=%f\n\n", x, ac, acRef);
}

int main(int argc, char **argv) {
   (void)argc;
   (void)argv;

   t(-1.0f);
   t(1.0f);
   t(0.5f);
   t(0.123f);

   printf("xxx sqrt(0.0f)=%f\n", sqrtf(0.0f));

   return 0;
}
