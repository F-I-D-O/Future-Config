
function RunUsageTest{
    param([string]$path, [string]$platform, [string]$compiler, [switch]$fconfig_vcpkg_install, [switch]$debug, [string]$triplet_overlay_dir, [string]$triplet)

    $starter_path = "$PSScriptRoot/$path/ctest_starter.cmake" -replace '\\', '/'

    Write-Output "Running usage tests for path: $starter_path, platform: $platform, vcpkg install: $fconfig_vcpkg_install"
    Write-Output "------------------------------------------------------------------------------------------------"

    $config = If($debug) {"Debug"} Else {"Release"}

    $common_args = "-C $config -D FCONFIG_CTEST_CONFIGURATION=$config -D FCONFIG_TEST_PLATFORM_NAME=$platform -D FCONFIG_VCPKG_INSTALL=$fconfig_vcpkg_install"

    If ($platform -like "Windows*") {
        If (-not $triplet) {
            If ($platform -like "*shared*") {
                $triplet = "x64-windows"
                $link_shared = "ON"
            }
            Else{
                $triplet = "x64-windows-static"
                $link_shared = "OFF"
            }
        } Else {
            # If triplet is provided, determine link_shared from platform name or default to OFF
            If ($platform -like "*shared*") {
                $link_shared = "ON"
            }
            Else{
                $link_shared = "OFF"
            }
        }

        $command = "ctest -S $starter_path -D FCONFIG_TOOLCHAIN='c:/vcpkg/scripts/buildsystems/vcpkg.cmake' -D FCONFIG_VCPKG_TRIPLET=$triplet -D FCONFIG_TEST_BUILD_SHARED=$link_shared $common_args"

        if ($triplet_overlay_dir) {
            $command = "$command -D FCONFIG_VCPKG_TEST_OVERLAY_TRIPLET_DIR=$triplet_overlay_dir"
        }

        Write-Output "Running test command: $command"
        Invoke-Expression $command
    } Elseif ($platform -like "WSL*") {
        If (-not $triplet) {
            $triplet = "x64-linux"
        }
        $wsl_command = "ctest -S `$(wslpath $starter_path) -D FCONFIG_TOOLCHAIN='/opt/vcpkg/scripts/buildsystems/vcpkg.cmake' -D FCONFIG_VCPKG_TRIPLET=$triplet $common_args"
#        $wsl_command = "$wsl_command" -replace '<PLATFORM_NAME>', $platform

        if ($compiler -ne "") {
            $wsl_command = "$wsl_command -D FCONFIG_TEST_CXX_COMPILER=$compiler"
        }

        if ($triplet_overlay_dir) {
            $wsl_command = "$wsl_command -D FCONFIG_VCPKG_TEST_OVERLAY_TRIPLET_DIR=`$(wslpath $triplet_overlay_dir)"
        }

#        Write-Output "Running WSL tests: $wsl_command"
        wsl -u root bash -lc $wsl_command
    }

    Write-Output ""
}

# for these test, fconfig is installed locally using CMake
RunUsageTest -path "usage_test" -platform "Windows"
RunUsageTest -path "usage_test" -platform "Windows-shared"
RunUsageTest -path "usage_test" -platform "Windows-shared-debug" -debug
RunUsageTest -path "usage_test" -platform "WSL"
$triplet_overlay_dir = "$PSScriptRoot/vcpkg_gcc11" -replace '\\', '/'
RunUsageTest -path "usage_test" -platform "WSL-GCC_11" -compiler "g++-11" -triplet "x64-linux-gcc11" -triplet_overlay_dir $triplet_overlay_dir

## for these test, fconfig is installed using vcpkg
RunUsageTest -path "usage_test" -platform "Windows-vcpkg" -fconfig_vcpkg_install
RunUsageTest -path "usage_test" -platform "Windows-shared-vcpkg" -fconfig_vcpkg_install
RunUsageTest -path "usage_test" -platform "WSL-vcpkg" -fconfig_vcpkg_install