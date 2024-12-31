# Short Circuiting Operators

The C standard guarantees that && and || short-circuit when we don't need the second 
operand.

This isn't just a performance optimization; the second operand might not change the result of the expression, but evaluating it can have visible side effects (might perform I/O or update global variables).

If our compiler doesn't implement && and || as short-circuiting operators, some compiled
programs may behave incorrectly.