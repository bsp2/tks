/*
 * aus c't 19/03
 */

#include <stdio.h>
#include <sys/time.h>

struct timeval t;

// RealCalcLoop
int main(int _c, char**_argv) {
  gettimeofday(&t, 0);
    int Res=0;
    int x,y,z;
    for(x=1; x<=100; x++)
	{
	    for(y=1; y<=1000; y++)
		{
		    for(z=1; z<=1000; z++)
			{ 
			    Res+=(x+y+z) % 4;
			}
		}
	}
    struct timeval t2;
    gettimeofday(&t2, 0);
    printf("ticks=%i", (t2.tv_sec-t.tv_sec)*1000 + (t2.tv_usec-t.tv_usec)/1000 );
    return 0;
}


