---
name: Coding Style
--- 

# Coding Style Skill

## Core Principles

- **Minimal implementations**: Favor simplicity and directness over clever constructs
- **Code reuse**: Leverage existing infrastructure whenever possible
- **Avoid duplication**: Unnecessary abstraction should be eliminated
- **Prioritize readability**: Never sacrifice clarity for brevity

## Naming Conventions

### General Rules

- Use clear and meaningful variable names that self-document their purpose
- Avoid cryptic abbreviations unless they are widely understood in the specific context
- Names should reflect intent and domain terminology

### Language-Specific Conventions

#### TypeScript / JavaScript

- **Constants**: `UPPER_SNAKE_CASE` (e.g., `MAX_RETRIES`)
- **Variables & Functions**: `camelCase` (e.g., `connectionStatus`, `handleClick()`)
- **Classes & Interfaces**: `PascalCase` (e.g., `ConnectionManager`, `ISettings`)
- **Private members**: Prefix with underscore `_privateMethod()`
- **Vue components**: `PascalCase` with `.vue` extension (e.g., `ConnectionSettingsView.vue`)

#### C++

- **Constants & Macros**: `UPPER_SNAKE_CASE` (e.g., `MAX_BUFFER_SIZE`)
- **Functions**: `camelCase` or `PascalCase` depending on context (e.g., `handleEvent()`, `ProcessData()`)
- **Classes**: `PascalCase` (e.g., `LedController`)
- **Member variables**: Prefix with underscore `_memberVar`
- **Private methods**: Prefix with underscore `_privateMethod()`

## Code Structure

### Function Organization

- **Keep functions small and focused**: Each function should have a single, clear responsibility
- **Single level of abstraction**: Functions should operate at one conceptual level
- **Prefer straightforward logic**: Avoid clever or complex constructs that reduce readability

### File Organization

- **Group related functionality**: Keep related code in the same file or module
- **Logical ordering**: Define utilities before usage, public interfaces before implementations
- **Clear separation of concerns**: Different concerns should be in different files

## Line Length & Formatting

- **Target line length**: 100 characters maximum
- **Respect language conventions**: Follow language-specific style guides (Prettier for TS/JS, clang-format for C++)
- **Indentation**: Use consistent indentation (2 spaces for YAML/JSON, 4 spaces for code)

## Import Organization

### TypeScript/JavaScript

```typescript
// 1. External libraries
import Vue from 'vue'
import axios from 'axios'

// 2. Internal absolute imports
import { useStore } from '@/stores'
import { Logger } from '@/utils'

// 3. Relative imports
import { helper } from './helpers'
```

### C++

```cpp
// 1. System headers
#include <iostream>
#include <vector>

// 2. Library headers
#include <Arduino.h>

// 3. Local headers
#include "Config.h"
#include "LedController.h"
```

## Type Safety

- **Use types meaningfully**: Leverage language type systems to prevent errors
- **Avoid `any` in TypeScript**: Use proper typing; use `unknown` only when truly necessary
- **Be explicit**: Don't rely on implicit conversions

## Common Patterns

### Error Handling

- Use exceptions or result types consistently across the codebase
- Document error conditions in function documentation
- Avoid silent failures

### State Management

- Minimize mutable state
- Make state changes explicit and predictable
- Document state transitions when non-obvious

### Performance Considerations

- Write readable code first; optimize only where profiling shows bottlenecks
- Document performance-critical sections with explanatory comments
- Include benchmarks for critical paths if performance is important

## Review Checklist

When reviewing code for style compliance:

- [ ] Names clearly express intent
- [ ] Functions have single responsibility
- [ ] No unnecessary duplication
- [ ] Code is readable without extensive comments
- [ ] Language conventions are followed
- [ ] Import order is correct
- [ ] No overly complex control flow
