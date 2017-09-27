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

import com.google.common.collect.Lists;
import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author fido
 */
public class ConfigDataLoader {
//    public ConfigData loadConfigData(BufferedReader... configReaders) throws IOException{
//        ArrayList<Map<String,Object>> configDataList = new ArrayList<>();
//        for (BufferedReader configReader : configReaders) {
//            configDataList.add(new ConfigParser().parseConfigFile(configReader));
//        }
//        return new ConfigData(new ConfigDataResolver(configDataList).resolve());
//    }
    
    public ConfigData loadConfigData(Object... configSources) throws IOException{
        ConfigSource[] configSourceDefinitions = new ConfigSource[configSources.length];
        for (int i = 0; i < configSources.length; i++) {
            configSourceDefinitions[i] = new ConfigSource(configSources[i], (String[]) null);
        }
        return loadConfigData(configSourceDefinitions);
    }
    
    public ConfigData loadConfigData(ConfigSource... configSourceDefinitions) throws IOException{
        ArrayList<Map<String,Object>> configDataList = new ArrayList<>();
        for (ConfigSource configSourceDefinition : configSourceDefinitions) {
            Object source = configSourceDefinition.source;
            Map<String,Object> configMapFromSource = null;
            
            if(source instanceof BufferedReader){
                configMapFromSource = new Parser().parseConfigFile((BufferedReader) source);
            }
            else if(source instanceof Map){
                configMapFromSource = (Map) source;
            }
            
            if(configSourceDefinition.path != null){
                for(String objectName: Lists.reverse(configSourceDefinition.path)){
                    Map<String,Object> parentMap = new HashMap<>();
                    parentMap.put(objectName, configMapFromSource);
                    configMapFromSource = parentMap;
                }
            }
            
            configDataList.add(configMapFromSource);
        }
        return new ConfigData(new VariableResolver(new Merger(configDataList).merge()).resolveVariables());
    }
}
