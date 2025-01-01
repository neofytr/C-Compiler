# Short Circuiting Operators

The C standard guarantees that && and || short-circuit when we don't need the second  operand.

This isn't just a performance optimization; the second operand might not change the result of the expression, but evaluating it can have visible side effects (might perform I/O or update global variables).

If our compiler doesn't implement && and || as short-circuiting operators, some compiled programs may behave incorrectly.

# Comparison and Jumps in Assembly

The "condition" that all conditional instructions depend on is the state of the RFLAGS register.

Unlike general-purpose registers, we usually can't directly set RFLAGS. Instead, the CPU updates RFLAGS automatically every time it issues an instruction. As the name suggests, each bit in this register is a flag that reports some fact about the last instruction or the status of the CPU.

Different instructions update different flags: the `add`, `sub`, and `cmp` instructions update all the flags we'll talk about next, and the `mov` instruction doesn't update any of them. We ignore the effects of other instructions for now. Whenever we refer to the "last instruction" or "last result" while discussing RFLAGS, we mean the last instruction that affects the particular flag we're talking about.

Right now, we care about three flags:

## Zero Flag (ZF)

ZF is set to 1 if the result of the last instruction was zero; otherwise, it's cleared to 0.

## Sign Flag (SF)

SF is set to 1 if the most significant bit of the last result was 1; otherwise, it's cleared to 0. Since in two's complement, the most significant bit is 1 iff the number is negative. Therefore, the sign flag tells us whether the result of the last instruction was positive or negative. (If the last result should be interpreted as an unsigned integer, it can't be negative, so the sign flag is meaningless).

## Overflow Flag (OF)

OF is set to 1 if the last instruction resulted in a signed integer overflow, and 0 otherwise.

A signed integer overflow occurs when the result of a signed integer operation can't be represented in the number of bits available. A positive result overflows when it's larger than the maximum value the type can hold. A negative result underflows (or overflows) when it's smaller than the smallest value the type can hold.

Suppose we're operating on 4-bit integers. The largest signed number we can represent is 7, or 0111 in binary. If we add one to it with the `add` instruction, the result is 1000. If we represent this as an unsigned integer, it's value is 8, but it's value is -8 if we interpret it as a two's complement signed integer. The result of the computation should be positive, but since it overflowed, it appears negative. This computation sets the overflow flag to 1.

We also encounter integer overflow (or underflow) in the opposite situation: when the result should be negative, but it's below the smallest possible value. For example, in ordinary math, -8-1 = -9. But if we use the `sub` instruction to subtract one from the 4-bit two's complement representation of -8, which is 1000, we end up with 0111, or 7. The overflow flag is set to 1 in this case too.

An unsigned result can also be too large or small for it's type to represent, but we won't refer to this as unsigned integer overflow here. Instead, we say the result *wrapped around*, which is more consistent with the terminology for unsigned operations in the C standard and in most discussions of x64 assembly.

We draw this distinction because unsigned wraparound follows different rules from signed integer overflow in the C standard, and the CPU detects it differently. We'll see how to handle unsigned wraparound in Part Two. Like SF, OF is meaningless if the result is unsigned.

For now, all integer types we implement are signed.

The following summarizes the cases where each kind of signed integer overflow/underflow is possible:

| Operation | Condition | Result                             |
|-----------|-----------|------------------------------------|
| a + b     | b > 0     |                                    |
|           | a > 0     | Overflow from positive to negative |
|           | a < 0     | Neither                            |
|           | b < 0     |                                    |
|           | a > 0     | Neither                            |
|           | a < 0     | Overflow from negative to positive |
| a - b     | b > 0     |                                    |
|           | a > 0     | Neither                            |
|           | a < 0     | Overflow from negative to positive |
|           | b < 0     |                                    |
|           | a > 0     | Overflow from positive to negative |
|           | a < 0     | Neither                            |
