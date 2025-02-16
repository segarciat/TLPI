# Exercise 06-02

Write a program to see what happens if we try to `longjmp()` into a function
that has already returned.

## Solution

In my implementation, the simple setup I had was:

1. `main` calls a function.
2. The called function sets the jump location with `setjmp` using
the file-scoped *env* object of type `jmp_buf`, and immediately returns.
3. `main` proceeds to call a different function.
4. The function uses `longjmp` to make the jump, does not execute any code after the jump.
5. The function that was the destination of the jump does in fact execute, resuming at the marked location. Any variables declared before that `setjmp()` in that function are not assigned, so they contain trash.
6. `main` resumes, and eventually ends.

Therefore, making a long jump to a function that has already returned results in it executing
with variables before the marked location containing trash.

The book describes that the actual behavior depends on the state of the stack,
and it can result in a program crash, an infinite call-return loop, or the program
behaving as if it did return from function that was not currently executing.
The TLPI source code destribution includes listing `proc/bad_longjmp.c` that
includes one more nesting level, and in that case, the result is a segmentation fault.
