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

For addition (a + b):

| a value | b > 0                              | b < 0                              |
| ------- | ---------------------------------- | ---------------------------------- |
| a > 0   | Overflow from positive to negative | Neither                            |
| a < 0   | Neither                            | Overflow from negative to positive |

For subtraction (a - b):

| a value | b > 0                              | b < 0                              |
| ------- | ---------------------------------- | ---------------------------------- |
| a > 0   | Neither                            | Overflow from positive to negative |
| a < 0   | Overflow from negative to positive | Neither                            |

The instruction `cmp b, a` computes a - b, exactly like the `sub` instruction, and has the same impact on RFLAGS, but it discards the result instead of storing it in a. This is more convenient when we want to subtract two numbers only in order to compare then and don't want to overwrite a.

Let's figure out the values of ZF and AF after the instruction `cmp b, a` for signed integers a and b:

1. If a == b, then a - b is zero, so ZF is 1 and SF is 0.
2. If a > b, then a - b is a positive number, so both SF and ZF are 0.
3. If a < b, then a - b is a negative number, so SF is 1 and ZF is 0.

By issuing a `cmp` instruction and then checking ZF and SF, we can handle any comparison we'll implement in this section. That's not quite true since a - b could result in an integer overflow (or underflow), which would flip SF. Let's consider how that impacts each case:

1. If a == b, then a - b can't overflow because it's 0.
2. If a > b, then a - b could overflow when a is positive and b is negative. The correct result in this case is positive, but if it overflows, the result will be negative. In that case, SF and OF will be 1.
3. If a < b, then a - b could overflow when a is negative and b is positive. In this case, the correct result is negative, but if it underflows, the actual result will be positive. That means SF will be 0, but OF will be 1.

The below table gives the values of these flags in every case we've considered:

For signed integers a and b, impact of `cmp` instruction on status flags:

|                    | ZF  | OF  | SF  |
| ------------------ | --- | --- | --- |
| a == b             | 1   | 0   | 0   |
| a > b, no overflow | 0   | 0   | 0   |
| a > b, overflow    | 0   | 1   | 1   |
| a < b, no overflow | 0   | 0   | 1   |
| a < b, overflow    | 0   | 1   | 0   |

You can tell whether a or b is larger by checking whether SF and OF are the same. If they are, we know that a >= b. Either both (SF and OF) are zero, because we got a positive (or 0) result with no overflow, or both are 1, because we got a large positive result that overflowed until it became negative. a == b and a > b can then be demarcated by futher che If SF and OF are different, we know that a < b. Either we got a negative result with no underflow, or we got a negative result that underflowed and became positive.

Thus for

1. a == b: ZF = 1
2. a > b: ZF = 0 and OF = SF
3. a >= b: OF = SF
4. a < b: ZF = 0 and OF != SF
5. a <= b: OF != SF

# Undefined Behavior Alert!

If the `add` and `sub` instructions can result in signed integer overflow, why didn't we account for that before? We didn't need to because the signed integer overflow in C is undefined behavior, where the standard doesn't tell you what should happen. Compilers are permitted to handle undefined behavior however they want - or not handle it at all.

When an signed integer expression in C overflows, for examples, the result usually wraps around like the examples we saw earlier. However, it's equally acceptable for the program to generate a result at random, raise a signal, or erase your hard drive. That last option may sound unlikely, but production compilers really do handle undefined behavior in surprising (and arguably undesirable) ways. Take the following program:

```c

#include <stdio.h>

int main(void)
{
    for (int i = 2147483646; i > 0; i = i + 1)
    {
        printf ("The number is %d\n", i);
    }
    return 0;
}

```

The largest value an int can hold is 2,147,483,647, so the expression i + 1 overflows the second time we execute it. When the add assembly instruction overflows, it produces a negative result, so we might expect this loop to execute twice, then stop because the condition i > 0 no longer holds. That’s exactly what happens if you compile this program without optimizations, at least with the versions of Clang and GCC that I tried.

```bash

$ clang overflow.c
$ ./a.out
The number is 2147483646
The number is 2147483647

```

But if we enable optimizations, the behavior might change completely:

```bash

$ clang -O overflow.c
$ ./a.out
The number is 2147483646
The number is 2147483647
The number is -2147483648
The number is -2147483647
The number is -2147483646
The number is -2147483645
--snip--

```

What happened? The compiler tried to optimize the program by removing conditional checks that always succeed. The initial value of i is positive, and it’s updated only in the expression i = i + 1, so the compiler concluded that the condition i > 0 is always true. That’s correct, as long as i doesn’t overflow. It’s incorrect if i does overflow, of course, but that’s undefined behavior, so the compiler didn’t have to account for it. It therefore removed the condition entirely, resulting
in a loop that never terminates.

I used Clang for this example because GCC produced completely different, even less intuitive behavior. You may well see different results if you compile this program on your own machine. Try it out with a few different optimization levels, and see what happens.

Note that setting the overflow flag in assembly doesn't necessarily indicate overflow in the source program. For example, when we implement an expression like a < 10 with `cmp`, that `cmp` instruction may set the overflow flag. But the result of a < 10 is either 0 or 1 - both of which are in the range of `int` - so the expression itself doesn't overflow. This expression won't produce undefined behavior, regardless of how we implement it in assembly.

C has a bunch of different kinds of undefined behavior; integer overflow is just one example. It’s a particularly ugly example, though, because it’s difficult to avoid and can have dire consequences, including security vulnerabilities. To address this long-standing problem, the next version of the C standard, C23, adds a few standard library functions that perform checked integer operations. If you use the new ckd_add, ckd_sub, and ckd_mul functions instead of the +, -, and * operators, you’ll get an informative return code instead of undefined behavior when the result is out of bounds. To learn more about these new library functions, see Jens Gustedt’s blog post titled “Checked Integer Arithmetic in the Prospect of C23” (https://gustedt.wordpress.com/2022/12/18/checked -integer-arithmetic-in-the-prospect-of-c23/).

Undefined behavior is different from *unspecified behavior*. If some aspect of a program's behavior is unspecified, there are several possible ways it could behave, but it can't behave totally unpredictably. For example, before, we learned that the operands in a binary expression are unsequenced (or indeterminately sequenced, if either is a function call), so their evaluation order is unspecified. This doesn't mean the expression's behavior is undefined: there's just two possibilities (evaluating left operand before right or vice versa) and a compiler designer must chose which to implement. 

When we evaluate the expression printf("Hello, ") + printf("World!"), the program can print either "Hello, " or "World!" first, but it can’t go off and do something else entirely. Unsequenced operations can produce undefined behavior under certain circumstances—say, if you perform two unsequenced updates to the same variable—but performing unsequenced or indeterminately sequenced operations is not an undefined behavior in and of itself.

Unspecified behavior is a normal part of any C program. It's a problem only if your program relies on a particular behavior that the standard doesn't specify, like in the Hello, World! example. Undefined behavior, on the other hand, is always a problem; if it occurs anywhere in your program, you can't count on any part of the program to work correctly.

# Conditional Set Instructions

To implement a relational operator, we first set some flags using the `cmp` instruction, then set the result of the expression based on those flags. We perform that second step with a *conditional set* instruction. Each conditional set instruction takes a single register or memory address as an operand, which it sets to 0 or 1 based on the state of the RFLAGS. The conditional set instructions are all identical, except that they test for different conditions.

| Instruction | Meaning                                        | Flags                   |
| ----------- | ---------------------------------------------- | ----------------------- |
| sete        | Set byte if a == b                             | ZF is set               |
| setne       | Set byte if a != b                             | ZF not set              |
| setg        | Set byte if a > b for signed integers a and b  | ZF not set and SF == OF |
| setge       | Set byte if a >= b for signed integers a and b | SF == OF                |
| setl        | Set byte if a < b for signed integers a and b  | SF != OF and ZF not set |
| setle       | Set byte if a <= b for signed integers a and b | SF != OF                |

Unlike the other instructions we saw before, conditional set instructions take only 1-byte operands. Also, an instruction like `sete al` only affects the AL register, the least significant byte of RAX; the upper bytes of RAX remain untouched. To conditionally set the whole of RAX to 0 or 1, we need to zero out RAX before we set AL.

If it's operand is a memory address, a conditional set instruction will update the single byte at that address. Note that a memory address can be 1-byte, 4-byte, or 8-byte operand, depending on the context if it's clear through the context which size we are referring to; or otherwise through use of memory size specifiers in front of the memory operand (like word, qword, byte etc).

# Jump instructions

The `jmp` assembly instruction takes a label as an argument and performs an unconditional jump to that label. Jump assembly instructions manipulate another special-purpose register, RIP, which always holds the memory address of the next instruction to execute (IP stands for *instruction pointer*). To execute a sequence of instructions, the CPU carries out the *fetch-execute cycle*:

1. Fetch an instruction from the memory address in RIP and store it in a special-purpose *instruction register*. (This register doesn't have a name since we can't access it in assembly.)
2. Increment RIP to point to the next instruction. Instructions in x64 aren't all the same length, so the CPU has to check the length of the instruction it just fetched and increment RIP by that many bytes.
3. Run the instruction in the instruction register.
4. Repeat.

Normally, instructions are executed sequentially in the order they appear in memory. However, the `jmp` instruction modifies the RIP (Instruction Pointer) register, changing the address of the next instruction to execute. The assembler and linker convert the label in the `jmp` instruction into a *signed 32-bit relative offset*, relative to the current value of RIP at the time of execution. This offset indicates how much to increment or decrement RIP, a technique known as a RIP-relative jump. The relative offset is limited to a range of ±2 GB from the current value of RIP. In Position-Independent Code (PIC), if the target address of the jump exceeds this ±2 GB range, a Procedure Linkage Table (PLT) and Global Offset Table (GOT) approach is used. In this case, the label is resolved to a PLT entry, which will be within the ±2 GB range, ensuring that the jump is valid even for distant addresses.

Consider the snippet of assembly:

```asm
    add eax, 1
    jmp foo
    mov eax, 0

foo:
    ret
```

The machine instruction for `mov eax, 0` is 5 bytes long. To jump over it and execute the `ret` instruction instead, `jmp` needs to increment RIP by an extra 5 bytes. The assembler and linker therefore convert `jmp foo` into the machine instruction for `jmp rip+5`. Then, when the CPU executes this instruction, it:

1. Fetches the instruction `jmp rip+5` and stored it in the instruction register.
2. Increments RIP to point to the next instruction, `mov eax, 0`
3. Executes the jump instruction. This adds 5 bytes to the RIP so that it point to `ret`.
4. Fetches the instruction RIP points to, `ret`, and continues the fetch-execute cycle from there.

Note that labels aren't instructions: the CPU doesn't execute them, and they don't appear in the text section of the final executable (the section that contains the machine instructions).

A *conditional jump* takes a label as an argument but jumps to that label only if the condition holds. Conditional jumps look a lot like conditional set instructions; they depend on the same conditions, using the same flags in RFLAGS.

# Emitting Labels

Some assembly labels are autogenerated by the compiler, while others -- function names -- are user-defined identifiers. Because labels must be unique, autogenerated labels must not conflict with any function names that could appear in a program.

We'll avoid conflicts by adding a special *local label* prefix to our autogenerated labels. The local label prefix is `.L` on Linux and `L` on macOS. On Linux, these labels won't conflict with user-defined identifiers because identifiers in C can't contain periods. On macOS, they won't conflict because we prefix all user-defined names with underscores.

Local labels are handy for another reason: they won't confuse GDB or LLDB (LLVM debugger) when you need to debug this code. The assembler puts most labels in the object file's symbol table, but it leaves out any that start with the local label prefix. If your autogenerated labels were in the symbol table, GDB and LLDB would mistake them for function anmes, which could cause problems when you tried to disassemble a function or view a stack trace.


