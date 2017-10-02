package ninja.fido.config;

import java.util.Map;

/**
 * Interface for the contract between Configuration initialization and generated source file.
 *
 * @author F.I.D.O.
 * @param <C> Main config class.
 */
public interface GeneratedConfig<C> {

	public C fill(Map config);
}
