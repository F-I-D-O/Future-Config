/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ninja.fido.config;

import java.util.HashMap;

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
}
