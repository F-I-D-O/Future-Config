package ninja.fido.config.testproject.config;

import java.lang.Double;
import java.lang.String;
import java.util.List;
import java.util.Map;

public class InnerInnerObject {
  public String composed;

  public List array;

  public Double floatProperty;

  public InnerInnerObject(Map innerInnerObject) {
    this.composed = (String) innerInnerObject.get("composed");
    array = (List) innerInnerObject.get("array");
    this.floatProperty = (Double) innerInnerObject.get("float");
  }
}
