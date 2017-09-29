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

import java.util.Map.Entry;

/**
 *
 * @author David Fiedler
 */
public abstract class ConfigDataObject<T,K,V,PT extends ConfigDataObject,PK> implements Iterable<Entry<K,V>>{
    
    protected final T configObject;
    
    final PT parentConfigObject;
    
    final PK keyInParent;
    
    
    public T getConfigObject(){
        return configObject;
    }
    
    
//    public O getConfigObject() {
//		return configObject;
//	}

    public ConfigDataObject(T configObject, PT parentConfigObject, PK keyInParent) {
        this.configObject = configObject;
        this.parentConfigObject = parentConfigObject;
        this.keyInParent = keyInParent;
    }
    
    
    
    
    public abstract V get(K key);
    
    public abstract boolean containsKey(K key);
    
    public abstract void put(K key, V value);

}
