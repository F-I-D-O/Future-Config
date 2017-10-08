package ninja.fido.config.testproject.config;

import java.lang.Integer;
import java.lang.String;
import java.util.Map;

public class ObjectHierarchyArrayOfObjectsItem {
  public Integer legs;

  public String animal;

  public ObjectHierarchyArrayOfObjectsItem(Map objectHierarchyArrayOfObjectsItem) {
    this.legs = (Integer) objectHierarchyArrayOfObjectsItem.get("legs");
    this.animal = (String) objectHierarchyArrayOfObjectsItem.get("animal");
  }
}
