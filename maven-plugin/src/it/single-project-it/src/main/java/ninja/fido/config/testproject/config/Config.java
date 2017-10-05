package ninja.fido.config.testproject.config;

import java.lang.Boolean;
import java.lang.Double;
import java.lang.Integer;
import java.lang.String;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import ninja.fido.config.GeneratedConfig;

public class Config implements GeneratedConfig {
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

  public Config() {
  }

  public Config fill(Map config) {
    this.booleanProperty = (Boolean) config.get("boolean");
    this.string = (String) config.get("string");
    array = (List) config.get("array");
    this.composedString = (String) config.get("composed_string");
    this.objectWithComposed = new ObjectWithComposed((Map) config.get("object_with_composed"));
    this.objectHierarchy = new ObjectHierarchy((Map) config.get("object_hierarchy"));
    this.arrayOfObjects = new ArrayList();
    List arrayOfObjectsList = (List) config.get("array_of_objects");
    for (java.lang.Object object: arrayOfObjectsList) {
      arrayOfObjects.add(new ArrayOfObjectsItem((Map)object));
    }
    this.integer = (Integer) config.get("integer");
    this.floatProperty = (Double) config.get("float");
    this.object = new Object((Map) config.get("object"));
    return this;
  }
}
