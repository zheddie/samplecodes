
import zg.*;
public class AESTest {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		MyCrypto cto = new MyCrypto();
		String s = "HelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorld";
		byte[] data = s.getBytes();
		byte[] to = new byte[100];
		byte[] toDec = new byte[100];
		int[] rkeys = new int[10];
		for(int i =0;i<10;i++){
			rkeys[i] = i*10;
		}
		System.out.println("Cto.isAESSupported="+cto.isAESSupported());
		System.out.println("Cto.S="+s);
		cto.doAES(data, 0, 99, to,0, rkeys, 10, true);
		String toS = new String(to);
		System.out.println("Cto.toS="+toS);
		cto.doAES(to, 0, 99, toDec,0, rkeys, 10, false);
		String toSDec = new String(toDec);
		System.out.println("Cto.toSDec="+toSDec);

	}

}
