import java.io.*;
public class getProperty{
    public static void main(String[] args) throws IOException{
        String PropGot = System.getProperty(args[0]);
        System.out.println("GotProperty:"+PropGot);      
    }
}