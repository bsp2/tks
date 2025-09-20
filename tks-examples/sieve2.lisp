; by Stefan Nobis, see http://de.nntp2http.com/comp/lang/funktional/2005/02/62c0c046b89ab4a43a615bc2937d0e0c.html

 (defun primep (k primes)
   (primep-helper k (first (reverse primes)) (rest (reverse primes)))
   )

 (defun primep-helper (k p primes)
   (cond ((endp primes) t)
         ((integerp (/ k p)) nil)
         (t (primep-helper k (first primes) (rest primes))))
   )


 (defun sieve (n)
   (let ((primes '(2))
         (i 3)
         )
     (loop
      (cond ((> i n) (return primes))
            ((primep i primes) (push i primes)))
      (setq i (+ i 2))
      )
     )
   )

 (defun sieve2 (n)
   (sieve2-helper n 3 '(2)))

 (defun sieve2-helper (n k primes)
   (cond ((> k n) primes)
         (t (when (primep k primes) (push k primes))
            (sieve2-helper n (+ k 2) primes))))

; (time (sieve 1000))
;(time (dotimes (iter 10) (sieve2 1000)))
(print (sieve2 1000))

