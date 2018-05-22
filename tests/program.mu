(defun main ()
    (printlns "Hello!!!")
    (printlns "I'm `lang-mu` and I want to know everything.")
    (printlns "What is your name?")
    (prints ">>> ")
    (let ((name (scanline )))
        (if (eq (to-lower name) "arseniy")
            {
                (printlns "Oh! My creator is also named Arseniy")
                (printlns "Or you trying to pretend that you are my creator?!")
                (let ((answer (to-lower (scanline))))
                    (cond
                        ((eq answer "yes") (error "Access denied!!!"))
                        ((eq answer "no") (error "Name overloading not supported!!!"))
                        (T
                            (printlns "What do you mean???")
                            (printlns "Doesn't matter!")
                            (error "Access denied!!!")
                        )
                    )
                )
            }
            {
                (printlns (str-join " " `("Hello" ,name "!!!")))
            }
        )
    )
)

(main)

