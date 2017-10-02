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
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import static ninja.fido.config.Parser.REFERENCE_PATTERN;

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

	public ConfigDataMap loadConfigData(Object... configSources) throws IOException {
		ConfigSource[] configSourceDefinitions = new ConfigSource[configSources.length];
		for (int i = 0; i < configSources.length; i++) {
			configSourceDefinitions[i] = new ConfigSource(configSources[i], (String[]) null);
		}
		return loadConfigData(configSourceDefinitions);
	}

	public ConfigDataMap loadConfigData(ConfigSource... configSourceDefinitions) {
		ArrayList<ConfigDataMap> configDataList = new ArrayList<>();
		for (ConfigSource configSourceDefinition : configSourceDefinitions) {
			Object source = configSourceDefinition.source;
			ConfigDataMap configMapFromSource = null;

			if (source instanceof BufferedReader) {
                try {
                    configMapFromSource = new Parser().parseConfigFile((BufferedReader) source);
                } catch (IOException ex) {
                    Logger.getLogger(ConfigDataLoader.class.getName()).log(Level.SEVERE, null, ex);
                }
			}
			else if (source instanceof ConfigDataMap) {
				configMapFromSource = (ConfigDataMap) source;
			}

			if (configSourceDefinition.path != null) {
				configMapFromSource = changeConfigContext(configMapFromSource, configSourceDefinition.path);
			}

			configDataList.add(configMapFromSource);
		}
		return new VariableResolver(new Merger(configDataList).merge()).resolveVariables();
	}

	private ConfigDataMap changeConfigContext(ConfigDataMap configMapFromSource, List<String> path) {
		for (String objectName : Lists.reverse(path)) {

			/* add prefix to all variables path */
			for (ConfigProperty configProperty : configMapFromSource.getVariableIterable()) {
				Matcher matcher = REFERENCE_PATTERN.matcher((String) configProperty.value);
				matcher.find();
				String result = matcher.replaceAll("\\$" + objectName + ".$1");
				configProperty.set(result);
			}

			/* move object to new parent */
			ConfigDataMap parentMap = new ConfigDataMap(new HashMap<>(), null, null);
			parentMap.put(objectName, new ConfigDataMap(configMapFromSource.configObject, parentMap, objectName));
			configMapFromSource = parentMap;
		}
		return configMapFromSource;
	}
}
