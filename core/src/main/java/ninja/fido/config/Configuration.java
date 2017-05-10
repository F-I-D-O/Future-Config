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
	
	private static final String DEFAULT_CONFIG_PATH_LOCATION = "config-location.txt";
    
    private static final String DEFAULT_LOCAL_CONFIG_PATH_LOCATION = "local-config-location.txt";
    
    
    /**
     * Loads config from config fillepath configured in maven property config-location.
     * @param <C> Generated config root class type.
     * @param generatedConfig Generated config root class.
     * @return Config root class containig all variable and object of variables defined by config file.
     */
    public static <C extends GeneratedConfig<C>> C load(C generatedConfig){
        
        // config path
		String configPath = getConfigPath(DEFAULT_CONFIG_PATH_LOCATION, generatedConfig);
        if(configPath == null){
            String path = new File(generatedConfig.getClass().getClassLoader()
                    .getResource(DEFAULT_CONFIG_PATH_LOCATION).getFile()).getAbsolutePath();
            try {
                throw new FileNotFoundException("Base config file not found in: " + path);
            } catch (FileNotFoundException ex) {
                Logger.getLogger(Configuration.class.getName()).log(Level.SEVERE, null, ex);
                return null;
            }
        }
        
        // local config path
        String localConfigPath = getConfigPath(DEFAULT_LOCAL_CONFIG_PATH_LOCATION, generatedConfig);
        
        Config baseConfig = null;
        try {
            baseConfig = new ConfigParser().parseConfigFile(new File(configPath));
        } catch (IOException ex) {
            Logger.getLogger(Configuration.class.getName()).log(Level.SEVERE, null, ex);
            return null;
        }
        
        // load local config
        Config localConfig = null;
        if(localConfigPath != null){
            try {
                localConfig = new ConfigParser().parseConfigFile(new File(localConfigPath));
            } catch (IOException ex) {
                Logger.getLogger(Configuration.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        
        if(localConfig != null){
            baseConfig.override(localConfig);
        }
        
        C config = generatedConfig.fill(baseConfig.getConfig());
		return config;
    }

	private static String getConfigPath(String pathToConfigPathFile, GeneratedConfig buildedConfig) {
        File file = null;
        try{
            file = new File(
                    buildedConfig.getClass().getClassLoader().getResource(pathToConfigPathFile).getFile());
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
}
