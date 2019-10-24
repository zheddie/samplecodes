
import com.ibm.jit.*;
public class AESTest1 {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Crypto cto = Crypto.getCrypto();
		String s = "HelloWorldHelloWorldHelloWorldHelloWorldHelloWorld";
		byte[] data = s.getBytes();
		byte[] to = new byte[100];
		byte[] toDec = new byte[100];
		int[] rkeys = new int[10];
		for(int i =0;i<10;i++){
			rkeys[i] = i*37;
		}
		System.out.println("Cto.isAESSupportedByHardware="+cto.isAESSupportedByHardware());
		System.out.println("Cto.S="+s);
		cto.doAESInHardware(data, 0, 50, to,0, rkeys, 10, true);
		String toS = new String(to);
		System.out.println("Cto.toS="+toS);
		cto.doAESInHardware(to, 0, 50, toDec,0, rkeys, 10, false);
		String toSDec = new String(toDec);
		System.out.println("Cto.toSDec="+toSDec);

	}

}
