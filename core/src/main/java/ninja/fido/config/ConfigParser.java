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
public class ConfigParser {
    private static Logger logger = LoggerFactory.getLogger(ConfigParser.class);
    
    private static final Pattern WHITESPACE_LINE_PATTERN = Pattern.compile("^\\s*$");
    private static final Pattern INDENTION_PATTERN = Pattern.compile("^(    )*");
    private static final Pattern KEY_PATTERN = Pattern.compile("^([a-zA-Z][a-zA-Z0-9_]+)(:)");
    private static final Pattern SIMPLE_VALUE_PATTERN = Pattern.compile("^\\s*([^\\s]+.*)");
    private static final Pattern NUMBER_PATTERN = Pattern.compile("^([0-9])");
    private static final Pattern BOOLEAN_PATTERN = Pattern.compile("^(true|false)");
    private static final Pattern REFERENCE_PATTERN = Pattern.compile("\\$([\\S_]+)");
    private static final Pattern OPERATOR_PATTERN = Pattern.compile("[+\\-]");
    private static final Pattern OPERATOR_EXPRESSION_PATTERN = Pattern.compile("\\s*('[^']+'+)\\s*([+])?");
    
    
    private final HashMap<String,Object> config;
	
	private final Stack<Object> objectStack;
    
    private final Queue<QueueEntry> referenceQueue;
    
    private HashMap<String,Object> currentObject;
    
    private ArrayList <Object> currentArray;
    
    private Object currentContext;
    
    private String currentKey;
    
    private Object currentValue;
	
    private boolean inArray;
    
    
    /**
     * Constructor.
     */
    public ConfigParser() {
        config = new HashMap<>();
		currentObject = config;
        currentContext = currentObject;
		objectStack = new Stack<>();
//        arrayStack = new Stack<>();
        referenceQueue = new LinkedList<>();
        inArray = false;
    }
    
    
    /**
     * Main method for parsing config file.
     * @param configFile Config file.
     * @return Config object containing all variables from config file.
     * @throws FileNotFoundException
     * @throws IOException 
     */
    public Config parseConfigFile(File configFile) throws FileNotFoundException, IOException{
        try (BufferedReader br = new BufferedReader(new FileReader(configFile))) {
            return parseConfigFile(br);
        }
    }
    
    public Config parseConfigFile(BufferedReader configFileReader) throws FileNotFoundException, IOException{
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
                if(inArray){
                    objectStack.push(currentArray);
                }
                else{
                    objectStack.push(currentObject);
                }
                
                boolean lastInArray = inArray;
                
                /* new object */
                if(line.contains("{")){
                    currentObject = new HashMap<>();
                    currentContext = currentObject;
                    inArray = false;
                }

                /* new array */
                if(line.contains("[")){
                    currentArray = new ArrayList<>();
                    currentContext = currentArray;
                    inArray = true;
                }
                
                if(lastInArray){
                    ((List<Object>) objectStack.peek()).add(currentContext);
                }
                else{
                    ((Map<String,Object>) objectStack.peek()).put(currentKey, currentContext);
                }
            }
            
            else if(line.contains("}") || line.contains("]")){
                Object currentContext = objectStack.pop();
                if(currentContext instanceof HashMap){
                    currentObject = (HashMap<String, Object>) currentContext;
                    inArray = false;
                }
                else{
                    currentArray = (ArrayList<Object>) currentContext;
                    inArray = true;
                }
            }
            
            else{
                parseLine(line);
            }
        }
        processQueue();
        
        return new Config(config);
    }

    private void parseLine(String line) {
        line = stripIndention(line);
        
        if(!inArray){
            line = parseKey(line);
        }
        
        if(parseValue(line)){
            if(inArray){
                currentArray.add(currentValue);
            }
            else{
                currentObject.put(currentKey, currentValue);
            }
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
            return parseExpressionWithReferences(value);
        }
        else{
            return parseSimpleValue(value);
        }
    }

    private Object parseSimpleValue(String value) {
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

    private Object parseExpressionWithReferences(String value) {
        List<String> references = parseReferences(value);
        for (String reference : references) {
            Object variable = getReferencedValue(reference);
            if(variable == null){
                referenceQueue.add(new QueueEntry(currentKey, value, currentObject));
                return null;
            }

            value = value.replace("$" + reference, "'" + variable.toString() + "'");
        }
        Matcher matcher = OPERATOR_PATTERN.matcher(value);
        if(matcher.find()){
            return parseExpressionWithOperators(value);
        }
        else{
            return parseSimpleValue(value);
        }
    }

    private List<String> parseReferences(String value) {
        LinkedList<String> references = new LinkedList<>();
        Matcher matcher = REFERENCE_PATTERN.matcher(value);
        while(matcher.find()){
            references.add(matcher.group(1));
        }
        return references;  
    }

    private Object getReferencedValue(String reference) {
        HashMap<String,Object> currentObject = config;
        String[] parts = reference.split("\\.");
        if(parts.length == 0){
            parts = new String[1];
            parts[0] = reference;
        }
        for (int i = 0; i < parts.length; i++) {
            String part = parts[i];
            if(currentObject.containsKey(part) && currentObject.get(part) != null){
                if(i < parts.length - 1){
                    currentObject = (HashMap<String, Object>) currentObject.get(part);
                }
                else{
                    return currentObject.get(part);
                }
            }
            else{
                return null;
            }
        }
        return null;
    }

    private Object parseExpressionWithOperators(String value) {
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

    private Object resolveStringExpression(LinkedList<Object> operandsParsed, LinkedList<String> operators) {
        String resultSting = "";
        for (Object operand : operandsParsed) {
            resultSting += operand.toString();
        }
        return resultSting;
    }

    private void terminate() {
        System.exit(1);
    }
    
    private class QueueEntry{
        private final String key;
        
        private final String value;
        
        private final HashMap<String,Object> parent;

        public String getKey() {
            return key;
        }

        public String getValue() {
            return value;
        }

        public HashMap<String, Object> getParent() {
            return parent;
        }
        
        

        public QueueEntry(String key, String value, HashMap<String, Object> parent) {
            this.key = key;
            this.value = value;
            this.parent = parent;
        }

    }
    
    private void processQueue(){
        while (!referenceQueue.isEmpty()) {
            QueueEntry entry = referenceQueue.poll();
            Object variableValue = parseExpressionWithReferences(entry.getValue());
            if(variableValue == null){
                referenceQueue.add(entry);
            }
            else{
                entry.parent.put(entry.key, variableValue);
            }
        }
    }
}
