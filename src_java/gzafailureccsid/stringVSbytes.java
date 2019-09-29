public class stringVSbytes
{

    public static void main(String[] args)
    {
    	byte[] raw = {
            (byte) 0xd0, (byte) 0x0b, (byte) 0x01, (byte) 0x2a, (byte) 0x63,
            (byte) 0x78, (byte) 0x01};
        System.out.println(getHex(raw));
    }
    private static final String    HEXES    = "0123456789ABCDEF";

    static String getHex(byte[] raw) {
        final StringBuilder hex = new StringBuilder(2 * raw.length);
        for (final byte b : raw) {
            hex.append(HEXES.charAt((b & 0xF0) >> 4)).append(HEXES.charAt((b & 0x0F)));
        }
        return hex.toString();
    }
}