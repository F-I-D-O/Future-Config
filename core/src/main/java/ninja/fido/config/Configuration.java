package ninja.fido.config;

import java.io.File;
import java.io.IOException;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author F.I.D.O.
 */
public class Configuration {
	
	private static final String DEFAULT_CONFIG_PATH_LOCATION = "config-location.txt";
    
    public static <C extends BuildedConfig<C>> C load(C buildedConfig){
		String configPath = getConfigPath(buildedConfig);
		C config = null;
		try {
			config = buildedConfig.fill(new ConfigParser().parseConfigFile(new File(configPath)).getConfig());
		} catch (IOException ex) {
			Logger.getLogger(Configuration.class.getName()).log(Level.SEVERE, null, ex);
		}
		return config;
    }

	private static String getConfigPath(BuildedConfig buildedConfig) {
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

//	private static <C extends Object & BuildedConfig> String getConfigPath(C buildedConfig) {
//		Model model = null;
//		FileReader reader = null;
//		MavenXpp3Reader mavenreader = new MavenXpp3Reader();
//		try {
//			reader = new FileReader(pomfile);
//			model = mavenreader.read(reader);
//			model.setPomFile(pomfile);
//			MavenProject project = new MavenProject(model);
//			project.getProperties();
//		}catch(Exception ex){}
//		
//	}
}
