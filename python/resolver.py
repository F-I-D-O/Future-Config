import re
import parser

from collections import deque
from config_data_object import ConfigDataObject
from config_property import ConfigProperty


class Resolver:
	STRING_VALUE_PATTERN_STRING = "'[^']*'"

	OPERATOR_EXPRESSION_PATTERN \
		= re.compile(r"\s*({}|{})\s*([+])?".format(parser.NAME_PATTERN_STRING, STRING_VALUE_PATTERN_STRING))

	def __init__(self, config_data_object: ConfigDataObject):
		self.root_object = config_data_object
		self.reference_queue = deque()

	def resolve_variables(self):
		self._add_all_variables_to_queue(self.root_object)
		self._process_queue()
		return self.root_object

	def _add_all_variables_to_queue(self, config_data_object: ConfigDataObject):
		def add_to_queue (config_property: ConfigProperty, reference_queue: deque):
			reference_queue.append()new
			QueueEntry(key, stringValue, configDataObject));
			configDataObject.put(key, null);

		for key, value in config_data_object.items():
			if (value instanceof ConfigDataObject) {
				addAllVariablesToQueue((ConfigDataObject) value);
			}
			else if (value instanceof String) {
				String stringValue = (String) value;
				Matcher matcher = REFERENCE_PATTERN.matcher(stringValue);
				if (matcher.find()) {

				}
			}

#
# 	private void processQueue() {
# 		int lastQueueLength = referenceQueue.size();
# 		int checkCounter = lastQueueLength;
# 		while (!referenceQueue.isEmpty()) {
# 			QueueEntry entry = referenceQueue.poll();
# 			Object variableValue = parseExpressionWithReferences(entry.value);
# 			if (variableValue == null) {
# 				referenceQueue.add(entry);
# 			}
# 			else {
# 				entry.parent.put(entry.key, variableValue);
# 			}
#
# 			if (checkCounter == 0) {
# 				if (lastQueueLength == referenceQueue.size()) {
# 					LOGGER.error("None of the remaining variables can be resolved. Remaining variables: {}",
# 							referenceQueue);
# 					terminate();
# 				}
# 				lastQueueLength = referenceQueue.size();
# 				checkCounter = lastQueueLength;
# 			}
# 			checkCounter--;
# 		}
# 	}
#
# 	private Object parseExpressionWithReferences(String value) {
# 		List<String> references = parseReferences(value);
# 		for (String reference : references) {
# 			Object variable = getReferencedValue(reference);
# 			if (variable == null) {
# //                referenceQueue.add(new QueueEntry(currentKey, value, currentObject));
# 				return null;
# 			}
#
# 			// now String variables only
# 			value = value.replaceFirst("\\$" + reference, "'" + variable.toString() + "'");
# 		}
# 		Matcher matcher = OPERATOR_PATTERN.matcher(value);
# 		if (matcher.find()) {
# 			return parseExpressionWithOperators(value);
# 		}
# 		else {
# 			return Parser.parseSimpleValue(value);
# 		}
# 	}
#
# 	private static Object parseExpressionWithOperators(String value) {
# 		Matcher matcher = OPERATOR_EXPRESSION_PATTERN.matcher(value);
# 		LinkedList<String> operands = new LinkedList<>();
# 		LinkedList<String> operators = new LinkedList<>();
# 		while (matcher.find()) {
# 			operands.add(matcher.group(1));
# 			if (matcher.groupCount() == 2) {
# 				operators.add(matcher.group(2));
# 			}
# 		}
#
# 		LinkedList<Object> operandsParsed = new LinkedList<>();
# 		for (String operand : operands) {
# 			operandsParsed.add(parseSimpleValue(operand));
# 		}
#
# 		Object resolvedExpression = null;
#
# 		if (operandsParsed.get(0) instanceof Number) {
#
# 		}
# 		else {
# 			resolvedExpression = resolveStringExpression(operandsParsed, operators);
# 		}
# 		return resolvedExpression;
# 	}
#
# 	private static Object resolveStringExpression(LinkedList<Object> operandsParsed, LinkedList<String> operators) {
# 		String resultSting = "";
# 		for (Object operand : operandsParsed) {
# 			resultSting += operand.toString();
# 		}
# 		return resultSting;
# 	}
#
# 	private List<String> parseReferences(String value) {
# 		LinkedList<String> references = new LinkedList<>();
# 		Matcher matcher = REFERENCE_PATTERN.matcher(value);
# 		while (matcher.find()) {
# 			references.add(matcher.group(1));
# 		}
# 		return references;
# 	}
#
# 	private Object getReferencedValue(String reference) {
# 		ConfigDataObject currentObject = rootMap;
# 		String[] parts = reference.split("\\.");
# 		if (parts.length == 0) {
# 			parts = new String[1];
# 			parts[0] = reference;
# 		}
# 		for (int i = 0; i < parts.length; i++) {
# 			String part = parts[i];
# 			if (currentObject.containsKey(part) && currentObject.get(part) != null) {
# 				if (i < parts.length - 1) {
# 					currentObject = (ConfigDataObject) currentObject.get(part);
# 				}
# 				else {
# 					return currentObject.get(part);
# 				}
# 			}
# 			else {
# 				return null;
# 			}
# 		}
# 		return null;
# 	}
#
# 	private void terminate() {
# 		System.exit(1);
# 	}
#
		class QueueEntry:
			def __init__(self, key: any, value: str,  parent):

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