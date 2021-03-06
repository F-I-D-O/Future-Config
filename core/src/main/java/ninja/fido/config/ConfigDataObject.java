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

import java.util.Iterator;
import java.util.Map.Entry;
import java.util.Stack;

/**
 *
 * @author David Fiedler
 */
public abstract class ConfigDataObject<T, K, V> implements Iterable<Entry<K, V>> {

	protected final T configObject;

	protected ConfigDataObject parentConfigObject;

	protected Object keyInParent;

	protected String path;

	public String getPath() {
		return path;
	}

	public T getConfigObject() {
		return configObject;
	}

//    public O getConfigObject() {
//		return configObject;
//	}
	public ConfigDataObject(T configObject, ConfigDataObject parentConfigObject, Object keyInParent) {
		this.configObject = configObject;
		this.parentConfigObject = parentConfigObject;
		this.keyInParent = keyInParent;
		createPath();
	}

	public abstract V get(K key);

	public abstract boolean containsKey(K key);

	public abstract void put(K key, V value);
	
	public abstract int getSize();

	public void moveTo(ConfigDataObject parentConfigObject, Object keyInParent) {
		this.parentConfigObject = parentConfigObject;
		this.keyInParent = keyInParent;
		createPath();
	}
	
	public Iterable<ConfigProperty> getIterable() {
		ConfigDataObject configDataObject = this;
		return new Iterable<ConfigProperty>() {

			VariableIterator iterator = new VariableIterator(configDataObject, false);

			@Override
			public Iterator<ConfigProperty> iterator() {
				return iterator;
			}
		};
	}

	public Iterable<ConfigProperty> getVariableIterable() {
		ConfigDataObject configDataObject = this;
		return new Iterable<ConfigProperty>() {

			VariableIterator iterator = new VariableIterator(configDataObject, true);

			@Override
			public Iterator<ConfigProperty> iterator() {
				return iterator;
			}
		};
	}
	
	public String getStringForPrint(){
		StringBuilder out = new StringBuilder();
		for(ConfigProperty configProperty: getIterable()){
			out.append(String.format("%s: %s%n", configProperty.getPath(), configProperty.value));
		}
		return out.toString();
	}
    
    public abstract T getInternalObjects();

	private String createPath() {
		path = "";
		ConfigDataObject currentObject = this;
		while (currentObject.keyInParent != null) {
			Object keyInParentParent = currentObject.keyInParent;
			if (keyInParentParent instanceof String) {
				if(currentObject == this){
					path = (String) keyInParentParent;
				}
				else if(path.startsWith("[")){
					path = keyInParentParent + path;
				}
				else{
					path = keyInParentParent + "." + path;
				}
			}
			else {
				path = "[" + Integer.toString((int) keyInParentParent) + "]." + path;
			}
			currentObject = currentObject.parentConfigObject;
		}
		return path;
	}

	class VariableIterator implements Iterator<ConfigProperty> {

		private final Stack<VariableIteratorContext> contextStack;
		
		private final boolean varsOnly;

		private VariableIteratorContext currentContext;

		private Iterator<Entry<K, V>> currentIterator;

		private Entry<K, V> currentEntry;

		VariableIterator(ConfigDataObject configDataObject, boolean varsOnly) {
			this.varsOnly = varsOnly;
			contextStack = new Stack<>();
			currentContext = new VariableIteratorContext(configDataObject, configDataObject.iterator());
			currentIterator = currentContext.iterator;
		}

		@Override
		public boolean hasNext() {
			if (currentEntry != null && isRequestedType(currentEntry.getValue())) {
				return true;
			}
			checkIterator();
			while (currentIterator.hasNext()) {

				/* fetching next entry */
				currentEntry = currentIterator.next();
				Object currentValue = currentEntry.getValue();

				if (isRequestedType(currentValue)) {
					return true;
				}
				else if (currentValue instanceof ConfigDataObject) {
					contextStack.push(currentContext);
					ConfigDataObject currentConfigObject = (ConfigDataObject) currentValue;
					currentContext = new VariableIteratorContext(currentConfigObject, currentConfigObject.iterator());
					currentIterator = currentContext.iterator;
				}
				checkIterator();
			}
			return false;
		}

		@Override
		public ConfigProperty next() {
			if (hasNext()) {
				ConfigProperty property = new ConfigProperty(
						currentContext.configDataObject, currentEntry.getKey(), currentEntry.getValue());
				currentEntry = null;
				return property;
			}
			else {
				return null;
			}
		}
		
		private boolean isRequestedType(Object value){
			if(varsOnly){
				return Parser.containsVariable(value);
			}
			else{
				return !(value instanceof ConfigDataObject);
			}
		}

		private void checkIterator() {
			while (!currentIterator.hasNext() && !contextStack.empty()) {
				currentContext = contextStack.pop();
				currentIterator = currentContext.iterator;
			}
		}

		private class VariableIteratorContext {

			private final ConfigDataObject configDataObject;

			private final Iterator<Entry<K, V>> iterator;

			VariableIteratorContext(ConfigDataObject configDataObject, Iterator<Entry<K, V>> iterator) {
				this.configDataObject = configDataObject;
				this.iterator = iterator;
			}
		}

	}
}
