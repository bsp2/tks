(defun primer ()
 ((lambda (&rest q)
  (lambda ()
  ((lambda (u n) (funcall u u n))
   (lambda (u n) (if (some (lambda (x y) (and (/= 1 y) (= (mod x y) 0)))
                           ((lambda (x &aux (y (list x))) (setf (cdr y) y)) n) q)
                     (funcall u u (1+ n)) (setf q (cons n q) n n))) (1+ (car q))))) 1))
 
;; Example use:
(defparameter *png* (primer))
(loop repeat 8190 collect (funcall *png*))
