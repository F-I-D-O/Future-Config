package ninja.fido.config.testproject;

import ninja.fido.config.testproject.config.ArrayOfObjectsItem;
import ninja.fido.config.testproject.config.ObjectHierarchyArrayOfObjectsItem;
import ninja.fido.config.testproject.config.TestprojectConfig;
import ninja.fido.config.testprojectclient.config.TestprojectclientConfig;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

/**
 *
 * @author F.I.D.O.
 */
public class ClientTest {
	
	@Test
	public void test(){
		TestprojectclientConfig config = new TestprojectclientConfig();
		TestprojectConfig libConfig = ClientIn.configure(config);

		assertEquals("string replaced twice", libConfig.string);
		assertEquals("composed string replaced twice", libConfig.composedString);
		assertEquals(111111, (int) libConfig.integer);
		assertEquals(3.14, (double) libConfig.floatProperty, 0.1);

		/* object */
		assertEquals("test", libConfig.object.string);
		assertEquals(9, (int) libConfig.object.integer);
		assertEquals(1.23, libConfig.object.floatProperty, 0.1);

		/* array */
		assertEquals(1, libConfig.array.get(0));
		assertEquals(5, libConfig.array.get(1));
		assertEquals(6, libConfig.array.get(2));

		/* array of objects */
		ArrayOfObjectsItem objectInArray = (ArrayOfObjectsItem) libConfig.arrayOfObjects.get(0);
		assertEquals(571, (int) objectInArray.start);
		assertEquals(672, (int) objectInArray.end);

		/* object with composed */
		assertEquals("string replaced twice that is composed", libConfig.objectWithComposed.string);
		assertEquals("double replaced in child project composed string replaced twice", libConfig.objectWithComposed.doubleComposedString);
		assertEquals("string replaced twice that is composed within object", libConfig.objectWithComposed.innerComposition);

		/* hierarchy */
		assertEquals("another_string", libConfig.objectHierarchy.anotherString);
		assertEquals(987654, (int) libConfig.objectHierarchy.innerObject.integer);
		assertEquals("string replaced twice is funny to compose", libConfig.objectHierarchy.innerObject.composed);
		assertEquals(false, libConfig.objectHierarchy.innerObject.booleanProperty);
		
		ObjectHierarchyArrayOfObjectsItem animal 
				= (ObjectHierarchyArrayOfObjectsItem) libConfig.objectHierarchy.arrayOfObjects.get(1);
		assertEquals("chicken", animal.animal);
		assertEquals(2, (int) animal.legs);
		assertEquals(9.87654, libConfig.objectHierarchy.innerObject.innerInnerObject.floatProperty, 0.1);
		assertEquals("string replaced twice is funny to compose multiple times", 
				libConfig.objectHierarchy.innerObject.innerInnerObject.composed);
		assertEquals(1, libConfig.objectHierarchy.innerObject.innerInnerObject.array.get(0));
		assertEquals(2, libConfig.objectHierarchy.innerObject.innerInnerObject.array.get(1));
		assertEquals(3, libConfig.objectHierarchy.innerObject.innerInnerObject.array.get(2));
		
		assertEquals("replaced twice", config.replacement);
	}
}
