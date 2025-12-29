# Glyph VM - Formal Specification v1.0

## 1. Overview

Glyph is a 32-bit register-based virtual machine with character-encoded opcodes. It is designed for manual programming, educational purposes, and as a foundation for fantasy consoles.

## 2. Machine State

### 2.1 Registers
- **Count**: 128 registers (r[0] through r[127])
- **Width**: 32 bits unsigned per register
- **Indexing**: Registers are addressed by their ASCII value
  - r[0-31]: General purpose (non-printable ASCII)
  - r[32-127]: Character-indexed (printable ASCII)

### 2.2 Special Registers
- **r[46] (r['.'])**: Program Counter (PC)
  - Points to next instruction to execute
  - Automatically incremented during instruction fetch
  - Range: [0, memory_length)
  
- **r[44] (r[','])**: Stack Pointer (STK)
  - Points to top of call stack
  - Grows downward (decrements on push)
  - Initially 0

### 2.3 Memory
- **Layout**: Flat, byte-addressable memory space
- **Size**: Must be a power of 2 (recommended: 65536 bytes)
- **Access**: Wraps using bitwise AND: `address & (size - 1)`
- **Endianness**: Little-endian for multi-byte values

### 2.4 I/O Ports
- **Count**: 256 ports (p[0] through p[255])
- **Width**: 32 bits unsigned per port
- **Purpose**: Memory-mapped I/O for device communication

### 2.5 Status Flags
- **halt**: Boolean, stops execution when true
- **err**: Unsigned 32-bit error code
- **trace**: Boolean, enables debug output (implementation-defined)

## 3. Execution Model

### 3.1 Instruction Cycle
1. Check halt flag; if true, stop
2. Fetch opcode byte from memory[PC], increment PC
3. Check PC bounds; if exceeded, set error and halt
4. Decode opcode
5. Fetch operands (0-3 bytes depending on opcode)
6. Execute operation
7. Handle errors if any
8. Repeat from step 1

### 3.2 Operand Encoding
- **Register operands**: Single byte (ASCII value serves as register index)
- **Immediate values**: Encoded with ':' prefix (see section 4.5)

### 3.3 Error Handling
When an error occurs:
1. Set `err` field to error code
2. Set `halt` flag to true
3. Stop execution immediately

Error codes:
- `0`: No error
- `1`: PC out of bounds
- `2`: Division by zero
- `3`: Invalid opcode

## 4. Instruction Set Architecture

### 4.1 Instruction Format
```
[opcode:1 byte] [operand1:1 byte] [operand2:1 byte] [operand3:1 byte]
```
- Opcode determines number of operands (1-4 total bytes per instruction)
- Operands are register indices (ASCII characters)

### 4.2 Arithmetic Operations (Alchemy)

#### ADD: '+'  (ASCII 43)
- **Format**: `+ a b c` (4 bytes)
- **Semantics**: `r[a] ← r[b] + r[c]`
- **Overflow**: Wraps (modulo 2^32)

#### SUB: '-'  (ASCII 45)
- **Format**: `- a b c` (4 bytes)
- **Semantics**: `r[a] ← r[b] - r[c]`
- **Underflow**: Wraps (modulo 2^32)

#### MUL: '*'  (ASCII 42)
- **Format**: `* a b c` (4 bytes)
- **Semantics**: `r[a] ← r[b] × r[c]`
- **Overflow**: Wraps (modulo 2^32)

#### DIV: '/'  (ASCII 47)
- **Format**: `/ a b c` (4 bytes)
- **Semantics**: `r[a] ← r[b] ÷ r[c]` (unsigned integer division)
- **Error**: If `r[c] = 0`, set err=2, halt
- **Rounding**: Truncates toward zero

#### MOD: '%'  (ASCII 37)
- **Format**: `% a b c` (4 bytes)
- **Semantics**: `r[a] ← r[b] mod r[c]`
- **Error**: If `r[c] = 0`, set err=2, halt

### 4.3 Bitwise Operations (Shaping)

#### AND: '&'  (ASCII 38)
- **Format**: `& a b c` (4 bytes)
- **Semantics**: `r[a] ← r[b] & r[c]` (bitwise AND)

#### OR: '|'  (ASCII 124)
- **Format**: `| a b c` (4 bytes)
- **Semantics**: `r[a] ← r[b] | r[c]` (bitwise OR)

#### XOR: '^'  (ASCII 94)
- **Format**: `^ a b c` (4 bytes)
- **Semantics**: `r[a] ← r[b] ⊕ r[c]` (bitwise XOR)

#### NOT: '~'  (ASCII 126)
- **Format**: `~ a b` (3 bytes)
- **Semantics**: `r[a] ← ¬r[b]` (bitwise NOT)

#### LSH: '<'  (ASCII 60)
- **Format**: `< a b c` (4 bytes)
- **Semantics**: `r[a] ← r[b] << r[c]` (left shift)
- **Behavior**: Zeros shifted in from right

#### RSH: '>'  (ASCII 62)
- **Format**: `> a b c` (4 bytes)
- **Semantics**: `r[a] ← r[b] >> r[c]` (logical right shift)
- **Behavior**: Zeros shifted in from left

### 4.4 Memory Operations (Substrate)

#### LOAD: '@'  (ASCII 64)
- **Format**: `@ a b` (3 bytes)
- **Semantics**: `r[a] ← memory[r[b] & (size-1)]`
- **Width**: Loads 1 byte, zero-extends to 32 bits

#### STORE: '!'  (ASCII 33)
- **Format**: `! a b` (3 bytes)
- **Semantics**: `memory[r[a] & (size-1)] ← r[b] & 0xFF`
- **Width**: Stores lowest 8 bits of r[b]

### 4.5 Immediate Operations (Manifestation)

All immediate operations use the ':' prefix (ASCII 58).

#### Format: `: a m [data]`
- **Opcode**: ':' (1 byte)
- **Destination**: a (1 byte, register index)
- **Mode**: m (1 byte, determines interpretation)
- **Data**: 1-4 bytes depending on mode

#### Modes:

**'\'' (ASCII 39) - Raw Byte**
- **Format**: `: a ' N` (4 bytes)
- **Semantics**: `r[a] ← N` (N is unsigned byte)

**'d' (ASCII 100) - Decimal Digit**
- **Format**: `: a d N` (4 bytes)
- **Semantics**: `r[a] ← N - 48` (converts ASCII digit to value)
- **Valid**: N must be ASCII '0'-'9' (behavior undefined otherwise)

**'x' (ASCII 120) - Hexadecimal Digit**
- **Format**: `: a x N` (4 bytes)
- **Semantics**: `r[a] ← hex_value(N)`
  - If N ∈ ['0','9']: value = N - 48
  - If N ∈ ['A','F'] or ['a','f']: value = (N & 0xDF) - 55
- **Valid**: N must be ASCII '0'-'9', 'A'-'F', 'a'-'f'

**'w' (ASCII 119) - Word (32-bit)**
- **Format**: `: a w B0 B1 B2 B3` (7 bytes)
- **Semantics**: `r[a] ← B0 | (B1 << 8) | (B2 << 16) | (B3 << 24)`
- **Endianness**: Little-endian (B0 is LSB)

### 4.6 I/O Operations (Portals)

#### INPUT: '('  (ASCII 40)
- **Format**: `( a b` (3 bytes)
- **Semantics**: `r[a] ← port[r[b] & 0xFF]`
- **Port range**: Only lowest 8 bits of r[b] used

#### OUTPUT: ')'  (ASCII 41)
- **Format**: `) a b` (3 bytes)
- **Semantics**: `port[r[a] & 0xFF] ← r[b]`
- **Port range**: Only lowest 8 bits of r[a] used

### 4.7 Control Flow Operations (Ritual)

#### JUMP: '.'  (ASCII 46)
- **Format**: `. a` (2 bytes)
- **Semantics**: `PC ← r[a]`
- **Bounds**: Next fetch will check if PC < size

#### SKIP: '?'  (ASCII 63)
Conditional skip of next byte.

**Format**: `? cond b c` (4 bytes)

**Condition codes**:
- **'='** (ASCII 61): Skip if `r[b] = r[c]`
- **'!'** (ASCII 33): Skip if `r[b] ≠ r[c]`
- **'>'** (ASCII 62): Skip if `r[b] > r[c]` (unsigned)
- **'<'** (ASCII 60): Skip if `r[b] < r[c]` (unsigned)

**Semantics**: 
```
if condition(r[b], r[c]) then
    PC ← PC (no change, executes next)
else
    PC ← PC + 1 (skips next byte)
```

#### CALL: ';'  (ASCII 59)
- **Format**: `; a` (2 bytes)
- **Semantics**:
  1. `STK ← (STK - 4) & (size - 1)`
  2. `memory[STK..STK+3] ← PC` (little-endian)
  3. `PC ← r[a]`

**Stack format**: Stores 4-byte return address

#### RETURN: ','  (ASCII 44)
- **Format**: `,` (1 byte)
- **Semantics**:
  1. `PC ← word_at(memory[STK])` (little-endian load)
  2. `STK ← (STK + 4) & (size - 1)`

#### HALT: '\0'  (ASCII 0)
- **Format**: `\0` (1 byte)
- **Semantics**: 
  1. `halt ← true`
  2. Stop execution

### 4.8 Invalid Opcodes
Any byte value not listed above is invalid.

**Behavior**:
1. Set `err ← 3`
2. Set `halt ← true`
3. Stop execution

## 5. Initial State

When a VM is initialized:
- All registers: `r[i] ← 0` for i ∈ [0, 127]
- All ports: `p[i] ← 0` for i ∈ [0, 255]
- All memory: `memory[i] ← 0` for i ∈ [0, size)
- Status: `halt ← false`, `err ← 0`, `trace ← false`
- PC: `r['.'] ← 0`
- STK: `r[','] ← 0`

## 6. Program Loading

To load a program:
1. Copy program bytes to memory starting at address 0
2. Set `PC ← 0`
3. Set `halt ← false`
4. Set `err ← 0`

## 7. Execution

To run a program:
```
while halt = false do
    execute_one_instruction()
end
return err
```

## 8. Implementation Requirements

### 8.1 Mandatory Features
- All 26 opcodes must be implemented
- Error handling for division by zero, invalid opcodes, and bounds
- Proper little-endian encoding for multi-byte values
- Stack operations must correctly store/load 4-byte addresses

### 8.2 Optional Features
- Trace mode for debugging
- Register/memory dump utilities
- Single-step execution
- Breakpoint support

### 8.3 Memory Constraints
- Memory size must be power of 2 for address masking
- Minimum: 256 bytes
- Recommended: 65536 bytes (64KB)

## 9. Test Cases

A conforming implementation must pass:
1. All arithmetic operations (including overflow)
2. All bitwise operations
3. Memory load/store with wrapping
4. All immediate load modes
5. I/O port operations
6. Jump and conditional skip
7. Call/return with proper stack management
8. Error conditions (div-by-zero, invalid opcode, bounds)

## 10. Example Program

Simple addition:
```
Offset  Bytes         Assembly      Description
------  -----         --------      -----------
0x00    : a ' 5       :a'5          Load 5 into r['a']
0x04    : b ' 3       :b'3          Load 3 into r['b']
0x08    + c a b       +cab          r['c'] = r['a'] + r['b']
0x0C    \0            \0            Halt

Result: r['c'] = 8
```

## 11. Formal Properties

### 11.1 Determinism
Given identical initial state and program, execution produces identical results.

### 11.2 Termination
Programs may not terminate (infinite loops possible).

### 11.3 Memory Safety
All memory accesses are bounds-checked via masking: `addr & (size-1)`.
Out-of-bounds PC access results in immediate halt with error.

### 11.4 Type Safety
All values are untyped 32-bit unsigned integers.
No type checking or coercion occurs.

---

**Specification Version**: 1.0  
**Date**: 2025-12-29  
**Status**: Stable
