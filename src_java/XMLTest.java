
/*
*
* Copyright (c) 2005 Standard Performance Evaluation Corporation (SPEC) All
* rights reserved. Copyright (c) 2005 Sun Microsystems, Inc. All rights
* reserved.
*
* This source code is provided as is, without any express or implied warranty.
*/
//package spec.jbb.infra.Util;

import java.io.FileOutputStream;
import java.io.FileDescriptor;
import java.util.ArrayList;
import java.util.NoSuchElementException;
import java.util.Random;
import java.util.Set;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import org.xml.sax.SAXException;
import java.io.IOException;
import org.w3c.dom.*;

class XMLLineDocumentException extends RuntimeException {
   /**
    * serialversionUID = 1 for first release
    */
   private static final long serialVersionUID = 1L;

   // This goes right after each class/interface statement
   static final String       COPYRIGHT        = "SPECjbb2005,"
                                                      + "Copyright (c) 2005 Standard Performance Evaluation Corporation (SPEC),"
                                                      + "All rights reserved,"
                                                      + "(C) Copyright Sun Microsystems, Inc. 2005"
                                                      + "All rights reserved,"
                                                      + "Licensed Materials - Property of SPEC";

   XMLLineDocumentException() {
       super();
   };

   XMLLineDocumentException(String s) {
       super(s);
   };
};



public class XMLTest implements Runnable {
   // This goes right after each class/interface statement
   static final String     COPYRIGHT = "SPECjbb2005,"
                                             + "Copyright (c) 2005 Standard Performance Evaluation Corporation (SPEC),"
                                             + "All rights reserved,"
                                             + "(C) Copyright Sun Microsystems, Inc."
                                             + "All rights reserved,"
                                             + "Licensed Materials - Property of SPEC";

   static FileOutputStream fout      = new FileOutputStream(FileDescriptor.out);

   static Document         templateDocument;

   static DocumentBuilder  builder;
   static {
       // initialize document
       DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
       try {
           builder = factory.newDocumentBuilder();
           templateDocument = builder.parse("template-document.xml");
       }
       catch (SAXException sxe) {
           // Error generated during parsing)
           System.err
                   .println("SAX Error in template-document initialization.");
           Exception ex = sxe;
           if (sxe.getException() != null)
               ex = sxe.getException();
           ex.printStackTrace();
           System.exit(1);
       }
       catch (ParserConfigurationException pce) {
           // Parser with specified options can't be built
           System.err
                   .println("Parser configuration error in template-document initialization.");
           pce.printStackTrace();
           System.exit(1);
       }
       catch (IOException ioe) {
           // I/O error
           System.err
                   .println("I/O error in template-document initialization.");
           ioe.printStackTrace();
           System.exit(1);
       }
   }

   public ArrayList<Node> lineCache = null;
   public static Vector<Thread>        threadList;
   Document                document;
   public static long diffTotal= 0 ;
   public static long diffCount = 0;
   public static long cacheSize = 0;
   public static long maxDiff = 0;
   public static long newNodeCount = 0;
   public static Vector<Integer> al = null;
   public static Object sync = null; 
   
   public static void main(String[] args) {
		// TODO Auto-generated method stub
		int iThrdCount = 8;
		//System.out.print("args[1]="+args[1]);
		if(args.length > 0){
			iThrdCount = Integer.valueOf(args[0]).intValue();
		}
		threadList = new Vector<Thread>();
		sync = new Object();
		al = new Vector<Integer>();
		//System.out.println("zgHash.size="+zgHash.size());
		//HashMapTest [] ht=new HashMapTest[128]; 
		for(int i =0;i<iThrdCount;i++){
			XMLTest ht = new XMLTest();
			Thread aT = new Thread(ht);
			threadList.add(aT);
			aT.start();
		}
		while (!threadList.isEmpty()) {
           try {
               while (threadList.firstElement().isAlive()) {
               	Thread.sleep(500);
               }
               threadList.remove(0);
           }
           catch (NoSuchElementException e) {
               System.out.println("No first element in the thread list: ");
               e.printStackTrace();
           } catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
       }	
	   System.out.println("diffTotal="+diffTotal
			   +",maxDiff="+maxDiff
			   +",diffCount="+diffCount
			   +",avgDiff="+diffTotal/iThrdCount
			   +",newNodeCount="+ newNodeCount
			   +",avgCacheSize="+Double.valueOf(cacheSize)/iThrdCount
			   +",allDiffs="+al.toString()
			   );

	}
   public XMLTest() {
       // create new line cache
       lineCache = new ArrayList<Node>(0);
       // copy from template XML document
       copy(templateDocument);
   }

   public void clear() {
       Element baseElement = document.getDocumentElement();
       Node current_node = baseElement.getLastChild();
       Node next_node = null;
       while ((next_node = current_node.getPreviousSibling()) != null) {
           Node lineNode = baseElement.removeChild(current_node);
           if (lineNode.getNodeName().equals("Line")) {
               // set the removed line's LineData Text Value to ""
               lineNode.getLastChild().getLastChild().setNodeValue("");
               // add the removed line to the lineCache
               lineCache.add(lineNode);
           }
           else{
        	  System.out.println("NodeName="+lineNode.getNodeName()); 
           }
           current_node = next_node;
       }
   };

   public void copy(Document master) {
       // copy the document
       document = (Document) master.cloneNode(true);
   }

   public final void putLine(String s, int n) {
       int line_number = n;
       Node jbbDocument = document.getLastChild();
       // Check and see if a line element is available
       // in the line cache
       int cacheLength = lineCache.size();
       if (cacheLength > 0) {
    	   //System.out.println("lineCacheSize="+lineCache.size());
           // fetch a line from the line cache
           Node lineNode = lineCache.remove(cacheLength - 1);
           jbbDocument.appendChild(lineNode);
           // set the TextNode of the LineData child from lineNode
           lineNode.getLastChild().getLastChild().setNodeValue(s);
       }
       else {
    	   synchronized(XMLTest.sync){
    		   XMLTest.newNodeCount ++;
    	   }
           // Create a new line element and append it to the document
           Element lineNode = (Element) document.createElement("Line");
           jbbDocument.appendChild(lineNode);
           Element newData = (Element) document.createElement("LineData");
           lineNode.appendChild(newData);
           Node new_node = document.createTextNode(s);
           newData.appendChild(new_node);
       }
   }
@Override
public void run() {
	// TODO Auto-generated method stub
	   XMLTest xt = new XMLTest();
	   
	   String ip = new String();
	   for(int j = 0;j<80;j++){
		   ip = ip+ String.valueOf(j%10);
	   }
	   Random r = new Random();
	   r.setSeed(System.currentTimeMillis());
	   long dt = 0;
	   long md = 0;
	   int dc =0;
	   
	   //System.out.println("ip="+ip);
	   long loopCount= 1000*100;
	   for(int j = 0;j<loopCount;j++){
		   long timeMill = System.currentTimeMillis();
		   xt.clear();
		   
		   
		   int x = r.nextInt()%24;
		   for(int i =0;i<x;i++){
			   xt.putLine(ip, i);
		   }
		   long diff = System.currentTimeMillis() - timeMill;
		   if(diff > 0){
			   dt+=diff;
			   if(diff > md ) md = diff;
			   dc++;
		   }
		   
	   }
	   System.out.println("["+Thread.currentThread().getId()+"],diffTotal="+dt+",maxDiff="+md+",diffCont="+diffCount);
	   synchronized(XMLTest.sync){
		   XMLTest.diffTotal += dt;
		   if(md > XMLTest.maxDiff) XMLTest.maxDiff = md;
		   XMLTest.diffCount+=dt;
		   XMLTest.cacheSize += xt.lineCache.size();
		   //XMLTest.al.add(Integer.valueOf((int) diff));
	   }		   

}
}
