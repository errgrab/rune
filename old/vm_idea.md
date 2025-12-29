| opcode | asm | Pseudo C |
|--------|-----|-|
| ` ***` | nop | |
| `_***` | hlt | (idk if i want an instruction to halt) |
| `+abc` | add | `a = b + c` |
| `-abc` | sub | `a = b - c` |
| `*abc` | mul | `a = b * c` |
| `/abc` | div | `a = b / c` |
| `%abc` | mod | `a = b % c` |
| `&abc` | and | `a = b & c` |
| `\|abc` | or  | `a = b \| c` |
| `^abc` | xor | `a = b ^ c` |
| `~ab*` | not | `a = !b` |
| `!apt` | str | `*a = p` |
| `@apt` | lod | `a = *p` |
| `#at*` | imm | `a = (*imm*)` |
| `:a**` | jmp | `pc = a` |
| `;abc` | cnd | `if (b == c) pc = a` |
| `?abc` | cmp | `switch (*a) '=': a = b == c; ...` |
| `(a**` | cal | `push(pc); pc = a` |
| `)a**` | ret | `pc = pop();` |
