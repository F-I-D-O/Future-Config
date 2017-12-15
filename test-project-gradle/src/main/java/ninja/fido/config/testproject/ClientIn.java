package ninja.fido.config.testproject;

import java.io.File;
import ninja.fido.config.Configuration;
import ninja.fido.config.GeneratedConfig;
import ninja.fido.config.testproject.config.TestprojectConfig;

/**
 *
 * @author F.I.D.O.
 */
public class ClientIn {
	public static <C extends GeneratedConfig<C>> TestprojectConfig configure(C clientConfig){
		TestprojectConfig config = new TestprojectConfig();
		Configuration.load(config, clientConfig, "parent");
		return config;
	}
	
	public static <C extends GeneratedConfig<C>> TestprojectConfig configure(C clientConfig, File localConfigFile){
		TestprojectConfig config = new TestprojectConfig();
//		Configuration.load(config, clientConfig, localConfigFile, "parent");
		return config;
	}
}
