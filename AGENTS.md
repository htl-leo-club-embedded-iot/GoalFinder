# GoalFinder Agents Information

## Overview

This repository defines a set of agents used in the GoalFinder project.
Agents are expected to operate using modular skills defined in the `skills/` directory. Each skill defines specific rules, constraints, and implementation guidelines.

## Project Repository

GitHub: https://github.com/htl-leo-club-embedded-iot/GoalFinder
Use the GitHub MCP server for retrieving additional information

## Structure

All behavior is defined through skills:

- `skills/coding-style.md` → coding conventions and structure rules
- `skills/comments-and-docs.md` → documentation rules
- `skills/control-flow.md` → restrictions on control flow and returns


## Rule of Precedence

1. Skills files define **mandatory rules**
2. This file defines only **navigation and structure**
3. If a rule is missing here, assume it is defined in `skills/`

## Usage

Agents must:
- Always consult relevant skills before implementing code
- Follow all applicable skills without exception
- Combine skills when multiple apply to a task