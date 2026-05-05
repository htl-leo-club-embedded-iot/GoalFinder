---
name: Comments and Documentation
---

# Comments and Documentation Skill

## Core Rule

**Do NOT write comments that restate what the code already clearly expresses.**

Comments should add value, not noise. If code requires explanation about what it does, it should be rewritten to be clearer.

## When Comments Are Allowed

Comments provide value only when they explain:

- **Why**: Rationale for design decisions, algorithm choices, or constraints
- **Why not**: Alternatives considered and reasons they were rejected
- **Non-obvious logic**: Edge cases, workarounds, or complex algorithms that aren't self-evident
- **Reusable/shared functions**: Public APIs and their contracts
- **Domain-specific knowledge**: Context that isn't apparent from code alone
- **Performance considerations**: Why a particular approach was chosen for efficiency
- **Known issues**: Limitations, TODOs, or temporary workarounds

## Anti-Pattern Examples

**Do NOT write comments like this:**

```typescript
// increment i
i++;

// set value
value = 10;

// return result
return result;

// check if user exists
if (user !== null) {
  // add user to list
  users.push(user);
}
```

**Instead, make the code clear:**

```typescript
i++;  // No comment needed

adminThreshold = 10;  // Still clear without comment

return result;  // Self-evident

if (user !== null) {
  users.push(user);  // Clear intent
}
```

## Function Documentation Standards

### TypeScript / JavaScript

Use JSDoc with proper structure:

```typescript
/**
 * Brief one-line description of what the function does.
 *
 * More detailed explanation if needed, including behavior,
 * side effects, or important constraints.
 *
 * @param {Type} paramName - Description of parameter
 * @param {Type} optionalParam - Description (optional)
 * @returns {Type} Description of return value
 * @throws {ErrorType} Condition when error is thrown
 * @example
 * const result = myFunction('input')
 * // Returns: processed output
 */
function myFunction(paramName: string, optionalParam?: number): Result {
  // implementation
}
```

### C++

Use Doxygen-style documentation:

```cpp
/**
 * Brief one-line description of what the function does.
 *
 * More detailed explanation if needed, including behavior,
 * side effects, or important constraints.
 *
 * @param paramName Description of parameter
 * @param optionalParam Description of parameter (optional)
 * @return Description of return value
 * @throws ExceptionType Condition when exception is thrown
 *
 * @note Any important implementation notes
 * @see RelatedFunction
 */
void myFunction(const std::string& paramName, int optionalParam = 0) {
  // implementation
}
```

### Vue Components

```typescript
/**
 * ConnectionSettingsView - Manages network connection configuration.
 *
 * Displays connection settings, validates input, and persists changes
 * to local storage. Emits events for connection state changes.
 *
 * @component
 * @example
 * <ConnectionSettingsView />
 */
export default defineComponent({
  name: 'ConnectionSettingsView',
  // ...
})
```

## Inline Comments

Use inline comments sparingly and only for non-obvious logic:

```typescript
// Good: Explains why we need this specific comparison
// Use loose equality to handle null and undefined the same way
if (value == null) {
  value = defaultValue
}

// Bad: Just repeats what the code does
// Check if value is null
if (value == null) {
  value = defaultValue
}
```

## Block Comments

Block comments should explain *why* a complex algorithm or approach was chosen:

```cpp
// We use a binary search here instead of linear scan because
// the speed improvement (O(log n) vs O(n)) is critical for
// sensor data processing with large buffers.
size_t index = binarySearch(data, targetValue);
```

## Documentation Rules by Language

### TypeScript/JavaScript/Vue

- **Always** document exported functions and components
- **Always** document public methods
- **Document** complex algorithms or non-obvious logic
- Use JSDoc consistently; tools depend on it
- Keep documentation synchronized with code changes

### C++

- **Always** document public methods and functions
- **Document** class purpose and responsibility
- Use Doxygen format for API documentation
- Document template parameters
- Document preprocessor macros

## Technical Documentation Requirements

For new infrastructure or architectural changes, ensure external documentation includes:

- [ ] **Purpose**: Why this was added and what problem it solves
- [ ] **Architecture**: How it fits into the system
- [ ] **Integration**: How it works with existing components
- [ ] **Configuration**: Required setup and configuration
- [ ] **Troubleshooting**: Common issues and how to resolve them
- [ ] **Examples**: Code examples showing typical usage
- [ ] **Performance**: Any performance characteristics or constraints
- [ ] **Limitations**: Known limitations or gotchas

## User Documentation Requirements

For new features or user-facing changes, document:

- [ ] **What**: What the feature does in plain language
- [ ] **Why**: When and why users should use it
- [ ] **How**: Step-by-step instructions
- [ ] **Examples**: At least one practical, real-world example
- [ ] **Limitations**: Any important constraints or caveats
- [ ] **Troubleshooting**: Common issues specific to this feature
- [ ] **Related features**: Links to related documentation

## Comment Style Guidelines

- Use `//` for single-line comments in C++ and JavaScript/TypeScript
- Use `/* */` for multi-line comments only when necessary
- Keep comments close to the code they describe
- Maintain comments when modifying code
- Remove outdated comments during refactoring

## Documentation Issue Templates

When creating issues for documentation, outline only what needs to be added or updated in external documentation. Reference this skill for what should be documented and at what level of detail.

## Review Checklist

When reviewing documentation:

- [ ] All public APIs are documented
- [ ] Comments explain *why*, not *what*
- [ ] No redundant comments
- [ ] Examples are accurate and current
- [ ] Links and cross-references are correct
- [ ] Code examples follow project style
- [ ] Documentation is at appropriate detail level
- [ ] No outdated information
