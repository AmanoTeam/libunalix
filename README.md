# Unalix

Unalix is a library written in C. It implements the specification used by the [ClearURLs](https://github.com/ClearURLs/Addon) addon for removing tracking fields from URLs.

# Installation

The source code uses C99 features, so C99 enabled compiler is required. Unalix requires CMake 1.12 or higher to build properly.

Go to the source root directory an run the commands bellow:

```bash
cmake -DCMAKE_BUILD_TYPE=Release .
cmake --build .
cmake --install .
```

## CMake flags

- `UNALIX_BUILD_TESTING` : `ON`/`OFF` (default: `ON`)
  - Enable or disable building the test suite

## Running tests

The run the test suite, build Unalix with `UNALIX_BUILD_TESTING` set to `ON` and then run:

```
make test
```

## Contributing

If you have discovered a bug in this library and know how to fix it, fork this repository and open a Pull Request.

## Third party software

Unalix includes some third-party software in it's codebase. See them below:

- **ClearURLs**
  - Author: Kevin Röbert
  - Repository: [ClearURLs/Rules](https://github.com/ClearURLs/Rules)
  - License: [GNU Lesser General Public License v3.0](https://gitlab.com/ClearURLs/Rules/blob/master/LICENSE)
