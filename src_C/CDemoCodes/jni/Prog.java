public class Prog {
		public static native void f1();
		
        public static void main(String[] args) {
               System.out.println("Hello World" + args[0]);
				System.loadLibrary("Pro");
				f1();	
        }
}
