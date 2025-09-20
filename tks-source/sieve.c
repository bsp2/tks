
#include <stdio.h>
int flags[8191];

int main(int _argc, char**_argv) {
   int count;
   int size= 8190;
   int i, prime, k, iter;
   printf("Eratosthenes Sieve prime number calculation\n");
    
   printf("100000 iterations\n");
   for (iter = 1; iter <= 100000; iter++)
	{
      count = 0;
      for (i = 0; i <= size; i++) flags[i] = 1;
      for (i = 0; i <= size; i++)
		{
         if (flags[i])
			{
            prime = i + i + 3;
            k = i + prime;
            while (k <= size)
				{
               flags[k] = 0;
               k += prime;
				}
            count ++;
			}
		}
	}
   printf("%d primes\n", count);
}

// ---- original program in the "Dscript" language :
//  println("Eratosthenes Sieve prime number calculation");

//  size = 8190;
//  sizepl = 8191;

//  var flags = new Array(sizepl);

//  var i, prime, k, count, iter;

//  println("10 iterations<br>");
//  starttime = new Date();
//  for (iter = 1; iter <= 10; iter++)
//  {   count = 0;
//      for (i = 0; i <= size; i++)
//  	flags[i] = true;
//      for (i = 0; i <= size; i++)
//      {   if (flags[i])
//  	{   prime = i + i + 3;
//  	    k = i + prime;
//  	    while (k <= size)
//  	    {
//  		flags[k] = false;
//  		k += prime;
//  	    }
//  	    count += 1;
//  	}
//      }
//  }
//  elapsedtime = new Date() - starttime;
//  println(count + " primes");
//  println("elapsed time = " + elapsedtime);
