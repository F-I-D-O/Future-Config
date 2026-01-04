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
- 