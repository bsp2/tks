; CLISP 2.46 : 
;     real    0m9.548s
;     user    0m9.360s
;     sys     0m0.061s
;
; SBCL 1.0.19 (JIT) :
;     real    0m0.782s
;     user    0m0.000s
;     sys     0m0.015s
;
; TkScript 0.9.10.44 (interpreted mode) :
;     real    0m0.075s
;     user    0m0.015s
;     sys     0m0.015s
;
;
; TkScript 0.9.10.44 (JIT mode) :
;     real    0m0.036s
;     user    0m0.000s
;     sys     0m0.031s

; SBCL built with: /usr/local/bin/sbcl -batch -eval '(load "sieve.lisp")'
; ### START nbody.sbcl_compile
; (proclaim '(optimize (speed 3) (safety 0) (debug 0) (compilation-speed 0) (space 0)))
; (handler-bind ((sb-ext:defconstant-uneql      (lambda (c) (abort c))))      (load (compile-file "sieve.lisp" ))) (save-lisp-and-die "sbcl.core" :purify t)
; sbcl --noinform --core sbcl.core --userinit /dev/null --load sieve.fasl


(setf *size* 8191)
(dotimes (iter 100)
  (setq flags (make-array *size*))
  (setq count 0)
  (dotimes (i *size*) 
    (setf (aref flags i) 1)
    )
  (dotimes (i *size*)
    (if (= (aref flags i) 1)
	(progn
	  (setq prime (+ i i 3))
	  (setq k (+ i prime))
	  (loop while (< k *size*) do
		(setf (aref flags k) 0)
		(setf k (+ k prime))
		)
	  (incf count)
	  )
      ) ; endif
    ) ; end i<size
  ;(print flags)
  )
(print count)

;(sb-ext:quit) ;; for SBCL version

