/*
 * Created on Aug 18, 2008
 *
 * TODO To change the template for this generated file go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
package com.ibm.sdwb.cps.cmdline.syntax;

/**
 * @author zambrano
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */

import java.util.Hashtable;

import com.ibm.bldcommon.util.StringUtilities;
import com.ibm.bldcommon.util.xml.XMLUtils;
import com.ibm.bst.fixcommon.exceptions.FixException;
import com.ibm.bst.fixcommon.utility.FixDataXMLUtilities;
import com.ibm.bst.fixcommon.client.webservice.ReqPtfClient;
import com.ibm.sdwb.cps.reqfix.ReqFixInfo;
import com.ibm.sdwb.cps.reqfix.client.ReqFixCommandLineDriver;
import com.ibm.sdwb.cps.reqfix.client.ReqFixSyntax;
import com.ibm.sdwb.cps.reqfix.gui.IReqFixGui;
import com.ibm.sdwb.cps.reqfix.gui.ReqFixGui;
import com.ibm.sdwb.cpscommon.ProcessFlowValidation;
import com.ibm.sdwb.ide.cpswb.CPSWBLaunchable;


public class ReqFix implements IReqFixGui, CPSWBLaunchable
{
	private static final String DEBUG_FLAG_NAME = "debug";
	private ReqFixSyntax reqFixSyntax = null;
	/**
	 * Constructor for ReqFix.
	 */
	public ReqFix(String[] args)
	{
		reqFixSyntax = new ReqFixSyntax(args, REQFIX_TOOLNAME);
		
		if (reqFixSyntax.isNoPrompt())
		{
			int rc = initCommandLine(reqFixSyntax);
			System.exit(rc);
		}
		else
		{
			initGui(reqFixSyntax);
		}
	}

	/**
	 * Constructor for ReqFix
	 */
	public ReqFix()
	{
	}

	/**
	 * Method requestFix
	 * @param xmlFileLocation
	 * @param debugOn
	 * @param debugLevel
	 */
	public void requestFix(String xmlFileLocation, boolean debugOn, int debugLevel) throws FixException
	{
		initCommandLine(xmlFileLocation, debugOn, debugLevel);
	}

	/**
	 * Method getReqFixInfo
	 * @param syntax
	 * @return ReqFixInfo
	 */
	private ReqFixInfo getReqFixInfo(ReqFixSyntax syntax)
	{
		return getReqFixInfo(syntax.isDebugOn(), syntax.getDebugLevel());
	}

	/**
	 * Method getReqFixInfo
	 * @param debugOn
	 * @param debugLevel
	 * @return ReqFixInfo
	 */
	private ReqFixInfo getReqFixInfo(boolean debugOn, int debugLevel)
	{
		ReqFixInfo reqFixInfo = new ReqFixInfo();
		// If we have the syntax we are not being called within the same JVM
		reqFixInfo.setRunningInSameJVM(false);
		reqFixInfo.setDebug(debugOn);
		reqFixInfo.setDebugLevel(debugLevel);
		return reqFixInfo;
	}

	/**
	 * Method initCommandLine
	 * @param syntax
	 * @return int
	 */
	private int initCommandLine(ReqFixSyntax syntax)
	{
		ReqFixInfo info = getReqFixInfo(syntax);
		
		ProcessFlowValidation pfv = null;
		
		String xmlFile = null;
		
		String fixName = null;
		
		String product = null;
		
		String release = null;
		try
		{
			xmlFile = new XMLUtils().convertFromFileToXMLString(syntax.getFile());
			
			fixName = FixDataXMLUtilities.getTagValueFromXML("fixName", xmlFile);
			product = FixDataXMLUtilities.getTagValueFromXML("product", xmlFile);
       		release = FixDataXMLUtilities.getTagValueFromXML("release", xmlFile);
			
       		if(null != syntax.getFixName())
		    {	
				pfv = new ProcessFlowValidation(syntax.getFixName());
		    }
			else if (null != syntax.getProduct() && null != syntax.getRelease())
			{
				pfv = new ProcessFlowValidation(syntax.getProduct(), syntax.getRelease());
			}
			else if(!StringUtilities.isEmpty(fixName))
			{
				pfv = new ProcessFlowValidation(fixName);
			}
			else if(!StringUtilities.isEmpty(product) && !StringUtilities.isEmpty(release))
			{
				pfv = new ProcessFlowValidation(product, release);
			}
		  
			if(null != pfv && !pfv.isToolEnabled(true))
			{
				System.out.println("(INFO) Invoking Web Services for the request. . .");
				ReqPtfClient webserviceClient = new ReqPtfClient();
				
				System.out.println(webserviceClient.reqPtf(null,null, "W3", xmlFile,
									Integer.toString(syntax.getDebugLevel())));
				
				System.exit(0);
			}
		}
		catch(Exception e)
		{
			System.out.println("The following exception occurred when ensuring the tool is enabled. " + e.toString());
      		System.exit(0);
		}
		
		try
		{
			ReqFixCommandLineDriver commandLineDriver = new ReqFixCommandLineDriver(info, syntax);
			commandLineDriver.requestFix();
			return 0;
		}
		catch (FixException rfe)
		{
			String error = "An error occurred while attempting to request the fix.  The error was:  " + rfe.getMessage();
			info.getLogger().severe(error);
			//rfe.printStackTrace();
			return 10;
		}
		catch (Exception e)
		{
			String error = "An unexpected error occurred while attempting to request the fix.  The error was:  " + e.toString();
			info.getLogger().severe(error);
			//e.printStackTrace();
			return 20;
		}
	}

	/**
	 * Method initCommandLine
	 * @param xmlFileLocation
	 * @param debugOn
	 * @param debugLevel 
	 */
	private void initCommandLine(String xmlFileLocation, boolean debugOn, int debugLevel) throws FixException
	{
		ReqFixInfo info = getReqFixInfo(debugOn, debugLevel);
		try
		{
			ReqFixCommandLineDriver commandLineDriver = new ReqFixCommandLineDriver(info, xmlFileLocation);
			commandLineDriver.requestFix();
		}
		catch (FixException rfe)
		{
			String error = "An error occurred while attempting to request the fix.  The error was:  " + rfe.getMessage();
			info.getLogger().severe(error);
			throw new FixException(error);
		}
		catch (Exception e)
		{
			String error = "An unexpected error occurred while attempting to request the fix.  The error was:  " + e.toString();
			info.getLogger().severe(error);
			throw new FixException(error);
		}
	}

	public void initGui(ReqFixSyntax syntax)
	{
		ReqFixGui gui = new ReqFixGui(getReqFixInfo(syntax), syntax);
	}

	public static void main(String[] args)
	{
		ReqFix reqFix = new ReqFix(args);
	}

	public void showFrame(Object[] arg)
	{
		final Hashtable attrHt1 = (Hashtable) arg[0];
		final String frameID = (String) arg[1];

		ThreadGroup myTG = new ThreadGroup(REQFIX_TITLE);

		Runnable myR = new Runnable()
		{
			public void run()
			{
				initGui(getSyntax(attrHt1));
			}

			private ReqFixSyntax getSyntax(Hashtable attrHt)
			{

				ReqFixSyntax syntax = new ReqFixSyntax();
				syntax.setRunningInSameJvm(true);
				if (attrHt.containsKey(CPSWBLaunchable.HK_PTF))
				{
					syntax.setFixName((String) attrHt.get(CPSWBLaunchable.HK_PTF));
				}
				if (attrHt.containsKey(CPSWBLaunchable.HK_PROD))
				{
					syntax.setProduct((String) attrHt.get(CPSWBLaunchable.HK_PROD));
				}
				if (attrHt.containsKey(CPSWBLaunchable.HK_RELE))
				{
					syntax.setRelease((String) attrHt.get(CPSWBLaunchable.HK_RELE));
				}
				if (attrHt.containsKey(CPSWBLaunchable.HK_PTR))
				{
					syntax.setProblemId((String) attrHt.get(CPSWBLaunchable.HK_PTR));
				}
				if (attrHt.containsKey(CPSWBLaunchable.HK_FEAT))
				{
					syntax.setNlv((String) attrHt.get(CPSWBLaunchable.HK_FEAT));
				}
				return syntax;
			}
		};
		Thread myThread = new Thread(myTG, myR, REQFIX_TITLE + "T");
		myThread.start();
	}
}

