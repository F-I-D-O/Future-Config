# 1.1.0

## Changed
- `loader.get_config_content_from_resource`
    - `sys.exit()` changed to exceptions
    - `config_name` param is no longer ignored
- only one template is now used internally for bth root and normal config file
    
## Added
- docstring comments for several methods:
    - `loader.get_config_content_from_resource`
    
## Fixed
- array generation in builder for root file

## Tests
- new test for correct variable overiding added to `resolver_test.py`: `test_config_override`
- builder tests