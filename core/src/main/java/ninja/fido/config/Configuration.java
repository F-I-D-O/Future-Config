package ninja.fido.config;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Map;
import java.util.logging.Level;
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

//	private static final String DEFAULT_CONFIG_LOCATION_FILENAME = "config-location.txt";
//
//	private static final String DEFAULT_LOCAL_CONFIG_LOCATION_FILENAME = "local-config-location.txt";

	
	
	
	
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
                Map<String,Object> configMap = ((ConfigDataMap) config).getInternalObjects();
                generatedConfig.fill((Map) configMap.get(keyInClient));
                generatedConfig.fill((Map) configMap.get(keyInClient));

            } catch (FileNotFoundException ex) {
                LOGGER.error(ex.getMessage());
            }
        }
    }
    
    public static <C extends GeneratedConfig<C>, CC extends GeneratedConfig<CC>> void load(
            C generatedConfig, CC clientGeneratedConfig, String keyInClient){
        ConfigSource defaultConfigSource = new ConfigSource(getDefaultConfig(generatedConfig), keyInClient);
        ConfigSource defaultClientConfigSource 
                    = new ConfigSource(getDefaultConfig(clientGeneratedConfig), null);
        ConfigDataMap config = new ConfigDataLoader().loadConfigData(defaultConfigSource, defaultClientConfigSource);
        
        Map<String,Object> configMap = ((ConfigDataMap) config).getInternalObjects();
        generatedConfig.fill((Map) configMap.get(keyInClient));
        clientGeneratedConfig.fill((Map) configMap);
    }
    
    public static <C extends GeneratedConfig<C>> void load(C generatedConfig){
        ConfigSource defaultConfigSource = new ConfigSource(getDefaultConfig(generatedConfig), null);
        ConfigDataMap config = new ConfigDataLoader().loadConfigData(defaultConfigSource);
        
        Map<String,Object> configMap = ((ConfigDataMap) config).getInternalObjects();
        generatedConfig.fill((Map) configMap);
    }
    
    private static <C extends GeneratedConfig<C>> BufferedReader getDefaultConfig(C generatedConfig){
        String resourcePath = getConfigPath(generatedConfig, DEFAULT_CONFIG_FILENAME);
        InputStream inputStream = generatedConfig.getClass().getResourceAsStream(resourcePath);
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
        return bufferedReader;
    }


//	/**
//	 * Loads config from config fillepath configured in maven property config-location.
//	 *
//	 * @param <C> Generated config root class type.
//	 * @param generatedConfig Generated config root class.
//	 * @return Config root class containig all variable and object of variables defined by config file.
//	 */
//	public static <C extends GeneratedConfig<C>> C load(C generatedConfig) {
//		return load(generatedConfig, null);
//	}
//
//	/**
//	 * Loads config from config fillepath configured in maven property config-location.
//	 *
//	 * @param <C> Generated config root class type.
//	 * @param generatedConfig Generated config root class.
//	 * @param localConfigPath Absolute path to local config file
//	 * @return Config root class containing all variable and object of variables defined by config file.
//	 */
//	public static <C extends GeneratedConfig<C>> C load(C generatedConfig, String localConfigPath) {
//		ConfigDataMap baseConfig
//				= getConfig(generatedConfig, getConfigFilePath(generatedConfig, DEFAULT_CONFIG_LOCATION_FILENAME));
//
//		// load local config
//		if (localConfigPath == null) {
//			localConfigPath = getConfigFilePath(generatedConfig, DEFAULT_LOCAL_CONFIG_LOCATION_FILENAME);
//		}
//
//		ConfigDataMap localConfig = getConfig(generatedConfig, localConfigPath);
//
//		if (localConfig != null) {
//			baseConfig.override(localConfig);
//		}
//
//		C config = generatedConfig.fill(baseConfig.getConfig());
//		return config;
//	}

//	private static String getConfigFilePath(GeneratedConfig generatedConfig, String configLocationFilename) {
//		String configPath
//				= getConfigPath(generatedConfig, getConfigLocationFilePath(generatedConfig, configLocationFilename));
//		return configPath;
//	}

//	private static ConfigDataMap getConfig(GeneratedConfig generatedConfig, String configPath) {
//		LOGGER.log(Level.FINE, "Config file location: {0}", configPath);
//
//		if (configPath == null) {
//			return null;
//		}
//
//		ConfigDataMap config;
//		File configFile;
//		if (isRelativePath(configPath)) {
//			configPath = configPath.replaceFirst("\\.", "");
//			LOGGER.log(Level.FINE, "Relative config file location, changed to: {0}", configPath);
////            configFile = new File(Configuration.class.getResource(configPath.replaceFirst(".", "")).getFile());
//			InputStream inputStream = generatedConfig.getClass().getResourceAsStream(configPath);
//			BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
//			try {
//				config = new Parser().parseConfigFile(reader);
//			}
//			catch (IOException ex) {
//				LOGGER.log(Level.SEVERE, "Error loading config file: {0}", configPath);
//				return null;
//			}
//		}
//		else {
//			configFile = new File(configPath);
//			try {
//				config = new Parser().parseConfigFile(configFile);
//			}
//			catch (IOException ex) {
//				System.err.println("Config file not found at location: " + configFile.getAbsolutePath());
//				//            Logger.getLogger(Configuration.class.getName()).log(Level.SEVERE, null, ex);
//				return null;
//			}
//		}
//
//		return config;
//	}

	/**
	 * Search resources for config path.
	 *
	 * @param pathToConfigPathFile Relative path to config file.
	 * @return Returns path to config file if the config location file is in resources.
	 */
//	private static String getConfigPath(GeneratedConfig generatedConfig, String pathToConfigPathFile) {
//		LOGGER.log(Level.FINE, "Config location file path: {0}", pathToConfigPathFile);
//
//		File file = null;
//
//		try {
//			file = new File(
//					generatedConfig.getClass().getResource(pathToConfigPathFile).getFile());
//			LOGGER.log(Level.FINE, "Config location file found at: {0}", file);
//		}
//		catch (NullPointerException npe) {
//			LOGGER.log(Level.SEVERE, "Config location file not found");
//			return null;
//		}
//
//		InputStream inputStream = generatedConfig.getClass().getResourceAsStream(pathToConfigPathFile);
//		BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
//
//		String path = null;
//		try {
//			path = reader.readLine();
//		}
//		catch (IOException ex) {
//			LOGGER.log(Level.SEVERE, "Failed to load config file location from: {0}", file);
//		}
//
////        File file = null;
////        
////        System.out.println("Config location file path: " + pathToConfigPathFile);
////        
////        System.out.println("Resource from class: " + generatedConfig.getClass());
////        
////        try{
////            file = new File(
////                    generatedConfig.getClass().getResource(pathToConfigPathFile).getFile());
//////            file = new File(Configuration.class.getResource(pathToConfigPathFile).getFile());
////        }
////        catch(NullPointerException npe){
////            return null;
////        }
////        System.out.println("Config location file loaded from: " + file);
////		String path = null;
////		try (Scanner scanner = new Scanner(file)) {
////			path = scanner.nextLine();
////			scanner.close();
////		}
////        catch(FileNotFoundException fileNotFoundException){
////            return null;
////        }
////
//		return path;
//	}

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

}
