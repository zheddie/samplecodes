public class Test {
    public static void main(String[] args)
        throws ClassNotFoundException
    {
//    	ClassLoader cl = this.getClass().getClassLoader();
        // get the Class instance using forName method
        Class c1 = Class.forName("javax.jms.JMSException");
 
        System.out.print("Class represented by c1: "
                         + c1.toString());
    }
}
