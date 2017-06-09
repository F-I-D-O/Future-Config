package ninja.fido.config;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * This class expose config loading for projects.
 * @author F.I.D.O.
 */
public class Configuration {
	
	private static final String DEFAULT_CONFIG_LOCATION_FILENAME = "config-location.txt";
    
    private static final String DEFAULT_LOCAL_CONFIG_LOCATION_FILENAME = "local-config-location.txt";
    
    
    /**
     * Loads config from config fillepath configured in maven property config-location.
     * @param <C> Generated config root class type.
     * @param generatedConfig Generated config root class.
     * @return Config root class containig all variable and object of variables defined by config file.
     */
    public static <C extends GeneratedConfig<C>> C load(C generatedConfig){
        Config baseConfig = getConfig(generatedConfig, DEFAULT_CONFIG_LOCATION_FILENAME);
        
        // load local config
        Config localConfig = getConfig(generatedConfig, DEFAULT_LOCAL_CONFIG_LOCATION_FILENAME);
        
        if(localConfig != null){
            baseConfig.override(localConfig);
        }
        
        C config = generatedConfig.fill(baseConfig.getConfig());
		return config;
    }
    
    private static Config getConfig(GeneratedConfig generatedConfig, String configLocationFilename){
        String configPath 
                = getConfigPath(getConfigLocationFilePath(generatedConfig, configLocationFilename));
        
        if(configPath == null){
            return null;
        }
        
        Config config;
        File configFile;
        if(isRelativePath(configPath)){
             configFile = new File(Configuration.class.getResource(configPath.replaceFirst(".", "")).getFile());
        }
        else{
            configFile = new File(configPath);
        }
        try {
            config = new ConfigParser().parseConfigFile(configFile);
        } catch (IOException ex) {
            System.err.println("Config file not found at location: " + configFile.getAbsolutePath());
//            Logger.getLogger(Configuration.class.getName()).log(Level.SEVERE, null, ex);
            return null;
        }
        return config;
    }

    /**
     * Search resources for config path.
     * @param pathToConfigPathFile Relative path to config file.
     * @return Returns path to config file if the config location file is in resources.
     */
	private static String getConfigPath(String pathToConfigPathFile) {
        File file = null;
        try{
//            file = new File(
//                    buildedConfig.getClass().getClassLoader().getResource(pathToConfigPathFile).getFile());
            file = new File(Configuration.class.getResource(pathToConfigPathFile).getFile());
        }
        catch(NullPointerException npe){
            return null;
        }
		String path = null;
		try (Scanner scanner = new Scanner(file)) {
			path = scanner.nextLine();
			scanner.close();
		}
        catch(FileNotFoundException fileNotFoundException){
            return null;
        }

		return path;
	}
    
    private static String getConfigLocationFilePath(GeneratedConfig generatedConfig, String filename){
        return File.separator + getPackageStructure(generatedConfig.getClass()) + File.separator + filename;
    }
    
    private static String getPackageStructure(Class type){
        String cannonicalName = type.getCanonicalName();
        String packageName = cannonicalName.substring(0, cannonicalName.lastIndexOf('.'));
        return packageName.replace('.', File.separatorChar);
    }
    
//    private static String buildAbsolutePath(Class type, String relativePath){
//        
//    }

    private static boolean isRelativePath(String path) {
        return path.startsWith(".");
    }
    
}
