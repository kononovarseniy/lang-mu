(print '\t')
(print '(hello world))
(prints '!' '! "!" ''!' ''! '"!")
(print 2 '+ '2 '= (+ 2 2))
(print 2 '+ 3 '+ 4 '= (+ 2 3 4))

(def x (+ 40 2))
(print x)

(def x (+ x 2))
(print x)

(prints "<<< check special atoms >>>")
(print '(, ,@ b ,a ,@a))
(prints ''(1 2 3))
(prints `(1 ,(+ 1 1) 3 ,@(cons 4 (cons 5 nil)) 6))
(prints `(defmacro ,",name," (print ,",args," ,@'(",body,"))))

(prints "<<< check lists >>>")
(print (cons 'a (cons 1 nil)))
(def make-tree
     (lambda ()
         (cons 'a
                (cons '(1 2 3)
                      nil))))
(def tree (make-tree))
(print tree)
(print (head (head (tail tree))))

(prints "<<< check functions >>>")
(def
    foo
    (lambda (a b)
        (prints "(" 'foo a b ")" '--> (+ a b))
        (print 'x '= x)
        (+ a b)))
(foo 5 -6)
(def get-nil (lambda nil nil))
(get-nil)

(def print-to-10 (lambda (&optional (n 0))
    (cond ((+ n -10) (print n) (print-to-10 (+ n 1))))
))
(print-to-10)

(def list (lambda (&rest items) items))
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
(defm defmacro (macro (name args &rest body)
    `(defm ,name (macro ,args ,@body))
))
(prints "defmacro-def end")
(defmacro defun (name args &rest body)
    `(def ,name (lambda ,args ,@body))
)
(prints "defun-def end")
(defun foo (a b &rest c) (+ a b))
(prints "foo-def end")
(prints "2 + 3 =" (foo 2 3 4 5 6))
(print (macroexpand 'defun '(bar (a b &rest c) 1 2 3)))

(prints "<<< RUN GARBAGE COLLECTOR >>>")
(def gc-res (gc-collect))
(def gc-atoms (head gc-res))
(def gc-pairs (head (tail gc-res)))
(def gc-objects (head (tail (tail gc-res))))
(prints "garbage collected")
(prints "\tatoms:" gc-atoms)
(prints "\tpairs:" gc-pairs)
(prints "\tobjects:" gc-objects)

(prints "<<< context tests >>>")
(def global-val-1 "Unchanged")
(def global-val-2 "Unchanged")

(defun set-global-val ()
    (set global-val-1 "Changed") // global
    (def global-val-2 "Changed") // local
)
(set-global-val)
(prints "Changed value is" global-val-1)
(prints "Unchanged value is" global-val-2)

(prints "<<< set pair test >>>")
(def res '(1 2))
(set-head res 'head)
(set-tail res '(tail))
(print res)

