# Future Config
Future config is a configuration system for software projects and a set of libraries to make this system work in various programming languages.
Currently supported languages are C++, Python and Java.

Future Config can be used as a configuration system for any project but it is designed for complicated projects with dozens of configurable properties. For just a few properties, using program arguments or environment variables may be a better choice.

The main features of Future Config are:
- **Established Config File Format**: Future Config uses [YAML](https://yaml.org/) as the config file format. 
- **Default/User Configs**: The configuration is defined in a *default* config file specifying the default values for the project. The user can then create a *user* config file, which is supplied at runtime and overrides the values from the default config file. 
- **Config Class Generation**: Future Config generates config classes representing the config file structure. This allows for reliable and type-safe access to the config properties. The classes are generated automatically, no manual work is needed.
- **Config Hierarchy**: Future Config is designed to support the configuration of whole project dependency chains. This means that a *project B* depending on a *project A* can specify some configuration for *project A* in its own config file. Moreover, the user can then use a single config file for the whole project chain.
- **String Variables**: Future Config supports string variables/placeholders in the config file. This allows for the reuse of values and the composition of more complex values.

Note that Future Config is still not fully implemented. The following table shows the current status of the implementation in different programming languages.

| Feature | C++ | Python | Java |
|---------|-----|--------|------|
| Config File Format | YAML | custom temporary format | custom temporary format |
| Default/User Configs | yes | yes | yes |
| Config Class Generation | yes | yes | yes |
| Config Hierarchy | yes | yes | yes |
| String Variables | yes | yes | yes |
| Documentation | yes | no | no |

Although the config system should be identical in all supported languages, the necessary integration steps differ. Use the following links to get more information about the integration in a specific language:
- [C++](doc/C++.md)
- Python
- Java

## Why another config format?

The following table lists the features of Future-Config and the support of those features in different config formats.


|                         | Future-Config | ini | XML | YAML | TOML | JSON |
|-------------------------|---------------|-----|-----|------|------|------|
| Simple Datatypes        | yes           | yes | yes | yes  | yes  | yes  |
| Objects                 | yes           | yes | yes | yes  | yes  | yes  |
| Arrays                  | yes           | no  | yes | yes  | yes  | yes  |
| Variables               | yes           | no  | no  | no   | no   | no   |
| Unlimited Hierarchy     | yes           | no  | yes | yes  | yes  | yes  |
| Comments                | yes           | yes | yes | yes  | yes  | no   |
| Config Class Generation | yes           | no  | yes | yes  | no   | yes  |
| Master/Local Configs    | yes           | no  | no  | no   | no   | no   |
| Config Inheritance      | yes           | no  | no  | no   | no   | no   |



## Usage

### Python

1. `pip install fconfig`
2. Create your master config file in your project's resources
3. Generate config classes:
    ```
	from fconfig import configuration
	configuration.generate_config()
	```
4. Use the generated config classes:
	```
	import yourproject.confi.zourproject_config.config
	```


### Java


### C++
The C++ implementation of Future Config requires C++20 support. The library is available as a CMake package.

1. Install the package:
	- vcpkg: `vcpkg install future-config`
1. Modify your CMakeLists.txt:
	```cmake
	set(CMAKE_CXX_STANDARD 20) # or higher

	find_package(future-config CONFIG REQUIRED)
	target_link_libraries(your_target PRIVATE future-config::future-config)

	# Run the builder tool that generates the config classes
	run_fconfig_builder()
	```
1. create your master config file
	- by default, the file should be: `<CMakelists.txt directory>/data/config.yaml`.
	- alternatively, pass the custom path to the `run_fconfig_builder` function:
		```cmake
		run_fconfig_builder(MAIN_CONFIG_FILE <path_to_config_file>)
		```
1. configure your project using cmake
1. Now, your config classes are generated and ready to use in your project. Basic usage:
	```cpp
	// Include the generated config file
	#include "config/<your_project>_config.h"

	int main() {

		// load the config
		auto config = load<<Your_project>_config>(local_config_path);
	```

For more details, see the [C++ documentation](cpp/Manual.md).


## Syntax

### Base Types

```
string: "String"

int: 1

float: 1.0

boolean: true

negative_int: -3

negative_float: -2.0
```

### Objects
```
object:
{
	string: 'test'
	integer: 9
	float: 1.23
}
```

### Arrays
```
array:
[
	1
	5
	6
]
```

### Variables
```
string: 'string'

composed_string: 'composed ' + $string
```

### Complex Example
```
object_hierarchy:
{
	inner_object:
	{
		integer: 987654
		composed: $string + ' is funny to compose'
		inner_inner_object:
		{
			float: 9.87654
			composed: $object_hierarchy.inner_object.composed + ' multiple times'
			array:
			[
				1
				2
				3
			]
		}
		boolean: false
	}
	another_string: 'another_string'

	array_of_objects:
	[
		{
			animal: "bear"
			legs: 4
		}
		{
			animal: "chicken"
			legs: 2
		}
	]
}
```


# Principles
- Config propagation to config instances of the dependencies is handled automatically by the builder. The data transfer is a part of the generated code.
- There is only one config instance per project. Dependency config instance in the main/child project is the same as in the dependency project.
- dependency configs need to be specified in a form of key value pairs, where:
	- key is the name (key) of the in the master config file of the main project
	- value is the specification of the dependency config
