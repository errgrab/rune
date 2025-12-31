# Glyph

A runic virtual machine. ~100 lines of C.

## Quickstart

```bash
make
./glyph examples/hello.glyph
```

## The Art of Inscription

Glyph programs are sequences of **runes** — single characters that command the machine. Each rune draws power from **vessels** (registers named by any character) and can reach into the **void** (memory) or connect to **laylines** (ports) that resonate with the outside world.

### Vessels

128 vessels hold your values, each named by a character. `a` through `z`, `A` through `Z`, `0` through `9`, even `!` or `?` — any glyph can name a vessel.

```
:0a5      ← inscribe 5 into vessel 'a'
:0bF      ← inscribe 15 into vessel 'b'  
+'cab     ← vessel 'c' receives the sum of 'a' and 'b'
```

### Arithmetic Runes

The basic runes of transformation:

| Rune | Meaning |
|------|---------|
| `+abc` | a ← b + c |
| `-abc` | a ← b − c |
| `*abc` | a ← b × c |
| `/abc` | a ← b ÷ c |
| `%abc` | a ← b mod c |
| `&abc` | a ← b AND c |
| `\|abc` | a ← b OR c |
| `^abc` | a ← b XOR c |
| `<abc` | a ← b shifted left by c |
| `>abc` | a ← b shifted right by c |
| `~ab` | a ← NOT b |

### The Colon `:` — Rune of Inscription

The colon is polymorphic. Its second glyph determines its nature:

| Form | Effect |
|------|--------|
| `:'aX` | Inscribe the literal glyph 'X' into vessel 'a' |
| `:0aF` | Inscribe hex value F (15) into vessel 'a' |
| `:.ab` | Copy vessel 'b' into vessel 'a' |

### The Gate `@` — Rune of the Void

Memory is the void — a vast darkness you can store things in and retrieve them from. The gate rune `@` opens passages:

| Form | Effect |
|------|--------|
| `@<ab` | **Sense** — read from void at address 'b' into vessel 'a' |
| `@>ab` | **Emit** — write vessel 'b' into void at address 'a' |

The `<` draws inward, the `>` pushes outward.

### The Hash `#` — Rune of Resonance

Laylines connect your program to the world beyond. When you touch a layline, **resonance** occurs — the outside world feels your intention, or you feel its presence.

| Form | Effect |
|------|--------|
| `#<ab` | **Sense** — feel the layline 'b', store in vessel 'a' |
| `#>ab` | **Emit** — send vessel 'b' through layline 'a' |

The console emulator connects these laylines:

| Layline | Purpose |
|---------|---------|
| `'c'` | Sense incoming characters |
| `'o'` | Emit to the world (stdout) |
| `'e'` | Emit warnings (stderr) |
| `'C'` | Console resonance vector |
| `'X'` | Exit with a code |

### The Dot `.` — Rune of Motion (Backward)

To move through your inscription, use the dot. First, mark your place with a **label** `'L`, then leap:

| Form | Effect |
|------|--------|
| `'L` | Mark this spot as 'L' |
| `..L` | Leap to 'L' |
| `.=L` | Leap if equal |
| `.!L` | Leap if not equal |
| `.>L` | Leap if greater |
| `.<L` | Leap if less |

Before leaping conditionally, you must **divine** with `?ab` — this compares two vessels and stores the omen in `?`.

### The Braces `{ }` — Rune of Definition

To inscribe a spell that waits to be invoked:

```
{F          ← define spell 'F', skip over its body
  :0r5      ← the spell's power
  ,         ← return from whence you came
}F          ← end of definition

;F          ← invoke spell 'F'
..F         ← or simply leap into it
```

The opening brace `{F` does two things: it remembers where the spell begins (in vessel 'F'), and it skips past the spell's body so you don't execute it immediately.

### The Brackets `[ ]` — Rune of Warding

To conditionally skip over dangerous inscriptions:

| Form | Effect |
|------|--------|
| `[=W ... ]W` | Skip to ward 'W' if equal |
| `[!W ... ]W` | Skip if not equal |
| `[>W ... ]W` | Skip if greater |
| `[<W ... ]W` | Skip if less |

```
?ab         ← divine the relationship of a and b
[=S         ← if equal, skip this...
  :0r9      ← ...dangerous inscription
]S          ← ...to here
```

### Invocation

| Rune | Effect |
|------|--------|
| `;F` | Invoke spell at 'F' (remembers where to return) |
| `,` | Return from invocation |

## Example: Echo

```
{E #<co #>oc ,}E    ← define echo: read char, write char, return
:'Co                ← set console vector to emit on 'o'
:'CE                ← on input, invoke spell E
```

## Console Emulator

```bash
./glyph program.glyph    # run a program
./glyph -e "<runes>"     # run inline
echo "Hi" | ./glyph examples/echo.glyph
```

Programs begin at address 0x0100. When input arrives, the console resonance vector is invoked.

## Library Usage

```c
#define GLYPH_IMPL
#include "glyph.h"

uint8_t mem[4096];
Glyph vm;

glyph_init(&vm, mem, sizeof(mem));
vm.emit = on_resonance_out;
vm.sense = on_resonance_in;
memcpy(mem, ":0a5 :0b3 +cab", 15);
glyph_run(&vm);
// vessel 'c' now holds 8
```

## Quick Reference

| Rune | Form | Meaning |
|------|------|---------|
| `:` | `:'aX` `:0aF` `:.ab` | inscribe literal, hex, or copy |
| `@` | `@<ab` `@>ab` | sense/emit the void (memory) |
| `#` | `#<ab` `#>ab` | sense/emit laylines (ports) |
| `.` | `..a` `.=a` `.!a` `.>a` `.<a` | leap backward |
| `{` `}` | `{L ... }L` | define spell, skip over |
| `[` `]` | `[=W ... ]W` | conditional ward (skip) |
| `'` | `'L` | mark label |
| `?` | `?ab` | divine (compare) |
| `;` `,` | `;F` `,` | invoke / return |
