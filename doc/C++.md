For using the Future Config in C++, you need to install the future-config package first. Note that the package has dependencies, namely:
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [inja](https://github.com/pantor/inja)
- [spdlog](https://github.com/gabime/spdlog)

You can install the future-config package:
- from vcpkg: `vcpkg install future-config`, or
- manually: 
    1. install the dependencies
    1. Clone the repository
    2. crate some build directory anywhere
    3. configure the project with cmake: `cmake <path_to_fconfig_repo>`
    1. install the project: `cmake --build . --target install`

To use the Future Config in your CMake project, you need to add the following lines to your `CMakeLists.txt`:
```cmake
find_package(future-config CONFIG REQUIRED) # Find the future-config package

target_link_libraries(<your target> PRIVATE future-config::future-config) # Link the library

setup_fconfig( # Generate config classes and copy master config files for the target
	TARGET_NAMES <your target>
)

```
The `setup_fconfig` function has the following arguments:
- `TARGET_NAMES`: the targets that need the generated configuration and copied master config files.
- `MAIN_CONFIG_PATHS`: the paths to the main config files for the project. By default, it is `<CMakelists.txt directory>/data/config.yaml`.
- `ROOT_CONFIG_CLASS_NAME`: the name of the root config class. By default, it is the name of the CMake project.
- `SOURCE_DIR`: the directory where the generated config classes will be placed. By default, it is `<CMakeLists.txt directory>/src`.

After CMake project configuration, you should see the generated config classes in the specified directory. To obtain the root config object in your code, you can use the following code:

```cpp
#include "configuration.h"
#include "<root config include path>"


std::vector<std::unique_ptr<fc::Config_definition>> config_definitions;
config_definitions.emplace_back(
    std::make_unique<fc::Config_definition>(fc::Config_type::MAIN, "<path_to_main_config_file>"));
auto config = load<<root config classname>>(config_definitions, filename);
```
