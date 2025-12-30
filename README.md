# Glyph

Single-header character-based VM in ~100 lines of C.

## Build

```bash
make        # builds glyph console emulator and tests
./test      # runs tests
./glyph examples/hello.glyph   # prints "Hello, World!"
```

## Console Emulator

The `glyph` binary is a console emulator inspired by UXN, designed for bootstrapping languages.

```bash
# Run a program file
./glyph program.glyph [args...]

# Run inline code  
./glyph -e "<code>"

# Pipe input
echo "Hello" | ./glyph examples/echo.glyph
```

### Device Ports

All ports are 32-bit. Uppercase = device vector, lowercase = device ports.

**Console Device:**
| Port | Description |
|------|-------------|
| `'C'` | Console vector (input callback address) |
| `'c'` | Read: input character (set before callback) |
| `'o'` | Write: stdout |
| `'e'` | Write: stderr |

**System:**
| Port | Description |
|------|-------------|
| `'X'` | Exit with code |

**Input model (like UXN):**
1. Program runs to completion
2. For each stdin char: set `port['c']`, call vector
3. When stdin exhausted, program exits

Programs start at address 0x0100.

## Library Usage

```c
#define GLYPH_IMPL
#include "glyph.h"

uint8_t mem[256];
Glyph vm;

glyph_init(&vm, mem, sizeof(mem));
vm.emit = on_port_write;   // resonance out
vm.sense = on_port_read;   // resonance in
memcpy(mem, ":ax5 :bx3 +cab", 15);
glyph_run(&vm);
// vm.reg['c'] == 8
```

## Instructions

| Op | Args | Description |
|----|------|-------------|
| `+` `-` `*` `/` `%` | abc | a = b op c |
| `&` `\|` `^` `<` `>` | abc | bitwise |
| `~` | ab | a = ~b |
| `:` | agX | load glyph X |
| `:` | axN | load hex N |
| `:` | a.b | copy reg |
| `@` | ab | a = mem[b] |
| `!` | ab | mem[a] = b |
| `#` | <ab | a = port[b] |
| `#` | >ab | port[a] = b |
| `.` | a | jump to a |
| `?` | =bct | jump to t if b==c |
| `?` | !bct | jump to t if b!=c |
| `?` | >bct | jump to t if b>c |
| `?` | <bct | jump to t if b<c |
| `;` | a | call |
| `,` | | return |
| `\0` | | halt |

Whitespace is ignored. 128 registers (indexed by char). 256 ports.
