/*
 * Copyright 2017 fido.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package ninja.fido.config;

import java.util.ArrayList;
import java.util.Map;

/**
 *
 * @author fido
 */
public class Merger {
    private final ArrayList<ConfigDataMap> configDataList;
    
    
    private ConfigDataMap finalConfigData;
    
    
    

    public Merger(ArrayList<ConfigDataMap> configDataList) {
        this.configDataList = configDataList;
    }
    
    public ConfigDataMap merge(){
        finalConfigData = configDataList.get(0);
        for (int i = 1; i < configDataList.size(); i++) {
            overrideWith(configDataList.get(i));
        }
        
        return finalConfigData;
    }
    
    private void overrideWith(ConfigDataMap configData){
        overrideLevel(finalConfigData, configData);
    }
    
    private void overrideLevel(ConfigDataMap currentMap, ConfigDataMap<ConfigDataMap,Object> overridingMap){
        for (Map.Entry<String, Object> entry : overridingMap) {
            String key = entry.getKey();
            Object value = entry.getValue();
            if(value instanceof ConfigDataMap){
                overrideLevel((ConfigDataMap) currentMap.get(key), (ConfigDataMap<ConfigDataMap, Object>) value);
            }
            else{
                currentMap.put(key, value);
            }
        }
    }
}
