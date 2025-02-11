
function RunUsageTest{
    param([string]$path, [string]$platform, [string]$compiler)

    $starter_path = "$PSScriptRoot/$path/ctest_starter.cmake" -replace '\\', '/'

    Write-Output "Running usage tests for path: $starter_path, platform: $platform"
    Write-Output "------------------------------------------------------------------------------------------------"

    If ($platform -like "Windows*") {
        If ($platform -like "*shared*") {
            $triplet = "x64-windows"
        }
        Else{
            $triplet = "x64-windows-static"
        }

        ctest -S $starter_path -C Release -D FCONFIG_TEST_PLATFORM_NAME=$platform -D FCONFIG_TOOLCHAIN="c:/vcpkg/scripts/buildsystems/vcpkg.cmake" -D FCONFIG_VCPKG_TRIPLET=$triplet

    } Elseif ($platform -like "WSL*") {
        $wsl_command = "ctest -S `$(wslpath $starter_path) -D FCONFIG_TEST_PLATFORM_NAME=<PLATFORM_NAME> -D FCONFIG_TOOLCHAIN='/opt/vcpkg/scripts/buildsystems/vcpkg.cmake' -D FCONFIG_VCPKG_TRIPLET=x64-linux"
        $wsl_command = "$wsl_command" -replace '<PLATFORM_NAME>', $platform

        if ($compiler -ne "") {
            $wsl_command = "$wsl_command -D FCONFIG_TEST_CXX_COMPILER=$compiler"
        }

        Write-Output "Running WSL tests: $wsl_command"
        wsl -u root bash -lc $wsl_command
    }

    Write-Output ""
}

RunUsageTest -path "usage_test" -platform "Windows"
RunUsageTest -path "usage_test" -platform "Windows-shared"
RunUsageTest -path "usage_test" -platform "WSL"