import java.security.NoSuchAlgorithmException;
import java.security.MessageDigest;

public class GetInstanceTest {

/**
* @param args
*/
public static void main(String[] args) {
// TODO Auto-generated method stub
MessageDigest md=null;
try {
//md = MessageDigest.getInstance("MD5","IBMJSSEProvider");
md = MessageDigest.getInstance("MD5");
} catch (NoSuchAlgorithmException e) {
// TODO Auto-generated catch block
e.printStackTrace();
} catch(Exception e){
e.printStackTrace();
}
System.out.printf("Provider: %s, algorithm: %s\n",md.getProvider(), md.getAlgorithm());
}

}
