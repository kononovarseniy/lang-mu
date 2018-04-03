(print '\t')
(print '(hello world))
(prints '!' '! "!" ''!' ''! '"!")
(print 2 '+ '2 '= (+ 2 2))
(print 2 '+ 3 '+ 4 '= (+ 2 3 4))

(set x (+ 40 2))
(print x)

(set x (+ x 2))
(print x)

(set
    foo
    (lambda (a b)
        (prints "(" 'foo a b ")" '--> (+ a b))
        (print 'x '= x)
        (+ a b)))

(foo 5 -6)
 
