/*
 * Benchmark to find out what algorithm for matrix4x4 is faster
 *
 * this is a supplemental tool for the tkmath package, see http://tkscript.de
 *
 * will output benchmark info on stderr and
 * suggested flag on stdout
 *
 * (w) 2008 by Carsten Busse
 *
 */

#include <cstdio>
#include <cstdlib>
#ifdef YAC_WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
#endif // YAC_WIN32
#include <cmath>


#include "tkmath_yac.h"
#include <yac_host.cpp>
YAC_Host *yac_host;

#define LOOP 1000000

typedef union {
   sF32 f;
   sSI i;
} anti_pun32;
#undef Dfltequal
#undef Dfltnotequal

sBool Dfltequal(sF32 a, sF32 b) {
   anti_pun32 _a,_b;
   _a.f=a;
   _b.f=b;
   if (_a.i<0) _a.i=0x80000000-_a.i;
   if (_b.i<0) _b.i=0x80000000-_b.i;
   if (abs(_a.i-_b.i)<=yac_epsilon_flt_units) return true;
   else return false;
}

sBool Dfltnotequal(sF32 a, sF32 b) {
   anti_pun32 _a, _b;
   _a.f=a;
   _b.f=b;
   if (_a.i<0) _a.i=0x80000000-_a.i;
   if (_b.i<0) _b.i=0x80000000-_b.i;
   if (abs(_a.i-_b.i)>yac_epsilon_flt_units) return true;
   else return false;
}

class testmatrix {
   public:
   testmatrix& operator=(testmatrix& v) {
      floats[0]=v.floats[0];
      floats[1]=v.floats[1];
      floats[2]=v.floats[2];
      floats[3]=v.floats[3];
      floats[4]=v.floats[4];
      floats[5]=v.floats[5];
      floats[6]=v.floats[6];
      floats[7]=v.floats[7];
      floats[8]=v.floats[8];
      floats[9]=v.floats[9];
      floats[10]=v.floats[10];
      floats[11]=v.floats[11];
      floats[12]=v.floats[12];
      floats[13]=v.floats[13];
      floats[14]=v.floats[14];
      floats[15]=v.floats[15];
      return *this;
   }
   sF32 det(void);
   void transpose(void);
   void invert(void);
   void invert2(void);
   float floats[16];
};

sF32 testmatrix::det(void) {
#define PDET(a,b,c,d) (floats[0*4+a]*floats[1*4+b]*floats[2*4+c]*floats[3*4+d])
   return (PDET(0,1,2,3)-PDET(0,1,3,2)-PDET(0,2,1,3)+PDET(0,2,3,1)+PDET(0,3,1,2)-PDET(0,3,2,1)
          -PDET(1,0,2,3)+PDET(1,0,3,2)+PDET(1,2,0,3)-PDET(1,2,3,0)-PDET(1,3,0,2)+PDET(1,3,2,0)
          +PDET(2,0,1,3)-PDET(2,0,3,1)-PDET(2,1,0,3)+PDET(2,1,3,0)+PDET(2,3,0,1)-PDET(2,3,1,0)
          -PDET(3,0,1,2)+PDET(3,0,2,1)+PDET(3,1,0,2)-PDET(3,1,2,0)-PDET(3,2,0,1)+PDET(3,2,1,0));
}

void testmatrix::transpose(void) {
   sF32 a,b,c,d,e,f;
   a=floats[1];
   b=floats[2];
   c=floats[3];
   d=floats[6];
   e=floats[7];
   f=floats[11];
   floats[1]=floats[4];
   floats[2]=floats[8];
   floats[3]=floats[12];
   floats[6]=floats[9];
   floats[7]=floats[13];
   floats[11]=floats[14];
   floats[4]=a;
   floats[8]=b;
   floats[12]=c;
   floats[9]=d;
   floats[13]=e;
   floats[14]=f;
}

void testmatrix::invert(void) {
#define DET3(_a,_b,_c,_d,_e,_f,_g,_h,_i) ((_a*_e*_i)+(_b*_f*_g)+(_c*_d*_h)-(_c*_e*_g)-(_a*_f*_h)-(_b*_d*_i))
#define A floats[0]
#define B floats[1]
#define C floats[2]
#define D floats[3]
#define E floats[4]
#define F floats[5]
#define G floats[6]
#define H floats[7]
#define I floats[8]
#define J floats[9]
#define K floats[10]
#define L floats[11]
#define M floats[12]
#define N floats[13]
#define O floats[14]
#define P floats[15]
   sF32 detval=det();
   if (Dfltnotequal(detval,0.0f)) {
      sF32 a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;

      a=DET3(F,G,H,J,K,L,N,O,P)/detval;
      b=-DET3(B,C,D,J,K,L,N,O,P)/detval;
      c=DET3(B,C,D,F,G,H,N,O,P)/detval;
      d=-DET3(B,C,D,F,G,H,J,K,L)/detval;

      e=-DET3(E,G,H,I,K,L,M,O,P)/detval;
      f=DET3(A,C,D,I,K,L,M,O,P)/detval;
      g=-DET3(A,C,D,E,G,H,M,O,P)/detval;
      h=DET3(A,C,D,E,G,H,I,K,L)/detval;

      i=DET3(E,F,H,I,J,L,M,N,P)/detval;
      j=-DET3(A,B,D,I,J,L,M,N,P)/detval;
      k=DET3(A,B,D,E,F,H,M,N,P)/detval;
      l=-DET3(A,B,D,E,F,H,I,J,L)/detval;

      m=-DET3(E,F,G,I,J,K,M,N,O)/detval;
      n=DET3(A,B,C,I,J,K,M,N,O)/detval;
      o=-DET3(A,B,C,E,F,G,M,N,O)/detval;
      p=DET3(A,B,C,E,F,G,I,J,K)/detval;

      floats[0]=a;
      floats[1]=b;
      floats[2]=c;
      floats[3]=d;
      floats[4]=e;
      floats[5]=f;
      floats[6]=g;
      floats[7]=h;
      floats[8]=i;
      floats[9]=j;
      floats[10]=k;
      floats[11]=l;
      floats[12]=m;
      floats[13]=n;
      floats[14]=o;
      floats[15]=p;

   } else std::printf("Oops, got a matrix with a 0 determinant\n");
}

void testmatrix::invert2(void) {
   sF32 m0,m1,m2,m3,s;
   sF32 rd0[8],rd1[8],rd2[8],rd3[8],*r0=rd0, *r1=rd1, *r2=rd2, *r3=rd3, *tmp;

  r0[0] = floats[0];
  r0[1] = floats[0*4+1];
  r0[2] = floats[0*4+2];
  r0[3] = floats[0*4+3];
  r0[4] = 1.0;
  r0[5] = 0;
  r0[6] = 0;
  r0[7] = 0.0;

  r1[0] = floats[(1)*4+(0)];
  r1[1] = floats[(1)*4+(1)];
  r1[2] = floats[(1)*4+(2)];
  r1[3] = floats[(1)*4+(3)];
  r1[5] = 1.0;
  r1[4] = 0;
  r1[6] = 0;
  r1[7] = 0.0;

  r2[0] = floats[(2)*4+(0)];
  r2[1] = floats[(2)*4+(1)];
  r2[2] = floats[(2)*4+(2)];
  r2[3] = floats[(2)*4+(3)];
  r2[6] = 1.0;
  r2[4] = 0;
  r2[5] = 0;
  r2[7] = 0.0;

  r3[0] = floats[(3)*4+(0)];
  r3[1] = floats[(3)*4+(1)];
  r3[2] = floats[(3)*4+(2)];
  r3[3] = floats[(3)*4+(3)];
  r3[7] = 1.0;
  r3[4] = 0;
  r3[5] = 0;
  r3[6] = 0.0;

  /* Choose myPivot, or die. */
  if (fabsf(r3[0])>fabsf(r2[0])) {tmp=r3;r3=r2;r2=tmp;};
  if (fabsf(r2[0])>fabsf(r1[0])) {tmp=r2;r2=r1;r1=tmp;};
  if (fabsf(r1[0])>fabsf(r0[0])) {tmp=r1;r1=r0;r0=tmp;};
  if (0.0 == r0[0]) {
     std::printf("Oops, got a matrix with a 0 determinant\n");
    //assert(!"could not invert matrix");
    //  Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::invert2 Invalid Matrix (det==0)");

  }
  /* Eliminate first variable. */
  m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
  s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
  s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
  s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
  s = r0[4];
  if (s != 0.0f) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r0[5];
  if (s != 0.0f) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r0[6];
  if (s != 0.0f) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r0[7];
  if (s != 0.0f) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* Choose myPivot, or die. */
  if (fabsf(r3[1])>fabsf(r2[1])) {tmp=r3;r3=r2;r2=tmp;};
  if (fabsf(r2[1])>fabsf(r1[1])) {tmp=r2;r2=r1;r1=tmp;};
  if (0.0f == r1[1]) {
     std::printf("Oops, got a matrix with a 0 determinant\n");
    //assert(!"could not invert matrix");
    //  Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::invert2 Invalid Matrix (det==0)");
  }
  m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
  r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
  r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
  s = r1[4]; if (0.0f != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r1[5]; if (0.0f != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r1[6]; if (0.0f != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r1[7]; if (0.0f != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* Choose myPivot, or die. */
  if (fabsf(r3[2])>fabsf(r2[2])) {tmp=r3;r3=r2;r2=tmp;};
  if (0.0f == r2[2]) {
     std::printf("Oops, got a matrix with a 0 determinant\n");
   // assert(!"could not invert matrix");
   //   Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::invert2 Invalid Matrix (det==0)");
  }
  /* Eliminate third variable. */
  m3 = r3[2]/r2[2];
  r3[3] -= m3 * r2[3];
  r3[4] -= m3 * r2[4];
  r3[5] -= m3 * r2[5];
  r3[6] -= m3 * r2[6];
  r3[7] -= m3 * r2[7];
  /* Last check. */
  if (0.0f == r3[3]) {
    std::printf("Oops, got a matrix with a 0 determinant\n");
    //assert(!"could not invert matrix");
    //  Dyac_throw_def(TkMathDivisionByZero, "tkmath::Matrix4f::invert2 Invalid Matrix (det==0)");
  }
  s = 1.0f/r3[3];              /* Now back substitute row 3. */
  r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

  m2 = r2[3];                 /* Now back substitute row 2. */
  s  = 1.0f/r2[2];
  r2[4] = s * (r2[4] - r3[4] * m2);
  r2[5] = s * (r2[5] - r3[5] * m2);
  r2[6] = s * (r2[6] - r3[6] * m2);
  r2[7] = s * (r2[7] - r3[7] * m2);
  m1 = r1[3];
  r1[4] -= r3[4] * m1;
  r1[5] -= r3[5] * m1;
  r1[6] -= r3[6] * m1;
  r1[7] -= r3[7] * m1;
  m0 = r0[3];
  r0[4] -= r3[4] * m0;
  r0[5] -= r3[5] * m0;
  r0[6] -= r3[6] * m0;
  r0[7] -= r3[7] * m0;

  m1 = r1[2];                 /* Now back substitute row 1. */
  s  = 1.0f/r1[1];
  r1[4] = s * (r1[4] - r2[4] * m1);
  r1[5] = s * (r1[5] - r2[5] * m1);
  r1[6] = s * (r1[6] - r2[6] * m1);
  r1[7] = s * (r1[7] - r2[7] * m1);
  m0 = r0[2];
  r0[4] -= r2[4] * m0;
  r0[5] -= r2[5] * m0;
  r0[6] -= r2[6] * m0;
  r0[7] -= r2[7] * m0;

  m0 = r0[1];                 /* Now back substitute row 0. */
  s  = 1.0f/r0[0];
  r0[4] = s * (r0[4] - r1[4] * m0);
  r0[5] = s * (r0[5] - r1[5] * m0);
  r0[6] = s * (r0[6] - r1[6] * m0);
  r0[7] = s * (r0[7] - r1[7] * m0);

  floats[(0)*4+(0)] = r0[4]; floats[(0)*4+(1)] = r0[5];
  floats[(0)*4+(2)] = r0[6]; floats[(0)*4+(3)] = r0[7];
  floats[(1)*4+(0)] = r1[4]; floats[(1)*4+(1)] = r1[5];
  floats[(1)*4+(2)] = r1[6]; floats[(1)*4+(3)] = r1[7];
  floats[(2)*4+(0)] = r2[4]; floats[(2)*4+(1)] = r2[5];
  floats[(2)*4+(2)] = r2[6]; floats[(2)*4+(3)] = r2[7];
  floats[(3)*4+(0)] = r3[4]; floats[(3)*4+(1)] = r3[5];
  floats[(3)*4+(2)] = r3[6]; floats[(3)*4+(3)] = r3[7];

}

#ifdef YAC_WIN32
double gettime(void) {
   return (double) GetTickCount();
}
#else
double gettime(void) {
   struct timeval tv;
   gettimeofday(&tv,NULL);
   double res=tv.tv_sec*1000000.0;
   res+=tv.tv_usec;
   return res;
}
#endif // YAC_WIN32



int main(int argc, char** argv) {
   testmatrix m, w;
   double start, stop, bench1, bench2;
   int i;
   std::fprintf(stderr,"Benchmarking Matrix4x4 invert Algorithms\n");
   std::fprintf(stderr,"Using %d loops\n",LOOP);
   m.floats[0]=2;
   m.floats[1]=2;
   m.floats[2]=3;
   m.floats[3]=4;
   m.floats[4]=5;
   m.floats[5]=7;
   m.floats[6]=7;
   m.floats[7]=8;
   m.floats[8]=9;
   m.floats[9]=10;
   m.floats[10]=12;
   m.floats[11]=12;
   m.floats[12]=13;
   m.floats[13]=14;
   m.floats[14]=15;
   m.floats[15]=17;
   std::fprintf(stderr,"Warm up for dynamic speed cpu's\n");
   for (i=0;i<LOOP;i++) {
      m.transpose();
      w=m;
      w.invert();
   }
   std::fprintf(stderr,"Benchmarking Determinant Inversion\n");
   start=gettime();
   for (i=0;i<LOOP;i++) {
      m.transpose();
      w=m;
      w.invert();
   }
   stop=gettime();
   bench1=(LOOP*1000.0)/(stop-start);
   std::fprintf(stderr,"Time used %lfms, Resulting speed is %lf loops/[ms]\n",
      (stop-start)/1000.0, bench1);

   std::fprintf(stderr,"Benchmarking Standard solution\n");
   start=gettime();
   for (i=0;i<LOOP;i++) {
      m.transpose();
      w=m;
      w.invert2();
   }
   stop=gettime();
   bench2=(LOOP*1000.0)/(stop-start);
   std::fprintf(stderr,"Time used %lfms, Resulting speed is %lf loops/[ms]\n",
      (stop-start)/1000.0, bench2);
   if (bench1>=bench2) {
      std::fprintf(stdout,"-DINVERT_DET\n");
   } else {
      std::fprintf(stdout,"-UINVERT_DET\n");
   }
}
