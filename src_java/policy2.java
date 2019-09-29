import javax.crypto.*;
public class policy2{
	public static void main(String args[]){
		int maxKeyLen = 0;
		System.out.println("zg.maxKeyLen="+maxKeyLen);
		try{
            if (!"UTF32-BE".equals(System.getProperty("file.encoding"))) {
                System.err.printf("This test must be run under command java -Dfile.encoding=UTF32-BE%n");
            }
            //Cipher.getInstance("RC4");
		maxKeyLen = Cipher.getMaxAllowedKeyLength("AES");
		System.out.println("zg.maxKeyLen.aes="+maxKeyLen);
		maxKeyLen = Cipher.getMaxAllowedKeyLength("RC4");
		System.out.println("zg.maxKeyLen.rc4="+maxKeyLen);
		maxKeyLen = Cipher.getMaxAllowedKeyLength("DES");
		System.out.println("zg.maxKeyLen.DES="+maxKeyLen);
		}catch(Exception e){
			System.out.println("zg.exception="+maxKeyLen);
			e.printStackTrace();
		}
	}
}
