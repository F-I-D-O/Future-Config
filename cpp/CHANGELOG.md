# 0.2.1
## Added

- new section in README.md about WSL test environment setup


## Fixed

- cmake package version set
- project-specific cmake variables are used for config builder runner defaults instead of top-level ones.
- Config_object transformer refactored to prevent unreached code warnings.
- usage tests fixed so that failed library installation does not kill the test suite.
- dependecy installation added to usage tests.
- usage tests manual library installation: compiler now match the compiler of the client project.
- gcc11 test vcpkg toolchain fixed to pint to `usr/local/bin` instead of `/usr/bin`.
- mamba gcc11 installation script fixed to create the environment in the home directory instead of `/envs`
- Project versioning in CMakeLists.txt fixed


# 0.2.0

## Added

- Command line config definition: Added support for command line arguments to the config definition
- New `load()` function overload for tests that uses only the default config file (master config only)
- GCC-11 mamba installation script for testing (`cpp/test/vcpkg_gcc11/install_gcc-11-mamba.sh`)
- New test data for builder with hyphen in key (`cpp/data/test/builder/hyphen_in_key.h`)
- Usage test for GCC11
- Support for overlay triplets in usage tests

## Changed

- Builder name sanitizing: renamed `sanitize_root_object_name()` to `sanitize_key()` and now sanitizes all property keys, not just root object names
- Configuration loading interface: removed `FUTURE_CONFIG_EXPORT` from template functions `load_config()` and `load_config_for_builder()`
- Template generation now uses sanitized names (`property.name`) for struct members while preserving original keys for config access
- CMakeLists.txt: added `loading.h` to the install target
- Documentation: Manual.md replaced with Developer Manual.md

## Fixed

- Missing `#include <optional>` in `loading.h`
- Configuration loading bugs: fixed optional handling in `load_config()` to properly handle empty config definitions
- Format library detection logic: Format library detection now uses `__has_include` instead of compile-time flag, improving compatibility
- Gitignore pattern fixed (changed from `build*/` to `**/build/*`)

