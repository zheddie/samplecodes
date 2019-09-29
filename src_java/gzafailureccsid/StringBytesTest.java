public class StringBytesTest
{
    private static final String    HEXES    = "0123456789ABCDEF";

    String getHex(byte[] raw) {
        final StringBuilder hex = new StringBuilder(2 * raw.length);
        for (final byte b : raw) {
            hex.append(HEXES.charAt((b & 0xF0) >> 4)).append(HEXES.charAt((b & 0x0F)));
        }
        return hex.toString();
    }
    public void printinBytes(String str)
    {
        char[] chars = str.toCharArray();
        byte[] bytes = new byte[chars.length * 2];
        for (int i = 0; i < chars.length; i++)
        {
            bytes[i ] = (byte) chars[i];
        }
        return bytes;
    }
    public static void main(String[] args)
    {
        StringBytesTest t = new StringBytesTest();
        t.StringBytesTest();
    }

    public void StringBytesTest()
    {
        String input = "Hèllo world";
        char[] chars = input.toCharArray();
        byte[] bytes = new byte[chars.length ];
        for(int i=0;i<chars.length;i++){
            bytes[i] = (byte)chars[i];
        }
        System.out.println(getHex(bytes));
        // byte[] inputBytes = GetBytes(input);
        
        // String output = GetString(inputBytes);
        // System.out.println(output);
    }

    public byte[] GetBytes(String str)
    {
        char[] chars = str.toCharArray();
        byte[] bytes = new byte[chars.length * 2];
        for (int i = 0; i < chars.length; i++)
        {
            bytes[i * 2] = (byte) (chars[i] >> 8);
            bytes[i * 2 + 1] = (byte) chars[i];
        }

        return bytes;
    }

    public String GetString(byte[] bytes)
    {
        char[] chars = new char[bytes.length / 2];
        char[] chars2 = new char[bytes.length / 2];
        for (int i = 0; i < chars2.length; i++)
            chars2[i] = (char) ((bytes[i * 2] << 8) + (bytes[i * 2 + 1] & 0xFF));

        return new String(chars2);

    }
}