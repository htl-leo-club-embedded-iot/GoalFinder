# Contributor

Some brief guidelines about using development tools are lined out here.

## GitHub

### Projects

#### Iterative Development

The Iterative Development Project has multiple views:

Feature Requests, User stories, Features view are used to view issues with the corresponding labels as well as the Iteration board with following columns:

- **Backlog**: User stories that need to be sub-issued into feature-requests or features
- **Feature Request**: More specific ideas that still need review
- **Feature**: Specific feature to be implemented and sub-issued into tasks
- **Todo** — Ready to be worked on; not yet started.
- **In Progress** — Work is actively underway.
- **Review** — Change is implemented and awaiting verification by another team member.
- **Done** — The issue is done and closed.

Procedure for new issues:

1. Triage: close if the issue is irrelevant or out of scope.
2. Reproduce: attempt to reproduce the problem; request more information if needed.
3. Classify: if reproducible, decide whether it is a bug, user story, or feature request. For user stories/features, convert or move the issue to the `Feature` column after review.
4. Assign: add appropriate tags and assign a team member.
5. Plan: move the issue to `Todo` for planning.
6. Work: when work begins, move the issue to `In Progress`.
7. Verify: after implementation, move it to `Review` for verification.
8. Close: once a reviewer accepts the change, move the issue to `Done`.

#### Bug Tracker

The Bug Tracker uses a single board with these columns:

- **Identified** — The issue has been reported and acknowledged.
- **Todo** — Ready to be worked on; not yet started.
- **In Progress** — Work is actively underway.
- **Review** — Change is implemented and awaiting verification by another team member.
- **Done** — The issue is resolved and closed.

Procedure for new issues:

1. Triage the issue: close if irrelevant or out of scope.
2. If not information is not sufficient; request more information.
3. If reproducible, assign a severity label (`severe`, `major`, or `minor`) — consult the team when unsure.
6. Move the issue to `Todo` for planning.
7. When work starts, move it to `In Progress`.
8. After implementation, move it to `Review` for verification.
9. A reviewer verifies the fix; if accepted, move the issue to `Done`.