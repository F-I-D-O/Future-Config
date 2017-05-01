package ninja.fido.config;

import java.util.HashMap;

/**
 *
 * @author F.I.D.O.
 * @param <C>
 */
public interface BuildedConfig<C> {
	public C fill(HashMap config);
}
