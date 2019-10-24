package com.ibm.sdwb.cps.reqfix;

import java.util.HashMap;
import java.util.Vector;
import java.util.logging.Logger;

import com.ibm.bst.fixcommon.FixUser;
import com.ibm.bst.fixcommon.exceptions.FixException;
import com.ibm.dv2Db.ptfDb.ifObjects.FixPack;
import com.ibm.dv2Db.ptfDb.ifObjects.Ptf;
import com.ibm.dv2Db.ptfDb.ifObjects.PtfInstructions;
import com.ibm.dv2Db.ptfDb.ifObjects.VEFIXCCOption;
import com.ibm.dv2common.LoggingInfoObject;
import com.ibm.sdwb.cpscommon.PtfAuthority;

/**
 * File:  ReqFixInfo.java
 * @author Aaron Hislop, ahislop@us.ibm.com
 * Date:  Mar 12, 2003
 * 
 * Change Activity:
 * 
 * PTR     	Date     		Pgmr ID  	Change Description
 * --------	-------- 		---------	----------------------------------
 * 			Mar 12, 2003		ahislop		Initial creation of the file
 */
public class ReqFixInfo extends LoggingInfoObject
{
	protected final static String PROCESS_TYPE_FIELD = "Process Type";
	protected final static String PROCESS_SUBTYPE_FIELD = "Process Subtype";
	protected final static String PTFASSOCIATIONS_ON_REQUEST_FIELD = "PtfAssociations on Request";
	protected final static String PTF_FOR_REQUEST_FIELD = "PTF For Request";
	protected final static String REQUESTED_FEATURE_CODE_FIELD = "Requested Feature Code";
	protected final static String REQUESTORS_UID_FIELD = "Requestors UID";
	protected final static String STAGE_FOR_REQUEST_FIELD = "Stage for Request";
	protected final static String PTF_DATABASE_FIELD = "PTF Database";
	protected final static String PRF_DATABASE_FIELD = "PRF Database";
	protected final static String PTF_TO_LOAD_DATA_FROM = "PTF to Load Data From";
	protected final static String REQFIX_AUTHORITY = "ReqFix Authority";
	protected final static String PROFILE_STATUS_FIELD = "Profile Status";
	protected final static String REQFIX_UPDATE_FIELD = "IReqFixUpdate";
	protected final static String TARGETED_FIX_PACK = "TARGETED_FIX_PACK";
	protected final static String SPES_THAT_WERE_PREVIOUSLY_ASSOCIATED_FIELD = "SPEs That Were Previously Associated";
	protected final static String SPE_INFO_FIELD = "SPE Info Field";
	protected final static String REQUESTED_ACTIVATION_INSTRUCTIONS_FIELD = "Request Activation Instructions";
	protected final static String EVENT_HANDLERS_FIELD = "Event Handlers";
	protected final static String POTENTIAL_EVENT_HANDLERS_FIELD = "Potential Event Handlers";
	protected final static String EXIT_PROGRAMS_FIELD = "Exit Programs";
	protected final static String EXTRA_MRI_PTFS_NEEDED = "Extra MRI PTFs Needed";
	protected final static String FORCE_INTO_NEXT_CUME_FIELD = "Force into next CUME";
	protected final static String HIPER_FIELD = "HIPER";
	protected final static String HOLD_FROM_CUME_FIELD = "Hold from CUME";
	protected final static String HOLD_FROM_RESAVE_FIELD = "Hold from Resave";
	protected final static String JOB_PRECONDITIONS_FIELD = "Job Preconditions";
	protected final static String MANUFACTURING_LINE_BREAKIN_NEEDED_FIELD = "Manufacturing Line Break-in Needed";
	protected final static String MRI_FEATURE_CODES_TO_CREATE_MRI_PTFS_FIELD = "MRI Feature Codes to create MRI PTFs for";
	protected final static String MRI_PARTS_ON_SHIPLIST = "MRI Parts on ShipList";
	protected final static String QPZ1_PARTS_ON_SHIPLIST = "QPZ1 Parts on ShipList";
	protected final static String OBJECT_PRECONDITIONS_FIELD = "Object Preconditions";
	protected final static String ONLY_DELAY_APPLY_IS_ALLOWED_FIELD = "Only Delay Applied is Allowed";
	protected final static String ONLY_IMMEDIATE_APPLY_IS_ALLOWED_FIELD = "Only Immediate Applied is Allowed";
	protected final static String POST_BUILD_INSTRUCTIONS_FIELD = "Postbuild Instructions";
	protected final static String PRE_BUILD_INSTRUCTIONS_FIELD = "Prebuild Instructions";
	protected final static String PTF_BUILDER_FIELD = "PTF Builder";
	protected final static String REQUISITES_FIELD = "Requisites";
	protected final static String REQUESTED_SPECIAL_INSTRUCTIONS_FIELD = "Requested Special Instructions";
	protected final static String TEST_FIX_ANSWER_CHANGED_FIELD = "Test Fix Answer Changed";
	protected final static String TEST_FIX_FIELD = "Test Fix";
	protected final static String DISRUPTIVE_FIX_FIELD = "Disruptive";
	protected final static String MAY_NEED_TO_MARK_DISRUPTIVE = "May Need to Mark Disruptive";
	protected final static String OVERRIDE_INFO = "OverrideInfo";
	protected final static String SUPERSEDE_INFO = "SupersedeInfo";
	protected final static String LOCK_DISRUPTIVE_FIELD = "Lock Disruptive";
	protected final static String FULL_PATH_TO_XML_FILE_FIELD = "Full Path To XML File";
	protected final static String SPE_PTF_ORIGINALLY_MAPPED_TO = "SPE_PTF_ORIGINALLY_MAPPED_TO";
	protected final static String SKIP_EXISTING_COMPONENT_PTF_CHECK = "SKIP_EXISTING_COMPONENT_PTF_CHECK";
	protected final static String FIX_USER_FIELD = "Fix User";
	protected final static String VEFIXCCOPTION_FIELD = "VEFIXCCOption";
	protected final static String REASON_FOR_DISRUPTIVE_FIELD = "Reason for disruptive";
	protected final static String REASON_FOR_DEFERRED_FIELD = "Reason for deferred";
	protected final static String EXIT_PROGRAM_EXPLANATION_FOR_ACTIVATION = "Explanation of exit program for activation.";
	protected final static String EXIT_PROGRAM_EXPLANATION_FOR_REMOVAL = "Explanation of exit program for removal.";
	protected final static String OPERATING_SYSTEM_FIELD = "Operating System";
	protected final static String SECONDARY_REQUEST_INFO = "Secondary Request Info";
	protected final static String PRE_INSTALL_INSTRUCTIONS_FIELD = "PreInstall Instructions";
	protected final static String POST_INSTALL_INSTRUCTIONS_FIELD = "PostInstall Instructions";
	protected final static String PRE_UNINSTALL_INSTRUCTIONS_FIELD = "PreUninstall Instructions";
	protected final static String POST_UNINSTALL_INSTRUCTIONS_FIELD = "PostUninstall Instructions";
	protected final static String GENERAL_INSTRUCTIONS_FIELD = "General Instructions";
	protected final static String BUILDER_INSTRUCTIONS_FIELD = "Builder Instructions";
	protected final static String EXTERNAL_REQUISITES_FIELD = "External Requisites";
	protected final static String BUILD_TYPE_FIELD = "Build Type";
	protected final static String DELAY_REMOVE_FIELD = "Delay-remove";
	protected final static String IS_CLONE_FIELD = "Is Clone";
	protected final static String FIXPACK_APPLY_FIELD = "FixPack Apply Field";
	protected final static String ENABLE_DEFERRED_APPLY_FIELD = "Enable Deferred Apply Field";
	protected final static String ENABLE_CONCURRENT_APPLY_FIELD = "Enable Concurrent Apply Field";
	protected final static String IS_DISTRIBUTABLE_INITIATIVE_FIELD = "Is Distributable Initiative";
	protected final static String OVERRIDE_BUILD_ORDER_FIELD = "Override Build Order";
	

	//////////////////
	public String getFullPathToXMLFile()
	{
		return (String) getField(FULL_PATH_TO_XML_FILE_FIELD);
	} //end getFullPathToXMLFile()

	public String getSPEPtfOriginallyMappedTo()
	{
		return (String) getField(SPE_PTF_ORIGINALLY_MAPPED_TO);
	} //end getSPEPtfOriginallyMappedTo()

	public boolean isDisruptive() throws FixException
	{
		Boolean bool = (Boolean) getField(DISRUPTIVE_FIX_FIELD);
		if (bool == null)
		{
			String error = "The '" + DISRUPTIVE_FIX_FIELD + "' flag was not set in the info object.  Unable to determine if this LID Build is Distruptive.";
			throw new FixException(error);
		}
		return bool.booleanValue();
	}

	public boolean getSkipExistingComponentPTFCheck()
	{
		Boolean bool = (Boolean) getField(SKIP_EXISTING_COMPONENT_PTF_CHECK);
		if (bool == null)
		{
			//default to false
			return false;
		} //end if
		return bool.booleanValue();
	} //ed getSkipExistingComponentPTFCheck()

	public boolean getMayNeedToMarkAsDisruptive() throws FixException
	{
		Boolean bool = (Boolean) getField(MAY_NEED_TO_MARK_DISRUPTIVE);
		if (bool == null)
		{
			String error = "The '" + MAY_NEED_TO_MARK_DISRUPTIVE + "' flag was not set in the info object.  Unable to determine if this LID Build should be " + "marked as disruptive.";
			throw new FixException(error);
		} //end if
		return bool.booleanValue();
	} //end getMayNeedToMarkAsDisruptive()

	public OverrideInfo getOverrideInfo() throws FixException
	{
		OverrideInfo overrideInfo = (OverrideInfo) getField(OVERRIDE_INFO);
		if (overrideInfo == null)
		{
			throw new FixException("Unable to get the OverrideInfo object. The object was null.");
		} //end if
		return overrideInfo;
	} //end getOverrirdeInfo

	public SupersedeInfo getSupersedeInfo() throws FixException
	{
		SupersedeInfo supersedeInfo = (SupersedeInfo) getField(SUPERSEDE_INFO);
		if (supersedeInfo == null)
		{
			throw new FixException("Unable to get the SupersedeInfo object. The object was null.");
		} //end if
		return supersedeInfo;
	} //end getSupersedeInfo()

	/**
	 * Method setDisruptive.
	 * @param disruptive
	 */
	public void setDisruptive(boolean disruptive)
	{
		Boolean bool = new Boolean(disruptive);
		setField(DISRUPTIVE_FIX_FIELD, bool);
	}

	public void setMayNeedToMarkAsDisruptive(boolean mayNeedToMarkAsDisruptive)
	{
		Boolean bool = new Boolean(mayNeedToMarkAsDisruptive);
		setField(MAY_NEED_TO_MARK_DISRUPTIVE, bool);
	} //end setMayNeedToMarkAsDisruptive()

	public void setOverrideInfo(OverrideInfo overrideInfo) throws FixException
	{
		if (overrideInfo == null)
		{
			throw new FixException("Unable to set the OverrideInfo object. The object was null.");
		} //end if
		setField(OVERRIDE_INFO, overrideInfo);
	} //end setOverrrideInfo()

	public void setSupersedeInfo(SupersedeInfo supersedeInfo) throws FixException
	{
		if (supersedeInfo == null)
		{
			throw new FixException("Unable to set the SupersedeInfo object.  The object was null.");
		} //end if
		setField(SUPERSEDE_INFO, supersedeInfo);
	} //end setSupersedeInfo()

	/**
	 * Constructor for ReqFixInfo.
	 */
	public ReqFixInfo()
	{
		super();
	}

	public ReqFixAuthority getAuthority() throws FixException
	{
		ReqFixAuthority authority = (ReqFixAuthority) getField(REQFIX_AUTHORITY);
		if (authority == null)
		{
			authority = new ReqFixAuthority(getFixUser());
			setAuthority(authority);
		}
		return authority;
	}

	public FixPack getTargetedFixPack()
	{
		return (FixPack) getField(TARGETED_FIX_PACK);
	}

	public MetadataUtilities getMetadataDatabaseConnection()
	{
		return getMetadataDatabaseConnection(true);
	}

	public MetadataUtilities getMetadataDatabaseConnection(boolean makeConnection)
	{
		MetadataUtilities metaDataUtilities = (MetadataUtilities) getField(PRF_DATABASE_FIELD);
		if (metaDataUtilities == null && makeConnection)
		{
			metaDataUtilities = new MetadataUtilities();
			setMetadataDatabaseConnection(metaDataUtilities);
		}
		return metaDataUtilities;
	}

	public PTFDBConnectionUtilities getPTFDatabaseConnection() throws FixException
	{
		return getPTFDatabaseConnection(true);
	} //end getPTFDatabaseConnection()

	public PTFDBConnectionUtilities getPTFDatabaseConnection(boolean makeConnection) throws FixException
	{
		PTFDBConnectionUtilities ptfDbConnection = (PTFDBConnectionUtilities) getField(PTF_DATABASE_FIELD);
		if (ptfDbConnection == null && makeConnection)
		{
			try
			{
				ptfDbConnection = new PTFDBConnectionUtilities(getFixUser());
				setPTFDatabaseConnection(ptfDbConnection);
			}
			catch (FixException fe)
			{
				String error = "An error occurred while attempting to create the connection to the Fix Database.  The error was:  " + fe.getMessage();
				throw new FixException(error);
			}
		}
		return ptfDbConnection;
	} //end getPTFDatabaseConnection()

	public Ptf getPtfForRequest()
	{
		return (Ptf) getField(PTF_FOR_REQUEST_FIELD);
	}

	public Ptf getPtfToLoadDataFrom()
	{
		return (Ptf) getField(PTF_TO_LOAD_DATA_FROM);
	} //end getPtfToLoadDataFrom()

	public IReqFixUpdate getReqFixUpdate()
	{
		return (IReqFixUpdate) getField(REQFIX_UPDATE_FIELD);
	}

	public String getRequestedFeatureCode()
	{
		return (String) getField(REQUESTED_FEATURE_CODE_FIELD);
	}

	public String getRequestorUID()
	{
		return (String) getField(REQUESTORS_UID_FIELD);
	}

	public String getProcessSubType()
	{
		return (String) getField(PROCESS_SUBTYPE_FIELD);
	}

	public String getProcessType()
	{
		return (String) getField(PROCESS_TYPE_FIELD);
	}

	public String getStageForRequest()
	{
		return (String) getField(STAGE_FOR_REQUEST_FIELD);
	}

	/**
	 * Method getProfileStatus returns a String that contains the Profile Status.
	 * @return String
	 */
	public String getProfileStatus()
	{
		return (String) getField(PROFILE_STATUS_FIELD);
	}

	public Vector getPtfAssociationsOnRequest()
	{
		return (Vector) getField(PTFASSOCIATIONS_ON_REQUEST_FIELD);
	}

	public void setAuthority(ReqFixAuthority authority)
	{
		if (authority != null)
		{
			setField(REQFIX_AUTHORITY, authority);
		}
	}

	public SPEInfo getSPEInfo()
	{
		return (SPEInfo) getField(SPE_INFO_FIELD);
	} //end getSPEInfo()

	public Vector getSPEsThatWerePreviouslyAssociated()
	{
		return (Vector) getField(SPES_THAT_WERE_PREVIOUSLY_ASSOCIATED_FIELD);
	} //end getSPEsThatWerePreviouslyAssociated()

	public void setMetadataDatabaseConnection(MetadataUtilities metadataUtil)
	{
		if (metadataUtil != null)
		{
			setField(PRF_DATABASE_FIELD, metadataUtil);
		}
	}

	public void setPTFDatabaseConnection(PTFDBConnectionUtilities ptfDbConnectionUtilities)
	{
		if (ptfDbConnectionUtilities != null)
		{
			setField(PTF_DATABASE_FIELD, ptfDbConnectionUtilities);
		}
	}

	public void setPtfForRequest(Ptf ptfForRequest)
	{
		if (ptfForRequest != null)
		{
			setField(PTF_FOR_REQUEST_FIELD, ptfForRequest);
		}
	}

	public void setPtfToLoadDataFrom(Ptf ptfToLoadDataFrom)
	{
		//its ok if the PTF passed in is null
		setField(PTF_TO_LOAD_DATA_FROM, ptfToLoadDataFrom);
	} //end setPtfToLoadDataFrom()

	public void setReqFixUpdate(IReqFixUpdate reqFixUpdate)
	{
		if (reqFixUpdate != null)
		{
			setField(REQFIX_UPDATE_FIELD, reqFixUpdate);
		}
	}

	public void setRequestedFeature(String requestedFeature)
	{
		if (requestedFeature != null)
		{
			setField(REQUESTED_FEATURE_CODE_FIELD, requestedFeature);
		}
	}

	public void setRequestorUID(String requestorUID)
	{
		if (requestorUID != null)
		{
			setField(REQUESTORS_UID_FIELD, requestorUID);
		}
	}

	public void setProcessSubType(String processSubType)
	{
		if (processSubType != null)
		{
			setField(PROCESS_SUBTYPE_FIELD, processSubType);
		}
	}

	public void setProcessType(String processType)
	{
		if (processType != null)
		{
			setField(PROCESS_TYPE_FIELD, processType);
		}
	}

	public void setStageForRequest(String stageForRequest)
	{
		if (stageForRequest != null)
		{
			setField(STAGE_FOR_REQUEST_FIELD, stageForRequest);
		}
	}

	/**
	 * Method setProfileStatus.
	 * @param profileStatus
	 */
	public void setProfileStatus(String profileStatus)
	{
		if (profileStatus != null)
		{
			setField(PROFILE_STATUS_FIELD, profileStatus);
		}
	}

	public void setPtfAssociationsOnRequest(Vector tracksOnRequest)
	{
		if (tracksOnRequest != null)
		{
			setField(PTFASSOCIATIONS_ON_REQUEST_FIELD, tracksOnRequest);
		}
	}

	public void setTargetedFixPack(FixPack fixPack)
	{
		setField(TARGETED_FIX_PACK, fixPack);
	}

	public void setSPEsThatWerePreviouslyAssociated(Vector spesThatWerePreviouslyAssociated)
	{
		if (spesThatWerePreviouslyAssociated != null)
		{
			setField(SPES_THAT_WERE_PREVIOUSLY_ASSOCIATED_FIELD, spesThatWerePreviouslyAssociated);
		} //end if
	} //end setSPEsThatWerePreviouslyAssociated()

	public void setSPEInfo(SPEInfo speInfo)
	{
		if (speInfo != null)
		{
			setField(SPE_INFO_FIELD, speInfo);
		} //end if
	} //end setSPEInfo()

	/**
	 * Method areExtraMRIPtfsNeeded returns a boolean stating if extra MRI PTFs need to be created.
	 * @return boolean
	 */
	public boolean areExtraMRIPtfsNeeded()
	{
		Boolean bool = (Boolean) getField(EXTRA_MRI_PTFS_NEEDED);
		if (bool == null)
		{
			//default the value to false
			return false;
		}
		return bool.booleanValue();
	}

	/**
	 * Method areMRIPartsOnShipList returns a boolean stating if the ShipList contains any MRI parts.
	 * @return boolean
	 */
	public boolean areMRIPartsOnShipList()
	{
		Boolean bool = (Boolean) getField(MRI_PARTS_ON_SHIPLIST);
		if (bool == null)
		{
			//default the value to false
			return false;
		}
		return bool.booleanValue();
	}

	public boolean areQPZ1PartsOnShipList()
	{
		Boolean bool = (Boolean) getField(QPZ1_PARTS_ON_SHIPLIST);
		if (bool == null)
		{
			//default the value to false
			return false;
		}
		return bool.booleanValue();
	}

	/**
	 * Method didTestFixAnswerChange returns a boolean stating if the Test Fix answer changed.
	 * @return boolean
	 * @throws FixException
	 */
	public boolean didTestFixAnswerChange() throws FixException
	{
		Boolean bool = (Boolean) getField(TEST_FIX_ANSWER_CHANGED_FIELD);
		if (bool == null)
		{
			String error = "The '" + TEST_FIX_ANSWER_CHANGED_FIELD + "' flag was not set in the info object.  Unable to determine if the Test Fix answer changed.";
			throw new FixException(error);
		}
		return bool.booleanValue();
	}

	/**
	 * Method getActivationInstructions returns a PtfInstructions object containing the Activation Instructions.
	 * @return PtfInstructions
	 */
	public PtfInstructions getRequestedActivationInstructions()
	{
		return (PtfInstructions) getField(REQUESTED_ACTIVATION_INSTRUCTIONS_FIELD);
	}

	/**
	 * Method getEventHandlers returns a Vector containing the Event Handlers.
	 * @return Vector
	 */
	public Vector getEventHandlers()
	{
		return (Vector) getField(EVENT_HANDLERS_FIELD);
	}

	/*
	 * Method getEventHandlers returns a Vector containing the potential Event Handlers.
	 * @return Vector
	 */
	public Vector getPotentialEventHandlers()
	{
		return (Vector) getField(POTENTIAL_EVENT_HANDLERS_FIELD);
	}

	/**
	 * Method getExitPrograms returns a Vector containing the Exit Programs.
	 * @return Vector
	 */
	public Vector getExitPrograms()
	{
		return (Vector) getField(EXIT_PROGRAMS_FIELD);
	}

	/**
	 * Method getJobPreconditions returns a Vector containing the Job Preconditions.
	 * @return Vector
	 */
	public Vector getJobPreconditions()
	{
		return (Vector) getField(JOB_PRECONDITIONS_FIELD);
	}

	/**
	 * Method getMRIFeatureCodesToCreateMRIPTFsFor returns a Vector of the MRI Feature Codes to Create PTFs for.
	 * @return Vector
	 */
	public Vector getMRIFeatureCodesToCreateMRIPTFsFor()
	{
		Vector mriFeatures = (Vector) getField(MRI_FEATURE_CODES_TO_CREATE_MRI_PTFS_FIELD);
		if (mriFeatures == null)
		{
			mriFeatures = new Vector();
		} //end if
		return mriFeatures;
	}

	/**
	 * Method getObjectPreconditions returns a Vector containing the Object Preconditions.
	 * @return Vector
	 */
	public Vector getObjectPreconditions()
	{
		return (Vector) getField(OBJECT_PRECONDITIONS_FIELD);
	}

	/**
	 * Method getPostBuildInstructions returns a PtfInstructions object containing the Post Build Instructions.
	 * @return PtfInstructions
	 */
	public PtfInstructions getPostBuildInstructions()
	{
		return (PtfInstructions) getField(POST_BUILD_INSTRUCTIONS_FIELD);
	}

	/**
	 * Method getPreBuildInstructions returns a PtfInstructions object containing the Pre Build Instructions.
	 * @return PtfInstructions
	 */
	public PtfInstructions getPreBuildInstructions()
	{
		return (PtfInstructions) getField(PRE_BUILD_INSTRUCTIONS_FIELD);
	}

	/**
	 * Method getRequisites returns a Vector of the Requisites.
	 * @return Vector
	 */
	public Vector getRequisites()
	{
		return (Vector) getField(REQUISITES_FIELD);
	}

	/**
	 * Method getSpecialInstructions returns a PtfInstructions object containing the SpecialInstructions.
	 * @return PtfInstructions
	 */
	public PtfInstructions getRequestedSpecialInstructions()
	{
		return (PtfInstructions) getField(REQUESTED_SPECIAL_INSTRUCTIONS_FIELD);
	}

	/**
	 * Method isForcedIntoNextCume.
	 * @return boolean
	 * @throws FixException
	 */
	public boolean isForcedIntoNextCume() throws FixException
	{
		Boolean bool = (Boolean) getField(FORCE_INTO_NEXT_CUME_FIELD);
		if (bool == null)
		{
			String error = "The '" + FORCE_INTO_NEXT_CUME_FIELD + "' flag was not set in the info object.  Unable to determine if this PTF should be forced onto the next CUME Package.";
			throw new FixException(error);
		}
		return bool.booleanValue();
	}

	public boolean isDisruptiveLocked()
	{
		Boolean bool = (Boolean) getField(LOCK_DISRUPTIVE_FIELD);
		if (bool == null)
		{
			// field is not set, so default to no
			return false;
		}
		return bool.booleanValue();
	} //end isDisruptiveLocked()

	/**
	 * Method isHiper.
	 * @return boolean
	 * @throws FixException
	 */
	public boolean isHiper() throws FixException
	{
		Boolean bool = (Boolean) getField(HIPER_FIELD);
		if (bool == null)
		{
			String error = "The '" + HIPER_FIELD + "' flag was not set in the info object.  Unable to determine if this PTF is HIPER.";
			throw new FixException(error);
		}
		return bool.booleanValue();
	}

	/**
	 * Method isManufacturingLineBreakinNeeded.
	 * @return boolean
	 * @throws FixException
	 */
	public boolean isManufacturingLineBreakinNeeded() throws FixException
	{
		Boolean bool = (Boolean) getField(MANUFACTURING_LINE_BREAKIN_NEEDED_FIELD);
		if (bool == null)
		{
			String error = "The '" + MANUFACTURING_LINE_BREAKIN_NEEDED_FIELD + "' flag was not set in the info object.  Unable to determine if this PTF requires a Manufacturing line break-in.";
			throw new FixException(error);
		}
		return bool.booleanValue();
	}

	/**
	 * Method isOnlyDelayAppliedIsAllowed.
	 * @return boolean
	 * @throws FixException
	 */
	public boolean isOnlyDelayAppliedIsAllowed() throws FixException
	{
		Boolean bool = (Boolean) getField(ONLY_DELAY_APPLY_IS_ALLOWED_FIELD);
		if (bool == null)
		{
			String error = "The '" + ONLY_DELAY_APPLY_IS_ALLOWED_FIELD + "' flag was not set in the info object.  Unable to determine if this PTF must be delay applied.";
			throw new FixException(error);
		}
		return bool.booleanValue();
	}

	/**
	 * Method isOnlyDelayAppliedIsAllowed.
	 * @return boolean
	 * @throws FixException
	 */
	public boolean isOnlyImmediateAppliedIsAllowed() throws FixException
	{
		Boolean bool = (Boolean) getField(ONLY_IMMEDIATE_APPLY_IS_ALLOWED_FIELD);
		if (bool == null)
		{
			String error = "The '" + ONLY_IMMEDIATE_APPLY_IS_ALLOWED_FIELD + "' flag was not set in the info object.  Unable to determine if this PTF must be delay applied.";
			throw new FixException(error);
		}
		return bool.booleanValue();
	}

	/**
	 * Method isPtfBuilder.
	 * @return boolean
	 * @throws FixException
	 */
	public boolean isPtfBuilder() throws FixException
	{
		Boolean bool = (Boolean) getField(PTF_BUILDER_FIELD);
		if (bool == null)
		{
			return false;
		}
		return bool.booleanValue();
	}

	/**
	 * Method isTestFix.
	 * @return boolean
	 * @throws FixException
	 */
	public boolean isTestFix()
	{
		Boolean bool = (Boolean) getField(TEST_FIX_FIELD);
		if (bool == null)
		{
			setField(TEST_FIX_FIELD, new Boolean(false));
			return false;
		}
		return bool.booleanValue();
	}

	/**
	 * Method setActivationInstructions.
	 * @param activationInstructions
	 */
	public void setRequestedActivationInstructions(PtfInstructions requestedActivationInstructions)
	{
		if (requestedActivationInstructions != null)
		{
			setField(REQUESTED_ACTIVATION_INSTRUCTIONS_FIELD, requestedActivationInstructions);
		}
	}

	/**
	 * Method setEventHandlers.
	 * @param eventHandlers
	 */
	public void setEventHandlers(Vector eventHandlers)
	{
		if (eventHandlers != null)
		{
			setField(EVENT_HANDLERS_FIELD, eventHandlers);
		}
	}

	/**
	 * Method setEventHandlers.
	 * @param eventHandlers
	 */
	public void setPotentialEventHandlers(Vector potentialEventHandlers)
	{
		if (potentialEventHandlers != null)
		{
			setField(POTENTIAL_EVENT_HANDLERS_FIELD, potentialEventHandlers);
		}
	}

	/**
	 * Method setExitPrograms.
	 * @param exitPrograms
	 */
	public void setExitPrograms(Vector exitPrograms)
	{
		if (exitPrograms != null)
		{
			setField(EXIT_PROGRAMS_FIELD, exitPrograms);
		}
	}

	/**
	 * Method setExtraMRIPtfsNeeded.
	 * @param extraMRIPtfsNeeded
	 */
	public void setExtraMRIPtfsNeeded(boolean extraMRIPtfsNeeded)
	{
		Boolean bool = new Boolean(extraMRIPtfsNeeded);
		setField(EXTRA_MRI_PTFS_NEEDED, bool);
	}

	/**
	 * Method setForcedIntoNextCume.
	 * @param forceOntoNextCume
	 */
	public void setForcedIntoNextCume(boolean forceOntoNextCume)
	{
		Boolean bool = new Boolean(forceOntoNextCume);
		setField(FORCE_INTO_NEXT_CUME_FIELD, bool);
	}

	/**
	 * Method setHiper.
	 * @param hiper
	 */
	public void setHiper(boolean hiper)
	{
		Boolean bool = new Boolean(hiper);
		setField(HIPER_FIELD, bool);
	}

	/**
	 * Method setHoldFromCume.
	 * @param holdFromCume
	 */
	public void setHoldFromCume(boolean holdFromCume)
	{
		Boolean bool = new Boolean(holdFromCume);
		setField(HOLD_FROM_CUME_FIELD, bool);
	}

	/**
	 * Method setHoldFromResave.
	 * @param holdFromResave
	 */
	public void setHoldFromResave(boolean holdFromResave)
	{
		Boolean bool = new Boolean(holdFromResave);
		setField(HOLD_FROM_RESAVE_FIELD, bool);
	}

	/**
	 * Method setJobPreconditions.
	 * @param jobPreconditions
	 */
	public void setJobPreconditions(Vector jobPreconditions)
	{
		setField(JOB_PRECONDITIONS_FIELD, jobPreconditions);
	}

	/**
	 * Method setManufacturingLineBreakinNeeded.
	 * @param manufacturingLineBreakinNeeded
	 */
	public void setManufacturingLineBreakinNeeded(boolean manufacturingLineBreakinNeeded)
	{
		Boolean bool = new Boolean(manufacturingLineBreakinNeeded);
		setField(MANUFACTURING_LINE_BREAKIN_NEEDED_FIELD, bool);
	}

	/**
	 * Method setMRIFeatureCodesToCreateMRIPTFsFor.
	 * @param featuresCodes
	 */
	public void setMRIFeatureCodesToCreateMRIPTFsFor(Vector featuresCodes)
	{
		if (featuresCodes != null)
		{
			setField(MRI_FEATURE_CODES_TO_CREATE_MRI_PTFS_FIELD, featuresCodes);
		}
	}

	/**
	 * Method setObjectPreconditions.
	 * @param objectPreconditions
	 */
	public void setObjectPreconditions(Vector objectPreconditions)
	{
		setField(OBJECT_PRECONDITIONS_FIELD, objectPreconditions);
	}

	/**
	 * Method setOnlyDelayAppliedIsAllowed.
	 * @param onlyDelayAppliedIsAllowed
	 */
	public void setOnlyDelayAppliedIsAllowed(boolean onlyDelayAppliedIsAllowed)
	{
		Boolean bool = new Boolean(onlyDelayAppliedIsAllowed);
		setField(ONLY_DELAY_APPLY_IS_ALLOWED_FIELD, bool);
	}

	/**
	 * Method setOnlyImmediateAppliedIsAllowed.
	 * @param onlyDelayAppliedIsAllowed
	 */
	public void setOnlyImmediateAppliedIsAllowed(boolean onlyImmediateAppliedIsAllowed)
	{
		Boolean bool = new Boolean(onlyImmediateAppliedIsAllowed);
		setField(ONLY_IMMEDIATE_APPLY_IS_ALLOWED_FIELD, bool);
	}

	/**
	 * Method setPostBuildInstructions.
	 * @param postBuildInstructions
	 */
	public void setPostBuildInstructions(PtfInstructions postBuildInstructions)
	{
		if (postBuildInstructions != null)
		{
			setField(POST_BUILD_INSTRUCTIONS_FIELD, postBuildInstructions);
		}
	}

	/**
	 * Method setPreBuildInstructions.
	 * @param preBuildInstructions
	 */
	public void setPreBuildInstructions(PtfInstructions preBuildInstructions)
	{
		if (preBuildInstructions != null)
		{
			setField(PRE_BUILD_INSTRUCTIONS_FIELD, preBuildInstructions);
		}
	}

	/**
	 * Method setPtfBuilder.
	 * @param isPtfBuilder
	 */
	public void setPtfBuilder(boolean isPtfBuilder)
	{
		Boolean bool = new Boolean(isPtfBuilder);
		setField(PTF_BUILDER_FIELD, bool);
	}

	/**
	 * Method setRequisites.
	 * @param requisites
	 */
	public void setRequisites(Vector requisites)
	{
		if (requisites != null)
		{
			setField(REQUISITES_FIELD, requisites);
		}
	}

	/**
	 * Method setSpecialInstructions.
	 * @param specialInstructions
	 */
	public void setRequestedSpecialInstructions(PtfInstructions specialInstructions)
	{
		setField(REQUESTED_SPECIAL_INSTRUCTIONS_FIELD, specialInstructions);
	}

	/**
	 * Method setTestFix.
	 * @param testFix
	 */
	public void setTestFix(boolean testFix)
	{
		Boolean bool = new Boolean(testFix);
		setField(TEST_FIX_FIELD, bool);
	}

	public void setSkipExistingComponentPTFCheck(boolean skipExistingComponentPTFCheck)
	{
		Boolean bool = new Boolean(skipExistingComponentPTFCheck);
		setField(SKIP_EXISTING_COMPONENT_PTF_CHECK, bool);
	} //end setSkipExistingComponentPTFCheck()

	public void setAreMRIPartsOnShipList(boolean areMRIPartsOnShiplist)
	{
		Boolean bool = new Boolean(areMRIPartsOnShiplist);
		setField(MRI_PARTS_ON_SHIPLIST, bool);
	} //end setAreMRIPartsOnShiplist()

	public void setAreQPZ1PartsOnShipList(boolean areQPZ1PartsOnShiplist)
	{
		Boolean bool = new Boolean(areQPZ1PartsOnShiplist);
		setField(QPZ1_PARTS_ON_SHIPLIST, bool);
	} //end setAreQPZ1PartsOnShiplist()

	/**
	 * Method setTestFixAnswerChanged.
	 * @param testFixAnswerChanged
	 */
	public void setTestFixAnswerChanged(boolean testFixAnswerChanged)
	{
		Boolean bool = new Boolean(testFixAnswerChanged);
		setField(TEST_FIX_ANSWER_CHANGED_FIELD, bool);
	}

	/**
	 * Method shouldHoldFromCume.
	 * @return boolean
	 * @throws FixException
	 */
	public boolean shouldHoldFromCume() throws FixException
	{
		Boolean bool = (Boolean) getField(HOLD_FROM_CUME_FIELD);
		if (bool == null)
		{
			String error = "The '" + HOLD_FROM_CUME_FIELD + "' flag was not set in the info object.  Unable to determine if this PTF should be held from the CUME Package.";
			throw new FixException(error);
		}
		return bool.booleanValue();
	}

	/**
	 * Method shouldHoldFromResave.
	 * @return boolean
	 * @throws FixException
	 */
	public boolean shouldHoldFromResave() throws FixException
	{
		Boolean bool = (Boolean) getField(HOLD_FROM_RESAVE_FIELD);
		if (bool == null)
		{
			String error = "The '" + HOLD_FROM_RESAVE_FIELD + "' flag was not set in the info object.  Unable to determine if this PTF should be held from the resave.";
			throw new FixException(error);
		}
		return bool.booleanValue();
	}

	public void setLockDisruptive(boolean lockDisruptive)
	{
		Boolean bool = new Boolean(lockDisruptive);
		setField(LOCK_DISRUPTIVE_FIELD, bool);
	} //end setLockDisruptive()

	public void clearRequestFields() throws FixException
	{
		HashMap hashMap = new HashMap();
		hashMap.put(DEBUG_ON_FIELD, new Boolean(isDebugOn()));
		hashMap.put(DEBUG_LEVEL_FIELD, new Integer(getDebugLevel()));
		hashMap.put(RUNNING_IN_SAME_JVM_FIELD, new Boolean(isRunningInSameJVM()));
		ReqFixAuthority authority = getAuthority();
		if (authority != null)
		{
			hashMap.put(REQFIX_AUTHORITY, authority);
		}
		MetadataUtilities metaDataConnection = getMetadataDatabaseConnection(false);
		if (metaDataConnection != null)
		{
			hashMap.put(PRF_DATABASE_FIELD, metaDataConnection);
		}
		PTFDBConnectionUtilities ptfDbConnection = getPTFDatabaseConnection(false);
		if (ptfDbConnection != null)
		{
			hashMap.put(PTF_DATABASE_FIELD, ptfDbConnection);
		}
		FixUser fixUser = getFixUser();
		if (fixUser != null)
		{
			hashMap.put(FIX_USER_FIELD, fixUser);
		}
		hashMap.put(REQFIX_UPDATE_FIELD, getReqFixUpdate());

		setInfo(hashMap);
	}

	public void setFullPathToXMLFile(String fullPathToXMLFile)
	{
		setField(FULL_PATH_TO_XML_FILE_FIELD, fullPathToXMLFile);
	} //end setFullPathToXMLFile()

	public void setSPEPtfOriginallyMappedTo(String spePtfOriginallyMappedTo)
	{
		setField(SPE_PTF_ORIGINALLY_MAPPED_TO, spePtfOriginallyMappedTo);
	} //end setSPEPtfOriginallyMappedTo()

	/**
	 * Method setFixUser
	 * @param fixUser
	 */
	public void setFixUser(FixUser fixUser)
	{
		setField(FIX_USER_FIELD, fixUser);
	}

	/**
	 * Method getFixUserWithoutAuthentication
	 * @return FixUser
	 */
	public FixUser getFixUserWithoutAuthentication()
	{
		FixUser fixUser = (FixUser) getField(FIX_USER_FIELD);
		if (fixUser == null)
		{
			fixUser = new FixUser();
			setFixUser(fixUser);
		}
		return fixUser;
	}

	/**
	 * Method getFixUser
	 * @return FixUser
	 */
	public FixUser getFixUser() throws FixException
	{
		FixUser fixUser = getFixUserWithoutAuthentication();
		fixUser.authenticateAction(PtfAuthority.REQUEST_PTF_ACTION);
		return fixUser;
	}

	/**
	 * Method getLogger
	 * @return Logger
	 */
	public Logger getLogger()
	{
		return getFixUserWithoutAuthentication().getLogger();
	}

	/**
	 * Method setVEFIXCCOptionForRequest
	 * @param veFixCCOption
	 */
	public void setVEFIXCCOptionForRequest(VEFIXCCOption veFixCCOption)
	{
		setField(VEFIXCCOPTION_FIELD, veFixCCOption);
	}

	/**
	 * Method getVEFIXCCOptionForRequest
	 * @return VEFIXCCOption
	 */
	public VEFIXCCOption getVEFIXCCOptionForRequest()
	{
		return (VEFIXCCOption) getField(VEFIXCCOPTION_FIELD);
	}

	public void setReasonForDisruptive(String reason)
	{
		setField(REASON_FOR_DISRUPTIVE_FIELD, reason);
	}

	public String getReasonForDisruptive()
	{
		return (String) getField(REASON_FOR_DISRUPTIVE_FIELD);
	}

	public void setReasonForDeferred(String reason)
	{
		setField(REASON_FOR_DEFERRED_FIELD, reason);
	}

	public String getReasonForDeferred()
	{
		return (String) getField(REASON_FOR_DEFERRED_FIELD);
	}

	public void setDescriptionOfExitProgramForActivation(String explanation)
	{
		setField(EXIT_PROGRAM_EXPLANATION_FOR_ACTIVATION, explanation);
	}

	public String getDescriptionOfExitProgramForActivation()
	{
		return (String) getField(EXIT_PROGRAM_EXPLANATION_FOR_ACTIVATION);
	}

	public void setDescriptionOfExitProgramForRemoval(String explanation)
	{
		setField(EXIT_PROGRAM_EXPLANATION_FOR_REMOVAL, explanation);
	}

	public String getDescriptionOfExitProgramForRemoval()
	{
		return (String) getField(EXIT_PROGRAM_EXPLANATION_FOR_REMOVAL);
	}

	public void setOperatingSystem(String operatingSystem)
	{
		setField(OPERATING_SYSTEM_FIELD, operatingSystem);
	}

	public String getOperatingSystem()
	{
		return (String) getField(OPERATING_SYSTEM_FIELD);
	}

	public void setSecondaryRequestInfo(Vector secondaryRequestInfoList)
	{
		setField(SECONDARY_REQUEST_INFO, secondaryRequestInfoList);
	}

	public void addSecondaryRequestInfo(ReqFixInfo info)
	{
		Vector secondaryRequestInfoList = getSecondaryRequestInfo();
		if (secondaryRequestInfoList == null)
		{
			secondaryRequestInfoList = new Vector();
		}
		secondaryRequestInfoList.addElement(info);
		setSecondaryRequestInfo(secondaryRequestInfoList);
	}

	public Vector getSecondaryRequestInfo()
	{
		return (Vector) getField(SECONDARY_REQUEST_INFO);
	}
	
	public ReqFixInfo getReqInfoByPlatform(String platformOs)
	{
	    ReqFixInfo reqInfo = null;
	    boolean found = false;
	    
	    Vector secondaryReqFixInfo = getSecondaryRequestInfo();
	    if (secondaryReqFixInfo == null)
	        return null;
	    
	    for (int i = 0; i < secondaryReqFixInfo.size(); i++)
	    {
	        reqInfo = (ReqFixInfo)secondaryReqFixInfo.get(i);
	        if (reqInfo.getOperatingSystem().equals(platformOs))
	        {
	            found = true;
	            break;
	        }
	    }
	    
	    if (found)
	        return reqInfo;
	    else
	        return null;
	}

	public void setPreInstallInstruction(PtfInstructions instructions)
	{
		setField(PRE_INSTALL_INSTRUCTIONS_FIELD, instructions);
	}

	public PtfInstructions getPreInstallInstructions()
	{
		return (PtfInstructions) getField(PRE_INSTALL_INSTRUCTIONS_FIELD);
	}

	public void setPostInstallInstruction(PtfInstructions instructions)
	{
		setField(POST_INSTALL_INSTRUCTIONS_FIELD, instructions);
	}

	public PtfInstructions getPostInstallInstructions()
	{
		return (PtfInstructions) getField(POST_INSTALL_INSTRUCTIONS_FIELD);
	}

	public void setPreUninstallInstruction(PtfInstructions instructions)
	{
		setField(PRE_UNINSTALL_INSTRUCTIONS_FIELD, instructions);
	}

	public PtfInstructions getPreUninstallInstructions()
	{
		return (PtfInstructions) getField(PRE_UNINSTALL_INSTRUCTIONS_FIELD);
	}

	public void setPostUninstallInstruction(PtfInstructions instructions)
	{
		setField(POST_UNINSTALL_INSTRUCTIONS_FIELD, instructions);
	}

	public PtfInstructions getPostUninstallInstructions()
	{
		return (PtfInstructions) getField(POST_UNINSTALL_INSTRUCTIONS_FIELD);
	}

	public void setGeneralInstruction(PtfInstructions instructions)
	{
		setField(GENERAL_INSTRUCTIONS_FIELD, instructions);
	}

	public PtfInstructions getGeneralInstructions()
	{
		return (PtfInstructions) getField(GENERAL_INSTRUCTIONS_FIELD);
	}

	public void setBuilderInstruction(PtfInstructions instructions)
	{
		setField(BUILDER_INSTRUCTIONS_FIELD, instructions);
	}

	public PtfInstructions getBuilderInstructions()
	{
		return (PtfInstructions) getField(BUILDER_INSTRUCTIONS_FIELD);
	}

	public void setExternalRequisites(Vector externalRequisites)
	{
		setField(EXTERNAL_REQUISITES_FIELD, externalRequisites);
	}

	public Vector getExternalRequisites()
	{
		return (Vector) getField(EXTERNAL_REQUISITES_FIELD);
	}

	public void setBuildType(String buildType)
	{
		setField(BUILD_TYPE_FIELD, buildType);
	}

	public String getBuildType()
	{
		return (String) getField(BUILD_TYPE_FIELD);
	}

	public boolean isDelayRemove() throws FixException
	{
		Boolean bool = (Boolean) getField(DELAY_REMOVE_FIELD);
		if (bool == null)
		{
			String error = "The '" + DELAY_REMOVE_FIELD + "' flag was not set in the info object.  Returning false by default.";
			return false;
		}
		return bool.booleanValue();
	} //end isDelayRemove()

	public void setIsDelayRemove(boolean isDelayRemove)
	{
		Boolean bool = new Boolean(isDelayRemove);
		setField(DELAY_REMOVE_FIELD, bool);
	} //end setIsDelayRemove()

	public void setIsClone(boolean isClone)
	{
		Boolean bool = new Boolean(isClone);
		setField(IS_CLONE_FIELD, bool);
	} //end setIsClone()

	public boolean isClone() throws FixException
	{
		Boolean bool = (Boolean) getField(IS_CLONE_FIELD);
		if (bool == null)
		{
			String error = "The '" + IS_CLONE_FIELD + "' flag was not set in the info object.  Returning false by default.";
			return false;
		}
		return bool.booleanValue();
	} //end isClone()

	public boolean isConcurrent()
	{
		String value = (String) getField(FIXPACK_APPLY_FIELD);
		if (value != null && value.trim().equalsIgnoreCase("N"))
		{
			//its concurrent..return true
			return true;
		} //end if
		else
		{
			//its not concurrent...return false
			return false;
		} //end else
	} //end isConcurrent()

	public boolean isdisruptive()
	{
		String value = (String) getField(FIXPACK_APPLY_FIELD);
		if (value != null && value.trim().equalsIgnoreCase("Y"))
		{
			//its disruptive..return true
			return true;
		} //end if
		else
		{
			//its not disruptive...return false
			return false;
		} //end else
	} //end isDisruptive()

	public boolean isDeferred()
	{
		String value = (String) getField(FIXPACK_APPLY_FIELD);
		if (value != null && value.trim().equalsIgnoreCase("D"))
		{
			//its deferred..return true
			return true;
		} //end if
		else
		{
			//its not deferred...return false
			return false;
		} //end else
	} //end isDeferred()

	public void setConcurrent()
	{
		setField(FIXPACK_APPLY_FIELD, PTFConstants.CONCURRENT_VALUE);
	} //end setConcurrent()

	public void setdisruptive()
	{
		setField(FIXPACK_APPLY_FIELD, PTFConstants.DISRUPTIVE_VALUE);
	} //end setDisruptive()

	public void setDeferred()
	{
		setField(FIXPACK_APPLY_FIELD, PTFConstants.DEFERRED_VALUE);
	} //end setDeferred()

	public void setEnableDeferredField(boolean enable)
	{
		Boolean bool = new Boolean(enable);
		setField(ENABLE_DEFERRED_APPLY_FIELD, bool);
	}

	public boolean getEnableDeferredField() throws FixException
	{
		Boolean bool = (Boolean) getField(ENABLE_DEFERRED_APPLY_FIELD);
		if (bool == null)
		{
			// The ENABLE_DEFERRED_APPLY_FIELD flag was not set in the info object.  Returning false by default.;
			return false;
		}
		return bool.booleanValue();
	}

	public void setEnableConcurrentField(boolean enable)
	{
		Boolean bool = new Boolean(enable);
		setField(ENABLE_CONCURRENT_APPLY_FIELD, bool);
	}

	public boolean getEnableConcurrentField() throws FixException
	{
		Boolean bool = (Boolean) getField(ENABLE_CONCURRENT_APPLY_FIELD);
		if (bool == null)
		{
			// The ENABLE_CONCURRENT_APPLY_FIELD flag was not set in the info object.  Returning false by default.;
			return false;
		}
		return bool.booleanValue();
	}
	
	public void setIsDistributable(boolean isDistributable)
	{
	    Boolean bool = new Boolean(isDistributable);
	    setField(IS_DISTRIBUTABLE_INITIATIVE_FIELD, bool);
	}
	
	public boolean getIsDistributable()
	{
	    Boolean bool = (Boolean) getField(IS_DISTRIBUTABLE_INITIATIVE_FIELD);
	    if (bool == null)
	        return false;	//return false by default
	    
	    return bool.booleanValue();
	}
	
	public void setOverrideBuildOrder(boolean overrideBuildOrder)
	{
	    Boolean bool = new Boolean(overrideBuildOrder);
	    setField(OVERRIDE_BUILD_ORDER_FIELD, bool);
	}
	
	public boolean getOverrideBuildOrder()
	{
	    Boolean bool = (Boolean) getField(OVERRIDE_BUILD_ORDER_FIELD);
	    if (bool == null)
	        return false;	//return false by default
	    
	    return bool.booleanValue();
	}
}