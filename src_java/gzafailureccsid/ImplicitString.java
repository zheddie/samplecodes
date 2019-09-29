package com.ibm.iTestJava.gvt.String;


import java.io.*;

import com.ibm.iseries.Environment;

import javasoft.sqe.javatest.Status;
import javasoft.sqe.javatest.lib.MultiTest;



/**
 * @author alwadhwa 3/2006
 * Purpose of test: To test whether Implicit byte-to-char conversion works
 * for different locales
 * 
 * 
 */

public class ImplicitString extends MultiTest {
    
    public static  String getPath()
    {
        return Environment.getCwdForPackage("com/ibm/iTestJava/gvt/String/");
    }
    
    public static String getClassPath()
    {        
        String packagename = "com/ibm/iTestJava/gvt/String/";
        return getPath().substring(0,(getPath().length() - packagename.length())).trim();
    }    

    public static void main(String[] args) 
    {
        ImplicitString test = new ImplicitString();
        test.run(args, System.err, System.out).exit();
    }
     
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - English
     * 
     * @return whether it does or not
     */
    public Status ImplicitString001()
    {
        String name = "English";
        String langId = "en";
        String countryId = "US";
        String shortDateStr = "10/25/97";
        String shortTimeStr = "9:59 PM";
        String numberStr = "123,456.789";
        int    ccsid = 37;
        String encoding = "Cp037";

	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");

    }
    
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - British
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0002()
    {
        String name = "British";
        String langId = "en";
        String countryId = "GB";
        String shortDateStr = "25/10/97";
        String shortTimeStr = "21:59";
        String numberStr = "123,456.789";
        int    ccsid = 37;
        String encoding = "Cp037";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");

    }
    
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - French
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0003()
    {
        String name = "French";
        String langId = "fr";
        String countryId = "FR";
        String shortDateStr = "25/10/97";
        String shortTimeStr = "21:59";
        String numberStr = "123\u00a0456,789";
        int    ccsid = 297;
        String encoding = "Cp297";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");

    }
    
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - German
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0004()
    {
        String name = "German";
        String langId = "de";
        String countryId = "DE";
        String shortDateStr = "25.10.97";
        String shortTimeStr = "21:59";
        String numberStr = "123.456,789";
        int    ccsid = 273;
        String encoding = "Cp273";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");

    }
    
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - Italian
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0005()
    {
        String name = "Italian";
        String langId = "it";
        String countryId = "IT";
        String shortDateStr = "25/10/97";
        String shortTimeStr = "21.59";
        String numberStr = "123.456,789";
        int    ccsid = 280;
        String encoding = "Cp280";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");
    }
    
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - Spanish
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0006()
    {
        String name = "Spanish";
        String langId = "es";
        String countryId = "ES";
        String shortDateStr = "25/10/97";
        String shortTimeStr = "21:59";
        String numberStr = "123.456,789";
        int    ccsid = 284;
        String encoding = "Cp284";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");
    }
    
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - Russian
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0007()
    {
        String name = "Russian";
        String langId = "ru";
        String countryId = "RU";
        String shortDateStr = "25.10.97";
        String shortTimeStr = "21:59";
        String numberStr = "123\u00a0456,789";
        int    ccsid = 1025;
        String encoding = "Cp1025";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");
    }
    
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - Greek
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0008()
    {
        String name = "Greek";
        String langId = "el";
        String countryId = "GR";
        String shortDateStr = "25/10/1997";
        String shortTimeStr = "9:59 \u03bc\u03bc";
        String numberStr = "123.456,789";
        int    ccsid = 875;
        String encoding = "Cp875";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");
    }
    
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - Japanese
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0010()
    {
        String name = "Japanese";
        String langId = "ja";
        String countryId = "JP";
        String shortDateStr = "97/10/25";
        String shortTimeStr = "21:59";
        String numberStr = "123,456.789";
        int    ccsid = 930;
        String encoding = "Cp930";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");
    }
  
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - Japanese
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0011()
    {
        String name = "Japanese";
        String langId = "ja";
        String countryId = "JP";
        String shortDateStr = "97/10/25";
        String shortTimeStr = "21:59";
        String numberStr = "123,456.789";
        int    ccsid = 939;
        String encoding = "Cp939";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");
    }

    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - Japanese
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0012()
    {
        String name = "Japanese";
        String langId = "ja";
        String countryId = "JP";
        String shortDateStr = "97/10/25";
        String shortTimeStr = "21:59";
        String numberStr = "123,456.789";
        int    ccsid = 942;
        String encoding = "Cp942";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");
    }

    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - Japanese
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0009()
    {
        String name = "Japanese";
        String langId = "ja";
        String countryId = "JP";
        String shortDateStr = "97/10/25";
        String shortTimeStr = "21:59";
        String numberStr = "123,456.789";
        int    ccsid = 932;
        String encoding = "SJIS";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");
    }
    
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - S-Chinese
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0013()
    {
        String name = "S-Chinese";
        String langId = "zh";
        String countryId = "CN";
        String shortDateStr = "97-10-25";
        String shortTimeStr = "\u4e0b\u53489:59";
        String numberStr = "123,456.789";
        int    ccsid = 1381;
        // String encoding = "GB2312";   // Not supported in J9.  Should really be GBK
	// Could also use EUC_CN, but some think this is not a correct alias.
	String encoding = "GBK";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;

        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");
    }

    public Status ImplicitString0013a()
    {
        String name = "S-Chinese";
        String langId = "zh";
        String countryId = "CN";
        String shortDateStr = "97-10-25";
        String shortTimeStr = "\u4e0b\u53489:59";
        String numberStr = "123,456.789";
        int    ccsid = 1381;
        // String encoding = "GB2312";   // Not supported in J9.  Should really be GBK
	// Could also use EUC_CN, but some think this is not a correct alias.
	String encoding = "EUC_CN";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;

        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");
    }
    
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - T-Chinese
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0014()
    {
        String name = "T-Chinese";
        String langId = "zh";
        String countryId = "TW";
        String shortDateStr = "1997/10/25";
        String shortTimeStr = "\u4e0b\u5348 9:59";
        String numberStr = "123,456.789";
        int    ccsid = 950;
        String encoding = "Big5";
        
	String cmd_classpath = " -cp "+getClassPath()+" ";

        Process process = null;
        
        String cmd = "java -Dfile.encoding=" + encoding + cmd_classpath + " com.ibm.iTestJava.gvt.String.ImplicitStringHELPER " + encoding;
        try 
        {
            process = Runtime.getRuntime().exec(cmd);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
            return Status.failed("IOException: Was not able to fork off a process to run this command " + cmd);
        }
        
        System.out.println("cmd = " + cmd);
        
        // Wait for the above process to complete
        
        try 
        {
            process.waitFor();
        } 
        catch (InterruptedException e) 
        {
            e.printStackTrace();
            return Status.failed("We were interrupted while waiting for the Process to finish ");
        }
        
        System.out.println("Process 1 exit value is : " + process.exitValue());
        
        //  Get the output stream of the process
        
        int nextCharacter1 = 0;
        
        BufferedReader br1 = null;
        StringBuffer sb1 = new StringBuffer();
        
        br1 = new BufferedReader ( new InputStreamReader( process.getInputStream())) ;        
        
        try 
        {
            
            nextCharacter1 = br1.read();
            
            while ( nextCharacter1 != -1 )
            {
                sb1.append((char) nextCharacter1);
                nextCharacter1 = br1.read();
            }
            
            br1.close();   
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return Status.failed("IOException: was not able to read from the process output stream");
        }
        
        String strFromStdOut = sb1.substring(0);
          
        System.out.println("strFromStdOut = " + strFromStdOut);
        
        String EncStrFromStdOut = null;
        try 
        {
           EncStrFromStdOut = new String (sb1.substring(0).getBytes(), encoding);
        }
        catch (UnsupportedEncodingException e) 
        {
           e.printStackTrace();
           return Status.failed("Unsupported encoding exception");
           
        }
       
       System.out.println("EncStrFromStdOut = " + EncStrFromStdOut);
       System.out.println("EncStrFromStdOut.indexOf(\"PASSED\")=" + EncStrFromStdOut.indexOf("PASSED"));
       if (EncStrFromStdOut.indexOf("PASSED") >= 0)
       {
           // Passed
       }
       else
       {
           return Status.failed("This encoding " + encoding + " failed. Expected PASSED Actual : " + EncStrFromStdOut + strFromStdOut); 
       }    
       
       return Status.passed("Encoding " + encoding + " passed.");
    }
    
    /**
     * Tests if Implicit byte-to-char conversion works for 
     * - Korean
     * 
     * @return whether it does or not
     */
    public Status ImplicitString0015()
    {
        String name = "Korean";
        String langId = "ko";
        String countryId = "KO";
        String shortDateStr = "97. 10. 25";
        String shortTimeStr = "\uc624\ud6c4 9:59";
        String numberStr = "123,456.789";
        int    ccsid = 949;
        String encoding = "KSC5601";
        
        
        return Status.passed("Encoding " + encoding + " passed.");

    }
    
    
}

//package javasoft.sqe.tests.os400.nls.String; 
//
//import java.io.*;
//import java.text.*;
//import java.util.*;
//import javasoft.sqe.tests.os400.nls.util.*;
//
//class ImplicitString {
//  static private String outFileName1 = null;
//  static final int      BUFFERLEN  = 1024;
//
//  public static void main (String args[]) {
//
//    byte[]              buffer1             = new byte[BUFFERLEN];
//    byte[]              buffer2             = new byte[BUFFERLEN];
//    String              inputFilename       = null;
//    BufferedInputStream input               = null;
//    String              s1                  = null;
//    String              s2                  = null;
//    TestLocale          testLocale          = null;
//
//    ProcessCommandLine(args);
//
//    // Setup Error handling
//    Utilities.init("ImplicitString");
//
//    // Setup for statistics gathering
//    Stats stats = new Stats("ImplicitString");
//
//    // Setup the locale-specific information
//    TestLocales.init();
//
//    // Loop through test locales
//    for (int i = 0; i < TestLocales.numTestLocales(); ++i) { 
//       testLocale = TestLocales.nthTestLocale(i);
//
//       for (int j = 0; j < testLocale.numEncodings(); j++) {
//         // Get the encoding
//         String encoding = testLocale.getEncoding(j);
//         inputFilename = TestLocales.FILEPATH +"Inputdata." + encoding;
//
//         try {
//           // Open the input file
//           input = new BufferedInputStream( new FileInputStream( inputFilename ) );
//
//           // Read contents of file into 'buffer'
//           int inputLen = input.read(buffer1, 0, BUFFERLEN);
//           if (inputLen == BUFFERLEN) {
//             Utilities.InternalTestError("Input file too large for buffer");
//           } // if   
//         } // try
//         catch (IOException e) { Utilities.DumpExceptionMessage( e, inputFilename ); }
//
//         // Instantiate a string to perform implicit conversion
//         s1 = new String(buffer1);
//         
//         // Instantiate another string using explicit conversion
//         try {
//           s2 = new String(buffer1, encoding);
//         }
//         catch (UnsupportedEncodingException e) { Utilities.DumpExceptionMessage(e, encoding); }
//         
//         // Compare s1 with s2
//         if (! s1.equals(s2)) {
//           System.out.println("s1 = " + s1);
//           System.out.println("s2 = " + s2);
//           Utilities.TestError("Implicit byte-to-char conversion failed for encoding " + encoding);
//         }
//
//       } // for j
//    } // for i
//
//    if (Utilities.FileCompare( "foo", "bar" )) {
//      System.out.println("Files are the same");
//    } else {
//      System.out.println("Files are different");
//    }
//    
//  } // main
//
//
//  static private void ProcessCommandLine( String args[] ) {
//    // Check number of arguments
//    if ( args.length > 0 ) {
//      Usage();
//    }
//  }
//
//  static private void Usage() {
//    System.err.println();
//    System.err.println("Usage: ");
//    System.err.println("  ImplicitString");
//    System.err.println();
//
//    System.exit(1); 
//  }
//
//} // ImplicitString