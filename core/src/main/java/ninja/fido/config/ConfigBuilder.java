package ninja.fido.config;

import com.google.common.base.CaseFormat;
import com.squareup.javapoet.ClassName;
import com.squareup.javapoet.FieldSpec;
import com.squareup.javapoet.JavaFile;
import com.squareup.javapoet.MethodSpec;
import com.squareup.javapoet.MethodSpec.Builder;
import com.squareup.javapoet.TypeSpec;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.lang.model.element.Modifier;

/**
 * Builds config classes from suplied config file.
 * @author F.I.D.O.
 */
public class ConfigBuilder {
	
    /**
     * Configuration file
     */
	private final File configFile;

	/**
     * Src dir of the project where all config files will be created. Most time the ...src/main/java/ dir.
     */
	private final File outputSrcDir;
	
    /**
     * Name of the config package - for example projectrootpackage.someotherpackage.config
     */
	private final String configPackageName;

    
    
    
    /**
     * Constructor.
     * @param configFile Config file.
     * @param outputSrcDir Src dir of the project where all config files will be created.
     * Most time the ...src/main/java/ dir.
     * @param configPackageName Name of the config package - for example projectrootpackage.someotherpackage.config
     */
	public ConfigBuilder(File configFile, File outputSrcDir, String configPackageName) {
		this.configFile = configFile;
		this.configPackageName = configPackageName;
		this.outputSrcDir = outputSrcDir;
	}
	
	
	
    
    /**
     * Starts the building process.
     */
	public void buildConfig(){
		try {
			Config config = new ConfigParser().parseConfigFile(configFile);
			HashMap<String,Object> configMap = config.getConfig();
			generateConfig(configMap, "config", true);
		} catch (IOException ex) {
			Logger.getLogger(ConfigBuilder.class.getName()).log(Level.SEVERE, null, ex);
		}
	}


	private void generateConfig(HashMap<String, Object> configMap, String mapName, boolean isRoot) {
		
		Builder constructorBuilder = MethodSpec.constructorBuilder().addModifiers(Modifier.PUBLIC);
		TypeSpec.Builder objectBuilder 
				= TypeSpec.classBuilder(getClassName(mapName)).addModifiers(Modifier.PUBLIC);
		
		String mapParamName = getPropertyName(mapName);
		
		Builder parametrBuilder = constructorBuilder;
		
		if(isRoot){
			// in root, properties are filled in fill method instead of the constructor
			parametrBuilder = MethodSpec.methodBuilder("fill").addModifiers(Modifier.PUBLIC)
				.returns(ClassName.get(configPackageName, getClassName(mapName)));
			
			// root class implements BuildedConfig interface
			objectBuilder.addSuperinterface(GeneratedConfig.class);
		}

		parametrBuilder.addParameter(HashMap.class, mapParamName);
		
		for (Entry<String, Object> entry : configMap.entrySet()) {
			String key = entry.getKey();
			Object value = entry.getValue();

			String propertyName = getPropertyName(key);

			FieldSpec.Builder fieldBuilder;

			if(value instanceof Map){
				ClassName newObjectType = ClassName.get(configPackageName, getClassName(key));
				generateConfig((HashMap<String, Object>) value, key, false);
				fieldBuilder = FieldSpec.builder(newObjectType, propertyName);
				parametrBuilder.addStatement("this.$N = new $T(($T) $N.get(\"$N\"))", propertyName, newObjectType, 
						HashMap.class, mapName, key);
			}
			else{
				fieldBuilder = FieldSpec.builder(value.getClass(), propertyName);
				parametrBuilder.addStatement("this.$N = ($T) $N.get(\"$N\")", propertyName, value.getClass(), 
						mapParamName, key);
			}
			objectBuilder.addField(fieldBuilder.addModifiers(Modifier.PUBLIC).build());
		}

		if(isRoot){
			parametrBuilder.addStatement("return this");
			objectBuilder.addMethod(parametrBuilder.build());
		}
			
		TypeSpec object = objectBuilder.addMethod(constructorBuilder.build()).build();

		JavaFile javaFile = JavaFile.builder(configPackageName, object).build();
		try {
			javaFile.writeTo(outputSrcDir);
		} catch (IOException ex) {
			Logger.getLogger(ConfigBuilder.class.getName()).log(Level.SEVERE, null, ex);
		}
	}

	
	private String getClassName(String name){
		return CaseFormat.UPPER_UNDERSCORE.to(CaseFormat.UPPER_CAMEL, name);
	}
	
	private String getPropertyName(String name){
		return CaseFormat.UPPER_UNDERSCORE.to(CaseFormat.LOWER_CAMEL, name);
	}

}
