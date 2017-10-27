/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ninja.fido.config;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * Class for encapsulating parsed configuration data.
 *
 * @author fido
 */
public class ConfigDataMap extends ConfigDataObject<Map<String, Object>, String, Object> {

	public ConfigDataMap(Map<String, Object> configObject, ConfigDataObject parentConfigObject, Object keyInParent) {
		super(configObject, parentConfigObject, keyInParent);
	}

	public ConfigDataMap(ConfigDataObject parentConfigObject, Object keyInParent) {
		super(new HashMap<>(), parentConfigObject, keyInParent);
	}

	@Override
	public Iterator<Map.Entry<String, Object>> iterator() {
		return configObject.entrySet().iterator();
	}

	@Override
	public Object get(String key) {
		return configObject.get(key);
	}

	@Override
	public boolean containsKey(String key) {
		return configObject.containsKey(key);
	}

    @Override
	public void put(String key, Object value) {
		configObject.put(key, value);
	}

	@Override
	public int getSize() {
		return configObject.size();
	}

    @Override
    public Map<String, Object> getInternalObjects() {
        Map<String,Object> out = new HashMap<>();
        for (Map.Entry<String, Object> entry : this) {
            Object value = entry.getValue();
            String key = entry.getKey();
            if(value instanceof ConfigDataObject){
                out.put(key, ((ConfigDataObject) value).getInternalObjects());
            }
            else{
                out.put(key, value);
            }
        }
        return out;
    }

}
