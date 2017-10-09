package ninja.fido.config.testprojectclient.config;

import java.lang.String;
import java.util.Map;
import ninja.fido.config.GeneratedConfig;

public class TestprojectclientConfig implements GeneratedConfig {
  public String replacement;

  public TestprojectclientConfig() {
  }

  public TestprojectclientConfig fill(Map testprojectclientConfig) {
    this.replacement = (String) testprojectclientConfig.get("replacement");
    return this;
  }
}
