import java.nio.charset.Charset; 
import java.util.Iterator; 
import java.util.Map; 
  
public class DefaultCS { 
  
    public static void main(String[] args) 
    { 
  
        // Get the default charset of the machine 
        Charset cs = Charset.defaultCharset(); 
  
        System.out.println("The default charset of the machine is :" + cs.displayName()); 
    } 
} 
