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
/// Math                                                                    ///
///////////////////////////////////////////////////////////////////////////////
(defun abs (num)
    (unless (number? num)
        (error "abs: argument not a number"))
    (if (< num 0) (- num) num)
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
    (unless (int? start)
        (error "str-to-list: argument `start` must be an integer"))
    (when (or (< start 0) (> start (str-len str)))
        (error "str-to-list: argument `start` is out of range"))
    (when len
        (unless (int? len)
            (error "str-to-list: argument `len` must be an integer"))
        (when (or (< len 0) (> (+ start len ) (str-len str)))
            (error "str-to-list: argument `len` is out of range")
        ))
    (__str-to-list str start (if (nil? len) (- (str-len str) start) len))
)

(defun str-repeat (sub count)
    (unless (int? count)
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

(defun str-cat-list (args)
    (unless (all string? args)
        (error "str-cat-list: string expected"))
    (apply __str-cat args)
)

(defun str-ind (str sub &optional (start 0) (count nil))
    (unless (string? str)
        (error "str-ind: argument `str` not a string"))
    (unless (or (string? sub) (char? sub))
        (error "str-ind: argument `sub` not a string or char"))
    (unless (int? start)
        (error "str-ind: argument `start` not an integer"))
    (when (or (< start 0) (> start (str-len str)))
        (error "str-ind: argument `start` is out of range"))
    (when count
        (unless (int? count)
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
        (unless (int? start)
            (error "str-last-ind: argument `start` not an integer"))
        (when (or (< start 0) (> start (str-len str)))
            (error "str-last-ind: argument `start` is out of range"))
    )
    
    (when count
        (unless (int? count)
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

(defun str-insert (str index sub)
    (unless (string? str)
        (error "str-insert: argument `str` not a string"))
    (unless (or (string? sub) (char? sub))
        (error "str-insert: argument `sub` not a string or character"))
    (unless (int? index)
        (error "str-insert: argument `index` not an integer"))
    (when (or (< index 0) (> index (str-len str)))
        (error "str-insert: argument `index` is out of range"))
    (__str-insert str index sub)
)

(defun str-sub (str index &optional (len nil))
    (unless (string? str)
        (error "str-sub: argument `str` not a string"))
    (unless (int? index)
        (error "str-sub: argument `index` not an integer"))
    (when len
        (unless (int? len)
            (error "str-sub: argument `len` not an integer"))
        (when (or (< len 0) (> len (- (str-len str) index)))
            (error "str-sub: argument `index` is out of range"))
    )
    (__str-sub str index (if len len (- (str-len str) index)))
)

(defun str-remove (str index &optional (len nil))
    (unless (string? str)
        (error "str-remove: argument `str` not a string"))
    (unless (int? index)
        (error "str-remove: argument `index` not an integer"))
    (when len
        (unless (int? len)
            (error "str-remove: argument `len` not an integer"))
        (when (or (< len 0) (> len (- (str-len str) index)))
            (error "str-remove: argument `index` is out of range"))
    )
    (__str-remove str index (if len len (- (str-len str) index)))
)

(defun str-contains (str sub &optional (start 0) (count nil))
    (unless (string? str)
        (error "str-contains: argument `str` not a string"))
    (unless (or (string? sub) (char? sub))
        (error "str-contains: argument `sub` not a string or char"))
    (unless (int? start)
        (error "str-contains: argument `start` not an integer"))
    (when (or (< start 0) (> start (str-len str)))
        (error "str-contains: argument `start` is out of range"))
    (when count
        (unless (int? count)
            (error "str-contains: argument `count` not an integer"))
        (when (or (< count 0) (> (+ start count) (str-len str)))
            (error "str-contains: argument `count` is out of range"))
    )
    (> (str-ind str sub start count) -1)
)

(defun str-starts-with (str sub)
    (unless (string? str)
        (error "str-starts-with: argument `str` not a string"))
    (unless (or (string? sub) (char? sub))
        (error "str-starts-with: argument `sub` not a string or char"))
    (cond
        ((string? sub)
            (== 0 (str-cmp str 0 (str-len sub) sub 0 (str-len sub))))
        ((char? sub)
            (eq (str-at str 0) sub))
    )
)

(defun str-ends-with (str sub)
    (unless (string? str)
        (error "str-ends-with: argument `str` not a string"))
    (unless (or (string? sub) (char? sub))
        (error "str-ends-with: argument `sub` not a string or char"))
    (cond
        ((string? sub)
            (== 0
                (str-cmp
                    str (- (str-len str) (str-len sub)) (str-len sub)
                    sub 0 (str-len sub))))
        ((char? sub)
            (eq (str-at str (- (str-len str) 1)) sub))
    )
)

(defun str-empty? (str)
    (unless (or (nil? str) (string? str))
        (error "str-empty?: argument not a string"))
    (or (nil? str) (eq str ""))
)

(defun __str-join-insert-separator (sep lst)
    (cond
        ((nil? lst) nil)
        ((nil? (tail lst)) (list (head lst)))
        (T (cons (head lst) (cons sep (__str-join-insert-separator sep (tail lst)))))
    )
)

(defun str-join (sep strings)
    (unless (string? sep)
        (error "str-join: argument `sep` not a string"))
    (unless (list? strings)
        (error "str-join: argument `strings` not a list"))
    (unless (all string? strings)
        (error "str-join: argument `strings` not a list of strings"))
    (str-cat-list (__str-join-insert-separator sep strings))
)

(defun __str-split (str sep remove-empty start)
    (let ((ind (str-ind str sep start)))
        (cond
            ((== -1 ind)
                (unless (and remove-empty (== start (str-len str)))
                    (list (str-sub str start))
                )
            )
            ((and remove-empty (== ind start))
                (__str-split str sep remove-empty (+ ind (str-len sep)))
            )
            (T (cons
                (str-sub str start (- ind start))
                (__str-split str sep remove-empty (+ ind (str-len sep)))))
        )
    )
)
(defun str-split (str sep &optional (remove-empty nil))
    (unless (string? str)
        (error "str-split: argument `str` not a string"))
    (unless (string? sep)
        (error "str-split: argument `sep` not a string"))
    (__str-split str sep remove-empty 0)
)

(defun dec-str->int (str i acc)
    (if (== i (str-len str))
        acc
        (let ((ch (str-at str i)))
            (cond
                ((and (<= '0' ch '9'))
                    (dec-str->int str (+ i 1) (+ (* acc 10) (- ch '0')))
                )
                (T
                    (error "dec-str->int: unexpected character"))
            )
        )
    )
)
(defun str->int (str)
    (unless (string? str)
        (error "str->int: argument not a string"))
    (if (< (str-len str) 1)
        0
        (let ((ch (str-at str 0)))
            (cond
                ((== ch '+') (dec-str->int str 1 0))
                ((== ch '-') (- (dec-str->int str 1 0)))
                (T        (dec-str->int str 0 0))
            )
        )
    )
)

(defun __int->str (num base acc)
    (def digits "0123456789abcdefghijklmnopqrstuvwxyz")
    (cond
        ((== 0 num) acc)
        (T
            (__int->str
                (/ num base)
                base
                (cons (str-at digits (% num base)) acc)))
    )
)
(defun int->str (num &optional (base 10))
    (unless (int? num)
        (error "int->str: argument not an int"))
    (let (
            (n (abs num))
            (digits (__int->str n base nil))
            (characters
                (cond
                    ((< num 0) (cons '-' digits))
                    ((> num 0) digits)
                    (T ' ('0'))))
        )
        (str-from-list characters)
    )
)
