package ninja.fido.config.plugin;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import ninja.fido.config.Configuration;
import ninja.fido.config.JavaLanguageUtil;
import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.plugins.annotations.Mojo;
import org.apache.maven.plugins.annotations.Parameter;
import org.apache.maven.project.MavenProject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * FutureConfig plugin. It simply run a build config procedure in your project's root package directory using the config
 * file suplied in the path parametr.
 */
@Mojo(name = "build-config")
public class Plugin extends AbstractMojo {
	
	private static final Logger LOGGER = LoggerFactory.getLogger(Configuration.class);

	/**
	 * Absolute path to the config file.
	 */
	@Parameter(property = "build-config.path")
	private String path;

	/**
	 * Maven project.
	 */
	@Parameter(defaultValue = "${project}", required = true, readonly = true)
	private MavenProject project;

	@Override
	public void execute() throws MojoExecutionException {
		String mainPackageName = PluginTools.getMainPackageName(project);
		BufferedReader defaultConfigFile = getConfigFile();
		String srcPath = (String) project.getCompileSourceRoots().get(0);
		String configPackage = mainPackageName + "." + Configuration.DEFAULT_CONFIG_PACKAGE;
        String mainConfigClassName = project.getArtifactId() + "_config";

		new ConfigBuilder(defaultConfigFile, new File(srcPath), configPackage, mainConfigClassName).buildConfig();
	}

	private BufferedReader getConfigFile(){ 
		String path = PluginTools.getPathToProjectResourceDir(project) + JavaLanguageUtil.DIR_SEPARATOR
				+ Configuration.DEFAULT_CONFIG_PACKAGE + JavaLanguageUtil.DIR_SEPARATOR
				+ Configuration.DEFAULT_CONFIG_FILENAME;

		try {
			return new BufferedReader(new FileReader(path));
		}
		catch (FileNotFoundException ex) {
			LOGGER.error("Default config file not found at: {}", path);
			return null;
		}
	}
}
