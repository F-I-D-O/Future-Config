package ninja.fido.config;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.logging.ConsoleHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * This class expose config loading for projects.
 *
 * @author F.I.D.O.
 */
public class Configuration {

	private static final Logger LOGGER = Logger.getLogger(Configuration.class.getName());

	private static final String DEFAULT_CONFIG_LOCATION_FILENAME = "config-location.txt";

	private static final String DEFAULT_LOCAL_CONFIG_LOCATION_FILENAME = "local-config-location.txt";

	private static final char DIR_SEPARATOR = '/';

	static {
		Handler handlerObj = new ConsoleHandler();
		handlerObj.setLevel(Level.ALL);
		LOGGER.addHandler(handlerObj);
		LOGGER.setLevel(Level.ALL);
		LOGGER.setUseParentHandlers(false);
	}

	/**
	 * Loads config from config fillepath configured in maven property config-location.
	 *
	 * @param <C> Generated config root class type.
	 * @param generatedConfig Generated config root class.
	 * @return Config root class containig all variable and object of variables defined by config file.
	 */
	public static <C extends GeneratedConfig<C>> C load(C generatedConfig) {
		return load(generatedConfig, null);
	}

	/**
	 * Loads config from config fillepath configured in maven property config-location.
	 *
	 * @param <C> Generated config root class type.
	 * @param generatedConfig Generated config root class.
	 * @param localConfigPath Absolute path to local config file
	 * @return Config root class containing all variable and object of variables defined by config file.
	 */
	public static <C extends GeneratedConfig<C>> C load(C generatedConfig, String localConfigPath) {
		ConfigDataMap baseConfig
				= getConfig(generatedConfig, getConfigFilePath(generatedConfig, DEFAULT_CONFIG_LOCATION_FILENAME));

		// load local config
		if (localConfigPath == null) {
			localConfigPath = getConfigFilePath(generatedConfig, DEFAULT_LOCAL_CONFIG_LOCATION_FILENAME);
		}

		ConfigDataMap localConfig = getConfig(generatedConfig, localConfigPath);

		if (localConfig != null) {
			baseConfig.override(localConfig);
		}

		C config = generatedConfig.fill(baseConfig.getConfig());
		return config;
	}

	private static String getConfigFilePath(GeneratedConfig generatedConfig, String configLocationFilename) {
		String configPath
				= getConfigPath(generatedConfig, getConfigLocationFilePath(generatedConfig, configLocationFilename));
		return configPath;
	}

	private static ConfigDataMap getConfig(GeneratedConfig generatedConfig, String configPath) {
		LOGGER.log(Level.FINE, "Config file location: {0}", configPath);

		if (configPath == null) {
			return null;
		}

		ConfigDataMap config;
		File configFile;
		if (isRelativePath(configPath)) {
			configPath = configPath.replaceFirst("\\.", "");
			LOGGER.log(Level.FINE, "Relative config file location, changed to: {0}", configPath);
//            configFile = new File(Configuration.class.getResource(configPath.replaceFirst(".", "")).getFile());
			InputStream inputStream = generatedConfig.getClass().getResourceAsStream(configPath);
			BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
			try {
				config = new Parser().parseConfigFile(reader);
			}
			catch (IOException ex) {
				LOGGER.log(Level.SEVERE, "Error loading config file: {0}", configPath);
				return null;
			}
		}
		else {
			configFile = new File(configPath);
			try {
				config = new Parser().parseConfigFile(configFile);
			}
			catch (IOException ex) {
				System.err.println("Config file not found at location: " + configFile.getAbsolutePath());
				//            Logger.getLogger(Configuration.class.getName()).log(Level.SEVERE, null, ex);
				return null;
			}
		}

		return config;
	}

	/**
	 * Search resources for config path.
	 *
	 * @param pathToConfigPathFile Relative path to config file.
	 * @return Returns path to config file if the config location file is in resources.
	 */
	private static String getConfigPath(GeneratedConfig generatedConfig, String pathToConfigPathFile) {
		LOGGER.log(Level.FINE, "Config location file path: {0}", pathToConfigPathFile);

		File file = null;

		try {
			file = new File(
					generatedConfig.getClass().getResource(pathToConfigPathFile).getFile());
			LOGGER.log(Level.FINE, "Config location file found at: {0}", file);
		}
		catch (NullPointerException npe) {
			LOGGER.log(Level.SEVERE, "Config location file not found");
			return null;
		}

		InputStream inputStream = generatedConfig.getClass().getResourceAsStream(pathToConfigPathFile);
		BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));

		String path = null;
		try {
			path = reader.readLine();
		}
		catch (IOException ex) {
			LOGGER.log(Level.SEVERE, "Failed to load config file location from: {0}", file);
		}

//        File file = null;
//        
//        System.out.println("Config location file path: " + pathToConfigPathFile);
//        
//        System.out.println("Resource from class: " + generatedConfig.getClass());
//        
//        try{
//            file = new File(
//                    generatedConfig.getClass().getResource(pathToConfigPathFile).getFile());
////            file = new File(Configuration.class.getResource(pathToConfigPathFile).getFile());
//        }
//        catch(NullPointerException npe){
//            return null;
//        }
//        System.out.println("Config location file loaded from: " + file);
//		String path = null;
//		try (Scanner scanner = new Scanner(file)) {
//			path = scanner.nextLine();
//			scanner.close();
//		}
//        catch(FileNotFoundException fileNotFoundException){
//            return null;
//        }
//
		return path;
	}

	private static String getConfigLocationFilePath(GeneratedConfig generatedConfig, String filename) {
		return DIR_SEPARATOR + getPackageStructure(generatedConfig.getClass()) + DIR_SEPARATOR + filename;
	}

	private static String getPackageStructure(Class type) {
		String cannonicalName = type.getCanonicalName();
		String packageName = cannonicalName.substring(0, cannonicalName.lastIndexOf('.'));
		return packageName.replace('.', DIR_SEPARATOR);
	}

//    private static String buildAbsolutePath(Class type, String relativePath){
//        
//    }
	private static boolean isRelativePath(String path) {
		return path.startsWith(".");
	}

}
