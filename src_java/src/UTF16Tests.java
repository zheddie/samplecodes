/*
 * Copyright (c) 2003, 2004, Oracle and/or its affiliates. All rights reserved.
 */

/*
 *   @(#)UTF16.test.xml 04/02/18 Pavel Ozhdikhin
 * Generated from : api/java_io/mbCharEncoding/UTF16.test.xml
 *
 * UTF-16 encoding tests
 * <P>See spec for <code>java.nio.charset.Charset</code>:
 * 
 * <h4>Standard charsets</h4>
 * 
 * <p> Every implementation of the Java platform is required to support the
 * following standard charsets.  Consult the release documentation for your
 * implementation to see if any other charsets are supported.
 * 
 * <blockquote><table width="80%">
 * <tr><td valign=top><tt>US-ASCII</tt></td>
 * <td>Seven-bit ASCII, a.k.a. <tt>ISO646-US</tt>,
 * a.k.a. the Basic Latin block of the Unicode character set</td></tr>
 * <tr><td valign=top><tt>ISO-8859-1&nbsp;&nbsp;</tt></td>
 * <td>ISO Latin Alphabet No. 1, a.k.a. <tt>ISO-LATIN-1</tt></td></tr>
 * <tr><td valign=top><tt>UTF-8</tt></td>
 * <td>Eight-bit UCS Transformation Format</td></tr>
 * <tr><td valign=top><tt>UTF-16BE</tt></td>
 * <td>Sixteen-bit UCS Transformation Format,
 * big-endian byte&nbsp;order</td></tr>
 * <tr><td valign=top><tt>UTF-16LE</tt></td>
 * <td>Sixteen-bit UCS Transformation Format,
 * little-endian byte&nbsp;order</td></tr>
 * <tr><td valign=top><tt><b>UTF-16</b></tt></td>
 * <td><b>Sixteen-bit UCS Transformation Format,
 * byte&nbsp;order identified by an optional byte-order mark</b></td></tr>
 * </table></blockquote>
 * 
 * <p> The <tt>UTF-8</tt> charset is specified by <a
 * href="http://ietf.org/rfc/rfc2279.txt"><i>RFC&nbsp;2279</i></a>; the
 * transformation format upon which it is based is specified in
 * Amendment&nbsp;2 of ISO&nbsp;10646-1 and is also described in
 * &#167;&nbsp;3.8 of <a
 * href="http://www.unicode.org/unicode/standard/standard.html"><i>The Unicode
 * Standard, Version&nbsp;3.0</i></a> (<a
 * href="http://www.unicode.org/unicode/uni2errata/UTF-8_Corrigendum.html">amended</a>).
 * 
 * <p> The <tt>UTF-16</tt> charsets are specified by <a
 * href="http://ietf.org/rfc/rfc2781.txt"><i>RFC&nbsp;2781</i></a>; the
 * transformation formats upon which they are based are specified in
 * Amendment&nbsp;1 of ISO&nbsp;10646-1 and are also described in
 * &#167;&nbsp;3.8 of <a
 * href="http://www.unicode.org/unicode/standard/standard.html"><i>The Unicode
 * Standard, Version&nbsp;3.0</i></a>.
 * 
 * <p> The <tt>UTF-16</tt> charsets use sixteen-bit quantities and are
 * therefore sensitive to byte order.  In these encodings the byte order of a
 * stream may be indicated by an initial <i>byte-order mark</i> represented by
 * the Unicode character <tt>'&#92;uFEFF'</tt>.  Byte-order marks are handled
 * as follows:
 * 
 * <ul>
 * 
 * <li><p> When decoding, the <tt>UTF-16BE</tt> and <tt>UTF-16LE</tt>
 * charsets ignore byte-order marks; when encoding, they do not write
 * byte-order marks. </p></li>
 * 
 * <li><p> When decoding, the <tt>UTF-16</tt> charset interprets a byte-order
 * mark to indicate the byte order of the stream but defaults to big-endian
 * if there is no byte-order mark; when encoding, it uses big-endian byte
 * order and writes a big-endian byte-order mark. </p></li>
 * 
 * </ul>
 * 
 * In any case, when a byte-order mark is read at the beginning of a decoding
 * operation it is omitted from the resulting sequence of characters.  Byte
 * order marks occuring after the first element of an input sequence are not
 * omitted since the same code is used to represent <small>ZERO-WIDTH
 * NON-BREAKING SPACE</small>.
 */

package javasoft.sqe.tests.api.java.nio.charset.Charset;

import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import javasoft.sqe.javatest.Status;
import javasoft.sqe.javatest.lib.MultiTest;
import javasoft.sqe.jck.lib.EncodingUtils.*;


public class UTF16Tests extends MultiTest {

    /* standalone interface */
    public static void main(String argv[]) {
        UTF16Tests test = new UTF16Tests();
        test.run(argv, System.err, System.out).exit();
    }

    /**
     * Assertion testing
     * for UTF-16 encoding tests,
     * <p>Every implementation of the Java platform is required to support the following standard charset:
     * UTF-16 - Sixteen-bit UCS Transformation Format, byte&nbsp;order identified by an optional byte-order mark.
     * <p>See <code>java.nio.charset.Charset</code> specification:
     * <p>When decoding, the UTF-16 charset interprets a byte-order mark to indicate the byte order of the stream but defaults to big-endian if
     * there is no byte-order mark; when encoding, it uses big-endian byte order and writes a big-endian byte-order mark.
     * <p>The test checks UTF-16 encoding of the base range of char values, excluding surrogate values..
     * <br><b>Expected results</b>: Encoded byte array should be binary equal to the byte array encoded by the reference encoder.
     */
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

    /**
     * Assertion testing
     * for UTF-16 encoding tests,
     * <p>Every implementation of the Java platform is required to support the following standard charset:
     * UTF-16 - Sixteen-bit UCS Transformation Format, byte&nbsp;order identified by an optional byte-order mark.
     * <p>See <code>java.nio.charset.Charset</code> specification:
     * <p>When decoding, the UTF-16 charset interprets a byte-order mark to indicate the byte order of the stream but defaults to big-endian if
     * <p>The test checks UTF-16 encoding of the surrogate char values..
     * <br><b>Expected results</b>: Encoded byte array should be binary equal to the byte array encoded by the reference encoder.
     */
    public Status EncodeSurrogates0001() {
        Status status = null;
        char[] test_chars = new char[2];
        EncodingUtils utils = new EncodingUtils(log);
        for (int i = EncodingUtils.HIGH_SURROGATE_START; i <= EncodingUtils.HIGH_SURROGATE_END; i++) {
            test_chars[0] = (char) i;
            test_chars[1] = (char) (EncodingUtils.LOW_SURROGATE_START + (i - EncodingUtils.HIGH_SURROGATE_START));
            byte[] ref_bytes = utils.encodeUTF_16(test_chars);
            if (ref_bytes == null) {
                return Status.failed("Reference encoder failed to encode character array.");
            }
            status = utils.testCharToByte(test_chars, ref_bytes, EncodingUtils.UTF_16);
            if (status.isFailed()) {
                return status;
            }
        }
        return Status.passed("OKAY");
    }

    /**
     * Assertion testing
     * for UTF-16 encoding tests,
     * <p>Every implementation of the Java platform is required to support the following standard charset:
     * UTF-16 - Sixteen-bit UCS Transformation Format, byte&nbsp;order identified by an optional byte-order mark.
     * <p>See <code>java.nio.charset.Charset</code> specification:
     * <p>When decoding, the UTF-16 charset interprets a byte-order mark to indicate the byte order of the stream but defaults to big-endian if
     * <p>The test checks UTF-16 encoding of the surrogate char values..
     * <br><b>Expected results</b>: Decoded character array should be equal to the character array decoded by the reference decoder.
     */
    public Status DecodeBaseRange0001() {
        Status status = null;
        EncodingUtils utils = new EncodingUtils(log);
        char[] ref_chars = null;

        // Default big-endian format (except the case when 0xFEFF has been obtained as a cycle iterator value)

        for (int i = 0; i < EncodingUtils.HIGH_SURROGATE_START - 1; i++) {
            byte[] test_bytes = new byte[4];
            test_bytes[0] = (byte)(i >> 8);
            test_bytes[1] = (byte)(i & 0xFF);
            test_bytes[2] = (byte)((i + 1) >> 8);
            test_bytes[3] = (byte)((i + 1) & 0xFF);
            ref_chars = utils.decodeUTF_16(test_bytes);
            if (ref_chars == null) {
                return Status.failed("Reference decoder failed to decode byte array.");
            }
            status = utils.testByteToChar(test_bytes, ref_chars, EncodingUtils.UTF_16);
            if (status.isFailed()) {
                return status;
            }
        }

        // skip surrogates area, 0xFFFF and 0xFFFE - non-Unicode characters
        for (int i = EncodingUtils.LOW_SURROGATE_END + 1; i < EncodingUtils.LE_BYTE_ORDER_MARK - 1; i++) {
            byte[] test_bytes = new byte[4];
            test_bytes[0] = (byte)(i >> 8);
            test_bytes[1] = (byte)(i & 0xFF);
            test_bytes[2] = (byte)((i + 1) >> 8);
            test_bytes[3] = (byte)((i + 1) & 0xFF);
            ref_chars = utils.decodeUTF_16(test_bytes);
            if (ref_chars == null) {
                return Status.failed("Reference decoder failed to decode byte array.");
            }
            status = utils.testByteToChar(test_bytes, ref_chars, EncodingUtils.UTF_16);
            if (status.isFailed()) {
                return status;
            }
        }

        // Big-endian format indicated by the byte order mark

        for (int i = 0; i < EncodingUtils.HIGH_SURROGATE_START - 1; i++) {
            byte[] test_bytes = new byte[6];
            test_bytes[0] = (byte)(EncodingUtils.BE_BYTE_ORDER_MARK >> 8);
            test_bytes[1] = (byte)(EncodingUtils.BE_BYTE_ORDER_MARK & 0xFF);
            test_bytes[2] = (byte)(i >> 8);
            test_bytes[3] = (byte)(i & 0xFF);
            test_bytes[4] = (byte)((i + 1) >> 8);
            test_bytes[5] = (byte)((i + 1) & 0xFF);
            ref_chars = utils.decodeUTF_16(test_bytes);
            if (ref_chars == null) {
                return Status.failed("Reference decoder failed to decode byte array.");
            }
            status = utils.testByteToChar(test_bytes, ref_chars, EncodingUtils.UTF_16);
            if (status.isFailed()) {
                return status;
            }
        }

        // skip surrogates area and 0xFFFF, 0xFFFE - non-Unicode characters
        for (int i = EncodingUtils.LOW_SURROGATE_END + 1; i < EncodingUtils.LE_BYTE_ORDER_MARK - 1; i++) {
            byte[] test_bytes = new byte[6];
            test_bytes[0] = (byte)(EncodingUtils.BE_BYTE_ORDER_MARK >> 8);
            test_bytes[1] = (byte)(EncodingUtils.BE_BYTE_ORDER_MARK & 0xFF);
            test_bytes[2] = (byte)(i >> 8);
            test_bytes[3] = (byte)(i & 0xFF);
            test_bytes[4] = (byte)((i + 1) >> 8);
            test_bytes[5] = (byte)((i + 1) & 0xFF);
            ref_chars = utils.decodeUTF_16(test_bytes);
            if (ref_chars == null) {
                return Status.failed("Reference decoder failed to decode byte array.");
            }
            status = utils.testByteToChar(test_bytes, ref_chars, EncodingUtils.UTF_16);
            if (status.isFailed()) {
                return status;
            }
        }

        // Low-endian format indicated by the byte order mark

        for (int i = 0; i < EncodingUtils.HIGH_SURROGATE_START - 1; i++) {
            byte[] test_bytes = new byte[6];
            test_bytes[0] = (byte)(EncodingUtils.LE_BYTE_ORDER_MARK >> 8);
            test_bytes[1] = (byte)(EncodingUtils.LE_BYTE_ORDER_MARK & 0xFF);
            test_bytes[2] = (byte)(i & 0xFF);
            test_bytes[3] = (byte)(i >> 8);
            test_bytes[4] = (byte)((i + 1) & 0xFF);
            test_bytes[5] = (byte)((i + 1) >> 8);
            ref_chars = utils.decodeUTF_16(test_bytes);
            if (ref_chars == null) {
                return Status.failed("Reference decoder failed to decode byte array.");
            }
            status = utils.testByteToChar(test_bytes, ref_chars, EncodingUtils.UTF_16);
            if (status.isFailed()) {
                return status;
            }
        }

        // skip surrogates area and 0xFFFF, 0xFFFE - non-Unicode characters
        for (int i = EncodingUtils.LOW_SURROGATE_END + 1; i < EncodingUtils.LE_BYTE_ORDER_MARK - 1; i++) {
            byte[] test_bytes = new byte[6];
            test_bytes[0] = (byte)(EncodingUtils.LE_BYTE_ORDER_MARK >> 8);
            test_bytes[1] = (byte)(EncodingUtils.LE_BYTE_ORDER_MARK & 0xFF);
            test_bytes[2] = (byte)(i & 0xFF);
            test_bytes[3] = (byte)(i >> 8);
            test_bytes[4] = (byte)((i + 1) & 0xFF);
            test_bytes[5] = (byte)((i + 1) >> 8);
            ref_chars = utils.decodeUTF_16(test_bytes);
            if (ref_chars == null) {
                return Status.failed("Reference decoder failed to decode byte array.");
            }
            status = utils.testByteToChar(test_bytes, ref_chars, EncodingUtils.UTF_16);
            if (status.isFailed()) {
                return status;
            }
        }
        return Status.passed("OKAY");
    }
}
