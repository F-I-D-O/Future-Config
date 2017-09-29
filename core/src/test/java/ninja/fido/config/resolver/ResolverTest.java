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
package ninja.fido.config.resolver;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Map;
import ninja.fido.config.ConfigDataList;
import ninja.fido.config.ConfigDataMap;
import ninja.fido.config.ConfigDataLoader;
import ninja.fido.config.parser.ParserTester;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 *
 * @author fido
 */
public class ResolverTest {
    public static Map<String,Object> tryParseFiles(String... resourcePaths) throws IOException{
        Object[] readers = new BufferedReader[resourcePaths.length];
        for (int i = 0; i < resourcePaths.length; i++) {
            InputStream inputStream = ParserTester.class.getResourceAsStream("/ninja/fido/config/" + resourcePaths[i]);
            readers[i] = new BufferedReader(new InputStreamReader(inputStream));
        }
        ConfigDataMap config = new ConfigDataLoader().loadConfigData(readers);
        return config.getConfigObject();
    }
    
    @Test
    public void test() throws IOException{
        Map<String,Object> config = tryParseFiles("complete.cfg", "complete_override.cfg");
        
        assertEquals("string replaced", config.get("string"));
        assertEquals("composed string replaced", config.get("composed_string"));
        assertEquals(654321, config.get("integer"));
        assertEquals(3.14, config.get("float"));
        
        /* object */
        assertNotNull(config.get("object"));
        assertTrue(config.get("object") instanceof ConfigDataMap);
        ConfigDataMap object = (ConfigDataMap) config.get("object");
        assertEquals("tset", object.get("string"));
        assertEquals(9, object.get("integer"));
        assertEquals(1.23, object.get("float"));
        
        /* array */
        assertNotNull(config.get("array"));
        assertTrue(config.get("array") instanceof ConfigDataList);
        ConfigDataList array = (ConfigDataList) config.get("array");
        assertEquals(5, array.get(0));
        
        /* array of objects */
        assertNotNull(config.get("array_of_objects"));
        assertTrue(config.get("array_of_objects") instanceof ConfigDataList);
        ConfigDataList arrayOfObjects = (ConfigDataList) config.get("array_of_objects");
        assertNotNull(arrayOfObjects.get(0));
        assertTrue(arrayOfObjects.get(0) instanceof ConfigDataMap);
        ConfigDataMap objectInArray = (ConfigDataMap) arrayOfObjects.get(0);
        assertEquals(571, objectInArray.get("start"));
        assertEquals(672, objectInArray.get("end"));
        assertNotNull(arrayOfObjects.get(1));
        assertTrue(arrayOfObjects.get(1) instanceof ConfigDataMap);
        ConfigDataMap objectInArray2 = (ConfigDataMap) arrayOfObjects.get(1);
        assertEquals(572, objectInArray2.get("start"));
        assertEquals(673, objectInArray2.get("end"));
        
        /* object with composed */
        assertNotNull(config.get("object_with_composed"));
        assertTrue(config.get("object_with_composed") instanceof ConfigDataMap);
        ConfigDataMap objectWithComposedVariables = (ConfigDataMap) config.get("object_with_composed");
        assertEquals("string replaced that is composed", objectWithComposedVariables.get("string"));
        assertEquals("double composed string replaced", objectWithComposedVariables.get("double_composed_string"));
        assertEquals("string replaced that is composed within object replaced", objectWithComposedVariables.get("inner_composition"));
        
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
        assertEquals("string replaced is funny to compose", innerObject.get("composed"));
        assertEquals(true, innerObject.get("boolean"));
        assertEquals("chicken", animal.get("animal"));
        assertEquals(2, animal.get("legs"));
        assertEquals(1.23456, innerInnerObject.get("float"));
        assertEquals("string replaced is funny to compose multiple times", innerInnerObject.get("composed"));
        assertEquals(3, innerInnerObjectArray.get(0));
        assertEquals(2, innerInnerObjectArray.get(1));
        assertEquals(1, innerInnerObjectArray.get(2));
    }
}
