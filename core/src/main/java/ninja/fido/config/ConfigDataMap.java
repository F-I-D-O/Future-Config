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
 * @author fido
 */
public class ConfigDataMap<PT extends ConfigDataObject,PK> extends ConfigDataObject<Map<String,Object>,String,Object,PT,PK>{

    public ConfigDataMap(Map<String, Object> configObject, PT parentConfigObject, PK keyInParent) {
        super(configObject, parentConfigObject, keyInParent);
    }
    
    public ConfigDataMap(PT parentConfigObject, PK keyInParent) {
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
    
    public void put(String key, Object value){
        configObject.put(key, value);
        Map<String,Object> test = configObject;
    }
	
}
