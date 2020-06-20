
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import javasoft.sqe.javatest.Status;
import javasoft.sqe.javatest.lib.MultiTest;
import javasoft.sqe.jck.lib.EncodingUtils.*;



public class encode
{
    public static void main(String args[])
    {
        encode ss = new encode();
        ss.EncodeBaseRange0001(); 
    } 
 
 public Status EncodeBaseRange0001() {
        Status status = null;
        char[] test_chars = new char[2];
        EncodingUtils utils = new EncodingUtils(log);
        for (int i = 0; i < EncodingUtils.HIGH_SURROGATE_START - 1; i++) {
            test_chars[0] = (char)i;
            test_chars[1] = (char)(i+1);
            byte[] ref_bytes = utils.encodeUTF_16(test_chars);
            if (ref_bytes == null) {
                return Status.failed("Reference encoder failed to encode character array.");
            }
            status = utils.testCharToByte(test_chars, ref_bytes, EncodingUtils.UTF_16);
            if (status.isFailed()) {
                return status;
            }
        }

        // skip surrogates area
        for (int i = EncodingUtils.LOW_SURROGATE_END + 1; i < EncodingUtils.MAX_NON_SURROGATE_VALUE; i++) {
            test_chars[0] = (char)i;
            test_chars[1] = (char)(i+1);
            byte[] ref_bytes = utils.encodeUTF_16(test_chars);
            if (ref_bytes == null) {
                return Status.failed("Reference encoder failed to encode character array.");
            }
            status = utils.testCharToByte(test_chars, ref_bytes, EncodingUtils.UTF_16);
            if (status.isFailed()) {
                return status;
            }
        }

        // Test for 0xFFFF
        test_chars[0] = test_chars[1] = (char)0xFFFF;
        byte[] ref_bytes = utils.encodeUTF_16(test_chars);
        if (ref_bytes == null) {
            return Status.failed("Reference encoder failed to encode character array.");
        }
        status = utils.testCharToByte(test_chars, ref_bytes, EncodingUtils.UTF_16);
        if (status.isFailed()) {
            return status;
        }
        return Status.passed("OKAY");
    }
}