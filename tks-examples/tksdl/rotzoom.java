//****************************
// Retroto Zoomer
// Copyright 2001
// Tommi Laukkanen
//****************************

import java.lang.*;
import java.io.*;

public class rotzoom {
    public static void main(String[] args) {

	int tpix[];
	tpix = new int[256*256];
	int buffer[];
	buffer = new int[256*256];  // Frame buffer

	long l=System.currentTimeMillis();

	int z=1000;
	// Variables
	float deltat=0.0f;
	while(--z>0)
	    {
		deltat+=0.03f;
		// Calculate the zooming and rotating    
		int dx=(int)(Math.sin(deltat)*(800+(Math.sin(deltat/3)*500)));
		int dy=(int)(Math.cos(deltat)*(800+(Math.sin(deltat/3)*500)));
		int fx=((128+(int)(Math.sin(deltat/4)*900))<<8)-128*dx-128*dy;
		int fy=((128+(int)(Math.cos(deltat/5)*300))<<8)-128*dy+128*dx;
		
		int color=0;
		int col, row;
		int ffx, ffy;
		int i=0;

		// Do the draw thing
		for(col=0; col<256; col++)
		    {
			ffx = fx; ffy = fy;
			for(row=0; row<256; row++)
			    {
				buffer[i++] = tpix[((fy+=dy)&0xff00)|(((fx+=dx)>>8)&0xff)];
			    }
			fx = ffx + dy; fy = ffy - dx;
		    }
	    }
	l=System.currentTimeMillis()-l;
	System.out.println("time="+l+"ms\n");
 	System.out.println((1000.0f/(l/1000.0f))+"fps.");
    }
};














