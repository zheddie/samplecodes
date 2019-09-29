/* File SerializableTestClass.java created by Emily Benner on Tue Jun 17 1997. */

/* Copyright/Security Notice -----------------------------------------*/
/*                                                                    */
/*    IBM Confidential                                                */
/*                                                                    */
/*    OCO Source Materials                                            */ 
/*                                                                    */
/*    The Source code for this program is not published or otherwise  */
/*    divested of its trade secrets, irrespective of what has been    */
/*    deposited with the U.S. Copyright Office.                       */
/*                                                                    */
/*    5769-999 (C) Copyright IBM Corp. 1997, 1997                     */
/*                                                                    */
/* End Copyright -----------------------------------------------------*/


// Class------------------------------------------
// 
// Description: A helper class for testing io 
//              serialization exceptions. 
//
//------------------------------------------------

//package com.ibm.iTestJava.ibm97.exceptions.io;

import java.io.*;

public class SerializableTestClass implements Serializable 
{
    boolean yOrN;
    int [] intArray;
    int arraySize = 5;
    int arraySize1;

    SerializableTestClass ()
    {
	yOrN = false;
	intArray = new int [arraySize];
	for (int i = 0; i < arraySize; ++i)
	{
	    intArray[i] = i;
	}
    }
}

// Change Log ---------------------------------------------------
//                                                                    
//  Flag   Reason   Userid    Date      Description                   
//  ----  --------  --------  --------  -----------
//
// End Change Log -----------------------------------------------
