# Star Language

## About The Project

Star Language is a custom, interpreted programming language designed to handle simple arithmetic and text operations. It provides a structured syntax for variable declarations, operations, loops, and user input/output, processed by a custom lexical analyzer and interpreter written in C.

## Features

*   **Variable Declarations**: Supports `int` and `text` (string) data types.
*   **Arithmetic Operations**: Performs addition (+), subtraction (-), multiplication (*), and division (/).
*   **String Manipulation**: Supports concatenation (+) and substring removal (-).
*   **Looping**: Implements a `loop` mechanism for repeated execution of code blocks.
*   **I/O Operations**: Includes `read` for user input and `write` for console output.
*   **Lexical Analysis**: A custom lexer tokenizes the source code before interpretation.

## Getting Started

To run the Star Language interpreter, you need a C compiler (like GCC).

1.  **Clone the repository:**
    ```sh
    git clone https://github.com/kahramanemir/Star-Language.git
    cd Star-Language
    ```
2.  **Compile the interpreter:**
    ```sh
    gcc Codes/Interpreter.c -o interpreter
    ```
3.  **Run the interpreter:**

    The interpreter is hardcoded to read a file named `test.sta` in the same directory.
    ```sh
    ./interpreter
    ```
    The interpreter will execute the code in `Codes/test.sta` and print the output to your console. It also generates a lexical analysis output file named `output.lex`.

## Syntax & Usage

Star Language uses a simple, keyword-based syntax. Statements are terminated with a period (`.`).

### Variable Declaration & Assignment

Declare variables using `int` or `text`, followed by the variable name, the `is` keyword, and a value.

```star
/* Declare and initialize integers */
int a is 5, b is 10.

/* Declare and initialize strings */
text message is "Hello, World!".

/* Re-assign a new value to an existing variable */
a is 15.
```

### Arithmetic Operations

Perform standard arithmetic operations on integers.

```star
int x is 5 + 10. /* x becomes 15 */
int y is 10 - 5. /* y becomes 5 */
int z is x * 2.  /* z becomes 30 */
```

### String Operations

Concatenate strings with `+` or remove the first occurrence of a substring with `-`.

```star
text greeting is "Hello" + " World".
write greeting. /* Output: Hello World */

text phrase is "ice ice baby".
phrase is phrase - "ice ".
write phrase. /* Output: ice baby */
```

### Control Flow: Loops

Use the `loop` keyword to repeat a block of code a specific number of times.

```star
/* Simple loop */
loop 5 times {
    write "*".
}
/* Output: ***** */

/* Loop using a variable */
int count is 3.
loop count times {
    write "Hello".
    newLine.
}
```

### Input & Output

Interact with the user via the console.

```star
/* Print text and variables */
int score is 100.
write "Your score is: ", score.
newLine.

/* Read user input */
text name.
read "Enter your name: ", name.
write "Hello, ", name.
```

## Limitations

*   **Negative Numbers**: Negative integers resulting from operations default to `0`. Declaring a negative integer literal will also result in `0`.
*   **Nested Loops**: Nested loops are only supported if the inner loop is a single line (not enclosed in `{}`).
*   **Integer Size**: Integers are limited to a maximum value of `99999999`.
*   **Identifier Length**: Variable names are limited to 10 characters.
*   **String Length**: Strings are limited to a maximum length of 256 characters.

## Contributors

*   Emir Kahraman
*   Bülen Yıldırım
*   Alp Kutay Köksal

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
