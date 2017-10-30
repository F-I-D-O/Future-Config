
package ninja.fido.config;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

/**
 *
 * @author David Fiedler
 */
public class ConfigDataList extends ConfigDataObject<List, Integer, Object> {

	List<Entry<Integer, Object>> entrySet;

	public ConfigDataList(List configObject, ConfigDataObject parentConfigObject, Object keyInParent) {
		super(configObject, parentConfigObject, keyInParent);
	}

	public ConfigDataList(ConfigDataObject parentConfigObject, Object keyInParent) {
		super(new ArrayList(), parentConfigObject, keyInParent);
	}

	@Override
	public Iterator<Entry<Integer, Object>> iterator() {
		if (entrySet == null) {
			createEntrySet();
		}
		return entrySet.iterator();
	}


	@Override
	public Object get(Integer key) {
		return configObject.get(key);
	}

	@Override
	public boolean containsKey(Integer key) {
		return configObject.size() > key;
	}

	@Override
	public void put(Integer key, Object value) {
		if (key == null) {
			configObject.add(value);
		}
		else {
			configObject.add(key, value);
		}
	}

	@Override
	public int getSize() {
		return configObject.size();
	}
    
    @Override
    public List<Object> getInternalObjects() {
        List<Object> out = new ArrayList<>();
        for (Map.Entry<Integer, Object> entry : this) {
            Object value = entry.getValue();
            Integer key = entry.getKey();
            if(value instanceof ConfigDataObject){
                out.add(key, ((ConfigDataObject) value).getInternalObjects());
            }
            else{
                out.add(key, value);
            }
        }
        return out;
    }
    private void createEntrySet() {
        entrySet = new LinkedList<>();
        for (int i = 0; i < configObject.size(); i++) {
            entrySet.add(new ListEntry(i));
        }
    }

	private class ListEntry implements Entry<Integer, Object> {

		private final int key;

		ListEntry(int key) {
			this.key = key;
		}

		@Override
		public Integer getKey() {
			return key;
		}

		@Override
		public Object getValue() {
			return configObject.get(key);
		}

		@Override
		public Object setValue(Object value) {
			Object oldObject = configObject.get(key);
			configObject.add(key, value);
			return oldObject;
		}

	}

}
