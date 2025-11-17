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

