# Contribute

Contributions are welcome — including bug reports, feature requests, documentation improvements, and code changes.

> [!NOTE]
> This documentation might be out of date as it is just a joined version of the contribution category of the [GoalFinder Technical Documentation](https://goalfinder.github.io/technical)

---

## Have an Idea?

If you have an idea to improve GoalFinder:

- Open an issue with the `feature-request` or `user-story` label  
- Include:
  - A short summary
  - The motivation behind the idea  

Please do **not** add any other labels — the project team will assign labels as needed.

---

## Found an Issue?

If you find a bug or incorrect behavior:

- Open an issue with the `bug` label  
- Include:
  - Steps to reproduce
  - Expected vs. actual behavior
  - Environment details
  - Logs or screenshots (if available)

Please do **not** add any other labels — the project team will assign labels as needed.

---

## Want to Contribute Code?

1. Fork the repository  
2. Create your changes  
3. Submit a pull request including:
   - A concise summary of changes
   - Related issues
   - Tests or screenshots demonstrating the update  

---

## Already a Project Member?

Read the documentation in `github.md` for detailed GitHub setup instructions.

---

# GitHub Projects

## Iterative Development Project

The Iterative Development Project contains multiple views:

- Feature Requests  
- User Stories  
- Features  
- Iteration Board  

### Iteration Board Columns

- **Backlog** — User stories that need to be sub-issued into feature-requests or features  
- **Feature Request** — More specific ideas that still need review  
- **Feature** — Specific feature to be implemented and sub-issued into tasks  
- **Todo** — Ready to be worked on; not yet started  
- **In Progress** — Work is actively underway  
- **Review** — Implemented and awaiting verification by another team member  
- **Done** — The issue is completed and closed  

### Procedure for New Issues (Iterative Development)

1. **Triage** — Close if irrelevant or out of scope  
2. **Reproduce** — Attempt to reproduce; request more information if needed  
3. **Classify** — If reproducible:
   - Decide whether it is a bug, user story, or feature request  
   - For user stories/features, convert or move to the `Feature` column after review  
4. **Assign** — Add appropriate labels and assign a team member  
5. **Plan** — Move to `Todo`  
6. **Work** — Move to `In Progress` when work begins  
7. **Verify** — Move to `Review` after implementation  
8. **Close** — Move to `Done` once approved  

---

## Bug Tracker

The Bug Tracker uses a single board with these columns:

- **Identified** — Reported and acknowledged  
- **Todo** — Ready to be worked on  
- **In Progress** — Work underway  
- **Review** — Awaiting verification  
- **Done** — Resolved and closed  

### Procedure for New Bug Issues

1. **Triage** — Close if irrelevant or out of scope  
2. **Information Check** — If insufficient information, request more  
3. **Reproduce** — Attempt reproduction  
4. **Severity Assignment** — Apply one of:
   - `severe`
   - `major`
   - `minor`  
   (Consult the team if unsure.)
5. **Plan** — Move to `Todo`  
6. **Work** — Move to `In Progress`  
7. **Review** — Move to `Review` after implementation  
8. **Close** — Move to `Done` once verified  

---

# Labels

The following custom labels can be applied to issues.

---

## Bugs

- `bug` — Identified bug  
- `bug-fix` — Bug fixing task  
- `severe` — Severe bug  
- `major` — Major bug  
- `minor` — Minor bug  

Apply a severity label to each bug issue.

---

## Categories

- `cad` — CAD design (housing, mounting, ...)  
- `doc` — Internal / technical documentation  
- `hardware` — Hardware-related  
- `firmware` — Firmware-related  
- `web-app` — Web access point of the GoalFinder device  
- `pages` — Web page  
- `pcb` — PCB design  
- `infrastructure` — Infrastructure  

Apply a category label to features and parent tasks.

---

## Priority

- `p0` – `p4` — Priority from highest to lowest  

Apply a priority label to every non User Story / Feature (Request) issue.

---

## Iterative Development Labels

- `user-story` — User Story  
- `feature-request` — Feature Request  
- `feature` — Feature  

---

## Other

- `enhancement` — Task not worthy of any other label  
- `notice` — Notice (Not to be worked on)  
- `review` — Simple review task  
- `task` — Task not categorized otherwise  

---

# Deploying GitHub Pages

Deployment is configured to use the `deploy` branch.

Commits on the `main` branch do **not** trigger deployment automatically.

---

## Pull Request for Deployment

Create a pull request:

```
base: deploy ← compare: main
```

### Naming Scheme

```
Deploy at dd.mm.yyyy hh:mm
```

- Date in European format  
- Time in `hh:mm` format  

You may add one or more reviewers if unsure about changes.

---

# Workflow

# Release

This section describes how to release new firmware versions to GitHub.

---

## Release Workflow

Workflow defined in `.github/workflows/main.yml`:

1. Checkout repository with full history  
2. Extract version from tag and current git commit hash  
3. Update version files using Python script  
4. Setup Node.js and install web dependencies  
5. Build web app  
6. Cache Python and PlatformIO dependencies  
7. Setup Python and install PlatformIO  
8. Build firmware and filesystem  
9. Publish firmware and filesystem binaries as GitHub release  

This ensures the entire firmware builds successfully before release.

---

## Versioning Rules

Version format:

```
vX.Y.Z(o)
```

### Version Components

- **X** — Major changes (no backward compatibility)  

Must be discussed with the team first  

- **Y** — New features or enhancements  
  - Backward compatibility maintained or improved  

- **Z** — Patch release  
  - Bug fixes  
  - Performance improvements  
  - Minor internal changes  

---

### Pre-Release Tags (Optional `o`)

- `a` — Alpha (very early, limited testing)  
- `b` — Beta (more stable, tested)  
- none — Full release (production ready)  

---

## Release to GitHub

Tag and push to trigger automatic build and release:

```bash
git tag vX.Y.Z(o)
git push origin vX.Y.Z(o)
```

---

## Dev Builds & Testing

**Development use only — not for production.**

Update the version displayed in the web app using:

```bash
python update-version.py X.Y.Z(o)-dev
```

### Regular Builds

```bash
python update-version.py X.Y.Z(o)
```

---

# Upload Tool (ULT)

Always run the upload tool from the root directory.

The default workflow executes:

1. `npm run build` — Build and compress latest web app  
2. `pio run -t upload` — Upload firmware  
3. `pio run -t uploadfs` — Upload filesystem (Web App)  

---

## Parameters

- `-f` — Erase flash and clean `pio`  
- `-m` — Monitor after upload  

✔ Parameters can be combined.
