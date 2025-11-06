# Dependencies

To install all dependencies using vcpkg, run:
```bash
vcpkg install yaml-cpp inja spdlog tclap
```

# Testing
There are three types of tests in the project:

- Unit Tests: Testing individual function
  - to run them, build and execute the `test_run` target
- Smoke Tests: Testing the configuration, build, and installation on various platforms
    - to run them, execute `test/run_tests_on_all_platforms.ps1`
- Usage Tests: Testing the usage of the future-config in client code
  - to run them, execute `test/run_usage_tests.ps1`
