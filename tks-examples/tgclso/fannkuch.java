/* The Great Computer Language Shootout
   http://shootout.alioth.debian.org/

   contributed by Paul Lofte
*/

public class fannkuch {
    public static void main(String[] args) {
        int n = 7;
        if (args.length > 0) n = Integer.parseInt(args[0]);

        System.out.println("Pfannkuchen(" + n + ") = " + fannkuch(n));
    }

    public static int fannkuch(int n) {
        int[] perm = new int[n];
        int[] perm1 = new int[n];
        int[] count = new int[n];
        int[] maxPerm = new int[n];
        int maxFlipsCount = 0;
        int m = n - 1;

        long milliSeconds=System.currentTimeMillis();

        for (int i = 0; i < n; i++) perm1[i] = i;
        int r = n;

        while (true) {
            while (r != 1) { count[r - 1] = r; r--; }
            if (!(perm1[0] == 0 || perm1[m] == m)) {
                for (int i = 0; i < n; i++) perm[i] = perm1[i];

                int flipsCount = 0;
                int k;

                while (!((k = perm[0]) == 0)) {
                    int k2 = (k + 1) >> 1;
                    for (int i = 0; i < k2; i++) {
                        int temp = perm[i]; perm[i] = perm[k - i]; perm[k - i] = temp;
                    }
                    flipsCount++;
                }

                if (flipsCount > maxFlipsCount) {
                    maxFlipsCount = flipsCount;
                    for (int i = 0; i < n; i++) maxPerm[i] = perm1[i];
                }
            }

            while (true) {
                if (r == n) 
                   {
                      milliSeconds=System.currentTimeMillis()- milliSeconds;
                      System.out.println("t(ms)="+milliSeconds);
                      return maxFlipsCount;
                   }
                int perm0 = perm1[0];
                int i = 0;
                while (i < r) {
                    int j = i + 1;
                    perm1[i] = perm1[j];
                    i = j;
                }
                perm1[r] = perm0;

                count[r] = count[r] - 1;
                if (count[r] > 0) break;
                r++;
            }
        }
    }
}

