
function RunUsageTest{
    param([string]$path, [string]$platform, [string]$compiler, [switch]$fconfig_vcpkg_install)

    $starter_path = "$PSScriptRoot/$path/ctest_starter.cmake" -replace '\\', '/'

    Write-Output "Running usage tests for path: $starter_path, platform: $platform, vcpkg install: $fconfig_vcpkg_install"
    Write-Output "------------------------------------------------------------------------------------------------"

    $common_args = "-D FCONFIG_TEST_PLATFORM_NAME=$platform -D FCONFIG_VCPKG_INSTALL=$fconfig_vcpkg_install"

    If ($platform -like "Windows*") {
        If ($platform -like "*shared*") {
            $triplet = "x64-windows"
            $link_shared = "ON"
        }
        Else{
            $triplet = "x64-windows-static"
            $link_shared = "OFF"
        }

        $command = "ctest -S $starter_path -C Release -D FCONFIG_TOOLCHAIN='c:/vcpkg/scripts/buildsystems/vcpkg.cmake' -D FCONFIG_VCPKG_TRIPLET=$triplet -D FCONFIG_TEST_BUILD_SHARED=$link_shared $common_args"
        Invoke-Expression $command
#        ctest -S $starter_path -C Release -D FCONFIG_TEST_PLATFORM_NAME=$platform -D FCONFIG_TOOLCHAIN="c:/vcpkg/scripts/buildsystems/vcpkg.cmake" -D FCONFIG_VCPKG_TRIPLET=$triplet -D FCONFIG_TEST_BUILD_SHARED=$link_shared

    } Elseif ($platform -like "WSL*") {
        $wsl_command = "ctest -S `$(wslpath $starter_path) -D FCONFIG_TOOLCHAIN='/opt/vcpkg/scripts/buildsystems/vcpkg.cmake' -D FCONFIG_VCPKG_TRIPLET=x64-linux $common_args"
#        $wsl_command = "$wsl_command" -replace '<PLATFORM_NAME>', $platform

        if ($compiler -ne "") {
            $wsl_command = "$wsl_command -D FCONFIG_TEST_CXX_COMPILER=$compiler"
        }

#        Write-Output "Running WSL tests: $wsl_command"
        wsl -u root bash -lc $wsl_command
    }

    Write-Output ""
}

RunUsageTest -path "usage_test" -platform "Windows"
RunUsageTest -path "usage_test" -platform "Windows-shared"
RunUsageTest -path "usage_test" -platform "WSL"

RunUsageTest -path "usage_test" -platform "Windows-vcpkg" -fconfig_vcpkg_install
RunUsageTest -path "usage_test" -platform "Windows-shared-vcpkg" -fconfig_vcpkg_install
#RunUsageTest -path "usage_test" -platform "WSL-vcpkg" -fconfig_vcpkg_install