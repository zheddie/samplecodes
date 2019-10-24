/**
 * File:  ReqFixCommandLineDriver.java
 * @author Aaron Hislop, ahislop@us.ibm.com
 * Date:  Jun 18, 2003
 */
package com.ibm.sdwb.cps.reqfix.client;

import java.util.Vector;
import java.util.logging.Handler;
import java.util.logging.Level;

import com.ibm.bst.fixcommon.exceptions.FixException;
import com.ibm.bst.problem_system.deployment.fix2web.ProblemSystemFactory;
import com.ibm.bst.problem_system.exceptions.ProblemSystemException;
import com.ibm.bst.problem_system.exceptions.ReleaseRecordNotFoundException;
import com.ibm.bst.problem_system.interfaces.problem.ProblemKey;
import com.ibm.bst.problem_system.interfaces.release_record.ReleaseRecordKey;
import com.ibm.bst.problem_system.interfaces.release_record.ReleaseRecordTO;
import com.ibm.dv2Db.ptfDb.ifObjects.Ptf;
import com.ibm.dv2Db.ptfDb.ifObjects.PtfAssociation;
import com.ibm.dv2Db.ptfDb.ifObjects.PtfEventHandler;
import com.ibm.dv2Db.ptfDb.ifObjects.PtfExitProgram;
import com.ibm.dv2Db.ptfDb.ifObjects.PtfInstructions;
import com.ibm.dv2Db.ptfDb.ifObjects.PtrTrack;
import com.ibm.dv2common.ConsoleLogHandler;
import com.ibm.dv2common.CurrentUser;
import com.ibm.dv2common.PtrName;
import com.ibm.dv2common.userinfo.PersonNotFoundException;
import com.ibm.sdwb.cps.PtfFile;
import com.ibm.sdwb.cps.PtfXmlConstants;
import com.ibm.sdwb.cps.fixpack.FixPackConstants;
import com.ibm.sdwb.cps.reqfix.AddTrackInfo;
import com.ibm.sdwb.cps.reqfix.IReqFixUpdate;
import com.ibm.sdwb.cps.reqfix.InvalidTrack;
import com.ibm.sdwb.cps.reqfix.MetadataUtilities;
import com.ibm.sdwb.cps.reqfix.PTFConstants;
import com.ibm.sdwb.cps.reqfix.ReqFixInfo;
import com.ibm.sdwb.cps.reqfix.ReqFixWarning;
import com.ibm.sdwb.cps.reqfix.RequisiteUtilities;
import com.ibm.sdwb.cps.reqfix.TrackUtilities;
import com.ibm.sdwb.cps.reqfix.gui.IReqFixGui;

/* 
 * Change Activity:
 * 
 * PTR     		Date     		Pgmr ID  	Change Description
 * ------------	------------ 	---------	----------------------------------
 * PDV06324		Jun 18, 2003	ahislop		Initial creation of the file
 * STGC00448752	Apr 16, 2008	orlando		Update createWorkItemIfNecessary() method to use Problem System.
 */
public class ReqFixCommandLineDriver implements IReqFixGui, IReqFixWorker, PTFConstants, WarningIfc, PtfXmlConstants, IReqFixUpdate
{
	private ReqFixInfo reqFixInfo = null;
	private PtfFile ptfFile = null;
	private ReqFixWorker reqFixWorker = null;

	/**
	 * Constructor for ReqFixCommandLineDriver
	 * @param info
	 * @param syntax
	 * @throws FixException
	 */
	public ReqFixCommandLineDriver(ReqFixInfo info, ReqFixSyntax syntax) throws FixException
	{
		this.reqFixInfo = info;
		setLogger();
		this.reqFixInfo.setReqFixUpdate(this);
		this.ptfFile = getPtfFile(syntax);
		this.reqFixWorker = new ReqFixWorker(this.reqFixInfo.getFixUser());
	}

	/**
	 * Constructor for ReqFixCommandLineDriver
	 * @param info
	 * @param xmlFileLocation
	 * @throws FixException
	 */
	public ReqFixCommandLineDriver(ReqFixInfo info, String xmlFileLocation) throws FixException
	{
		this.reqFixInfo = info;
		setLogger();
		this.reqFixInfo.setReqFixUpdate(this);
		this.ptfFile = getPtfFile(xmlFileLocation);
		this.reqFixWorker = new ReqFixWorker(this.reqFixInfo.getFixUser());
	}

	/**
	 * Method setLogger
	 */
	private void setLogger()
	{
		Handler[] handlerList = this.reqFixInfo.getLogger().getHandlers();
		if (handlerList != null)
		{
			for (int i = 0; i < handlerList.length; i++)
			{
				Handler handler = handlerList[i];
				this.reqFixInfo.getLogger().removeHandler(handler);
			}
		}
		ConsoleLogHandler consoleHandler = new ConsoleLogHandler();
		consoleHandler.setLevel(getLevel());
		this.reqFixInfo.getLogger().addHandler(consoleHandler);
	}

	private Level getLevel()
	{
		Level level = Level.INFO;
		if (this.reqFixInfo.isDebugOn())
		{
			int debugLevel = this.reqFixInfo.getDebugLevel();
			if (debugLevel < 3)
			{
				level = Level.FINE;
			}
			if (debugLevel < 5)
			{
				level = Level.FINER;
			}
			else
			{
				level = Level.FINEST;
			}
		}
		return level;
	}

	/**
	 * Method getPtfFile
	 * @param syntax
	 * @return PtfFile
	 * @throws FixException 
	 */
	private PtfFile getPtfFile(ReqFixSyntax syntax) throws FixException
	{
		reqFixInfo.getLogger().info("Running command line version.");
		try
		{
			reqFixInfo.getLogger().info("Construction PtfFile object.");
			PtfFile ptfFile = new PtfFile();
			String file = syntax.getFile();
			reqFixInfo.getLogger().info("Parsing file '" + file + "'.");
			ptfFile.parseFile(file, "reqptf");
			return ptfFile;
		}
		catch (Exception e)
		{
			String error = "An error occurred while attempting to construct the PTF file.  The error was:  " + e.toString();
			throw new FixException(error);
		}
	}

	private PtfFile getPtfFile(String xmlFileLocation) throws FixException
	{
		reqFixInfo.getLogger().info("Running command line version.");
		try
		{
			reqFixInfo.getLogger().info("Construction PtfFile object.");
			PtfFile ptfFile = new PtfFile();
			reqFixInfo.getLogger().info("Parsing file '" + xmlFileLocation + "'.");
			ptfFile.parseFile(xmlFileLocation, "reqptf");
			return ptfFile;
		}
		catch (Exception e)
		{
			String error = "An error occurred while attempting to construct the PTF file.  The error was:  " + e.toString();
			throw new FixException(error);
		}
	}

	/**
	 * Method requestFix
	 * @throws FixException 
	 */
	public void requestFix() throws FixException, ReqFixWarning
	{
		try
		{
			reqFixInfo.getLogger().finer("requestFix()");
			initializeInfoObject();
			reqFixInfo.getLogger().info("Adding tracks");
			addTracksToRequest();
			try
			{
				reqFixInfo.getLogger().info("Setting the owning PTF");
				setOwningPtf();
				reqFixInfo.getLogger().info("Validating compatibility of tracks");
				processTrackInfomation();
				reqFixInfo.getLogger().info("Submitting Request");
				requestAction();
			}
			catch (FixException rfe)
			{
				this.reqFixWorker.unlockRows(this.reqFixInfo);
				throw rfe;
			}
			catch (ReqFixWarning rfw)
			{
				this.reqFixWorker.unlockRows(this.reqFixInfo);
				throw rfw;
			}
		}
		finally
		{
			this.reqFixWorker.closeDatabaseConnections(this.reqFixInfo);
		}
	}

	/**
	 * Method initializeInfoObject
	 * @throws FixException 
	 */
	private void initializeInfoObject() throws FixException
	{
		reqFixInfo.getLogger().finer("initializeInfoObject()");
		this.reqFixInfo.clearRequestFields();
		try
		{
			this.reqFixInfo.setRequestorUID(CurrentUser.getDbPersonId());
		}
		catch (PersonNotFoundException pnfe)
		{
			String error = "Unable to get the UID of the person making this request.  The error was:  " + pnfe.toString();
			throw new FixException(error);
		}
		this.reqFixInfo.setPtfAssociationsOnRequest(new Vector());
	}

	/**
	 * Method addTracksToRequest
	 * @throws FixException 
	 */
	private void addTracksToRequest() throws FixException
	{
		this.reqFixInfo.getLogger().finer("addTracksToRequest()");
		Vector tracksForRequest = this.ptfFile.getPtrTracks();
		if (tracksForRequest == null || tracksForRequest.size() < 1)
		{
			String error = "No tracks were listed in the PTF File for the request.  Please check your xml file.";
			throw new FixException(error);
		}

		for (int i = 0; i < tracksForRequest.size(); i++)
		{
			PtrTrack ptrTrack = (PtrTrack) tracksForRequest.elementAt(i);
			if (ptrTrack != null)
			{
                // The work item may not be created if we're working in GFW trackless releases.
			    createWorkItemIfNecessary(ptrTrack.getPtrProduct(), ptrTrack.getPtrName(), ptrTrack.getRelease());
			    
				PtfAssociation ptfAssociation = new PtfAssociation(ptrTrack.getPtrProduct(), ptrTrack.getPtrName(), ptrTrack.getRelease(), ptrTrack.getFeatureCode());
				
				this.reqFixInfo.setRequestedFeature(ptfAssociation.getFeatureCode());
				this.reqFixInfo.setProcessType(this.reqFixWorker.getProcessType(this.reqFixInfo, ptfAssociation, this.ptfFile.getHighLevelProcess()));

				AddTrackInfo addTrackInfo = this.reqFixWorker.getAddTrackInfo(this.reqFixInfo, ptfAssociation);
				Vector invalidTracks = addTrackInfo.getTracksThatAreNotOKToAdd();
				if (invalidTracks != null)
				{
					for (int j = 0; j < invalidTracks.size(); j++)
					{
						InvalidTrack invalidTrack = (InvalidTrack) invalidTracks.elementAt(j);
						if (!invalidTrack.getFailFromCommandLine())
						{
							continue;
						}
						String error =
							"Unable to add Track " + invalidTrack.getPtfAssociationThatCouldNotBeProcessed() + ".  The error was: " + invalidTrack.getReasonPtfAssociationCouldNotBeProcessed();
						throw new FixException(error);
					}
				}

				Vector tracksThatHaveWarning = addTrackInfo.getTracksThatNeedWarningDisplayed();
				this.reqFixInfo.getLogger().warning("Tracks that had a warning = " + tracksThatHaveWarning);
			} //end if
		} //end for
		//begin code added by MHD; DV07801
		//At this point, the info object has been updated to contain all the tracks on the request...both the tracks the user explicitly
		//asked for, and the tracks that were pulled in under the covers

		//Remove any tracks that got added to the request but that were not actually specified by the user...do this because the xml is the 'bible' for this request.
		//When this method completes, the list of 'tracksCurrentlyOnRequest' in the info object will have been updated by the backend to include ONLY the tracks that
		//were explicitly asked for the by the user, and NOT any tracks that got pulled in under the covers.  Any tracks that were not asked for
		//and could not be removed will result in an error.
		TrackUtilities trackUtil = new TrackUtilities();
		Vector tracksUserAskedForAsPtfAssociations = trackUtil.convertPtrTracksToPtfAssociations(tracksForRequest, this.reqFixInfo.getPTFDatabaseConnection());
		this.reqFixWorker.removeUnspecifiedTracksFromRequest(this.reqFixInfo, tracksUserAskedForAsPtfAssociations);
		this.reqFixInfo.getLogger().fine("Tracks on request after add completed: **" + this.reqFixInfo.getPtfAssociationsOnRequest() + "**");
		//end code added by MHD; DV07801
	} //end addTracksToRequest()
	
    /**
     * This method is used to create a work item for the ptr if that work item doesn't already exist.
     * A work item can be thought of as a release record.
     * 
     * @param product - The product of the ptr
     * @param ptrName - The PTR name
     * @param release - The release of the ptr
     * 
     * @throws Exception
     */
    private void createWorkItemIfNecessary(String product, PtrName ptrName, String release) throws FixException
    {
        try
        {
            MetadataUtilities metadataUtil = new MetadataUtilities();
            String processGroup = metadataUtil.getRequestProcessGroup(product, release);

            // only create the work item if we're allowed to
            if (isValidForWorkItemCreation(processGroup))
            {
    			ProblemSystemFactory psFactory = ProblemSystemFactory.getFactory( product, release );

    			ProblemKey key = new ProblemKey();
    			key.setProduct( product );
    			key.setName( ptrName.getAsInternal() );

    			ReleaseRecordKey rrkey = new ReleaseRecordKey();
    		    rrkey.setProblemKey(key);
    		    rrkey.setFeatureCode( "none" );
    		    rrkey.setLibraryRelease( release );
    		    
    		    ReleaseRecordTO releaseRecordTO; 
    		    try{
    		    	releaseRecordTO = psFactory.getReleaseRecordService().viewReleaseRecord(rrkey); 
    		    }catch (ReleaseRecordNotFoundException rrnfe){
    	        	releaseRecordTO = new ReleaseRecordTO();
    	        	releaseRecordTO.setKey( rrkey );
    	        	psFactory.getReleaseRecordService().create( releaseRecordTO );	
    		    }
            }
		} catch (ProblemSystemException pse) {
            throw new FixException(pse.getMessage(), pse);
		}
    } // end createWorkItemIfNecessary(...)
    
    /**
     * This method will determine whether or not a process group is valid for the automatic
     * work item creation.
     * 
     * @param processGroup - The process group in question
     * 
     * @return True if a work item should be created, false otherwise.
     */
    private boolean isValidForWorkItemCreation(String processGroup)
    {
        // get a list of all the valid process groups
        String[] validProcessGroups = FixPackConstants.VALID_PROCESS_GROUPS_FOR_WORK_ITEM_CREATION;
        
        // loop through each valid process group
        for (int i = 0; i < validProcessGroups.length; i++)
        {
            // check to see if we have a valid group
            if (validProcessGroups[i].equalsIgnoreCase(processGroup))
            {
                return true;
            } // end process group check
        } // end loop through all process groups
        
        // default to return false
        return false;
    } // end isValidForWorkItemCreation(...)

	private void setOwningPtf() throws FixException
	{
		reqFixInfo.getLogger().finer("setOwningPtf()");
		Vector potentialOwningPtfs = this.reqFixWorker.getPotentialOwningPtfs(this.reqFixInfo);
		if (potentialOwningPtfs == null || potentialOwningPtfs.size() < 1)
		{
			String error = "Unable to get the owning PTF.";
			throw new FixException(error);
		}
		Ptf owningPtf = null;
		if (potentialOwningPtfs.size() == 1)
		{
			owningPtf = (Ptf) potentialOwningPtfs.elementAt(0);
			this.reqFixInfo.setPtfForRequest(owningPtf);
		}
		else
		{
			String ptfNameFromPtfFile = this.ptfFile.getPtf().getName();
			if (ptfNameFromPtfFile == null || ptfNameFromPtfFile.trim().equals(""))
			{
				owningPtf = (Ptf) potentialOwningPtfs.firstElement();
			}
			else
			{
				for (int i = 0; i < potentialOwningPtfs.size(); i++)
				{
					owningPtf = (Ptf) potentialOwningPtfs.elementAt(i);
					if (ptfNameFromPtfFile.equals(owningPtf.getName()))
					{
						break;
					}
				}
			}
			this.reqFixInfo.setPtfForRequest(owningPtf);
			this.reqFixInfo.setPtfToLoadDataFrom(owningPtf);
		}
	}

	/**
	 * Method processTrackInfomation
	 * @throws FixException 
	 */
	private void processTrackInfomation() throws FixException, ReqFixWarning
	{
		reqFixInfo.getLogger().finer("nextAction()");
		this.reqFixWorker.processTrackInformation(this.reqFixInfo, this);
		this.reqFixWorker.setFirmwareApplyTypeWithValidation(reqFixInfo, true, this.ptfFile.getPtf().getFirmwareInstallType());
	}

	/**
	 * Method requestAction
	 * @throws FixException 
	 */
	private void requestAction() throws FixException, ReqFixWarning
	{
		reqFixInfo.getLogger().finer("requestAction()");
		setInformationForRequest();
		this.reqFixWorker.requestAction(this.reqFixInfo, this);
	}

	/**
	 * Method setInformationForRequest
	 * @throws FixException 
	 */
	private void setInformationForRequest() throws FixException
	{
		this.reqFixInfo.getLogger().finer("setInformationForRequest()");
		String[] validTabs = this.reqFixWorker.getValidTabs(this.reqFixInfo);
		for (int i = 0; i < validTabs.length; i++)
		{
			String tabName = validTabs[i];
			if (tabName.equals(FIX_ATTRIBUTES))
			{
				setFixAttributeInformation();
			}
			else if (tabName.equals(REQUISITES))
			{
				setRequisiteInformation();
			}
			else if (tabName.equals(EXIT_PROGRAMS))
			{
				setExitProgramInformation();
			}
			else if (tabName.equals(SPECIAL_INSTRUCTIONS))
			{
				setSpecialInstructionInformation();
			}
			else if (tabName.equals(BUILD_INSTRUCTIONS))
			{
				setBuildInstructionInformation();
			}
			else if (tabName.equals(EVENT_HANDLERS))
			{
				setEventHandlerInformation();
			}
			else if (tabName.equals(NLV_SELECTION))
			{
				setNLVInformation();
			}
			else if (tabName.equals(DELIVERY_ATTRIBUTES))
			{
				setDeliveryAttributeInformation();
			}
		}
	}

	/**
	 * Method setSpecialInstructionInformation
	 * @throws FixException 
	 */
	private void setSpecialInstructionInformation() throws FixException
	{
		reqFixInfo.getLogger().finer("setSpecialInstructionInformation()");
		this.reqFixInfo = this.reqFixWorker.getSpecialInstructionsInformation(this.reqFixInfo);
		PtfInstructions specialInstructions = this.ptfFile.getPtfInstructions(REQUESTED_SPECIAL);

		if (specialInstructions == null || specialInstructions.getText() == null || specialInstructions.getText().trim().equals(""))
		{
			// Blank out all the special instructions info from the info object
			this.reqFixInfo.setRequestedSpecialInstructions(null);
			Ptf ptfForRequest = this.reqFixInfo.getPtfForRequest();
			ptfForRequest.setIsHeldSpi(false);
			ptfForRequest.setIsResaveHeldSpi(false);
			ptfForRequest.setReqHasSpecPreImm(false);
			ptfForRequest.setReqHasSpecPreDly(false);
			ptfForRequest.setReqHasSpecPostApp(false);
			this.reqFixInfo.setJobPreconditions(null);
			this.reqFixInfo.setObjectPreconditions(null);
		} //end if
		else
		{
			// Set all the special instructions information into the info object.
			specialInstructions = new PtfInstructions(PtfInstructions.TYPE_REQUESTED_SPECIAL, ReqFixWorker.formatText(specialInstructions.getText(), 72));
			this.reqFixInfo.setRequestedSpecialInstructions(specialInstructions);
			Ptf ptfForRequest = this.reqFixInfo.getPtfForRequest();
			ptfForRequest.setIsHeldSpi(this.ptfFile.getPtf().isHeldSpi());
			ptfForRequest.setIsResaveHeldSpi(this.ptfFile.getPtf().isResaveHeldSpi());
			ptfForRequest.setReqHasSpecPreImm(this.ptfFile.getPtf().reqHasSpecPreImm());
			ptfForRequest.setReqHasSpecPreDly(this.ptfFile.getPtf().reqHasSpecPreDly());
			ptfForRequest.setReqHasSpecPostApp(this.ptfFile.getPtf().reqHasSpecPostApp());
			this.reqFixInfo.setJobPreconditions(this.ptfFile.getPtfPreconJobs());
			this.reqFixInfo.setObjectPreconditions(this.ptfFile.getPtfPreconObjects());
		} //end else 
	} //end getSpecialInstructionInformation()

	/**
	 * Method setRequisiteInformation
	 * @throws FixException 
	 */
	private void setRequisiteInformation() throws FixException
	{
		reqFixInfo.getLogger().finer("setRequisiteInformation()");
		this.reqFixInfo = this.reqFixWorker.getRequestedRequisites(this.reqFixInfo);
		Vector ptfReqRequisites = this.ptfFile.getPtfReqRequisites();
		if (ptfReqRequisites != null && ptfReqRequisites.size() > 0)
		{
			//There may be 'shadowed' requisites that need to be added to the request. 
			//This method will add any needed shadow requisites to the list of requisites on the request.
			//If there are no shadow requisites that need to be added, then the list will not be changed.
			RequisiteUtilities requisiteUtil = new RequisiteUtilities(reqFixInfo.getFixUser());
			ptfReqRequisites =
				requisiteUtil.addNeededClonedRequisitesForMultipleSpecifiedRequisites(
					reqFixInfo.getPtfForRequest(),
					ptfReqRequisites,
					reqFixInfo.getFixUser().getClientIdentity());

			this.reqFixWorker.validateRequisite(this.reqFixInfo, ptfReqRequisites);
			this.reqFixInfo.setRequisites(ptfReqRequisites);
		}
	}

	/**
	 * Method setNLVInformation
	 * @throws FixException 
	 */
	private void setNLVInformation() throws FixException
	{
		reqFixInfo.getLogger().finer("setNLVInformation()");
		System.out.println("***************    setNLVInformation() doing no work.   *************");
	}

	/**
	 * Method setFixAttributeInformation
	 * @throws FixException 
	 */
	private void setFixAttributeInformation() throws FixException
	{
		reqFixInfo.getLogger().finer("setFixAttributeInformation()");
		this.reqFixWorker.setAttributeInformation(this.reqFixInfo);
		boolean applyTypeFieldValid = false;
		String[] fieldArray = this.reqFixWorker.getFixAttributeFields(this.reqFixInfo);
		for (int i = 0; i < fieldArray.length; i++)
		{
			String field = fieldArray[i];
			if (field.equals(OWNER_FIELD))
			{
				// Already set in the info object
				continue;
			}
			else if (field.equals(TEST_FIX_FIELD))
			{
				String ptfType = this.ptfFile.getPtf().getType();
				boolean isTestFix = ptfType.equals(Ptf.TESTFIX_TYPE);
				this.reqFixInfo.setTestFix(isTestFix);
			}
			else if (field.equals(FIRMWARE_APPLY_TYPE_FIELD))
			{
				String firmwareApplyType = this.ptfFile.getPtf().getFirmwareInstallType();
				if (firmwareApplyType == null)
				{
					// A value was not set on the PTF object..  Default it to concurrent
					firmwareApplyType = PTFConstants.CONCURRENT_VALUE;
				}
				if (firmwareApplyType.equals(PTFConstants.DEFERRED_VALUE))
				{
					// The apply type is deferred.   set it in the info object
					this.reqFixInfo.setDeferred();
					this.reqFixInfo.getPtfForRequest().setFirmwareInstallType(PTFConstants.DEFERRED_VALUE);
				}
				else if (firmwareApplyType.equals(PTFConstants.DISRUPTIVE_VALUE))
				{
					//	The apply type is disruptive.   set it in the info object
					this.reqFixInfo.setdisruptive();
					this.reqFixInfo.getPtfForRequest().setFirmwareInstallType(PTFConstants.DISRUPTIVE_VALUE);
				}
				else
				{
					// the apply type must be concurrent.  set it in the info object
					this.reqFixInfo.setConcurrent();
					this.reqFixInfo.getPtfForRequest().setFirmwareInstallType(PTFConstants.CONCURRENT_VALUE);
				}
			}
			else if (field.equals(AUTO_PERM_APPLY_FIELD))
			{
				boolean isAutoPermApply = this.ptfFile.getPtf().isAutoPermApplied();
				this.reqFixInfo.getPtfForRequest().setIsAutoPermApplied(isAutoPermApply);
			}
			else if (field.equals(RECOMPILE_FIELD))
			{
				boolean isCompileRequired = this.ptfFile.getPtf().isCompileRequired();
				this.reqFixInfo.getPtfForRequest().setIsCompileRequired(isCompileRequired);
			}
			else if (field.equals(SPE_FIELD))
			{
				String speName = this.ptfFile.getPtf().getSpeName();
				this.reqFixInfo.getPtfForRequest().setSpeName(speName);
			}
			else if (field.equals(SPECIAL_INSTRUCTIONS_FIELD))
			{
				this.reqFixInfo = this.reqFixWorker.getSpecialInstructionsInformation(this.reqFixInfo);
				PtfInstructions specialInstructions = this.ptfFile.getPtfInstructions(REQUESTED_SPECIAL);
				String specialInstructionsText = null;
				if (specialInstructions == null || specialInstructions.getText() == null || specialInstructions.getText().trim().equals(""))
				{
					specialInstructionsText = "";
				}
				else
				{
					specialInstructionsText = specialInstructions.getText();
				}
				//begin code commented out by MHD; DV07809
				/*
				specialInstructions.setText(specialInstructionsText);
				this.reqFixInfo.setRequestedSpecialInstructions(specialInstructions);
				*/
				//end code commented out by MHD; DV07809
				//begin code added by MHD; DV07809
				//This method will take the specified special instructions, strip off tool added data, append
				//library part and shiplist part information to them if needed, and then put them into the info object as
				//'requestedSpecialInstructions'.  When this method returns, the info object will have been updated.
				this.reqFixWorker.setSpecialInstructionsFromCommandLine(specialInstructionsText, this.reqFixInfo);
				//end code added by MHD; DV07809
			}
			else if (field.equals(APPLY_TYPE_FIELD))
			{
				applyTypeFieldValid = true;
			}
			if (applyTypeFieldValid)
			{
				String applyType = this.ptfFile.getPtf().getApplyType();
				if (applyType == null || applyType.trim().equals(""))
				{
					applyType = Ptf.IMMEDIATE_APPLYTYPE;
				}
				this.reqFixInfo.getPtfForRequest().setReqApplyType(applyType);
				if (applyType.equals(Ptf.IMMEDIATE_APPLYTYPE))
				{
					this.reqFixInfo.setRequestedActivationInstructions(this.ptfFile.getPtfInstructions(FINAL_ACTIVATION));
				}
			}
			else
			{
				this.reqFixInfo.getPtfForRequest().setReqApplyType(Ptf.DELAYED_APPLYTYPE);
			}
			String profileStatus = this.ptfFile.getPtf().getProfilingStatus();
			this.reqFixInfo.getPtfForRequest().setProfilingStatus(profileStatus);
			this.reqFixInfo.setProfileStatus(profileStatus);
		}
	}

	/**
	 * Method setExitProgramInformation
	 * @throws FixException 
	 */
	private void setExitProgramInformation() throws FixException
	{
		this.reqFixInfo.getLogger().finer("setExitProgramInformation()");
		Vector validatedExitPrograms = new Vector();
		Vector candidateExitProgramList = this.ptfFile.getPtfExitPrograms();
		if (candidateExitProgramList != null && candidateExitProgramList.size() > 0)
		{
			for (int i = 0; i < candidateExitProgramList.size(); i++)
			{
				PtfExitProgram candidatePtfExitProgram = (PtfExitProgram) candidateExitProgramList.elementAt(i);
				this.reqFixWorker.validateExitProgram(this.reqFixInfo, validatedExitPrograms, candidatePtfExitProgram);
				validatedExitPrograms.addElement(candidatePtfExitProgram);
			}
		}
		this.reqFixInfo.setExitPrograms(validatedExitPrograms);
	}

	/**
	 * Method setEventHandlerInformation
	 * @throws FixException 
	 */
	private void setEventHandlerInformation() throws FixException
	{
		reqFixInfo.getLogger().finer("setEventHandlerInformation()");
		Vector validatedEventHandlers = new Vector();

		Vector candidateEventHandlers = this.ptfFile.getPtfEventHandlers();
		if (candidateEventHandlers != null && candidateEventHandlers.size() > 0)
		{
			this.reqFixInfo = this.reqFixWorker.getEventHandlers(this.reqFixInfo);
			Vector eventHandlersInDb = this.reqFixInfo.getEventHandlers();
			if (eventHandlersInDb == null || eventHandlersInDb.size() < 1)
			{
				String error = "No valid Event Handlers were returned from the PTF database.   The specified Event Handler(s) are not allowed.";
				throw new FixException(error);
			}
			for (int i = 0; i < candidateEventHandlers.size(); i++)
			{
				boolean found = false;
				PtfEventHandler candidateEventHandler = (PtfEventHandler) candidateEventHandlers.elementAt(i);
				for (int j = 0; j < eventHandlersInDb.size(); j++)
				{
					PtfEventHandler eventHandlerInDb = (PtfEventHandler) eventHandlersInDb.elementAt(j);
					if (eventHandlerInDb != null)
					{
						if (eventHandlerInDb.getName().equals(candidateEventHandler.getName()) && eventHandlerInDb.getType().equals(candidateEventHandler.getType()))
						{
							found = true;
							break;
						}
					}
				}
				if (!found)
				{
					String error =
						"The Event Handler defined by the name '"
							+ candidateEventHandler.getName()
							+ "' and type '"
							+ candidateEventHandler.getType()
							+ "' is not valid.  The Event Handler is not listed as a valid candidate in the database.";
					throw new FixException(error);
				}
				else
				{
					validatedEventHandlers.addElement(candidateEventHandler);
				}
			}
		}
		this.reqFixInfo.setEventHandlers(validatedEventHandlers);
	}

	/**
	 * Method setDeliveryAttributeInformation
	 * @throws FixException 
	 */
	private void setDeliveryAttributeInformation() throws FixException
	{
		reqFixInfo.getLogger().finer("setDeliveryAttributeInformation()");
		if (this.reqFixWorker.getReleaseUsage(this.reqFixInfo).equals(PTE))
		{
			this.reqFixInfo.setHiper(false);
			this.reqFixInfo.setHoldFromCume(false);
			this.reqFixInfo.setHoldFromResave(false);
			this.reqFixInfo.setForcedIntoNextCume(false);
			this.reqFixInfo.setManufacturingLineBreakinNeeded(false);
		}
		else
		{
			// Should we be getting the value from isHiper or isReqHiper?
			this.reqFixInfo.setHiper(this.ptfFile.getPtf().reqIsHiper());
			this.reqFixInfo.setHoldFromCume(this.ptfFile.getPtf().isHeldDev());
			this.reqFixInfo.setHoldFromResave(this.ptfFile.getPtf().isResaveHeldDev());
			this.reqFixInfo.setForcedIntoNextCume(this.ptfFile.getPtf().isForcedToNextCum());
			this.reqFixInfo.setManufacturingLineBreakinNeeded(this.ptfFile.getPtf().isMfgLineRequested());
		}
	}

	/**
	 * Method setBuildInstructionInformation
	 * @throws FixException 
	 */
	private void setBuildInstructionInformation() throws FixException
	{
		reqFixInfo.getLogger().finer("setBuildInstructionInformation()");
		this.reqFixInfo = this.reqFixWorker.getPreAndPostBuildInstructions(this.reqFixInfo);
		this.reqFixInfo.setPreBuildInstructions(this.ptfFile.getPtfInstructions(PRE_BUILD));
		this.reqFixInfo.setPostBuildInstructions(this.ptfFile.getPtfInstructions(POST_BUILD));
	}

	/** 
	 * @see com.ibm.sdwb.cps.reqfix.client.WarningIfc#continueProcessingAfterWarning(com.ibm.sdwb.cps.reqfix.ReqFixWarning)
	 */
	public boolean continueProcessingAfterWarning(ReqFixWarning warning)
	{
		reqFixInfo.getLogger().finer("continueProcessingAfterWarning(" + warning + ")");
		reqFixInfo.getLogger().warning(warning.getMessage() + "  --  Continuing");
		return true;
	}

	public void updateUI(int progressBarInc, String statusText)
	{
	}
}
