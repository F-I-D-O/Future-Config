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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Map;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * This class expose config loading for projects.
 *
 * @author F.I.D.O.
 */
public class Configuration {
	
	/**
	 * Default config package both for config file (in resources) and for generated sources.
	 */
	public static final String DEFAULT_CONFIG_PACKAGE = "config";
	
	public static final String DEFAULT_CONFIG_FILENAME = "config.cfg";

	private static final Logger LOGGER = LoggerFactory.getLogger(Configuration.class);

    
	
	
    /**
     * Loads configuration using config file, client config file and local config file.
     * @param <C> Config type
     * @param <CC> Client config type
     * @param generatedConfig Config class
     * @param clientGeneratedConfig Client config class
     * @param clientLocalConfigFile Local config file
     * @param keyInClient Config object name in client config.
     */
	public static <C extends GeneratedConfig<C>, CC extends GeneratedConfig<CC>> void load(
            C generatedConfig, CC clientGeneratedConfig, File clientLocalConfigFile, String keyInClient){
        if(clientGeneratedConfig == null){
            load(generatedConfig);
        }
        else if(clientLocalConfigFile == null){
            load(generatedConfig, clientGeneratedConfig, keyInClient);
        }
        else{
            ConfigSource defaultConfigSource = new ConfigSource(getDefaultConfig(generatedConfig), keyInClient);
            ConfigSource defaultClientConfigSource 
                        = new ConfigSource(getDefaultConfig(clientGeneratedConfig), null);
            try {
                ConfigSource localClientConfigSource
                        = new ConfigSource(new BufferedReader(new FileReader(clientLocalConfigFile)), null);
                ConfigDataMap config = new ConfigDataLoader().loadConfigData(
                        defaultConfigSource, defaultClientConfigSource, localClientConfigSource);
                Map<String,Object> configMap = config.getInternalObjects();
                generatedConfig.fill((Map) configMap.get(keyInClient));
                clientGeneratedConfig.fill(configMap);

            } catch (FileNotFoundException ex) {
                LOGGER.error(ex.getMessage());
            }
        }
    }
    
    /**
     * Loads configuration using config file and client config file.
     * @param <C> Config type
     * @param <CC> Client config type
     * @param generatedConfig Config class
     * @param clientGeneratedConfig Client config class
     * @param keyInClient Config object name in client config.
     */
    public static <C extends GeneratedConfig<C>, CC extends GeneratedConfig<CC>> void load(
            C generatedConfig, CC clientGeneratedConfig, String keyInClient){
        ConfigSource defaultConfigSource = new ConfigSource(getDefaultConfig(generatedConfig), keyInClient);
        ConfigSource defaultClientConfigSource 
                    = new ConfigSource(getDefaultConfig(clientGeneratedConfig), null);
        ConfigDataMap config = new ConfigDataLoader().loadConfigData(defaultConfigSource, defaultClientConfigSource);
        
        Map<String,Object> configMap = config.getInternalObjects();
        generatedConfig.fill((Map) configMap.get(keyInClient));
        clientGeneratedConfig.fill(configMap);
    }
    
    /**
     * Loads configuration using one config file only.
     * @param <C> Config type
     * @param generatedConfig Config class
     */
    public static <C extends GeneratedConfig<C>> void load(C generatedConfig){
        ConfigSource defaultConfigSource = new ConfigSource(getDefaultConfig(generatedConfig), null);
        ConfigDataMap config = new ConfigDataLoader().loadConfigData(defaultConfigSource);
        
        Map<String,Object> configMap = config.getInternalObjects();
        generatedConfig.fill(configMap);
    }
    
    private static <C extends GeneratedConfig<C>> BufferedReader getDefaultConfig(C generatedConfig){
        String resourcePath = getConfigPath(generatedConfig, DEFAULT_CONFIG_FILENAME);
        InputStream inputStream = generatedConfig.getClass().getResourceAsStream(resourcePath);
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
        return bufferedReader;
    }

	private static String getConfigPath(GeneratedConfig generatedConfig, String filename) {
		return JavaLanguageUtil.DIR_SEPARATOR + getPathToClass(generatedConfig.getClass())
                + JavaLanguageUtil.DIR_SEPARATOR + filename;
	}

	private static String getPathToClass(Class type) {
		String cannonicalName = type.getCanonicalName();
		String packageName = cannonicalName.substring(0, cannonicalName.lastIndexOf('.'));
		return JavaLanguageUtil.packageToPath(packageName);
	}

	private static boolean isRelativePath(String path) {
		return path.startsWith(".");
	}

    private Configuration() {
    }

}
