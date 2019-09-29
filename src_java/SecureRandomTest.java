import java.security.*;
 /** 
 * 
 */
public class SecureRandomTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
        SecureRandomTest srt = new SecureRandomTest();
		srt.test();


	}

	public void test() {
		SecureRandom sr;
		byte [] ba= {0x15,0x01,0x02};
        byte [] b= {0x00, 0x01, 0x02,};
		try {
			//sr = SecureRandom.getInstance("SHA1PRNG");
			sr = SecureRandom.getInstance("NativePRNG");
			System.out.printf("Algorithm %s, provider %s\n", sr.getAlgorithm(), sr.getProvider());
	        //ba = sr.generateSeed(1);
	        sr.setSeed(ba);
	        sr.nextBytes(ba);
	        for (int i=0;i<ba.length;i++)
	        	   System.out.printf("%d  ", ba[i]);
	        System.out.println()
	        ;
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        

	}

} 