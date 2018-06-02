# lang-mu
Language for working with matrices and vectors

### Build
We are using Code::Blocks for build.  
To build, you need to install flex and bison.  

### Functions

def
---
(**def** *name* *value*)  
**name**: atom (not evaluated)  
**value**: any expression  
Locally bind atom name to value  
**returns**: binded value  

set
---
(**set** *name* *value*)  
**name**: atom (not evaluated)  
**value**: any expression  
If atom is binded set binding in the scope where atom was first binded  
else acts same as **def**  
**returns**: binded value  

defm, setm
----------
(**defm** *name* *value*)  
(**setm** *name* *value*)  
**name**: atom (not evaluated)  
**value**: macro object  
Functions for macro registration.  
**returns**: registered macro  

set-head, set-tail
------------------
(**set-head** *pair* *value*)  
(**set-tail** *pair* *value*)  
**pair**: pair  
**value**: any expression  
Set head/tail of pair  
**returns**: new head/tail

eq
--
(**eq** *left* *right*)
Checks the equality of values  
**returns**:  
`T` if **left** and **right** are equal atoms, numbers ot strings  
`nil` otherwise

and
---
(**and** &rest values)  
Conjunction  
**returns**: `T` if all arguments are true values, and `nil` otherwise

or
---
(**or** &rest values)  
Disjunction  
**returns**: `T` if any of arguments is true value, and `nil` otherwise

not
---
(**not** value)  
Negation  
**returns**: `T` if argument is false value, and `nil` otherwise

xor
---
(**xor** &rest values)  
Exclusive-disjunction  
**returns**: `T` if only one of arguments is true value, and `nil` otherwise

Not documented function
-----------------------
* lambda
* cond
* print
* prints
* quote

* cons
* head
* tail

* gensym
* backquote
* macro
* macroexpand
* gc-collect

* plus (+)
