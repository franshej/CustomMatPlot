# Working in this repository

## Format before committing

Run `clang-format` over every file you touched before committing, never after
pushing. The repo's `.clang-format` is `BasedOnStyle: Google`, which
`CONTRIBUTING.md` also requires of human contributors.

```bash
clang-format -i <the files you changed>
```

To check without rewriting, `clang-format --dry-run --Werror <file>` prints a
violation per offending line and is silent when the file is clean.

Format only the files the change touches. `cmp_plot_test.cpp` and
`cmp_plot3d_test.cpp` have pre-existing violations; reformatting them buries
the actual change in unrelated diff noise.

**Why before committing rather than before pushing:** formatting after the
commit leaves the pushed commit unformatted and the fix stranded in the
working tree. The commit is the thing that has to be clean.
