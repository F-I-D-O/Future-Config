package ninja.fido.config.testproject;

import ninja.fido.config.Configuration;
import ninja.fido.config.testproject.config.ArrayOfObjectsItem;
import ninja.fido.config.testproject.config.TestprojectConfig;
import ninja.fido.config.testproject.config.ObjectHierarchyArrayOfObjectsItem;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author F.I.D.O.
 */
public class SingleProjectTest {
	
	@Test
	public void test(){
		TestprojectConfig config = new TestprojectConfig();
		Configuration.load(config);

		assertEquals("string", config.string);
		assertEquals("composed string", config.composedString);
		assertEquals(123456, (int) config.integer);
		assertEquals(3.14, (double) config.floatProperty, 0.1);

		/* object */
		assertEquals("test", config.object.string);
		assertEquals(9, (int) config.object.integer);
		assertEquals(1.23, config.object.floatProperty, 0.1);

		/* array */
		assertEquals(1, config.array.get(0));
		assertEquals(5, config.array.get(1));
		assertEquals(6, config.array.get(2));

		/* array of objects */
		ArrayOfObjectsItem objectInArray = (ArrayOfObjectsItem) config.arrayOfObjects.get(0);
		assertEquals(571, (int) objectInArray.start);
		assertEquals(672, (int) objectInArray.end);

		/* object with composed */
		assertEquals("string that is composed", config.objectWithComposed.string);
		assertEquals("double composed string", config.objectWithComposed.doubleComposedString);
		assertEquals("string that is composed within object", config.objectWithComposed.innerComposition);

		/* hierarchy */
		assertEquals("another_string", config.objectHierarchy.anotherString);
		assertEquals(987654, (int) config.objectHierarchy.innerObject.integer);
		assertEquals("string is funny to compose", config.objectHierarchy.innerObject.composed);
		assertEquals(false, config.objectHierarchy.innerObject.booleanProperty);
		
		ObjectHierarchyArrayOfObjectsItem animal 
				= (ObjectHierarchyArrayOfObjectsItem) config.objectHierarchy.arrayOfObjects.get(1);
		assertEquals("chicken", animal.animal);
		assertEquals(2, (int) animal.legs);
		assertEquals(9.87654, config.objectHierarchy.innerObject.innerInnerObject.floatProperty, 0.1);
		assertEquals("string is funny to compose multiple times", 
				config.objectHierarchy.innerObject.innerInnerObject.composed);
		assertEquals(1, config.objectHierarchy.innerObject.innerInnerObject.array.get(0));
		assertEquals(2, config.objectHierarchy.innerObject.innerInnerObject.array.get(1));
		assertEquals(3, config.objectHierarchy.innerObject.innerInnerObject.array.get(2));
	}
}
