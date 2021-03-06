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
package ninja.fido.config.resolver;

import java.io.IOException;
import java.util.Map;
import ninja.fido.config.parser.ParserTester;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author David Fiedler
 */
public class StringConcatenationTest {
	
	@Test
	public void test() throws IOException {
		Map<String, Object> config = ParserTester.tryParseFile("string_concatenation.cfg");

		assertEquals("String", config.get("string"));
		assertEquals("String composed", config.get("string2"));
		assertEquals("composed String", config.get("string3"));
		assertEquals("Stringcomposed String", config.get("string4"));
		assertEquals("String String", config.get("string5"));
		assertEquals("another String composed", config.get("string6"));
		

	}
}
