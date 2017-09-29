/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ninja.fido.config;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Stack;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Config file parser
 * @author fido
 */
public class Parser {
    private static Logger logger = LoggerFactory.getLogger(Parser.class);
    
    private static final Pattern WHITESPACE_LINE_PATTERN = Pattern.compile("^\\s*$");
    private static final Pattern INDENTION_PATTERN = Pattern.compile("^(    )*");
    private static final Pattern KEY_PATTERN = Pattern.compile("^([a-zA-Z][a-zA-Z0-9_]+)(:)");
    private static final Pattern SIMPLE_VALUE_PATTERN = Pattern.compile("^\\s*([^\\s]+.*)");
    private static final Pattern NUMBER_PATTERN = Pattern.compile("^([0-9])");
    private static final Pattern BOOLEAN_PATTERN = Pattern.compile("^(true|false)");
    public static final Pattern REFERENCE_PATTERN = Pattern.compile("\\$([\\S_]+)");
    public static final Pattern OPERATOR_PATTERN = Pattern.compile("[+\\-]");
    private static final Pattern OPERATOR_EXPRESSION_PATTERN = Pattern.compile("\\s*('[^']+'+)\\s*([+])?");
    
    
    private final ConfigDataMap config;
	
	private final Stack<ConfigDataObject> objectStack;
    
    private ConfigDataObject currentObject;
    
    private Object currentKey;
    
    private Object currentValue;
	
    private boolean inArray;
    
    
    /**
     * Constructor.
     */
    public Parser() {
        config = new ConfigDataMap(new HashMap<>(), null, null);
		currentObject = config;
		objectStack = new Stack<>();
        inArray = false;
    }
    
    
    /**
     * Main method for parsing config file.
     * @param configFile Config file.
     * @return Config object containing all variables from config file.
     * @throws FileNotFoundException
     * @throws IOException 
     */
    public ConfigDataMap parseConfigFile(File configFile) throws FileNotFoundException, IOException{
        try (BufferedReader br = new BufferedReader(new FileReader(configFile))) {
            return parseConfigFile(br);
        }
    }
    
    public ConfigDataMap parseConfigFile(BufferedReader configFileReader) throws FileNotFoundException, IOException{
        String line;
        while ((line = configFileReader.readLine()) != null) {
            
            /* skip blank lines */
            Matcher matcher = WHITESPACE_LINE_PATTERN.matcher(line);
            if(matcher.find()){
                
            }
            
            /* comment line */
            else if(line.contains("#")){
                // possible comment processing
            }

            /* new array or object */
            else if(line.contains("{") || line.contains("[")){

                /* push old context to stack */
                objectStack.push(currentObject);
                
                /* new object */
                if(line.contains("{")){
                    currentObject = new ConfigDataMap(currentObject, currentKey);
                    inArray = false;
                }

                /* new array */
                if(line.contains("[")){
                    currentObject = new ConfigDataList(currentObject, currentKey);
                    inArray = true;
                }
                
                /* add new object to parent object */
                objectStack.peek().put(currentKey, currentObject);
                
                if(inArray){
                    currentKey = null;
                }
            }
            
            else if(line.contains("}") || line.contains("]")){
                currentObject = objectStack.pop();
                if(currentObject instanceof ConfigDataMap){
                    inArray = false;
                }
                else{
                    inArray = true;
                    currentKey = null;
                }
            }
            
            else{
                parseLine(line);
            }
        }
        
        return config;
    }

    private void parseLine(String line) {
        line = stripIndention(line);
        
        if(!inArray){
            line = parseKey(line);
        }
        
        if(parseValue(line)){
            currentObject.put(currentKey, currentValue);
        }
    }

    private String stripIndention(String line) {
        Matcher matcher = INDENTION_PATTERN.matcher(line);
        if (matcher.find()){
            return matcher.replaceAll("");
        }
        else{
            return line;
        }
    }
    
    

    private String parseKey(String line) {
        Matcher matcher = KEY_PATTERN.matcher(line);
        matcher.find();
        try{
            currentKey = matcher.group(1);
        }
        catch(IllegalStateException ex){
            logger.error("No key can be parsed from string '{}', parsing will terminate.", line);
            terminate();
        }
        return matcher.replaceAll("");
    }
    
    private boolean parseValue(String line) {
        Matcher matcher = SIMPLE_VALUE_PATTERN.matcher(line);
        if(matcher.find()){
            currentValue = parseExpression(matcher.group(1));
            return true;
        }
            
        return false;
    }

    private Object parseExpression(String value) {
        if(value.contains("$")){
            return value;
        }
        else{
            return parseSimpleValue(value);
        }
    }

    static Object parseSimpleValue(String value) {
        Matcher matcher = NUMBER_PATTERN.matcher(value);
        if(matcher.find()){
            if(value.contains(".")){
                return Double.parseDouble(value);
            }
            else{
                return Integer.parseInt(value);
            }
        }
        else{
            matcher = BOOLEAN_PATTERN.matcher(value);
            if(matcher.find()){
                return Boolean.parseBoolean(value);
            }
            else{
                if(value.startsWith("'")){
                    return value.replace("'", "");
                }
                if(value.startsWith("\"")){
                    return value.replace("\"", "");
                }
                return value;
            }
        }
    }

    static Object parseExpressionWithOperators(String value) {
        Matcher matcher = OPERATOR_EXPRESSION_PATTERN.matcher(value);
        LinkedList<String> operands = new LinkedList<>();
        LinkedList<String> operators = new LinkedList<>();
        while(matcher.find()) {
            operands.add(matcher.group(1));
            if(matcher.groupCount() == 2){
                operators.add(matcher.group(2));
            }
        }
        
        LinkedList<Object> operandsParsed = new LinkedList<>();
        for (String operand : operands) {
            operandsParsed.add(parseSimpleValue(operand));
        }
        
        Object resolvedExpression = null;
        
        if(operandsParsed.get(0) instanceof Number){
            
        }
        else{
            resolvedExpression = resolveStringExpression(operandsParsed, operators);
        }
        return resolvedExpression;
    }

   private static Object resolveStringExpression(LinkedList<Object> operandsParsed, LinkedList<String> operators) {
        String resultSting = "";
        for (Object operand : operandsParsed) {
            resultSting += operand.toString();
        }
        return resultSting;
    }

    private void terminate() {
        System.exit(1);
    }
    
    
    
    
}
