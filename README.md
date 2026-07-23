# Campus Food Court KIOSK System

## Git Workflow Guide

This guide explains how to create branches, work locally, and push changes safely so we don't overwrite each other's work.

### 1. Clone the repo (first time only)

```bash
git clone https://github.com/Leong08/Campus-Food-court-KIOSK-System.git
cd Campus-Food-court-KIOSK-System
```

### 2. Always branch off an up-to-date `main`

Never work directly on `main`. Before creating a new branch, make sure your local `main` is current:

```bash
git checkout main
git pull origin main
```

### 3. Create your own branch

Use a clear, personal branch name (e.g. `part2-<yourname>` or `<feature>-<yourname>`):

```bash
git checkout -b part2-yourname
```

This creates the branch **locally**. It does not exist on GitHub yet.

### 4. Push your branch to GitHub

The first time you push a new branch, set the upstream so `git push`/`git pull` work without extra flags afterward:

```bash
git push -u origin part2-yourname
```

After that, any future push on this branch is just:

```bash
git push
```

### 5. Everyday workflow

```bash
git add <files>              # stage specific files (avoid `git add .` blindly)
git commit -m "clear message describing the change"
git push                     # sends local commits to your remote branch
```

Nothing is visible to teammates on GitHub until `git push` is run — commits are local until then.

---

## Before pushing — checklist

- [ ] **Pull first.** If teammates may have pushed to the same branch, run `git pull` before `git push` to avoid conflicts.
- [ ] **Push to your own branch, not `main`.** Only push to `main` via an approved Pull Request, never directly.
- [ ] **Check `git status`** before committing — make sure you're not accidentally including unrelated files (build artifacts, IDE configs, dataset edits you didn't mean to touch).
- [ ] **Test/compile your code** before pushing so you don't break the branch for others.
- [ ] **Write a meaningful commit message** — describe *what* changed and *why*, not just "update" or "fix".
- [ ] **Don't force-push (`git push --force`)** to shared branches. If you need to fix history, ask the team first.
- [ ] **Don't push directly to someone else's personal branch** unless they've asked you to collaborate on it.

## Merging into `main`

1. Push your branch to GitHub.
2. Open a Pull Request (PR) from your branch into `main`.
3. Have at least one teammate review the PR before merging.
4. Resolve any merge conflicts locally if GitHub flags them:
   ```bash
   git checkout main
   git pull origin main
   git checkout part2-yourname
   git merge main
   # resolve conflicts, then:
   git add <resolved-files>
   git commit
   git push
   ```
5. Merge the PR on GitHub once approved.

## Avoiding `main.cpp` conflict hell — code structure rules

There are two kinds of files in this repo, and they must be treated very differently.

### 1. Your own module files — push freely, zero conflicts

These belong to **one person only** (e.g. `stall_assignment.*`, `session_history.*`, and each member's own `.h`/`.cpp`). Nobody else touches them, so you can commit and push anytime with no risk.

### 2. Shared files — everyone edits them, so **coordinate**

`main.cpp`, `structures.h`, and `file_handler.*` are shared. This is where merge conflicts come from. Rules:

- **Keep `main.cpp` thin.** Don't write your module's logic inside `main.cpp`. Put it in your own files and expose **one** entry function (e.g. `runStallModule()`, `runSessionHistory()`). `main.cpp` is just a menu that calls each:

  ```cpp
  #include "stall_assignment.h"
  #include "session_history.h"
  // ...
  int main() {
      // 1 -> runStallModule();
      // 2 -> runSessionHistory();
      // ...
  }
  ```

  That way your only edit to `main.cpp` is 3 tiny lines (one include, one menu line, one case) — those almost never conflict.

- **`structures.h` is append-only.** Add your struct at the bottom; don't rename or reformat existing ones. Use your **own** constants (e.g. `MAX_SESSION_HISTORY`) — don't reuse someone else's like `MAX_HISTORY`.

- **Don't commit build files.** Never commit `kiosk.exe` or `.o` files — add them to `.gitignore`. They cause pointless conflicts and bloat the repo.

- **One person owns `main.cpp` integration** so we're not all rewriting the menu.

**TL;DR:** your own files = do whatever. Shared files (`main.cpp`, `structures.h`, `file_handler`) = keep your edits tiny and talk before big changes.

## Quick reference

| Action | Command |
|---|---|
| Update local `main` | `git checkout main && git pull` |
| Create a new branch | `git checkout -b <branch-name>` |
| Push a new branch first time | `git push -u origin <branch-name>` |
| Push subsequent changes | `git push` |
| Get teammates' latest changes | `git pull` |
| Check what's changed | `git status` |
| Switch between branches | `git checkout <branch-name>` |
