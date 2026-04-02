# holbertonschool-printf

> A custom reimplementation of the `printf` function from the C standard library, created as part of the **Holberton School** curriculum.

---

## Table of Contents

1. [Project Description](#1-project-description)
2. [Educational Context](#2-educational-context)
3. [Prerequisites and Environment](#3-prerequisites-and-environment)
4. [File Structure](#4-file-structure)
5. [The Header File `main.h`](#5-the-header-file-mainh)
6. [Detailed Analysis of `_printf.c`](#6-detailed-analysis-of-_printfc)
   - 6.1 [The `print_number` function](#61-the-print_number-function)
   - 6.2 [The `_printf` function](#62-the-_printf-function)
7. [Supported Format Specifiers](#7-supported-format-specifiers)
8. [Step-by-Step Algorithm](#8-step-by-step-algorithm)
9. [Edge Cases and Special Behaviors](#9-edge-cases-and-special-behaviors)
10. [Compilation](#10-compilation)
11. [Usage Examples](#11-usage-examples)
12. [Return Values](#12-return-values)
13. [Differences from Standard `printf`](#13-differences-from-standard-printf)
14. [Authors](#14-authors)

---

## 1. Project Description

`_printf` is a partial reimplementation of the `printf` function defined in `<stdio.h>` of the C standard library (libc). The goal is to understand and reproduce the internal mechanism for formatting and displaying text in C, **without using `printf` or `sprintf`**.

The function reads a format string character by character. When it encounters a `%` followed by a specifier (such as `c`, `s`, `d`, `i`, or `%`), it retrieves the corresponding argument from the variable argument list (`va_list`) and writes it to standard output (`stdout`, file descriptor 1) via the `write` system call.

---

## 2. Educational Context

This project is part of the **first-year** curriculum at Holberton School. Its goals are to:

- Understand **variable argument lists** in C (`stdarg.h`: `va_list`, `va_start`, `va_arg`, `va_end`).
- Understand the internal workings of formatted output.
- Master the low-level `write(2)` system call for writing to standard output.
- Practice **recursion** (used for displaying integers).
- Follow the coding standards enforced by Betty (Holberton code style).

---

## 3. Prerequisites and Environment

| Element | Details |
|---|---|
| Operating System | Ubuntu 20.04 LTS (or any Linux/Unix distribution) |
| Compiler | `gcc` version 9.x or higher |
| C Standard | C89/C90 (`-std=c89`) or C99 |
| Editor | `vi`, `vim`, `emacs`, or any text editor |
| Allowed Files | `_printf.c`, `main.h`, `README.md` |
| External Libraries | None (the C standard library is used only for `stdarg.h`, `unistd.h`, and `stddef.h`) |
| Standard `printf` function | **Forbidden** inside the implementation |

---

## 4. File Structure

```
holbertonschool-printf/
├── _printf.c   # Contains the implementation of _printf and print_number
├── main.h      # Header file: function prototypes and includes
└── README.md   # Complete project documentation
```

### `_printf.c`

This is the core of the project. This file contains two functions:

1. **`print_number(int n)`** — a recursive helper function that prints a signed integer to stdout.
2. **`_printf(const char *format, ...)`** — the main function that parses the format string and dispatches to the appropriate display routines.

### `main.h`

Header file that declares the prototypes for all project functions. It is protected by an **include guard** (`#ifndef MAIN_H` / `#define MAIN_H` / `#endif`) to prevent multiple inclusions.

---

## 5. The Header File `main.h`

```c
#ifndef MAIN_H
#define MAIN_H

#include <stdarg.h>
#include <stddef.h>

int _printf(const char *format, ...);
int print_number(int n);

#endif
```

### Line-by-Line Breakdown

| Line | Explanation |
|---|---|
| `#ifndef MAIN_H` | Checks whether the `MAIN_H` macro has not yet been defined (protection against multiple inclusions). |
| `#define MAIN_H` | Defines the `MAIN_H` macro to block any subsequent inclusion of the same file. |
| `#include <stdarg.h>` | Includes the header file for managing variable argument lists (`va_list`, `va_start`, `va_arg`, `va_end`). |
| `#include <stddef.h>` | Includes the header file defining `size_t`, `NULL`, and other fundamental types/macros. |
| `int _printf(const char *format, ...);` | Prototype of the main function. The `...` indicates a variadic argument list. |
| `int print_number(int n);` | Prototype of the integer display helper function. |
| `#endif` | Closes the `#ifndef` block. |

---

## 6. Detailed Analysis of `_printf.c`

### 6.1 The `print_number` function

```c
int print_number(int n)
{
    int count = 0;
    char c;

    if (n == -2147483648)
    {
        write(1, "-2147483648", 11);
        return 11;
    }

    if (n < 0)
    {
        write(1, "-", 1);
        count++;
        n = -n;
    }

    if (n / 10)
        count += print_number(n / 10);

    c = (n % 10) + '0';
    write(1, &c, 1);
    count++;

    return count;
}
```

#### Purpose of the function

This function prints a signed integer (`int`) to standard output and returns the number of characters written.

#### Why not use `sprintf` or `itoa`?

These functions are either forbidden in the context of this project or not portably available. The conversion of an integer to a string of characters is therefore done **manually**, digit by digit, using recursion.

#### Step-by-Step Explanation

**Step 1 — Special case for `INT_MIN` (-2,147,483,648)**

```c
if (n == -2147483648)
{
    write(1, "-2147483648", 11);
    return 11;
}
```

The `int` type on 32 bits represents values from `-2,147,483,648` to `+2,147,483,647`. The value `-2,147,483,648` is problematic because its opposite (`+2,147,483,648`) **does not fit** in an `int` (integer overflow, undefined behavior). This case is therefore handled separately by writing the string literal directly.

**Step 2 — Handling the negative sign**

```c
if (n < 0)
{
    write(1, "-", 1);
    count++;
    n = -n;
}
```

If the number is negative, the `-` character is written to stdout, the counter is incremented, then `n` is converted to its absolute value so the rest of the function works only with a positive integer.

**Step 3 — Recursion for digit ordering**

```c
if (n / 10)
    count += print_number(n / 10);
```

The central idea of the algorithm: to display a number like `1234`, one must print `1`, then `2`, then `3`, then `4` (left to right). However, successive divisions by 10 yield digits **right to left**. Recursion reverses this order:

- `print_number(1234)` calls `print_number(123)`
- `print_number(123)` calls `print_number(12)`
- `print_number(12)` calls `print_number(1)`
- `print_number(1)`: `1 / 10 == 0`, no recursion → prints `'1'`
- Returns to `print_number(12)` → prints `'2'`
- Returns to `print_number(123)` → prints `'3'`
- Returns to `print_number(1234)` → prints `'4'`

Final result: `1234` printed in the correct order.

**Step 4 — Printing the current digit**

```c
c = (n % 10) + '0';
write(1, &c, 1);
count++;
```

`n % 10` gives the units digit of `n` (an integer between 0 and 9). Adding `'0'` (whose ASCII value is 48) gives the corresponding ASCII character. For example: `3 + '0'` → `'3'` (ASCII 51). This character is then written to stdout with `write`.

**Step 5 — Returning the counter**

```c
return count;
```

The function returns the total number of characters written (including the `-` sign if applicable).

---

### 6.2 The `_printf` function

```c
int _printf(const char *format, ...)
{
    va_list args;
    int i = 0, count = 0;
    char *str;
    char c;

    if (format == NULL)
        return (-1);

    va_start(args, format);

    while (format[i])
    {
        if (format[i] == '%')
        {
            i++;
            if (format[i] == '\0')
            {
                va_end(args);
                return (-1);
            }

            if (format[i] == 'c')
            {
                c = va_arg(args, int);
                write(1, &c, 1);
                count++;
            }
            else if (format[i] == 's')
            {
                str = va_arg(args, char *);
                if (!str)
                    str = "(null)";
                while (*str)
                {
                    write(1, str, 1);
                    str++;
                    count++;
                }
            }
            else if (format[i] == '%')
            {
                write(1, "%", 1);
                count++;
            }
            else if (format[i] == 'd' || format[i] == 'i')
            {
                int num = va_arg(args, int);
                count += print_number(num);
            }
            else
            {
                write(1, "%", 1);
                write(1, &format[i], 1);
                count += 2;
            }
        }
        else
        {
            write(1, &format[i], 1);
            count++;
        }
        i++;
    }

    va_end(args);
    return (count);
}
```

#### Purpose of the function

`_printf` reproduces the basic behavior of `printf`: it reads a format string, interprets **conversion specifiers** (`%x` sequences), and prints the formatted result to standard output. It returns the total number of characters written, or `-1` on error.

#### Declaration and Signature

```c
int _printf(const char *format, ...)
```

- `const char *format` : pointer to the format string (read-only). It may contain ordinary text and conversion specifiers.
- `...` : variable argument list. Each conversion specifier in `format` consumes one argument from this list.
- Return value: `int` — number of characters printed, or `-1` on error.

#### Local Variables

| Variable | Type | Role |
|---|---|---|
| `args` | `va_list` | Manages the variable argument list. |
| `i` | `int` | Current index in the `format` string. |
| `count` | `int` | Counter of the total number of characters written. |
| `str` | `char *` | Temporary pointer for traversing a string (`%s`). |
| `c` | `char` | Temporarily stores the character to display (`%c`). |

#### Step 1 — Validating the `format` argument

```c
if (format == NULL)
    return (-1);
```

If the `format` pointer is `NULL`, the function immediately returns `-1` (behavior consistent with `printf` on some implementations). There is nothing to display, and attempting to dereference a `NULL` pointer would cause a segfault.

#### Step 2 — Initializing `va_list`

```c
va_start(args, format);
```

`va_start` initializes the `args` variable to point just past the last fixed parameter (`format`). From there, `va_arg` can be called to retrieve subsequent arguments one by one.

#### Step 3 — Main loop traversing the format string

```c
while (format[i])
{
    ...
    i++;
}
```

The loop advances character by character through `format`, until the null terminator (`'\0'`).

#### Step 4 — Branching on `%`

```c
if (format[i] == '%')
{
    i++;
    ...
}
```

When the current character is `%`, the index `i` is immediately advanced to examine the next character (the specifier).

#### Step 4a — Guard against a trailing `%`

```c
if (format[i] == '\0')
{
    va_end(args);
    return (-1);
}
```

If `%` is the **last character** of the format string (not followed by a specifier), the string is invalid. The function frees the `va_list` resources and returns `-1`.

#### Step 4b — `%c` specifier (character)

```c
if (format[i] == 'c')
{
    c = va_arg(args, int);
    write(1, &c, 1);
    count++;
}
```

`va_arg(args, int)` retrieves the next argument as an `int` (short integer types are promoted to `int` when passed in a variadic list — this is the **default argument promotion** rule in C). The value is implicitly cast to `char` on assignment. This single character is written to stdout.

#### Step 4c — `%s` specifier (string)

```c
else if (format[i] == 's')
{
    str = va_arg(args, char *);
    if (!str)
        str = "(null)";
    while (*str)
    {
        write(1, str, 1);
        str++;
        count++;
    }
}
```

A `char *` is retrieved. If the pointer is `NULL`, it is replaced by the literal string `"(null)"` (the behavior adopted by GNU libc `printf`). The string is then traversed character by character until the terminal `'\0'`, writing each character to stdout.

#### Step 4d — `%%` specifier (literal `%` character)

```c
else if (format[i] == '%')
{
    write(1, "%", 1);
    count++;
}
```

The `%%` sequence in a format string represents a literal `%`. We simply write `%` to stdout.

#### Step 4e — `%d` and `%i` specifiers (decimal integers)

```c
else if (format[i] == 'd' || format[i] == 'i')
{
    int num = va_arg(args, int);
    count += print_number(num);
}
```

An `int` is retrieved and display is delegated to `print_number`. Both `%d` and `%i` specifiers are equivalent in output mode (both display a signed decimal integer). The difference between them only exists in input (`scanf`), where `%i` accepts octal and hexadecimal notations.

#### Step 4f — Unknown specifier

```c
else
{
    write(1, "%", 1);
    write(1, &format[i], 1);
    count += 2;
}
```

If the character following `%` is not recognized, the function reproduces GNU `printf` behavior: it prints the `%` followed by the unrecognized character as-is, and counts 2 characters.

#### Step 5 — Ordinary character (not `%`)

```c
else
{
    write(1, &format[i], 1);
    count++;
}
```

Any character that is not `%` is printed directly to stdout as-is.

#### Step 6 — Freeing `va_list` and returning

```c
va_end(args);
return (count);
```

`va_end` frees the resources associated with `args` (mandatory after `va_start`). The function returns the total number of characters written.

---

## 7. Supported Format Specifiers

| Specifier | Expected Argument | Description | Format Example | Output Example |
|---|---|---|---|---|
| `%c` | `int` (promoted from `char`) | Prints a single character. | `_printf("%c", 'A')` | `A` |
| `%s` | `char *` | Prints a null-terminated string. If the pointer is `NULL`, prints `(null)`. | `_printf("%s", "hello")` | `hello` |
| `%d` | `int` | Prints a signed decimal integer. | `_printf("%d", -42)` | `-42` |
| `%i` | `int` | Same as `%d`. Prints a signed decimal integer. | `_printf("%i", 100)` | `100` |
| `%%` | *(no argument)* | Prints the literal `%` character. | `_printf("100%%")` | `100%` |

---

## 8. Step-by-Step Algorithm

Here is the complete execution trace of `_printf("Hello, %s! You are %d years old.\n", "Alice", 30)`:

```
format = "Hello, %s! You are %d years old.\n"
arguments: "Alice", 30
```

| Step | `i` | `format[i]` | Action | `count` |
|---|---|---|---|---|
| 1 | 0 | `H` | Write `H` | 1 |
| 2 | 1 | `e` | Write `e` | 2 |
| 3 | 2 | `l` | Write `l` | 3 |
| 4 | 3 | `l` | Write `l` | 4 |
| 5 | 4 | `o` | Write `o` | 5 |
| 6 | 5 | `,` | Write `,` | 6 |
| 7 | 6 | ` ` | Write ` ` | 7 |
| 8 | 7 | `%` | Detect `%`, advance `i` → 8 | 7 |
| 9 | 8 | `s` | Specifier `%s`: retrieve `"Alice"`, write `A`, `l`, `i`, `c`, `e` | 12 |
| 10 | 9 | `!` | Write `!` | 13 |
| 11 | 10 | ` ` | Write ` ` | 14 |
| ... | ... | ... | ... (same for `You are `) | 22 |
| 12 | 18 | `%` | Detect `%`, advance `i` → 19 | 22 |
| 13 | 19 | `d` | Specifier `%d`: retrieve `30`, call `print_number(30)`, write `3`, `0` | 24 |
| ... | ... | ... | ... (same for ` years old.\n`) | 37 |

**Return value: 37**

---

## 9. Edge Cases and Special Behaviors

### 9.1 `format` is `NULL`

```c
_printf(NULL);
// Returns: -1
// Nothing is printed
```

The function protects against a `NULL` pointer at the start and returns `-1`.

### 9.2 `%` at end of string (without specifier)

```c
_printf("Hello%");
// Returns: -1
// Only "Hello" would have been printed before encountering the trailing %
// Note: the function actually returns -1 without having printed "Hello"
// because the detection happens during traversal; characters before % are
// already written to stdout by the time the final condition is detected.
```

More precisely, characters **before** the trailing `%` have already been written to stdout by the time the error is detected. The return value is `-1`.

### 9.3 `NULL` string for `%s`

```c
_printf("%s", NULL);
// Prints: (null)
// Returns: 6
```

If the argument for `%s` is a `NULL` pointer, the function prints the string `"(null)"` (6 characters) instead of causing a segfault.

### 9.4 `INT_MIN` integer for `%d` / `%i`

```c
_printf("%d", -2147483648);
// Prints: -2147483648
// Returns: 11
```

The value `INT_MIN` is handled as a special case in `print_number` to avoid integer overflow during negation.

### 9.5 Unknown specifier

```c
_printf("%x", 255);
// Prints: %x
// Returns: 2
```

An unsupported specifier is printed as-is (the `%` followed by the unknown character).

### 9.6 Empty format string

```c
_printf("");
// Prints: (nothing)
// Returns: 0
```

The loop does not execute, `count` remains 0.

### 9.7 `%%` (escaping the `%` symbol)

```c
_printf("100%%");
// Prints: 100%
// Returns: 4
```

The `%%` sequence is interpreted as a single literal `%`.

---

## 10. Compilation

### Standard Compilation

To compile the project with a test file `main.c`:

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 _printf.c main.c -o test_printf
```

| Option | Meaning |
|---|---|
| `-Wall` | Enables all common warnings. |
| `-Werror` | Treats warnings as errors. |
| `-Wextra` | Enables additional warnings. |
| `-pedantic` | Enforces strict C standard conformance. |
| `-std=gnu89` | Uses the C89 standard with GNU extensions (the standard used by Holberton). |

### Sample `main.c` for testing

```c
#include "main.h"
#include <stdio.h>

int main(void)
{
    int len;
    int len2;

    len = _printf("Let's try to printf a simple sentence.\n");
    len2 = printf("Let's try to printf a simple sentence.\n");
    _printf("Length: [%d, %i]\n", len, len);
    printf("Length: [%d, %i]\n", len2, len2);

    _printf("Negative: [%d]\n", -762534);
    printf("Negative: [%d]\n", -762534);

    _printf("Character: [%c]\n", 'H');
    printf("Character: [%c]\n", 'H');

    _printf("String: [%s]\n", "I am a string !");
    printf("String: [%s]\n", "I am a string !");

    _printf("Percent: [%%]\n");
    printf("Percent: [%%]\n");

    _printf("Unknown: [%r]\n");
    printf("Unknown: [%r]\n");

    return (0);
}
```

### Execution

```bash
./test_printf
```

---

## 11. Usage Examples

### Printing a simple string

```c
_printf("Hello, world!\n");
// Output: Hello, world!
// Return: 14
```

### Printing a character

```c
_printf("First letter: %c\n", 'A');
// Output: First letter: A
// Return: 16
```

### Printing a string

```c
_printf("Name: %s\n", "Alice");
// Output: Name: Alice
// Return: 12
```

### Printing a positive integer

```c
_printf("Value: %d\n", 42);
// Output: Value: 42
// Return: 10
```

### Printing a negative integer

```c
_printf("Temperature: %d degrees\n", -15);
// Output: Temperature: -15 degrees
// Return: 25
```

### Combining multiple specifiers

```c
_printf("%s is %d years old and %d cm tall.\n", "Bob", 25, 180);
// Output: Bob is 25 years old and 180 cm tall.
// Return: 37
```

### Printing the `%` character

```c
_printf("Success rate: 87%%\n");
// Output: Success rate: 87%
// Return: 19
```

### NULL string for `%s`

```c
_printf("Value: %s\n", NULL);
// Output: Value: (null)
// Return: 13
```

### `INT_MIN` value

```c
_printf("Min int: %d\n", -2147483648);
// Output: Min int: -2147483648
// Return: 21
```

---

## 12. Return Values

| Situation | Returned Value |
|---|---|
| Success | Number of characters written to stdout (≥ 0) |
| `format == NULL` | `-1` |
| `%` followed by `'\0'` (end of string) | `-1` (already written characters are not undone) |

---

## 13. Differences from Standard `printf`

| Feature | `printf` (libc) | `_printf` (this project) |
|---|---|---|
| `%c` | ✅ Supported | ✅ Supported |
| `%s` | ✅ Supported | ✅ Supported |
| `%d` | ✅ Supported | ✅ Supported |
| `%i` | ✅ Supported | ✅ Supported |
| `%%` | ✅ Supported | ✅ Supported |
| `%u` (unsigned int) | ✅ Supported | ❌ Not supported |
| `%o` (octal) | ✅ Supported | ❌ Not supported |
| `%x` / `%X` (hexadecimal) | ✅ Supported | ❌ Not supported (printed as-is) |
| `%f` (float) | ✅ Supported | ❌ Not supported |
| `%e` / `%E` (scientific notation) | ✅ Supported | ❌ Not supported |
| `%p` (pointer) | ✅ Supported | ❌ Not supported |
| `%n` (write counter) | ✅ Supported | ❌ Not supported |
| Flags (`-`, `+`, `0`, ` `, `#`) | ✅ Supported | ❌ Not supported |
| Field width (e.g., `%10d`) | ✅ Supported | ❌ Not supported |
| Precision (e.g., `%.5s`) | ✅ Supported | ❌ Not supported |
| Length modifiers (`l`, `h`, etc.) | ✅ Supported | ❌ Not supported |
| Output buffering | ✅ (FILE * with buffer) | ❌ (direct write, no buffer) |

> **Note on buffering**: `printf` uses an internal buffer and only writes to stdout when the buffer is full or a `\n` is encountered (in line-buffered mode). `_printf` uses the `write` system call directly for each character, which is less efficient but simpler to implement. In a context where performance is not critical (educational project), this is acceptable.

---

## 14. Authors

This project was created as part of the **Holberton School** curriculum by:

- **Mataius-web** — [GitHub](https://github.com/Mataius-web)

---

*Holberton School — `_printf` Project — All rights reserved.*