(print '\t')
(print '(hello world))
(prints '!' '! "!" ''!' ''! '"!")
(print 2 '+ '2 '= (+ 2 2))
(print 2 '+ 3 '+ 4 '= (+ 2 3 4))

(set x (+ 40 2))
(print x)

(set x (+ x 2))
(print x)

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
