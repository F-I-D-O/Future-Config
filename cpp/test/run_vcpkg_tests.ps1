function RunVCPKGTest{
    param([string]$triplet, [switch]$wsl, [string]$triplet_overly_dir)

    $starter_path = "$PSScriptRoot/vcpkg_tests/ctest_starter.cmake" -replace '\\', '/'

    Write-Output "Running vcpkg tests for path: $starter_path, triplet: $triplet"
    Write-Output "--------------------------------------------------------------"

    $ctest_args = @('-C', 'Release', '-D', "FCONFIG_VCPKG_TEST_TRIPLET=$triplet")

    If ($wsl) {
        if ($triplet_overly_dir) {
            $ctest_args += @('-D', "FCONFIG_VCPKG_TEST_OVERLAY_TRIPLET_DIR=`$(wslpath $triplet_overly_dir)")
        }

        $ctest_command = @('ctest', '-S', "`$(wslpath $starter_path)") + $ctest_args
        wsl -u root bash -lc "$ctest_command"
    } Else {
        if ($triplet_overly_dir) {
            $ctest_args += @('-D', "FCONFIG_VCPKG_TEST_OVERLAY_TRIPLET_DIR=$triplet_overly_dir")
        }

        $command = @('-S', "$starter_path") + $ctest_args
        Write-Output "Running test command: ctest $command"
        & ctest $command
    }

    Write-Output "Executed ctest command: $ctest_command"
    Write-Output ""
}

RunVCPKGTest -triplet "x64-windows"
RunVCPKGTest -triplet "x64-windows-static"
RunVCPKGTest -triplet "x64-windows-static-md"

RunVCPKGTest -triplet "x64-linux" -wsl
$triplet_overly_dir = "$PSScriptRoot/vcpkg_gcc11" -replace '\\', '/'
RunVCPKGTest -triplet "x64-linux-gcc11" -wsl -triplet_overly_dir $triplet_overly_dir
