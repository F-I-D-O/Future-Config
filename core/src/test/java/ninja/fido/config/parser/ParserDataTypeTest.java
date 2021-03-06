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
package ninja.fido.config.parser;

import java.io.IOException;
import java.util.Map;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author David Fiedler
 */
public class ParserDataTypeTest {

	@Test
	public void test() throws IOException {
		Map<String, Object> config = ParserTester.tryParseFile("dataTypes.cfg");

		assertEquals("String", config.get("string"));
		assertEquals(1, config.get("int"));
		assertEquals(1.0, config.get("double"));

	}
}
