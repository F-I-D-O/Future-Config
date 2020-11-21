# Future-Config
Future config is a configuration format and a set of libraries to use the config files.
Currently supported languages are Python and Java.

## Why another config format?
Future-Config is designed for complicated projects with dozens or hundereds configurable properties that needs to be appropriately structured. 
If you want simple config files, you should opt for some well-established alternative.
The differenc ebetween Future-Config and other config/text formats is displayed in table bellow:

|                         | Future-Config | ini | XML | YAML | TOML | JSON |
|-------------------------|---------------|-----|-----|------|------|------|
| Simple Datatypes        | yes           | yes | yes | yes  | yes  | yes  |
| Objects                 | yes           | yes | yes | yes  | yes  | yes  |
| Arrays                  | yes           | no  | yes | yes  | yes  | yes  |
| Variables               | yes           | no  | no  | no   | no   | no   |
| Comments                | yes           | yes | yes | yes  | yes  | no   |
| Config Inheritance      | yes           | no  | no  | no   | no   | no   |
| Config Class Generation | yes           | no  | yes | yes  | no   | yes  |
| Unlimited Hierarchy     | yes           | no  | yes | yes  | yes  | yes  |

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
