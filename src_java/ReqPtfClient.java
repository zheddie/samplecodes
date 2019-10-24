/*
 * ===========================================================================
 * IBM Confidential
 * OCO Source Materials
 *
 * (C) Copyright IBM Corp. 2005, 2006. 
 *
 * The source code for this program is not published or otherwise divested of 
 * its trade secrets, irrespective of what has been deposited with the U.S.
 * Copyright office.
 * 
 * ===========================================================================
 * Change History
 * 
 * Date         Change Flag    Shortname  Change Summary
 * ------------ -------------- ---------- --------------------------------------------
 * Sept 22,2006                 oscargh   Created.
 * May  30,2008					zambrano  Updated.
 */


package com.ibm.bst.fixcommon.client.webservice;

import com.ibm.bst.fixcommon.FixUser;
import com.ibm.bst.fixcommon.utility.FixProperties;
import com.ibm.bst.fixcommon.utility.IAuthorityConst;

/**
 * @author Oscar Gonzalez, oscargh@mx1.ibm.com
 */
 
public class ReqPtfClient {

	private static final String  REQ_PTF_SCHEMA_PROPERTY = "requestptf_schema.xsd";
	private static final String OPERATION_NAME = "requestPtf";
	private static final String TOOL_NAME = "reqPtf";
	
	public String reqPtf(String endPoint, String userId, String key, String file, String debugLevel) 
			throws Exception{
		
		String endPointConstant = "webservice.reqptf.endpoint";	
		String schemaLoc = null;
		
		
		if(null == System.getProperty("properties.url"))
		{
	        System.setProperty("properties.url", FixProperties.getPropertiesValue("properties.url"));       
	    }
		
		FixUser user = new FixUser();
		user.checkAuthorizationToAction(IAuthorityConst.REQUEST_PTF_ACTION);
		String owner=user.getPerson().getEmailAddress().toString();
		
		//Comment out this code if you need to validate your file against a schema
		/*XMLUtils xmlUtils = new XMLUtils();
		String baseURL = ApplicationProperties.getPropertyNoException("webserver.base.url");
       	if (baseURL != null && !baseURL.trim().equalsIgnoreCase("")) 
       	{
       		 schemaLoc = ApplicationProperties.getPropertyNoException(REQ_PTF_SCHEMA_PROPERTY);
       		if (schemaLoc != null && !schemaLoc.trim().equalsIgnoreCase("")) 
       		{
       			schemaLoc = baseURL + schemaLoc;
       		}
       	}
        xmlUtils.validateSchemaGivenXMLString(schemaLoc, file);*/
		
		CommandLineServices cmdLnServices = CommandLineServices.getInstance();
		String callResult =	cmdLnServices.callToFixDataWS(endPoint, endPointConstant,
				userId, key, debugLevel, file, OPERATION_NAME , owner, TOOL_NAME);
		return callResult;
	}
}
