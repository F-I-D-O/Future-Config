/*
 * Copyright 2017 David Fiedler.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package ninja.fido.config.plugin;

import ninja.fido.config.JavaLanguageUtil;
import org.apache.maven.project.MavenProject;

/**
 * Utility class for maven plugins development
 * @author David Fiedler
 */
public class PluginTools {
    
    /**
     * Returns the name of the main package if it is in accordance to group and artefact name.
     * @param project Maven project instance.
     * @return Returns the name of the main package if it is in accordance to group and artefact name.
     */
	public static String getMainPackageName(MavenProject project) {
		return project.getArtifact().getGroupId() + "." + project.getArtifact().getArtifactId();
	}
	
    /**
     * Returns the path to the root resource directory.
     * @param project Maven project instance.
     * @return Returns the path to the root resource directory.
     */
	public static String getPathToProjectResourceDir(MavenProject project){
		String resourcePath = project.getFile().getPath().replace("pom.xml", "") + "src/main/resources/";
		return resourcePath + JavaLanguageUtil.packageToPath(getMainPackageName(project));
	}

    private PluginTools() {
    }
}
