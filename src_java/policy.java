import javax.crypto.*;
public class policy{
	public static void main(String args[]){
		int maxKeyLen = 0;
		try{
			maxKeyLen = Cipher.getMaxAllowedKeyLength("AES");
		}catch(Exception e){
			e.printStackTrace();
		}
		System.out.println("zg.maxKeyLen="+maxKeyLen);
	}
}
