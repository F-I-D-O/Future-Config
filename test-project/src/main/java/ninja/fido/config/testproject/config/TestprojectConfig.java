package ninja.fido.config.testproject.config;

import java.lang.Boolean;
import java.lang.Double;
import java.lang.Integer;
import java.lang.String;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import ninja.fido.config.GeneratedConfig;

public class TestprojectConfig implements GeneratedConfig {
  public Boolean booleanProperty;

  public String string;

  public List array;

  public String composedString;

  public ObjectWithComposed objectWithComposed;

  public ObjectHierarchy objectHierarchy;

  public List arrayOfObjects;

  public Integer integer;

  public Double floatProperty;

  public Object object;

  public TestprojectConfig() {
  }

  public TestprojectConfig fill(Map testprojectConfig) {
    this.booleanProperty = (Boolean) testprojectConfig.get("boolean");
    this.string = (String) testprojectConfig.get("string");
    array = (List) testprojectConfig.get("array");
    this.composedString = (String) testprojectConfig.get("composed_string");
    this.objectWithComposed = new ObjectWithComposed((Map) testprojectConfig.get("object_with_composed"));
    this.objectHierarchy = new ObjectHierarchy((Map) testprojectConfig.get("object_hierarchy"));
    this.arrayOfObjects = new ArrayList();
    List arrayOfObjectsList = (List) testprojectConfig.get("array_of_objects");
    for (java.lang.Object object: arrayOfObjectsList) {
      arrayOfObjects.add(new ArrayOfObjectsItem((Map)object));
    }
    this.integer = (Integer) testprojectConfig.get("integer");
    this.floatProperty = (Double) testprojectConfig.get("float");
    this.object = new Object((Map) testprojectConfig.get("object"));
    return this;
  }
}
