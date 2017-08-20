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

import java.io.IOException;
import java.util.List;
import java.util.Map;
import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

/**
 *
 * @author David Fiedler
 */
public class ParserSimpleArrayTest {
    
    @Test
    public void test() throws IOException{
        Map<String,Object> config = ParserTester.tryParseFile("simpleArray.cfg");
        
        assertNotNull(config.get("array"));
        assertTrue(config.get("array") instanceof List);
        
        List array = (List) config.get("array");

        assertEquals(1, array.get(0));
        assertEquals(3, array.get(2));
        assertEquals(6, array.get(5));
    }
}
