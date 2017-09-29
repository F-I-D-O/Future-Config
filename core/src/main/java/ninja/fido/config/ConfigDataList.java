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
package ninja.fido.config;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;
import java.util.Set;

/**
 *
 * @author David Fiedler
 */
public class ConfigDataList<PT extends ConfigDataObject,PK> extends ConfigDataObject<List,Integer,Object,PT,PK>{
    
    Set<Entry<Integer,Object>> entrySet;

    public ConfigDataList(List configObject, PT parentConfigObject, PK keyInParent) {
        super(configObject, parentConfigObject, keyInParent);
    }
    
    public ConfigDataList(PT parentConfigObject, PK keyInParent) {
        super(new ArrayList(), parentConfigObject, keyInParent);
    }

    @Override
    public Iterator<Entry<Integer, Object>> iterator() {
        if(entrySet == null){
            createEntrySet();
        }
        return entrySet.iterator();
    }

    private void createEntrySet() {
        entrySet = new HashSet<>();
        for (int i = 0; i < configObject.size(); i++) {
            entrySet.add(new ListEntry(i));
        }
    }

    @Override
    public Object get(Integer key) {
        return configObject.get(key);
    }

    @Override
    public boolean containsKey(Integer key) {
        return configObject.size() > key;
    }
    
    public void put(Integer key, Object value){
        if(key == null){
            configObject.add(value);
        }
        else{
            configObject.add(key, value);
        }
    }
    
    private class ListEntry implements Entry<Integer, Object>{
        
        private final int key;

        public ListEntry(int key) {
            this.key = key;
        }

        @Override
        public Integer getKey() {
            return key;
        }

        @Override
        public Object getValue() {
            return configObject.get(key);
        }

        @Override
        public Object setValue(Object value) {
            Object oldObject = configObject.get(key);
            configObject.add(key, value);
            return oldObject;
        }
        
    }
    
}