import java.io.UnsupportedEncodingException;


public class Convert {
	public static void main(String[] args) {
		byte[] z = {0x15,0x25};

		String t;
		String fe = args[0];
		try {
			t = new String(z, fe);

			byte[] ba = t.getBytes(fe);
			if(z[0]==ba[0]) System.out.println("OK");
			if(z[1]==ba[1]) System.out.println("OK1");
			//System.out.println(z);
			//System.out.println(ba);
			System.out.println(" HexValue: "+Integer.toHexString(((int)ba[0]))+","+Integer.toHexString(((int)ba[1])));
			System.out.println(" DecValue: "+Integer.toString(((int)ba[0]))+","+Integer.toString(((int)ba[1])));
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
