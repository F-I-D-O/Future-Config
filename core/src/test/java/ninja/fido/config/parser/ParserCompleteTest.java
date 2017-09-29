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
import ninja.fido.config.ConfigDataList;
import ninja.fido.config.ConfigDataMap;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 *
 * @author David Fiedler
 */
public class ParserCompleteTest {
    @Test
    public void test() throws IOException{
        Map<String,Object> config = ParserTester.tryParseFile("complete.cfg");
        
        assertEquals("string", config.get("string"));
        assertEquals("composed string", config.get("composed_string"));
        assertEquals(123456, config.get("integer"));
        assertEquals(3.14, config.get("float"));
        
        /* object */
        assertNotNull(config.get("object"));
        assertTrue(config.get("object") instanceof ConfigDataMap);
        ConfigDataMap object = (ConfigDataMap) config.get("object");
        assertEquals("test", object.get("string"));
        assertEquals(9, object.get("integer"));
        assertEquals(1.23, object.get("float"));
        
        /* array */
        assertNotNull(config.get("array"));
        assertTrue(config.get("array") instanceof ConfigDataList);
        ConfigDataList array = (ConfigDataList) config.get("array");
        assertEquals(1, array.get(0));
        assertEquals(5, array.get(1));
        assertEquals(6, array.get(2));
        
        /* array of objects */
        assertNotNull(config.get("array_of_objects"));
        assertTrue(config.get("array_of_objects") instanceof ConfigDataList);
        ConfigDataList arrayOfObjects = (ConfigDataList) config.get("array_of_objects");
        assertNotNull(arrayOfObjects.get(0));
        assertTrue(arrayOfObjects.get(0) instanceof ConfigDataMap);
        ConfigDataMap objectInArray = (ConfigDataMap) arrayOfObjects.get(0);
        assertEquals(571, objectInArray.get("start"));
        assertEquals(672, objectInArray.get("end"));
        
        /* object with composed */
        assertNotNull(config.get("object_with_composed"));
        assertTrue(config.get("object_with_composed") instanceof ConfigDataMap);
        ConfigDataMap objectWithComposedVariables = (ConfigDataMap) config.get("object_with_composed");
        assertEquals("string that is composed", objectWithComposedVariables.get("string"));
        assertEquals("double composed string", objectWithComposedVariables.get("double_composed_string"));
        assertEquals("string that is composed within object", objectWithComposedVariables.get("inner_composition"));
        
        /* hierarchy */
        assertNotNull(config.get("object_hierarchy"));
        assertTrue(config.get("object_hierarchy") instanceof ConfigDataMap);
        ConfigDataMap objectHierarchy = (ConfigDataMap) config.get("object_hierarchy");
        
        assertNotNull(objectHierarchy.get("inner_object"));
        assertTrue(objectHierarchy.get("inner_object") instanceof ConfigDataMap);
        ConfigDataMap innerObject = (ConfigDataMap) objectHierarchy.get("inner_object");
        
        assertNotNull(objectHierarchy.get("array_of_objects"));
        assertTrue(objectHierarchy.get("array_of_objects") instanceof ConfigDataList);
        ConfigDataList innerArrayOfObjects = (ConfigDataList) objectHierarchy.get("array_of_objects");
        
        assertNotNull(innerArrayOfObjects.get(1));
        assertTrue(innerArrayOfObjects.get(1) instanceof ConfigDataMap);
        ConfigDataMap animal = (ConfigDataMap) innerArrayOfObjects.get(1);
        
        assertNotNull(innerObject.get("inner_inner_object"));
        assertTrue(innerObject.get("inner_inner_object") instanceof ConfigDataMap);
        ConfigDataMap innerInnerObject = (ConfigDataMap) innerObject.get("inner_inner_object");
        
        assertNotNull(innerInnerObject.get("array"));
        assertTrue(innerInnerObject.get("array") instanceof ConfigDataList);
        ConfigDataList innerInnerObjectArray = (ConfigDataList) innerInnerObject.get("array");

        assertEquals("another_string", objectHierarchy.get("another_string"));
        assertEquals(987654, innerObject.get("integer"));
        assertEquals("string is funny to compose", innerObject.get("composed"));
        assertEquals(false, innerObject.get("boolean"));
        assertEquals("chicken", animal.get("animal"));
        assertEquals(2, animal.get("legs"));
        assertEquals(9.87654, innerInnerObject.get("float"));
        assertEquals("string is funny to compose multiple times", innerInnerObject.get("composed"));
        assertEquals(1, innerInnerObjectArray.get(0));
        assertEquals(2, innerInnerObjectArray.get(1));
        assertEquals(3, innerInnerObjectArray.get(2));
        
        
    }
}
