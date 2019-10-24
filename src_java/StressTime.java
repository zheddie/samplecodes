/******************************************************************************/
/* LICENSE AND DISCLAIMER:                                                    */
/*                                                                            */
/* This material contains IBM copyrighted sample programming source code for  */
/* your consideration. This sample code has not been thoroughly tested under  */
/* all conditions. IBM, therefore, cannot guarantee or imply reliability,     */
/* serviceability, or function.  IBM provides no program services for this    */
/* material. This material is provided "AS IS" WITHOUT WARRANTY OF ANY KIND,  */
/* EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED      */
/* WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT. SOME  */
/* JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF IMPLIED WARRANTIES, SO THE     */
/* ABOVE EXCLUSIONS MAY NOT APPLY TO YOU. IN NO EVENT WILL IBM BE LIABLE TO   */
/* ANY PARTY FOR ANY DIRECT, INDIRECT, SPECIAL OR OTHER CONSEQUENTIAL DAMAGES */
/* FOR ANY USE OF THIS MATERIAL INCLUDING, WITHOUT LIMITATION, ANY LOST       */
/* PROFITS, BUSINESS INTERRUPTION, LOSS OF PROGRAMS OR OTHER DATA ON YOUR     */
/* INFORMATION HANDLING SYSTEM OR OTHERWISE, EVEN IF EXPRESSLY ADVISED OF THE */
/* POSSIBILITY OF SUCH DAMAGES.                                               */
/*                                                                            */
/* COPYRIGHT                                                                  */
/* ---------                                                                  */
/* (C) Copyright IBM CORP. 2014, 2014                                         */
/* All rights reserved.                                                       */
/* US Government Users Restricted Rights -                                    */
/* Use, duplication or disclosure restricted                                  */
/* by GSA ADP Schedule Contract with IBM Corp.                                */
/* Licensed Material - Property of IBM                                        */
/******************************************************************************/
import java.util.Hashtable;
import java.util.Enumeration;

/**
 * StressDummy is a utility to test how fast a synchronization can happen with 
 * multiple threads in a JVM.
 */  
public class StressTime {
   
   /**
    * Main method, kicks off each test case.
    * @param args Not used.
    */           
   public static void main(String[] args) {
      try {
//         System.out.println("Press [ENTER] when ready to start test.");         //Pause to allow any traces to be set up          
  //       System.in.read();
    //     while(System.in.available()>0) System.in.read();
         runTest(10, 10000, 150);
         Thread.sleep(1000);
         runTest(20, 10000, 150);
         Thread.sleep(1000);
         runTest(30, 10000, 150);
         Thread.sleep(2000);
         runTest(30, 50000, 300);
      } catch(Exception e) {
         e.printStackTrace();                                                   //Don't expect errors
      }
   }

   /**
    * Runs one test accessing the hash table.
    * @param threads Number of threads to create.
    * @param loops Number of times each thread should run the clean up operation.
    * @param cachesize Number of entries that should be added to the table prior to testing.
    */                       
   private static void runTest(int threads, int loops, int cachesize) throws Exception {
      System.out.println("Running Synchronization test");
      System.out.println("  Number of threads:  " + threads);
      System.out.println("  Loops per thread:   " + loops);
      System.out.println("  Locks per loop:     " + cachesize);
      
      Long now = System.currentTimeMillis();
      Thread[] activethreads = new Thread[threads];
      for(int i=0; i<threads; i++) activethreads[i] = new Thread(new SyncAbuser(loops, cachesize)); //Create a thread using the Runnable interface
      for(int i=0; i<threads; i++) activethreads[i].start();                    //Start each thread
      for(int i=0; i<threads; i++) activethreads[i].join();                     //Block until each thread is finished
      Long end = System.currentTimeMillis();
      System.out.println("  Total time:         " + (end-now) + " ms");  
   }

   /**
    * Thread implemenation.  Runs the specified number of loops and exits.
    */       
   private static class SyncAbuser implements Runnable {
      private static SyncTime clock_ = new SyncTime();
      private int loopcount_;
      private int lockcount_;
      
      public SyncAbuser(int loopcount, int lockcount) {
         loopcount_ = loopcount;
         lockcount_ = lockcount;
      }
      
      public void run() { 
         for(int i=0; i<loopcount_; i++) {
            cleanUp();
         }        
      }
      
      private void cleanUp() {
         for(int i=0; i<lockcount_; i++) {
            long time = clock_.getTime();
         }
      }
   }
   
   private static class SyncTime {
      public long getTime() {
         return System.currentTimeMillis();
      }
   }
}
