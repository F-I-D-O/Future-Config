from config_data_object import ConfigDataObject


def merge(config_data_list):
	final_config_data = config_data_list[0]
	for config_data_object in config_data_list[1:]:
		_override_level(final_config_data, config_data_object)

	return final_config_data


def _override_level(current_map: ConfigDataObject, overriding_map: ConfigDataObject):
	for key, value in overriding_map.items():
		if isinstance(value, ConfigDataObject) and current_map.get(key):
			_override_level(current_map.get(key), value)
		else:
			current_map.put(key, value)
