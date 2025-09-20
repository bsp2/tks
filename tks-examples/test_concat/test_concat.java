// $Id: lists.java,v 1.1 2001/01/14 18:13:15 doug Exp $
// http://www.bagley.org/~doug/shootout/

import java.io.*;
import java.util.*;
import java.text.*;


public class test_concat {
   public static void main(String args[]) {
      long t = System.currentTimeMillis();
      
      String r, s1="hallo, ", s2="welt...", s3="some ", s4="string", s5="concatenations";
      
      int l = 0;
      int i=0;
      for(int k=0; k<10000000; k++)
      {
         r = s1 + s2 + s3 + s4 + s5;
         l = r.length();
         i++;
      }
      
      System.out.println("i=" + i);
      System.out.println("l=" + l);
      System.out.println("ms = " + (System.currentTimeMillis()-t));
      
   }
}

