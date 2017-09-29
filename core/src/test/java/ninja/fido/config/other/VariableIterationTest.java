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
package ninja.fido.config.other;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import ninja.fido.config.ConfigDataList;
import ninja.fido.config.ConfigDataLoader;
import ninja.fido.config.ConfigDataMap;
import ninja.fido.config.ConfigDataObject;
import ninja.fido.config.ConfigProperty;
import ninja.fido.config.ConfigSource;
import ninja.fido.config.Merger;
import ninja.fido.config.Parser;
import ninja.fido.config.VariableResolver;
import ninja.fido.config.parser.ParserTester;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 *
 * @author fido
 */
public class VariableIterationTest {
    
    public static Map<String,Object> tryParseFile(String resourcePath) throws IOException{
        InputStream inputStream = ParserTester.class.getResourceAsStream("/ninja/fido/config/" + resourcePath);
        BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
        
        ArrayList<ConfigDataMap> configDataList = new ArrayList<>();
        
        ConfigSource configSourceDefinition = new ConfigSource(reader, null);
        Object source = configSourceDefinition.source;
        ConfigDataMap configMapFromSource = new Parser().parseConfigFile((BufferedReader) source);
        configDataList.add(configMapFromSource);
        ConfigDataMap mergedMap = new Merger(configDataList).merge();
        
        return mergedMap.getConfigObject();
    }
    
    @Test
    public void test() throws IOException{
        Map<String,Object> config = tryParseFile("complete.cfg");
        
        /* hierarchy */
        assertNotNull(config.get("object_hierarchy"));
        assertTrue(config.get("object_hierarchy") instanceof ConfigDataMap);
        ConfigDataMap objectHierarchy = (ConfigDataMap) config.get("object_hierarchy");
        
        Map<String,Object> map = new HashMap<>();
        for (ConfigProperty configProperty : objectHierarchy.getVariableIterable()) {
            map.put(configProperty.getPath(), configProperty.value);
        }
        
        assertEquals(2, map.size());
        assertEquals("$string + ' is funny to compose'", map.get("object_hierarchy.inner_object.composed"));
        assertEquals("$object_hierarchy.inner_object.composed + ' multiple times'", 
                map.get("object_hierarchy.inner_object.inner_inner_object.composed"));     
    }

}
