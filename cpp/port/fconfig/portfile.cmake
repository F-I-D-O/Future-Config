vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO F-I-D-O/Future-Config
	REF master
	SHA512 d8a9bc00ee312d04f5f575f5b1636d830d088ead93a9de662bdf2910b9ea6981bd3a202eb34e013b0ec99ef4fee75f8a315bccd1c8a2aaee27adad67b7f6699c
)

vcpkg_cmake_configure(
	SOURCE_PATH ${SOURCE_PATH}/cpp
	OPTIONS -DVCPKG_APPLOCAL_DEPS=ON
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(PACKAGE_NAME "FutureConfig")