import logging
import re
import sys

from fconfig.config_data_object import ConfigDataObject
# from fconfig.loader import ConfigSource

NAME_PATTERN_STRING = "([a-zA-Z][a-zA-Z0-9_]+)"

NUMBER_PATTERN = re.compile("^([0-9])")
BOOLEAN_PATTERN = re.compile("^(true|false)")
OPERATOR_PATTERN = re.compile(r"[+\-]")
REFERENCE_PATTERN = re.compile(r"\$({}(\.{})*)".format(NAME_PATTERN_STRING, NAME_PATTERN_STRING))



def contains_variable(expression):
    if not isinstance(expression, str):
        return False
    return "$" in expression


def parse_simple_value(value: str):
    if NUMBER_PATTERN.match(value):
        if "." in value:
            return float(value)
        else:
            return int(value)
    elif BOOLEAN_PATTERN.match(value):
        return bool(value)
    elif value.startswith("'"):
        return value.replace("'", "")
    elif value.startswith("\""):
        return value.replace("\"", "")


class Parser:
    WHITESPACE_LINE_PATTERN = re.compile(r"^\s*$")
    INDENTION_PATTERN = re.compile("^(    |	)*")
    KEY_PATTERN = re.compile("^" + NAME_PATTERN_STRING + "(:)")
    VALUE_PATTERN = re.compile(r"^\s*([^\s]+.*)")
    BUILDER_DIRECTIVE_PATTERN = re.compile(r"^!([^\s]*)")

    @staticmethod
    def strip_indention(line):
        line = Parser.INDENTION_PATTERN.sub("", line)
        return line

    @staticmethod
    def parse_expression(value):

        # do not parse before references are resolved
        if contains_variable(value):
            return value
        else:
            return parse_simple_value(value)

    @staticmethod
    def terminate():
        sys.exit(1)

    def __init__(self, use_builder_directives=False):
        self.config = ConfigDataObject(False)
        self.object_stack = []
        self.use_builder_directives = use_builder_directives
        self.current_object = self.config
        self.current_key = 0
        self.current_value = None
        self.in_array = False
        self.skip_next_object = False

    def parse_config_source(self, config_source: str)-> ConfigDataObject:
        for line in config_source:

            # skip blank lines
            if Parser.WHITESPACE_LINE_PATTERN.match(line):
                pass

            # comment line
            elif "#" in line:

                # possible comment processing
                pass

            # directive line
            elif line.startswith("!"):
                if self.use_builder_directives:
                    self.resolve_builder_directive(line)

            # new array or object
            elif "{" in line or "[" in line:

                # push old context to stack
                self.object_stack.append(self.current_object)

                self.in_array = "[" in line

                self.current_object = ConfigDataObject(self.in_array, self.current_object, self.current_key)

                # add new object to parent object
                if self.skip_next_object:
                    self.skip_next_object = False
                else:
                    self.object_stack[-1].put(self.current_key, self.current_object)
                if self.in_array:
                    self.current_key = 0

            elif "}" in line or "]" in line:
                self.current_object = self.object_stack.pop()
                if self.current_object.is_array:
                    self.in_array = True
                    self.current_key = self.current_object.getSize()
                else:
                    self.in_array = False
            else:
                self.parse_line(line)

        return self.config

    def parse_line(self, line):
        line = self.strip_indention(line)

        if not self.in_array:
            line = self.parse_key(line)

        if self.parse_value(line):
            self.current_object.put(self.current_key, self.current_value)

        if self.in_array:
            self.current_key += 1

    def parse_key(self, line):
        match = Parser.KEY_PATTERN.match(line)
        if match:
            self.current_key = match.group(1)
        else:
            logging.critical("No key can be parsed from string '%s', parsing will terminate.", line)
            self.terminate()
        return Parser.KEY_PATTERN.sub("", line)

    def parse_value(self, line):
        match = Parser.VALUE_PATTERN.match(line)
        if match:
            self.current_value = self.parse_expression(match.group(1))
            return True
        return False

    def resolve_builder_directive(self, line):
        match = self.BUILDER_DIRECTIVE_PATTERN.match(line)
        directive = match.group(1);
        if directive == "parent":
            self.skip_next_object = True