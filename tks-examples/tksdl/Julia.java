
//  package Julia;

import java.lang.*;
import java.io.*;

public class Julia {
    public static void main(String[] args) {
	System.out.println("sdfksfd\n");
	long l=System.currentTimeMillis();

	int x;
	int y;
	int z;
	float xx;
	float yy;
	float angle_i;
	float angle_j;
	angle_i=0.0f;
	angle_j=0.013f;//26854f;
	float c1;
	float c2;
	float s_xx;
	float s_yy;
	float c_yy;
	int iy;
	float c_xx;
	int iter;
	float qxx;
	float qyy;
	int itd;
	float ft=0.0f;
	itd=16;

	for(z=0; z<1024; z++)
	    {
		c1=(float)Math.sin(angle_i+Math.sin(angle_i)*2.0f*3.1415)*0.75f;
		c2=(float)Math.cos(angle_j)*0.7f;
		
		s_xx= 1.0f/32.0f;
		s_yy= 1.0f/32.0f;

		c_yy= -1.8f;
		for(y=0; y<256; y++)
		    {
			c_xx= -2.0f;
			for(x=0; x<256; x++)
			    {
				qxx=c_xx*c_xx;
				qyy=c_yy*c_yy;
				xx=c_xx;
				yy=c_yy;
				//trace("qxx+qyy="+(qxx+qyy));
				for(iter=0; (iter<itd) && ((qxx+qyy)<=4) ; iter++)
				    {
					yy=2.0f*xx*yy+c2;
					xx=qxx-qyy+c1;
					qxx=xx*xx;
					qyy=yy*yy;
				    }
				//  			TextureModPrim.setXY32(dtex, x, y, iter<<3);
				c_xx+=s_xx;
			    }
			c_yy+=s_yy;
		    }
		    angle_i+=0.01f;
		    angle_j+=0.06485f;
		    if(angle_i>2.0f*3.1415f)
			angle_i-=2.0f*3.1415f;
		    if(angle_j>2.0f*3.1415f)
			angle_j-=2.0f*3.1415f;
	    }

	l=System.currentTimeMillis()-l;
	System.out.println("time="+l+"ms\n");
 	System.out.println((1000.0f/(l/1024.0f))+"fps.");
    }
};

