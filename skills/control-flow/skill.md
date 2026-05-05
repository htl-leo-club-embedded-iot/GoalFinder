---
name: Control Flow
---

# Control Flow Skill

## Core Philosophy

**Goal**: Ensure execution flow is predictable, testable, and deterministic. Restrict patterns that make reasoning about code behavior difficult.

## Return Statement Rules

### Mandatory Constraints

- **Functions returning values MUST have exactly ONE return statement**
- **Early returns are strictly forbidden**
- **Void functions MUST NOT use any return statement**

### Rationale

Single return points make code behavior predictable and testable:
- Easier to trace execution paths
- Simpler to add cleanup logic
- Better support for debugging and profiling
- Follows principles used in safety-critical systems

### Examples

**Standard - Single Return Pattern:**

```typescript
function validateUser(user: User): boolean {
  let isValid = false;
  
  if (user.id && user.email && user.role) {
    if (user.email.includes('@')) {
      isValid = true;
    }
  }
  
  return isValid;
}
```

**Standard - Void Function (No Return):**

```cpp
void logEvent(const Event& event) {
  if (isEnabled()) {
    Logger::write(event.toString());
  }
}
```

## Loop and Flow Restrictions

### Forbidden Constructs

- **`goto` is strictly forbidden** - Unconditional jumps make code flow unpredictable
- **`break` outside `switch` statements is forbidden** - Use flags or restructure loops instead
- **`continue` should be avoided** - Restructure logic instead; only use with explicit justification in comments

### Rationale

These constructs create non-local jumps in execution flow that:
- Make it difficult to reason about code behavior
- Complicate static analysis and testing
- Can obscure resource cleanup paths
- Violate structured programming principles

### Examples

**Standard - Loop with Flag Variable:**

```cpp
size_t foundIndex = -1;
bool found = false;

for (size_t i = 0; i < data.size() && !found; ++i) {
  if (data[i] == target) {
    foundIndex = i;
    found = true;
  }
}
```

**Standard - Inverted Logic:**

```typescript
for (let user of users) {
  if (user.active) {
    processUser(user);
  }
}
```

## Preferred Control Flow Structure

### Linear, Sequential Logic

Structure code in clear, sequential blocks that flow from top to bottom:

```typescript
function processConnection(config: Config): Result {
  let result: Result = { success: false };
  
  // Step 1: Validate input
  const isValidConfig = validateConfig(config);
  
  // Step 2: Establish connection
  let connection: Connection | null = null;
  if (isValidConfig) {
    connection = establishConnection(config);
  }
  
  // Step 3: Perform operations
  let operationSuccess = false;
  if (connection !== null) {
    operationSuccess = performOperations(connection);
  }
  
  // Step 4: Cleanup and prepare result
  if (connection !== null) {
    closeConnection(connection);
  }
  
  result.success = isValidConfig && operationSuccess;
  
  return result;
}
```

### State-Based Flow (When Appropriate)

Use state variables and state machines for complex flows:

```cpp
enum class ConnectionState {
  IDLE,
  CONNECTING,
  CONNECTED,
  ERROR,
  CLOSED
};

class Connection {
private:
  ConnectionState _state = ConnectionState::IDLE;
  
public:
  void handleEvent(const Event& event) {
    switch (_state) {
      case ConnectionState::IDLE:
        if (event.type == EventType::CONNECT) {
          _state = ConnectionState::CONNECTING;
        }
        break;
      case ConnectionState::CONNECTING:
        if (event.type == EventType::SUCCESS) {
          _state = ConnectionState::CONNECTED;
        } else if (event.type == EventType::FAILURE) {
          _state = ConnectionState::ERROR;
        }
        break;
      // ... other states
    }
  }
};
```

### Switch Statements

Switch statements are acceptable for handling discrete options:

```typescript
function handleMessage(type: MessageType, data: any) {
  switch (type) {
    case MessageType.CONNECT:
      establishConnection(data);
      break;
    case MessageType.DISCONNECT:
      closeConnection();
      break;
    case MessageType.DATA:
      processData(data);
      break;
    default:
      logUnknownMessage(type);
  }
}
```

## Mandatory Exceptions

### Break in Switch Statements

`break` is **required** in switch statements to prevent fall-through:

```typescript
switch (event) {
  case 'click':
    handleClick();
    break;
  case 'hover':
    handleHover();
    break;
}
```

## Benefits of These Rules

✓ **Testability**: Single execution paths are easier to test  
✓ **Debugging**: Call stacks are predictable and traceable  
✓ **Maintainability**: Code changes have localized impact  
✓ **Performance**: No unpredictable jumps affecting branch prediction  
✓ **Safety**: Follows practices from safety-critical system standards  
✓ **Clarity**: Control flow is immediately obvious on code review  

## Review Checklist

When reviewing control flow compliance:

- [ ] No early returns in non-void functions
- [ ] No `goto` statements
- [ ] No `break` outside switch statements
- [ ] `continue` is justified (if present) with a comment
- [ ] Void functions do not have unnecessary returns
- [ ] Execution flow is linear and predictable
- [ ] State machines used where appropriate for complex logic
- [ ] Loop exit conditions are clear
- [ ] Exception handling (if used) doesn't create hidden control flow

## Migration Guide

If working with legacy code that violates these rules, prioritize refactoring:

1. **High priority**: Remove all `goto` statements
2. **High priority**: Consolidate multiple return statements
3. **Medium priority**: Replace `break` in loops with state flags
4. **Medium priority**: Restructure `continue` patterns
5. **Low priority**: Clean up other control flow patterns

Document exceptions in code review comments and create follow-up tasks for refactoring.
