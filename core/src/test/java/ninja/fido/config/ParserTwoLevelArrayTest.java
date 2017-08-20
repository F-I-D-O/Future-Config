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
public class ParserTwoLevelArrayTest {
    
    @Test
    public void test() throws IOException{
        Map<String,Object> config = ParserTester.tryParseFile("twoLevelArray.cfg");
        
        assertNotNull(config.get("array"));
        assertTrue(config.get("array") instanceof List);
        
        List array = (List) config.get("array");
        
        assertNotNull(array.get(0));
        assertNotNull(array.get(1));
        
        assertTrue(array.get(0) instanceof Map);
        assertTrue(array.get(1) instanceof Map);
        
        Map<String,Object> innerMap = (Map<String,Object>) array.get(0);

        assertEquals(1, innerMap.get("start"));
        assertEquals(2, innerMap.get("end"));
        
        innerMap = (Map<String,Object>) array.get(1);

        assertEquals(3, innerMap.get("start"));
        assertEquals(4, innerMap.get("end"));
    }
}
