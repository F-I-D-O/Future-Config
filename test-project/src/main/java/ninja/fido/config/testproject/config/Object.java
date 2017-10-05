package ninja.fido.config.testproject.config;

import java.lang.Double;
import java.lang.Integer;
import java.lang.String;
import java.util.Map;

public class Object {
  public String string;

  public Integer integer;

  public Double floatProperty;

  public Object(Map object) {
    this.string = (String) object.get("string");
    this.integer = (Integer) object.get("integer");
    this.floatProperty = (Double) object.get("float");
  }
}
