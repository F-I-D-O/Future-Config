
# Testing
There are many types of tests in the C++ implementation of Future Config. All tests are located in the `test` directory.

There are unit tests that can be execuded by running the `test_run` target

Additionally, there are tests that can executed by PowerShell scripts in the `test` directory. Note that these scripts require a preconfigured Windows and WSL environment to run the tests on all platforms.

- Smoke tests (`run_main_tests.ps1`): test the configuration, build and unit tests on various platforms.
- Usage tests (`run_usage_tests.ps1`): Test the usage of the library in client projects.
- Vcpkg tests (`run_vcpkg_tests.ps1`): Test the integration of the library with the vcpkg package manager for various triplets.

Both the smoke tests and the usage tests use Vcpkg to get the library dependencies. On each platform, the default triplet is used. In contrast, the Vcpkg tests use various triplets.

Note that so far, the usage tests do not install the dependencies. Be sure to install them manually if you want to run all the tests without errors:
```bash
vcpkg install yaml-cpp inja spdlog tclap
```