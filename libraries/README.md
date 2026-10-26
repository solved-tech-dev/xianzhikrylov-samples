# Library Packages

Place an extracted XianzhiKrylov package in this directory:

```text
libraries/
└── XianzhiKrylov/
    ├── include/
    ├── lib/
    └── license.key
```

The directory name may include a version. `common.mk` searches:

```text
libraries/*/include/XianzhiKrylovAPI.h
```

and uses the matching package automatically. If more than one package is
present, set `LIBRARY_ROOT` explicitly when building or running a sample.

Library binaries and license files are not tracked by Git. Download the package
and its matching license from
[Solved Tech](https://store.solved-tech.com/) and keep the extracted package
intact, including any architecture subdirectories it contains.
