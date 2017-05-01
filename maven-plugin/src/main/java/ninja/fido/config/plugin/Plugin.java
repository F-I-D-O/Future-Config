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
 * Says "Hi" to the user.
 *
 */
@Mojo(name = "build-config")
public class Plugin extends AbstractMojo
{
	
	@Parameter(property = "build-config.path")
	private String path;
	
//	@Parameter(property = "project.build.sourceDirectory")
//    private File outputDirectory;
	
	@Parameter(defaultValue = "${project}", required = true, readonly = true)
	MavenProject project;
	
	@Override
    public void execute() throws MojoExecutionException
    {
        getLog().info( "Hello, world." + path);
		File configFile = new File(path);
		MavenProject project = (MavenProject) getPluginContext().get("project");
		
		List roots = project.getCompileSourceRoots();
		getLog().info( project.getBasedir().getAbsolutePath());
//		new ConfigBuilder(file).buildConfig();
//		String mainPackagePath = getMainPackagePath();
		String srcPath = (String) project.getCompileSourceRoots().get(0);
		String mainPackageName = getMainPackageName();
		
		new ConfigBuilder(configFile, new File(srcPath), mainPackageName + ".config").buildConfig();
    }

	private String getMainPackagePath() {
		String srcDirPath = (String) project.getCompileSourceRoots().get(0);
		String packageDirPath = getDirPathFromGroupId();
		String finalDirName = project.getArtifact().getArtifactId();
		return srcDirPath + File.separator + packageDirPath + File.separator + finalDirName;
	}

	private String getDirPathFromGroupId() {
		return project.getArtifact().getGroupId().replace('.', File.separatorChar);
	}

	private String getMainPackageName() {
		return project.getArtifact().getGroupId() + "." + project.getArtifact().getArtifactId();
	}
}
