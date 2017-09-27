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
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import ninja.fido.config.ConfigData;
import ninja.fido.config.ConfigDataLoader;
import ninja.fido.config.ConfigSource;
import ninja.fido.config.parser.ParserTester;
import static ninja.fido.config.resolver.ResolverTest.tryParseFiles;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 *
 * @author fido
 */
public class TwoProjectsEmulationTest {
    public static Map<String,Object> tryParseFiles(String... resourcePaths) throws IOException{
        Object[] readers = new BufferedReader[resourcePaths.length];
        for (int i = 0; i < resourcePaths.length; i++) {
            InputStream inputStream = ParserTester.class.getResourceAsStream("/ninja/fido/config/" + resourcePaths[i]);
            readers[i] = new BufferedReader(new InputStreamReader(inputStream));
        }
        
        ConfigSource source1 = new ConfigSource(readers[0], "parent");
        ConfigSource source2 = new ConfigSource(readers[1], "parent");
        ConfigSource source3 = new ConfigSource(readers[2], null);
        ConfigSource source4 = new ConfigSource(readers[3], null);
        
        ConfigData config = new ConfigDataLoader().loadConfigData(source1, source2, source3, source4);
        return config.getConfig();
    }
    
    @Test
    public void test() throws IOException{
        Map<String,Object> config = tryParseFiles("complete.cfg", "complete_override.cfg", 
                "complete_child_project.cfg", "complete_child_project_override.cfg");
        
        assertEquals("string replaced twice", config.get("string"));
        assertEquals("composed string replaced twice", config.get("composed_string"));
        assertEquals(222222, config.get("integer"));
        assertEquals(3.14, config.get("float"));
        
        /* object */
        assertNotNull(config.get("object"));
        assertTrue(config.get("object") instanceof Map);
        Map object = (Map) config.get("object");
        assertEquals("tset", object.get("string"));
        assertEquals(9, object.get("integer"));
        assertEquals(1.23, object.get("float"));
        
        /* array */
        assertNotNull(config.get("array"));
        assertTrue(config.get("array") instanceof List);
        List array = (List) config.get("array");
        assertEquals(5, array.get(0));
        
        /* array of objects */
        assertNotNull(config.get("array_of_objects"));
        assertTrue(config.get("array_of_objects") instanceof List);
        List arrayOfObjects = (List) config.get("array_of_objects");
        assertNotNull(arrayOfObjects.get(0));
        assertTrue(arrayOfObjects.get(0) instanceof Map);
        Map objectInArray = (Map) arrayOfObjects.get(0);
        assertEquals(571, objectInArray.get("start"));
        assertEquals(672, objectInArray.get("end"));
        assertNotNull(arrayOfObjects.get(1));
        assertTrue(arrayOfObjects.get(1) instanceof Map);
        Map objectInArray2 = (Map) arrayOfObjects.get(1);
        assertEquals(572, objectInArray2.get("start"));
        assertEquals(673, objectInArray2.get("end"));
        
        /* object with composed */
        assertNotNull(config.get("object_with_composed"));
        assertTrue(config.get("object_with_composed") instanceof Map);
        Map objectWithComposedVariables = (Map) config.get("object_with_composed");
        assertEquals("string replaced twice that is composed", objectWithComposedVariables.get("string"));
        assertEquals("double replaced in child project composed string replaced twice", objectWithComposedVariables.get("double_composed_string"));
        assertEquals("string replaced twice that is composed within object replaced", objectWithComposedVariables.get("inner_composition"));
        
        /* hierarchy */
        assertNotNull(config.get("object_hierarchy"));
        assertTrue(config.get("object_hierarchy") instanceof Map);
        Map objectHierarchy = (Map) config.get("object_hierarchy");
        
        assertNotNull(objectHierarchy.get("inner_object"));
        assertTrue(objectHierarchy.get("inner_object") instanceof Map);
        Map innerObject = (Map) objectHierarchy.get("inner_object");
        
        assertNotNull(objectHierarchy.get("array_of_objects"));
        assertTrue(objectHierarchy.get("array_of_objects") instanceof List);
        List innerArrayOfObjects = (List) objectHierarchy.get("array_of_objects");
        
        assertNotNull(innerArrayOfObjects.get(1));
        assertTrue(innerArrayOfObjects.get(1) instanceof Map);
        Map animal = (Map) innerArrayOfObjects.get(1);
        
        assertNotNull(innerObject.get("inner_inner_object"));
        assertTrue(innerObject.get("inner_inner_object") instanceof Map);
        Map innerInnerObject = (Map) innerObject.get("inner_inner_object");
        
        assertNotNull(innerInnerObject.get("array"));
        assertTrue(innerInnerObject.get("array") instanceof List);
        List innerInnerObjectArray = (List) innerInnerObject.get("array");

        assertEquals("another_string", objectHierarchy.get("another_string"));
        assertEquals(987654, innerObject.get("integer"));
        assertEquals("string replaced twice is funny to compose", innerObject.get("composed"));
        assertEquals(true, innerObject.get("boolean"));
        assertEquals("chicken", animal.get("animal"));
        assertEquals(2, animal.get("legs"));
        assertEquals(1.23456, innerInnerObject.get("float"));
        assertEquals("string replaced twice is funny to compose multiple times", innerInnerObject.get("composed"));
        assertEquals(3, innerInnerObjectArray.get(0));
        assertEquals(2, innerInnerObjectArray.get(1));
        assertEquals(1, innerInnerObjectArray.get(2));
    }
}