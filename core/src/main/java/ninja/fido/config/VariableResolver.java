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

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.regex.Matcher;
import static ninja.fido.config.Parser.OPERATOR_PATTERN;
import static ninja.fido.config.Parser.REFERENCE_PATTERN;

/**
 *
 * @author fido
 */
public class VariableResolver {
    
    private final Queue<QueueEntry> referenceQueue;
    
    private final Map<String,Object> configMap;

    
    
    
    public VariableResolver(Map<String,Object> configMap) {
        this.configMap = configMap;
        referenceQueue = new LinkedList<>();
    }
    


    public Map<String,Object> resolveVariables() {
        addAllUnresolvedVariablesToQueue();
        processQueue();
        return configMap;
    }
    
    private void addAllUnresolvedVariablesToQueue() {
        addAllUnresolvedVariablesToQueueForMap(configMap);
    }
    
    private void addAllUnresolvedVariablesToQueueForMap(Map<String, Object> currentMap) {
         for (Map.Entry<String, Object> entry : currentMap.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();
            if(value instanceof Map){
                addAllUnresolvedVariablesToQueueForMap((HashMap<String,Object>) value);
            }
            else if(value instanceof List){
                addAllUnresolvedVariablesToQueueForList((List) value);
            }
            else if(value instanceof String){
                String stringValue = (String) value;
                Matcher matcher = REFERENCE_PATTERN.matcher(stringValue);
                if(matcher.find()){
                    referenceQueue.add(new QueueEntry(key, stringValue, currentMap));
                }
            }
        }
    }
    
    private void addAllUnresolvedVariablesToQueueForList(List currentList) {
         for (Object entry : currentList) {
            if(entry instanceof Map){
                addAllUnresolvedVariablesToQueueForMap((Map<String,Object>) entry);
            }
            else if(entry instanceof List){
                addAllUnresolvedVariablesToQueueForList((List) entry);
            }
            else if(entry instanceof String){
                String stringValue = (String) entry;
                Matcher matcher = REFERENCE_PATTERN.matcher(stringValue);
                if(matcher.find()){
                    referenceQueue.add(new QueueEntry(null, stringValue, currentList));
                }
            }
        }
    }
    
    private void processQueue(){
        while (!referenceQueue.isEmpty()) {
            QueueEntry entry = referenceQueue.poll();
            Object variableValue = parseExpressionWithReferences(entry.value);
            if(variableValue == null){
                referenceQueue.add(entry);
            }
            else if(entry.parent instanceof Map){
                ((Map) entry.parent).put(entry.key, variableValue);
            }
            else{
                ((List) entry.parent).add(variableValue);
            }
        }
    }
    
    private Object parseExpressionWithReferences(String value) {
        List<String> references = parseReferences(value);
        for (String reference : references) {
            Object variable = getReferencedValue(reference);
            if(variable == null){
//                referenceQueue.add(new QueueEntry(currentKey, value, currentObject));
                return null;
            }
            
            // now String variables only
            value = value.replace("$" + reference, "'" + variable.toString() + "'");
        }
        Matcher matcher = OPERATOR_PATTERN.matcher(value);
        if(matcher.find()){
            return Parser.parseExpressionWithOperators(value);
        }
        else{
            return Parser.parseSimpleValue(value);
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
        Map<String,Object> currentObject = configMap;
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

   

   
    
    
    private class QueueEntry{
        private final String key;
        
        private final String value;
        
        private final Object parent;
        

        public QueueEntry(String key, String value, Object parent) {
            this.key = key;
            this.value = value;
            this.parent = parent;
        }

    }
}
