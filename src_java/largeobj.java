import java.io.*;
public class largeobj {

	static int limit = 20;
	static int size1 = 1000000;
	static int size2 =2*size1;
	public static void main(String []args) throws IOException {
		for (int index0=0; true; index0++) {
			if (0 == index0 % 100) System.out.println(index0);
			ByteArrayOutputStream bos = new ByteArrayOutputStream(); 	
			ObjectOutputStream oos = new ObjectOutputStream(bos);
			oos.writeObject(String.class);
			oos.close();
			Object   array1 = null;
			for(int i1=0; i1<limit; i1++) {
				System.out.println("" + i1);
				array1 = new Object[size1] ;
				for (int i2=0; i2<limit; i2++) {
					array1 = new Object[size2]; 
				} 
			}
			array1=null;
		}
	} 
}
