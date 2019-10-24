
package zg;

import com.ibm.jit.*;

public class MyCrypto {
	private Crypto prvCrypto; 
	public  MyCrypto(){
		prvCrypto = Crypto.getCrypto();
	}
	public boolean isAESSupported (){
		return prvCrypto.isAESSupportedByHardware();
	}
	public boolean doAES(byte[] data, int off, int length, byte[] to, int pos, int[] rkeys, int nr, boolean encrypt){
		return prvCrypto.doAESInHardware(data, off, length, to, pos, rkeys, nr, encrypt);
	}
}
