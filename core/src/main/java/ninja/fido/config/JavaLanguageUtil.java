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

import com.google.common.base.CaseFormat;
import java.text.Collator;
import java.util.Arrays;
import java.util.Locale;

/**
 *
 * @author David Fiedler
 */
public class JavaLanguageUtil {
    
    private static final Collator englishCollator = Collator.getInstance(Locale.ENGLISH);
    
    private static final String keywords[] = {
        "abstract",  "assert",       "boolean",    "break",      "byte",      "case",
        "catch",     "char",         "class",      "const",     "continue",
        "default",   "do",           "double",     "else",      "extends",
        "false",     "final",        "finally",    "float",     "for",
        "goto",      "if",           "implements", "import",    "instanceof",
        "int",       "interface",    "long",       "native",    "new",
        "null",      "package",      "private",    "protected", "public",
        "return",    "short",        "static",     "strictfp",  "super",
        "switch",    "synchronized", "this",       "throw",     "throws",
        "transient", "true",         "try",        "void",      "volatile",
        "while"
    };
    
    public static boolean isJavaKeyword(String keyword) {
        return (Arrays.binarySearch(keywords, keyword, englishCollator) >= 0);
    } 
    
    public static String sanitizePropertyName(String propertyName){
        if(isJavaKeyword(propertyName)){
            propertyName += "Property";
        }
        
        return propertyName;
    }
    
    public static String getPropertyName(String string){
		return sanitizePropertyName(CaseFormat.UPPER_UNDERSCORE.to(CaseFormat.LOWER_CAMEL, string));
	}
    
}
