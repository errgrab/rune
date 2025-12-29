# Glyph VM

A minimalist character-based register virtual machine inspired by [UXN](https://wiki.xxiivv.com/site/uxn.html).

## Philosophy

Glyph is a register-based VM with character opcodes, designed for:
- Manual programming by hand with memorable character-based instructions
- A fantasy console or game development platform
- Learning VM architecture and low-level programming
- Having fun with "wizard coding" - treating programming as spellcasting

The name "Glyph" comes from using ASCII characters as opcodes, turning programs into incantations.

## Architecture

- **128 32-bit registers**: r[0-31] general purpose, r[32-127] character-indexed
- **Special registers**: 
  - `r['.']` (PC) - Program Counter
  - `r[',']` (STK) - Stack Pointer
- **256 I/O ports**: For device communication
- **Memory**: Byte-addressable, size must be power-of-2 (recommended: 64KB)
- **Instruction format**: `[opcode] [arg1] [arg2] [arg3]`

## Instruction Set

All instructions use register operands (register indices as ASCII characters).

### Alchemy (Math Operations)
| Opcode | Format | Description |
|--------|--------|-------------|
| `+` | `+abc` | r[a] = r[b] + r[c] |
| `-` | `-abc` | r[a] = r[b] - r[c] |
| `*` | `*abc` | r[a] = r[b] * r[c] |
| `/` | `/abc` | r[a] = r[b] / r[c] (error on div-by-zero) |
| `%` | `%abc` | r[a] = r[b] % r[c] (error on div-by-zero) |

### Shaping (Bitwise Operations)
| Opcode | Format | Description |
|--------|--------|-------------|
| `&` | `&abc` | r[a] = r[b] & r[c] |
| `\|` | `\|abc` | r[a] = r[b] \| r[c] |
| `^` | `^abc` | r[a] = r[b] ^ r[c] |
| `~` | `~ab` | r[a] = ~r[b] |
| `<` | `<abc` | r[a] = r[b] << r[c] |
| `>` | `>abc` | r[a] = r[b] >> r[c] |

### Manifestation (Immediate Values)
| Opcode | Format | Description |
|--------|--------|-------------|
| `:` | `:a'N` | Load raw byte N into r[a] |
| `:` | `:adN` | Load decimal digit N into r[a] |
| `:` | `:axN` | Load hex digit N into r[a] |
| `:` | `:awNNNN` | Load 32-bit word (4 bytes, little-endian) |

### Substrate (Memory Operations)
| Opcode | Format | Description |
|--------|--------|-------------|
| `@` | `@ab` | r[a] = mem[r[b]] (load byte) |
| `!` | `!ab` | mem[r[a]] = r[b] (store byte) |

### Portals (I/O Operations)
| Opcode | Format | Description |
|--------|--------|-------------|
| `(` | `(ab` | r[a] = port[r[b]] (read from port) |
| `)` | `)ab` | port[r[a]] = r[b] (write to port) |

### Ritual (Control Flow)
| Opcode | Format | Description |
|--------|--------|-------------|
| `.` | `.a` | PC = r[a] (jump) |
| `?` | `?=ab` | Skip next if r[a] == r[b] |
| `?` | `?!ab` | Skip next if r[a] != r[b] |
| `?` | `?>ab` | Skip next if r[a] > r[b] |
| `?` | `?<ab` | Skip next if r[a] < r[b] |
| `;` | `;a` | Call: push PC, jump to r[a] |
| `,` | `,` | Return: pop PC |
| `\0` | - | Halt (end program) |

## Building

```bash
make all      # Build vm and test
make vm       # Build main executable
make test     # Build test suite
make debug    # Build with debug symbols
make clean    # Clean all build artifacts
```

## Usage

### Running Demos
```bash
./vm          # Run built-in demos
```

### API Usage
```c
#define G_IMPL
#include "glyph.h"

int main() {
    u8 memory[256];
    g vm;
    
    // Initialize VM
    g_init(&vm, memory, sizeof(memory));
    
    // Load program: :a'5 :b'3 +cab
    u8 prog[] = {
        ':', 'a', '\'', 5,
        ':', 'b', '\'', 3,
        '+', 'c', 'a', 'b',
        '\0'
    };
    g_load(&vm, prog, sizeof(prog));
    
    // Run program
    g_r(&vm);
    
    // Check result
    printf("Result: %u\n", vm.r['c']); // Outputs: 8
    
    return 0;
}
```

## Example Programs

### Fibonacci
```
:a'0 :b'1 :n'd8    ; fib(8)
; Loop: c=a+b, a=b, b=c
+cab :_'a :b'_ :a'c
; TODO: Add loop logic
```

### Memory Copy
```
:s'100 :d'200 :c'd16  ; source, dest, count
; Copy byte: @as !da
; TODO: Add loop
```

## Testing

Run the comprehensive test suite:
```bash
./test
```

Tests cover:
- All arithmetic operations
- Bitwise operations
- Memory read/write
- I/O ports
- Control flow (jumps, conditionals)
- Call/return
- Error handling (div-by-zero, invalid opcodes, bounds)

## Improvements Made

### Critical Fixes
- ✅ Fixed stack operations to properly store/load 4-byte return addresses
- ✅ Added PC bounds checking with error reporting
- ✅ Fixed memory length type from u8 to u32
- ✅ Fixed NEXT() macro to prevent side effects
- ✅ Added missing stdbool.h include

### Features Added
- ✅ I/O operations via port array
- ✅ Program loading API (g_init, g_load, g_reset)
- ✅ Error handling with error codes and messages
- ✅ Debug support (register dump, memory dump, trace mode)
- ✅ Comprehensive test suite

### Optimizations
- ✅ Replaced modulo with bitwise AND for address masking
- ✅ Improved build system with warnings and optimization flags
- ✅ Added debug/release configurations

### Code Quality
- ✅ Comprehensive documentation with opcode reference
- ✅ Improved code comments explaining all operations
- ✅ Example programs demonstrating features
- ✅ Clear error messages

## Error Codes

- `G_OK (0)`: No error
- `G_ERR_BOUNDS (1)`: PC out of bounds
- `G_ERR_DIV_ZERO (2)`: Division by zero
- `G_ERR_INVALID_OP (3)`: Invalid opcode

## Future Ideas

- Assembler for symbolic programming
- Interactive debugger/REPL
- More complex example programs
- Documentation for building games/demos
- Computed goto optimization for dispatch
- JIT compilation

## License

See project repository for license information.

---

*"Just don't forget to have fun."* - Original README
