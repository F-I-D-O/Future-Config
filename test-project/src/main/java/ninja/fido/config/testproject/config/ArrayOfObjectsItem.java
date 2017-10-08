package ninja.fido.config.testproject.config;

import java.lang.Integer;
import java.util.Map;

public class ArrayOfObjectsItem {
  public Integer start;

  public Integer end;

  public ArrayOfObjectsItem(Map ArrayOfObjectsItem) {
    this.start = (Integer) ArrayOfObjectsItem.get("start");
    this.end = (Integer) ArrayOfObjectsItem.get("end");
  }
}
