///
/// Macro `defmacro` allows to create macros in a simple way
///
(defm defmacro (macro (name args &rest body)
    `(defm ,name (macro ,args ,@body))
))

///
/// Macro `defun` allows to create function in a simple way
///
(defmacro defun (name args &rest body)
    `(def ,name (lambda ,args ,@body))
)

(defmacro if (condition then else)
    `(cond
        (,condition ,then)
        (T ,else))
)

(defmacro when (condition &rest actions)
    `(cond
        (,condition ,@actions)))

(defmacro unless (condition &rest actions)
    `(cond
        (,condition nil)
        (T ,@actions)))

///
/// Returns `T` if `expr` is `nil`, otherwise returns nil`
///
(defun nil? (expr) (not expr))

///
/// Returns `T` if `expr` is char or int, otherwise returns `nil`
///
(defun integer? (expr) (or (char? expr) (int? expr)))

///
/// Returns `T` if `expr` is char, int or real, otherwise returns `nil`
///
(defun number? (expr) (or (integer? expr) (real? expr)))

///
/// Returns `T` if `expr` is nil-terminated list, otherwise returns `nil`
///
(defun list? (expr) (if (nil? expr) T (and (pair? expr) (list? (tail expr)))))

(defun __map (l func)
    (cond
        ((nil? l) nil)
        (T (cons (func (head l)) (__map (tail l) func)))))
(defun map (l func)
    (cond
        ((not (list? l))
            (error "map: first argument is not a list"))
        ((or (not (function? func)) (macro? func))
            (error "map: second argument is not a function"))
        (T (__map l func))))

///
/// Constructs list which contains all arguments
///
(defun list (&rest items) items)

(defun __append (a b)
    (cond
        ((nil? a) b)
        (T (cons (head a) (__append (tail a) b)))))
(defun append (a b)
    (cond
        ((not (list? a))
            (error "append: first argument not a list")
        )
        ((not (list? b))
            (error "append: second argument not a list")
        )
        (T (__append a b))))

(defun __get-at (n l)
    (cond
        ((nil? l)
            (error "get-at: index out of range"))
        ((== n 0) (head l))
        (T (__get-at (- n 1) (tail l)))))
(defun get-at (n l)
    (cond
        ((not (integer? n))
            (error "get-at: index not an integer"))
        ((or (< n 0) )
            (error "get-at: index" n "is out of range"))
        ((not (list? l))
            (error "get-at: second argument not a list"))
        (T (__get-at n l))))
        
        
