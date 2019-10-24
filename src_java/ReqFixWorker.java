/**
 * File:  ReqFixWorker.java
 * @author Aaron Hislop, ahislop@us.ibm.com
 * Date:  Jun 18, 2003
 */
package com.ibm.sdwb.cps.reqfix.client;

import java.util.HashMap;
import java.util.Vector;

import com.ibm.bst.fixcommon.FixUser;
import com.ibm.bst.fixcommon.exceptions.FixException;
import com.ibm.dv2Db.ptfDb.ifObjects.FixPeriod;
import com.ibm.dv2Db.ptfDb.ifObjects.Option;
import com.ibm.dv2Db.ptfDb.ifObjects.Ptf;
import com.ibm.dv2Db.ptfDb.ifObjects.PtfAssociation;
import com.ibm.dv2Db.ptfDb.ifObjects.PtfExitProgram;
import com.ibm.dv2Db.ptfDb.ifObjects.PtfInstructions;
import com.ibm.dv2Db.ptfDb.ifObjects.PtfReqRequisite;
import com.ibm.dv2Db.ptfDb.ifObjects.VEContent;
import com.ibm.dv2Db.ptfDb.ifObjects.VEFIXCCOption;
import com.ibm.sdwb.cps.reqfix.AddTrackInfo;
import com.ibm.sdwb.cps.reqfix.FirmwareApplyTypeInfo;
import com.ibm.sdwb.cps.reqfix.MetadataUtilities;
import com.ibm.sdwb.cps.reqfix.PTFConstants;
import com.ibm.sdwb.cps.reqfix.PTFDBConnectionUtilities;
import com.ibm.sdwb.cps.reqfix.ProductAndReleaseInfo;
import com.ibm.sdwb.cps.reqfix.RemovePtfAssociationInfo;
import com.ibm.sdwb.cps.reqfix.ReqFixInfo;
import com.ibm.sdwb.cps.reqfix.ReqFixWarning;
import com.ibm.sdwb.cps.reqfix.RequestProcessDriver;
import com.ibm.sdwb.cps.reqfix.TrackUtilities;
import com.ibm.sdwb.cpscommon.FixDatabase;
import com.ibm.sdwb.cpscommon.metadata.Release;
import com.ibm.sdwb.cpscommon.metadata.ReleaseException;
import com.ibm.sdwb.cpscommon.supersede.FixSupersedeManager;
import com.ibm.sdwb.cpscommon.supersede.LinearSupersedeChain;
import com.ibm.sdwb.cpscommon.ve.IVEConst;
import com.ibm.sdwb.cpscommon.ve.VEUtil;

/* 
 * Change Activity:
 * 
 * PTR     	Date     		Pgmr ID  	Change Description
 * --------	-------- 		---------	----------------------------------
 * PDV06324	Jun 18, 2003	ahislop		Initial creation of the file
 */
public class ReqFixWorker implements IReqFixWorker
{
	private RequestProcessDriver processDriver = null;
	private boolean skipExistingComponentPtfCheck = false;

	public ReqFixWorker(FixUser fixUser)
	{
		this.processDriver = new RequestProcessDriver(fixUser);
	}

	//begin code added by MHD; DV07801
	public RemovePtfAssociationInfo removeUnspecifiedTracksFromRequest(ReqFixInfo info, Vector tracksUserExplicitlyAskedFor) throws FixException
	{
		if (info == null)
		{
			throw new FixException("Unable to remove unspecified tracks from the request. The info object was null.");
		} //end if
		info.getLogger().finer("removeUnspecifiedTracksFromRequest(" + info + ", " + tracksUserExplicitlyAskedFor + ")");
		return processDriver.removeUnspecifiedTracksFromRequest(info, tracksUserExplicitlyAskedFor);
	} //end 
	//end code added by MHD; DV07801

	public AddTrackInfo getAddTrackInfo(ReqFixInfo info, PtfAssociation ptfAssociation) throws FixException
	{
		info.getLogger().finer("addTracks(" + info + ", " + ptfAssociation + ")");
		String processType = info.getProcessType();
		if (processType.equals(TRADITIONAL_ISERIES_PTF_PROCESS_TYPE) || processType.equals(ESERVER_ISERIES_FIX_PROCESS_TYPE))
		{
			return processDriver.addNewTracks(info, new AddTrackInfo(), ptfAssociation);
		}
		else if (processType.equals(BLD_STAGE_LID_BLD_PROCESS_TYPE))
		{
			return processDriver.addNewTracksForBldStageLID(info, new AddTrackInfo(), ptfAssociation);
		}
		else if (processType.equals(LID_BASED_ISERIES_PTF_PROCESS_TYPE) || processType.equals(LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE))
		{
			return processDriver.addNewTracksForPackagePTF(info, new AddTrackInfo(), ptfAssociation);
		}
		else if (processType.equals(TRADITIONAL_CUE_BLD_PROCESS_TYPE))
		{
			return processDriver.addNewTracks(info, new AddTrackInfo(), ptfAssociation);
		}
		else if (processType.equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
		{
			return processDriver.addNewTracks(info, new AddTrackInfo(), ptfAssociation);
		}
		else if (processType.equals(VE1_COMMON_FIX_PROCESS_TYPE))
		{
			return processDriver.addNewTracks(info, new AddTrackInfo(), ptfAssociation);
		}
		else if (processType.equals(VE_COMMON_FIX_PROCESS_TYPE)
                 || processType.equals(BBP_PROCESS_TYPE))
		{
			String error =
				"Process type '"
					+ processType
					+ "' does not support adding tracks by a single track.  Unable to add tracks for product '"
					+ ptfAssociation.getPtrProduct()
					+ "', '"
					+ ptfAssociation.getRelease()
					+ "', problem id '"
					+ ptfAssociation.getPtrName()
					+ "' and NLV '"
					+ ptfAssociation.getFeatureCode()
					+ "'.";
			throw new FixException(error);
			//return processDriver.addNewTracks(info, new AddTrackInfo(), ptfAssociation);
		}
		else if (processType.equals(FSP_PROCESS_TYPE))
		{
			return processDriver.addNewTracks(info, new AddTrackInfo(), ptfAssociation);
		}
		else if (processType.equals(PFW_PROCESS_TYPE))
		{
			return processDriver.addNewTracks(info, new AddTrackInfo(), ptfAssociation);
		}
		else if (processType.equals(GFW_LPP_PROCESS_TYPE)
                 || processType.equals(GFW_LPP_HIGH_PROCESS_TYPE))
        {
            // This should map to the FSP process
            return processDriver.addNewTracks(info, new AddTrackInfo(), ptfAssociation);
        }
		else
		{
			String error = "An unsupported process type '" + processType + "' is set in the info object. Unable to add track.";
			throw new FixException(error);
		}
	}

	public AddTrackInfo getAddTrackInfo(ReqFixInfo info, String fixName) throws FixException
	{
		info.getLogger().finer("addTracks(" + info + ", " + fixName + ")");

		String processType = info.getProcessType();
		if (processType.equals(TRADITIONAL_ISERIES_PTF_PROCESS_TYPE) || processType.equals(ESERVER_ISERIES_FIX_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixName);
		}
		else if (processType.equals(BLD_STAGE_LID_BLD_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracksForBldStageLID(info, new AddTrackInfo(), fixName);
		}
		else if (processType.equals(LID_BASED_ISERIES_PTF_PROCESS_TYPE) || processType.equals(LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE))
		{
			return processDriver.addNewTracksForPackagePTF(info, new AddTrackInfo(), fixName);
		}
		else if (processType.equals(TRADITIONAL_CUE_BLD_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixName);
		}
		else if (processType.equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixName);
		}
		else if (processType.equals(VE_COMMON_FIX_PROCESS_TYPE)
                 || processType.equals(VE1_COMMON_FIX_PROCESS_TYPE)
                 || processType.equals(BBP_PROCESS_TYPE))
		{
			String error = "Process type '" + processType + "' does not support adding tracks by fix name.  Unable to add tracks for the fix '" + fixName + "'.";
			throw new FixException(error);
		}
		else if (processType.equals(FSP_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixName);
		}
		else if (processType.equals(PFW_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixName);
		}
		else if (processType.equals(GFW_LPP_PROCESS_TYPE)
                 || processType.equals(GFW_LPP_HIGH_PROCESS_TYPE))
        {
            // these flows should map to match the FSP process...much like the PFW process maps to it
            return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixName);
        }
		else
		{
			String error = "An unsupported process type '" + processType + "' is set in the info object. Unable to add tracks for the fix name '" + fixName + "'.";
			throw new FixException(error);
		}
	}

	public AddTrackInfo getAddTrackInfo(ReqFixInfo info, Ptf fixObject) throws FixException
	{
		info.getLogger().finer("addTracks(" + info + ", " + fixObject + ")");
		String processType = info.getProcessType();
		if (processType.equals(TRADITIONAL_ISERIES_PTF_PROCESS_TYPE) || processType.equals(ESERVER_ISERIES_FIX_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixObject);
		}
		else if (processType.equals(BLD_STAGE_LID_BLD_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracksForBldStageLID(info, new AddTrackInfo(), fixObject);
		}
		else if (processType.equals(LID_BASED_ISERIES_PTF_PROCESS_TYPE) || processType.equals(LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE))
		{
			return processDriver.addNewTracksForPackagePTF(info, new AddTrackInfo(), fixObject);
		}
		else if (processType.equals(TRADITIONAL_CUE_BLD_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixObject);
		}
		else if (processType.equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixObject);
		}
		else if (processType.equals(VE_COMMON_FIX_PROCESS_TYPE)
                 || processType.equals(VE1_COMMON_FIX_PROCESS_TYPE)
                 || processType.equals(BBP_PROCESS_TYPE))
		{
			String error = "Process type '" + processType + "' does not support adding tracks by fix name.  Unable to add tracks for the fix '" + fixObject.getName() + "'.";
			throw new FixException(error);
		}
		else if (processType.equals(FSP_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixObject);
		}
		else if (processType.equals(PFW_PROCESS_TYPE))
		{
			return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixObject);
		}
		else if (processType.equals(GFW_LPP_PROCESS_TYPE)
                 || processType.equals(GFW_LPP_HIGH_PROCESS_TYPE))
        {
            // This should Map to a similar flow as the FSP process
            return this.processDriver.addNewTracks(info, new AddTrackInfo(), fixObject);
        }
		else
		{
			String error = "An unsupported process type '" + processType + "' is set in the info object. Unable to add tracks for the fix '" + fixObject.getName() + "'.";
			throw new FixException(error);
		}
	}

	public AddTrackInfo getAddTrackInfo(ReqFixInfo info, Vector ptfAssociationList) throws FixException
	{
		info.getLogger().finer("addTracks(" + info + ", " + ptfAssociationList + ")");
		String processType = info.getProcessType();
		if (processType.equals(LID_BASED_ISERIES_PTF_PROCESS_TYPE) || processType.equals(LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE))
		{
			return processDriver.addNewApprovedTracksToPackagePTF(info, new AddTrackInfo(), ptfAssociationList);
		}
		else if (processType.equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
		{
			return processDriver.addEligibleTracksToEServerCommonPTF(info, new AddTrackInfo(), ptfAssociationList);
		}
		else if (processType.equals(VE_COMMON_FIX_PROCESS_TYPE)
                 || processType.equals(BBP_PROCESS_TYPE))
		{
			return processDriver.addEligibleTracksToEServerCommonPTF(info, new AddTrackInfo(), ptfAssociationList);
		}
		else if (processType.equals(VE1_COMMON_FIX_PROCESS_TYPE))
		{
			return processDriver.addEligibleTracksToEServerCommonPTF(info, new AddTrackInfo(), ptfAssociationList);
		}
		else
		{
			String error = "Adding a list of selected track objects is not supported for the process type '" + processType + "'.";
			throw new FixException(error);
		}
	}

	public Vector getPotentialOwningPtfs(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("setOwningPtf(" + info + ")");
		return processDriver.getOwningPTFForRequest(info);
	}

	public boolean shouldSkipExiistingComponentCheck()
	{
		return this.skipExistingComponentPtfCheck;
	}

	public void processTrackInformation(ReqFixInfo info, WarningIfc warningIfc) throws FixException, ReqFixWarning
	{
		info.getLogger().finer("processTrackInformation(" + info + ")");
		skipExistingComponentPtfCheck = false;
		String processType = info.getProcessType();
		if (processType.equals(TRADITIONAL_ISERIES_PTF_PROCESS_TYPE) || processType.equals(ESERVER_ISERIES_FIX_PROCESS_TYPE))
		{
			this.processDriver.processTrackInformation(info);
		}
		else if (processType.equals(LID_BASED_ISERIES_PTF_PROCESS_TYPE)
                 || processType.equals(FSP_PROCESS_TYPE)
                 || processType.equals(PFW_PROCESS_TYPE)
                 || processType.equals(LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE)
                 || processType.equals(GFW_LPP_PROCESS_TYPE)
                 || processType.equals(GFW_LPP_HIGH_PROCESS_TYPE))
		{
			try
			{
				this.processDriver.processTrackInformationForFixPackComponentPTF(info);
			}
			catch (ReqFixWarning warning)
			{
				if (!warningIfc.continueProcessingAfterWarning(warning))
				{
					// Rollback anything that was not committed
					info.getPTFDatabaseConnection().rollback();
					String error = "Unable to continue the request.  The user selected to abort the request after the warning [" + warning.getMessage() + "] was issued.";
					throw new ReqFixWarning(error, true);
				}
				else
				{
					skipExistingComponentPtfCheck = true;
				}
			}
		}
		else if (processType.equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
		{
			this.processDriver.processTrackInformationForEServerCommonCode(info);
		}
		else if (processType.equals(VE1_COMMON_FIX_PROCESS_TYPE))
		{
			this.processDriver.processTrackInformationForEServerCommonCode(info);
		}
		else if (processType.equals(VE_COMMON_FIX_PROCESS_TYPE))
		{
			this.processDriver.processTrackInformationForEServerCommonCode(info);
		}
		else if (processType.equals(BBP_PROCESS_TYPE))
		{
		    this.processDriver.processTrackInformationForEServerCommonCode(info);
		}
		else if (processType.equals(BLD_STAGE_LID_BLD_PROCESS_TYPE))
		{
			try
			{
				this.processDriver.processTrackInformationForPhypBld(info);
			}
			catch (ReqFixWarning warning)
			{
				if (!warningIfc.continueProcessingAfterWarning(warning))
				{
					// Rollback anything that was not committed
					info.getPTFDatabaseConnection().rollback();
					String error = "Unable to continue the request.  The user selected to abort the request after the warning [" + warning.getMessage() + "] was issued.";
					throw new ReqFixWarning(error, true);
				}
			}
		}
		else if (processType.equals(TRADITIONAL_CUE_BLD_PROCESS_TYPE))
		{
			this.processDriver.processTrackInformationForBldStageCUE(info);
		}
		else
		{
			String error = "An unsupported process type '" + processType + "' is set in the info object. Unable to process track information.";
			throw new FixException(error);
		}

		//If there are tracks on the request that are not reprocessed yet, but do have library parts on them,
		//then we need to verify that there is at least one part on the request that is not reprocessed and does
		//have a shiplist...if all of the tracks that are not reprocessed have empty shiplists, then reprocessing will
		//get skipped, and the library parts on those tracks will not get compiled...which would be bad 
		this.processDriver.verifyAtLeastOneNonReproTrackWithPartsOnItHasShiplist(info);
	}

	public ReqFixInfo submitRequestAction(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("submitRequestAction(" + info + ")");
		return info;
	}
	
	/**
	 * Method getProcessType
	 * @param info
	 * @param ptfAssociation
	 * @param selectedRequestType
	 * @return String
	 * @throws FixException
	 */
	public String getProcessType(ReqFixInfo info, PtfAssociation ptfAssociation, String selectedRequestType) throws FixException
	{
		info.getLogger().finer("getProcessType(" + info + ", " + ptfAssociation + ", " + selectedRequestType + ")");
		return getProcessType(info, ptfAssociation.getPtrProduct(), ptfAssociation.getRelease(), selectedRequestType);
	}

	/**
	 * Method getValidTabs
	 * @param info
	 * @return String[]
	 * @throws FixException 
	 */
	public String[] getValidTabs(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("getValidTabs()");
		String processType = info.getProcessType();
		if (processType.equals(TRADITIONAL_ISERIES_PTF_PROCESS_TYPE) || processType.equals(ESERVER_ISERIES_FIX_PROCESS_TYPE))
		{
			if (info.getRequestedFeatureCode().equals("2924"))
			{
				return ISERIES_TABS_WITH_NLV;
			}
			else
			{
				return ISERIES_TABS_WITHOUT_NLV;
			}
		}
		else if (processType.equals(BLD_STAGE_LID_BLD_PROCESS_TYPE))
		{
			return PHYP_BLD_STAGE_TABS;
		}
		else if (processType.equals(LID_BASED_ISERIES_PTF_PROCESS_TYPE))
		{
			return PHYP_ISERIES_TABS;
		}
		else if (processType.equals(LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE))
		{
		    return PHYP_ISERIES_TABS;
		}
		else if (processType.equals(TRADITIONAL_CUE_BLD_PROCESS_TYPE))
		{
			return CUE_TABS;
		}
		else if (processType.equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
		{
			return ESERVER_TABS;
		}
		else if (processType.equals(VE_COMMON_FIX_PROCESS_TYPE)
                 || processType.equals(VE1_COMMON_FIX_PROCESS_TYPE)
                 || processType.equals(BBP_PROCESS_TYPE))
		{
			return VE_TABS;
		}
		else if (processType.equals(FSP_PROCESS_TYPE))
		{
			return PFW_FSP_ISERIES_TABS;
		}
		else if (processType.equals(PFW_PROCESS_TYPE))
		{
			return PFW_FSP_ISERIES_TABS;
		}
		else if (processType.equals(GFW_LPP_PROCESS_TYPE)
                 || processType.equals(GFW_LPP_HIGH_PROCESS_TYPE))
        {
            return PFW_FSP_ISERIES_TABS;
        }
		else
		{
			String error = "An unsupported process type '" + processType + "' is set in the info object. Unable to format the tabs.";
			throw new FixException(error);
		}
	}

	/**
	 * Method getProcessType
	 * @param info
	 * @param product
	 * @param release
	 * @param selectedRequestType
	 * @return String
	 * @throws FixException
	 */
	public String getProcessType(ReqFixInfo info, String product, String release, String selectedRequestType) throws FixException
	{
		info.getLogger().finer("getProcessType(" + info + ", " + product + ", " + release + ", " + selectedRequestType + ")");
		String processGroup = info.getMetadataDatabaseConnection().getRequestProcessGroup(product, release);
		if (processGroup == null || processGroup.trim().equals(""))
		{
			String error = "Unable to determine the process group for product '" + product + "' and release '" + release + "'.";
			throw new FixException(error);
		}
		
		if (selectedRequestType.equals(REQUEST_ISERIES_PTF))
        {
            // Requesting an iSeries PTF
            if (processGroup.equals(DEFAULT_PROCESS_GROUP))
            {
                return TRADITIONAL_ISERIES_PTF_PROCESS_TYPE;
            }
            else if (processGroup.equals(PHYP_BASED_GROUP))
            {
                return LID_BASED_ISERIES_PTF_PROCESS_TYPE;
            }
            else if (processGroup.equals(PHYP_LPP_BASED_GROUP))
            {
                return LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE;
            }
            else if (processGroup.equals(PFW_BASED_GROUP))
            {
                return PFW_PROCESS_TYPE;
            }
            else if (processGroup.equals(FSP_BASED_GROUP))
            {
                return FSP_PROCESS_TYPE;
            }
            else if (processGroup.equals(ESERVER_ISERIES_GROUP))
            {
                return ESERVER_ISERIES_FIX_PROCESS_TYPE;
            }
            else if (processGroup.equals(LPP_BASED_GROUP))
            {
                return GFW_LPP_PROCESS_TYPE;
            }
            else if (processGroup.equals(LPP_HIGH_BASED_GROUP))
            {
                return GFW_LPP_HIGH_PROCESS_TYPE;
            }
        }
		else
		{
			// Requesting a bld stage build
			if (processGroup.equals(DEFAULT_PROCESS_GROUP) || processGroup.equals(PFW_BASED_GROUP))
			{
				return TRADITIONAL_CUE_BLD_PROCESS_TYPE;
			}
			else if (processGroup.equals(PHYP_BASED_GROUP)
                     || processGroup.equals(PHYP_LPP_BASED_GROUP))
            {
                return BLD_STAGE_LID_BLD_PROCESS_TYPE;
            }
			else if (processGroup.equals(ESERVER_BASED_GROUP))
			{
				return ESERVER_COMMON_FIX_PROCESS_TYPE;
			}
			else if (processGroup.equals(VE_BASED_GROUP))
			{
				// This is a VE Group.   Since cmvc releases span VE1 and VE we must check to see if we are a VE1 or a VE type
				// Get the initiative from the info object
				VEFIXCCOption initiative = info.getVEFIXCCOptionForRequest();
				// Get the fixPeriod for the initiative
				FixPeriod fixPeriod = info.getPTFDatabaseConnection().getFixPeriodById(initiative.getFixPeriodID());
				// Check to see if the fixPeriod is for VE1
				if (VEUtil.isFixPeriodForVE1(fixPeriod.getName()))
				{
					// FixPeriod is for VE1 return that process type
					return VE1_COMMON_FIX_PROCESS_TYPE;
				}
				// FixPeriod is for the normal VE flow. 
				return VE_COMMON_FIX_PROCESS_TYPE;
			}
			else if (processGroup.equals(VE1_BASED_GROUP))
			{
				return VE1_COMMON_FIX_PROCESS_TYPE;
			}
			else if (processGroup.equals(BBP_BASED_GROUP))
			{
			    return BBP_PROCESS_TYPE;
			}
		}
		String error = "Unable to determine the process type for product '" + product + "', release '" + release + "' and request type '" + selectedRequestType + "'.";
		throw new FixException(error);
	}

	/**
	 * Method getProcessType
	 * @param info
	 * @param fixName
	 * @return String
	 * @throws FixException
	 */
	public String getProcessType(ReqFixInfo info, String fixName) throws FixException
	{
		info.getLogger().finer("getProcessType(" + info + ", " + fixName + ")");
		return getProcessType(info, info.getPTFDatabaseConnection().getPtf(fixName));
	}

	/**
	 * Method getProcessType
	 * @param info
	 * @param ptfObj
	 * @return String
	 * @throws FixException
	 */
	public String getProcessType(ReqFixInfo info, Ptf ptfObj) throws FixException
	{
		info.getLogger().finer("getProcessType(" + info + ", " + ptfObj + ")");
		if (ptfObj == null)
		{
			String error = "Unable to get the Process Type.  The passed in Fix object was null.";
			throw new FixException(error);
		}
		String type = ptfObj.getType();
		String subtype = ptfObj.getSubType();

		if (type.equals(Ptf.BLD_TYPE))
		{
			if (subtype.equals(Ptf.SUBTYPE_PHYP) || (subtype.equals(Ptf.SUBTYPE_PHYP_LPP)))
            {
                return BLD_STAGE_LID_BLD_PROCESS_TYPE;
            }
			else if (subtype.equals(Ptf.SUBTYPE_ESERVER))
			{
				return ESERVER_COMMON_FIX_PROCESS_TYPE;
			}
			else if (subtype.equals(Ptf.SUBTYPE_VE))
			{
				return VE_COMMON_FIX_PROCESS_TYPE;
			}
			else if (subtype.equals(Ptf.SUBTYPE_BBP1))
			{
			    return BBP_PROCESS_TYPE;
			}
			else if (subtype.equals(Ptf.SUBTYPE_VE1))
			{
				return VE1_COMMON_FIX_PROCESS_TYPE;
			}
		}
		else if (type.equals(Ptf.CUE_TYPE))
		{
			return TRADITIONAL_CUE_BLD_PROCESS_TYPE;
		}
		else // should be either PTE or final
		{
			if (subtype.equals(Ptf.SUBTYPE_ISERIES))
            {
                return TRADITIONAL_ISERIES_PTF_PROCESS_TYPE;
            }
            else if (subtype.equals(Ptf.SUBTYPE_PHYP))
            {
                return LID_BASED_ISERIES_PTF_PROCESS_TYPE;
            }
            else if (subtype.equals(Ptf.SUBTYPE_PHYP_LPP))
            {
                return LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE;
            }
			else if (subtype.equals(Ptf.SUBTYPE_PFW))
			{
				return PFW_PROCESS_TYPE;
			}
			else if (subtype.equals(Ptf.SUBTYPE_FSP) || subtype.equals(Ptf.SUBTYPE_MARKER))
			{
				return FSP_PROCESS_TYPE;
			}
			else if (subtype.equals(Ptf.SUBTYPE_ESERVER))
			{
				return ESERVER_ISERIES_FIX_PROCESS_TYPE;
			}
			else if (subtype.equals(Ptf.SUBTYPE_GFW_LPP))
			{
			    return GFW_LPP_PROCESS_TYPE;
			}
			else if (subtype.equals(Ptf.SUBTYPE_GFW_LPP_HIGH))
			{
			    return GFW_LPP_HIGH_PROCESS_TYPE;
			}
		}
		String error = "Unable to map the type/subtype [" + type + "/" + subtype + "] to a process. Please report this error via DEV2000 Central.";
		throw new FixException(error);
	}

	public String getProcessType(ReqFixInfo info, VEFIXCCOption veFixCCOption) throws FixException
	{
		info.getLogger().finer("getProcessType(" + info + ", " + veFixCCOption + ")");
		Option ccOption = info.getPTFDatabaseConnection().getOptionByID(veFixCCOption.getCCOptionID());
		return getProcessType(info, ccOption.getLibraryProduct(), ccOption.getLibraryRelease(), REQUEST_BLD_STAGE_BUILD);
	}

	/**
	 * Method getApprovedTracks
	 * @param info
	 * @param product
	 * @param release
	 * @return Vector of PtfAssociation objects
	 * @throws FixException 
	 */
	public Vector getApprovedTracks(ReqFixInfo info, String product, String release) throws FixException
	{
		info.getLogger().finer("getApprovedTracks(" + info + ", " + product + ", " + release + ")");
		return this.processDriver.getApprovedTracksForRequest(info.getPTFDatabaseConnection(), info.getMetadataDatabaseConnection(), product, release, info.getPtfAssociationsOnRequest(), true);
	}

	/**
	 * Method determineTracksToRemoveFromPackagePTF
	 * @param info
	 * @param selectedPtfAssociationList
	 * @return Vector
	 * @throws FixException 
	 */
	public Vector determineTracksToRemoveFromPackagePTF(ReqFixInfo info, Vector selectedPtfAssociationList) throws FixException
	{
		info.getLogger().finer("determineTracksToRemoveFromPackagePTF(" + info + ", " + selectedPtfAssociationList + ")");
		return this.processDriver.determineTracksToRemoveFromPackagePTF(info.getPTFDatabaseConnection(), selectedPtfAssociationList, info.getPtfAssociationsOnRequest());
	}

	/**
	 * Method removePtfAssociationsFromRequest
	 * @param info
	 * @param selectedPtfAssociationList
	 * @return RemovePtfAssociationInfo
	 * @throws FixException 
	 */
	public RemovePtfAssociationInfo removePtfAssociationsFromRequest(ReqFixInfo info, Vector selectedPtfAssociationList) throws FixException
	{
		info.getLogger().finer("removePtfAssociationsFromRequest(" + info + ", " + selectedPtfAssociationList + ")");
		return processDriver.removePtfAssociationsFromRequest(info.getPtfAssociationsOnRequest(), selectedPtfAssociationList, new RemovePtfAssociationInfo());
	}

	/**
	 * Method setAttributeInformation
	 * @param info
	 * @throws FixException 
	 */
	public void setAttributeInformation(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("setAttributeInformation(" + info + ")");
		String processType = info.getProcessType();
		if (processType.equals(TRADITIONAL_ISERIES_PTF_PROCESS_TYPE) || processType.equals(ESERVER_ISERIES_FIX_PROCESS_TYPE))
		{
			processDriver.getAttributesInformationForISeriesPTF(info);
		}
		else if (processType.equals(LID_BASED_ISERIES_PTF_PROCESS_TYPE)
                 || processType.equals(LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE))
		{
			processDriver.getAttributesInformationForPackagePTF(info);
		}
		else if (processType.equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
		{
			// No logic needed.  			
		}
		else if (processType.equals(VE_COMMON_FIX_PROCESS_TYPE))
		{
			// No logic needed.  			
		}
		else if (processType.equals(VE1_COMMON_FIX_PROCESS_TYPE))
		{
			// No logic needed.  			
		}
		else if (processType.equals(BBP_PROCESS_TYPE))
		{
		    // No logic needed.
		}
		else if (processType.equals(FSP_PROCESS_TYPE)
                 || processType.equals(PFW_PROCESS_TYPE)
                 || processType.equals(GFW_LPP_PROCESS_TYPE)
                 || processType.equals(GFW_LPP_HIGH_PROCESS_TYPE))
		{
			processDriver.getAttributesInformationForNonPhypComponent(info);
		}
	}

	/**
	 * Method getRequestedRequisites
	 * @param info
	 * @return ReqFixInfo
	 * @throws FixException 
	 */
	public ReqFixInfo getRequestedRequisites(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("getRequestedRequisites(" + info + ")");
		return this.processDriver.getRequestedRequisites(info);
	}

	/**
	 * Method getExitPrograms
	 * @param info
	 * @return ReqFixInfo
	 * @throws FixException 
	 */
	public ReqFixInfo getExitPrograms(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("getExitPrograms(" + info + ")");
		return this.processDriver.getExitPrograms(info);
	}

	/**
	 * Method getSpecialInstructionsInformation
	 * @param info
	 * @return ReqFixInfo
	 * @throws FixException 
	 */
	public ReqFixInfo getSpecialInstructionsInformation(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("getSpecialInstructionsInformation(" + info + ")");
		return this.processDriver.getSpecialInstructionsInformation(info);
	}

	/**
	 * Method getPreAndPostBuildInstructions
	 * @param info
	 * @return ReqFixInfo
	 * @throws FixException 
	 */
	public ReqFixInfo getPreAndPostBuildInstructions(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("getPreAndPostBuildInstructions(" + info + ")");
		return this.processDriver.getPreAndPostBuildInstructions(info);
	}

	/**
	 * Method getEventHandlers
	 * @param info
	 * @return ReqFixInfo
	 * @throws FixException 
	 */
	public ReqFixInfo getEventHandlers(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("getEventHandlers(" + info + ")");
		return processDriver.getEventHandlers(info);
	}

	public void requestAction(ReqFixInfo info, WarningIfc warningIfc) throws FixException, ReqFixWarning
	{
		info.getLogger().finer("requestAction(" + info + ", " + warningIfc + ")");
		try
		{
			processAttributesInformation(info);
			String processType = info.getProcessType();
			if (processType.equals(TRADITIONAL_ISERIES_PTF_PROCESS_TYPE)
                || processType.equals(ESERVER_ISERIES_FIX_PROCESS_TYPE))
			{
				requestISeriesAction(info, warningIfc);
			}
			else if (processType.equals(LID_BASED_ISERIES_PTF_PROCESS_TYPE)
                     || processType.equals(LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE))
			{
				requestPhypISeriesAction(info, warningIfc);
			}
			else if (processType.equals(BLD_STAGE_LID_BLD_PROCESS_TYPE))
			{
				requestPhypBldAction(info, warningIfc);
			}
			else if (processType.equals(TRADITIONAL_CUE_BLD_PROCESS_TYPE))
			{
				requestCUEAction(info, warningIfc);
			}
			else if (processType.equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
			{
				requestEServerCommonAction(info, warningIfc);
			}
			else if (processType.equals(VE_COMMON_FIX_PROCESS_TYPE)
                     || processType.equals(BBP_PROCESS_TYPE))
			{
				requestVECommonAction(info, warningIfc);
			}
			else if (processType.equals(VE1_COMMON_FIX_PROCESS_TYPE))
			{
				requestVE1CommonAction(info, warningIfc);
			}
			else if (processType.equals(FSP_PROCESS_TYPE)
                     || processType.equals(PFW_PROCESS_TYPE)
                     || processType.equals(GFW_LPP_HIGH_PROCESS_TYPE)
                     || processType.equals(GFW_LPP_PROCESS_TYPE))
			{
				requestNonPhypComponentAction(info, warningIfc);
			}
			else
			{
				String error = "An unsupported processType '" + processType + "' is set in the info object. Unable to submit request.";
				throw new FixException(error);
			}
		}
		catch (FixException fe)
		{
			try
			{
				info.getPTFDatabaseConnection().rollback();
			}
			catch (FixException re)
			{
				re.printStackTrace();
			}
			throw fe;
		}
	}

	/**
	 * Method processAttributesInformation
	 * @param info
	 * @throws FixException 
	 */
	public void processAttributesInformation(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("processAttributesInformation()");
		info.getReqFixUpdate().updateUI(1, "Processing fix attribute information");
		this.processDriver.processAttributesInformation(info);
	}

	/**
	 * Method requestISeriesAction
	 * @param info
	 * @param warningIfc
	 * @throws FixException 
	 */
	public void requestISeriesAction(ReqFixInfo info, WarningIfc warningIfc) throws FixException, ReqFixWarning
	{
		info.getLogger().finer("requestISeriesAction(" + info + ")");
		try
		{
			this.processDriver.doInitialFinalProcessingForISeries(info);
		}
		catch (ReqFixWarning warning)
		{
			if (!warningIfc.continueProcessingAfterWarning(warning))
			{
				// Rollback anything that was not committed
				info.getPTFDatabaseConnection().rollback();
				String error = "Unable to continue the request.  The user selected to abort the request after the warning [" + warning.getMessage() + "] was issued.";
				throw new ReqFixWarning(error, true);
			}
		}
		this.processDriver.doRemainingFinalProcessingForISeries(info);
	}

	/**
	 * Method requestPhypISeriesAction
	 * @param info
	 * @param warningIfc
	 * @throws FixException 
	 */
	public void requestPhypISeriesAction(ReqFixInfo info, WarningIfc warningIfc) throws FixException
	{
		info.getLogger().finer("requestPhypISeriesAction(" + info + ", " + warningIfc + ")");
		try
		{
			//do the final processing for the explicitly requested fix
			this.processDriver.doFinalProcessingForPackagePTF(info);

			//do whatever processing is needed for any clones
			this.processDriver.doCloning(info);
			
			//The call to BA was moved here, and taken out of the 'doFinalProcessing()' method, so that we can be
			//assured that it will run after the cloning stuff has completed.
			this.processDriver.scheduleBuildAutomation(info, false);
		}
		catch (ReqFixWarning rfw)
		{
			// Rollback anything that was not committed
			info.getPTFDatabaseConnection().rollback();
			String error = rfw.getMessage();
			throw new FixException(error);
		}
	}

	/**
	 * Method requestPhypBldAction
	 * @param info
	 * @param warningIfc
	 * @throws FixException 
	 */
	public void requestPhypBldAction(ReqFixInfo info, WarningIfc warningIfc) throws FixException, ReqFixWarning
	{
		info.getLogger().finer("doFinalProcessing()");
		info.getReqFixUpdate().updateUI(1, "Performing initial processing");
		try
		{
			this.processDriver.doInitialFinalProcessingForBldStageLID(info);
		}
		catch (ReqFixWarning warning)
		{
			if (!warningIfc.continueProcessingAfterWarning(warning))
			{
				// Rollback anything that was not committed
				info.getPTFDatabaseConnection().rollback();
				String error = "Unable to continue the request.  The user selected to abort the request after the warning [" + warning.getMessage() + "] was issued.";
				throw new ReqFixWarning(error, true);
			}
		}
		info.getReqFixUpdate().updateUI(1, "Performing middle processing");
		try
		{
			this.processDriver.doMiddleFinalProcessingForBldStageLid(info);
		}
		catch (ReqFixWarning warning)
		{
			if (!warningIfc.continueProcessingAfterWarning(warning))
			{
				// Rollback anything that was not committed
				info.getPTFDatabaseConnection().rollback();
				String error = "Unable to continue the request.  The user selected to abort the request after the warning [" + warning.getMessage() + "] was issued.";
				throw new ReqFixWarning(error, true);
			}
		}
		info.getReqFixUpdate().updateUI(1, "Performing final processing");
		this.processDriver.doFinalProcessingForBldStageLid(info);
		info.getReqFixUpdate().updateUI(1, "Request complete");
	}

	/**
	 * Method requestNonPhypComponentAction
	 * @param info
	 * @param warningIfc
	 * @throws FixException 
	 */
	public void requestNonPhypComponentAction(ReqFixInfo info, WarningIfc warningIfc) throws FixException
	{
		info.getLogger().finer("requestNonPhypComponentAction(" + info + ", " + warningIfc + ")");
		try
		{
			//do the final processing for the explicitly requested fix
			this.processDriver.doFinalProcessingForNonPhypComponent(info);

			//do whatever processing needs to be done for clones
			this.processDriver.doCloning(info);
			
			//The call to BA was moved here, and taken out of the 'doFinalProcessing()' method, so that we can be
			//assured that it will run after the cloning stuff has completed.
			this.processDriver.scheduleBuildAutomation(info, false);
		}
		catch (ReqFixWarning rfw)
		{
			// Rollback anything that was not committed
			info.getPTFDatabaseConnection().rollback();
			String error = rfw.getMessage();
			throw new FixException(error);
		}
	}

	/**
	 * Method requestCUEAction
	 * @param info
	 * @param warningIfc
	 * @throws FixException 
	 */
	public void requestCUEAction(ReqFixInfo info, WarningIfc warningIfc) throws FixException
	{
		info.getLogger().finer("requestCUEAction(" + info + ", " + warningIfc + ")");
		this.processDriver.doFinalProcessingForBldStageCUE(info);
	}

	/**
	 * Method requestEServerCommonAction
	 * @param info
	 * @param warningIfc
	 * @throws FixException 
	 */
	public void requestEServerCommonAction(ReqFixInfo info, WarningIfc warningIfc) throws FixException, ReqFixWarning
	{
		info.getLogger().finer("requestEServerCommonAction(" + info + ", " + warningIfc + ")");
		try
		{
			this.processDriver.doInitialFinalProcessingForEServerCommonFix(info);
		}
		catch (ReqFixWarning warning)
		{
			if (!warningIfc.continueProcessingAfterWarning(warning))
			{
				// Rollback anything that was not committed
				info.getPTFDatabaseConnection().rollback();
				String error = "Unable to continue the request.  The user selected to abort the request after the warning [" + warning.getMessage() + "] was issued.";
				throw new ReqFixWarning(error, true);
			}
		}
		this.processDriver.doFinalProcessingForEServerCommonFix(info);
	}

	/**
	 * Method requestVECommonAction
	 * @param info
	 * @param warningIfc
	 * @throws FixException 
	 */
	public void requestVECommonAction(ReqFixInfo info, WarningIfc warningIfc) throws FixException, ReqFixWarning
	{
		info.getLogger().finer("requestVECommonAction(" + info + ", " + warningIfc + ")");
		this.processDriver.doFinalProcessingForVeCommonFix(info);
	}

	/**
	 * Method requestVE1CommonAction
	 * @param info
	 * @param warningIfc
	 * @throws FixException 
	 */
	public void requestVE1CommonAction(ReqFixInfo info, WarningIfc warningIfc) throws FixException, ReqFixWarning
	{
		info.getLogger().finer("requestVE1CommonAction(" + info + ", " + warningIfc + ")");
		this.processDriver.doFinalProcessingForVe1CommonFix(info);
	}

	public void ensureReadmeRebuildIsValid(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("ensureReadmeRebuildIsValid(" + info + ")");
		this.processDriver.ensureReadmeRebuildIsValid(info);
	}

	/**
	 * Method saveToDatabaseForVEReadmeRebuild
	 * @param info
	 * @throws FixException 
	 */
	public void saveToDatabaseForVEReadmeRebuild(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("doReadmeReprocessingForVeCommonFix(" + info + ")");
		this.processDriver.saveToDatabaseForVEReadmeRebuild(info);
	}

	/**
	 * Method doReadmeReprocessingForVeCommonFix
	 * @param info
	 * @throws FixException 
	 */
	public void doReadmeReprocessingForVeCommonFix(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("doReadmeReprocessingForVeCommonFix(" + info + ")");
		this.processDriver.doReadmeReprocessingForVeCommonFix(info);
	}

	/**
	 * Method getReleaseUsage
	 * @param info
	 * @return String
	 * @throws FixException 
	 */
	public String getReleaseUsage(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("getReleaseUsage()");
		TrackUtilities trackUtil = new TrackUtilities(info.getFixUser());
		return trackUtil.getRelUsageBasedOnTracks(info);
	}

	/**
	 * Method getFixAttributeFields
	 * @param info
	 * @return String[]
	 */
	public String[] getFixAttributeFields(ReqFixInfo info)
	{
		info.getLogger().finer("getFixAttributeFields(" + info + ")");
		String processType = info.getProcessType();
		if (processType.equals(LID_BASED_ISERIES_PTF_PROCESS_TYPE) || processType.equals(LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE))
		{
			return PHYP_ISERIES_PTF_FIX_ATTRIBUTE_FIELDS;
		}
		else if (processType.equals(BLD_STAGE_LID_BLD_PROCESS_TYPE))
		{
			Vector ptfAssociationList = info.getPtfAssociationsOnRequest();
			if (ptfAssociationList != null)
			{
				for (int i = 0; i < ptfAssociationList.size(); i++)
				{
					PtfAssociation ptfAssociation = (PtfAssociation) ptfAssociationList.elementAt(i);
					if (ptfAssociation != null)
					{
						try
						{
							Release release = new Release(ptfAssociation.getPtrProduct(), ptfAssociation.getRelease(), false);
							if ((release.getMetadataType()).equalsIgnoreCase("PTE"))
							{
								return PHYP_BLD_STAGE_PTE_FIX_ATTRIBUTE_FIELDS;
							}
						}
						catch (ReleaseException re)
						{
							// Eat exception.. Try next track.   If all error will return PTF fields.  
						}
					}
				}
			}
			return PHYP_BLD_STAGE_PTF_FIX_ATTRIBUTE_FIELDS;
		}
		else if (processType.equals(TRADITIONAL_ISERIES_PTF_PROCESS_TYPE) || processType.equals(ESERVER_ISERIES_FIX_PROCESS_TYPE))
		{
			return ISERIES_FIX_ATTRIBUTE_FIELDS;
		}
		else if (processType.equals(TRADITIONAL_CUE_BLD_PROCESS_TYPE))
		{
			return CUE_FIX_ATTRIBUTE_FIELDS;
		}
		else if (processType.equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
		{
			return ESERVER_FIX_ATTRIBUTE_FIELDS;
		}
		else if (processType.equals(VE_COMMON_FIX_PROCESS_TYPE)
                 || processType.equals(BBP_PROCESS_TYPE))
		{
			return VE_FIX_ATTRIBUTE_FIELDS;
		}
		else if (processType.equals(VE1_COMMON_FIX_PROCESS_TYPE))
		{
			return VE1_FIX_ATTRIBUTE_FIELDS;
		}
		else if (processType.equals(FSP_PROCESS_TYPE)
                 || processType.equals(PFW_PROCESS_TYPE))
		{
		    // GFW and GFW_HIGH both use this flow
			return PFW_FSP_ISERIES_FIX_ATTRIBUTE_FIELDS;
		}
		else if (processType.equals(GFW_LPP_PROCESS_TYPE)
                 || processType.equals(GFW_LPP_HIGH_PROCESS_TYPE))
		{
		    return GFW_ISERIES_FIX_ATTRIBUTE_FIELDS;
		}
		else
		{
			return ALL_VALID_FIX_ATTRIBUTE_FIELDS;
		}
	}

	/**
	 * Method formatText
	 * @param text
	 * @param maxWidth
	 * @return String
	 */
	public static String formatText(String text, int maxWidth)
	{
		int currentLocation = 0;
		StringBuffer textBuffer = new StringBuffer();
		int locationOfNewLine = text.indexOf('\n');
		while (currentLocation < text.length() && (text.length() - currentLocation) > maxWidth)
		{
			locationOfNewLine = text.indexOf('\n', currentLocation);
			if ((locationOfNewLine == -1) || ((locationOfNewLine - currentLocation) > maxWidth))
			{
				textBuffer.append(text.substring(currentLocation, (currentLocation + maxWidth)));
				textBuffer.append('\n');
				currentLocation += maxWidth;
			}
			else
			{
				textBuffer.append(text.substring(currentLocation, (locationOfNewLine + 1)));
				currentLocation = locationOfNewLine + 1;
			}
		}
		if (currentLocation < text.length())
			textBuffer.append(text.substring(currentLocation));
		return textBuffer.toString();
	}

	/**
	 * Method validateRequisite
	 * @param ptfReqRequisiteList
	 * @throws FixException
	 */
	public void validateRequisite(ReqFixInfo info, Vector ptfReqRequisiteList) throws FixException
	{
		info.getLogger().finer("validateRequisite(" + info + ", " + ptfReqRequisiteList + ")");

		if (ptfReqRequisiteList == null || ptfReqRequisiteList.size() < 1)
		{
			String error = "Unable to validate requisites.  No requisites were passed to be validated.";
			throw new FixException(error);
		}
		for (int i = 0; i < ptfReqRequisiteList.size(); i++)
		{
			PtfReqRequisite ptfReqRequisite = (PtfReqRequisite) ptfReqRequisiteList.elementAt(i);
			if (ptfReqRequisite != null)
			{
				try
				{
					this.processDriver.validateRequisite(info, ptfReqRequisite);
				}
				catch (FixException rfe)
				{
					String error = "An error occurred while attempting to validate requisite.  The error was:  " + rfe.getMessage();
					throw new FixException(error);
				}
			}
		}
	}

	/**
	 * Method getCommonCodeTemplateData
	 * @param info
	 * @return String
	 * @throws FixException
	 * @throws ReqFixWarning 
	 */
	public String getCommonCodeTemplateData(ReqFixInfo info) throws FixException, ReqFixWarning
	{
		info.getLogger().finer("getCommonCodeTemplateData(" + info + ")");
		return this.processDriver.getCommonCodeTemplateData(info);
	}

	/**
	 * Method getPotentialFeatureCodes
	 * @param info
	 * @return Vector of String objects
	 * @throws FixException 
	 */
	public Vector getPotentialFeatureCodes(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("getPotentialFeatureCodes(" + info + ")");
		Vector ptfAssociationList = info.getPtfAssociationsOnRequest();
		if (ptfAssociationList == null || ptfAssociationList.size() < 1)
		{
			return new Vector();
		}
		else
		{
			PtfAssociation ptfAssociation = (PtfAssociation) ptfAssociationList.firstElement();
			return getPotentialFeatureCodes(info, ptfAssociation.getPtrProduct(), ptfAssociation.getRelease());
		}
	}

	/**
	 * Method getPotentialFeatureCodes
	 * @param info
	 * @param product
	 * @param release
	 * @return Vector of String objects
	 * @throws FixException 
	 */
	public Vector getPotentialFeatureCodes(ReqFixInfo info, String product, String release) throws FixException
	{
		info.getLogger().finer("getPotentialFeatureCodes(" + info + ", " + product + ", " + release + ")");
		if (product == null || product.trim().equals(SELECT_PRODUCT_TEXT) || release == null || release.trim().equals(SELECT_RELEASE_TEXT))
		{
			return new Vector();
		}
		else
		{
			return this.processDriver.getPotentialFeatureCodes(product, release);
		}
	}

	/**
	 * Method validateExitProgram
	 * @param info
	 * @param exitProgamsOnList
	 * @param ptfExitProgram
	 * @throws FixException 
	 */
	public void validateExitProgram(ReqFixInfo info, Vector exitProgamsOnList, PtfExitProgram ptfExitProgram) throws FixException
	{
		info.getLogger().finer("validateExitProgram(" + info + ", " + exitProgamsOnList + ", " + ptfExitProgram + ")");
		this.processDriver.validateExitProgram(info, exitProgamsOnList, ptfExitProgram);
	}

	/**
	 * Method isProfilingSupported
	 * @param info
	 * @return boolean
	 * @throws FixException 
	 */
	public boolean isProfilingSupported(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("isProfilingSupported(" + info + ")");

		TrackUtilities trackUtilities = new TrackUtilities(info.getFixUser());
		Vector prodReleaseInfoList = trackUtilities.getUniqueProdRelCombos(info.getPtfAssociationsOnRequest());
		if (prodReleaseInfoList != null)
		{
			for (int i = 0; i < prodReleaseInfoList.size(); i++)
			{
				ProductAndReleaseInfo prodReleaseInfo = (ProductAndReleaseInfo) prodReleaseInfoList.elementAt(i);
				String asProfileRequired = info.getMetadataDatabaseConnection().getProfileRequired(prodReleaseInfo.getCMVCProduct(), prodReleaseInfo.getCMVCRelease());
				if (asProfileRequired.trim().toUpperCase().startsWith("Y"))
				{
					return true;
				}
			}
		}
		return false;
	}

	/**
	 * Method saveFixToDatabase
	 * @param info
	 * @throws FixException 
	 */
	public void saveFixToDatabase(ReqFixInfo info, WarningIfc warningIfc) throws FixException, ReqFixWarning
	{
		info.getLogger().finer("saveFixToDatabase(" + info + ")");
		String processType = info.getProcessType();
		if (processType.equals(TRADITIONAL_CUE_BLD_PROCESS_TYPE))
		{
			this.processDriver.saveToDatabaseForCUE(info);
		}
		else if (processType.equals(BLD_STAGE_LID_BLD_PROCESS_TYPE))
		{
			try
			{
				this.processDriver.initialSaveToDatabaseForEServerCommonAndPhypBld(info);
			}
			catch (ReqFixWarning warning)
			{
				if (!warningIfc.continueProcessingAfterWarning(warning))
				{
					// Rollback anything that was not committed
					info.getPTFDatabaseConnection().rollback();
					String error = "Unable to continue the request.  The user selected to abort the save to database after the warning [" + warning.getMessage() + "] was issued.";
					throw new ReqFixWarning(error, true);
				}
			}
			this.processDriver.finalSaveToDatabaseForPhypBld(info);
		}
		else if (processType.equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
		{
			try
			{
				this.processDriver.initialSaveToDatabaseForEServerCommonAndPhypBld(info);
			}
			catch (ReqFixWarning warning)
			{
				if (!warningIfc.continueProcessingAfterWarning(warning))
				{
					// Rollback anything that was not committed
					info.getPTFDatabaseConnection().rollback();
					String error = "Unable to continue the request.  The user selected to abort the save to database after the warning [" + warning.getMessage() + "] was issued.";
					throw new ReqFixWarning(error, true);
				}
			}
			this.processDriver.finalSaveToDatabaseForEServerCommon(info);
		}
		else if (processType.equals(VE_COMMON_FIX_PROCESS_TYPE)
                 || processType.equals(BBP_PROCESS_TYPE))
		{
			this.processDriver.saveToDatabaseForVECommon(info);
		}
		else if (processType.equals(VE1_COMMON_FIX_PROCESS_TYPE))
		{
			this.processDriver.saveToDatabaseForVE1Common(info);
		}
		else
		{
			this.processDriver.saveToDatabase(info);
		}
	}

	/**
	 * Method exportFixToXml
	 * @param info
	 * @throws FixException 
	 */
	public void exportFixToXml(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("exportFixToXml(" + info + ")");
		this.processDriver.writeToXMLFile(info);
	}

	/**
	 * Method lockPTFAndPtrTrackRows
	 * @param info
	 * @throws FixException 
	 */
	public void lockPTFAndPtrTrackRows(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("lockPTFAndPtrTrackRows(" + info + ")");
		boolean doPackageLock = false;
		String reasonForLock = GENERIC_LOCK;
		String processType = info.getProcessType();
		if (processType.equals(LID_BASED_ISERIES_PTF_PROCESS_TYPE)
            || processType.equals(LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE))
        {
            reasonForLock = PACKAGE_LOCK;
            doPackageLock = true;
        }
		else if (processType.equals(BLD_STAGE_LID_BLD_PROCESS_TYPE))
		{
			reasonForLock = BLD_LOCK;
		}
		else if (processType.equals(TRADITIONAL_ISERIES_PTF_PROCESS_TYPE) || processType.equals(ESERVER_ISERIES_FIX_PROCESS_TYPE))
		{
			reasonForLock = ISERIES_LOCK;
		}
		else if (processType.equals(TRADITIONAL_CUE_BLD_PROCESS_TYPE))
		{
			reasonForLock = CUE_LOCK;
		}
		else if (processType.equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
		{
			reasonForLock = GENERIC_LOCK;
		}
		else if (processType.equals(VE_COMMON_FIX_PROCESS_TYPE))
		{
			reasonForLock = GENERIC_LOCK;
		}
		else if (processType.equals(VE1_COMMON_FIX_PROCESS_TYPE))
		{
			reasonForLock = GENERIC_LOCK;
		}
		else if (processType.equals(BBP_PROCESS_TYPE))
		{
		    reasonForLock = GENERIC_LOCK;
		}
		else if (processType.equals(FSP_PROCESS_TYPE)
                 || processType.equals(PFW_PROCESS_TYPE)
                 || processType.equals(GFW_LPP_PROCESS_TYPE)
                 || processType.equals(GFW_LPP_HIGH_PROCESS_TYPE))
		{
			reasonForLock = ISERIES_LOCK;
		}
		this.processDriver.lockPTFAndPtrTrackRows(info.getPtfForRequest(), info.getPtfAssociationsOnRequest(), info.getPTFDatabaseConnection(), reasonForLock, doPackageLock);
	}

	/**
	 * Method unlockPTFAndPtrTrackRows
	 * @param info
	 * @throws FixException 
	 */
	public void unlockPTFAndPtrTrackRows(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("unlockPTFAndPtrTrackRows(" + info + ")");
		this.processDriver.unlockPTFAndPtrTrackRows(info);
		if (VE_COMMON_FIX_PROCESS_TYPE.equalsIgnoreCase(info.getProcessType())
            || BBP_PROCESS_TYPE.equalsIgnoreCase(info.getProcessType()))
		{
			//  Unlock all of the tracks and PTFs for the other operating systems
			Vector associatedInfo = info.getSecondaryRequestInfo();
			if (associatedInfo != null)
			{
				for (int i = 0; i < associatedInfo.size(); i++)
				{
					ReqFixInfo osInfo = (ReqFixInfo) associatedInfo.elementAt(i);
					this.processDriver.unlockPTFAndPtrTrackRows(osInfo);
				}
			}
		}
	}

	/**
	 * Method unlockRows
	 * @param info
	 * @throws FixException 
	 */
	public void unlockRows(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("unlockRows(" + info + ")");
		this.processDriver.unlockPTFAndPtrTrackRows(info);
		// Only do unlock if in the VE Process
		if (VE_COMMON_FIX_PROCESS_TYPE.equalsIgnoreCase(info.getProcessType())
            || VE1_COMMON_FIX_PROCESS_TYPE.equalsIgnoreCase(info.getProcessType())
            || BBP_PROCESS_TYPE.equalsIgnoreCase(info.getProcessType()))
		{
			if (VE_COMMON_FIX_PROCESS_TYPE.equalsIgnoreCase(info.getProcessType())
                || BBP_PROCESS_TYPE.equalsIgnoreCase(info.getProcessType()))
			{
				//  Unlock all of the tracks and PTFs for the other operating systems
				Vector associatedInfo = info.getSecondaryRequestInfo();
				if (associatedInfo != null)
				{
					for (int i = 0; i < associatedInfo.size(); i++)
					{
						ReqFixInfo osInfo = (ReqFixInfo) associatedInfo.elementAt(i);
						this.processDriver.unlockPTFAndPtrTrackRows(osInfo);
					}
				}
			}
			this.processDriver.unlockVEFixCCOption(info);
		}
	}

	public void validateVEFixCCOptionForRequest(ReqFixInfo info) throws FixException, ReqFixWarning
	{
		info.getLogger().finer("validateVEFixCCOptionForRequest(" + info + ")");
		// Make sure the VEFIXCCOption is valid to do a request on.
		this.processDriver.validateVEFixCCOptionForRequest(info);
	}

	/**
	 * @param info
	 * @throws FixException
	 */
	public void lockVEFixCCOption(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("lockVEFixCCOption(" + info + ")");
		this.processDriver.lockVEFixCCOption(info);
	}

	/**
	 * Method cancelRequest
	 * @param info
	 * @throws FixException 
	 */
	public void cancelRequest(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("lockPTFAndPtrTrackRows(" + info + ")");
		info.getPTFDatabaseConnection().rollback();
		unlockRows(info);
		closeDatabaseConnections(info);
	}

	/**
	 * Method closeDatabaseConnections
	 * @param info
	 * @throws FixException 
	 */
	public void closeDatabaseConnections(ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("closeDatabaseConnections(" + info + ")");
		StringBuffer errorBuffer = new StringBuffer("An error occurred while attempting to close database connect.  The error was:  ");
		boolean errorOccurred = false;
		try
		{
			PTFDBConnectionUtilities ptfDbConnection = info.getPTFDatabaseConnection();
			ptfDbConnection.close();
		}
		catch (FixException fe)
		{
			errorOccurred = true;
			errorBuffer.append("Unable to close PTF database connection:  " + fe.getMessage() + "  ");
		}
		try
		{
			MetadataUtilities metaDataConnection = info.getMetadataDatabaseConnection();
			metaDataConnection.close();
		}
		catch (FixException fe)
		{
			errorOccurred = true;
			errorBuffer.append("Unable to close PRF database connection:  " + fe.getMessage() + "  ");

		}
		// If an error occurred..  Report it. 
		if (errorOccurred)
		{
			throw new FixException(errorBuffer.toString());
		}
	}

	/**
	 * Method getTracksEligibleForEServerFix
	 * @param product
	 * @param release
	 * @param featureCode
	 * @param info
	 * @throws FixException
	 */
	public Vector getTracksEligibleForEServerFix(String product, String release, String featureCode, ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("getTracksEligibleForEServerFix(" + product + ", " + release + ", " + featureCode + ", " + info + ")");
		return this.processDriver.getTracksEligibleForEServerFix(product, release, featureCode, info.getPTFDatabaseConnection(), info.getPtfAssociationsOnRequest());
	}

	/**
	 * Method getPtfAssociationsTargettingVEFixCCOption
	 * @param info
	 * @param veFixCCOption
	 * @return Vector of PtfAssociation objects
	 * @throws FixException
	 */
	public Vector getPtfAssociationsTargettingVEFixCCOption(ReqFixInfo info, VEFIXCCOption veFixCCOption) throws FixException
	{
		info.getLogger().finer("getPtfAssociationsTargettingVEFixCCOption(" + info + ", " + veFixCCOption + ")");
		return info.getPTFDatabaseConnection().getPtfAssociationsByVEFIXCCOptionId(veFixCCOption.getId());
	}

	/**
	 * Method appendListOfLibraryPartsToSpecialInstructions
	 * @param userEnteredSpecialInstructions
	 * @param info
	 * @return String
	 * @throws FixException
	 */
	public String appendListOfLibraryPartsToSpecialInstructions(String userEnteredSpecialInstructions, ReqFixInfo info) throws FixException
	{
		info.getLogger().finer("appendListOfLibraryPartsToSpecialInstructions(" + userEnteredSpecialInstructions + ", " + info + ")");
		if (info.getProcessType().equals(ESERVER_COMMON_FIX_PROCESS_TYPE))
		{
			return this.processDriver.appendListOfLibraryPartsToSpecialInstructions(userEnteredSpecialInstructions, info.getPtfAssociationsOnRequest(), info.getMetadataDatabaseConnection());
		}
		else if (info.getProcessType().equals(VE_COMMON_FIX_PROCESS_TYPE)
                 || info.getProcessType().equals(BBP_PROCESS_TYPE))
		{

			// Decided this was not needed for SWF.   Possibly will need it for OS's not build from the tools in Roch.  
			//	return this.processDriver.appendListOfLibraryPartsToSpecialInstructions(userEnteredSpecialInstructions, info.getPtfAssociationsOnRequest(), info.getMetadataDatabaseConnection());
			String error = "appendListOfLibraryPartsToSpecialInstructions not supported for VE";
			throw new FixException(error);
		}
		else if (info.getProcessType().equals(VE1_COMMON_FIX_PROCESS_TYPE))
		{
			return this.processDriver.appendListOfLibraryPartsToSpecialInstructions(userEnteredSpecialInstructions, info.getPtfAssociationsOnRequest(), info.getMetadataDatabaseConnection());
		}
		return formatText(userEnteredSpecialInstructions, 72);
	}

	//begin code added by MHD; PDV07809
	public void setSpecialInstructionsFromCommandLine(String instructions, ReqFixInfo info) throws FixException
	{
		//VALIDATE INPUT
		if (info == null)
		{
			throw new FixException("Unable to update special instructions from the command line.  The info object passed in was null.");
		} //end if
		info.getLogger().finer("updateSpecialInstructionsFromCommandLine(" + instructions + ", " + info + ")");

		//MODIFY SPECIAL INSTRUCTIONS IF NEEDED
		instructions = this.processDriver.setSpecialInstructionsFromCommandLine(instructions, info);

		//FORMAT THE TEXT
		if (instructions == null)
		{
			//do this to avoid a null pointer in the 'formatText()' method
			instructions = "";
		} //end if
		instructions = formatText(instructions, 72);

		//SET THE SPECIAL INSTRUCTIONS INTO THE INFO OBJECT
		if (instructions == null || instructions.trim().equalsIgnoreCase(""))
		{
			//the instructions are empty or null, so set a null out the value in the info object
			info.getLogger().fine("There are no special instructions...nulling out the special instructions in the info object.");
			info.setRequestedSpecialInstructions(null);
		} //end if
		else
		{
			//there are instructions...use them
			info.getLogger().fine("There are special instructions...using them.");
			PtfInstructions ptfInstructions = new PtfInstructions(PtfInstructions.TYPE_REQUESTED_SPECIAL, instructions);
			info.setRequestedSpecialInstructions(ptfInstructions);
		} //end else
	} //end updateSpecialInstructionsFromCommandLine()
	//end code added by MHD; PDV07809

	public ReqFixInfo setFirmwareApplyType(ReqFixInfo reqFixInfo) throws FixException
	{
		reqFixInfo.getLogger().finer("gatherFirmwareApplyTypeInfo(" + reqFixInfo + ")");
		return setFirmwareApplyTypeWithValidation(reqFixInfo, false, null);
	}

	public ReqFixInfo setFirmwareApplyTypeWithValidation(ReqFixInfo reqFixInfo, boolean doValidation, String firmwareApplyTypeToValidate) throws FixException
	{
		reqFixInfo.getLogger().finer("gatherFirmwareApplyTypeInfo(" + reqFixInfo + ")");
		FirmwareApplyTypeInfo applyTypeInfo = null;
		if (PTFConstants.BLD_STAGE_LID_BLD_PROCESS_TYPE.equalsIgnoreCase(reqFixInfo.getProcessType()))
		{
			applyTypeInfo = this.processDriver.gatherFirmwareApplyTypeInfo(reqFixInfo);
		}
		else if (PTFConstants.LID_BASED_ISERIES_PTF_PROCESS_TYPE.equalsIgnoreCase(reqFixInfo.getProcessType())
                 || PTFConstants.LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE.equalsIgnoreCase(reqFixInfo.getProcessType()))
        {
			applyTypeInfo = this.processDriver.getDefaultFirmwareApplyTypeForPhypPackagePTF(reqFixInfo);
		}
		if (applyTypeInfo != null)
		{
			if (doValidation)
			{
				this.processDriver.ensureSpecifiedApplyTypeIsValid(firmwareApplyTypeToValidate, applyTypeInfo);
			}
			// Set the apply type in the info object
			String applyType = applyTypeInfo.getDefaultFirmwareApplyType();
			if (PTFConstants.DISRUPTIVE_VALUE.equals(applyType))
			{
				reqFixInfo.setdisruptive();
			}
			else if (PTFConstants.DEFERRED_VALUE.equals(applyType))
			{
				reqFixInfo.setDeferred();
			}
			else
			{
				// It is not disruptive of deferred mark it concurrent.
				reqFixInfo.setConcurrent();
			}
			enableFirmwareApplyType(reqFixInfo, applyTypeInfo);
		}
		return reqFixInfo;
	}

	private ReqFixInfo enableFirmwareApplyType(ReqFixInfo reqFixInfo, FirmwareApplyTypeInfo applyTypeInfo)
	{
		if (applyTypeInfo != null)
		{
			reqFixInfo.setEnableConcurrentField(applyTypeInfo.isConcurrentEnabled());
			if (!applyTypeInfo.isConcurrentEnabled())
			{
				reqFixInfo.getLogger().info("The fix cannot be marked concurrent for this reason: " + applyTypeInfo.getReasonConcurrentIsNotAllowed());
			}
			reqFixInfo.setEnableDeferredField(applyTypeInfo.isDeferredEnabled());
			if (!applyTypeInfo.isDeferredEnabled())
			{
				reqFixInfo.getLogger().info("The fix cannot be marked deferred for this reason: " + applyTypeInfo.getReasonDeferredIsNotAllowed());
			}
		}
		return reqFixInfo;
	}

	/**
	 * Method getSearchResults
	 * @param reqFixInfo
	 * @param product
	 * @param release
	 * @param featureCode
	 * @return Vector of PtfAssociation objects
	 * @throws FixException
	 */
	public Vector getSearchResults(ReqFixInfo reqFixInfo, String product, String release, String featureCode) throws FixException
	{
		reqFixInfo.getLogger().finer("getSearchResults(" + reqFixInfo + ", " + product + ", " + release + ", " + featureCode + ")");
		String processType = reqFixInfo.getProcessType();
		// Get the valid candidate tracks 
		if (LID_BASED_ISERIES_PTF_PROCESS_TYPE.equals(reqFixInfo.getProcessType())
            || LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE.equals(reqFixInfo.getProcessType()))
        {
            // This is a PHYP 'package' iSeries PTF.. Get the approved bld stage lid builds.
            return getApprovedTracks(reqFixInfo, product, release);
        }
		else if (ESERVER_COMMON_FIX_PROCESS_TYPE.equals(reqFixInfo.getProcessType()))
		{
			// This is an eServer common code fix.  Get all the tracks that have a track process state of working. 
			return getTracksEligibleForEServerFix(product, release, featureCode, reqFixInfo);
		}
		// This is not a valid process type for the search option.   Report it appropriately.
		String error = "The search option is not supported for the process type '" + reqFixInfo.getProcessType() + "'";
		throw new FixException(error);
	}

	/**
	 * Method getRemovePtfAssociationInfo
	 * @param reqFixInfo
	 * @param associationsToRemove
	 * @return RemovePtfAssociationInfo
	 * @throws FixException
	 */
	public RemovePtfAssociationInfo getRemovePtfAssociationInfo(ReqFixInfo reqFixInfo, Vector associationsToRemove) throws FixException
	{
		reqFixInfo.getLogger().finer("getRemovePtfAssociationInfo(" + reqFixInfo + ", " + associationsToRemove + ")");
		try
		{
			if (LID_BASED_ISERIES_PTF_PROCESS_TYPE.equals(reqFixInfo.getProcessType())
                || LID_BASED_GFW_ISERIES_PTF_PROCESS_TYPE.equals(reqFixInfo.getProcessType()))
            {
                associationsToRemove = determineTracksToRemoveFromPackagePTF(reqFixInfo,
                                                                             associationsToRemove);
            }
            return removePtfAssociationsFromRequest(reqFixInfo, associationsToRemove);
		}
		catch (Exception e)
		{
			String error = "Unable to remove the selected tracks.  The error was:  " + e.toString();
			throw new FixException(error);
		}
	}

	/**
	 * Method getRequiredNotIncludedPtfAssociations
	 * @param reqFixInfo
	 * @return Vector
	 * @throws FixException
	 */
	public Vector getRequiredNotIncludedPtfAssociations(ReqFixInfo reqFixInfo) throws FixException
	{
		reqFixInfo.getLogger().finer("getRequiredNotIncludedPtfAssociations(" + reqFixInfo + ")");
		return this.processDriver.getRequiredNotIncludedPtfAssociations(reqFixInfo);
	}


	/**
	 * Method getPreviousFixPeriodInstructions
	 * @param initiative
	 * @param fixDatabase
	 * @return HashMap
	 * @throws FixException
	 */
	public HashMap getPreviousFixPeriodInstructions(VEFIXCCOption initiative, FixDatabase fixDatabase) throws FixException
	{
		HashMap return_hashmap = new HashMap();

		// get all the OSFixPacks associated with the Initiative input parameter
		// for each OSFixPack, get the supersede chain... and grab instructions from the previous link in the chain

		// get all the OSFixPacks for the Initiative input parm
        Vector osfixpackList = fixDatabase.getVEContentByVEFIXCCOptionId( initiative.getId() );
        if ( osfixpackList != null )
        {
        	// loop through all the OSFixPacks -- get the previous/superseding OSFixPack, and
        	// grab all the different instructions from that
            for (int i = 0; i < osfixpackList.size(); i++)
            {
                VEContent current_osFixPack = (VEContent) osfixpackList.elementAt(i);
                if( current_osFixPack != null )
                {
                	// get the current OSFixPack name 
                    Option osOption = fixDatabase.getOptionByID( current_osFixPack.getOSOptionID() );
                    String current_osFixPack_name = osOption.getOs();

                    // skip commoncode
            		if ( current_osFixPack_name.equalsIgnoreCase(IVEConst.COMMON_CODE_OS) )
            		{
            			// continue to the next OSFixPack in the loop
                		continue;
            		}

                	// have an OSFixPack... looking for the supersede
            		// get the whole chain of OSFixPacks, then get the previous one

            		// My view of things...
            		// LinearSupersedeChain -- basically represents a 'link' in the chain, with access
            		// to the other links
            		//     the current 'link in the chain' object is accessible via getChainNode()
            		//     the previous and next 'link in the chain' objects are accessible by other methods:
            		//          getSupersededChain() -- moves one link 'back', to previous link
            		//          getSupersedingChain() -- moves one link 'forward', to next link
            		// 
            		// So, FixSupersedeManager.getVEContentSupersedeChain( osfixpack )
            		// returns the specified initiative 'link' in the chain
            		// From that 'link', getSupersededChain() returns the previous osfixpack 'link' in the chain
            		// And from that 'link', getChainNode() returns the actual osfixpack object
            		FixSupersedeManager fix_supersede_manager = new FixSupersedeManager( fixDatabase );
            		LinearSupersedeChain linear_supersede_chain = fix_supersede_manager.getVEContentSupersedeChain( current_osFixPack );
            		LinearSupersedeChain previous_supersede_chain = linear_supersede_chain.getSupersededChain();
            		// check if there is NOT a previous supersede chain
            		if ( previous_supersede_chain == null )
            		{// there is NOT a previous supersede chain
            			// there are no previous instructions to retrieve for this OSFixPack
            			// continue to the next OSFixPack in the loop
            			continue;
            		}

            		// get the actual OSFixPack object
            		VEContent previous_osFixPack = (VEContent) previous_supersede_chain.getChainNode();
            		// get the Ptf object associated with the osfixpack
            		Vector ptfList = fixDatabase.getPTFsThatMapToVEContent( previous_osFixPack );
					if ( ptfList != null  &&  ptfList.size() > 0 )
					{
						// get the one Ptf object from the Vector
						Ptf current_Ptf = (Ptf) ptfList.get(0);

						// put the instructions related to the Ptf object into a HashMap
	            		HashMap current_hashMap = new HashMap();
	            		if ( fixDatabase.getPtfInstructions(current_Ptf, PtfInstructions.TYPE_PREINSTALL) != null )
	            		{
	            			current_hashMap.put(PtfInstructions.TYPE_PREINSTALL, fixDatabase.getPtfInstructions(current_Ptf, PtfInstructions.TYPE_PREINSTALL).getText());
	            		}
	            		if ( fixDatabase.getPtfInstructions(current_Ptf, PtfInstructions.TYPE_POSTINSTALL) != null )
	            		{
	            			current_hashMap.put(PtfInstructions.TYPE_POSTINSTALL, fixDatabase.getPtfInstructions(current_Ptf, PtfInstructions.TYPE_POSTINSTALL).getText());
	            		}
	            		if ( fixDatabase.getPtfInstructions(current_Ptf, PtfInstructions.TYPE_PREUNINSTALL) != null )
	            		{
	            			current_hashMap.put(PtfInstructions.TYPE_PREUNINSTALL, fixDatabase.getPtfInstructions(current_Ptf, PtfInstructions.TYPE_PREUNINSTALL).getText());
	            		}
	            		if ( fixDatabase.getPtfInstructions(current_Ptf, PtfInstructions.TYPE_POSTUNINSTALL) != null )
	            		{
	            			current_hashMap.put(PtfInstructions.TYPE_POSTUNINSTALL, fixDatabase.getPtfInstructions(current_Ptf, PtfInstructions.TYPE_POSTUNINSTALL).getText());
	            		}
	            		if ( fixDatabase.getPtfInstructions(current_Ptf, PtfInstructions.TYPE_GENERAL) != null )
	            		{
	            			current_hashMap.put(PtfInstructions.TYPE_GENERAL, fixDatabase.getPtfInstructions(current_Ptf, PtfInstructions.TYPE_GENERAL).getText());
	            		}

	            		// add the new HashMap to the master HashMap, using the OSFixPack name as the key
	            		return_hashmap.put( current_osFixPack_name, current_hashMap );
					}

                }//current_osFixPack != null
            }//for loop
        }//osfixpackList != null

        // return the hashmap with the Initiative's supersede instructions
		return return_hashmap;
	}//getPreviousFixPeriodInstructions

}
