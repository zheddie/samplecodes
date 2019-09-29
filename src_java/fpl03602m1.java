/*
 * Ident: @(#)fpl03602m1.java generated from: @(#)fpl03602m.jmpp 14/09/16
 *
 * Copyright (c) 1998, 2014, Oracle and/or its affiliates. All rights reserved.
 */


import java.io.PrintStream;
  
public class fpl03602m1 {

    public static void main (String args[]) {
        System.exit(run(args, System.out) + 95/*STATUS_TEMP*/);
    };


    public static int run (String args[], PrintStream out) {
	return (new fpl03602m1()).reallyRun(args,out);
    };

    strictfp int reallyRun (String args[], PrintStream out) {
	boolean OK = true;
	int format_std_width = 24; // single-standard mantissa width
	int format_min_width = 52; //32; // shortest single-extended mantissa
	int format_max_width = 54; // need 63-bit positive long to check format 62

	// If actual float format has mantissa wider than format_max_width 
	// bits, this test does not checks correctness of the implementation.

	float correct_value = Float.intBitsToFloat(0x3f800001); // 1+epsilon
	out.println("correct_value:" + correct_value + " == " +
     "Float.intBitsToFloat(0x3f800001)");
	out.println(Integer.toBinaryString(Float.floatToIntBits(correct_value)));
	out.println("");

	//for (int format_width=format_min_width; 
	//	 format_width<=format_max_width;
	//	 format_width++) {
	int	format_width = 53;
	    int diff = format_width - format_std_width;

	    long tricky_bits;
	    float test_value;
	    int test;

	    tricky_bits = (((1L << format_std_width) + 1) << diff) + 1;
	    out.println("tricky_bits:" + tricky_bits);
	    test_value = (float)tricky_bits;
	    out.println("after (float):" + test_value);
	    test = Float.floatToIntBits(test_value);
	    out.println("test_int:" + test);

	long zLong = tricky_bits / (1L<<diff)/(1L<<format_std_width);
	float zfloat = (float)tricky_bits;
	out.println("zg.java.zLong = "+zLong);
//	    test_value = (float)tricky_bits / (1L<<diff)/(1L<<format_std_width);
	    test_value = (float)zLong;
		
	  //  if (test_value != correct_value) {
		out.println("");
		out.println("float test: format width=" +
		    format_width);
		out.println("test_value: " + test_value);
		out.println("correct_val:" + correct_value + " == " +
		    "Float.intBitsToFloat(0x3f000001)");
		out.println("");

		OK = false;
	    //};
	//};

        return (OK? 0/*STATUS_PASSED*/: 2/*STATUS_FAILED*/);
    };

}
