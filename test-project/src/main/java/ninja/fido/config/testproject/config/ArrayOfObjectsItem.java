package ninja.fido.config.testproject.config;

import java.lang.Integer;
import java.util.Map;

public class ArrayOfObjectsItem {
  public Integer start;

  public Integer end;

  public ArrayOfObjectsItem(Map arrayOfObjectsItem) {
    this.start = (Integer) arrayOfObjectsItem.get("start");
    this.end = (Integer) arrayOfObjectsItem.get("end");
  }
}
