package ninja.fido.config.testproject;

import java.io.File;
import ninja.fido.config.Configuration;
import ninja.fido.config.GeneratedConfig;
import ninja.fido.config.testproject.config.Config;

/**
 *
 * @author F.I.D.O.
 */
public class ClientIn {
	public static <C extends GeneratedConfig<C>> Config configure(C clientConfig){
		Config config = new Config();
		Configuration.load(config, clientConfig, "parent");
		return config;
	}
	
	public static <C extends GeneratedConfig<C>> Config configure(C clientConfig, File localConfigFile){
		Config config = new Config();
		Configuration.load(config, clientConfig, localConfigFile, "parent");
		return config;
	}
}
