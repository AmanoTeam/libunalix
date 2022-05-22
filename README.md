# Unalix

Unalix is a library and CLI tool written in C++. It implements the specification used by the [ClearURLs](https://github.com/ClearURLs/Addon) addon for removing tracking fields from URLs.

# Usage

The C++ API is pretty simple. Here are some snippets:

```cpp
#include <string>
#include <cassert>

#include <unalix/unalix.hpp>

int main() {
    const std::string url = "https://deezer.com/track/891177062?utm_source=deezer";
    const std::string result = clean_url(url);
    
    assert (result == "https://deezer.com/track/891177062");
}
```

Unalix can also unshort URLs:

```cpp
#include <string>
#include <cassert>

#include <unalix/unalix.hpp>

int main() {
    const std::string url = "https://bitly.is/Pricing-Pop-Up";
    const std::string result = unshort_url(url);
    
    assert (result == "https://bitly.com/pages/pricing");
}
```

# Installation

The source code uses C++11 features, so C++11 enabled compiler is required. Unalix requires CMake 1.12 or higher to build properly.

Go to the source root directory an run the commands bellow:

```bash
cmake -DCMAKE_BUILD_TYPE=Release .
cmake --build .
cmake --install .
```

## CMake flags

- `UNALIX_BUILD_TESTING` : `ON`/`OFF` (default: `ON`)
  - Enable or disable building the test suite

- `UNALIX_BUILD_JNI` : `ON`/`OFF` (default: `OFF`)
  - Enable or disable building Unalix with JNI support

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

- **BearSSL**
  - Author: Thomas Pornin
  - Repository: [BearSSL at bearssl.org](https://bearssl.org/gitweb/?p=BearSSL)
  - License: [MIT License](https://bearssl.org/gitweb/?p=BearSSL;a=blob;f=LICENSE.txt)
