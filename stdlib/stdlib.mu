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

(defmacro apply (func args) `(eval (cons ,func ,args)))

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

(defun __map (func l)
    (cond
        ((nil? l) nil)
        ((not (pair? l))
            (error "map: argument not a list"))
        (T (cons (func (head l)) (__map func(tail l))))))
(defun map (func l)
    (cond
        ((or (not (function? func)) (macro? func))
            (error "map: first argument is not a function"))
        (T (__map func l))))

(defun __all (func l)
    (cond
        ((nil? l) T)
        ((not (pair? l))
            (error "all: argument not a list"))
        (T (and (func (head l)) (__all func(tail l))))))
(defun all (func l)
    (cond
        ((or (not (function? func)) (macro? func))
            (error "all: first argument is not a function"))
        (T (__all func l))))
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

(defun length (lst)
    (cond
        ((nil? lst) 0)
        ((not (list? lst))
            (error "length: argument not a list"))
        (T (+ 1 (length (tail lst))))))

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
        ((< n 0)
            (error "get-at: index" n "is out of range"))
        ((not (list? l))
            (error "get-at: second argument not a list"))
        (T (__get-at n l))))
(defun second (l)
    (cond
        ((not (list? l))
            (error "second: argument not a list"))
        (T (head (tail l)))
    )
)
(defun third (l)
    (cond
        ((not (list? l))
            (error "third: argument not a list"))
        (T (head (tail (tail l))))
    )
)

(defun equals (a b)
    (cond
        ((eq a b) T)
        ((and (pair? a) (pair? b))
            (and
                (equals (head a) (head b))
                (equals (tail a) (tail b))))
    )
)
///////////////////////////////////////////////////////////////////////////////
/// Strings                                                                 ///
///////////////////////////////////////////////////////////////////////////////

(defun str-from-list (lst &optional (len nil))
    (let ((lst-len (length lst)))
        (unless (list? lst)
            (error "str-from-list: argument `lst` must be a list"))
        (when len
            (when (or (< len 0) (> len lst-len))
                (error "str-from-list: argument `len` is out of range")))
        (__str-from-list lst (if (nil? len) lst-len len))
    )
)

(defun str-to-list (str &optional (start 0) (len nil))
    (unless (string? str)
        (error "str-to-list: argument `string` must be a string"))
    (unless (integer? start)
        (error "str-to-list: argument `start` must be an integer"))
    (when (or (< start 0) (> start (str-len str)))
        (error "str-to-list: argument `start` is out of range"))
    (when len
        (unless (integer? len)
            (error "str-to-list: argument `len` must be an integer"))
        (when (or (< len 0) (> (+ start len ) (str-len str)))
            (error "str-to-list: argument `len` is out of range")
        ))
    (__str-to-list str start (if (nil? len) (- (str-len str) start) len))
)

(defun str-repeat (sub count)
    (unless (integer? count)
        (error "str-repeat: argument `count` not an integer"))
    (cond
        ((string? sub) (__str-repeat-str sub count))
        ((char? sub) (__str-repeat-char sub count))
        (T (error "str-repeat: argument `sub` not a string or char"))))

(defun check-match (args pattern)
    (cond
        ((nil? args) (nil? pattern))
        ((nil? pattern) nil)
        (T
            (unless (pair? args)
                (error "check-match: argument `args` not a list"))
            (unless (and (pair? pattern) (function? (head pattern)))
                (error "check-match: invalid pattern"))
            (when ((head pattern) (head args))
                (check-match (tail args) (tail pattern))
            )
        )
    )
)

(defun __str-cmp-full (a b)
    (__str-cmp-sub a 0 (str-len a) b 0 (str-len b)))
    
(defun __str-cmp-from (a a_start b b_start)
    (__str-cmp-sub
        a a_start (- (str-len a) a_start)
        b b_start (- (str-len b) b_start)))

(defun str-cmp (&rest args)
    (cond
        ((check-match args (list string? string?))
            (apply __str-cmp-full args))
        ((check-match args (list string? int? string? int?))
            (apply __str-cmp-from args))
        ((check-match args (list string? int? int? string? int? int?))
            (apply __str-cmp-sub args))
        (T (error "str-cmp: no suitable overload"))
    )
)

(defun str-cat (&rest args)
    (unless (all string? args)
        (error "str-cat: string expected"))
    (apply __str-cat args)
)

(defun str-ind (str sub &optional (start 0) (count nil))
    (unless (string? str)
        (error "str-ind: argument `str` not a string"))
    (unless (or (string? sub) (char? sub))
        (error "str-ind: argument `sub` not a string or char"))
    (unless (integer? start)
        (error "str-ind: argument `start` not an integer"))
    (when (or (< start 0) (> start (str-len str)))
        (error "str-ind: argument `start` is out of range"))
    (when count
        (unless (integer? count)
            (error "str-ind: argument `count` not an integer"))
        (when (or (< count 0) (> (+ start count) (str-len str)))
            (error "str-ind: argument `count` is out of range"))
    )
    (__str-ind nil str sub start (if count count (- (str-len str) start)))
)
    
(defun str-last-ind (str sub &optional (start nil) (count nil))
    (unless (string? str)
        (error "str-last-ind: argument `str` not a string"))
    (unless (or (string? sub) (char? sub))
        (error "str-last-ind: argument `sub` not a string or char"))
    
    (when start
        (unless (integer? start)
            (error "str-last-ind: argument `start` not an integer"))
        (when (or (< start 0) (> start (str-len str)))
            (error "str-last-ind: argument `start` is out of range"))
    )
    
    (when count
        (unless (integer? count)
            (error "str-last-ind: argument `count` not an integer"))
        (when (or (< count 0)
                  (and start (> count (+ start 1))))
            (error "str-last-ind: argument `count` is out of range"))
    )
    
    (cond
        ((and start count)
            (__str-ind T str sub start count))
        (start
            (__str-ind T str sub start start))
        ((== (str-len str) 0)
            (__str-ind T str sub 0 0))
        (T
            (__str-ind T str sub (str-len str) (+ 1 (str-len str))))
    )
)
