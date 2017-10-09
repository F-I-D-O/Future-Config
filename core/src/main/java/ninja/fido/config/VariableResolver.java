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

import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Queue;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import static ninja.fido.config.Parser.OPERATOR_PATTERN;
import static ninja.fido.config.Parser.REFERENCE_PATTERN;
import static ninja.fido.config.Parser.parseSimpleValue;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 *
 * @author fido
 */
public class VariableResolver {

	private static final Logger logger = LoggerFactory.getLogger(VariableResolver.class);
	
	private static final String STRING_VALUE_PATERN_STRING = "'[^']*'";
	
	private static final Pattern OPERATOR_EXPRESSION_PATTERN = Pattern.compile(
			String.format("\\s*(%s|%s)\\s*([+])?", Parser.NAME_PATERN_STRING, STRING_VALUE_PATERN_STRING));

	private final Queue<QueueEntry> referenceQueue;

	private final ConfigDataMap rootMap;

	public VariableResolver(ConfigDataMap rootMap) {
		this.rootMap = rootMap;
		referenceQueue = new LinkedList<>();
	}

	public ConfigDataMap resolveVariables() {
		addAllVariablesToQueue(rootMap);
		processQueue();
		return rootMap;
	}

	private <K> void addAllVariablesToQueue(ConfigDataObject<?, K, Object> configDataObject) {

		for (Entry<K, Object> entry : configDataObject) {
			K key = entry.getKey();
			Object value = entry.getValue();
			if (value instanceof ConfigDataObject) {
				addAllVariablesToQueue((ConfigDataObject) value);
			}
			else if (value instanceof String) {
				String stringValue = (String) value;
				Matcher matcher = REFERENCE_PATTERN.matcher(stringValue);
				if (matcher.find()) {
					referenceQueue.add(new QueueEntry(key, stringValue, configDataObject));
                    configDataObject.put(key, null);
				}
			}
		}
	}

	private void processQueue() {
		int lastQueueLength = referenceQueue.size();
		int checkCounter = lastQueueLength;
		while (!referenceQueue.isEmpty()) {
			QueueEntry entry = referenceQueue.poll();
			Object variableValue = parseExpressionWithReferences(entry.value);
			if (variableValue == null) {
				referenceQueue.add(entry);
			}
			else {
				entry.parent.put(entry.key, variableValue);
			}

			if (checkCounter == 0) {
				if (lastQueueLength == referenceQueue.size()) {
					logger.error("None of the remaining variables can be resolved. Remaining variables: {}",
							referenceQueue);
					terminate();
				}
				lastQueueLength = referenceQueue.size();
				checkCounter = lastQueueLength;
			}
			checkCounter--;
		}
	}

	private Object parseExpressionWithReferences(String value) {
		List<String> references = parseReferences(value);
		for (String reference : references) {
			Object variable = getReferencedValue(reference);
			if (variable == null) {
//                referenceQueue.add(new QueueEntry(currentKey, value, currentObject));
				return null;
			}

			// now String variables only
			value = value.replaceFirst("\\$" + reference, "'" + variable.toString() + "'");
		}
		Matcher matcher = OPERATOR_PATTERN.matcher(value);
		if (matcher.find()) {
			return parseExpressionWithOperators(value);
		}
		else {
			return Parser.parseSimpleValue(value);
		}
	}
	
	private static Object parseExpressionWithOperators(String value) {
		Matcher matcher = OPERATOR_EXPRESSION_PATTERN.matcher(value);
		LinkedList<String> operands = new LinkedList<>();
		LinkedList<String> operators = new LinkedList<>();
		while (matcher.find()) {
			operands.add(matcher.group(1));
			if (matcher.groupCount() == 2) {
				operators.add(matcher.group(2));
			}
		}

		LinkedList<Object> operandsParsed = new LinkedList<>();
		for (String operand : operands) {
			operandsParsed.add(parseSimpleValue(operand));
		}

		Object resolvedExpression = null;

		if (operandsParsed.get(0) instanceof Number) {

		}
		else {
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

	private List<String> parseReferences(String value) {
		LinkedList<String> references = new LinkedList<>();
		Matcher matcher = REFERENCE_PATTERN.matcher(value);
		while (matcher.find()) {
			references.add(matcher.group(1));
		}
		return references;
	}

	private Object getReferencedValue(String reference) {
		ConfigDataObject currentObject = rootMap;
		String[] parts = reference.split("\\.");
		if (parts.length == 0) {
			parts = new String[1];
			parts[0] = reference;
		}
		for (int i = 0; i < parts.length; i++) {
			String part = parts[i];
			if (currentObject.containsKey(part) && currentObject.get(part) != null) {
				if (i < parts.length - 1) {
					currentObject = (ConfigDataObject) currentObject.get(part);
				}
				else {
					return currentObject.get(part);
				}
			}
			else {
				return null;
			}
		}
		return null;
	}

	private void terminate() {
		System.exit(1);
	}

	private class QueueEntry {

		private final Object key;

		private final String value;

		private final ConfigDataObject parent;

		public QueueEntry(Object key, String value, ConfigDataObject parent) {
			this.key = key;
			this.value = value;
			this.parent = parent;
		}

		@Override
		public String toString() {
			return new ConfigProperty(parent, key, value).getPath() + ": " + value;
		}
	}
}
