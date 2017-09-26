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
package ninja.fido.config;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 *
 * @author fido
 */
public class ConfigDataLoader {
    public ConfigData loadConfigData(BufferedReader... configReaders) throws IOException{
        ArrayList<Map<String,Object>> configDataList = new ArrayList<>();
        for (BufferedReader configReader : configReaders) {
            configDataList.add(new ConfigParser().parseConfigFile(configReader));
        }
        return new ConfigData(new ConfigDataResolver(configDataList).resolve());
    }
}
