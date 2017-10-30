
package ninja.fido.config.other;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import ninja.fido.config.ConfigDataLoader;
import ninja.fido.config.ConfigDataMap;
import ninja.fido.config.parser.ParserTester;
import org.junit.Test;

/**
 *
 * @author David Fiedler
 */
public class PrintTest {
	private static ConfigDataMap tryParseFile(String resourcePath) throws IOException {
		InputStream inputStream = ParserTester.class.getResourceAsStream("/ninja/fido/config/" + resourcePath);
		BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
		ConfigDataMap config = new ConfigDataLoader().loadConfigData(reader);
		return config;
	}

	@Test
	public void test() throws IOException {
		ConfigDataMap config = tryParseFile("complete.cfg");
		
		String variablePrint = config.getStringForPrint();

	}
}
