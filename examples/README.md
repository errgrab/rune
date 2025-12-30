# Glyph Example Programs

## Device Ports

**Console:** `'C'`=vector, `'c'`=read, `'o'`=stdout, `'e'`=stderr  
**System:** `'X'`=exit

## Examples

### hello.glyph
Prints "Hello, World!" to stdout.

### echo.glyph
Echoes stdin to stdout using the console vector.

## Running

```bash
./glyph examples/hello.glyph
echo "Hello" | ./glyph examples/echo.glyph
```
