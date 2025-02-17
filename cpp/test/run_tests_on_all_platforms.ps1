$starter_path = "$PSScriptRoot/ctest_starter.cmake" -replace '\\', '/'
$wsl_command = "ctest -S `$(wslpath $starter_path) -D FCONFIG_TEST_PLATFORM_NAME=<PLATFORM_NAME> -D FCONFIG_TOOLCHAIN='/opt/vcpkg/scripts/buildsystems/vcpkg.cmake'"

# run Windows tests
Write-Output "Running Windows tests"
Write-Output "---------------------"
ctest -S $starter_path -C Release -D FCONFIG_TEST_PLATFORM_NAME=Windows -D FCONFIG_TOOLCHAIN="c:/vcpkg/scripts/buildsystems/vcpkg.cmake" -D FCONFIG_TEST_VCPKG_INSTALL=ON
Write-Output ""

# run Windows shared library tests
Write-Output "Running Windows shared library tests"
Write-Output "------------------------------------"
ctest -S $starter_path -C Release -D FCONFIG_TEST_PLATFORM_NAME=Windows-shared -D FCONFIG_TOOLCHAIN="c:/vcpkg/scripts/buildsystems/vcpkg.cmake" -D FCONFIG_TEST_BUILD_SHARED=ON -D FCONFIG_TEST_VCPKG_INSTALL=ON
Write-Output ""

# run WSL tests
Write-Output "Running WSL tests"
Write-Output "-----------------"
$wsl_command_latest = "$wsl_command -D FCONFIG_TEST_VCPKG_INSTALL=ON" -replace '<PLATFORM_NAME>', 'WSL'
wsl -u root bash -lc $wsl_command_latest
Write-Output ""

# run WSL shared library tests
Write-Output "Running WSL shared library tests"
Write-Output "--------------------------------"
$wsl_command_shared = "$wsl_command -D FCONFIG_TEST_BUILD_SHARED=ON" -replace '<PLATFORM_NAME>', 'WSL-shared'
wsl -u root bash -lc $wsl_command_shared
Write-Output ""

# run WSL GCC 11 tests
Write-Output "Running WSL GCC 11 tests"
Write-Output "------------------------"
$wsl_command = "$wsl_command -D FCONFIG_TEST_CXX_COMPILER=g++-11" -replace '<PLATFORM_NAME>', 'WSL-GCC_11'
wsl -u root bash -lc $wsl_command
Write-Output ""

