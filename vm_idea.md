**there shall be names of spells like:**
- The Arcanum specification (the ISA)
- The runes (ASCII value, or gliphs)
- The incantations (a string, that represent a chant, "+ab")
- The Trinity (instructions with \[verb]\[subject]\[object])
- The monad, diad, triad, tetrad
    (size of the instructions, how many arguments it recieves)
- The Spirits (registers)
- The Totems (stacks)
- The eye (IP/PC register)
- The aether (entire memory / world)
- \[Spell]\[Spirit]\[Catalyst] (verb)(subject)(object)
- The Manifestation (instructions that creates values, like mov, #)
- Alchemy (instructions that does mixing on values (math) like +, -, \*, / and %)
- Shaping (instructions that does bitwise operations like \|, &, ^ and ~)
- Divination (instructions that compares like = > and <)
- Teleportation (instructions that messes with control flow jmp, call, ret)
- some good names: Augment (add), Diminish (sub), Amplify (mul), Fracture (div),
    Titrate (mod), Bind (and), Merge (or), Flux (xor), Recall (read),
    Inscribe (write), Warp (jmp), Omen (jeq), Invoke (call), Return (ret),
    Judge (cmp), Ascend (lsf), Descend (rsf), 

# The Arcanum specification
- The aether (magnitude of 65535 glyphs)
- There are 128 spirits 32 of them are shy.
  - Spirits can hold 4 glyphs or a word.
- There are at least 256 portals.
## The spells
- Manifestation (#)
 - Manifest/Echo (imm)
- Void (@ !)
 - Recall   (read)
 - Inscribe (write)
- Portals (())
 - Scry    (input)
 - Project (output)
- Alchemy (+ - \* / %)
 - Augment  (add)
 - Diminish (sub)
 - Amplify  (mul)
 - Fracture (div)
 - Titrate  (mod)
- Shaping (& | ^ ~ { })
 - Bind    (and)
 - Merge   (bor)
 - Flux    (xor)
 - Invert  (inv)
 - Ascend  (lst)
 - Descend (rst)
- Divination (= > <)
 - Equal (equ)
 - Above (gth)
 - Under (lth)
- Teleportation (: ; . , ?)
 - Invoke (call)
 - Return (ret)
 - Warp   (jmp)
 - Omen   (jcn)
 - Judge  (cmp)
