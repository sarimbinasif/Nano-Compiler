# NanoScript Compiler

A complete three-phase compiler for **NanoScript**, a statically-typed scripting language built with C++.

## Overview

This compiler implements:
- **Scanner** (Lexical Analysis) - Tokenizes source code
- **Parser** (Syntax Analysis) - Builds Abstract Syntax Tree (AST)
- **Semantic Analyzer** - Type checking and scope validation

## Language Features

### Keywords
`var` `int` `float` `bool` `if` `else` `while` `func` `return` `print` `and` `or` `true` `false`

### Data Types
- `int` - 32-bit signed integers
- `float` - 64-bit floating-point numbers
- `bool` - Boolean values (true/false)

### Operators
- **Arithmetic**: `+` `-` `*` `/`
- **Comparison**: `>` `<` `>=` `<=` `==` `!=`
- **Logical**: `and` `or` `!`
- **Assignment**: `=`

### Supported Constructs
- Variable declarations: `var int x = 10;`
- Functions: `func int add(int a, int b) { return a + b; }`
- Conditionals: `if (condition) { ... } else { ... }`
- Loops: `while (condition) { ... }`
- Function calls: `add(5, 10)`
- Print statements: `print(x);`

## Example Code

```nanoscript
// Factorial function
var int N = 5;

func int factorial(int n) {
    if (n == 0) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

var int result = factorial(N);
print(result);
```

## Compilation & Usage

### Compile the Compiler (Linux/WSL)
```bash
g++ -std=c++17 src/main.cpp src/compiler/*.cpp -o nano-compiler
```

### Run NanoScript Files
```bash
./nano-compiler <script.ns>
```

### Example
```bash
./nano-compiler tests/test-scanner.ns
./nano-compiler tests/test-parser.ns
./nano-compiler tests/test-semantic.ns
./nano-compiler tests/test-factorial.ns
```

### Expected Output
```
--- Compiling tests/test-scanner.ns ---
[Phase 1] Scanning...
[Phase 2] Parsing...
[Phase 3] Semantic Analysis...
Success! Valid NanoScript code.
```

## Project Structure

```
nano-compiler/
├── src/
│   ├── main.cpp                    # Entry point
│   ├── compiler/
│   │   ├── scanner.cpp/.h          # Lexical analyzer
│   │   ├── parser.cpp/.h           # Syntax analyzer
│   │   └── semantic-analyzer.cpp/.h # Semantic analyzer
│   ├── data/
│   │   ├── token.h                 # Token structure
│   │   ├── token-type.h            # Token types enum
│   │   └── AST.h                   # AST node definitions
│   └── util/
│       ├── error-handler.h         # Error reporting
│       └── symbol-table.h          # Scope & variable tracking
└── tests/
    ├── test-scanner.ns             # Scanner tests
    ├── test-parser.ns              # Parser tests
    ├── test-semantic.ns            # Semantic tests
    └── test-factorial.ns           # Factorial example
```

## Compiler Phases

### Phase 1: Scanner
- Reads source code character by character
- Produces tokens (keywords, identifiers, operators, literals)
- Handles comments (`//`)

### Phase 2: Parser
- Recursive descent parser
- Validates syntax against grammar rules
- Builds Abstract Syntax Tree (AST)

### Phase 3: Semantic Analyzer
- Type checking (prevents invalid operations like `bool + int`)
- Scope resolution (tracks variable declarations)
- Function return type validation
- Implicit type conversions (int → float allowed)

## Error Handling

The compiler reports errors with line numbers:
```
[line 12] Error: Cannot compare incompatible types.
[line 15] Error: Undefined variable 'x'.
Compilation failed with semantic errors.
```

Exit codes:
- `0` - Success
- `1` - File error or invalid usage
- `65` - Scanner or Parser error
- `70` - Semantic error

## Language Rules

1. Variables must be declared before use
2. Variables must be initialized at declaration
3. Type mismatches are errors (except int → float)
4. Functions must specify return type and parameter types
5. Return statements must match function return type
6. Conditions (if/while) must evaluate to `bool`
7. Arithmetic operators work on `int` and `float` only
8. Logical operators work on `bool` only

## Authors
Sarim Asif
Mohammad Anas
Abdullah Farooqui
Anas Hussain