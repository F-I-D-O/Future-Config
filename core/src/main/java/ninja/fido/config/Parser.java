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
package ninja.fido.config;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Stack;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Config file parser
 *
 * @author fido
 */
public class Parser {
    public static final Pattern OPERATOR_PATTERN = Pattern.compile("[+\\-]");
	
	static final String LEGAL_NAME_CHARACTER = "a-zA-Z0-9_";
	static final String NAME_PATERN_STRING = String.format("([a-zA-Z][%s]+)", LEGAL_NAME_CHARACTER);
    public static final Pattern REFERENCE_PATTERN
            = Pattern.compile(String.format("\\$(%s(\\.%s)*)", NAME_PATERN_STRING, NAME_PATERN_STRING));
    private static final Logger logger = LoggerFactory.getLogger(Parser.class);

	private static final Pattern WHITESPACE_LINE_PATTERN = Pattern.compile("^\\s*$");
	private static final Pattern INDENTION_PATTERN = Pattern.compile("^(    |	)*");
	private static final Pattern KEY_PATTERN = Pattern.compile("^" + NAME_PATERN_STRING + "(:)");
	private static final Pattern VALUE_PATTERN = Pattern.compile("^\\s*([^\\s]+.*)");
	private static final Pattern NUMBER_PATTERN = Pattern.compile("^([0-9])");
	private static final Pattern BOOLEAN_PATTERN = Pattern.compile("^(true|false)");
//	private static final Pattern OPERATOR_EXPRESSION_PATTERN = Pattern.compile("\\s*('[^']+'+)\\s*([+])?");
	private static final Pattern BUILDER_DIRECTIVE_PATTERN = Pattern.compile("^!([^\\s]*)");
    
    
    
    public static boolean containsVariable(Object expression) {
		if (expression == null || !(expression instanceof String)) {
			return false;
		}
		return ((String) expression).contains("$");
	}

	static Object parseSimpleValue(String value) {
		Matcher matcher = NUMBER_PATTERN.matcher(value);
		if (matcher.find()) {
			if (value.contains(".")) {
				return Double.parseDouble(value);
			}
			else {
				return Integer.parseInt(value);
			}
		}
		else {
			matcher = BOOLEAN_PATTERN.matcher(value);
			if (matcher.find()) {
				return Boolean.parseBoolean(value);
			}
			else {
				if (value.startsWith("'")) {
					return value.replace("'", "");
				}
				if (value.startsWith("\"")) {
					return value.replace("\"", "");
				}
				return value;
			}
		}
	}
    
    
	

	private final ConfigDataMap config;

	private final Stack<ConfigDataObject> objectStack;
	
	private final boolean useBuilderDirectives;

	private ConfigDataObject currentObject;

	private Object currentKey;

	private Object currentValue;

	private boolean inArray;
	
	private boolean skipNextObject;

	/**
	 * Constructor.
	 */
	public Parser(){
		this(false);
	}
			
	public Parser(boolean useBuilderDirectives) {
		this.useBuilderDirectives = useBuilderDirectives;
		config = new ConfigDataMap(new HashMap<>(), null, null);
		currentObject = config;
		objectStack = new Stack<>();
		inArray = false;
		skipNextObject = false;
	}

	/**
	 * Main method for parsing config file.
	 *
	 * @param configFile Config file.
	 * @return Config object containing all variables from config file.
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public ConfigDataMap parseConfigFile(File configFile) throws FileNotFoundException, IOException {
		try (BufferedReader br = new BufferedReader(new FileReader(configFile))) {
			return parseConfigFile(br);
		}
	}

	public ConfigDataMap parseConfigFile(BufferedReader configFileReader) throws FileNotFoundException, IOException {
		String line;
		while ((line = configFileReader.readLine()) != null) {

			/* skip blank lines */
			Matcher matcher = WHITESPACE_LINE_PATTERN.matcher(line);
			if (matcher.find()) {

			}

			/* comment line */
			else if (line.contains("#")) {
				// possible comment processing
			}
			
			/* comment line */
			else if (line.startsWith("!")) {
				if(useBuilderDirectives){
					resolveBuilderDirective(line);
				}
			}

			/* new array or object */
			else if (line.contains("{") || line.contains("[")) {

				/* push old context to stack */
				objectStack.push(currentObject);

				/* new object */
				if (line.contains("{")) {
					currentObject = new ConfigDataMap(currentObject, currentKey);
					inArray = false;
				}

				/* new array */
				if (line.contains("[")) {
					currentObject = new ConfigDataList(currentObject, currentKey);
					inArray = true;
				}
				
				/* add new object to parent object */
				if(skipNextObject){
					skipNextObject = false;
				}
				else{
					objectStack.peek().put(currentKey, currentObject);
				}

				if (inArray) {
					currentKey = 0;
				}
			}

			else if (line.contains("}") || line.contains("]")) {
				currentObject = objectStack.pop();
				if (currentObject instanceof ConfigDataMap) {
					inArray = false;
				}
				else {
					inArray = true;
					currentKey = currentObject.getSize();
				}
			}

			else {
				parseLine(line);
			}
		}

		return config;
	}

	private void parseLine(String line) {
		line = stripIndention(line);

		if (!inArray) {
			line = parseKey(line);
		}

		if (parseValue(line)) {
			currentObject.put(currentKey, currentValue);
		}
		
		if (inArray) {
			currentKey = (int) currentKey + 1;
		}
	}

	private String stripIndention(String line) {
		Matcher matcher = INDENTION_PATTERN.matcher(line);
		if (matcher.find()) {
			return matcher.replaceAll("");
		}
		else {
			return line;
		}
	}

	private String parseKey(String line) {
		Matcher matcher = KEY_PATTERN.matcher(line);
		matcher.find();
		try {
			currentKey = matcher.group(1);
		}
		catch (IllegalStateException ex) {
			logger.error("No key can be parsed from string '{}', parsing will terminate.", line);
			terminate();
		}
		return matcher.replaceAll("");
	}

	private boolean parseValue(String line) {
		Matcher matcher = VALUE_PATTERN.matcher(line);
		if (matcher.find()) {
			currentValue = parseExpression(matcher.group(1));
			return true;
		}

		return false;
	}

	private Object parseExpression(String value) {
        
        /* do not parse before references are resolved */
		if (containsVariable(value)) {
			return value;
		}
		else {
			return parseSimpleValue(value);
		}
	}

	

	private void terminate() {
		System.exit(1);
	}

	private void resolveBuilderDirective(String line) {
		Matcher matcher = BUILDER_DIRECTIVE_PATTERN.matcher(line);
		matcher.find();
		String directive = matcher.group(1);
		switch(directive){
			case "parent":
				skipNextObject = true;
		}
	}

}
