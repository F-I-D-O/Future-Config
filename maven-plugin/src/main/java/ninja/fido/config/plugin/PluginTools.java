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
 *
 * @author David Fiedler
 */
public class PluginTools {
	public static String getMainPackageName(MavenProject project) {
		return project.getArtifact().getGroupId() + "." + project.getArtifact().getArtifactId();
	}
	
	public static String getPathToProjectResourceDir(MavenProject project){
		String resourcePath = project.getFile().getPath().replace("pom.xml", "") + "src/main/resources/";
		return resourcePath + JavaLanguageUtil.packageToPath(getMainPackageName(project));
	}
}
