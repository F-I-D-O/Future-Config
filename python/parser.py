import re
import logging
import sys

from config_data_object import ConfigDataObject

NUMBER_PATTERN = re.compile("^([0-9])")
BOOLEAN_PATTERN = re.compile("^(true|false)")


def contains_variable(expression):
    if not isinstance(expression, basestring):
        return False
    return "$" in expression

def parse_simple_value(value):
    if NUMBER_PATTERN.match(value):
        if "." in value:
            return float(value)
        else:
            return int(value)
    elif BOOLEAN_PATTERN.match(value):
        return bool(value)
    elif value.startsWith("'"):
        return value.replace("'", "")
    elif value.startsWith("\""):
        return value.replace("\"", "")


class Parser:
    NAME_PATTERN_STRING = "([a-zA-Z][a-zA-Z0-9_]+)"

    WHITESPACE_LINE_PATTERN = re.compile(r"^\s*$")
    INDENTION_PATTERN = re.compile("^(    |	)*")
    KEY_PATTERN = re.compile("^" + NAME_PATTERN_STRING + "(:)")
    VALUE_PATTERN = re.compile(r"^\s*([^\s]+.*)")


    REFERENCE_PATTERN = re.compile(r"\$({}(\.{})*)".format(NAME_PATTERN_STRING, NAME_PATTERN_STRING))
    OPERATOR_PATTERN = re.compile(r"[+\-]")
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
        sys.exit(1);

    def __init__(self, use_builder_directives=False):
        self.config = []
        self.object_stack = []
        self.useBuilderDirectives = use_builder_directives
        self.current_object = self.config
        self.current_key = 0
        self.current_value = None
        self.in_array = False
        self.skip_next_object = False

    def parse_config_file(self, filename):
        with open(filename) as f:
            content = f.readlines()
            for line in content:

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
                    self.object_stack.push(self.current_object)

                    self.in_array = "[" in line

                    current_object = ConfigDataObject(self.current_object, self.current_key)

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