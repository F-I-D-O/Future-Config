/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ninja.fido.config;

import java.util.HashMap;
import java.util.Map;

/**
 * Class for encapsulating parsed configuration data.
 * @author fido
 */
public class Config {
    
    /**
     * Root config dictionary.
     */
    private final HashMap<String,Object> config;

    
    /**
     * Root config dictionary getter.
     * @return Root config dictionary
     */
	public HashMap<String, Object> getConfig() {
		return config;
	}
	
	

    
    /**
     * Construct object from root config dictionary.
     * @param config Root config dictionary.
     */
    public Config(HashMap<String, Object> config) {
        this.config = config;
    }
    
    
    
    
    /**
     * Config getter. Returns config value according to suplied path. If the path in config file is 
     * main.values.first, you shoul call: get("main", "values", "first"). 
     * @param <T> Type of the requested value.
     * @param path Config path.
     * @return Config value as specified by path.
     */
    public <T> T get(String... path){
        HashMap<String,Object> currentObject = config;
        for(int i = 0; i < path.length; i++){
            if(i == path.length - 1){
                return (T) currentObject.get(path[i]);
            }
            else{
                currentObject = (HashMap<String, Object>) currentObject.get(path[i]);
            }
        }
        return null;
    }
    
    public void override(Config config){
        overrideLevel(this.config, config.getConfig());
    }
    
    private void overrideLevel(HashMap<String, Object> currentMap, HashMap<String, Object> overridingMap){
        for (Map.Entry<String, Object> entry : overridingMap.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();
            if(value instanceof HashMap){
                overrideLevel((HashMap<String,Object>) currentMap.get(key), (HashMap<String,Object>) value);
            }
            else{
                currentMap.put(key, value);
            }
        }
    }
}
