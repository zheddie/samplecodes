package com.ibm.sdwb.cps.reqfix.client;

import com.ibm.dv2common.syntax.ActionFlag;
import com.ibm.dv2common.syntax.BaseFlagError;
import com.ibm.dv2common.syntax.Flag;
import com.ibm.dv2common.syntax.JSyntax;

/**
 * File:  ReqFixSyntax.java
 * @author Aaron Hislop, ahislop@us.ibm.com
 * Date:  Feb 7, 2003
 * 
 * Change Activity:
 * 
 * PTR     	Date     		Pgmr ID  	Change Description
 * --------	-------- 		---------	----------------------------------
 * 			Feb 7, 2003		ahislop		Initial creation of the file
 */
public class ReqFixSyntax
{
	private static final String HELP_FLAG = "help";
	private static final String TYPE_FLAG = "type";
	private static final String PRODUCT_FLAG = "product";
	private static final String RELEASE_FLAG = "release";
	private static final String PROBLEM_ID_FLAG = "problemid";
	private static final String NLV_FLAG = "nlv";
	private static final String FIX_NAME_FLAG = "fixname";
	private static final String DEBUG_FLAG = "debug";
	private static final String NO_PROMPT_FLAG = "noprompt";
	private static final String FILE_FLAG = "file";
	private static final String X = "x";
	private static final String GUI_FLAG = "gui";
	private static final String PTR_FLAG = "ptr"; // same as problem id
	private static final String FEATURE_FLAG = "feature"; // same as nlv	
	private static final String PTF_FLAG = "ptf"; // same as fix..    
	private static final String FAMILY_FLAG = "family";
	private static final String NOTHING = "nothing";
	;

	private static final int ZERO_FLAG_INDEX = 0;
	private static final int TYPE_FLAG_INDEX = 1;
	private static final int PRODUCT_FLAG_INDEX = 2;
	private static final int RELEASE_FLAG_INDEX = 3;
	private static final int PROBLEM_ID_FLAG_INDEX = 4;
	private static final int NLV_FLAG_INDEX = 5;
	private static final int FIX_NAME_FLAG_INDEX = 6;
	private static final int DEBUG_FLAG_INDEX = 7;
	private static final int NO_PROMPT_FLAG_INDEX = 8;
	private static final int FILE_FLAG_INDEX = 9;
	private static final int GUI_FLAG_INDEX = 10;
	private static final int PTR_FLAG_INDEX = 11;
	private static final int FEATURE_FLAG_INDEX = 12;
	private static final int PTF_FLAG_INDEX = 13;
	private static final int FAMILY_FLAG_INDEX = 14;
	private static final int FINALFLAG_FLAG_INDEX = 15;
	private static final int hide = Flag.HIDEFLAG;

	private JSyntax syntax;

	private String requestType = null;
	private String product = null;
	private String release = null;
	private String problemId = null;
	private String nlv = null;
	private String fixName = null;
	private boolean debugOn = false;
	private int debugLevel = 3;
	private boolean noPrompt = false;
	private boolean runningInTheSameJvm = false;
	private String file = null;

	/**
	 * Constructor for ReqFixSyntax
	 */
	public ReqFixSyntax()
	{
	}

	/**
	 * Constructor for ReqFixSyntax
	 * @param args
	 * @param toolName
	 */
	public ReqFixSyntax(String[] args, String toolName)
	{
		setUpSyntax(args, toolName);
		setUpValues();
	}

	/**
	 * Method addFlagsToAction.
	 * @param af
	 * @param flArr
	 */
	public void addFlagsToAction(ActionFlag af, Flag[] flArr)
	{
		for (int i = 0; i < flArr.length; i++)
		{
			af.add(flArr[i]);
		}
	}

	/**
	 * Method printHelp.
	 * @throws BaseFlagError
	 */
	public void printHelp() throws BaseFlagError
	{
		syntax.printSyntax();
		System.exit(0);
	}

	/**
	 * Method setUpSyntax.
	 * @param args
	 */
	// The method that creates the flags and hooks up the syntax 
	public void setUpSyntax(String[] args, String toolName)
	{
		// First create JSyntax class
		syntax = new JSyntax(toolName);

		//Declare constants for the flagnames
		try
		{ // Flag creation can throw a BaseFlagError
			Flag optTypeFlag = new Flag(TYPE_FLAG, 4, 1, 1, "iseries, bld", TYPE_FLAG_INDEX, Flag.OPTFLAG, Flag.mx(FIX_NAME_FLAG_INDEX), Flag.co(ZERO_FLAG_INDEX));

			Flag optProductFlag = new Flag(PRODUCT_FLAG, 4, 1, 1, PRODUCT_FLAG, PRODUCT_FLAG_INDEX, Flag.OPTFLAG, Flag.mx(FIX_NAME_FLAG_INDEX), Flag.co(ZERO_FLAG_INDEX));

			Flag optReleaseFlag = new Flag(RELEASE_FLAG, 4, 1, 1, RELEASE_FLAG, RELEASE_FLAG_INDEX, Flag.OPTFLAG, Flag.mx(FIX_NAME_FLAG_INDEX), Flag.co(ZERO_FLAG_INDEX));

			Flag optProblemIdFlag = new Flag(PROBLEM_ID_FLAG, 4, 1, 1, "problem id", PROBLEM_ID_FLAG_INDEX, Flag.OPTFLAG, Flag.mx(FIX_NAME_FLAG_INDEX), Flag.co(ZERO_FLAG_INDEX));

			Flag optNlvFlag = new Flag(NLV_FLAG, 3, 1, 1, NLV_FLAG, NLV_FLAG_INDEX, Flag.OPTFLAG, Flag.mx(FIX_NAME_FLAG_INDEX), Flag.co(ZERO_FLAG_INDEX));

			Flag optFixNameFlag =
				new Flag(
					FIX_NAME_FLAG,
					3,
					1,
					1,
					FIX_NAME_FLAG,
					FIX_NAME_FLAG_INDEX,
					Flag.OPTFLAG,
					Flag.mx(TYPE_FLAG_INDEX).plus(PRODUCT_FLAG_INDEX).plus(RELEASE_FLAG_INDEX).plus(PROBLEM_ID_FLAG_INDEX).plus(NLV_FLAG_INDEX),
					Flag.co(ZERO_FLAG_INDEX));

			Flag optDebugFlag = new Flag(DEBUG_FLAG, 5, 1, 0, "0 - 5", DEBUG_FLAG_INDEX, Flag.OPTFLAG, Flag.mx(ZERO_FLAG_INDEX), Flag.co(ZERO_FLAG_INDEX));

			Flag optNoPromptFlag =
				new Flag(
					NO_PROMPT_FLAG,
					8,
					0,
					0,
					NO_PROMPT_FLAG,
					NO_PROMPT_FLAG_INDEX,
					Flag.OPTFLAG,
					Flag.mx(TYPE_FLAG_INDEX).plus(PRODUCT_FLAG_INDEX).plus(RELEASE_FLAG_INDEX).plus(PROBLEM_ID_FLAG_INDEX).plus(NLV_FLAG_INDEX).plus(FIX_NAME_FLAG_INDEX),
					Flag.co(FILE_FLAG_INDEX));

			Flag optFileFlag =
				new Flag(
					FILE_FLAG,
					4,
					1,
					1,
					"xmlFile",
					FILE_FLAG_INDEX,
					Flag.OPTFLAG,
					Flag.mx(TYPE_FLAG_INDEX).plus(PRODUCT_FLAG_INDEX).plus(RELEASE_FLAG_INDEX).plus(PROBLEM_ID_FLAG_INDEX).plus(NLV_FLAG_INDEX).plus(FIX_NAME_FLAG_INDEX),
					Flag.co(NO_PROMPT_FLAG_INDEX));

			Flag optGuiFlag = new Flag(GUI_FLAG, 3, 0, 0, X, GUI_FLAG_INDEX, Flag.OPTFLAG, Flag.mx(ZERO_FLAG_INDEX), Flag.co(ZERO_FLAG_INDEX), hide);

			Flag optPtrFlag = new Flag(PTR_FLAG, 3, 1, 1, "ptr", PTR_FLAG_INDEX, Flag.OPTFLAG, Flag.mx(PROBLEM_ID_FLAG_INDEX), Flag.co(ZERO_FLAG_INDEX), hide);

			Flag optFeatureCodeFlag = new Flag(FEATURE_FLAG, 4, 1, 1, FEATURE_FLAG, FEATURE_FLAG_INDEX, Flag.OPTFLAG, Flag.mx(NLV_FLAG_INDEX), Flag.co(ZERO_FLAG_INDEX), hide);

			Flag optPtfFlag = new Flag(PTF_FLAG, 3, 1, 1, X, PTF_FLAG_INDEX, Flag.OPTFLAG, Flag.mx(FIX_NAME_FLAG_INDEX), Flag.co(ZERO_FLAG_INDEX), hide);

			Flag optFamilyFlag = new Flag(FAMILY_FLAG, 6, 1, 1, X, FAMILY_FLAG_INDEX, Flag.OPTFLAG, Flag.mx(ZERO_FLAG_INDEX), Flag.co(ZERO_FLAG_INDEX), hide);

			//Action flags

			ActionFlag help = new ActionFlag(HELP_FLAG, 1, 0, 0, X);
			syntax.add(help);

			ActionFlag noAction = new ActionFlag(ActionFlag._NULLACT, 0, 0, 0, NOTHING);
			Flag[] noActionArray =
				{
					optTypeFlag,
					optProductFlag,
					optReleaseFlag,
					optProblemIdFlag,
					optNlvFlag,
					optFixNameFlag,
					optDebugFlag,
					optNoPromptFlag,
					optFileFlag,
					optGuiFlag,
					optFamilyFlag,
					optPtrFlag,
					optFeatureCodeFlag,
					optPtfFlag };
			addFlagsToAction(noAction, noActionArray);
			syntax.add(noAction);

			// parse the user input
			syntax.parseInput(args);

			// is the action help
			if (syntax.action().equalsIgnoreCase(HELP_FLAG))
				printHelp();
		} // end try

		catch (BaseFlagError e)
		{
			// Error already printed to screen, just exit
			System.exit(33);
		}
	} // end setUpSyntax method

	/**
	 * Method setUpValues.
	 */
	private void setUpValues()
	{
		try
		{
			setRunningInSameJvm(false);
			if (syntax.flagIsAssigned(TYPE_FLAG))
			{
				setRequestType(syntax.getFlagParms(TYPE_FLAG));
			}
			if (syntax.flagIsAssigned(PRODUCT_FLAG))
			{
				setProduct(syntax.getFlagParms(PRODUCT_FLAG));
			}
			if (syntax.flagIsAssigned(RELEASE_FLAG))
			{
				setRelease(syntax.getFlagParms(RELEASE_FLAG));
			}
			if (syntax.flagIsAssigned(PROBLEM_ID_FLAG))
			{
				setProblemId(syntax.getFlagParms(PROBLEM_ID_FLAG));
			}
			if (syntax.flagIsAssigned(PTR_FLAG))
			{
				setProblemId(syntax.getFlagParms(PTR_FLAG));
			}
			if (syntax.flagIsAssigned(NLV_FLAG))
			{
				setNlv(syntax.getFlagParms(NLV_FLAG));
			}
			if (syntax.flagIsAssigned(FEATURE_FLAG))
			{
				setNlv(syntax.getFlagParms(FEATURE_FLAG));
			}
			if (syntax.flagIsAssigned(FIX_NAME_FLAG))
			{
				setFixName(syntax.getFlagParms(FIX_NAME_FLAG));
			}
			if (syntax.flagIsAssigned(PTF_FLAG))
			{
				setFixName(syntax.getFlagParms(PTF_FLAG));
			}
			// Deal with the debug flag
			if (syntax.flagIsAssigned(DEBUG_FLAG))
			{
				setDebugOn(true);
				String levelStr = syntax.getFlagParms(DEBUG_FLAG);
				if (levelStr == null || levelStr.trim().equals(""))
				{
					// no level to set...  Leave the default.
				}
				else
				{
					try
					{
						int level = Integer.parseInt(levelStr.trim());
						setDebugLevel(level);
					}
					catch (NumberFormatException nfe)
					{
						// Eat this exception..  Leave the debug level the default level
					}
				}
			}
			if (syntax.flagIsAssigned(NO_PROMPT_FLAG))
			{
				setNoPrompt(true);
			}
			if (syntax.flagIsAssigned(FILE_FLAG))
			{
				setFile(syntax.getFlagParms(FILE_FLAG));
			}
		}
		catch (BaseFlagError BFE)
		{
			System.out.println(BFE);
		}
	}

	/**
	 * Method getDebugLevel
	 * @return
	 */
	public int getDebugLevel()
	{
		return debugLevel;
	}

	/**
	 * Method isDebugOn
	 * @return
	 */
	public boolean isDebugOn()
	{
		return debugOn;
	}

	/**
	 * Method getNlv
	 * @return String
	 */
	public String getNlv()
	{
		return this.nlv;
	}

	/**
	 * Method getFile
	 * @return
	 */
	public String getFile()
	{
		return file;
	}

	/**
	 * Method getFixName
	 * @return
	 */
	public String getFixName()
	{
		return fixName;
	}

	/**
	 * Method isNoPrompt
	 * @return
	 */
	public boolean isNoPrompt()
	{
		return noPrompt;
	}

	/**
	 * Method isRunningInTheSameJvm
	 * @return boolean
	 */
	public boolean isRunningInTheSameJvm()
	{
		return this.runningInTheSameJvm;
	}

	/**
	 * Method getProduct
	 * @return
	 */
	public String getProduct()
	{
		return product;
	}

	/**
	 * Method getProblemId
	 * @return String
	 */
	public String getProblemId()
	{
		return problemId;
	}

	/**
	 * Method getRelease
	 * @return
	 */
	public String getRelease()
	{
		return release;
	}

	/**
	 * Method getRequestType
	 * @return
	 */
	public String getRequestType()
	{
		return requestType;
	}

	/**
	 * Method setDebugLevel
	 * @param i
	 */
	public void setDebugLevel(int i)
	{
		debugLevel = i;
	}

	/**
	 * Method setDebugOn
	 * @param b
	 */
	public void setDebugOn(boolean b)
	{
		debugOn = b;
	}

	/**
	 * Method setNlv
	 * @param string 
	 */
	public void setNlv(String string)
	{
		this.nlv = string;
	}

	/**
	 * Method setFile
	 * @param string
	 */
	public void setFile(String string)
	{
		file = string;
	}

	/**
	 * Method setFixName
	 * @param string
	 */
	public void setFixName(String string)
	{
		fixName = string;
	}

	/**
	 * Method setNoPrompt
	 * @param b
	 */
	public void setNoPrompt(boolean b)
	{
		noPrompt = b;
	}

	/**
	 * Method setProduct
	 * @param string
	 */
	public void setProduct(String string)
	{
		product = string;
	}

	/**
	 * Method setProblemId
	 * @param string 
	 */
	public void setProblemId(String string)
	{
		this.problemId = string;
	}

	/**
	 * Method setRelease
	 * @param string
	 */
	public void setRelease(String string)
	{
		release = string;
	}

	/**
	 * Method setRequestType
	 * @param string
	 */
	public void setRequestType(String string)
	{
		requestType = string;
	}

	/**
	 * Method setRunningInSameJvm
	 * @param flag 
	 */
	public void setRunningInSameJvm(boolean flag)
	{
		this.runningInTheSameJvm = flag;
	}
	
}
