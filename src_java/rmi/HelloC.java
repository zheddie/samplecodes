import java.rmi.*;


public class HelloC {

	public static void main (String[] argv) {

		try {

			HelloInterface hello =(HelloInterface) Naming.lookup ("//localhost:8808/Hello");

			System.out.println (hello.say());

		} catch (Exception e) {

			System.out.println ("HelloClient exception: " + e);

		}

	}

}
