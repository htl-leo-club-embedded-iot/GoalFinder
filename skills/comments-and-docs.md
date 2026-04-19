# Comments and Documentation Skill

## Core Rule

Do NOT write comments that restate what the code already clearly expresses.

## When Comments Are Allowed

Only add comments if they provide **additional value**, such as:

- Explaining *why* a decision was made (not what the code does)
- Clarifying non-obvious logic or edge cases
- Documenting reusable or shared functions
- Explaining domain-specific behavior that is not self-evident from code

## Function Documentation Standards

### TypeScript

Use JSDoc:

```ts
/**
 * Brief description of purpose.
 * Explain behavior, side effects, or constraints if necessary.
 */
function myFunction() {
  // implementation
}
```

### C++

Use Doxygen style:

```cpp
/**
 * Brief description of purpose.
 * Explain behavior, side effects, or constraints if necessary.
 */
void myFunction() {
  // implementation
}
```


## Anti-Pattern

Do not write comments like:

```ts
// increment i
i++;

// set value
value = 10;

// return result
return result;
```

If the code requires such explanation, it should be rewritten to be clearer.

## Guiding Principle

**Code should explain *what*, comments should explain *why*.**

---

## Documentation Issue Templates

When creating issues for documentation updates, outline only what needs to be added to external documentation.

### Technical Documentation Checklist

For new infrastructure or architectural changes, document:

- [ ] Purpose and why this was added
- [ ] How it integrates with existing systems
- [ ] Configuration requirements
- [ ] Troubleshooting guidance

### User Documentation Checklist

For new features or user-facing changes, document:

- [ ] What the feature does
- [ ] How to use it (step-by-step)
- [ ] When/why to use it
- [ ] At least one practical example
- [ ] Any limitations or important notes
