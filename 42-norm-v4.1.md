# 42 Norm (v4.1) — Clean Rules Summary

> This is a concise reference of the official 42 C coding standard (“The Norm”). Use it as a checklist while coding and reviewing.

---

## Scope
- Applies by default to all **C projects** in the **Common Core** and to any project where it’s explicitly required.  

---

## Naming
- **Prefixes**: `s_` (struct), `t_` (typedef), `u_` (union), `e_` (enum), `g_` (global).  
- **Identifiers** (functions, variables, types): **snake_case**, **lowercase**, digits, and `_` only. **No capitals**.  
- **Files/dirs**: snake_case (lowercase, digits, `_`).  
- **ASCII only** (except inside string/char literals).  
- Names should be **clear/explicit** in English (or mnemonic).  
- **Globals**: using **non-const, non-static** globals is **forbidden** (unless a project explicitly allows them).  
- Your files **must compile**.  

---

## Formatting
- **Function length**: **≤ 25 lines** (excluding the function’s own braces).  
- **Line width**: **≤ 80 columns**, comments included. Tabs count as multiple columns (their visual width).  
- **Function separation**: **exactly one empty line** between functions (comments/preprocessor lines allowed between).  
- **Indentation**: **real tabs of width 4** (ASCII `\t`, not 4 spaces).  
- **Braces**: on their **own line**, and **blocks are indented**; exception: brace placement inside `struct/enum/union` declarations.  
- **Blank lines**: must be truly empty (no spaces/tabs). No **trailing whitespace** at EOL.  
- **No double empties** and **no double spaces**.  
- **Declarations at top** of each function.  
- **Align variable names** in a scope to the same column (types are already indented by block).  
- Pointer asterisks stick to **variable**: `char *p`, not `char* p`.  
- **One declaration per line**.  
- **No init on the same line** as a declaration, except for **globals** (if allowed), **static**, and **constants**.  
- Inside a function, add **one empty line** between **declarations** and the **rest** of the function — no other empty lines inside functions.  
- **One instruction/control structure per line**.  
- Long instructions may wrap to multiple lines: indent continuation lines; if an operator is split, it starts the **new** line.  
- **Spacing**:  
  - After every comma and semicolon (unless at EOL).  
  - Around operators: exactly **one space** between operands and operators.  
  - C keywords followed by **one space**, **except** type keywords (`int`, `char`, `float`, …) and `sizeof`.  
- **Control structures** (`if`, `while`, …) must use **braces**, unless they have a **single instruction on one line**.  

---

## Functions
- **Max 4 named parameters**.  
- A function with **no arguments** must be prototyped as `void`.  
- **Prototype parameters must be named**.  
- **≤ 5 local variables** per function.  
- **Return value** must be written **in parentheses**: `return (value);` (unless returning nothing).  
- Place **one tab** between the **return type** and the **function name** in the definition.  

---

## Typedef / struct / enum / union
- Put a **space** after the keyword: `struct s_x { ... }`, same for `enum`, `union`.  
- When declaring a variable of a `struct/enum/union` type, follow normal indentation rules for the **variable name**.  
- Inside the braces, **indent** like any other block.  
- Put a **space** after `typedef`, and indent the **new type name** normally.  
- **Align all structure names to the same column** within a scope.  
- **Do not declare a structure inside a `.c` file** (declare in headers).  

---

## Headers (`.h`)
- Header files may contain: **includes**, **declarations**, **defines**, **prototypes**, **macros**.  
- **All includes go at the top** of the file.  
- **Never include a `.c` file** anywhere.  
- **Header guards required**: for `ft_foo.h` use `FT_FOO_H`.  
- **Unused headers** are forbidden. You may justify inclusions with comments (in both `.c` and `.h`).  

---

## 42 File Header (banner)
- Every `.c` and `.h` file **must begin** with the **standard 42 header** (multi-line comment with required metadata).  
- The header must stay **up-to-date** (author login + student email `@student.campus`, creation date, last updater + date). It should update **automatically on save** (configure your editor if needed).  

---

## Macros & Preprocessor
- Your own `#define` are only for **literal/constant values**.  
- `#define` that **bypass the Norm** or **obfuscate code** are **forbidden**.  
- You may use **standard-library macros** if they’re allowed by the project.  
- **No multiline macros**.  
- Macro names are **ALL_CAPS**.  
- Indent preprocessor directives **inside** `#if/#ifdef/#ifndef` blocks.  
- Preprocessor directives are **global-scope only** (not inside functions, etc.).  

---

## Forbidden
- **Keywords**: `for`, `do...while`, `switch`, `case`, `goto`.  
- **Ternary operator** `?:` is not allowed.  
- **VLA** (variable length arrays) are not allowed.  
- **Implicit type** in variable declarations is not allowed.  

---

## Comments
- **No comments inside function bodies.**  
- Comments must be **on their own line** or **at the end of a line** (in allowed locations).  
- Comments should be **useful** and **in English**.  
- Don’t use comments to justify **carryall / bad functions** (e.g., vague names like `f1`, `a`, `b`, etc., or functions created only to dodge Norm rules).  

---

## Files
- **Do not include `.c` files** in other `.c` files.  
- **Max 5 function definitions per `.c` file**.  

---

## Makefile
- Mandatory rules: **`all`** (default), **`$(NAME)`**, **`clean`**, **`fclean`**, **`re`**.  
- The Makefile must **not relink** unnecessarily.  
- **Multi-binary projects**: provide a rule per binary (e.g., `$(NAME_1)`, `$(NAME_2)`, …); `all` builds **all binaries**.  
- If your project uses a non-system **local library** (e.g., `libft`) that lives with your sources, your Makefile must **build it automatically**.  
- **List source files explicitly** — no wildcards like `*.c` or `*.o`.  

---

### Quick Example (style sketch)
```c
typedef struct s_point
{
	int		x;
	int		y;
}   t_point;

int	add(int a, int b)
{
	int	result;

	result = a + b;
	return (result);
}
```
> Tabs shown as `\t` here for clarity; in real code use **actual tabs** (ASCII 9) and keep line width ≤ 80.
