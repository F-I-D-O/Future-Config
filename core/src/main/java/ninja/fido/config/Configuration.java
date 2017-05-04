package ninja.fido.config;

import java.io.File;
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
    
    
    /**
     * Loads config from config fillepath configured in maven property config-location.
     * @param <C> Generated config root class type.
     * @param generatedConfig Generated config root class.
     * @return Config root class containig all variable and object of variables defined by config file.
     */
    public static <C extends GeneratedConfig<C>> C load(C generatedConfig){
		String configPath = getConfigPath(generatedConfig);
		C config = null;
		try {
			config = generatedConfig.fill(new ConfigParser().parseConfigFile(new File(configPath)).getConfig());
		} catch (IOException ex) {
			Logger.getLogger(Configuration.class.getName()).log(Level.SEVERE, null, ex);
		}
		return config;
    }

	private static String getConfigPath(GeneratedConfig buildedConfig) {
		File file = new File(
				buildedConfig.getClass().getClassLoader().getResource(DEFAULT_CONFIG_PATH_LOCATION).getFile());	
		String path = null;
		try (Scanner scanner = new Scanner(file)) {
			path = scanner.nextLine();
			scanner.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return path;
	}
}
