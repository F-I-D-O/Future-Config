/* 
 * Copyright 2017 fido.
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

import com.google.common.base.CaseFormat;
import com.squareup.javapoet.ClassName;
import com.squareup.javapoet.FieldSpec;
import com.squareup.javapoet.JavaFile;
import com.squareup.javapoet.MethodSpec;
import com.squareup.javapoet.MethodSpec.Builder;
import com.squareup.javapoet.TypeSpec;
import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.lang.model.element.Modifier;
import ninja.fido.config.ConfigDataList;
import ninja.fido.config.ConfigDataLoader;
import ninja.fido.config.ConfigDataMap;
import ninja.fido.config.Configuration;
import ninja.fido.config.GeneratedConfig;
import ninja.fido.config.JavaLanguageUtil;
import org.apache.commons.io.FileUtils;
import org.slf4j.LoggerFactory;

/**
 * Builds config classes from suplied config file.
 *
 * @author F.I.D.O.
 */
public class ConfigBuilder {
	
	private static final Pattern ARRAY_INDEXING_PATTERN = Pattern.compile("\\[[0-9]+\\]");
	
	private static final org.slf4j.Logger LOGGER = LoggerFactory.getLogger(Configuration.class);

	/**
	 * Configuration file
	 */
	private final BufferedReader configFile;

	/**
	 * Src dir of the project where all config files will be created. Most time the ...src/main/java/ dir.
	 */
	private final File outputSrcDir;

	/**
	 * Name of the config package - for example projectrootpackage.someotherpackage.config
	 */
	private final String configPackageName;
    
    private final String rootClassName;

	/**
	 * Constructor.
	 *
	 * @param configFile Config file.
	 * @param outputSrcDir Src dir of the project where all config files will be created. Most time the
	 * ...src/main/java/ dir.
	 * @param configPackageName Name of the config package - for example projectrootpackage.someotherpackage.config
     * @param rootClassName The name of the main config class.
	 */
	public ConfigBuilder(BufferedReader configFile, File outputSrcDir, String configPackageName, String rootClassName) {
		this.configFile = configFile;
		this.configPackageName = configPackageName;
		this.outputSrcDir = outputSrcDir;
        this.rootClassName = rootClassName;
	}

	/**
	 * Starts the building process.
	 */
	public void buildConfig() {
		deleteOldFiles();
		try {
			ConfigDataMap configMap = new ConfigDataLoader(true).loadConfigData(configFile);
			generateConfig(configMap, rootClassName, true);
		}
		catch (IOException ex) {
			Logger.getLogger(ConfigBuilder.class.getName()).log(Level.SEVERE, null, ex);
		}
	}
	
	private void deleteOldFiles(){
		String pathToOutputDir = outputSrcDir.getAbsolutePath() + JavaLanguageUtil.DIR_SEPARATOR 
				+ JavaLanguageUtil.packageToPath(configPackageName);
		File outputDir = new File(pathToOutputDir);
		if(outputDir.exists()){
			try { 
				FileUtils.cleanDirectory(new File(pathToOutputDir));
			}
			catch (IOException ex) {
				LOGGER.error(ex.getMessage());
			}
		}
	}

	private void generateConfig(ConfigDataMap configMap, String mapName, boolean isRoot) {

		TypeSpec.Builder objectBuilder
				= TypeSpec.classBuilder(getClassName(mapName)).addModifiers(Modifier.PUBLIC);
		
		Builder constructorBuilder = MethodSpec.constructorBuilder().addModifiers(Modifier.PUBLIC);
		
		

		String mapParamName = JavaLanguageUtil.getPropertyName(mapName);

		Builder parametrBuilder = constructorBuilder;

		if (isRoot) {
			// in root, properties are filled in fill method instead of the constructor
			parametrBuilder = MethodSpec.methodBuilder("fill").addModifiers(Modifier.PUBLIC)
					.returns(ClassName.get(configPackageName, getClassName(mapName)));

			// root class implements BuildedConfig interface
			objectBuilder.addSuperinterface(GeneratedConfig.class);
		}

		parametrBuilder.addParameter(Map.class, mapParamName);

		for (Entry<String, Object> entry : configMap) {
			String key = entry.getKey();
			Object value = entry.getValue();

			String propertyName = JavaLanguageUtil.getPropertyName(key);

			FieldSpec.Builder fieldBuilder;

			if (value instanceof ConfigDataMap) {
				ClassName newObjectType = ClassName.get(configPackageName, getClassName(key));
				generateConfig((ConfigDataMap) value, key, false);
				fieldBuilder = FieldSpec.builder(newObjectType, propertyName);
				parametrBuilder.addStatement("this.$N = new $T(($T) $N.get(\"$N\"))", propertyName, newObjectType,
						Map.class, mapParamName, key);
			}
			else if (value instanceof ConfigDataList) {
				ConfigDataList list = (ConfigDataList) value;
				fieldBuilder = FieldSpec.builder(List.class, propertyName);

				Object representative = list.get(0);
				if (representative instanceof ConfigDataMap) {
					/* representative generation */
					String itemName = configMap.getPath() == null ? key + "_item" 
							: configMap.getPath() + "_" + key + "_item";
					itemName = get_iteme_name_from_path(itemName);
					ClassName newObjectType = ClassName.get(configPackageName, getClassName(itemName));
					generateConfig((ConfigDataMap) representative, itemName, false);

					parametrBuilder.addStatement("this.$N = new $T()", propertyName, ArrayList.class);

					String inputListName = propertyName + "List";
					parametrBuilder.addStatement("$T $N = ($T) $N.get(\"$N\")", List.class, inputListName, List.class,
							mapParamName, key);
					String representativeObjectName = "object";
					parametrBuilder.beginControlFlow("for ($T $N: $N)", Object.class, representativeObjectName,
							inputListName);
					parametrBuilder.addStatement("$N.add(new $T(($T)$N))", propertyName, newObjectType, Map.class,
							representativeObjectName);
					parametrBuilder.endControlFlow();
				}
				else {
					parametrBuilder.addStatement("$N = ($T) $N.get(\"$N\")", propertyName, List.class,
							mapParamName, key);
				}
			}
			else {
				fieldBuilder = FieldSpec.builder(value.getClass(), propertyName);
				parametrBuilder.addStatement("this.$N = ($T) $N.get(\"$N\")", propertyName, value.getClass(),
						mapParamName, key);
			}
			objectBuilder.addField(fieldBuilder.addModifiers(Modifier.PUBLIC).build());
		}

		if (isRoot) {
			parametrBuilder.addStatement("return this");
			objectBuilder.addMethod(parametrBuilder.build());
		}
		
		// add reference to object in constructor
		if(isRoot){
			ClassName rootObjectClassName = ClassName.get(configPackageName, getClassName(mapName));
			objectBuilder.addField(rootObjectClassName, JavaLanguageUtil.getPropertyName(mapName), 
					Modifier.PUBLIC, Modifier.STATIC).build();
		}

		TypeSpec object = objectBuilder.addMethod(constructorBuilder.build()).build();
		
		JavaFile javaFile = JavaFile.builder(configPackageName, object).build();
		try {
			javaFile.writeTo(outputSrcDir);
		}
		catch (IOException ex) {
			Logger.getLogger(ConfigBuilder.class.getName()).log(Level.SEVERE, null, ex);
		}
	}

	private String getClassName(String name) {
		return CaseFormat.UPPER_UNDERSCORE.to(CaseFormat.UPPER_CAMEL, name);
	}

	private String get_iteme_name_from_path(String itemName) {
		Matcher matcher = ARRAY_INDEXING_PATTERN.matcher(itemName);
		if (matcher.find()) {
			itemName = matcher.replaceAll("");
		}
		itemName = itemName.replaceAll("\\.", "_");
		
		return itemName;
	}

}
