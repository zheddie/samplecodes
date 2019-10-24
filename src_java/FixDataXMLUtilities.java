/*
 * @author Oscar Gonzalez, oscargh@mx1.ibm.com
 * 
 * 
 * =======================================================================
 * Change history
 * =======================================================================
 * 	Tag     Date      	Defect   Notes ID   Description
 * ------  ----------  --------  ---------  --------------------------------
 * 	N/A    	03-16-07  	 N/A     oscargh    New class created.
 * 
 *  
 */

package com.ibm.bst.fixcommon.utility;

import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.ibm.bldcommon.util.xml.XMLUtils;
import com.ibm.dv2common.SAXParsingUtil;
import com.ibm.dv2common.XMLElement;
import com.ibm.dv2common.XMLElementException;

public class FixDataXMLUtilities {
	
	
    public static String exportFixDataAsXML(String xmlDoc, String debugLevel,String sessionId, String toolUserId) throws Exception{
        return exportFixDataAsXML(xmlDoc, debugLevel, sessionId, toolUserId, null);
    }
    
	public static String exportFixDataAsXML(String xmlDoc, String debugLevel,String sessionId, String toolUserId, String logResults) throws Exception{
		 String lineSeparator = System.getProperty("line.separator");
		 StringBuffer finalXmlDoc= new StringBuffer("<?xml version=\"1.0\"?>");
		 finalXmlDoc.append(lineSeparator);
		 finalXmlDoc.append("<fixDataEntities>");
		 finalXmlDoc.append(lineSeparator);		 
		 if( (toolUserId!=null)&& (toolUserId.length()!=0)) {
			 finalXmlDoc.append("<toolUserId>");
			 finalXmlDoc.append(toolUserId);
			 finalXmlDoc.append("</toolUserId>");
			 finalXmlDoc.append(lineSeparator);
		 }
		 if( (debugLevel!=null)&& (debugLevel.length()!=0)) {
			 finalXmlDoc.append("<debugLevel>");
			 finalXmlDoc.append(debugLevel);
			 finalXmlDoc.append("</debugLevel>");
			 finalXmlDoc.append(lineSeparator);
		 }
		 if( (sessionId!=null)&& (sessionId.length()!=0)){
			 finalXmlDoc.append("<sessionId>");
			 finalXmlDoc.append(sessionId);
			 finalXmlDoc.append("</sessionId>");
			 finalXmlDoc.append(lineSeparator);
		 }
		 if (logResults!=null && logResults.length()>0) {
			 finalXmlDoc.append("<logResults>");
			 finalXmlDoc.append(logResults);
			 finalXmlDoc.append("</logResults>");
			 finalXmlDoc.append(lineSeparator);
		 }
		 if( (xmlDoc!=null)&& (xmlDoc.length()!=0)){
		 	//finalXmlDoc.append(XMLUtils.getXMLSnippetFromXMLDoc(xmlDoc));
		 	finalXmlDoc.append(removeXMLVersion(xmlDoc));
		 }
		 finalXmlDoc.append("</fixDataEntities>");
	     return finalXmlDoc.toString();
	}
	
	public static Vector retreiveFixDataAndDebugLevelFromXML(String xmlDoc) throws Exception{
		String xmlSnippet= null;
		String debugLevel = null;
		SAXParsingUtil saxParsingUtil = new SAXParsingUtil(true);
		XMLElement parent = saxParsingUtil.parseXMLString(xmlDoc);
		XMLElement[] childrenLevel1 = parent.getInnerElements();
		for (int i=0; i < childrenLevel1.length ; i++){
			XMLElement childLevel1 = childrenLevel1[i];
			if(childLevel1!=null && childLevel1.getName().equals("debugLevel")){
				debugLevel = childLevel1.getValue();
			}
			else if(childLevel1!=null && childLevel1.getName().equals("fixData")){
				xmlSnippet = childLevel1.toXMLString(true, true, 0);
			}
		}
		Vector debugLevelAndXmlDoc = new Vector();
		debugLevelAndXmlDoc.add(debugLevel);
		debugLevelAndXmlDoc.add(XMLUtils.getXMLDocFromXMLSnippet(xmlSnippet));
	    return debugLevelAndXmlDoc;
	}
	
	public static String retreiveFixDataFromXML(String xmlDoc) throws Exception{
		String xmlSnippet= null;
		SAXParsingUtil saxParsingUtil = new SAXParsingUtil(true);
		System.out.println("xmlDoc: "+xmlDoc);
		XMLElement parent = saxParsingUtil.parseXMLString(xmlDoc);
		XMLElement[] childrenLevel1 = parent.getInnerElements();
		for (int i=0; i < childrenLevel1.length ; i++){
			XMLElement childLevel1 = childrenLevel1[i];
			 if(childLevel1!=null && childLevel1.getName().equals("fixData")){
				xmlSnippet = childLevel1.toXMLString(true, true, 0);
			}
		}
 	    return XMLUtils.getXMLDocFromXMLSnippet(xmlSnippet);
	}
	
	public static String retreiveOwnerFromXML(String xmlDoc) throws Exception{
		String owner= null;
		SAXParsingUtil saxParsingUtil = new SAXParsingUtil(true);
		XMLElement parent = saxParsingUtil.parseXMLString(xmlDoc);
		XMLElement[] childrenLevel1 = parent.getInnerElements();
		for (int i=0; i < childrenLevel1.length ; i++){
			XMLElement childLevel1 = childrenLevel1[i];
			 if(childLevel1!=null && childLevel1.getName().equals("toolUserId")){
				owner = childLevel1.getValue();
			}
		}
		System.out.println("Inside retreiveOwnerFromXML, toolUserId is: " + owner);
 	    return owner;
	}
	
	private static String removeXMLVersion(String xml){
		int endIndex = xml.indexOf("?>");
		if (endIndex > -1){
			xml = xml.substring(endIndex+2);
			xml = xml.substring(xml.indexOf("<"));
		}
		return xml;
	}
	
	public static String getTagValueFromXML ( String xmlTag , String xml){		
		Pattern replacerPattern = null;
	    Matcher replacerMatcher = null;
	    String value = "";
	    replacerPattern = Pattern.compile( "\\< *" + xmlTag  + " *\\>.*\\s*\\<\\/ *" + xmlTag +" *\\>" , Pattern.DOTALL);
	    if(xml!=null){
	        replacerMatcher = replacerPattern.matcher( xml );
	        if (replacerMatcher.find()) {
	            String tag = xml.substring( replacerMatcher.start() , replacerMatcher.end());
	            int start = tag.indexOf(">") + 1 ;
				int end = tag.indexOf("</");
	           if (start != -1 && end != -1){
	           		value = tag.substring(start , end);
	           }
	        }
	    }
	    return value.trim();
	}
	
	public static String removeTagFromXML ( String xmlTag , String xml){		
		Pattern pattern = null;
	    Matcher matcher = null;
	    pattern = Pattern.compile( "(<"+xmlTag+">.*\\s*</"+xmlTag+">)" , Pattern.DOTALL);
	    if(xml != null){
	        matcher = pattern.matcher(xml);
	        if (matcher.find()) {
	            xml = (xml.replaceAll(matcher.group(1), "")).trim();
	        }
	    }
	    return xml;
	}
	
	public static String reformatXML(String xml){
		String invalidTags[] = {"sessionId", "state","owner","type","subType", "requestDate", "stateDate", "loadId","fixID",
	       		"trackState", "optionId", "minLevel", "actualMaxLevel", "requestedMaxLevel", "driver", "testFixFdlTimeStamp",
	       		"fixSize","owningProblemId","owningProblemName","retainWorkState","fixingApar","fixingFix","historyEntries",
	       		"fixCreationDate", "fixOID","loadWasDoneOnDataObject","displaySummaryQuestionsValues"};
	        
	    //remove invalid tags from xml
	    for(int i=0; i<invalidTags.length; i++){
	      	xml = removeTagFromXML(invalidTags[i],xml );
	    }
	    
	    //remove null values
	    xml = xml.replaceAll("null", "\n");
	    
		return xml;
	}
	
	public static XMLElement retrieveElementFromXML(String xmlFile, String element) throws XMLElementException
	{
		XMLElement elementFinded = null;
		SAXParsingUtil saxParsingUtil = new SAXParsingUtil(true);
		XMLElement parent = saxParsingUtil.parseXMLString(xmlFile);
		XMLElement[] childrenLevel1 = parent.getInnerElements();
		
		for(int i = 0; i < childrenLevel1.length; i++)
		{
			XMLElement childLevel1 = childrenLevel1[i];
			if(element.equalsIgnoreCase(childLevel1.getName()))
			{
				elementFinded = childLevel1;
			}
		}
		
		return elementFinded;
	}
	
	public static String getProblemAttributeFromXML(String xmlFile, String attribute)throws XMLElementException 
	{
	    SAXParsingUtil saxParsingUtil = new SAXParsingUtil(true);
        XMLElement parent = saxParsingUtil.parseXMLString(xmlFile);
        String field = null; 
        boolean exit = false;
        
        XMLElement[] childrenLevel1 = parent.getInnerElements();
		for (int j=0; j < childrenLevel1.length ; j++)
		{
		    XMLElement childLevel1 = childrenLevel1[j];
		    if("fixpack".equalsIgnoreCase(childLevel1.getName()))
			{
		        XMLElement[] childrenLevel2 = childLevel1.getInnerElements();
    		    
		        for(int k = 0; k < childrenLevel2.length; k++)
		        {
		            XMLElement childLevel2 = childrenLevel2[k];
    		        if("fixpack_component".equalsIgnoreCase(childLevel2.getName()))
        			{  
        			    
        		        XMLElement[] childrenLevel3 = childLevel2.getInnerElements();
        		        for(int l = 0; l < childrenLevel2.length; l++)
        		        {
        		            XMLElement childLevel3 = childrenLevel3[l];
        		            if("problem".equalsIgnoreCase(childLevel3.getName()))
                			{ 
                		        String[] attributes = childLevel3.getAttributeNames();
                			    
                			    for(int i = 0; i < attributes.length; i++)
                			    {
                			        if("product".equalsIgnoreCase(attributes[i]))
                			        {
                			            if( "product".equals(attribute))
                			            {
                			                field = childLevel3.getAttributeValue("product");
                			                exit = true;
                			                break;
                			            }
                			        }
                			        else if("release".equalsIgnoreCase(attributes[i]))
                			        {
                			            if( "release".equals(attribute))
                			            {
                			                field = childLevel3.getAttributeValue("release");
                			                exit = true;
                			                break;
                			            }
                			        }
                			        else if("id".equalsIgnoreCase(attributes[i]))
                			        {
                			            if( "id".equals(attribute))
                			            {
                			                field = childLevel3.getAttributeValue("id");
                			                exit = true;
                			                break;
                			            }
                			        }
                			        else if("feature".equalsIgnoreCase(attributes[i]))
                			        {
                			            if( "feature".equals(attribute))
                			            {
                			                field = childLevel3.getAttributeValue("feature");
                			                exit = true;
                			                break;
                			            }
                			        }
                			        else if("firmware_release".equalsIgnoreCase(attributes[i]))
                			        {
                			            if( "firmware_release".equals(attribute))
                			            {
                			                field = childLevel3.getAttributeValue("firmware_release");
                			                exit = true;
                			                break;
                			            }
                			        }
                			    }
                			}
        		            if(true == exit)
        		            {
        		                break;
        		            }
        		        }
        			}
    		        if(true == exit)
		            {
		                break;
		            }
		        }   
			}
		    if(true == exit)
            {
                break;
            }
		} 
		return field;
	}
	
	/**
	 * 
	 * @param xmlFile - The string with the xml data
	 * @param attribute - The attribute we are looking for in the xmlFile
	 * @param component - The element in which is located our attribute 
	 * 					  i.e. fixpack, fixpack_component, problem.
	 * @return
	 * @throws XMLElementException
	 */
	public static String getFixpackAttributeFromXML(String xmlFile, String attribute, 
													String component)throws XMLElementException 
	{
		 SAXParsingUtil saxParsingUtil = new SAXParsingUtil(true);
	     XMLElement parent = saxParsingUtil.parseXMLString(xmlFile);
	     String fixPackAttribute = null;
	     if ("fixData".equalsIgnoreCase(parent.getName()))
	     {
	         XMLElement[] childrenLevel = parent.getInnerElements();
	
	         if ("fixpack".equalsIgnoreCase(childrenLevel[0].getName()))
	         {
	         	fixPackAttribute =  getFixpackAttribute(childrenLevel[0], attribute, component);
	         }
	     }
	     else
	     {
	     	if ("fixpack".equalsIgnoreCase(parent.getName()))
	         {
	     		fixPackAttribute = getFixpackAttribute(parent, attribute, component);
	         }
	     }
	     return fixPackAttribute;
	     
	}
	/**
	 * 
	 * @param xmlFile
	 * @param attribute
	 * @return
	 * @throws XMLElementException
	 */
	public static String getFixpackAttribute(XMLElement parent, String attribute, 
													String component)throws XMLElementException 
	{

	    String fixPackAttribute = null;
		if(component.equalsIgnoreCase(parent.getName()))
		{
			String[] attributes = parent.getAttributeNames();
			
			for(int j = 0; j < attributes.length; j++)
		    {
				if(attribute.equalsIgnoreCase(attributes[j]))
				{
					fixPackAttribute = parent.getAttributeValue(attribute);
				}
		    }
		}
	    else if(parent.getInnerElements() != null)
	    {
	    	XMLElement[] elements = parent.getInnerElements(); 
	    	for(int i = 0; i < elements.length; i++)
	    	{
	    		if(component.equalsIgnoreCase(elements[i].getName()))
	    		{
	    			XMLElement element = elements[i];
	    			
	    			String[] attributes = element.getAttributeNames();
	    			
	    			for(int j = 0; j < attributes.length; j++)
	    		    {
	    				if(attribute.equalsIgnoreCase(attributes[j]))
	    				{
	    					fixPackAttribute = element.getAttributeValue(attribute);
	    				}
	    		    }
	    		}
	    		else if("problem".equalsIgnoreCase(component) && 
	    				"fixpack_component".equalsIgnoreCase(elements[i].getName()))
	    		{
	    			XMLElement[] fixpackComponentElements = elements[i].getInnerElements();
	    			for(int k = 0; k < fixpackComponentElements.length; k++)
	    	    	{
	    				if(component.equalsIgnoreCase(fixpackComponentElements[k].getName()))
	    	    		{
	    	    			XMLElement fixpackComponentElement = fixpackComponentElements[k];
	    	    			
	    	    			String[] attributes = fixpackComponentElement.getAttributeNames();
	    	    			
	    	    			for(int j = 0; j < attributes.length; j++)
	    	    		    {
	    	    				if(attribute.equalsIgnoreCase(attributes[j]))
	    	    				{
	    	    					fixPackAttribute = fixpackComponentElement.getAttributeValue(attribute);
	    	    				}
	    	    		    }
	    	    		}
	    	    	}
	    		}
	    	}
	    }
	    
	    return fixPackAttribute;
	}
	
	public static XMLElement getFixpackComponent(String xml, String name) throws XMLElementException
	{
		SAXParsingUtil saxParsingUtil = new SAXParsingUtil(true);
	    XMLElement parent = saxParsingUtil.parseXMLString(xml);
		XMLElement componentXML = null;
		XMLElement[] childrenLevel1 = null;
		
		if ("fixData".equalsIgnoreCase(parent.getName()))
        {
			childrenLevel1 = parent.getInnerElements();
        }
		else
		{
			childrenLevel1 = new XMLElement[] {parent}; 
		}	
		
		for (int i=0; i < childrenLevel1.length ; i++)
		{
			if("fixpack".equalsIgnoreCase(childrenLevel1[i].getName()))
			{
				XMLElement[] childrenLevel2 = childrenLevel1[i].getInnerElements();
				
				for(int j = 0; j < childrenLevel2.length; j++)
				{
					if("fixpack_component".equalsIgnoreCase(childrenLevel2[j].getName()))
					{
						String[] attributes = childrenLevel2[j].getAttributeNames();
						
						for(int k = 0; k < attributes.length; k++)
					    {
							if("name".equalsIgnoreCase(attributes[k]))
							{
								if(name.equalsIgnoreCase(childrenLevel2[j].getAttributeValue("name")))
								{
									componentXML = childrenLevel2[j];
								}
							}
					    }
					}
				}
			}
		}
		
		return componentXML;
	}
	
	/*private static String getFixpackAttribute(XMLElement element, String attribute) throws XMLElementException 
	{
		String field = null; 
        
		boolean exit = false;

	        String[] attributes = element.getAttributeNames();
		    
		    for(int i = 0; i < attributes.length; i++)
		    {
		        if("product".equalsIgnoreCase(attributes[i]))
		        {
		            if( "product".equals(attribute))
		            {
		                field = element.getAttributeValue("product");
		                exit = true;
		                break;
		            }
		        }
		        else if("release".equalsIgnoreCase(attributes[i]))
		        {
		            if( "release".equals(attribute))
		            {
		                field = element.getAttributeValue("release");
		                exit = true;
		                break;
		            }
		        }
		        else if("name".equalsIgnoreCase(attributes[i]))
		        {
		            if( "name".equals(attribute))
		            {
		                field = element.getAttributeValue("name");
		                exit = true;
		                break;
		            }
		        }
		        if(true == exit)
	            {
	                break;
	            }
		    }
	
		return field;
	}*/

	/*public static String getFixpackAttribute(XMLElement xmlElement, String attribute)throws XMLElementException 
	{
	    SAXParsingUtil saxParsingUtil = new SAXParsingUtil(true);
        XMLElement parent = saxParsingUtil.parseXMLString(xmlFile);
        String field = null; 
        boolean exit = false;
        
        XMLElement[] childrenLevel1 = parent.getInnerElements();
        
        for (int j=0; j < childrenLevel1.length ; j++)
		{
		    XMLElement childLevel1 = childrenLevel1[j];
		    if("fixpack".equalsIgnoreCase(childLevel1.getName()))
			{
		        String[] attributes = childLevel1.getAttributeNames();
			    
			    for(int i = 0; i < attributes.length; i++)
			    {
			        if("product".equalsIgnoreCase(attributes[i]))
			        {
			            if( "product".equals(attribute))
			            {
			                field = childLevel1.getAttributeValue("product");
			                exit = true;
			                break;
			            }
			        }
			        else if("release".equalsIgnoreCase(attributes[i]))
			        {
			            if( "release".equals(attribute))
			            {
			                field = childLevel1.getAttributeValue("release");
			                exit = true;
			                break;
			            }
			        }
			        else if("name".equalsIgnoreCase(attributes[i]))
			        {
			            if( "name".equals(attribute))
			            {
			                field = childLevel1.getAttributeValue("name");
			                exit = true;
			                break;
			            }
			        }
			        if(true == exit)
		            {
		                break;
		            }
			    }
			}
		    if(true == exit)
            {
                break;
            }
		}
        return field;
	}*/
	public static void main(String args[]) throws Exception
	{
	    
	    XMLUtils xmlUtils = new XMLUtils();
	    
	    String xml = xmlUtils.convertFromFileToXMLString("C:\\Projects\\phyp\\ApproveXMLLid.xml");
	    
	    System.out.println("Categories: "+FixDataXMLUtilities.retrieveElementFromXML(xml, "hiperQuestions").toXMLString(false, false, 0));
	    /*System.out.println("FProduct: "+FixDataXMLUtilities.getFixpackAttributeFromXML(xml, "product", "fixpack"));
	    System.out.println("FRelease: "+FixDataXMLUtilities.getFixpackAttributeFromXML(xml, "release", "fixpack"));
	    System.out.println("FName: "+FixDataXMLUtilities.getFixpackAttributeFromXML(xml, "name", "fixpack"));
	    
	    System.out.println("CName: "+FixDataXMLUtilities.getFixpackAttributeFromXML(xml, "name", "fixpack_component"));
	   
	    System.out.println("CProduct: "+FixDataXMLUtilities.getFixpackAttributeFromXML(xml, "product", "problem"));
	    System.out.println("CRelease: "+FixDataXMLUtilities.getFixpackAttributeFromXML(xml, "release", "problem"));
	    System.out.println("CFeature: "+FixDataXMLUtilities.getFixpackAttributeFromXML(xml, "feature", "problem"));
	        
	    XMLElement marker = FixDataXMLUtilities.getFixpackComponent(xml, "marker");
	    System.out.println("Marker: "+marker);
	    System.out.println("CName: "+FixDataXMLUtilities.getFixpackAttribute(marker, "name", "fixpack_component"));*/
	    
	}
}
