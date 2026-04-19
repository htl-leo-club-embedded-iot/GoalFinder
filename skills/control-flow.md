# Control Flow Skill

## Return Statement Rules

- Functions that return a value must have exactly **one return statement**
- Early returns are not allowed
- Void functions must NOT use any return statement

---

## Loop and Flow Restrictions

- `goto` is strictly forbidden
- `break` is only allowed inside `switch` statements
- `continue` should be avoided unless explicitly justified
- Alternative non-structured control flow patterns are not allowed

---

## Preferred Structure

- Prefer linear, readable flow
- Use flags or state variables instead of early exits
- Structure logic into clear, sequential blocks

---

## Intent

This ensures predictable execution flow and improves testability, readability, and determinism.
