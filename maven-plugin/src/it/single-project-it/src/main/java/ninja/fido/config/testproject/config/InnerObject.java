package ninja.fido.config.testproject.config;

import java.lang.Boolean;
import java.lang.Integer;
import java.lang.String;
import java.util.Map;

public class InnerObject {
  public Boolean booleanProperty;

  public String composed;

  public InnerInnerObject innerInnerObject;

  public Integer integer;

  public InnerObject(Map innerObject) {
    this.booleanProperty = (Boolean) innerObject.get("boolean");
    this.composed = (String) innerObject.get("composed");
    this.innerInnerObject = new InnerInnerObject((Map) innerObject.get("inner_inner_object"));
    this.integer = (Integer) innerObject.get("integer");
  }
}
