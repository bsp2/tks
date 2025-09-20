
/* about 3 times faster than the tkscript JIT */

#include <stdio.h>
#include <math.h>
#include <windows.h>

typedef signed int sSI;

static unsigned int ibuffer[65536*2];

float deltat;

void render(void) {    
  int dx;
  int dy;
  int fx;
  int fy;
  int ffx;
  int ffy;
  int i;
  int zx,zy;

  // Calculate the zooming and rotating
  dx=(int)(sin(deltat)*(550.0f+(sin(deltat/3.0f)*500.0f)));
  dy=(int)(cos(deltat)*(550.0f+(sin(deltat/3.0f)*500.0f)));
  fx=(((sSI)(128+(sin(deltat/4.0f)*900.0f)))<<8)-128*dx-128*dy;
  fy=((sSI)(128+(cos(deltat/4.0f)*300.0f))<<8)-128*dy+128*dx;
    
  i=65536;  // the "tpix" buffer is part of the "buffer"
  for(zx=0; zx<256; zx++)
    {
      ffx = fx; ffy = fy;
      for(zy=0; zy<256; zy++)
	{
	  fy+=dy; // JAVA :buffer[i++] = tpix[((fy+=dy)&0xff00)|(((fx+=dx)>>8)&0xff)];
	  fx+=dx;
	  ibuffer[i++]=ibuffer[(fy&0xff00)|((fx>>8)&0xFF)];
	}
      fx = ffx + dy; fy = ffy - dx;
    }
}

int main(int _argc, char **_argv) {
  int l;
  int i;

  deltat=0.0f;
  l=GetTickCount();
  for(i=0; i<256; i++)
    {
      render();
      deltat+=0.03f;
    }
  l=GetTickCount()-l;
  printf("time=%i ms\n", l);
  printf("time=%f fps\n", (1000.0/(l/256.0)));
  exit(0);
}
