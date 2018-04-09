(print '\t')
(print '(hello world))
(prints '!' '! "!" ''!' ''! '"!")
(print 2 '+ '2 '= (+ 2 2))
(print 2 '+ 3 '+ 4 '= (+ 2 3 4))

(set x (+ 40 2))
(print x)

(set x (+ x 2))
(print x)

(prints "<<< check special atoms >>>")
(print '(, ,@ b ,a ,@a))
(prints ''(1 2 3))
(prints `(1 ,(+ 1 1) 3 ,@(cons 4 (cons 5 nil)) 6))
(prints `(setmacro ,",name," (print ,",args," ,@'(",body,"))))

(prints "<<< check lists >>>")
(print (cons 'a (cons 1 nil)))
(set make-tree
     (lambda ()
         (cons 'a
                (cons '(1 2 3)
                      nil))))
(set tree (make-tree))
(print tree)
(print (head (head (tail tree))))

(prints "<<< check functions >>>")
(set
    foo
    (lambda (a b)
        (prints "(" 'foo a b ")" '--> (+ a b))
        (print 'x '= x)
        (+ a b)))
(foo 5 -6)
(set get-nil (lambda nil nil))
(get-nil)

(set print-to-10 (lambda (&optional (n 0))
    (cond ((+ n -10) (print n) (print-to-10 (+ n 1))))
))
(print-to-10)

(set list (lambda (&rest items) items))
(print (list 1 2 (+ 1 2)))

(prints "<<< cond tests >>>")
(print nil (cond))
(cond (0 (prints "never executed")
      (nil (prints "never executed"))))
(cond (0 (prints "never executed")
      (nil (prints "never executed")))
      (T (prints "Hello")))
(cond (0 (prints "never executed"))
      (T (prints "Hello"))
      (T (prints "never executed"))
      (atom not binded))
(print 2 (cond (0 1) (1 2)))

(prints "<<< gensym tests >>>")
(print (gensym))
(print (gensym))
(print (gensym))

(prints "<<< macro tests >>>")
(setmacro defmacro (macro (name args &rest body)
    `(setmacro ,name (macro ,args ,@body))
))
(print "123")
(defmacro defun (name args &rest body)
    `(set ,name (lambda ,args ,@body))
)
(defun foo (a b &rest c) (+ a b))
(prints "2 + 3 =" (foo 2 3 4 5 6))

