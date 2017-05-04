package ninja.fido.config.plugin;

import java.io.File;
import java.util.List;
import ninja.fido.config.ConfigBuilder;
import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.plugins.annotations.Mojo;
import org.apache.maven.plugins.annotations.Parameter;
import org.apache.maven.project.MavenProject;
 
/**
 * FutureConfig plugin. It simply run a build config procedure in your project's root package directory using 
 * the config file suplied in the path parametr.
 */
@Mojo(name = "build-config")
public class Plugin extends AbstractMojo
{
	
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
    public void execute() throws MojoExecutionException
    {
		File configFile = new File(path);
		String srcPath = (String) project.getCompileSourceRoots().get(0);
		String mainPackageName = getMainPackageName();
		
		new ConfigBuilder(configFile, new File(srcPath), mainPackageName + ".config").buildConfig();
    }

	private String getMainPackageName() {
		return project.getArtifact().getGroupId() + "." + project.getArtifact().getArtifactId();
	}
}
