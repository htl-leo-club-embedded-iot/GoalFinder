# GoalFinder Agents Information

## Overview

This repository defines a set of agents used in the GoalFinder project.
Agents are expected to operate using modular skills defined in the `skills/` directory. Each skill defines specific rules, constraints, and implementation guidelines.

## Project Repository

GitHub: https://github.com/htl-leo-club-embedded-iot/GoalFinder
Use the GitHub MCP server for retrieving additional information

## Structure

All behavior is defined through modular skills organized by concern:

- `skills/coding-style/skill.md` → coding conventions, naming, and structure rules
- `skills/comments-and-docs/skill.md` → documentation standards and comment rules
- `skills/control-flow/skill.md` → restrictions on control flow and function returns

Each skill directory contains:
- `skill.md` - The complete skill definition with industry standards
- Supporting files or examples (as needed)


## Rule of Precedence

1. Skills files define **mandatory rules** that must be followed
2. This file defines **navigation and skill organization**
3. If a rule is missing here, assume it is defined in the relevant `skills/*/skill.md`
4. In case of conflict, the specific skill file takes precedence over this overview

## Skill Categories

### Coding Style (`skills/coding-style/skill.md`)

Covers code structure, naming conventions, organization, and readability standards. Apply to all code changes affecting project style.

### Comments and Documentation (`skills/comments-and-docs/skill.md`)

Defines when and how to document code. Specifies documentation requirements for functions, modules, and architectural changes.

### Control Flow (`skills/control-flow/skill.md`)

Establishes restrictions on control flow structures to ensure predictable, testable code. Mandatory for all implementations.

## Usage

Agents must:
- Always consult relevant skill files **before implementing code**
- Follow **all** applicable skills without exception
- Combine skills when multiple apply to a task
- Use the skill files as definitive reference for standards and requirements
- Document any deviations from skills with explicit justification

## Applying Skills

### Before Writing Code

1. Identify which skills apply to your task
2. Read the relevant `skills/*/skill.md` files completely
3. Note any language-specific requirements
4. Review examples and anti-patterns in the skill

### During Implementation

1. Follow naming and structure rules from `coding-style/skill.md`
2. Ensure control flow complies with `control-flow/skill.md`
3. Write documentation according to `comments-and-docs/skill.md`

### During Code Review

1. Verify compliance with all applicable skills
2. Use skill files as reference for feedback
3. Link to relevant skill sections in review comments