package ninja.fido.config.testproject.config;

import java.lang.String;
import java.util.Map;

public class ObjectWithComposed {
  public String string;

  public String innerComposition;

  public String doubleComposedString;

  public ObjectWithComposed(Map objectWithComposed) {
    this.string = (String) objectWithComposed.get("string");
    this.innerComposition = (String) objectWithComposed.get("inner_composition");
    this.doubleComposedString = (String) objectWithComposed.get("double_composed_string");
  }
}
