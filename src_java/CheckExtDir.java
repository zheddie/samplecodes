
public class CheckExtDir {

    /**
     * @param args
     */
    public static void main(String[] args) {
        // TODO Auto-generated method stub

     // the classpath
        System.out.println("java.class.path="+ System.getProperty( "java.class.path" ) );

        // extension directories whose jars are included on the classpath
        System.out.println( "java.ext.dirs="+ System.getProperty( "java.ext.dirs" ) );

        // low level classpath, includes system jars
        System.out.println("java.library.path="+ System.getProperty( "java.library.path" ) );

        // character to separate (not terminate!) entries on the classpath, ; for Windows : for unix.
        System.out.println( "path.separator="+System.getProperty( "path.separator" ) );
    }

}
