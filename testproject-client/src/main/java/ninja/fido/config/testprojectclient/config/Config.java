package ninja.fido.config.testprojectclient.config;

import java.lang.String;
import java.util.Map;
import ninja.fido.config.GeneratedConfig;

public class Config implements GeneratedConfig {
  public String replacement;

  public Config() {
  }

  public Config fill(Map config) {
    this.replacement = (String) config.get("replacement");
    return this;
  }
}
