import java.io.FileOutputStream;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Hashtable;

public class CPConvert {

  public static final int NEEDS_MORE_DATA = -2;
  public static final int UNSUPPORTED_CHAR = -1;

  private static final Class byteArrayClass = (new byte[0]).getClass();
  private static final Class charArrayClass = (new char[0]).getClass();

  static Object btocconv; 
  static Object ctobconv;

/*
  private static final Hashtable sunMapping = new Hashtable();
  static {
    sunMapping.put("ebcdic-cp-us", "cp037");
    sunMapping.put("ebcdic-cp-ca", "cp037");
    sunMapping.put("ebcdic-cp-nl", "cp037");
    sunMapping.put("ebcdic-cp-dk", "cp277");
    sunMapping.put("ebcdic-cp-no", "cp277");
    sunMapping.put("ebcdic-cp-fi", "cp278");
    sunMapping.put("ebcdic-cp-se", "cp278");
    sunMapping.put("ebcdic-cp-it", "cp280");
    sunMapping.put("ebcdic-cp-es", "cp284");
    sunMapping.put("ebcdic-cp-gb", "cp285");
    sunMapping.put("ebcdic-cp-fr", "cp297");
    sunMapping.put("ebcdic-cp-ar1", "cp420");
    sunMapping.put("ebcdic-cp-he", "cp424");
    sunMapping.put("ebcdic-cp-ch", "cp500");
    sunMapping.put("ebcdic-cp-roece", "cp870");
    sunMapping.put("ebcdic-cp-yu", "cp870");
    sunMapping.put("ebcdic-cp-is", "cp871");
    sunMapping.put("ebcdic-cp-ar2", "cp918");
  }
*/

  public static Object getConverter(String className, String encoding) throws UnsupportedEncodingException {
    try {
      Class converterClass = Class.forName(className);
      Method getConverterMethod = converterClass.getMethod("getConverter", new Class[] {String.class});
      return getConverterMethod.invoke(null, new String[] {encoding});
    } catch (ClassNotFoundException e) {
      e.printStackTrace();
      throw new RuntimeException("Class not found: " + className);
    } catch (NoSuchMethodException e) {
      e.printStackTrace();
      throw new RuntimeException("Method not found: getConverter");
    } catch (IllegalAccessException e) {
      e.printStackTrace();
      throw new RuntimeException("Catched an Illegal Access Exception calling getConverter");
    } catch (InvocationTargetException e) {
      e.printStackTrace();
      throw new RuntimeException("Catched an InvocationTargetException calling getConverter");
    }
  }

  public static byte Encode(int ch) {
    int bytecount;
    char[] decoded = new char[1];
    decoded[0] = (char)ch;
    byte[] encoded = new byte[1];
    try {
      bytecount = charToByteConvert(ctobconv, decoded, 0, decoded.length, encoded, 0, encoded.length);
    } catch (Exception e) {
      return UNSUPPORTED_CHAR;
    }

    if (bytecount == 0) {
      return NEEDS_MORE_DATA;
    } else {
      return encoded[0];
    }
  }

  public static int Decode(byte ch) {
    int bytecount;
    byte[] encoded = new byte[1];
    encoded[0] = ch;
    char[] decoded = new char[1];
    try {
      bytecount = byteToCharConvert(btocconv, encoded, 0, encoded.length, decoded, 0, decoded.length);
    } catch (Exception e) {
      return UNSUPPORTED_CHAR;
    }

    if (bytecount == 0) {
      return NEEDS_MORE_DATA;
    } else {
      return ((int)decoded[0] & 0xff);
    }
  }

  public static int charToByteConvert(Object ctobconv, char ac[], int i, int j, byte abyte0[], int k, int l) {
    try {
      Method method = ctobconv.getClass().getMethod("convert", new Class[] {charArrayClass, Integer.TYPE, Integer.TYPE, byteArrayClass, Integer.TYPE, Integer.TYPE});
      return ((Integer) method.invoke(ctobconv, new Object[] {ac, new Integer(i), new Integer(j), abyte0, new Integer(k), new Integer(l)})).intValue();
    } catch (SecurityException e) {
      throw new RuntimeException("Cannot process non-standard encoding on a JDK that is not provided by Sun: " + e);
    } catch (NoSuchMethodException e) {
      throw new RuntimeException("Cannot process non-standard encoding on a JDK that is not provided by Sun: " + e);
    } catch (IllegalArgumentException e) {
      throw new RuntimeException("Cannot process non-standard encoding on a JDK that is not provided by Sun: " + e);
    } catch (IllegalAccessException e) {
      throw new RuntimeException("Cannot process non-standard encoding on a JDK that is not provided by Sun: " + e);
    } catch (InvocationTargetException e) {
      throw new RuntimeException("Cannot process non-standard encoding on a JDK that is not provided by Sun: " + e);
    }
  }

  public static int byteToCharConvert(Object btocconv, byte[] abyte0, int i, int j, char ac[], int k, int l) {
    try {
      Method method = btocconv.getClass().getMethod("convert", new Class[] {byteArrayClass, Integer.TYPE, Integer.TYPE, charArrayClass, Integer.TYPE, Integer.TYPE});
      return ((Integer) method.invoke(btocconv, new Object[] {abyte0, new Integer(i), new Integer(j), ac, new Integer(k), new Integer(l)})).intValue();
    } catch (SecurityException e) {
      throw new RuntimeException("Cannot process non-standard encoding on a JDK that is not provided by Sun: " + e);
    } catch (NoSuchMethodException e) {
      throw new RuntimeException("Cannot process non-standard encoding on a JDK that is not provided by Sun: " + e);
    } catch (IllegalArgumentException e) {
      throw new RuntimeException("Cannot process non-standard encoding on a JDK that is not provided by Sun: " + e);
    } catch (IllegalAccessException e) {
      throw new RuntimeException("Cannot process non-standard encoding on a JDK that is not provided by Sun: " + e);
    } catch (InvocationTargetException e) {
      throw new RuntimeException("Cannot process non-standard encoding on a JDK that is not provided by Sun: " + e);
    }
  }

  public static void main(String[] args) {
    String enc = new String(args[0]);

    try {
      btocconv = getConverter("sun.io.ByteToCharConverter", enc);
      ctobconv = getConverter("sun.io.CharToByteConverter", enc);

      System.out.println("Scenario: Java -> EBCDIC -> Java");
      System.out.println();

/*
      for (int i = 0; i<256; i++) {
        byte b = Encode(i);
        int i_roundtrip = Decode(b);
        System.out.println("Original (Java):" + i + ", Encoded (EBCDIC):" + ((int)b & 0xff) + " Decoded again: "+ i_roundtrip);
      }
*/

      byte b = Encode(10);
      int i_roundtrip = Decode(b);
      System.out.println("Original (Java):" + 10 + ", Encoded (EBCDIC):" + ((int)b & 0xff) + " Decoded again: "+ i_roundtrip);

      b = Encode(133);
      i_roundtrip = Decode(b);
      System.out.println("Original (Java):" + 133 + ", Encoded (EBCDIC):" + ((int)b & 0xff) + " Decoded again: "+ i_roundtrip);

      System.out.println();
      System.out.println("Scenario: EBCDIC -> Java -> EBCDIC");
      System.out.println();

/*
      for (int i = 0; i<256; i++) {
        int decoded = Decode((byte)i);
        byte b = Encode(decoded);
        System.out.println("Original (EBCDIC):" + i + " Decoded (Java):" + decoded + " Encoded again:" + ((int)b & 0xff));
      }
*/

      int decoded = Decode((byte)21);
      b = Encode(decoded);
      System.out.println("Original (EBCDIC):" + 21 + " Decoded (Java):" + decoded + " Encoded again:" + ((int)b & 0xff));

      decoded = Decode((byte)37);
      b = Encode(decoded);
      System.out.println("Original (EBCDIC):" + 37 + " Decoded (Java):" + decoded + " Encoded again:" + ((int)b & 0xff));

    } catch (java.io.UnsupportedEncodingException e) {
      System.out.println("shitte");
    }
  }
}
