package ninja.fido.config.testproject.config;

import java.lang.Object;
import java.lang.String;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class ObjectHierarchy {
  public String anotherString;

  public List arrayOfObjects;

  public InnerObject innerObject;

  public ObjectHierarchy(Map objectHierarchy) {
    this.anotherString = (String) objectHierarchy.get("another_string");
    this.arrayOfObjects = new ArrayList();
    List arrayOfObjectsList = (List) objectHierarchy.get("array_of_objects");
    for (Object object: arrayOfObjectsList) {
      arrayOfObjects.add(new ObjectHierarchyArrayOfObjectsItem((Map)object));
    }
    this.innerObject = new InnerObject((Map) objectHierarchy.get("inner_object"));
  }
}
