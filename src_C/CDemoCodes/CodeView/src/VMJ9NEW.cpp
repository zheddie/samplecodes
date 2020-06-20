/*
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2000, 2015 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * This file is part of product 5655-W32.
 *
 */

#define J9_EXTERNAL_TO_VM

#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#ifdef LINUX
#include <signal.h>
#endif

#if defined(J9OS_I5)
#include <sys/systemcfg.h>
#endif /* defined(J9OS_I5) */

#if defined(J9OS_I5_V6R1)
#define __power_vsx() (_system_configuration.vmx_version > 1)
#endif /* defined(J9OS_I5_V6R1) */

#include "j9cfg.h"
#include "vmaccess.h"
#include "j9list.h"
#include "j9port.h"
#include "j9consts.h"
#include "j9comp.h"
#include "j9method.h"
#include "j9fieldsInfo.h"
#include "jilconsts.h"
#include "fastJNI.h"
#include "asmprotos.h"
#include "VMJ9.h"
#include "cache.h"
#include "jitprotos.h"
#include "j9protos.h"
#include "ut_j9jit.h"
#include "j9jitnls.h"
#include "stackwalk.h"
#include "locknursery.h"
#include "thrtypes.h"

#include "j9modron.h"
#include "exports.h"

#include "AheadOfTimeCompile.hpp"
#include "AOTct.hpp"
#include "Compilation.hpp"
#include "CompilationThread.hpp"
#include "CodeGenerator.hpp"
#include "OptimizerImpl.hpp"
#include "GCStackMap.hpp"
#include "GCStackAtlas.hpp"
#include "IMTBuilder.hpp"
#include "IProfiler.hpp"
#include "HWProfiler.hpp"
#include "Instruction.hpp"
#include "J9Monitor.hpp"
#include "Node.hpp"
#include "Symbol.hpp"
#include "TreeTop.hpp"
#include "Snippet.hpp"
#include "CHTable.hpp"
#include "codertinit.hpp"
#include "CompilationController.hpp"
#include "ConcreteILGenRequests.hpp"

#include "ClassLoaderTable.hpp"

#include "NewRelocationRuntime.hpp"
#include "RAIIMonitor.hpp"

#if defined(NEW_RELOCATION_RUNTIME)
#include "RelocationRuntime.hpp"
#endif

#ifdef J9_CODE_CACHE_RECLAMATION
#include "CodeCacheReclamation.h"
#endif
#include "MultiCodeCache.hpp"
#include "DataCache.hpp"
#include <setjmp.h>

#include "Testarossa.hpp"

#ifndef J9_FINDKNOWNCLASS_FLAG_EXISTING_ONLY
#define J9_FINDKNOWNCLASS_FLAG_EXISTING_ONLY 2
#endif

#define REFERENCEFIELD "referent"
#define REFERENCEFIELDLEN 8
#define REFERENCERETURNTYPE "Ljava/lang/Object;"
#define REFERENCERETURNTYPELEN 18

#if SOLARIS || AIXPPC || LINUX
#include <strings.h>
#define J9OS_STRNCMP strncasecmp
#else
#define J9OS_STRNCMP strncmp
#endif

#define FIELD_OFFSET_NOT_FOUND -1

enum { IS_VOLATILE=1 };

extern "C" J9Method * VMCALL getNewInstancePrototype(J9VMThread *context);

static ILOps opCodesForLoad[TR_NumTypes] =
   {
   TR_BadILOp,  // TR_NoType
   TR_iload,    // TR_Int8
   TR_iload,    // TR_Int16
   TR_iload,    // TR_Int32
   TR_lload,    // TR_Int64
   TR_fload,    // TR_Float
   TR_dload,    // TR_Double
   TR_aload,    // TR_Address
   TR_oload,    // TR_Aggregate
   TR_BadILOp,  // TR_Method
   TR_aload,    // TR_Arraylet
#ifdef SUPPORT_DFP
   TR_dfload,   // TR_DecimalFloat
   TR_ddload,   // TR_DecimalDouble
   TR_deload,   // TR_DecimalLongDouble
#else
   TR_BadILOp,  // TR_DecimalFloat
   TR_BadILOp,  // TR_DecimalDouble
   TR_BadILOp,  // TR_DecimalLongDouble
#endif

#ifdef SUPPORT_WCODE
   TR_xfload,   // TR_ComplexFloat
   TR_xdload,   // TR_ComplexDouble
   TR_xeload,   // TR_ComplexLongDouble
   TR_eload,    // TR_LongDouble
#else
   TR_BadILOp,  // TR_ComplexFloat
   TR_BadILOp,  // TR_ComplexDouble
   TR_BadILOp,  // TR_ComplexLongDouble
   TR_BadILOp,  // TR_LongDouble
#endif
#if (defined(SUPPORT_WCODE) || defined(ENABLE_INTRINSICS))
   TR_pdload,     // TR_PackedDecimal
   TR_zdload,     // TR_ZonedDecimal
   TR_zdsleLoad,  // TR_ZonedDecimalSignLeadingEmbedded
   TR_zdslsLoad,  // TR_ZonedDecimalSignLeadingSeparate
   TR_zdstsLoad,  // TR_ZonedDecimalSignTrailingSeparate
   TR_udLoad,     // TR_UnicodeDecimal
   TR_udslLoad,   // TR_UnicodeDecimalSignLeading
   TR_udstLoad,   // TR_UnicodeDecimalSignTrailing
   TR_zfLoad,     // TR_ZonedFloat
   TR_ufLoad,     // TR_UnicodeFloat
#else
   TR_BadILOp,    // TR_PackedDecimal
   TR_BadILOp,    // TR_ZonedDecimal
   TR_BadILOp,    // TR_ZonedDecimalSignLeadingEmbedded
   TR_BadILOp,    // TR_ZonedDecimalSignLeadingSeparate
   TR_BadILOp,    // TR_ZonedDecimalSignTrailingSeparate
   TR_BadILOp,    // TR_UnicodeDecimal
   TR_BadILOp,    // TR_UnicodeDecimalSignLeading
   TR_BadILOp,    // TR_UnicodeDecimalSignTrailing
   TR_BadILOp,    // TR_ZonedFloat
   TR_BadILOp,    // TR_UnicodeFloat
#endif
   TR_BadILOp     // TR_Alet
#if defined(ENABLE_SIMD)
   ,TR_viload      // TR_VectorInt32
   ,TR_vdload      // TR_VectorDouble
#endif
   };

static ILOps opCodesForStore[TR_NumTypes] =
   {
   TR_BadILOp,  // TR_NoType
   TR_istore,   // TR_Int8
   TR_istore,   // TR_Int16
   TR_istore,   // TR_Int32
   TR_lstore,   // TR_Int64
   TR_fstore,   // TR_Float
   TR_dstore,   // TR_Double
   TR_astore,   // TR_Address
   TR_ostore,   // TR_Aggregate
   TR_BadILOp,  // TR_Method
   TR_astore,   // TR_Arraylet
#ifdef SUPPORT_DFP
   TR_dfstore,  // TR_DecimalFloat
   TR_ddstore,  // TR_DecimalDouble
   TR_destore,  // TR_DecimalLongDouble
#else
   TR_BadILOp, // TR_DecimalFloat
   TR_BadILOp,  // TR_DecimalDouble
   TR_BadILOp,   // TR_DecimalLongDouble
#endif
#ifdef SUPPORT_WCODE
   TR_xfstore, // TR_ComplexFloat
   TR_xdstore,  // TR_ComplexDouble
   TR_xestore,  // TR_ComplexLongDouble
   TR_estore,   // TR_LongDouble
#else
   TR_BadILOp,  // TR_ComplexFloat
   TR_BadILOp,   // TR_ComplexDouble
   TR_BadILOp,   // TR_ComplexLongDouble
   TR_BadILOp,    // TR_LongDouble
#endif
#if (defined(SUPPORT_WCODE) || defined(ENABLE_INTRINSICS))
   TR_pdstore,    // TR_PackedDecimal
   TR_zdstore,    // TR_ZonedDecimal
   TR_zdsleStore, // TR_ZonedDecimalSignLeadingEmbedded
   TR_zdslsStore, // TR_ZonedDecimalSignLeadingSeparate
   TR_zdstsStore, // TR_ZonedDecimalSignTrailingSeparate
   TR_udStore,    // TR_UnicodeDecimal
   TR_udslStore,  // TR_UnicodeDecimalSignLeading
   TR_udstStore,  // TR_UnicodeDecimalSignTrailing
   TR_zfStore,    // TR_ZonedFloat
   TR_ufStore,    // TR_UnicodeFloat
#else
   TR_BadILOp,    // TR_PackedDecimal
   TR_BadILOp,    // TR_ZonedDecimal
   TR_BadILOp,    // TR_ZonedDecimalSignLeadingEmbedded
   TR_BadILOp,    // TR_ZonedDecimalSignLeadingSeparate
   TR_BadILOp,    // TR_ZonedDecimalSignTrailingSeparate
   TR_BadILOp,    // TR_UnicodeDecimal
   TR_BadILOp,    // TR_UnicodeDecimalSignLeading
   TR_BadILOp,    // TR_UnicodeDecimalSignTrailing
   TR_BadILOp,    // TR_ZonedFloat
   TR_BadILOp,    // TR_UnicodeFloat
#endif
   TR_BadILOp     // TR_Alet
#if defined(ENABLE_SIMD)
   ,TR_vistore   // TR_VectorInt32
   ,TR_vdstore   // TR_VectorDouble
#endif
   };

static ILOps opCodesForIndirectLoad[TR_NumTypes] =
   {
   TR_BadILOp,  // TR_NoType
   TR_iiload,   // TR_Int8
   TR_iiload,   // TR_Int16
   TR_iiload,   // TR_Int32
   TR_ilload,   // TR_Int64
   TR_ifload,   // TR_Float
   TR_idload,   // TR_Double
   TR_iaload,   // TR_Address
   TR_BadILOp,  // TR_Aggregate
   TR_BadILOp,  // TR_Method
   TR_iaload,   // TR_Arraylet
#if defined(SUPPORT_DFP)
   TR_idfload,  // TR_DecimalFloat
   TR_iddload,  // TR_DecimalDouble
   TR_ideload,  // TR_DecimalLongDouble
#else
   TR_BadILOp,  // TR_DecimalFloat
   TR_BadILOp,  // TR_DecimalDouble
   TR_BadILOp,  // TR_DecimalLongDouble
#endif

#if defined(SUPPORT_WCODE)
   TR_ixfload,    // TR_ComplexFloat
   TR_ixdload,    // TR_ComplexDouble
   TR_ixeload,    // TR_ComplexLongDouble
   TR_ieload,     // TR_LongDouble
#else
   TR_BadILOp,    // TR_ComplexFloat
   TR_BadILOp,    // TR_ComplexDouble
   TR_BadILOp,    // TR_ComplexLongDouble
   TR_BadILOp,    // TR_LongDouble
#endif

#if (defined(SUPPORT_WCODE) || defined(ENABLE_INTRINSICS))
   TR_ipdload,       // TR_PackedDecimal
   TR_izdload,       // TR_ZonedDecimal
   TR_izdsleLoad,    // TR_ZonedDecimalSignLeadingEmbedded
   TR_izdslsLoad,    // TR_ZonedDecimalSignLeadingSeparate
   TR_izdstsLoad,    // TR_ZonedDecimalSignTrailingSeparate
   TR_iudLoad,       // TR_UnicodeDecimal
   TR_iudslLoad,     // TR_UnicodeDecimalSignLeading
   TR_iudstLoad,     // TR_UnicodeDecimalSignTrailing
   TR_izfLoad,        // TR_ZonedFloat
   TR_iufLoad,        // TR_UnicodeFloat
#else
   TR_BadILOp,    // TR_PackedDecimal
   TR_BadILOp,    // TR_ZonedDecimal
   TR_BadILOp,    // TR_ZonedDecimalSignLeadingEmbedded
   TR_BadILOp,    // TR_ZonedDecimalSignLeadingSeparate
   TR_BadILOp,    // TR_ZonedDecimalSignTrailingSeparate
   TR_BadILOp,    // TR_UnicodeDecimal
   TR_BadILOp,    // TR_UnicodeDecimalSignLeading
   TR_BadILOp,    // TR_UnicodeDecimalSignTrailing
   TR_BadILOp,    // TR_ZonedFloat
   TR_BadILOp,    // TR_UnicodeFloat
#endif
   TR_BadILOp     // TR_Alet
#if defined(ENABLE_SIMD)
   ,TR_iviload   // TR_VectorInt32
   ,TR_ivdload   // TR_VectorDouble
#endif
   };

static ILOps opCodesForIndirectStore[TR_NumTypes] =
   {
   TR_BadILOp,  // TR_NoType
   TR_iistore,  // TR_Int8
   TR_iistore,  // TR_Int16
   TR_iistore,  // TR_Int32
   TR_ilstore,  // TR_Int64
   TR_ifstore,  // TR_Float
   TR_idstore,  // TR_Double
   TR_iastore,  // TR_Address
   TR_BadILOp,  // TR_Aggregate
   TR_BadILOp,  // TR_Method
   TR_iastore,  // TR_Arraylet

#if defined(SUPPORT_DFP)
   TR_idfstore,   // TR_DecimalFloat
   TR_iddstore,   // TR_DecimalDouble
   TR_idestore,   // TR_DecimalLongDouble
#else
   TR_BadILOp,    // TR_DecimalFloat
   TR_BadILOp,    // TR_DecimalDouble
   TR_BadILOp,    // TR_DecimalLongDouble
#endif

#if defined(SUPPORT_WCODE)
   TR_ixfstore,   // TR_ComplexFloat
   TR_ixdstore,   // TR_ComplexDouble
   TR_ixestore,   // TR_ComplexLongDouble
   TR_iestore,    // TR_LongDouble
#else
   TR_BadILOp,    // TR_ComplexFloat
   TR_BadILOp,    // TR_ComplexDouble
   TR_BadILOp,    // TR_ComplexLongDouble
   TR_BadILOp,     // TR_LongDouble
#endif

#if (defined(SUPPORT_WCODE) || defined(ENABLE_INTRINSICS))
   TR_ipdstore,      // TR_PackedDecimal
   TR_izdstore,      // TR_ZonedDecimal
   TR_izdsleStore,   // TR_ZonedDecimalSignLeadingEmbedded
   TR_izdslsStore,   // TR_ZonedDecimalSignLeadingSeparate
   TR_izdstsStore,   // TR_ZonedDecimalSignTrailingSeparate
   TR_iudStore,      // TR_UnicodeDecimal
   TR_iudslStore,    // TR_UnicodeDecimalSignLeading
   TR_iudstStore,    // TR_UnicodeDecimalSignTrailing
   TR_zfStore,    // TR_ZonedFloat
   TR_ufStore,    // TR_UnicodeFloat
#else
   TR_BadILOp,    // TR_PackedDecimal
   TR_BadILOp,    // TR_ZonedDecimal
   TR_BadILOp,    // TR_ZonedDecimalSignLeadingEmbedded
   TR_BadILOp,    // TR_ZonedDecimalSignLeadingSeparate
   TR_BadILOp,    // TR_ZonedDecimalSignTrailingSeparate
   TR_BadILOp,    // TR_UnicodeDecimal
   TR_BadILOp,    // TR_UnicodeDecimalSignLeading
   TR_BadILOp,    // TR_UnicodeDecimalSignTrailing
   TR_BadILOp,    // TR_ZonedFloat
   TR_BadILOp,    // TR_UnicodeFloat
#endif
   TR_BadILOp     // TR_Alet
#if defined(ENABLE_SIMD)
   ,TR_ivistore  // TR_VectorInt32
   ,TR_ivdstore  // TR_VectorDouble
#endif
   };

// psuedo-call to asm function
extern "C" void _getSTFLEBits(int numDoubleWords, uint64_t * bits);  /* 390 asm stub */
extern "C" bool _isPSWInProblemState();  /* 390 asm stub */

extern U_8 *alignEndToMachineWord(U_8 *ptr, U_32 size, U_32 *newSize);

// Returns -1 if given vmThread is not a compilation thread
int32_t
TR_J9VMBase::getCompThreadIDForVMThread(void *vmThread)
   {
   int32_t id = -1; // unknown
   if (vmThread)
      {
      if (_vmThread == (J9VMThread*)vmThread)
         {
         if (_vmThreadIsCompilationThread == TR_yes)
            {
            assume(_compInfoPT, "if this is a verified fe for a comp thread, _compInfoPT must exist");
            id = _compInfoPT->getCompThreadId();
            }
         else if (_vmThreadIsCompilationThread == TR_maybe) // Let's find out the compilation thread and cache it
            {
            _compInfoPT = _compInfo->getCompInfoForThread((J9VMThread*)vmThread);
            id = _compInfoPT->getCompThreadId();
            }
         }
      else // Thread given as parameter is unrelated to this frontEnd
         {
         TR_CompilationInfoPerThread * cipt = _compInfo->getCompInfoForThread((J9VMThread*)vmThread);
         id = (cipt ? cipt->getCompThreadId() : -1);
         }
      }
   return id;
   }

bool
TR_J9VMBase::stackWalkerMaySkipFrames(TR_OpaqueMethodBlock *method, TR_OpaqueClassBlock *methodClass)
   {
   if(!method)
      return false;

   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();

         // Maybe I should be using isSameMethod ?
   if (vmThread()->javaVM->jlrMethodInvoke == NULL || ((J9Method *)method) == vmThread()->javaVM->jlrMethodInvoke )
      {
      releaseVMAccessIfNeeded(haveAcquiredVMAccess);
      return true;
      }

   if(!methodClass)
      {
      releaseVMAccessIfNeeded(haveAcquiredVMAccess);
      return false;
      }

   if ( ( vmThread()->javaVM->jlrAccessibleObject != NULL) &&
          isInstanceOf( methodClass, (TR_OpaqueClassBlock*) J9VM_J9CLASS_FROM_JCLASS(vmThread(), vmThread()->javaVM->jlrAccessibleObject),false) )
      {
      releaseVMAccessIfNeeded(haveAcquiredVMAccess);
      return true;
      }


#if defined(J9VM_OPT_SIDECAR)
   if ( ( vmThread()->javaVM->srMethodAccessor != NULL) &&
          isInstanceOf(methodClass, (TR_OpaqueClassBlock*) J9VM_J9CLASS_FROM_JCLASS(vmThread(), vmThread()->javaVM->srMethodAccessor), false) )
      {
      releaseVMAccessIfNeeded(haveAcquiredVMAccess);
      return true;
      }

   if ( ( vmThread()->javaVM->srConstructorAccessor != NULL) &&
          isInstanceOf(methodClass, (TR_OpaqueClassBlock*) J9VM_J9CLASS_FROM_JCLASS(vmThread(), vmThread()->javaVM->srConstructorAccessor), false) )
      {
      releaseVMAccessIfNeeded(haveAcquiredVMAccess);
      return true;
      }
#endif // J9VM_OPT_SIDECAR

   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return false;
   }


// Points to address: what if vmThread is not the compilation thread.
// What if the compilation thread does not have the classUnloadMonitor.
// What if jitConfig does not exist.
// What if we already have the compilationShouldBeInterrupted flag set.
// How do we set the error code when we set outOfMemory

// IMPORTANT: acquireVMAccessIfNeeded could throw j9OutOfMemory, hence it is important to have
// a valid jump buffer setup prior to calling it
bool acquireVMaccessIfNeeded(J9VMThread *vmThread, TR_YesNoMaybe isCompThread)
      {
      bool haveAcquiredVMAccess = false;
      if (TR_Options::getCmdLineOptions() == 0 || // if options haven't been created yet, there is no danger
          TR_Options::getCmdLineOptions()->getOption(TR_DisableNoVMAccess))
          return false; // don't need to acquire VM access

      // we need to test if the thread has VM access
      assume(vmThread, "vmThread must be not null");
      TR_CompilationInfoPerThread *compInfoPT = NULL;
      // We need to acquire VMaccess only for the compilation thread
      if (isCompThread == TR_no)
         {
#ifdef J9VM_GC_COMBINATION_SPEC
//         assume((vmThread->publicFlags &  J9_PUBLIC_FLAGS_VM_ACCESS) || (TR_Options::getCmdLineOptions()->getGcMode() == TR_WrtbarCardMarkIncremental), "Must have vm access if isCompThread==Tr_no");
#else
         assume((vmThread->publicFlags &  J9_PUBLIC_FLAGS_VM_ACCESS) || TR_Options::getCmdLineOptions()->regionBasedGC(), "Must have vm access if isCompThread==Tr_no");
#endif
         return false;
         }
      else if (isCompThread == TR_maybe)
         {
         // Test if the current thread is a compilation thread
         J9JITConfig        *jitConfig = vmThread->javaVM->jitConfig;
         TR_CompilationInfo *compInfo  = TR_CompilationInfo::get(jitConfig);
         // scan all compilation threads
         compInfoPT = compInfo->getCompInfoForThread(vmThread);
         if (!compInfoPT)
            {
            assume((vmThread->publicFlags & J9_PUBLIC_FLAGS_VM_ACCESS), "Must have vm access if this is not a compilation thread");
            return false;
            }
         }
      // At this point we know we deal with a compilation thread
      if (!(vmThread->publicFlags &  J9_PUBLIC_FLAGS_VM_ACCESS)) // I don't already have VM access
         {
         if (0 == vmThread->javaVM->internalVMFunctions->internalTryAcquireVMAccessWithMask(vmThread, J9_PUBLIC_FLAGS_HALT_THREAD_ANY_NO_JAVA_SUSPEND))
            {
            haveAcquiredVMAccess = true;
            }
         else // the GC is having exclusive VM access
            {
            // compilationShouldBeInterrupted flag is reset by the compilation
            // thread when a new compilation starts.

            // must test if we have the class unload monitor
            //vmThread->osThread == classUnloadMonitor->owner()
            J9JITConfig        *jitConfig = vmThread->javaVM->jitConfig;
            TR_CompilationInfo *compInfo  = TR_CompilationInfo::get(jitConfig);
            if (!compInfoPT)
               compInfoPT = compInfo->getCompInfoForThread(vmThread);
            // TODO: save the compInfoPT in the fe and then pass it as a parameter to this function
            assume(compInfoPT, "A compilation thread must have an associated compilationInfo");
            bool hadClassUnloadMonitor = false;
#if !defined(J9VM_GC_DYNAMIC_CLASS_UNLOADING)
            if (TR_Options::getCmdLineOptions()->getOption(TR_EnableHCR) || TR_Options::getCmdLineOptions()->getOption(TR_FullSpeedDebug))
#endif
               hadClassUnloadMonitor = TR_J9MonitorTable::get()->readReleaseClassUnloadMonitor(compInfoPT->getCompThreadId()) >= 0;
#if defined(J9VM_GC_DYNAMIC_CLASS_UNLOADING)
            // We must have had classUnloadMonitor by the way we architected the application
            assume(hadClassUnloadMonitor, "Comp thread must hold classUnloadMonitor when compiling without VMaccess");
#endif

            //--- GC CAN INTERVENE HERE ---

            // fprintf(stderr, "Have released class unload monitor temporarily\n"); fflush(stderr);

            // At this point we must not hold any JIT monitors that can also be accessed by the GC
            // As we don't know how the GC code will be modified in the future we will
            // scan the entire list of known monitors
#if defined(DEBUG) || defined(PROD_WITH_ASSUMES)
            TR_J9Monitor * heldMonitor = TR_J9MonitorTable::get()->monitorHeldByCurrentThread();
            assume3(!heldMonitor, "Current thread must not hold TR_J9Monitor %p called %s when trying to acquire VM access\n", 
                    heldMonitor, TR_J9VMBase::get(jitConfig, NULL)->getJ9MonitorName((J9ThreadMonitor*)heldMonitor->getVMMonitor()));
#endif // #if defined(DEBUG) || defined(PROD_WITH_ASSUMES)   

	    if (TR_Options::getCmdLineOptions()->realTimeGC())
               compInfoPT->waitForGCCycleMonitor(false); // used only for real-time

            acquireVMAccessNoSuspend(vmThread);   // blocking. Will wait for the entire GC
            if (hadClassUnloadMonitor)
               {
               TR_J9MonitorTable::get()->readAcquireClassUnloadMonitor(compInfoPT->getCompThreadId());
               //fprintf(stderr, "Have acquired class unload monitor again\n"); fflush(stderr);
               }

            // Now we can check if the GC has done some unloading or redefinition happened
            if (compInfoPT->compilationShouldBeInterrupted())
               {
               // bail out
               // fprintf(stderr, "Released classUnloadMonitor and will throw outOfMemory because GC unloaded classes\n"); fflush(stderr);
               //TR_J9MonitorTable::get()->readReleaseClassUnloadMonitor(0); // Main code should do it.
               // releaseVMAccess(vmThread);

               // Set the error code; we must be compiling something otherwise there is no point
               // in setting an error code. For multiple compilation threads we must have several
               // compInfo objects.
               TR_Compilation *comp = compInfoPT->getCompilation();
               if (comp)
                  {
                  comp->setErrorCode(COMPILATION_INTERRUPTED);
                  }
               else // I am not sure we are in a compilation; better release the monitor
                  {
                  if (hadClassUnloadMonitor)
                     TR_J9MonitorTable::get()->readReleaseClassUnloadMonitor(compInfoPT->getCompThreadId()); // Main code should do it.
                  }
               j9OutOfMemory(jitConfig, comp, "Compilation interrupted by GC unloading classes", compInfoPT->getJumpBuffer());
               }
            else // GC did not do any unloading
               {
               haveAcquiredVMAccess = true;
               }
            }
         }
      return haveAcquiredVMAccess;
      }

void releaseVMaccessIfNeeded(J9VMThread *vmThread, bool haveAcquiredVMAccess)
   {
   if (haveAcquiredVMAccess)
      {
      assume((vmThread->publicFlags &  J9_PUBLIC_FLAGS_VM_ACCESS), "Must have VM access"); // I don't already have VM access
      releaseVMAccess(vmThread);
      }
   }

void
TR_J9VMBase::releaseVMAccessIfNeeded(bool haveAcquiredVMAccess)
   {
   if (_vmThreadIsCompilationThread)
      {
      assume(_compInfoPT, "_compInfoPT must exist for a compilation thread\n");
      if (_compInfoPT->compilationShouldBeInterrupted())
         {
         assume2(_compInfoPT->compilationShouldBeInterrupted() != GC_COMP_INTERRUPT, "GC should not have cut in _compInfoPT=%p\n", _compInfoPT);
         // in prod builds take some corrective action
         TR_Compilation *comp = _compInfoPT->getCompilation();
         if (comp)
            comp->setErrorCode(COMPILATION_INTERRUPTED);
         // At shutdown time the compilation thread executes Java code and it may receive a sample (see D174900)
         // In that case there is no jumpbuffer set
         if (_compInfoPT->getJumpBuffer())
            {
            j9OutOfMemory(jitConfig, comp, "Compilation interrupted by shutdown", _compInfoPT->getJumpBuffer());
            }
         }
      }
   releaseVMaccessIfNeeded(vmThread(), haveAcquiredVMAccess);
   }

bool TR_J9VMBase::cachedStaticDFPAvailField = false;
int32_t * TR_J9VMBase::staticDFPHWAvailField = NULL;

bool TR_J9VMBase::cachedStaticAMRDCASAvailField = false;
int32_t * TR_J9VMBase::staticAMRDCASAvailField = NULL;

bool TR_J9VMBase::cachedStaticAMRDSetAvailField = false;
int32_t * TR_J9VMBase::staticAMRDSetAvailField = NULL;

bool TR_J9VMBase::cachedStaticASRDCASAvailField = false;
int32_t * TR_J9VMBase::staticASRDCASAvailField = NULL;

bool TR_J9VMBase::cachedStaticASRDSetAvailField = false;
int32_t *  TR_J9VMBase::staticASRDSetAvailField = NULL;


bool
TR_J9VMBase::releaseClassUnloadMonitorAndAcquireVMaccessIfNeeded(TR_Compilation *comp, bool *hadClassUnloadMonitor)
   {
  // When allocating a new code cache we must not hold any monitor because
   // of deadlock possibilities
   // This must be executed on the compilation thread so options must exist at this point
   assume(comp, "Compilation object must always be given as parameter\n");
   *hadClassUnloadMonitor = false;
   bool hadVMAccess = true;
   if (!TR_Options::getCmdLineOptions()->getOption(TR_DisableNoVMAccess))
      {
      assume1c(_vmThreadIsCompilationThread != TR_no, comp, "releaseClassUnloadMonitorAndAcquireVMaccessIfNeeded should only be called for compilation threads\n");
      // We need to acquire VMaccess only for the compilation thread
      assume3c(vmThread()==_jitConfig->javaVM->internalVMFunctions->currentVMThread(_jitConfig->javaVM), comp,
        "fe for thread %p is used on thread %p", vmThread(),
        _jitConfig->javaVM->internalVMFunctions->currentVMThread(_jitConfig->javaVM));
      assume1c(_compInfoPT, comp, "_compInfoPT must exist here\n");
      assume0c(_compInfo->getCompInfoForThread(vmThread()) == _compInfoPT, comp);

      if (_vmThreadIsCompilationThread == TR_maybe)
         {
         _vmThreadIsCompilationThread = TR_yes;
         }

#if defined(J9VM_GC_DYNAMIC_CLASS_UNLOADING)
      *hadClassUnloadMonitor = TR_J9MonitorTable::get()->readReleaseClassUnloadMonitor(_compInfoPT->getCompThreadId()) >= 0;
      assume1c(*hadClassUnloadMonitor, comp, "releaseClassUnloadMonitorAndAcquireVMaccessIfNeeded without classUnloadMonitor\n");
#else
      // We need to use classUnloadMonitor to stop the Compilation Thread when a class is redefined
      if (TR_Options::getCmdLineOptions()->getOption(TR_EnableHCR) || TR_Options::getCmdLineOptions()->getOption(TR_FullSpeedDebug))
         {
         *hadClassUnloadMonitor = TR_J9MonitorTable::get()->readReleaseClassUnloadMonitor(_compInfoPT->getCompThreadId()) >= 0;
         assume1c(*hadClassUnloadMonitor, comp, "releaseClassUnloadMonitorAndAcquireVMaccessIfNeeded without classUnloadMonitor\n");
         }
#endif

      if (!(vmThread()->publicFlags &  J9_PUBLIC_FLAGS_VM_ACCESS)) // I don't already have VM access
         {
         hadVMAccess = false;

#if defined(DEBUG) || defined(PROD_WITH_ASSUMES)
         TR_J9Monitor * heldMonitor = TR_J9MonitorTable::get()->monitorHeldByCurrentThread();
         assume3c(!heldMonitor, comp, "Current thread must not hold TR_J9Monitor %p called %s when trying to acquire VM access\n", 
                  heldMonitor, getJ9MonitorName((J9ThreadMonitor*)heldMonitor->getVMMonitor()));
#endif // #if defined(DEBUG) || defined(PROD_WITH_ASSUMES)   

         //--- GC can happen here ---

         acquireVMAccessNoSuspend(vmThread());
         // If the GC unloaded some classes, or HCR happened we must abort this compilation
         if (_compInfoPT->compilationShouldBeInterrupted())
            {
            comp->setErrorCode(COMPILATION_INTERRUPTED);
            //releaseVMAccess(vmThread()); // release VM access before blocking on the next acquire operation
            //if (*hadClassUnloadMonitor)
            //   TR_J9MonitorTable::get()->readAcquireClassUnloadMonitor(_compInfoPT->getCompThreadId());
            outOfMemory(comp, "Compilation interrupted");
            // After the longjump we will release the classUnloadMonitor and re-acquire the VM access
            }
         }
      else
         {
         hadVMAccess = true;
         }
      }
      return hadVMAccess;
   }


void
TR_J9VMBase::acquireClassUnloadMonitorAndReleaseVMAccessIfNeeded(TR_Compilation *comp, bool hadVMAccess, bool hadClassUnloadMonitor)
   {
   if (TR_Options::getCmdLineOptions() && // if options haven't been created yet, there is no danger
       !TR_Options::getCmdLineOptions()->getOption(TR_DisableNoVMAccess))
      {
      assume(vmThread()->publicFlags &  J9_PUBLIC_FLAGS_VM_ACCESS, "vmThread must have vmAccess at this point");
      if (_compInfoPT->compilationShouldBeInterrupted())
         {
         comp->setErrorCode(COMPILATION_INTERRUPTED);
         outOfMemory(comp, "Compilation interrupted");
         }
#if defined(J9VM_GC_DYNAMIC_CLASS_UNLOADING)
      if (hadClassUnloadMonitor)
         TR_J9MonitorTable::get()->readAcquireClassUnloadMonitor(_compInfoPT->getCompThreadId());
#else
      // We need to use classUnloadMonitor to stop the Compilation Thread when a class is redefined
      if ((TR_Options::getCmdLineOptions()->getOption(TR_EnableHCR) || TR_Options::getCmdLineOptions()->getOption(TR_FullSpeedDebug)) && hadClassUnloadMonitor)
         TR_J9MonitorTable::get()->readAcquireClassUnloadMonitor(_compInfoPT->getCompThreadId());
#endif // J9VM_GC_DYNAMIC_CLASS_UNLOADING
      if (!hadVMAccess)
         releaseVMAccess(vmThread());
      }
   }

#if !defined(NEW_CLASS_CHAINS)
// convert the 32bit offset into a string of 8 characters
void TR_J9VMBase::convertUnsignedToASCII(uint32_t offset, char *myBuffer)
   {
   for (int i = 7; i >= 0; i--, offset >>= 4)
      {
      uint8_t myNibble = offset & 0xf;
      myBuffer[i] = (myNibble > 9 ?  myNibble - 10 + 'a' :  myNibble + '0');
      }
   myBuffer[8] = 0;
   }

#if defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390) || defined(TR_HOST_ARM))
bool
TR_J9VMBase::findSharedCacheContainingPointer(void *ptr, int32_t & cacheListIndex, uint32_t & cacheOffset)
   {
   bool firstTime = true;
   bool found = false;
   J9SharedClassConfig * scConfig = _jitConfig->javaVM->sharedClassConfig;
   J9SharedClassCacheDescriptor* desc = scConfig->cacheDescriptorList;
   for (cacheListIndex = 0; !found && (desc != scConfig->cacheDescriptorList || firstTime); cacheListIndex++, desc=desc->next)
      {
      firstTime = false;
      TR_RelocationRuntime *rr;
      if (_compInfoPT)
         rr = _compInfoPT->reloRuntime();
      else
         rr = TR_CompilationInfo::get(_jitConfig)->reloRuntime();
      assume(rr, "Failed to get a relocation runtime");
      if (rr->isROMClassInSharedCaches((uintptrj_t)ptr, _jitConfig->javaVM))
         {
         found = true;
         cacheOffset = (uint8_t *) ptr - (uint8_t *) desc->cacheStartAddress;
         }
      }

   return found;
   }

bool TR_J9VMBase::getRomPtrHashString(void *romPtr, J9SharedClassConfig * scConfig,
                                         char *myBuffer, uint32_t &bufferLength, J9Method *method)
   {
   //J9ROMMethod * romMethod = J9_ROM_METHOD_FROM_RAM_METHOD(method);
   uint32_t romMethodOffset;
   int32_t scIndex;

   if (findSharedCacheContainingPointer(romPtr, scIndex, romMethodOffset))
      {
      assume(bufferLength > 17, "buffer not big enough to contain hash string");
      // convert the offset into a string of 8 characters
      convertUnsignedToASCII(scIndex, myBuffer);
      convertUnsignedToASCII(romMethodOffset, myBuffer+8);
      bufferLength = 16;
      }
   else
      {
      if (method)
         {
         J9UTF8 * className;
         J9UTF8 * name;
         J9UTF8 * signature;
         getClassNameSignatureFromMethod(method, className, name, signature);
         uint32_t sigLen = J9UTF8_LENGTH(className) + J9UTF8_LENGTH(name) + J9UTF8_LENGTH(signature) + 2;
         if (sigLen >= bufferLength)
            return false;

         bufferLength = printTruncatedSignature(myBuffer, bufferLength, (TR_OpaqueMethodBlock *) method);
         }
      else
         {
         return false;
         }
      }
   return true;
   }

int32_t TR_J9VMBase::addSharedCacheClassChain(UDATA *hierarchy, int32_t numHierarchyElements, J9ROMClass *romClass)
   {
#if defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390) || defined(TR_HOST_ARM))
   static bool SCfull = false;
   if (SCfull)
      return 1;

   if (TR_Options::sharedClassCache())        // shared classes and AOT must be enabled
      {
      J9SharedClassConfig * scConfig = _jitConfig->javaVM->sharedClassConfig;
      J9VMThread * vmThread = _vmThread ? _vmThread : getCurrentVMThread();

      J9SharedDataDescriptor dataDescriptor;
      dataDescriptor.address = (U_8*)hierarchy;
      dataDescriptor.length = numHierarchyElements * sizeof(UDATA);
      dataDescriptor.type = J9SHR_DATA_TYPE_AOTCLASSCHAIN;
      dataDescriptor.flags = J9SHRDATA_SINGLE_STORE_FOR_KEY_TYPE;

      char myBuffer[500];
      uint32_t bufferLength = 500;

      if (!getRomPtrHashString(romClass, scConfig, myBuffer, bufferLength, NULL))
         return 1;

      if (TR_Options::getAOTCmdLineOptions()->getOption(TR_EnableAOTStats))
         ((TR_JitPrivateConfig *)_jitConfig->privateConfig)->aotStats->numNewCHEntriesInSharedClass++;
      const void* store = scConfig->storeSharedData(vmThread, (const char*)myBuffer, bufferLength, &dataDescriptor);

      if (!store)
         return 1;//this method returns 1 of the class chain couldn't be added scc (regardless of the reason).
         //SCfull = true;
      }
   else
      {
      return 1;
      }

#endif // defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390))

   return 0;
   }

uint8_t * TR_J9VMBase::getExistingClassChain(void * vmThread, uint8_t *ramclass)
   {
   J9Class *j9Class = (J9Class *)ramclass;
   J9ROMClass *romClass = ((J9Class*)j9Class)->romClass;
   J9SharedDataDescriptor firstDescriptor;
   J9SharedClassCacheDescriptor *currentCacheDescriptor = _jitConfig->javaVM->sharedClassConfig->cacheDescriptorList;
   UDATA romStartAddress = (UDATA)currentCacheDescriptor->cacheStartAddress; // assume only 1 descriptor

   firstDescriptor.address = NULL;

   char myBuffer[500];
   uint32_t bufferLength = 500;;
   if (!getRomPtrHashString(romClass, _jitConfig->javaVM->sharedClassConfig, myBuffer, bufferLength, NULL))
      return NULL;

   //fprintf(stderr, "Have loaded AOT j9method %p Searching key %.*s\n", entry->_method, 8, myBuffer);
   _jitConfig->javaVM->sharedClassConfig->findSharedData((J9VMThread *)vmThread, (const char*)myBuffer, bufferLength, J9SHR_DATA_TYPE_AOTCLASSCHAIN,
                            FALSE, &firstDescriptor, NULL);

   if (firstDescriptor.address)
      {
      UDATA *hierarchyChain = (UDATA *)firstDescriptor.address;
      UDATA numElements = hierarchyChain[0];

      int superclassCount, currentSuperclassIndex;
      int32_t numHierarchyElements;
      J9Class **superclasses;
      J9Class *currentClass;
      superclasses = j9Class->superclasses;
      superclassCount = j9Class->classDepthAndFlags & J9_JAVA_CLASS_DEPTH_MASK;

      numHierarchyElements = superclassCount;
      currentSuperclassIndex = superclassCount - 1;

      int32_t hierarchyIndex = 1;

      if (numElements != (UDATA)(numHierarchyElements -1))
         {
         kprintf("numElements not equal!  numElements: %d, numHierarchyElements -1: %d\n", numElements, numHierarchyElements - 1); fflush(stdout);
         return NULL;
         }

      while (currentSuperclassIndex > 0)  // ">=0" then includes object, ">0" doesn't include
         {
         kprintf("currentSuperclass: %d\n", currentSuperclassIndex); fflush(stdout);
         currentClass = superclasses[currentSuperclassIndex];

         if (hierarchyChain[hierarchyIndex] != ((UDATA)currentClass->romClass - romStartAddress))
            {
             kprintf("romClass not same!  hierarchyChain[hierarchyIndex]: %x, ((UDATA)currentClass->romClass - romStartAddress): %x\n", hierarchyChain[hierarchyIndex], ((UDATA)currentClass->romClass - romStartAddress));
           if (TR_Options::getAOTCmdLineOptions()->getOption(TR_EnableAOTStats))
              ((TR_JitPrivateConfig *)_jitConfig->privateConfig)->aotStats->numCHInSharedCacheButFailValiation++;
            return NULL;
            }

         currentSuperclassIndex--;
         hierarchyIndex++;
         }

      kprintf("chain found and validated same!\n"); fflush(stdout);
      return (uint8_t *)hierarchyChain;
      }
   else
      {
       return NULL;
      }
   }
#endif // defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390))
#endif // !defined(NEW_CLASS_CHAINS)

#if !defined(HINTS_IN_SHAREDCACHE_OBJECT)
uint32_t TR_J9VMBase::getSharedCacheHint(J9VMThread * vmThread, J9Method *method, J9SharedClassConfig * scConfig)
   {
   J9ROMMethod * romMethod = J9_ROM_METHOD_FROM_RAM_METHOD(method);

   unsigned char storeBuffer[4];
   uint32_t bufferLength = 4;
   J9SharedDataDescriptor descriptor;
   descriptor.address = storeBuffer;
   descriptor.length = bufferLength;
   descriptor.type = J9SHR_ATTACHED_DATA_TYPE_JITHINT;
   descriptor.flags = J9SHR_ATTACHED_DATA_NO_FLAGS;
   IDATA dataIsCorrupt;
   const U_8 *store = scConfig->findAttachedData(vmThread, romMethod, &descriptor, &dataIsCorrupt);
   
   if ((store != (U_8 *)descriptor.address) || (dataIsCorrupt != -1))
      return 0;
   else
      return *((uint32_t *)store);
   }


uint16_t TR_J9VMBase::getAllSharedCacheHints(J9Method *method)
   {
#if defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390) || defined(TR_HOST_ARM))
   if (TR_Options::sharedClassCache() &&      // shared classes and AOT must be enabled
       !TR_Options::getAOTCmdLineOptions()->getOption(TR_DisableSharedCacheHints) &&
       TR_Options::getAOTCmdLineOptions()->getEnableSCHintFlags()!=0)
      {
      J9VMThread * vmThread = _vmThread ? _vmThread : getCurrentVMThread();
      uint32_t scHints = getSharedCacheHint(vmThread, method, _jitConfig->javaVM->sharedClassConfig);
      uint16_t SChintFlags = *((uint16_t *)&scHints);
      // mask the hints that we are not supposed to see
      SChintFlags &= (uint16_t)TR_Options::getAOTCmdLineOptions()->getEnableSCHintFlags();
      return SChintFlags;
      }
#endif // defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390) || defined(TR_HOST_ARM))
   return 0;
   }


bool TR_J9VMBase::isSharedCacheHint(J9Method *method, TR_SharedCacheHint hint, uint16_t *dataField)
   {
#if defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390) || defined(TR_HOST_ARM))
   if (TR_Options::sharedClassCache()       // shared classes and AOT must be enabled
       && !TR_Options::getAOTCmdLineOptions()->getOption(TR_DisableSharedCacheHints)
       && ((hint & TR_Options::getAOTCmdLineOptions()->getEnableSCHintFlags()) != 0)
       && hint != TR_NoHint)
      {
      J9ROMMethod * romMethod = J9_ROM_METHOD_FROM_RAM_METHOD(method);
      if (!romMethod)
         return false;

      J9VMThread * vmThread = _vmThread ? _vmThread : getCurrentVMThread();
      J9SharedClassConfig * scConfig = _jitConfig->javaVM->sharedClassConfig;
      uint32_t scHints = getSharedCacheHint(vmThread, method, scConfig);
      uint16_t SChintFlags = *((uint16_t *)&scHints);

      if (dataField)
         *dataField = *((uint16_t *)&scHints + 1);

      if (TR_Options::getCmdLineOptions()->getVerboseOption(TR_VerboseSCHints))
         {
         char buffer[500];
         printTruncatedSignature(buffer, 500, (TR_OpaqueMethodBlock *) method);
         TR_CompilationInfo * compInfo = TR_CompilationInfo::get(_jitConfig);
         TR_VerboseLog::writeLineLocked(TR_Vlog_SCHINTS,"is hint %x(%x) %s", SChintFlags, hint, buffer);
         }
      return (SChintFlags & hint) != 0;
      }
#endif // defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390))
   return false;
}

void TR_J9VMBase::addSharedCacheHint(J9Method * method, TR_SharedCacheHint hint)
   {
#if defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390) || defined(TR_HOST_ARM))
   static bool SCfull = false;
   if (SCfull)
      {
      return;
      }

   if (TR_Options::sharedClassCache()        // shared classes and AOT must be enabled
       && !TR_Options::getAOTCmdLineOptions()->getOption(TR_DisableSharedCacheHints)
       && ((hint & TR_Options::getAOTCmdLineOptions()->getEnableSCHintFlags()) != 0)
       && hint != TR_NoHint)
      {
      J9ROMMethod * romMethod = J9_ROM_METHOD_FROM_RAM_METHOD(method);
      J9SharedClassConfig * scConfig = _jitConfig->javaVM->sharedClassConfig;
      J9VMThread * vmThread = _vmThread ? _vmThread : getCurrentVMThread();

      char myBuffer[500];
      uint32_t bufferLength = 500;

      bool isFailedValidationHint = (hint == TR_HintFailedValidation);

      if (TR_Options::getCmdLineOptions()->getVerboseOption(TR_VerboseSCHints))
         {
         if (romMethod)
            {
            printTruncatedSignature(myBuffer, 500, (TR_OpaqueMethodBlock *) method);
            TR_CompilationInfo * compInfo = TR_CompilationInfo::get(_jitConfig);
            TR_VerboseLog::writeLineLocked(TR_Vlog_SCHINTS,"adding hint 0x%x %s", hint, myBuffer);
            }
         }

      // There is only one scenario where concurrency *may* matter, so we don't get a lock
      // The scenario is where a compilation thread wants to register a hint about the method it's compiling at
      // the same time that another thread is inling it *for the first time*. In that case, one of the hints
      // won't be registered. The affect, however is minimal, and likely to correct itself in the current run
      // (if the inlining hint is missed) or a subsequent run (if the other hint is missed).
      uint32_t scHints = getSharedCacheHint(vmThread, method, scConfig);
      uint16_t *flagsToBeWritten = (uint16_t *)&scHints;
      uint16_t *newScount = (uint16_t *)&scHints + 1; // Flags and new scount field needs to be in contiguous location to be stored into sharecache
      bool increaseHintCount = false;
      bool existsHintAlready = (*flagsToBeWritten != 0);

      if (hint & *flagsToBeWritten)
         { // Exists this hint already
         if (!isFailedValidationHint)
            {
            return;
            }
         else
            { // If exists a failed validation hint already, then find the current scount and increase it
            if (!(*newScount == TR_DEFAULT_INITIAL_COUNT)) // not yet reached cap scount
               {
               increaseHintCount = true;
               *newScount *= 10;
               }
            }
         }
      else if (isFailedValidationHint)
         { // Creating a new hint for failed validation.  Note that if a user specifies counts explicitly, then the user specified count is obeyed
         increaseHintCount = true;
         *newScount = 10 * MIN(TR_Options::getCmdLineOptions()->getInitialSCount(), TR_Options::getAOTCmdLineOptions()->getInitialSCount());
         if (*newScount == 0) *newScount = 10;
         }

      if (isFailedValidationHint && (*newScount > TR_DEFAULT_INITIAL_COUNT))
         { // Cap the new scount
         *newScount = TR_DEFAULT_INITIAL_COUNT;
         }

      if (!existsHintAlready)
         {
         *flagsToBeWritten |= hint;

         // store in the shared cache
         J9SharedDataDescriptor descriptor;
         descriptor.address = (U_8*)flagsToBeWritten;
         descriptor.length = 4; // Size includes the 2nd data field, currently only used for TR_HintFailedValidation
         descriptor.type = J9SHR_ATTACHED_DATA_TYPE_JITHINT;
         descriptor.flags = J9SHR_ATTACHED_DATA_NO_FLAGS;
         UDATA store = scConfig->storeAttachedData(vmThread, romMethod, &descriptor, 0);

         if (TR_Options::getCmdLineOptions()->getVerboseOption(TR_VerboseSCHints))
            {
            TR_CompilationInfo * compInfo = TR_CompilationInfo::get(jitConfig);

            if (store == 0)
               {
               TR_VerboseLog::writeLineLocked(TR_Vlog_SCHINTS,"hint added 0x%x (%p), key = %s, scount: %d", *flagsToBeWritten, store, myBuffer, *newScount);
               }
            else if (store != J9SHR_RESOURCE_STORE_FULL)
               {
               TR_VerboseLog::writeLineLocked(TR_Vlog_SCHINTS,"hint error: could not be added into SC");
               }
            else
               {
               SCfull = true;
               TR_VerboseLog::writeLineLocked(TR_Vlog_SCHINTS,"hint error: SCC full");
               }
            }
         }
      else
         {
         if (!isFailedValidationHint || (isFailedValidationHint && increaseHintCount))
            {
            *flagsToBeWritten |= hint;

            // store in the shared cache
            J9SharedDataDescriptor descriptor;
            descriptor.address = (U_8*)flagsToBeWritten;
            descriptor.length = 4; // Size includes the 2nd data field, currently only used for TR_HintFailedValidation
            descriptor.type = J9SHR_ATTACHED_DATA_TYPE_JITHINT;
            descriptor.flags = J9SHR_ATTACHED_DATA_NO_FLAGS;
            UDATA store = scConfig->updateAttachedData(vmThread, romMethod, 0, &descriptor); // update the existing attached data

            if (TR_Options::getCmdLineOptions()->getVerboseOption(TR_VerboseSCHints))
               {
               TR_CompilationInfo * compInfo = TR_CompilationInfo::get(jitConfig);

               if (store == 0)
                  {
                  TR_VerboseLog::writeLineLocked(TR_Vlog_SCHINTS,"hint updated 0x%x (%p), key = %s, scount: %d", *flagsToBeWritten, store, myBuffer, *newScount);
                  }
               else
                  {
                  TR_VerboseLog::writeLineLocked(TR_Vlog_SCHINTS,"hint error: could not be updated into SC");
                  }
               }
            }
         else
            {
            if (TR_Options::getCmdLineOptions()->getVerboseOption(TR_VerboseSCHints))
               {
               TR_CompilationInfo * compInfo = TR_CompilationInfo::get(jitConfig);
               TR_VerboseLog::writeLineLocked(TR_Vlog_SCHINTS,"hint reached max count of %d", *newScount);
               }
            }
         }
      }

#endif // defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390))
   }


void
TR_J9VMBase::persistIprofileInfo(TR_ResolvedMethodSymbol *method, TR_Compilation *comp)
   {
   if (getIProfiler())
      getIProfiler()->persistIprofileInfo(method, comp);
   }

bool
TR_J9VMBase::isSharedCacheMostlyFull()
   {
   J9JavaVM * javaVM = _jitConfig->javaVM;
   return (double) javaVM->sharedClassConfig->getFreeSpaceBytes(javaVM) / javaVM->sharedClassConfig->getCacheSizeBytes(javaVM) < 0.8;
   }
#endif // HINTS_IN_SHAREDCACHE_OBJECT

J9Class *
TR_J9VMBase::matchRAMclassFromROMclass(J9ROMClass * clazz, TR_Compilation * comp)
   {
   J9UTF8 *utf8 = J9ROMCLASS_CLASSNAME(clazz);
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   J9Class *ramClass = jitGetClassInClassloaderFromUTF8(vmThread(), ((TR_ResolvedJ9Method *)comp->getCurrentMethod())->getClassLoader(),
      (char *) J9UTF8_DATA(utf8), J9UTF8_LENGTH(utf8));
   if (!ramClass)
      {
      ramClass = jitGetClassInClassloaderFromUTF8(vmThread(), (J9ClassLoader *) vmThread()->javaVM->systemClassLoader,
         (char *) J9UTF8_DATA(utf8), J9UTF8_LENGTH(utf8));
      }
   comp->fe()->releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return ramClass;
   }

J9VMThread *
TR_J9VMBase::getCurrentVMThread()
   {
   return _vmThread ? _vmThread : _jitConfig->javaVM->internalVMFunctions->currentVMThread(_jitConfig->javaVM);
   }

#if defined(J9ZOS390)
/// TDB is Transaction Diagnostic Block used for Transactional Memory Debugging. It is a 256 Byte block stored in the J9VMThread and must be 8 byte boundary aligned.
uint16_t TR_J9VMBase::getTDBOffset()
   {
   /// This assume is valid to confirm the TDB is on an 8 byte boundary because we know the VMThread is already aligned on a 256 byte boundary
   /// If this assume fails, then that means someone modified the J9VMThread structure which has thrown off the alignment.
   assume(offsetof(J9VMThread, transactionDiagBlock)%8 == 0, "The Transaction Diagnostic Block must be aligned on a doubleword i.e. 8 byte boundary");
   return offsetof(J9VMThread, transactionDiagBlock);
   }
#endif

#if !defined(HINTS_IN_SHAREDCACHE_OBJECT)
bool TR_J9VMBase::isSharedCacheHint(TR_ResolvedMethod *method, TR_SharedCacheHint hint, uint16_t *dataField)
   {
#if defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390) || defined(TR_HOST_ARM))
   return isSharedCacheHint(((TR_ResolvedJ9Method *) method)->ramMethod(), hint, dataField);
#else
   return false;
#endif
}

void TR_J9VMBase::addSharedCacheHint(TR_ResolvedMethod * method, TR_SharedCacheHint hint)
   {
#if defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390) || defined(TR_HOST_ARM))
   addSharedCacheHint(((TR_ResolvedJ9Method *) method)->ramMethod(), hint);
#endif
   }
#endif


TR_J9VMBase *
TR_J9VMBase::get(J9JITConfig * jitConfig, J9VMThread * vmThread, VM_TYPE vmType)
   {
#if defined(J9VM_INTERP_AOT_COMPILE_SUPPORT) && !defined (J9VM_OPT_SHARED_CLASSES)
   assume(false, "Unsupported AOT configuration");
   return NULL;
#endif
   PORT_ACCESS_FROM_JITCONFIG(jitConfig);
   void* alloc = NULL;
   assume(vmThread || vmType==DEFAULT_VM, "Specific VM type ==> must supply vmThread");
   TR_J9VMBase * vmWithoutThreadInfo = (TR_J9VMBase *)jitConfig->compilationInfo;
#if defined(J9VM_INTERP_AOT_COMPILE_SUPPORT)
   TR_J9VMBase * aotVMWithoutThreadInfo = (TR_J9VMBase *)((J9AOTConfig*)jitConfig)->aotCompilationInfo;
#endif
   
   if (!vmWithoutThreadInfo)
      {
      enum
         {
         BEGIN_STARTUP,
         ALLOCATED_GLOBAL_VM,
#if defined(J9VM_INTERP_AOT_COMPILE_SUPPORT)
         ALLOCATED_GLOBAL_AOT_VM,
#endif
         ALLOCATED_COMPILATION_INFO,
         GLOBAL_ALLOCATION_COMPLETED
         } startupState; // This is to keep track of where we are in the allocation sequence
      do // while (false).  This is to have a common exit point.
         {
         startupState = BEGIN_STARTUP;
         jitConfig->compilationInfo = vmWithoutThreadInfo = new (PERSISTENT_NEW) TR_J9VM(jitConfig, NULL);
         if ( !vmWithoutThreadInfo )
            {
            break;
            }
         startupState = ALLOCATED_GLOBAL_VM;
#if defined(J9VM_INTERP_AOT_COMPILE_SUPPORT)
         if (vmType==AOT_VM /* TODO: || TR_Options::sharedClassCache() Would this make sense here? */)
            {
            aotVMWithoutThreadInfo = (TR_J9VMBase *)((J9AOTConfig*)jitConfig)->aotCompilationInfo;
            if (!aotVMWithoutThreadInfo)
               {
               ((J9AOTConfig*)jitConfig)->aotCompilationInfo = aotVMWithoutThreadInfo = new (PERSISTENT_NEW) TR_J9SharedCacheVM(jitConfig, NULL);
               if (!aotVMWithoutThreadInfo)
                  {
                  break;
                  }
               }
            startupState = ALLOCATED_GLOBAL_AOT_VM;
            }
#endif            
         if (!TR_CompilationInfo::get(jitConfig, 0) )
            {
            break;
            }
         startupState = ALLOCATED_COMPILATION_INFO;
         // Possibility for more stuff here later
         startupState = GLOBAL_ALLOCATION_COMPLETED;
         } while (false);
      switch (startupState)
         {
         case GLOBAL_ALLOCATION_COMPLETED :
            break;
         case ALLOCATED_COMPILATION_INFO :
            break; // To be modified if stuff is added later
         // From here on, cases will cascade into each other to unwind what we have done.
#if defined(J9VM_INTERP_AOT_COMPILE_SUPPORT)
         case ALLOCATED_GLOBAL_AOT_VM:
            if (vmType==AOT_VM /* TODO: || TR_Options::sharedClassCache() Would this make sense here? */)
               {
               ((J9AOTConfig*)jitConfig)->aotCompilationInfo = NULL;
               // j9mem_free_memory(aotVMWithoutThreadInfo); // Allocated with PERSISTENT_NEW
               }
#endif                  
         case ALLOCATED_GLOBAL_VM :
            jitConfig->compilationInfo = NULL;
            //j9mem_free_memory(vmWithoutThreadInfo);  // Allocated with PERSISTENT_NEW // Should there be a destroyVM() function to match createVM()?
         case BEGIN_STARTUP :
            return NULL;
            break; // Just in case
         default :
            assume(0, "Should never get here");
            break;
         }
      }
   if (vmThread)
      {
      // Check if this thread has cached the frontend inside
      TR_J9VMBase * vmWithThreadInfo = (TR_J9VMBase *)vmThread->jitVMwithThreadInfo;

#if defined(J9VM_INTERP_AOT_COMPILE_SUPPORT)
      if (vmType==AOT_VM)
         {
         TR_J9VMBase * aotVMWithThreadInfo = (TR_J9VMBase *)vmThread->aotVMwithThreadInfo;

         if (!aotVMWithThreadInfo)
            {
            alloc = j9mem_allocate_memory(sizeof(TR_J9SharedCacheVM), J9MEM_CATEGORY_JIT);
            if (alloc)
               {
               aotVMWithThreadInfo =  new (alloc) TR_J9SharedCacheVM(jitConfig, vmThread);
               }
            if (aotVMWithThreadInfo)
               {
               vmThread->aotVMwithThreadInfo = aotVMWithThreadInfo;
               // storing _compInfo in multiple places could spell trouble when we free the structure
               aotVMWithThreadInfo->_compInfo = vmWithoutThreadInfo->_compInfo;
               if (vmWithoutThreadInfo->_compInfo)
                  {
                  TR_CompilationInfoPerThread *compInfoPT = vmWithoutThreadInfo->_compInfo->getCompInfoForThread(vmThread);
                  aotVMWithThreadInfo->_vmThreadIsCompilationThread = (compInfoPT ? TR_yes : TR_no);
                  aotVMWithThreadInfo->_compInfoPT = compInfoPT;
                  }
               }
            else
               aotVMWithThreadInfo = aotVMWithoutThreadInfo; // may be incorrect because they are of different type
            }
         return aotVMWithThreadInfo;
         }
      else // We need to create a J9_VM
#endif
         {
         assume0(vmType==DEFAULT_VM || vmType==J9_VM);
         if (!vmWithThreadInfo) // This thread has not cached a frontend
            {
            alloc = j9mem_allocate_memory(sizeof(TR_J9VM), J9MEM_CATEGORY_JIT);
            if (alloc)
               {
               vmWithThreadInfo =  new (alloc) TR_J9VM(jitConfig, vmThread); // allocate the frontend
               }
            if (vmWithThreadInfo)
               {
               vmThread->jitVMwithThreadInfo = vmWithThreadInfo; // cache it
               // storing _compInfo in multiple places could spell trouble when we free the structure
               vmWithThreadInfo->_compInfo = vmWithoutThreadInfo->_compInfo;
               // Cache ths compilation thread as well
               if (vmWithoutThreadInfo->_compInfo)
                  {
                  TR_CompilationInfoPerThread *compInfoPT = vmWithoutThreadInfo->_compInfo->getCompInfoForThread(vmThread);
                  vmWithThreadInfo->_vmThreadIsCompilationThread = (compInfoPT ? TR_yes : TR_no);
                  vmWithThreadInfo->_compInfoPT = compInfoPT;
                  }
               }
            else
               vmWithThreadInfo = vmWithoutThreadInfo;
            }
         return vmWithThreadInfo;
         }
      }
   return vmWithoutThreadInfo;
   }

void
TR_J9VMBase::outOfMemory(TR_Compilation * comp, const char * reason)
   {
   j9OutOfMemory(_jitConfig, comp, reason);
   signalOutOfMemory(_jitConfig);
   }

bool signalOutOfMemory(J9JITConfig * jitConfig)
   {
   // if execution reaches here then there is no jump_buf
   // ie. the JIT is trying to allocateMemory out of a compile (e.g. a hook)
   // setup a notification of outOfMemory on the current thread & let the vm handle it
   //
#if 0
   // disabled as there is new code now to handle OOMs
   J9JITConfig *config = jitConfig;
   if (!config)
      config = ::jitConfig;

   if (config && !config->outOfMemoryJumpBuffer)
      {
      J9VMThread *vmThread = config->javaVM->internalVMFunctions->currentVMThread(config->javaVM);
      vmThread->javaVM->internalVMFunctions->setNativeOutOfMemoryError(vmThread, J9NLS_JIT_OUT_OF_MEMORY);
      return true;
      }
   //else
   //   fprintf(stderr, "no jitConfig to deal with out of memory!\n");
#endif
   return false;
   }

void j9OutOfMemory(J9JITConfig * jitConfig, TR_Compilation * comp, const char * reason, jmp_buf *jumpBuffer)
   {
   if (comp)
      {
      if (comp->getOptions()->getAnyOption(TR_TraceAll))
         {
         trace1c(comp, "\n=== OUT OF MEMORY (%s) ===\n", reason);
         }
#ifdef TR_MEM_STATS
      if (comp->getOption(TR_TraceCompMemUsage) || comp->getOption(TR_TraceCompMemUsageObj))
         {
         comp->trMemory()->printStats(comp->fe(), FESTDOUT);
         }
#endif
      if (comp->getOption(TR_BreakOnOOM))
         {
         fprintf(stderr, "\n=== OUT OF MEMORY (%s) compiling %s ===\n", reason, comp->signature());
         feDebugBreak();
         }
      }
   else if (TR_Options::getCmdLineOptions()->getOption(TR_BreakOnOOM))
      {
      fprintf(stderr, "\n=== OUT OF MEMORY (%s) ===\n", reason);
      feDebugBreak();
      }
   if (comp && (debug("traceOutOfMemory") || debug("stopOnOutOfMemory") || comp->getOptions()->getOption(TR_StopOnFailure)))
      {
      if (reason)
         {
         diagnostic2c(comp, "Out of %s memory while compiling %s\n", reason, comp->signature());
         fprintf(stderr, "Out of %s memory while compiling %s\n", reason, comp->signature());
         }
      else
         {
         diagnostic1c(comp, "Out of memory while compiling %s\n", comp->signature());
         fprintf(stderr, "Out of memory while compiling %s\n", comp->signature());
         }
      fflush(stderr);
      }
   if (!jumpBuffer)
      {
      // Try to get the jumpBuffer from the compilation object
      if (comp)
         {
         jumpBuffer = ((TR_CompilationInfoPerThreadBase*)(comp->trMemory()->getCompilationInfoPerThread()))->getJumpBuffer();
         // fprintf(stderr, "Will use jumpBuffer from comp\n");
         }
      else
         {
         // fprintf(stderr, "Will find compThread and use jumpBuffer\n");
         // find current thread, find the corresponding compInfoPT object and then the jumpBuffer
         if (!jitConfig)
            jitConfig = ::jitConfig;
         TR_CompilationInfo *compInfo = TR_CompilationInfo::get(jitConfig);
         J9VMThread *vmThread = jitConfig->javaVM->internalVMFunctions->currentVMThread(jitConfig->javaVM);

         TR_CompilationInfoPerThreadBase *cp = compInfo->getCompInfoForCompOnAppThread();
         if (cp)
            {
            jumpBuffer = cp->getJumpBuffer();
            }
         else if (compInfo->useSeparateCompilationThread())
            {
            TR_CompilationInfoPerThread *compInfoPT = compInfo->getCompInfoForThread(vmThread);
            if (compInfoPT)
               jumpBuffer = compInfoPT->getJumpBuffer();
            }
         }
      assume(jumpBuffer, "must have a jump buffer");
      // Last resort, look in jitConfig
      if (!jumpBuffer)
         jumpBuffer = (jmp_buf*)jitConfig->outOfMemoryJumpBuffer;
      }

   if (jumpBuffer)
      longjmp(*jumpBuffer, -1);

   assume(0, "out of memory and dont have the code to deal with it");
   fprintf(stderr, "Out of native memory and no error handler is set\n"); fflush(stderr);
   *((char*)0) = 1;
   }

TR_J9VMBase::TR_J9VMBase(J9JITConfig * jitConfig,
                         J9VMThread * vmThread,
                         ILOps * opCodesForStore,
                         ILOps * opCodesForLoad,
                         ILOps * opCodesForIndirectStore,
                         ILOps * opCodesForIndirectLoad,
                         ILOps * opCodesForIndirectArrayStore,
                         ILOps * opCodesForIndirectArrayLoad,
                         ILOps * opCodesForConst,
                         ILOps * opCodesForRegisterStore,
                         ILOps * opCodesForRegisterLoad,
                         ILOps * opCodesForCompareEquals,
                         ILOps * opCodesForCompareNotEquals)
   : TR_FrontEnd(opCodesForStore, opCodesForLoad,
           opCodesForIndirectStore, opCodesForIndirectLoad,
           opCodesForIndirectArrayStore, opCodesForIndirectArrayLoad,
           opCodesForConst,
           opCodesForRegisterStore, opCodesForRegisterLoad,
           opCodesForCompareEquals, opCodesForCompareNotEquals),
     _vmThread(vmThread),
     _jitConfig(jitConfig),
     _portLibrary(jitConfig->javaVM->portLibrary),
     _vmFunctionTable(jitConfig->javaVM->internalVMFunctions),
     _compInfo(0),
     _imtBuilder(0),
     _iProfiler(0),
     _hwProfiler(0),
     _vmThreadIsCompilationThread(TR_maybe),
     _compInfoPT(NULL),
     _shouldSleep(false)
   {
   for (int32_t i = 0; i < UT_MODULE_INFO.count; ++i)
      if (UT_ACTIVE[i])
         {
         setTraceIsEnabled(true);
         break;
         }

   if ((jitConfig->javaVM->runtimeFlags & J9_RUNTIME_SELECTIVE_DEBUG_ENABLED) != 0)
      setSelectiveDebug();

   _sharedCache = NULL;
   if (TR_Options::sharedClassCache())        // shared classes and AOT must be enabled
      {
      _sharedCache = new (PERSISTENT_NEW) TR_J9SharedCache(this);
      if (!_sharedCache)
         {
         TR_Options::getAOTCmdLineOptions()->setOption(TR_NoStoreAOT);
         TR_Options::getAOTCmdLineOptions()->setOption(TR_NoLoadAOT);
         ((TR_JitPrivateConfig *)jitConfig->privateConfig)->aotValidHeader = TR_no;
         }
      else
         {
#if !defined(PUBLIC_BUILD)
         TR_PersistentMemory * persistentMemory = (TR_PersistentMemory *)(jitConfig->scratchSegment);
         TR_PersistentClassLoaderTable *loaderTable = persistentMemory->getPersistentInfo()->getPersistentClassLoaderTable();
         _sharedCache->setPersistentClassLoaderTable(loaderTable);
#endif
         }
      }
   }

void
TR_J9VMBase::freeSharedCache()
   {
   if (_sharedCache)        // shared classes and AOT must be enabled
      {
      assume(TR_Options::sharedClassCache(), "Found shared cache with option disabled");
      jitPersistentFree(_sharedCache);
      _sharedCache = NULL;
      }
   }

TR_CompilationInfo * getCompilationInfo(J9JITConfig * jitConfig)
   {
   return TR_CompilationInfo::get(jitConfig);
   }

J9VMThread *
TR_J9VMBase::vmThread()
   {
   assume(_vmThread, "TR_J9VMBase::vmThread() TR_J9VMBase was created without thread information");
   return _vmThread;
   }

void
TR_J9VMBase::acquireCompilationLock()
   {
   if (_compInfo)
      _compInfo->acquireCompilationLock();
   }

void
TR_J9VMBase::releaseCompilationLock()
   {
   if (_compInfo)
      _compInfo->releaseCompilationLock();
   }

void
TR_J9VMBase::acquireLogMonitor()
   {
   if (_compInfo)
      _compInfo->acquireLogMonitor();
   }

void
TR_J9VMBase::releaseLogMonitor()
   {
   if (_compInfo)
      _compInfo->releaseLogMonitor();
   }

// To be used at codegen around instruction scheduling
void
TR_J9VMBase::acquireSchedulingMonitor()
   {
   if (TR_Options::getCmdLineOptions()->getOption(TR_DisableSchedulingLocking))
      return;
   if (_compInfo)
      {
      if (!_compInfo->useSeparateCompilationThread())
         return;

      /* Big piece of code that may be used if the critical section becomes too large
      // Typically, a compilation thread holds the classUnloadMonitor, but no VM access
      assume(_vmThreadIsCompilationThread == TR_yes, "vmThread must be a compilation thread");
      bool hadClassUnloadMonitor = false;
      if (!TR_Options::getCmdLineOptions()->getOption(TR_DisableNoVMAccess))
         {
         assume(_compInfoPT, "a fe for a compilation thread must have _compInfoPT set");
#if !defined(J9VM_GC_DYNAMIC_CLASS_UNLOADING)
         if (TR_Options::getCmdLineOptions()->getOption(TR_EnableHCR) || TR_Options::getCmdLineOptions()->getOption(TR_FullSpeedDebug))
#endif
            hadClassUnloadMonitor = TR_J9MonitorTable::get()->readReleaseClassUnloadMonitor(_compInfoPT->getCompThreadId()) >= 0;
         }
      else
         {
         releaseVMAccess(vmThread());
         }
      */
      _compInfo->acquireSchedulingMonitor();
      /*
      if (!TR_Options::getCmdLineOptions()->getOption(TR_DisableNoVMAccess))
         {
         if (hadClassUnloadMonitor)
            TR_J9MonitorTable::get()->readAcquireClassUnloadMonitor(_compInfoPT->getCompThreadId());
         }
      else
         {
         acquireVMAccessNoSuspend(vmThread());
         }

     // may drop compilation if classUnloading or HCR happened
     if (_compInfoPT->compilationShouldBeInterrupted())
        {
        // Set the error code
        _compInfoPT->getCompilation()->setErrorCode(COMPILATION_INTERRUPTED);
        j9OutOfMemory(jitConfig, _compInfoPT->getCompilation(), "Compilation interrupted by GC unloading classes", _compInfoPT->getJumpBuffer());
        }
      */
      }
   }

void
TR_J9VMBase::releaseSchedulingMonitor()
   {
   if (TR_Options::getCmdLineOptions()->getOption(TR_DisableSchedulingLocking))
      return;
   if (_compInfo && _compInfo->useSeparateCompilationThread())
      _compInfo->releaseSchedulingMonitor();
   }

bool
TR_J9VMBase::isSeparateThreadCompilation()
   {
   return _compInfo ? _compInfo->useSeparateCompilationThread() != 0 : false;
   }

bool
TR_J9VMBase::isAsyncCompilation()
   {
   return _compInfo ? _compInfo->asynchronousCompilation() : false;
   }


uintptrj_t
TR_J9VMBase::getProcessID()
   {
   PORT_ACCESS_FROM_ENV(_jitConfig->javaVM);
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   uintptrj_t result = j9sysinfo_get_pid();
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

// static method
char *
TR_J9VMBase::getJ9FormattedName(J9JITConfig *jitConfig, J9PortLibrary *portLibrary, char *buf, int32_t bufLength, char *name, char *format, bool suffix)
   {
   PORT_ACCESS_FROM_ENV(jitConfig->javaVM);
   J9StringTokens *tokens;
   I_64 curTime;
   J9VMThread *vmThread = jitConfig->javaVM->internalVMFunctions->currentVMThread(jitConfig->javaVM);
   //bool haveAcquiredVMAccess = acquireVMaccessIfNeeded(vmThread, TR_maybe);
   // No need to acquire VM access for j9str_create_tokens, j9str_subst_tokens or j9str_free_tokens
   curTime = j9time_current_time_millis();
   tokens = j9str_create_tokens(curTime);
   if (tokens == NULL)
      {
      //releaseVMaccessIfNeeded(vmThread, haveAcquiredVMAccess);
      return 0;
      }

   char tmp[1025];
   I_32 nameLength = (I_32)strlen(name);

   j9str_subst_tokens(tmp,1025,name,tokens);

   if(!(strcmp(tmp,name)==0))        //only append if there isn't a format specifier
      {
      strncpy(buf,tmp,strlen(tmp)+1);   //+1 to get the null terminator
      }
   else
      {
      strncpy(buf, name, nameLength);
      char *suffixBuf = &(buf[nameLength]);
      if (format)
         j9str_subst_tokens(suffixBuf, (bufLength - nameLength), format, tokens);
      else if(suffix)
         {
         // We have to break the string up to prevent CMVC keyword expansion
         j9str_subst_tokens(suffixBuf, (bufLength - nameLength), ".%Y" "%m" "%d." "%H" "%M" "%S.%pid", tokens);
         }
      else
         buf=name;
      }

   j9str_free_tokens(tokens);
   //releaseVMaccessIfNeeded(vmThread, haveAcquiredVMAccess);
   return buf;
   }


char *
TR_J9VMBase::getFormattedName(char *buf, int32_t bufLength, char *name, char *format, bool suffix)
   {
   return getJ9FormattedName(_jitConfig, _portLibrary, buf, bufLength, name, format, suffix);
   }


void
TR_J9VMBase::invalidateCompilationRequestsForUnloadedMethods(TR_OpaqueClassBlock *clazz, bool hotCodeReplacement)
   {
   // Only called from jitHookClassUnload so we don't need to acquire VM access
   if (_compInfo)
      _compInfo->invalidateRequestsForUnloadedMethods(clazz, vmThread(), hotCodeReplacement);
   }

void
TR_J9VMBase::initializeSystemProperties()
   {
   #if defined(TR_TARGET_POWER) || defined(TR_TARGET_S390) || defined(TR_TARGET_X86)
   initializeProcessorType();
   initializeHasFPU();
   initializeHasResumableTrapHandler();
   initializeHasFixedFrameC_CallingConvention();
   //initializeNumberBytesInaccessible();
   //initializeSupportsScaledIndexAddressing();
   #elif defined(TR_TARGET_ARM)
   #if defined(__VFP_FP__) && !defined(__SOFTFP__)
   initializeHasFPU();
   #endif
   initializeProcessorType();
   #endif
   }

bool
TR_J9VMBase::hasResumableTrapHandler()
   {
   return _compInfo ? _compInfo->getHasResumableTrapHandler() : false;
   }

bool
TR_J9VMBase::hasFPU()
   {
   return _compInfo ? _compInfo->getHasFPU() : false;
   }

bool
TR_J9VMBase::hasFixedFrameC_CallingConvention()
   {
   return _compInfo ? _compInfo->getHasFixedFrameC_CallingConvention() : false;
   }

bool
TR_J9VMBase::pushesOutgoingArgsAtCallSite(TR_Compilation *comp)
   {
   // TODO: What about cross-compilation?  Is there any way to know whether the
   // target should be using a variable frame even if the host is not?
#if defined(J9SW_NEEDS_JIT_2_INTERP_CALLEE_ARG_POP)
   return true;
#else
   return false;
#endif
   }

uint32_t
TR_J9VMBase::getNumberBytesReadInaccessible()
   {
   return _compInfo ? _compInfo->getNumberBytesReadInaccessible() : 0;
   }

uint32_t
TR_J9VMBase::getNumberBytesWriteInaccessible()
   {
   return _compInfo ? _compInfo->getNumberBytesWriteInaccessible() : 0;
   }

bool
TR_J9VMBase::supportsScaledIndexAddressing()
   {
   return _compInfo ? _compInfo->getSupportsScaledIndexAddressing() : false;
   }

bool
TR_J9VMBase::assumeLeftMostNibbleIsZero()
   {
   return false;
   }
#if defined(TR_TARGET_S390)
////////////////////////////////////////////////////////////////////////////////
// The following are the 2 versions of the /proc/cpuinfo file for Linux/390.
//
// When Gallileo (TREX) comes out, we will need to update this routine to support
// TREX and not end up generating default 9672 code!
// vendor_id       : IBM/S390
// # processors    : 2
// bogomips per cpu: 838.86
// processor 0: version = FF,  identification = 100003,  machine = 2064
// processor 1: version = FF,  identification = 200003,  machine = 2064
//
// vs.
//
// vendor_id       : IBM/S390
// # processors    : 3
// bogomips per cpu: 493.15
// processor 0: version = FF,  identification = 100003,  machine = 9672
// processor 1: version = FF,  identification = 200003,  machine = 9672
// processor 2: version = FF,  identification = 300003,  machine = 9672
////////////////////////////////////////////////////////////////////////////////

/* Definitions in codegen/Runtime.hpp
#define G5                              (9672)  // Not Supported as of Java 1.5
#define MULTIPRISE7000                  (7060)  // Not Supported as of Java 1.5
#define FREEWAY                         (2064)
#define Z800                            (2066)
#define MIRAGE                          (1090)
#define MIRAGE2                         (1091)
#define TREX                            (2084)
#define Z890                            (2086)
#define GOLDEN_EAGLE                    (2094)
#define DANU_GA2                        (2094)  // type doesn't change from GOLDEN_EAGLE
#define Z9BC                            (2096)
#define Z10                             (2097)
#define Z10BC                           (2098)  // zMR
#define ZG                              (2817)
#define ZGMR                            (2818)
#define ZG_RESERVE                      (2819)  // reserve for zGryphon
#define ZGP                             (2827)
#define ZGPMR                           (2828)
#define ZGP_RESERVE                     (2829)  // reserve for zHelix
#define ZNEXT                           (2964)
#define ZNEXTBC                         (2965)
#define ZH                              (2458)  // reserve for zHybrid
#define DATAPOWER                       (2462)  // reserve for DataPower on zHybrid
#define ZH_RESERVE1                     (2459)  // reserve for zHybrid
#define ZH_RESERVE2                     (2461)  // reserve for zHybrid
*/

static const int S390MachineTypes[] =
   {
   TR_FREEWAY, TR_Z800, TR_MIRAGE, TR_MIRAGE2, TR_TREX, TR_Z890, TR_GOLDEN_EAGLE, TR_DANU_GA2, TR_Z9BC,
   TR_Z10, TR_Z10BC, TR_ZG, TR_ZGMR, TR_ZEC12, TR_ZEC12MR, TR_ZG_RESERVE, TR_ZEC12_RESERVE,
   TR_ZNEXT, TR_ZNEXTBC,
   TR_ZH, TR_DATAPOWER, TR_ZH_RESERVE1, TR_ZH_RESERVE2
   };

static const int S390UnsupportedMachineTypes[] =
   {
   TR_G5, TR_MULTIPRISE7000
   };

////////////////////////////////////////////////////////////////////////////////
// Common facility checks between zLinux and zOS.
////////////////////////////////////////////////////////////////////////////////
static bool
portLibCall_supportsDFP()
   {
   /* NOTE:  STFLE is a zArchitecture instruction, hence not supported under zLinux-31.
             Need to return false since neither zArch nor DFP will work under zLinux-31
    */

   /* Per AR-10113-POK: STFLE bits:
      41 - The floating-point-support enhancement facility is installed in the z/Architecture architectural mode.
      42 - The DFP (decimal-floating-point) facility is installed in the z/Architecture architectural mode.
   */
#if defined(TR_HOST_S390) && (defined(J9ZOS390) || defined(LINUX))
   int numDoubleWords = 0; //will load 0 + 1 = 1 double words
   uint64_t mem = 0 ; //64 bits of storage
   _getSTFLEBits(numDoubleWords, &mem);
   return ((mem & (((uint64_t)1) << (63-41))) !=0 && //shift 1 left 23 bits
           (mem & (((uint64_t)1) << (63-42))) !=0);  //shift 1 left 22 bits
#endif
   return false;
   }

static bool
portLibCall_get390_supportsCompareAndSwapAndStore()
   {
#if defined(TR_HOST_S390) && (defined(J9ZOS390) || defined(LINUX))
   int numDoubleWords = 0; //will load 0 + 1 = 1 double words
   uint64_t mem = 0 ; //64 bits of storage
   _getSTFLEBits(numDoubleWords, &mem);
   return (mem & (((uint64_t)1) << (63-32))) !=0; // CompareAndSwapAndStoreFacility
#endif
   return false;
   }

static bool
portLibCall_get390_supportsCompareAndSwapAndStore2()
   {
#if defined(TR_HOST_S390) && (defined(J9ZOS390) ||defined(LINUX))
   int numDoubleWords = 0; //will load 0 + 1 = 1 double words
   uint64_t mem = 0 ; //64 bits of storage
   _getSTFLEBits(numDoubleWords, &mem);
   return (mem & (((uint64_t)1) << (63-33))) !=0; // CompareAndSwapAndStoreFacility2
#endif
   return false;
   }

static bool
portLibCall_get390_supportsGeneralInstructionsExtensions()
   {
#if defined(TR_HOST_S390) && (defined(J9ZOS390) || defined(LINUX))
   int numDoubleWords = 0; //will load 0 + 1 = 1 double words
   uint64_t mem = 0 ; //64 bits of storage
   _getSTFLEBits(numDoubleWords, &mem);
   return (mem & (((uint64_t)1) << (63-34))) !=0; // GeneralInstructionsExtensionsFacility
#endif
   return false;
   }

static bool
portLibCall_get390_supportsExecuteExtensions()
   {
#if defined(TR_HOST_S390) && (defined(J9ZOS390) || defined(LINUX))
   int numDoubleWords = 0; //will load 0 + 1 = 1  double words
   uint64_t mem = 0 ; //64 bits of storage
   _getSTFLEBits(numDoubleWords, &mem);
   return (mem & (((uint64_t)1) << (63-35))) !=0;  //ExecuteInstructionsFacility
#endif
   return false;
   }

static bool
portLibCall_get390_supportsFPE()
   {
   // As per AR-10113-POK, STFLE bit 41 indicates the floating-point-support enhancement facility is installed in the z/Architecture architectural mode.
#if defined(TR_HOST_S390) && defined(J9ZOS390)
   int numDoubleWords = 0; //will load 0 + 1 = 1  double words
   uint64_t mem = 0 ; //64 bits of storage
   _getSTFLEBits(numDoubleWords, &mem);
   return ((mem & (((uint64_t)1) << (63-41))) !=0);
#endif
   return false;
   }

static bool
portLibCall_get390_supportsZ6()
   {
#if defined(TR_HOST_S390) && (defined(J9ZOS390) || defined(LINUX))
   return portLibCall_get390_supportsGeneralInstructionsExtensions();
#endif
   return false;
   }

static bool
portLibCall_get390_supportsZGryphon()
   {
   // Location 200 is architected such that bit 45 is ON if zG
   // instruction is installed
#if defined(TR_HOST_S390) && (defined(J9ZOS390) || defined(LINUX))
   int numDoubleWords = 0; //will load 0 + 1 = 1  double words
   uint64_t mem = 0 ; //64 bits of storage
   _getSTFLEBits(numDoubleWords, &mem);
   return ((mem & (((uint64_t)1) << (63-45))) !=0);
#endif
   return false;
   }

static bool
portLibCall_get390_supportsRunTimeInstrumentation()
   {
#if defined(TR_HOST_S390) && (defined(J9ZOS390) || defined(LINUX))
   // AR-10465
   // 64 - The run-time-instrumentation (RI) facility is installed in the z/Architecture architectural mode.
   int numDoubleWords = 1; //will load 1 + 1 = 2  double words

   struct STFLEFacilities { // 128-bit storage
      uint64_t dw1;
      uint64_t dw2;
   } mem;

   _getSTFLEBits(numDoubleWords, (uint64_t*)&mem);
   return ((mem.dw2 & (((uint64_t)1) << (63-0))) !=0);
#endif
   return false;
   }

static bool
portLibCall_get390_supportsTransactionalExecution()
   {
#if defined(TR_HOST_S390) && (defined(J9ZOS390) || defined(LINUX))
   // AR-10461
   // 50 The constrained transactional-execution facility is installed in the z/Architecture architectural mode.  This bit is meaningful only when bit 73 is one.
   // 73 The transactional-execution facility is installed in the z/Architecture architectural mode.

   int numDoubleWords = 1; //will load 1 + 1 = 2  double words

   struct STFLEFacilities { // 128-bit storage
      uint64_t dw1;
      uint64_t dw2;
   } mem;

   _getSTFLEBits(numDoubleWords, (uint64_t*)&mem);
   return ((mem.dw1 & (((uint64_t)1) << (63-50))) !=0) && ((mem.dw2 & (((uint64_t)1) << (63-9))) !=0);
#endif
   return false;

   }
static bool
portLibCall_get390_supportsMiscellaneousInstructionExtension()
   {
   // AR-10440
   // 49 The execution-hint, load-and-trap, and miscellaneous-instruction-extensions facilities, are installed in the z/Architecture architectural mode.
#if defined(TR_HOST_S390) && (defined(J9ZOS390) || defined(LINUX))
   int numDoubleWords = 0; //will load 0 + 1 = 1  double words
   uint64_t mem = 0 ; //64 bits of storage
   _getSTFLEBits(numDoubleWords, &mem);
   return ((mem & (((uint64_t)1) << (63-49))) !=0);
#endif
   return false;
   }

static bool
portLibCall_get390_supportsZHelix()
   {
#if defined(TR_HOST_S390) && (defined(J9ZOS390) || defined(LINUX))
   return portLibCall_get390_supportsMiscellaneousInstructionExtension() &&
          portLibCall_get390_supportsTransactionalExecution() &&
          portLibCall_get390_supportsRunTimeInstrumentation();
#endif
   return false;
   }


////////////////////////////////////////////////////////////////////////////////
// zLinux specific checks
////////////////////////////////////////////////////////////////////////////////
static bool
portLibCall_get390zLinux_supportsStoreExtendedFacilityList()
   {
   char line[80];
   const int LINE_SIZE = sizeof(line) - 1;
   const char typeHeader[] = "Type: ";
   const int TYPE_LINE_SIZE = 27;
   const int TYPE_HEADER_SIZE = sizeof(typeHeader) - 1;
   const char zVMHeader[] = "VM00 Control Program: z/VM    ";
   const int zVM_LINE_SIZE = 39;
   const int zVM_HEADER_SIZE = sizeof(zVMHeader) - 1;
   const char procHeader[] = "features";
   const int PROC_HEADER_SIZE = sizeof(procHeader) - 1;
   bool supportedType = false;
   bool supportedzVM = true;

   FILE * fp = fopen("/proc/cpuinfo", "r");
   if (fp)
      {
      while (fgets(line, LINE_SIZE, fp) > 0)
         {
         if (!memcmp(line, procHeader, PROC_HEADER_SIZE))
            {
            if (strstr(line, "stfle"))
               {
               fclose(fp);
               return true;
               }
            }
         }
      fclose(fp);
      }

   fp = fopen("/proc/sysinfo", "r");
   if (fp)
      {
      while (fgets(line, LINE_SIZE, fp) > 0)
         {
         int len = strlen(line);
         if (len > TYPE_HEADER_SIZE &&
             !memcmp(line, typeHeader, TYPE_HEADER_SIZE) &&
             !supportedType)
            {
            if (len == TYPE_LINE_SIZE)
               {
               int type;
               sscanf(line, "%*s %d",&type);

               if (type == TR_GOLDEN_EAGLE || type == TR_DANU_GA2 || type == TR_Z9BC ||
                   type == TR_Z10 || type == TR_Z10BC || type == TR_ZG || type == TR_ZGMR ||
                   type == TR_ZEC12 || type == TR_ZEC12MR || type == TR_ZNEXT || type == TR_ZNEXTBC)
                  {
                  supportedType = true;
                  //printf("supportedType %d\n", supportedType);
                  }
               }
            }
         else if (len > zVM_HEADER_SIZE &&
                  !memcmp(line, zVMHeader, zVM_HEADER_SIZE))
            {
            if (len == zVM_LINE_SIZE)
               {
               int ver=0;
               int major=0;
               sscanf(line, "%*s %*s %*s %*s %d%*c%d%*c%*d%*c", &ver, &major);

               if (ver < 5 || (ver == 5 && major < 2))
                  {
                  supportedzVM = false;
                  //nprintf("supportedzVM %d\n", supportedzVM);
                  }
               }
            }
         }
      fclose(fp);
      }
   return (supportedType && supportedzVM);
   }

static TR_S390MachineType
portLib_get390zLinuxMachineType()
   {
   char line[80];
   const int LINE_SIZE = sizeof(line) - 1;
   const char procHeader[] = "processor ";
   const int PROC_LINE_SIZE = 69;
   const int PROC_HEADER_SIZE = sizeof(procHeader) - 1;
   TR_S390MachineType ret_machine = TR_DEFAULT_S390_MACHINE;  /* return value */

   FILE * fp = fopen("/proc/cpuinfo", "r");
   if (fp)
      {
      while (fgets(line, LINE_SIZE, fp) > 0)
         {
         int len = strlen(line);
         if (len > PROC_HEADER_SIZE && !memcmp(line, procHeader, PROC_HEADER_SIZE))
            {
            if (len == PROC_LINE_SIZE)
               {
               int i;
               int machine;
               sscanf(line, "%*s %*d%*c %*s %*c %*s %*s %*c %*s %*s %*c %d", &machine);

               // Scan list of unsupported machines - We do not initialize the JIT for such hardware.
               for (i = 0; i < sizeof(S390UnsupportedMachineTypes) / sizeof(int); ++i)
                  {
                  if (machine == S390UnsupportedMachineTypes[i])
                     {
                     PORT_ACCESS_FROM_ENV(jitConfig->javaVM);
                     j9nls_printf(jitConfig->javaVM->portLibrary, J9NLS_ERROR, J9NLS_J9JIT_390_UNSUPPORTED_HARDWARE, machine);
                     assume1(0,"Hardware is not supported.");
                     j9OutOfMemory(jitConfig, 0, "Hardware is not supported.");
                     }
                  }

               // Scan list of supported machines.
               for (i = 0; i < sizeof(S390MachineTypes) / sizeof(int); ++i)
                  {
                  if (machine == S390MachineTypes[i])
                     {
                     ret_machine = (TR_S390MachineType)machine;
                     }
                  }
               }
            }
         }
      fclose(fp);
      }

   return ret_machine;
  }

/* check for RAS highword register support on zLinux */
/* on 64-bit the highwords will always be available */
static bool
portLibCall_get390zLinux_supportsHPRDebug()
   {
#if defined(TR_TARGET_32BIT)
   char line[80];
   const int LINE_SIZE = sizeof(line) - 1;
   const char procHeader[] = "features";
   const int PROC_HEADER_SIZE = sizeof(procHeader) - 1;

   FILE * fp = fopen("/proc/cpuinfo", "r");
   if (fp)
      {
      while (fgets(line, LINE_SIZE, fp) > 0)
         {
         if (!memcmp(line, procHeader, PROC_HEADER_SIZE))
            {
            if (strstr(line, "highgprs"))
               {
               fclose(fp);
               return true;
               }
            }
         }
      fclose(fp);
      }
   return false;
#endif
   // 64-bit always support highword registers in RAS
   return true;
   }

/* check zLinux kernel support for Transactional Memory
   There will be a 'te' flag on the features line       */
static bool
portLib_get390zLinux_SupportTM()
   {
   char line[80];
   const int LINE_SIZE = sizeof(line) - 1;
   const char procHeader[] = "features";
   const int PROC_HEADER_SIZE = sizeof(procHeader) - 1;

   FILE * fp = fopen("/proc/cpuinfo", "r");
   if (fp)
      {
      while (fgets(line, LINE_SIZE, fp) > 0)
         {
         if (!memcmp(line, procHeader, PROC_HEADER_SIZE))
            {
            if (strstr(line, " te "))
               {
               fclose(fp);
               return true;
               }
            }
         }
      fclose(fp);
      }
   return false;
   }

/**
 zOS hardware detection checks a word at byte 200 of PSA.

 The following is a map of the bits in this word (zOS V1.6):
     200     (C8)    BITSTRING        1    FLCFACL0     Byte 0 of FLCFACL
                      1...  ....           FLCFN3       "X'80'" - N3
                                                        installed
                      .1..  ....           FLCFZARI     "X'40'" -
                                                        z/Architecture
                                                        installed
                      ..1.  ....           FLCFZARA     "X'20'" -
                                                        z/Architecture
                                                        activce
                      ....  ..1.           FLCFASLX     "X'02'" - ASN & LX
                                                        reuse facility
                                                        installed
     201     (C9)    BITSTRING        1    FLCFACL1     Byte 1 of FLCFACL
     202     (CA)    BITSTRING        1    FLCFACL2     Byte 2 of FLCFACL
                      1...  ....           FLCFETF2     "X'80'" Extended
                                                        Translation facility
                                                        2
                      .1..  ....           FLCFCRYA     "X'40'" Cryptographic
                                                        assist
                      ..1.  ....           FLCFLD       "X'20'" Long
                                                        Displacement facility
                      ...1  ....           FLCFLDHP     "X'10'" Long
                                                        Displacement High
                                                        Performance
                      ....  1...           FLCFHMAS     "X'08'" HFP Multiply
                                                        Add/Subtract
                      ....  .1..           FLCFEIMM     "X'04'" Extended
                                                        immediate when z/Arch
                      ....  .1..           FLCFETF3     "X'04'" Extended
                                                        Translation Facility
                                                        3 when z/Arch
                      ....  ...1           FLCFHUN      "X'01'" HFP
                                                        unnormalized
                                                        extension
     203     (CB)    BITSTRING        1    FLCFACL3     Byte 3 of FLCFACL
 */

static bool
portLibCall_get390zOS_N3Support()
   {
   //  Location 200 is architected such that bit 0 is ON if N3 instructions
   //  are available
   return (((*(int*) 200) & 0x80000000) != 0);
   }

static bool
portLibCall_get390zOS_ZArchSupport()
   {
   // Location 200 is architected such that
   //    bit 1 is ON if z/Architecture is installed
   //    bit 2 is ON if z/Architecture is active
   // We check both bits to ensure that z/Architecture is available.
   //
   // Alternative solution - Test if FLCARCH is not 0.
   //    2 FLCARCH     CHAR(1),               /* Architecture info    @LSA*/
   //      4 *        BIT(7),                 /*                      @LSA*/
   //      4 PSAZARCH BIT(1),                 /* z/Architecture       @LSA*/
   //        5 PSAESAME BIT(1),               /* z/Architecture       @LSA*/
   // FLCARCH is at offset x'A3'
   //
   return (((*(int*) 200) & 0x60000000) == 0x60000000);
   }

#if defined(J9ZOS390)
#include <sys/utsname.h>
#endif

static TR_S390MachineType
portLib_get390zOSMachineType()
   {
   TR_S390MachineType ret_machine = TR_DEFAULT_S390_MACHINE;
#if defined(J9ZOS390)
   struct utsname info;
   if (::uname(&info))
      return ret_machine;

   uint32_t type = atoi(info.machine);
   // Scan list of supported machines.
   for (int32_t i = 0; i < sizeof(S390MachineTypes) / sizeof(int); ++i)
      {
      if (type == S390MachineTypes[i])
         {
         ret_machine = (TR_S390MachineType)type;
         break;
         }
      }
#endif
   return ret_machine;
   }


static bool
portLibCall_get390zOS_TrexSupport()
   {
   // Location 200 is architected such that bit 0x00001000 is ON if Trex
   // instructons are available
   // The 0x'10' bit at byte 202 is the long displacement high performance bit
   // supported on TRex or higher hardware.
   return (((*(int*) 200) & 0x00001000) != 0);
   }

static bool
portLibCall_get390zOS_GoldenEagleSupport()
   {
   // Location 200 is architected such that bit 21 is ON if Golden Eagle
   // instructons are available
   //  Location 202  - 0x4 (bit 21) - Extended Immediate
   //  Location 202  - 0x2 (bit 22) - Translation Facility 3
   return (((*(int*) 200) & 0x00000400) != 0) &&
          (((*(int*) 200) & 0x00000200) != 0);
   }


static bool
portLibCall_get390zOS_supportsStoreExtendedFacilityList()
   {
   // Location 200 is architected such that bit 7 is ON if STFLE
   // instruction is installed
   //printf ("zOS supports STFLE: %d\n", (((*(int*) 200) & (0x80000000 >> 7)) != 0));
   return (((*(int*) 200) & (0x80000000 >> 7)) != 0);
   }

bool
TR_J9VMBase::getS390SupportsZGryphon()
   {
   return _compInfo->getS390SupportsZGryphon();
   }

bool
TR_J9VMBase::getS390SupportsZHelix()
   {
   return _compInfo->getS390SupportsZHelix();
   }

bool
TR_J9VMBase::getS390TuneForZGryphon()
   {
   return _compInfo->getS390TuneForZGryphon();
   }

bool
TR_J9VMBase::getS390TuneForZHelix()
   {
   return _compInfo->getS390TuneForZHelix();
   }

bool
TR_J9VMBase::getS390SupportsHPRDebug()
   {
   return _compInfo->getS390SupportsHPRDebug();
   }

bool
TR_J9VMBase::getS390SupportsZ6()
   {
   return _compInfo->getS390SupportsZ6();
   }

bool
TR_J9VMBase::getS390SupportsZArchitecture()
   {
   return _compInfo->getS390SupportsZArchitecture();
   }

bool
TR_J9VMBase::getS390SupportsTrex()
   {
   return _compInfo->getS390SupportsTrex();
   }

bool
TR_J9VMBase::getS390SupportsGoldenEagle()
   {
   return _compInfo->getS390SupportsGoldenEagle();
   }

TR_S390MachineType
TR_J9VMBase::getS390MachineType()
   {
   TR_S390MachineType type = _compInfo->getS390MachineType();

   if (!type)
      {
      TR_Target target = (TR_Target)jitConfig->targetName;
      type = isZLinux(target) ?
         portLib_get390zLinuxMachineType() :
         portLib_get390zOSMachineType();
      _compInfo->setS390MachineType(type);
      }

   return type;
   }

void TR_J9VMBase::getS390MachineName(TR_S390MachineType machineType, char* processorName)
   {
   switch(machineType)
      {
      case TR_FREEWAY:
         sprintf(processorName, "z900 (%d)", machineType);
      break;

      case TR_Z800:
         sprintf(processorName, "z800 (%d)", machineType);
      break;

      // zPDT
      case TR_MIRAGE:
      case TR_MIRAGE2:
         sprintf(processorName, "(%d)", machineType);
      break;

      case TR_TREX:
         sprintf(processorName, "z990 (%d)", machineType);
      break;

      case TR_Z890:
         sprintf(processorName, "z890 (%d)", machineType);
      break;

      case TR_GOLDEN_EAGLE:
      //case TR_DANU_GA2:   // TR_DANU_GA2 has same model code as TR_GOLDEN_EAGLE
         sprintf(processorName, "z9 (%d)", machineType);
      break;

      case TR_Z9BC:
         sprintf(processorName, "z9BC (%d)", machineType);
      break;

      case TR_Z10:
         sprintf(processorName, "z10 (%d)", machineType);
      break;

      case TR_Z10BC:
         sprintf(processorName, "z10BC (%d)", machineType);
      break;

      case TR_ZG:
         sprintf(processorName, "z196 (%d)", machineType);
      break;

      case TR_ZGMR:
         sprintf(processorName, "z114 (%d)", machineType);
      break;

      case TR_ZEC12:
         sprintf(processorName, "zEC12 (%d)", machineType);
      break;

      case TR_ZEC12MR:
         sprintf(processorName, "zBC12 (%d)", machineType);
      break;

      case TR_ZNEXT:
         sprintf(processorName, "zNext (%d)", machineType);
      break;

      case TR_ZNEXTBC:
         sprintf(processorName, "zNext BC (%d)", machineType);
      break;

      // Miscellaneous machine codes we were given.  Simply print out the model number.
      case TR_ZG_RESERVE:
      case TR_ZEC12_RESERVE:
      case TR_ZH:
      case TR_DATAPOWER:
      case TR_ZH_RESERVE1:
      case TR_ZH_RESERVE2:
      default:
         sprintf(processorName, "(%d)", machineType);
      break;
      }
   }

#endif // TR_TARGET_S390

static TR_Processor
portLib_getARMLinuxProcessor()
   {
   FILE * fp ;
   char buffer[120];
   char *line_p;
   char *cpu_name = NULL;
   char *position_l, *position_r;
   size_t n=120;
   int i=0;

   fp = fopen("/proc/cpuinfo","r");

   if ( fp == NULL )
      return TR_DefaultARMProcessor;

   line_p = buffer;

   while (!feof(fp))
      {
      fgets(line_p, n, fp);
      position_l = strstr(line_p, "Processor");
      if (position_l)
         {
         position_l = strchr(position_l, ':');
         if (position_l==NULL) return TR_DefaultARMProcessor;
         position_l++;
         while (*(position_l) == ' ') position_l++;

         position_r = strchr(line_p, '\n');
         if (position_r==NULL) return TR_DefaultARMProcessor;
         while (*(position_r-1) == ' ') position_r--;

         if (position_l>=position_r) return TR_DefaultARMProcessor;

         /* localize the cpu name */
         cpu_name = position_l;
         *position_r = '\000';

         break;
         }
      }
   if (cpu_name==NULL) return TR_DefaultARMProcessor;

   fclose(fp);

   if (strstr(cpu_name, "ARMv7"))
      return TR_ARMv7;
   else if (strstr(cpu_name, "ARMv6"))
      return TR_ARMv6;
   else
      return TR_DefaultARMProcessor;
   }

static TR_Processor
portLibCall_getARMProcessorType()
   {
   TR_Processor tp;
#if defined(LINUX)
   tp = portLib_getARMLinuxProcessor();
#else
   tp = TR_DefaultARMProcessor;
#endif
   return tp;
   }

J9PortLibrary*
portLibCall_privatePortLibrary(J9PortLibrary* initializePortLibrary)
   {
   static J9PortLibrary *privatePortLibrary = NULL;

   //Do a one time initialization from J9VMDllMain() in frontend/j9/rossa.cpp
   if(privatePortLibrary == NULL && initializePortLibrary != NULL)
      {
      privatePortLibrary = initializePortLibrary;
      }

   return privatePortLibrary;
   }

J9ProcessorDesc*
portLibCall_getJ9ProcessorDesc()
   {
   static int processorTypeInitialized = 0;
   static J9ProcessorDesc  processorDesc;

   if(processorTypeInitialized == 0)
      {
      PORT_ACCESS_FROM_PORT(portLibCall_privatePortLibrary());
      
      //Just make sure we initialize the J9ProcessorDesc at least once.
      j9sysinfo_get_processor_description(&processorDesc);

      processorTypeInitialized = 1;
      }
   return &processorDesc;
   }

TR_Processor
portLibCall_getPPCProcessorType()
   {
   return portLibCall_getProcessorType();
   }

TR_Processor
portLibCall_getProcessorType()
   {
   J9ProcessorDesc  *processorDesc        = portLibCall_getJ9ProcessorDesc();
   return mapJ9Processor(processorDesc->processor);
   }

TR_Processor
portLibCall_getPhysicalProcessorType()
   {
   J9ProcessorDesc  *processorDesc        = portLibCall_getJ9ProcessorDesc();
   return mapJ9Processor(processorDesc->physicalProcessor);
   }

TR_Processor mapJ9Processor(J9ProcessorArchitecture j9processor)
   {
   TR_Processor tp = TR_NullProcessor;

   switch(j9processor)
      {
      case PROCESSOR_UNDEFINED:
         tp = TR_NullProcessor;
         break;

      case PROCESSOR_S390_UNKNOWN:
         tp = TR_Default390Processor;
         break;
      case PROCESSOR_S390_GP6:
         tp = TR_s370gp6;
         break;
      case PROCESSOR_S390_GP7:
         tp = TR_s370gp7;
         break;
      case PROCESSOR_S390_GP8:
         tp = TR_s370gp8;
         break;
      case PROCESSOR_S390_GP9:
         tp = TR_s370gp9;
         break;
      case PROCESSOR_S390_GP10:
         tp = TR_s370gp10;
         break;
      case PROCESSOR_S390_GP11:
         tp = TR_s370gp10;          //This is not implemented yet.
         break;

      case PROCESSOR_PPC_UNKNOWN:
         tp = TR_DefaultPPCProcessor;
         break;
      case PROCESSOR_PPC_7XX:
         tp = TR_PPC7xx;
         break;
      case PROCESSOR_PPC_GP:
         tp = TR_PPCgp;
         break;
      case PROCESSOR_PPC_GR:
         tp = TR_PPCgr;
         break;
      case PROCESSOR_PPC_NSTAR:
         tp = TR_PPCnstar;
         break;
      case PROCESSOR_PPC_PULSAR:
         tp = TR_PPCpulsar;
         break;
      case PROCESSOR_PPC_PWR403:
         tp = TR_PPCpwr403;
         break;
      case PROCESSOR_PPC_PWR405:
         tp = TR_PPCpwr405;
         break;
      case PROCESSOR_PPC_PWR440:
         tp = TR_PPCpwr440;
         break;
      case PROCESSOR_PPC_PWR601:
         tp = TR_PPCpwr601;
         break;
      case PROCESSOR_PPC_PWR602:
         tp = TR_PPCpwr602;
         break;
      case PROCESSOR_PPC_PWR603:
         tp = TR_PPCpwr603;
         break;
      case PROCESSOR_PPC_PWR604:
         tp = TR_PPCpwr604;
         break;
      case PROCESSOR_PPC_PWR620:
         tp = TR_PPCpwr620;
         break;
      case PROCESSOR_PPC_PWR630:
         tp = TR_PPCpwr630;
         break;
      case PROCESSOR_PPC_RIOS1:
         tp = TR_PPCrios1;
         break;
      case PROCESSOR_PPC_RIOS2:
         tp = TR_PPCrios2;
         break;
      case PROCESSOR_PPC_P6:
         tp = TR_PPCp6;
         break;
      case PROCESSOR_PPC_P7:
         tp = TR_PPCp7;
         break;
      case PROCESSOR_PPC_P8:
         tp = TR_PPCp8;
         break;

      case PROCESSOR_X86_UNKNOWN:
         tp = TR_DefaultX86Processor;
         break;
      case PROCESSOR_X86_INTELPENTIUM:
         tp = TR_X86ProcessorIntelPentium;
         break;
      case PROCESSOR_X86_INTELP6:
         tp = TR_X86ProcessorIntelP6;
         break;
      case PROCESSOR_X86_INTELPENTIUM4:
         tp = TR_X86ProcessorIntelPentium4;
         break;
      case PROCESSOR_X86_INTELCORE2:
         tp = TR_X86ProcessorIntelCore2;
         break;
      case PROCESSOR_X86_INTELTULSA:
         tp = TR_X86ProcessorIntelTulsa;
         break;
      case PROCESSOR_X86_AMDK5:
         tp = TR_X86ProcessorAMDK5;
         break;
      case PROCESSOR_X86_AMDK6:
         tp = TR_X86ProcessorAMDK6;
         break;
      case PROCESSOR_X86_AMDATHLONDURON:
         tp = TR_X86ProcessorAMDAthlonDuron;
         break;
      case PROCESSOR_X86_AMDOPTERON:
         tp = TR_X86ProcessorAMDOpteron;
         break;
      case PROCESSOR_X86_INTELNEHALEM:             //Not yet mapped in TR_Processor.
      case PROCESSOR_X86_INTELWESTMERE:
      case PROCESSOR_X86_INTELSANDYBRIDGE:
      case PROCESSOR_X86_INTELHASWELL:
         tp = TR_X86ProcessorAMDOpteron;
         break;
      }

   return tp;
   }



static TR_Processor
portLibCall_getX86ProcessorType(const char *vendor, uint32_t processorSignature)
   {
   uint32_t familyCode = (processorSignature & 0x00000f00) >> 8;
   if (!strncmp(vendor, "GenuineIntel", 12))
      {
      switch (familyCode)
         {
         case 0x05:
            return TR_X86ProcessorIntelPentium;

         case 0x06:
            {
            uint32_t modelCode  = (processorSignature & 0x000000f0) >> 4;
            if (modelCode == 0xf)
               return TR_X86ProcessorIntelCore2;
            return TR_X86ProcessorIntelP6;
            }

         case 0x0f:
            return TR_X86ProcessorIntelPentium4;
         }
      }
   else if (!strncmp(vendor, "AuthenticAMD", 12))
      {
      switch (familyCode) // pull out the family code
         {
         case 0x05:
            {
            uint32_t modelCode  = (processorSignature & 0x000000f0) >> 4;
            if (modelCode < 0x04)
               return TR_X86ProcessorAMDK5;
            return TR_X86ProcessorAMDK6;
            }

         case 0x06:
            return TR_X86ProcessorAMDAthlonDuron;

         case 0x0f:
            return TR_X86ProcessorAMDOpteron;
         }
      }

   return TR_DefaultX86Processor;
   }

static uint32_t
portLibCall_getPPCAIX_kernelBitMode()
   {
#if defined(AIXPPC)
   static bitMode = ~0;
   return (bitMode = (bitMode == ~0 ?
                        (uint32_t) sysconf(_SC_AIX_KERNEL_BITMODE) :
                        bitMode));
#else
   return ~0;
#endif
   }

bool
TR_J9VMBase::getDFPBDOption()
   {
   return ((_jitConfig->javaVM->runtimeFlags & J9_RUNTIME_DFPBD) != 0);
   }

bool
TR_J9VMBase::getPPCSupportsVMX()
   {
#if defined(J9OS_I5)&& defined(J9OS_I5_V5R4)
   return FALSE;
#else
   J9ProcessorDesc   *processorDesc       = portLibCall_getJ9ProcessorDesc();
   J9PortLibrary     *privatePortLibrary  = portLibCall_privatePortLibrary();
   BOOLEAN feature = j9sysinfo_processor_has_feature(processorDesc, J9PORT_PPC_FEATURE_HAS_ALTIVEC);
   return (TRUE == feature);
#endif
   }

bool
TR_J9VMBase::getPPCSupportsAES()
   {
#if defined(J9OS_I5)&& defined(J9OS_I5_V5R4)
   return FALSE;
#else
   J9ProcessorDesc   *processorDesc       = portLibCall_getJ9ProcessorDesc();
   J9PortLibrary     *privatePortLibrary  = portLibCall_privatePortLibrary();
   BOOLEAN hasVMX  = j9sysinfo_processor_has_feature(processorDesc, J9PORT_PPC_FEATURE_HAS_ALTIVEC);
   BOOLEAN hasVSX  = j9sysinfo_processor_has_feature(processorDesc, J9PORT_PPC_FEATURE_HAS_VSX);
   BOOLEAN isP8    = (processorDesc->processor >= PROCESSOR_PPC_P8);
   return (isP8 && hasVMX && hasVSX);
#endif
   }

bool
TR_J9VMBase::getPPCSupportsTM()
   {
#if defined(J9OS_I5)&& defined(J9OS_I5_V5R4)
   return FALSE;
#else
   J9ProcessorDesc   *processorDesc       = portLibCall_getJ9ProcessorDesc();
   J9PortLibrary     *privatePortLibrary  = portLibCall_privatePortLibrary();
   BOOLEAN feature = j9sysinfo_processor_has_feature(processorDesc, J9PORT_PPC_FEATURE_HTM);
   return (TRUE == feature);
#endif
   }

bool
TR_J9VMBase::getPPCSupportsVSX()
   {
#if defined(J9OS_I5)&& defined(J9OS_I5_V5R4)
   return feature;
#else
   J9ProcessorDesc   *processorDesc       = portLibCall_getJ9ProcessorDesc();
   J9PortLibrary     *privatePortLibrary  = portLibCall_privatePortLibrary();
   BOOLEAN feature = j9sysinfo_processor_has_feature(processorDesc, J9PORT_PPC_FEATURE_HAS_VSX);
   return (TRUE == feature);
#endif
   }

bool
TR_J9VMBase::getPPCSupportsDFP()
   {
#if !defined(TR_HOST_POWER) || (defined(J9OS_I5)&& defined(J9OS_I5_V5R4))
   return FALSE;
#else
   J9ProcessorDesc   *processorDesc       = portLibCall_getJ9ProcessorDesc();
   J9PortLibrary     *privatePortLibrary  = portLibCall_privatePortLibrary();
   BOOLEAN feature = j9sysinfo_processor_has_feature(processorDesc, J9PORT_PPC_FEATURE_HAS_DFP);
   return (TRUE == feature);
#endif
   }

bool
TR_J9VMBase::getPPCSupportsMTFGPR()
   {
   /*static char *P6Arch = NULL;
   P6Arch = feGetEnv("TR_p6ArchMode");
#if defined(AIXPPC)
   return (_compInfo->getProcessor() == TR_PPCp6 &&
          !(P6Arch == NULL || _system_configuration.version == PV_6_Compat));
#endif
#if defined(LINUX)
   // If the running mode on Linux is not queried from AUX, no point to even
   // think about RAW mode.
   return false;
#endif*/
   return false;
   }

bool
TR_J9VMBase::getS390SupportsDFP()
   {
   return (_compInfo->getS390SupportsDFP()                                      &&
           !(TR_Options::getCmdLineOptions()->getOption(TR_DisableGoldenEagle)) &&
           !(TR_Options::getCmdLineOptions()->getOption(TR_DisableTrex)         &&
           !(TR_Options::getCmdLineOptions()->getOption(TR_DisableZ6))          &&
           !(TR_Options::getCmdLineOptions()->getOption(TR_DisableZArchitecture))));
   }

bool
TR_J9VMBase::getS390SupportsFPE()
   {
   static char *Z10FPE = feGetEnv("TR_disableFPE");
   if (Z10FPE != NULL)
      return false;
   return _compInfo->getS390SupportsFPE();
   }

bool
TR_J9VMBase::getS390SupportsTM()
   {
   return (_compInfo->getS390SupportsTM()                            &&
           !(TR_Options::getCmdLineOptions()->getOption(TR_DisableZHelix)));
   }

bool
TR_J9VMBase::getS390SupportsRI()
   {
   return (_compInfo->getS390SupportsRI()                            &&
           !(TR_Options::getCmdLineOptions()->getOption(TR_DisableZHelix)));
   }

bool
TR_J9VMBase::getSupportsHardwareRound()
   {
   if (isPPC((TR_Target)jitConfig->targetName))
      {
      assume(_compInfo->getProcessor() >= TR_FirstPPCProcessor &&
             _compInfo->getProcessor() <= TR_LastPPCProcessor, "Not a valid PPC Processor Type");

      return (_compInfo->getProcessor() >= TR_FirstPPCHwRoundProcessor);
      }
   else
      {
      return false;
      }
   }

bool
TR_J9VMBase::getSupportsHardwareCopySign()
   {
   if (isPPC((TR_Target)jitConfig->targetName))
      {
      assume(_compInfo->getProcessor() >= TR_FirstPPCProcessor &&
             _compInfo->getProcessor() <= TR_LastPPCProcessor, "Not a valid PPC Processor Type");

      return (_compInfo->getProcessor() >= TR_FirstPPCHwCopySignProcessor);
      }
   else
      {
      return false;
      }
   }

bool
TR_J9VMBase::getSupportsHardwareSQRT()
   {
   TR_Target target = (TR_Target)jitConfig->targetName;
   if(is390(target))
      {
      return true;
      }
   else if(isPPC(target))
      {
      assume(_compInfo->getProcessor() >= TR_FirstPPCProcessor &&
             _compInfo->getProcessor() <= TR_LastPPCProcessor, "Not a valid PPC Processor Type");

      if (_compInfo->getProcessor()>=TR_FirstPPCHwSqrtProcessor)
         return true;
      else
         return false;
      }
   else
      {
      return false;
      }
   }

bool
TR_J9VMBase::getPPCis64bit()
   {
   TR_Processor tp = _compInfo->getProcessor();
   assume(tp >= TR_FirstPPCProcessor &&
          tp <= TR_LastPPCProcessor, "Not a valid PPC Processor Type");

   if (tp>=TR_FirstPPC64BitProcessor)
      return true;

   #if defined(TR_TARGET_64BIT)
   if (tp == TR_DefaultPPCProcessor)
      return true;
   #endif

      return false;
   }

bool
TR_J9VMBase::getPPCis64BitKernel()
   {
#if defined(AIXPPC)
   return (portLibCall_getPPCAIX_kernelBitMode() == 64);
#else
   return getPPCis64bit();
#endif
   }
TR_Processor
TR_J9VMBase::getProcessor()
   {
   return _compInfo->getProcessor();
   }

TR_Processor
TR_J9VMBase::setProcessor(TR_Processor processor)
   {
   return _compInfo->setProcessor(processor);
   }


TR_Processor
TR_J9VMBase::getSchedulingProcessor()
   {
   return _compInfo->getSchedulingProcessor();
   }

TR_Processor
TR_J9VMBase::setSchedulingProcessor(TR_Processor processor)
   {
   return _compInfo->setSchedulingProcessor(processor);
   }

TR_Processor
TR_J9VMBase::getPPCProcessorType()
   {
   return portLibCall_getPPCProcessorType();
   }

UDATA TR_J9VMBase::thisThreadGetStackLimitOffset()                  {return offsetof(J9VMThread, stackOverflowMark);}
UDATA TR_J9VMBase::thisThreadGetHookedStackLimitOffset()            {return offsetof(J9VMThread, J9VMTHREAD_HOOK_FIELD);}
UDATA TR_J9VMBase::thisThreadGetUnhookedStackLimitOffset()          {return offsetof(J9VMThread, J9VMTHREAD_UNHOOK_FIELD);}
UDATA TR_J9VMBase::thisThreadGetTracingBufferSizeOffset()           {return offsetof(J9VMThread, J9VMTHREAD_TRACINGBUFFER_SIZE_FIELD);}
UDATA TR_J9VMBase::thisThreadGetTracingBufferCursorOffset()         {return offsetof(J9VMThread, J9VMTHREAD_TRACINGBUFFER_CURSOR_FIELD);}
UDATA TR_J9VMBase::thisThreadGetTracingBufferTopOffset()            {return offsetof(J9VMThread, J9VMTHREAD_TRACINGBUFFER_TOP_FIELD);}
UDATA TR_J9VMBase::thisThreadGetPendingExceptionOffset()            {return offsetof(J9VMThread, jitException);}
UDATA TR_J9VMBase::thisThreadGetCurrentExceptionOffset()            {return offsetof(J9VMThread, currentException);}
UDATA TR_J9VMBase::thisThreadGetPublicFlagsOffset()                 {return offsetof(J9VMThread, publicFlags);}
UDATA TR_J9VMBase::thisThreadGetJavaPCOffset()                      {return offsetof(J9VMThread, pc);}
UDATA TR_J9VMBase::thisThreadGetJavaSPOffset()                      {return offsetof(J9VMThread, sp);}
#if defined(J9VM_JIT_FREE_SYSTEM_STACK_POINTER)
UDATA TR_J9VMBase::thisThreadGetSystemSPOffset()                    {return offsetof(J9VMThread, systemStackPointer);}
bool  TR_J9VMBase::supportsJITFreeSystemStackPointer()              {return true;}
#endif

UDATA TR_J9VMBase::thisThreadGetJavaLiteralsOffset()                {return offsetof(J9VMThread, literals);}
UDATA TR_J9VMBase::thisThreadGetJavaFrameFlagsOffset()              {return offsetof(J9VMThread, jitStackFrameFlags);}
UDATA TR_J9VMBase::thisThreadGetMachineSPOffset()                   {return sizeof(J9VMThread);}
UDATA TR_J9VMBase::thisThreadGetMachineBPOffset(TR_Compilation * comp) {return comp->cg()->sizeOfJavaPointer();}  // DANGER: this is offset from *(machineSP)
UDATA TR_J9VMBase::thisThreadGetCurrentThreadOffset()               {return offsetof(J9VMThread, threadObject);}
UDATA TR_J9VMBase::thisThreadGetFloatTemp1Offset()                  {return offsetof(J9VMThread, floatTemp1);}
UDATA TR_J9VMBase::thisThreadGetFloatTemp2Offset()                  {return offsetof(J9VMThread, floatTemp2);}
UDATA TR_J9VMBase::thisThreadGetFloatTemp3Offset()                  {return offsetof(J9VMThread, floatTemp3);}
UDATA TR_J9VMBase::thisThreadGetFloatTemp4Offset()                  {return offsetof(J9VMThread, floatTemp4);}
#ifdef ENABLE_INTRINSICS
UDATA TR_J9VMBase::thisThreadGetTempSlotOffset()                    {return offsetof(J9VMThread, tempSlot);}
UDATA TR_J9VMBase::thisThreadGetReturnValueOffset()                 {return offsetof(J9VMThread, returnValue);}
#endif
UDATA TR_J9VMBase::getThreadDebugEventDataOffset(int32_t index) {J9VMThread *dummy=0; return offsetof(J9VMThread, debugEventData1) + (index-1)*sizeof(dummy->debugEventData1);} // index counts from 1
UDATA TR_J9VMBase::getThreadLowTenureAddressPointerOffset()         {return offsetof(J9VMThread, lowTenureAddress);}
UDATA TR_J9VMBase::getThreadHighTenureAddressPointerOffset()        {return offsetof(J9VMThread, highTenureAddress);}
UDATA TR_J9VMBase::getObjectHeaderSizeInBytes()                     {return sizeof(J9Object);}
UDATA TR_J9VMBase::getElementSizeOfBooleanArray()                   {return 1;}
UDATA TR_J9VMBase::getOffsetOfObjectVftField()                      {return TMP_OFFSETOF_J9OBJECT_CLAZZ;}
UDATA TR_J9VMBase::getOffsetOfSuperclassesInClassObject()           {return offsetof(J9Class, superclasses);}
UDATA TR_J9VMBase::getOffsetOfBackfillOffsetField()                 {return offsetof(J9Class, backfillOffset);}

UDATA TR_J9VMBase::getContiguousArrayHeaderSizeInBytes()            {return sizeof(J9IndexableObjectContiguous);}
UDATA TR_J9VMBase::getDiscontiguousArrayHeaderSizeInBytes()         {return sizeof(J9IndexableObjectDiscontiguous);}
UDATA TR_J9VMBase::getOffsetOfContiguousArraySizeField()            {return TMP_OFFSETOF_J9INDEXABLEOBJECTCONTIGUOUS_SIZE;}
UDATA TR_J9VMBase::getOffsetOfDiscontiguousArraySizeField()         {return TMP_OFFSETOF_J9INDEXABLEOBJECTDISCONTIGUOUS_SIZE;}
UDATA TR_J9VMBase::getJ9ObjectContiguousLength()                    {return offsetof(J9IndexableObjectContiguous, size);}
UDATA TR_J9VMBase::getJ9ObjectDiscontiguousLength()                 {return offsetof(J9IndexableObjectContiguous, size);}

UDATA TR_J9VMBase::getOffsetOfArrayClassRomPtrField()               {return offsetof(J9ArrayClass, romClass);}
UDATA TR_J9VMBase::getOffsetOfClassRomPtrField()                    {return offsetof(J9Class, romClass);}
UDATA TR_J9VMBase::getOffsetOfJ9ObjectJ9Class()                     {return offsetof(J9Object, clazz);}
UDATA TR_J9VMBase::getObjectHeaderHasBeenMovedInClass()             {return OBJECT_HEADER_HAS_BEEN_MOVED_IN_CLASS;}
UDATA TR_J9VMBase::getObjectHeaderHasBeenHashedInClass()            {return OBJECT_HEADER_HAS_BEEN_HASHED_IN_CLASS;}
UDATA TR_J9VMBase::getJ9ObjectFlagsMask32()                         {return (J9_REQUIRED_CLASS_ALIGNMENT - 1);} 
UDATA TR_J9VMBase::getJ9ObjectFlagsMask64()                         {return (J9_REQUIRED_CLASS_ALIGNMENT - 1);} 
UDATA TR_J9VMBase::getOffsetOfJ9ThreadJ9VM()                        {return offsetof(J9VMThread, javaVM);} 
UDATA TR_J9VMBase::getJ9GCObjectAlignmentInBytes()                  {return J9_GC_OBJECT_ALIGNMENT_IN_BYTES;}
UDATA TR_J9VMBase::getOffsetOfJ9ROMArrayClassArrayShape()           {return offsetof(J9ROMArrayClass, arrayShape);} 
UDATA TR_J9VMBase::getOffsetOfJavaVMIdentityHashData()              {return offsetof(J9JavaVM, identityHashData);}
UDATA TR_J9VMBase::getOffsetOfJ9IdentityHashData1()                 {return offsetof(J9IdentityHashData, hashData1);}
UDATA TR_J9VMBase::getOffsetOfJ9IdentityHashData2()                 {return offsetof(J9IdentityHashData, hashData2);}
UDATA TR_J9VMBase::getOffsetOfJ9IdentityHashData3()                 {return offsetof(J9IdentityHashData, hashData3);}
UDATA TR_J9VMBase::getOffsetOfJ9IdentityHashDataHashSaltTable()     {return offsetof(J9IdentityHashData, hashSaltTable);}
UDATA TR_J9VMBase::getJ9IdentityHashSaltPolicyStandard()            {return J9_IDENTITY_HASH_SALT_POLICY_STANDARD;}
UDATA TR_J9VMBase::getJ9IdentityHashSaltPolicyRegion()              {return J9_IDENTITY_HASH_SALT_POLICY_REGION;} 
UDATA TR_J9VMBase::getJ9IdentityHashSaltPolicyNone()                {return J9_IDENTITY_HASH_SALT_POLICY_NONE;} 
UDATA TR_J9VMBase::getJ9JavaClassRamShapeShift()                    {return J9_JAVA_CLASS_RAM_SHAPE_SHIFT; }
UDATA TR_J9VMBase::getObjectHeaderShapeMask()                       {return OBJECT_HEADER_SHAPE_MASK; }
UDATA TR_J9VMBase::getObjectHeaderShapePacked()                     {return OBJECT_HEADER_SHAPE_PACKED; }
UDATA TR_J9VMBase::getJ9ClassPackedArrayFlag()                      {return J9ClassPackedArray; }

UDATA TR_J9VMBase::getOffsetOfMaxStackInJ9OSRFrame()                {return offsetof(J9OSRFrame, maxStack);}
 
UDATA TR_J9VMBase::getIdentityHashSaltPolicy(){
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   J9VMThread * vmthread = vmThread();
   J9IdentityHashData *hashData = vmthread->javaVM->identityHashData;
   jint saltPolicy = hashData->hashSaltPolicy;
   releaseVMaccessIfNeeded( vmthread, haveAcquiredVMAccess);
   return saltPolicy;
}

UDATA TR_J9VMBase::getOffsetOfJLThreadJ9Thread(){
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded(); 
   J9VMThread * vmthread = vmThread();
   jint offset;
   offset = (jint) J9VMJAVALANGTHREAD_THREADREF_OFFSET(vmthread);
   releaseVMaccessIfNeeded( vmthread, haveAcquiredVMAccess);
   return offset;
}              

#ifndef J9JIT_HEAP_TAIL_PADDING
#define J9JIT_HEAP_TAIL_PADDING 0
#endif
UDATA TR_J9VMBase::getHeapTailPaddingSizeInBytes()                  {return J9JIT_HEAP_TAIL_PADDING;}
UDATA TR_J9VMBase::getOSRFrameHeaderSizeInBytes()                   {return sizeof(J9OSRFrame);}
UDATA TR_J9VMBase::getOSRBufferSizeInBytes()                        {return sizeof(J9OSRBuffer);}
UDATA TR_J9VMBase::getOSRFrameSizeInBytes(TR_OpaqueMethodBlock* method)  {return osrFrameSize((J9Method*) method);}

bool TR_J9VMBase::ensureOSRBufferSize(uintptrj_t osrFrameSizeInBytes, uintptrj_t osrScratchBufferSizeInBytes, uintptrj_t osrStackFrameSizeInBytes)
   {
   J9JavaVM *vm = _jitConfig->javaVM;
   return ::ensureOSRBufferSize(vm, osrFrameSizeInBytes, osrScratchBufferSizeInBytes, osrStackFrameSizeInBytes);
   }

#if !defined(PUBLIC_BUILD)
ILOps TR_J9VMBase::opCodeForIndirectJavaPackedStore(TR_DataTypes dt)
   {
   return opCodesForIndirectArrayStore[dt];
   }

ILOps TR_J9VMBase::opCodeForIndirectJavaPackedLoad(TR_DataTypes dt)
   {
   return opCodesForIndirectArrayLoad[dt];
   }

bool
TR_J9VMBase::isPackedSupported()
   {
#if defined(J9VM_OPT_PACKED)
   return true;
#else
   return false;
#endif
   }

bool
TR_J9VMBase::isPackedClass(TR_OpaqueClassBlock * clazz)
   {
#ifdef J9VM_OPT_PACKED
   J9Class *clazzPtr = convertClassOffsetToClassPtr(clazz);
   return clazzPtr && J9CLASS_IS_PACKED(clazzPtr);
#else
   return false;
#endif
   }

TR_YesNoMaybe
TR_J9VMBase::isPackedByName(TR_ResolvedMethod *method, char *signature, int32_t len)
   {
#if defined(J9VM_OPT_PACKED)
   if (isPackedSupported())
      {
      J9InternalVMFunctions *intFunc = vmThread()->javaVM->internalVMFunctions;
      UDATA isPacked = intFunc->isPackedByName(vmThread(), ((TR_ResolvedJ9Method *) method)->getClassLoader(), (U_8 *) signature, (UDATA) len);

      if (isPacked == J9ClassPackedConstraintYes)
         return TR_yes;
      else if (isPacked == J9ClassPackedConstraintNo)
         return TR_no;
      }
#endif

   return TR_maybe;
   }

bool
TR_J9VMBase::isPackedArrayClass(TR_OpaqueClassBlock * clazz)
   {
   J9Class *clazzPtr = convertClassOffsetToClassPtr(clazz);
   return clazzPtr && J9CLASS_IS_PACKED_ARRAY(clazzPtr);
   }

TR_YesNoMaybe
TR_J9VMBase::isPackedArrayClassByName(TR_ResolvedMethod *method, char *signature, int32_t len)
   {
   if (isPackedSupported())
      {
      J9InternalVMFunctions *intFunc = vmThread()->javaVM->internalVMFunctions;
      U_8 * sig = (U_8 *)signature;
      UDATA length = (UDATA)len;
      if (sig && sig[0] == 'L')
         {
         sig += 1;
         length -= 2;
         }

      UDATA isPacked = intFunc->isPackedArrayByName(vmThread(), ((TR_ResolvedJ9Method *) method)->getClassLoader(), sig, length);

      if (isPacked == J9ClassPackedConstraintYes)
         {
         return TR_yes;
         }
      else if (isPacked == J9ClassPackedConstraintNo)
         {
         return TR_no;
         }
      else
         {
         assume(0, "Cannot do further processing beyond this point: An array is either packed or is not.");
         return TR_maybe;
         }
      }
   assume(0, "Packed Object support disabled.");
   return TR_no;
   }

TR_Node *
TR_J9VMBase::createIsPackedTest(TR_Compilation *comp, TR_Node *arraySR)
   {
   TR_Node *branchNode = NULL;

#if defined(J9VM_OPT_PACKED)
   if (isPackedSupported())
      {
      TR_Node *arrayBaseAddr = arraySR->getFirstChild();
      TR_Node *index = arraySR->getSecondChild();

      TR_SymbolReferenceTable *symRefTab = comp->getSymRefTab();
      TR_Node *vftPointer = TR_Node::create(comp, TR_iaload, 1, arrayBaseAddr, symRefTab->findOrCreateVftSymbolRef());
      TR_Node *romPointer = TR_Node::create(comp, TR_iaload, 1, vftPointer, symRefTab->findOrCreateClassRomPtrSymbolRef());
      TR_Node *instanceShape = TR_Node::create(comp, TR_iiload, 1, romPointer, symRefTab->findOrCreateInstanceShapeFromROMClassSymbolRef());

      branchNode =
         TR_Node::createif(comp, TR_ificmpeq,
               TR_Node::create(comp, TR_iand, 2,
                  instanceShape,
                  TR_Node::iconst(comp, index, OBJECT_HEADER_SHAPE_MASK)),
               TR_Node::iconst(comp, index, OBJECT_HEADER_SHAPE_PACKED));
      }
#endif

   return branchNode;
   }

// For packed array element, create a newstructref as its packed object header
TR_Node *
TR_J9VMBase::createPackedArrayElementHeader(TR_Compilation *comp, TR_Node *arrayBaseAddr, TR_Node *index)
   {
   TR_SymbolReferenceTable *symRefTab = comp->getSymRefTab();
   bool is64BitTarget = comp->cg()->is64BitTarget();
   TR_Node *objectHandle = NULL;
   TR_Node *offset = NULL;
   TR_Node *elemOffset = NULL;
   TR_Node *vftPointer = NULL;
   TR_Node *packedHeader = NULL;
   ILOps opCode = TR_BadILOp;

   // get packed array header fields: object handle and offset
   objectHandle = TR_Node::create(comp, TR_iaload, 1, arrayBaseAddr, symRefTab->findOrCreateContiguousPackedArrayObjectSymbolRef());
   opCode = is64BitTarget ? TR_ilload : TR_iiload;
   offset = TR_Node::create(comp, opCode, 1, arrayBaseAddr, symRefTab->findOrCreateContiguousPackedArrayOffsetSymbolRef());
   offset->getByteCodeInfo().setDoNotProfile(true);

   // get array component type
   vftPointer = TR_Node::create(comp, TR_iaload, 1, arrayBaseAddr, comp->getSymRefTab()->findOrCreateVftSymbolRef());
   TR_Node *clazzPointer = TR_Node::create(comp, TR_iaload, 1, vftPointer, comp->getSymRefTab()->findOrCreateArrayComponentTypeSymbolRef());

   // get packedDataSize
   opCode = is64BitTarget ? TR_ilload : TR_iiload;
   TR_Node *packedDS = TR_Node::create(comp, opCode, 1, vftPointer, symRefTab->findOrCreateArrayPackedDataSizeSymbolRef());

   // calculate array element relative offset
   if (is64BitTarget)
      {
      elemOffset = TR_Node::create(comp, TR_i2l, 1, index);
      elemOffset = TR_Node::create(comp, TR_lmul, 2, elemOffset, packedDS);
      }
   else
      {
      elemOffset = TR_Node::create(comp, TR_imul, 2, index, packedDS);
      }

   // calculate array element absolute offset
   opCode = is64BitTarget ? TR_ladd : TR_iadd;
   offset = TR_Node::create(comp, opCode, 2, offset, elemOffset);

   offset->getByteCodeInfo().setDoNotProfile(true);
   elemOffset->getByteCodeInfo().setDoNotProfile(true);

   // create the packed object header
   packedHeader = TR_Node::create(comp, TR_newStructRef, 3, clazzPointer, objectHandle, offset, comp->getSymRefTab()->findOrCreateNewStructHeaderSymbolRef());

   return packedHeader;
   }

uintptrj_t
TR_J9VMBase::packedDataPointerOffset()
   {
#ifdef J9VM_OPT_PACKED
   return offsetof(J9PackedObject, object);
#else
   //assume()
   return 0;
#endif
   }

uintptrj_t
TR_J9VMBase::packedDataOffsetOffset()
   {
#ifdef J9VM_OPT_PACKED
   return offsetof(J9PackedObject, offset);
#else
   //assume()
   return 0;
#endif
   }

uintptrj_t
TR_J9VMBase::packedObjectSize()
   {
#ifdef J9VM_OPT_PACKED
   return J9SIZEOF_J9PackedObject;
#else
   //assume()
   return 0;
#endif
   }

UDATA 
TR_J9VMBase::getContiguousPackedArrayHeaderSizeInBytes()      
   {
#if defined(J9VM_OPT_PACKED)
   return isPackedSupported() ? sizeof(J9IndexablePackedObjectContiguous) : 0;
#else
   return 0;
#endif
   }

UDATA 
TR_J9VMBase::getDiscontiguousPackedArrayHeaderSizeInBytes()   
   {
#if defined(J9VM_OPT_PACKED)
   return isPackedSupported() ? sizeof(J9IndexablePackedObjectDiscontiguous) : 0;
#else
   return 0;
#endif
   }

UDATA 
TR_J9VMBase::getOffsetOfArrayPackedDataSizeField()            
   {
#if defined(J9VM_OPT_PACKED)
   return isPackedSupported() ? offsetof(J9ArrayClass, packedDataSize) : 0;
#else
   return 0;
#endif
   }

UDATA
TR_J9VMBase::getArrayPackedDataSize(TR_OpaqueClassBlock *clazz)
   {
#if defined(J9VM_OPT_PACKED)
   return isPackedSupported() ?
      ((J9ArrayClass *)convertClassOffsetToClassPtr(clazz))->packedDataSize : 0;
#else
   return 0;
#endif
   }

UDATA 
TR_J9VMBase::getOffsetOfContiguousPackedArrayObjectField()    
   {
#if defined(J9VM_OPT_PACKED)
   return isPackedSupported() ? offsetof(J9IndexablePackedObjectContiguous, object) : 0;
#else
   return 0;
#endif
   }

UDATA 
TR_J9VMBase::getOffsetOfContiguousPackedArrayOffsetField()    
   {
#if defined(J9VM_OPT_PACKED)
   return isPackedSupported() ? offsetof(J9IndexablePackedObjectContiguous, offset) : 0;
#else
   return 0;
#endif
   }

UDATA
TR_J9VMBase::getOffsetOfDiscontiguousPackedArrayObjectField() 
   {
#if defined(J9VM_OPT_PACKED)
   return isPackedSupported() ? offsetof(J9IndexablePackedObjectDiscontiguous, object) : 0;
#else
   return 0;
#endif
   }

UDATA 
TR_J9VMBase::getOffsetOfDiscontiguousPackedArrayOffsetField() 
   {
#if defined(J9VM_OPT_PACKED)
   return isPackedSupported() ? offsetof(J9IndexablePackedObjectDiscontiguous, offset) : 0;
#else
   return 0;
#endif
   }

UDATA
TR_J9VMBase::getOffsetOfInstanceShapeFromROMClassField()
   {
#if defined(J9VM_OPT_PACKED)
   return isPackedSupported() ? offsetof(J9ROMClass, instanceShape) : 0;
#else
   return 0;
#endif
   }
#endif  // !PUBLIC_BUILD

UDATA TR_J9VMBase::getMaskOfObjectVftField()
   {
   if (getOffsetOfHeaderFlags() != getOffsetOfObjectVftField())
      {
      // Flags are not in the VFT field, so no need for a mask
      ///static char *alwaysMaskVFTPointers = feGetEnv("TR_alwaysMaskVFTPointers");
      ///if (!alwaysMaskVFTPointers)
      if (TR_Options::getCmdLineOptions()->getOption(TR_DisableMaskVFTPointers))
         return ~(UDATA)0;
      }
   return (UDATA)(-J9_REQUIRED_CLASS_ALIGNMENT);
   }

UDATA TR_J9VMBase::thisThreadGetDLTBlockOffset()
   {
#if defined(J9VM_JIT_DYNAMIC_LOOP_TRANSFER)
   return offsetof(J9VMThread, dltBlock);
#else
   return 0;
#endif
   }

UDATA TR_J9VMBase::getDLTBufferOffsetInBlock()
   {
#if defined(J9VM_JIT_DYNAMIC_LOOP_TRANSFER)
   return offsetof(J9DLTInformationBlock, temps);
#else
   return 0;
#endif
   }

bool
TR_J9VMBase::compiledAsDLTBefore(TR_ResolvedMethod * method)
   {
#if defined(J9VM_JIT_DYNAMIC_LOOP_TRANSFER)
   return _compInfo->searchForDLTRecord(((TR_ResolvedJ9Method*)method)->ramMethod(), -1) != NULL;
#else
   return 0;
#endif
   }

static void dumpOneKnownObjectTo(FEFILE file, TR_J9KnownObjectTable *knot, TR_KnownObjectTable::Index i, TR_Compilation *comp, int pointerLen=0)
   {
   assume(comp->fe()->haveAccess(), "Must haveAccess in dumpOneKnownObjectTo");
   assume(!knot->isNull(i), "dumpOneKnownObjectTo cannot dump the null object");
   TR_J9VMBase *j9fe = (TR_J9VMBase*)comp->fe();
   J9MemoryManagerFunctions * mmf = jitConfig->javaVM->memoryManagerFunctions;
   uintptrj_t *ref = knot->getPointerLocation(i);
   int32_t len; char *className = j9fe->getClassNameChars(j9fe->getObjectClass(*ref), len);
   int32_t hashCode = mmf->j9gc_objaccess_getObjectHashCode((J9VMToken *)j9fe->vmThread(), (J9Object*)(*ref));
   fefprintf(j9fe, file, "  obj%-3d   %p   %p %8x   %.*s\n", i, ref, *ref, hashCode, len, className);
   }

TR_J9KnownObjectTable::TR_J9KnownObjectTable(TR_Compilation *comp):
   TR_KnownObjectTable(comp->fe()),
   _comp(comp),
   _references(comp->trMemory())
   {
   _references.add(NULL); // Reserve index zero for NULL
   }

TR_KnownObjectTable *
TR_J9VMBase::allocateKnownObjectTable(TR_Compilation *comp)
   {
   return new (comp->trHeapMemory()) TR_J9KnownObjectTable(comp);
   }

TR_KnownObjectTable::Index
TR_J9KnownObjectTable::getEndIndex()
   {
   return _references.size();
   }

bool
TR_J9KnownObjectTable::isNull(Index index)
   {
   return index == 0;
   }

TR_KnownObjectTable::Index
TR_J9KnownObjectTable::getIndex(uintptrj_t objectPointer)
   {
   assume(fe()->haveAccess(), "Must haveAccess in TR_J9KnownObjectTable::getIndex");
   if (objectPointer == 0)
      return 0; // Special Index value for NULL

   // Search for existing matching entry
   //
   uint32_t nextIndex = getEndIndex();
   for (uint32_t i = 1; i < nextIndex; i++)
      if (*_references.element(i) == objectPointer)
         return i;

   // No luck -- allocate a new one
   //
   J9VMThread *thread = getJ9VMThreadFromTR_VM(fe());
   assume0(thread);
   _references.setSize(nextIndex+1);
   _references[nextIndex] = (uintptrj_t*)thread->javaVM->internalVMFunctions->j9jni_createLocalRef((JNIEnv*)thread, (j9object_t)objectPointer);

   if (_comp->getOption(TR_TraceOptDetails))
      {
      TR_J9VMBase *j9fe = (TR_J9VMBase*)fe();
      bool haveAcquiredVMAccess = false;
      if (j9fe->tryToAcquireAccess(_comp, &haveAcquiredVMAccess))
         {
         dumpOneKnownObjectTo(_comp->getOutFile(), this, nextIndex, _comp);
         if (haveAcquiredVMAccess)
            j9fe->releaseAccess(_comp);
         }
      }

   return nextIndex;
   }

uintptrj_t *
TR_J9KnownObjectTable::getPointerLocation(Index index)
   {
   assume3(index != UNKNOWN && 0 <= index && index < _references.size(), "getPointerLocation(%d): index must be in range 0..%d", (int)index, _references.size());
   return _references[index];
   }

static int32_t simpleNameOffset(const char *className, int32_t len)
   {
   int32_t result;
   for (result = len; result > 0 && className[result-1] != '/'; result--)
      {}
   return result;
   }

void
TR_J9KnownObjectTable::dumpObjectTo(FEFILE file, Index i, const char *fieldName, const char *sep, TR_Compilation *comp, TR_BitVector &visited, TR_VMFieldsInfo **fieldsInfoByIndex, int32_t depth)
   {
   TR_J9VMBase *j9fe = (TR_J9VMBase*)fe();
   int32_t indent = 2*depth;
   if (comp->getKnownObjectTable()->isNull(i))
      {
      // Usually don't care about null fields
      // fefprintf(j9fe, file, "%*s%s%snull\n", indent, "", fieldName, sep);
      return;
      }
   else if (visited.isSet(i))
      {
      fefprintf(j9fe, file, "%*s%s%sobj%d\n", indent, "", fieldName, sep, i);
      return;
      }
   else
      {
      visited.set(i);

      uintptrj_t *ref = getPointerLocation(i);
      int32_t len; char *className = j9fe->getClassNameChars(j9fe->getObjectClass(*ref), len);
      J9MemoryManagerFunctions * mmf = jitConfig->javaVM->memoryManagerFunctions;
      int32_t hashCode = mmf->j9gc_objaccess_getObjectHashCode((J9VMToken *)j9fe->vmThread(), (J9Object*)(*ref));

      // Shorten the class name for legibility.  The full name is still in the ordinary known-object table dump.
      //
      int32_t offs = simpleNameOffset(className, len);
      fefprintf(j9fe, file, "%*s%s%sobj%d @ %p hash %8x %.*s", indent, "", fieldName, sep, i, *ref, hashCode, len-offs, className+offs);

      if (len == 29 && !strncmp("java/lang/invoke/DirectHandle", className, 29))
         {
         J9Method *j9method  = (J9Method*)J9VMJAVALANGINVOKEMETHODHANDLE_VMSLOT(j9fe->vmThread(), (J9Object*)(*ref));
         J9UTF8   *className = J9ROMCLASS_CLASSNAME(J9_CLASS_FROM_METHOD(j9method)->romClass);
         J9UTF8   *methName  = J9ROMMETHOD_NAME(J9_ROM_METHOD_FROM_RAM_METHOD(j9method));
         int32_t offs = simpleNameOffset(utf8Data(className), J9UTF8_LENGTH(className));
         fefprintf(j9fe, file, "  vmSlot: %.*s.%.*s",
            J9UTF8_LENGTH(className)-offs, utf8Data(className)+offs,
            J9UTF8_LENGTH(methName),       utf8Data(methName));
         }

      TR_VMFieldsInfo *fieldsInfo = fieldsInfoByIndex[i];
      if (fieldsInfo)
         {
         ListIterator<TR_VMField> primitiveIter(fieldsInfo->getFields());
         for (TR_VMField *field = primitiveIter.getFirst(); field; field = primitiveIter.getNext())
            {
            if (field->isReference())
               continue;
            if (!strcmp(field->signature, "I"))
               fefprintf(j9fe, file, "  %s: %d", field->name, j9fe->getInt32Field(*ref, field->name));
            }
         fefprintf(j9fe, file, "\n");
         ListIterator<TR_VMField> refIter(fieldsInfo->getFields());
         for (TR_VMField *field = refIter.getFirst(); field; field = refIter.getNext())
            {
            if (field->isReference())
               {
               uintptrj_t target = j9fe->getReferenceField(*ref, field->name, field->signature);
               Index targetIndex = getExistingIndexAt(&target);
               if (targetIndex != UNKNOWN)
                  dumpObjectTo(file, targetIndex, field->name, (field->modifiers & J9AccFinal)? " is " : " = ", comp, visited, fieldsInfoByIndex, depth+1);
               }
            }
         }
      else
         {
         fefprintf(j9fe, file, "\n");
         }
      }
   }

void
TR_J9KnownObjectTable::dumpTo(FEFILE file, TR_Compilation *comp)
   {
   TR_J9VMBase *j9fe = (TR_J9VMBase*)fe();
   J9MemoryManagerFunctions * mmf = jitConfig->javaVM->memoryManagerFunctions;
   bool haveAcquiredVMAccess = false;
   if (j9fe->tryToAcquireAccess(comp, &haveAcquiredVMAccess))
      {
      fefprintf(j9fe, file, "<knownObjectTable size=\"%d\"> // ", getEndIndex());
      int32_t pointerLen = fefprintf(j9fe, file, "%p", this);
      fefprintf(j9fe, file, "\n  %-6s   %-*s   %-*s %-8s   Class\n", "id", pointerLen, "JNI Ref", pointerLen, "Address", "Hash");
      for (Index i = 0; i < getEndIndex(); i++)
         {
         if (isNull(i))
            fefprintf(j9fe, file, "  obj%-3d   %*s   NULL\n", i, pointerLen, "");
         else
            dumpOneKnownObjectTo(file, this, i, comp, pointerLen);
         }
      fefprintf(j9fe, file, "</knownObjectTable>\n");

      if (comp->getOption(TR_TraceKnownObjectGraph))
         {
         fefprintf(j9fe, file, "<knownObjectGraph>\n");

         Index i;
         void* stackMark = comp->trMemory()->markStack();

         // Collect field info and determine which objects are reachable from other objects
         //
         TR_BitVector reachable(getEndIndex(), comp->trMemory(), stackAlloc, notGrowable);
         TR_VMFieldsInfo **fieldsInfoByIndex = (TR_VMFieldsInfo**)alloca(getEndIndex() * sizeof(TR_VMFieldsInfo*));
         for (i = 1; i < getEndIndex(); i++)
            {
            uintptrj_t    object = getPointer(i);
            J9Class      *clazz  = (J9Class*)j9fe->getObjectClass(object);
            if (clazz->romClass->modifiers & J9AccClassArray)
               {
               fieldsInfoByIndex[i] = NULL;
               continue; // TODO: Print out what reference arrays contain?
               }
            fieldsInfoByIndex[i] = new (comp->trStackMemory()) TR_VMFieldsInfo(comp, clazz, 1, stackAlloc);
            ListIterator<TR_VMField> fieldIter(fieldsInfoByIndex[i]->getFields());
            for (TR_VMField *field = fieldIter.getFirst(); field; field = fieldIter.getNext())
               {
               // For the purpose of "reachability", we only look at final
               // fields.  The intent is to reduce nondeterminism in the object
               // graph log.
               //
               if (field->isReference() && (field->modifiers & J9AccFinal))
                  {
                  uintptrj_t target = j9fe->getReferenceField(object, field->name, field->signature);
                  Index targetIndex = getExistingIndexAt(&target);
                  if (targetIndex != UNKNOWN)
                     reachable.set(targetIndex);
                  }
               }
            }

         // At the top level, walk objects not reachable from other objects
         //
         TR_BitVector visited(getEndIndex(), comp->trMemory(), stackAlloc, notGrowable);
         for (i = 1; i < getEndIndex(); i++)
            {
            if (!reachable.isSet(i) && !visited.isSet(i))
               dumpObjectTo(file, i, "", "", comp, visited, fieldsInfoByIndex, 0);
            }

         comp->trMemory()->releaseStack(stackMark);

         fefprintf(j9fe, file, "</knownObjectGraph>\n");
         }
      if (haveAcquiredVMAccess)
         j9fe->releaseAccess(comp);
      }
   else
      {
      fefprintf(j9fe, file, "<knownObjectTable size=\"%d\"/> // unable to acquire VM access to print table contents\n", getEndIndex());
      }
   }

void
TR_J9VMBase::freeKnownObjectTable(TR_KnownObjectTable *table, TR_Compilation *comp)
   {
   if (table)
      {
      J9VMThread *thread = vmThread();
      assume0(thread);

      TR_J9KnownObjectTable *j9table = (TR_J9KnownObjectTable*)table;

      bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
      TR_ArrayIterator<uintptrj_t> i(&j9table->_references);
      for (uintptrj_t *ref = i.getFirst(); !i.pastEnd(); ref = i.getNext())
         thread->javaVM->internalVMFunctions->j9jni_deleteLocalRef((JNIEnv*)thread, (jobject)ref);
      releaseVMaccessIfNeeded(thread, haveAcquiredVMAccess);
      }
   }

TR_OpaqueClassBlock *
TR_J9VMBase::getObjectClass(uintptrj_t objectPointer)
   {
   assume(haveAccess(), "Must haveAccess in getObjectClass");
   if (generateCompressedObjectHeaders())
      return (TR_OpaqueClassBlock*)((uintptrj_t)*(uint32_t*)(objectPointer + getOffsetOfObjectVftField()) & getMaskOfObjectVftField());
   else
      return (TR_OpaqueClassBlock*)(*(uintptrj_t*)(objectPointer + getOffsetOfObjectVftField()) & getMaskOfObjectVftField());
   }

uintptrj_t
TR_J9VMBase::getReferenceFieldAtAddress(uintptrj_t fieldAddress)
   {
   assume(haveAccess(), "Must haveAccess in getReferenceFieldAtAddress");
#if defined(J9VM_GC_COMPRESSED_POINTERS)
   uintptrj_t compressedResult = *(uint32_t*)fieldAddress;
   return (compressedResult << getCompressedPointersShift()) + getHeapBaseAddress();
#else
   return *(uintptrj_t*)fieldAddress;
#endif
   }

uintptrj_t
TR_J9VMBase::getReferenceFieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset)
   {
   assume(haveAccess(), "Must haveAccess in getReferenceFieldAt");
   return getReferenceFieldAtAddress(objectPointer + sizeof(J9Object) + fieldOffset);
   }

uintptrj_t
TR_J9VMBase::getVolatileReferenceFieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset)
   {
   assume(haveAccess(), "Must haveAccess in getVolatileReferenceFieldAt");
   return (uintptrj_t)vmThread()->javaVM->javaVM->memoryManagerFunctions->j9gc_objaccess_mixedObjectReadObject((J9VMToken*)vmThread(),
      (J9Object*)objectPointer, J9_OBJECT_HEADER_SIZE + fieldOffset, IS_VOLATILE);
   }

int32_t
TR_J9VMBase::getInt32FieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset)
   {
   assume(haveAccess(), "Must haveAccess in getInt32Field");
   return *(int32_t*)(objectPointer + sizeof(J9Object) + fieldOffset);
   }

void
TR_J9VMBase::setInt32FieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset, int32_t newValue)
   {
   assume(haveAccess(), "Must haveAccess in setInt32Field");
   *(int32_t*)(objectPointer + sizeof(J9Object) + fieldOffset) = newValue;
   }

int32_t
TR_J9VMBase::getVolatileInt32FieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset)
   {
   assume(haveAccess(), "Must haveAccess in getVolatileInt32Field");
   return (int32_t)vmThread()->javaVM->javaVM->memoryManagerFunctions->j9gc_objaccess_mixedObjectReadI32((J9VMToken*)vmThread(),
      (J9Object*)objectPointer, J9_OBJECT_HEADER_SIZE + fieldOffset, IS_VOLATILE);
   }

void
TR_J9VMBase::setVolatileInt32FieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset, int32_t newValue)
   {
   assume(haveAccess(), "Must haveAccess in setVolatileInt32Field");
   vmThread()->javaVM->javaVM->memoryManagerFunctions->j9gc_objaccess_mixedObjectStoreI32((J9VMToken*)vmThread(),
      (J9Object*)objectPointer, J9_OBJECT_HEADER_SIZE + fieldOffset, newValue, IS_VOLATILE);
   }

bool
TR_J9VMBase::compareAndSwapInt32FieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset, int32_t oldValue, int32_t newValue)
   {
   assume(haveAccess(), "Must haveAccess in compareAndSwapInt32FieldAt");
   UDATA success = vmThread()->javaVM->javaVM->memoryManagerFunctions->j9gc_objaccess_mixedObjectCompareAndSwapInt((J9VMToken*)vmThread(),
      (J9Object*)objectPointer, fieldOffset + getObjectHeaderSizeInBytes(), oldValue, newValue);
   return success != 0;
   }

int64_t
TR_J9VMBase::getInt64FieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset)
   {
   assume(haveAccess(), "Must haveAccess in getInt64Field");
   return *(int64_t*)(objectPointer + sizeof(J9Object) + fieldOffset);
   }

void
TR_J9VMBase::setInt64FieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset, int64_t newValue)
   {
   assume(haveAccess(), "Must haveAccess in setInt64Field");
   *(int64_t*)(objectPointer + sizeof(J9Object) + fieldOffset) = newValue;
   }

int64_t
TR_J9VMBase::getVolatileInt64FieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset)
   {
   assume(haveAccess(), "Must haveAccess in getVolatileInt64Field");
   return (int64_t)vmThread()->javaVM->javaVM->memoryManagerFunctions->j9gc_objaccess_mixedObjectReadI64((J9VMToken*)vmThread(),
      (J9Object*)objectPointer, J9_OBJECT_HEADER_SIZE + fieldOffset, IS_VOLATILE);
   }

void
TR_J9VMBase::setVolatileInt64FieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset, int64_t newValue)
   {
   assume(haveAccess(), "Must haveAccess in setVolatileInt64Field");
   vmThread()->javaVM->javaVM->memoryManagerFunctions->j9gc_objaccess_mixedObjectStoreI64((J9VMToken*)vmThread(),
      (J9Object*)objectPointer, J9_OBJECT_HEADER_SIZE + fieldOffset, newValue, IS_VOLATILE);
   }

bool
TR_J9VMBase::compareAndSwapInt64FieldAt(uintptrj_t objectPointer, uintptrj_t fieldOffset, int64_t oldValue, int64_t newValue)
   {
   assume(haveAccess(), "Must haveAccess in compareAndSwapInt64FieldAt");
   UDATA success = vmThread()->javaVM->javaVM->memoryManagerFunctions->j9gc_objaccess_mixedObjectCompareAndSwapLong((J9VMToken*)vmThread(),
      (J9Object*)objectPointer, fieldOffset + getObjectHeaderSizeInBytes(), oldValue, newValue);
   return success != 0;
   }

intptrj_t
TR_J9VMBase::getArrayLengthInElements(uintptrj_t objectPointer)
   {
   assume(haveAccess(), "Must haveAccess in getArrayLengthInElements");
   int32_t result = *(int32_t*)(objectPointer + getOffsetOfContiguousArraySizeField());
   if (useHybridArraylets() && (result == 0))
      result = *(int32_t*)(objectPointer + getOffsetOfDiscontiguousArraySizeField());
   return (intptrj_t)result;
   }

int32_t
TR_J9VMBase::getInt32Element(uintptrj_t objectPointer, int32_t elementIndex)
   {
   assume(haveAccess(), "getInt32Element requires VM access");

   uintptrj_t dataStart = objectPointer;
   bool       hasSpine  = canGenerateArraylets() && !useHybridArraylets();

   const uint32_t elementSize = sizeof(int32_t);

   intptrj_t arrayLengthInBytes = getArrayLengthInElements(dataStart) * elementSize;
   if (canGenerateArraylets() && useHybridArraylets() && isDiscontiguousArray(arrayLengthInBytes))
      {
      dataStart += sizeof(J9IndexableObjectDiscontiguous);
      hasSpine = true;
      }
   else
      {
      dataStart += sizeof(J9IndexableObjectContiguous);
      }

   int32_t result;
   if (hasSpine)
      {
#if defined(J9VM_GC_COMPRESSED_POINTERS)
      uint32_t *spine = (uint32_t*)dataStart;
      uintptrj_t compressedArrayletPtr = (uintptrj_t)(spine[elementIndex >> getArraySpineShift(elementSize)]);
      int32_t *arraylet = (int32_t*)((compressedArrayletPtr << getCompressedPointersShift()) + getHeapBaseAddress());
#else
      uintptrj_t *spine = (uintptrj_t*)dataStart;
      int32_t *arraylet = (int32_t *)(spine[elementIndex >> getArraySpineShift(elementSize)]);
#endif
      result = arraylet[elementIndex & getArrayletMask(elementSize)];
      }
   else
      {
      result = ((int32_t*)dataStart)[elementIndex];
      }

   return result;
   }

int64_t
TR_J9VMBase::getInt64Element(uintptrj_t objectPointer, int32_t elementIndex)
   {
   assume(haveAccess(), "getInt64Element requires VM access");
   assume(0, "Not yet implemented"); // TODO
   return -1;
   }

uintptrj_t
TR_J9VMBase::getReferenceElement(uintptrj_t objectPointer, intptrj_t elementIndex)
   {
   assume(haveAccess(), "getReferenceElement requires VM access");

   uintptrj_t dataStart = objectPointer;
   bool hasSpine = canGenerateArraylets() && !useHybridArraylets();

#if defined(J9VM_GC_COMPRESSED_POINTERS)
   const uint32_t elementSize = sizeof(uint32_t);
#else
   const uint32_t elementSize = sizeof(uintptrj_t);
#endif

   intptrj_t arrayLengthInBytes = getArrayLengthInElements(dataStart) * elementSize;
   if (canGenerateArraylets() && useHybridArraylets() && isDiscontiguousArray(arrayLengthInBytes))
      {
      dataStart += sizeof(J9IndexableObjectDiscontiguous);
      hasSpine = true;
      }
   else
      {
      dataStart += sizeof(J9IndexableObjectContiguous);
      }

   uintptrj_t rawResult;
   if (hasSpine)
      {
#if defined(J9VM_GC_COMPRESSED_POINTERS)
      uint32_t *spine = (uint32_t*)dataStart;
      uintptrj_t compressedArrayletPtr = (uintptrj_t)(spine[elementIndex >> getArraySpineShift(elementSize)]);
      uint32_t *arraylet = (uint32_t*)((compressedArrayletPtr << getCompressedPointersShift()) + getHeapBaseAddress());
#else
      uintptrj_t *spine = (uintptrj_t*)dataStart;
      uintptrj_t *arraylet = (uintptrj_t *)(spine[elementIndex >> getArraySpineShift(elementSize)]);
#endif
      rawResult = arraylet[elementIndex & getArrayletMask(elementSize)];
      }
   else
      {
#if defined(J9VM_GC_COMPRESSED_POINTERS)
      uint32_t *elementArray = (uint32_t*)dataStart;
#else
      uintptrj_t *elementArray = (uintptrj_t*)dataStart;
#endif
      rawResult = elementArray[elementIndex];
      }

#if defined(J9VM_GC_COMPRESSED_POINTERS)
   return (rawResult << getCompressedPointersShift()) + getHeapBaseAddress();
#else
   return rawResult;
#endif

   }

TR_OpaqueClassBlock *
TR_J9VMBase::getClassFromJavaLangClass(uintptrj_t objectPointer)
   {
   return (TR_OpaqueClassBlock*)J9VM_J9CLASS_FROM_HEAPCLASS(_vmThread, objectPointer);
   }

uintptrj_t
TR_J9VMBase::getJavaLangClassFromClass(TR_OpaqueClassBlock *clazz)
   {
   return (uintptrj_t)J9VM_J9CLASS_TO_HEAPCLASS((J9Class*)clazz);
   }

uintptrj_t
TR_J9VMBase::getConstantPoolFromMethod(TR_OpaqueMethodBlock *method)
   {
   return (uintptrj_t)J9_CP_FROM_METHOD((J9Method *)method);
   }

void TR_J9VMBase::emitNewPseudoRandomNumberVerbosePrefix()
   {
   TR_VerboseLog::vlogAcquire();
   TR_VerboseLog::writeLine(TR_Vlog_INFO,"%s ", PSEUDO_RANDOM_NUMBER_PREFIX);
   //vlogRelease();
   }

void TR_J9VMBase::emitNewPseudoRandomNumberVerbose(int32_t i)
   {
   //vlogAcquire();
   TR_VerboseLog::write("%d ", i);
   //vlogRelease();
   }

void TR_J9VMBase::emitNewPseudoRandomVerboseSuffix()
   {
   //vlogAcquire();
   TR_VerboseLog::write("%c ", PSEUDO_RANDOM_SUFFIX);
   TR_VerboseLog::vlogRelease();
   }


void TR_J9VMBase::emitNewPseudoRandomNumberVerboseLine(int32_t i)
   {
   //vlogAcquire();
   emitNewPseudoRandomNumberVerbosePrefix();
   emitNewPseudoRandomNumberVerbose(i);
   emitNewPseudoRandomVerboseSuffix();
   //vlogRelease();
   }



void TR_J9VMBase::emitNewPseudoRandomStringVerbosePrefix()
   {
   TR_VerboseLog::vlogAcquire();
   TR_VerboseLog::writeLine(TR_Vlog_INFO,"%s ", PSEUDO_RANDOM_STRING_PREFIX);
   //vlogRelease();
   }

void TR_J9VMBase::emitNewPseudoRandomStringVerbose(char *c)
   {
   //vlogAcquire();
   TR_VerboseLog::write("%s ", c);
   //vlogRelease();
   }


void TR_J9VMBase::emitNewPseudoRandomStringVerboseLine(char *c)
   {
     //vlogAcquire();
   emitNewPseudoRandomStringVerbosePrefix();
   emitNewPseudoRandomStringVerbose(c);
   emitNewPseudoRandomVerboseSuffix();
   //vlogRelease();
   }


bool TR_J9VMBase::newHeaderShape()
   {
#if defined(J9VM_INTERP_NEW_HEADER_SHAPE)
   return true;
#else
   return false;
#endif
   }

UDATA TR_J9VMBase::getLowTenureAddress()
   {
   return (UDATA) _vmThread->lowTenureAddress;
   }

UDATA TR_J9VMBase::getHighTenureAddress()
   {
   return (UDATA) _vmThread->highTenureAddress;
   }

bool TR_J9VMBase::generateClassesOnHeap()
   {
   return true;
   }

bool TR_J9VMBase::generateCompressedObjectHeaders()
   {
#if defined(J9VM_INTERP_COMPRESSED_OBJECT_HEADER)
   return true;
#else
   return false;
#endif
   }


bool TR_J9VMBase::generateCompressedPointers()
   {
   return TR_Options::useCompressedPointers();
   }


bool TR_J9VMBase::generateCompressedLockWord()
   {
#if defined(J9VM_THR_LOCK_NURSERY)
   if (sizeof(j9objectmonitor_t) == 4)
      return true;
#else
   if (sizeof((((J9Object *)NULL)->monitor)) == 4)
      return true;
#endif
   return false;
   }


int32_t *TR_J9VMBase::getCurrentLocalsMapForDLT(TR_Compilation *comp)
   {
   int32_t           *currentBundles = NULL;

#if defined(J9VM_JIT_DYNAMIC_LOOP_TRANSFER)
   TR_ResolvedMethod *currentMethod = comp->getCurrentMethod();
   J9Method          *j9method = (J9Method *)(currentMethod->getPersistentIdentifier());
   int32_t            numBundles = currentMethod->numberOfTemps() + currentMethod->numberOfParameterSlots();

   numBundles = (numBundles+31)/32;
   currentBundles = (int32_t *)comp->trMemory()->allocateHeapMemory(numBundles * sizeof(int32_t));
   jitConfig->javaVM->localMapFunction(_portLibrary, J9_CLASS_FROM_METHOD(j9method)->romClass, J9_ROM_METHOD_FROM_RAM_METHOD(j9method), comp->getDltBcIndex(), (U_32 *)currentBundles, NULL, NULL, NULL);
#endif

   return currentBundles;
   }

UDATA TR_J9VMBase::getOffsetOfJavaLangClassFromClassField()
   {
   return offsetof(J9Class, classObject);
   }


UDATA TR_J9VMBase::getOffsetOfInstanceShapeFromClassField()
   {
   return offsetof(J9Class, totalInstanceSize);
   }

UDATA TR_J9VMBase::getOffsetOfInstanceDescriptionFromClassField()
   {
   return offsetof(J9Class, instanceDescription);
   }

UDATA TR_J9VMBase::getOffsetOfDescriptionWordFromPtrField()
   {
   return 0;
   }


UDATA TR_J9VMBase::getOffsetOfClassFromJavaLangClassField()
   {
   return J9VMCONSTANTPOOL_ADDRESS_OFFSET(J9VMTOKEN_JAVAVM(_vmThread), J9VMCONSTANTPOOL_JAVALANGCLASS_VMREF);
   }

UDATA TR_J9VMBase::getOffsetOfAddressOfClassOfMethodSymbol()
   {
   return 0; // Used only as a dereference of a pointer, so 0 should always be correct
   }

UDATA TR_J9VMBase::getOffsetOfHeaderFlags()
   {
#if defined(J9VM_INTERP_FLAGS_IN_CLASS_SLOT)
   return getOffsetOfObjectVftField();
#else
   #if defined(TMP_OFFSETOF_J9OBJECT_FLAGS)
      return TMP_OFFSETOF_J9OBJECT_FLAGS;
   #else
      return 0;
   #endif
#endif
   }

UDATA TR_J9VMBase::getOffsetOfRamStaticsFromClassField()            {return offsetof(J9Class, ramStatics);}
UDATA TR_J9VMBase::getOffsetOfIsArrayFieldFromRomClass()            {return offsetof(J9ROMClass, modifiers);}
UDATA TR_J9VMBase::getOffsetOfClassAndDepthFlags()                  {return offsetof(J9Class, classDepthAndFlags);}
UDATA TR_J9VMBase::getOffsetOfClassFlags()                          {return offsetof(J9Class, classFlags);}
UDATA TR_J9VMBase::getOffsetOfPackedDataSize()                      {return offsetof(J9Class, packedDataSize);}
UDATA TR_J9VMBase::getOffsetOfClassTenantStatusIndex()              {
                                                                    #if defined(J9VM_OPT_TENANT)
                                                                    return offsetof(J9Class, tenantInitStatus);
                                                                    #else
                                                                    return 0;
                                                                    #endif
                                                                    }
UDATA TR_J9VMBase::getOffsetOfArrayComponentTypeField()             {return offsetof(J9ArrayClass, componentType);}
UDATA TR_J9VMBase::constReleaseVMAccessOutOfLineMask()              {return J9_PUBLIC_FLAGS_VMACCESS_RELEASE_BITS;}
UDATA TR_J9VMBase::constReleaseVMAccessMask()                       {return ~constAcquireVMAccessOutOfLineMask();}
UDATA TR_J9VMBase::constAcquireVMAccessOutOfLineMask()              {return J9_PUBLIC_FLAGS_VMACCESS_ACQUIRE_BITS;}
UDATA TR_J9VMBase::constJNICallOutFrameFlags()                      {return J9_SSF_JIT_JNI_CALLOUT;}
UDATA TR_J9VMBase::constJNICallOutFrameType()                       {return J9SF_FRAME_TYPE_JIT_JNI_CALLOUT;}
UDATA TR_J9VMBase::constJNICallOutFrameSpecialTag()                 {return 0;}
UDATA TR_J9VMBase::constJNICallOutFrameInvisibleTag()               {return J9SF_A0_INVISIBLE_TAG;}
UDATA TR_J9VMBase::constJNICallOutFrameFlagsOffset()                {return offsetof(J9SFMethodFrame, specialFrameFlags);}
UDATA TR_J9VMBase::constJNICallOutFrameBPOffset()                   {return offsetof(J9SFMethodFrame, savedA0);}

UDATA TR_J9VMBase::constJNIReferenceFrameAllocatedFlags()       {return J9_SSF_JIT_JNI_FRAME_COLLAPSE_BITS;}
UDATA TR_J9VMBase::constGenericTransitionFromJITFrameFlags()    {return J9_SSF_JIT_NATIVE_TRANSITION_FRAME;}

UDATA TR_J9VMBase::constClassFlagsPrimitive()   {return J9_JAVA_CLASS_PRIMITIVE_TYPE;}
UDATA TR_J9VMBase::constClassFlagsAbstract()    {return J9_JAVA_ABSTRACT;}
UDATA TR_J9VMBase::constClassFlagsFinal()       {return J9_JAVA_FINAL;}
UDATA TR_J9VMBase::constClassFlagsPublic()      {return J9_JAVA_PUBLIC;}

int32_t TR_J9VMBase::getFlagValueForPrimitiveTypeCheck()        {return J9AccClassInternalPrimitiveType;}
int32_t TR_J9VMBase::getFlagValueForArrayCheck()                {return J9AccClassArray;}
int32_t TR_J9VMBase::getFlagValueForFinalizerCheck()            {return J9_JAVA_CLASS_FINALIZE | J9_JAVA_CLASS_OWNABLE_SYNCHRONIZER;}


UDATA TR_J9VMBase::getGCForwardingPointerOffset()               {
                                                                   return 0;
                                                                }
UDATA TR_J9VMBase::javaVMHeapBaseOffset()                       {
                                                                #if defined(J9VM_OPT_REAL_TIME_EXTENSIONS)
                                                                   return offsetof(J9JavaVM, heapBase);
                                                                #else
                                                                   assume(0,"heapBase field offset inaccessible");
                                                                   return 0;
                                                                #endif
                                                                }
UDATA TR_J9VMBase::javaVMHeapTopOffset()                        {
                                                                #if defined(J9VM_OPT_REAL_TIME_EXTENSIONS)
                                                                   return offsetof(J9JavaVM, heapTop);
                                                                #else
                                                                   assume(0,"heapTop field offset inaccessible");
                                                                   return 0;
                                                                #endif
                                                                }

void *TR_J9VMBase::getHeapBase()
                                                                {
                                                                #if defined(J9VM_OPT_REAL_TIME_EXTENSIONS)
                                                                   return _jitConfig->javaVM->heapBase;
                                                                #else
                                                                   assume(0,"heapBase field inaccessible");
                                                                   return 0;
                                                                #endif
                                                                }

void *TR_J9VMBase::getHeapTop()
                                                                {
                                                                #if defined(J9VM_OPT_REAL_TIME_EXTENSIONS)
                                                                   return _jitConfig->javaVM->heapTop;
                                                                #else
                                                                   assume(0,"heapTop field inaccessible");
                                                                   return 0;
                                                                #endif
                                                                }
UDATA TR_J9VMBase::javaVMScopesCreatedCountOffset()             {
                                                                #if defined(J9VM_OPT_REAL_TIME_EXTENSIONS)
                                                                   //return offsetof(J9JavaVM, scopesCreatedCount);
                                                                   return 0;
                                                                #else
                                                                   assume(0,"scopesCreatedCount field offset inaccessible");
                                                                   return 0;
                                                                #endif
                                                                }

bool TR_J9VMBase::getNurserySpaceBounds(uintptrj_t *base, uintptrj_t *top)
   {
   J9JavaVM *vm = _jitConfig->javaVM;
   J9MemoryManagerFunctions * mmf = vm->memoryManagerFunctions;
   mmf->j9mm_get_guaranteed_nursery_range(getJ9JITConfig()->javaVM, (void**) base, (void**) top);

   return true;
   }

bool TR_J9VMBase::jniRetainVMAccess(TR_ResolvedMethod *method) { return (((TR_ResolvedJ9Method *)method)->getJNIProperties() & J9_FAST_JNI_RETAIN_VM_ACCESS) != 0; }
bool TR_J9VMBase::jniNoGCPoint(TR_ResolvedMethod *method) { return (((TR_ResolvedJ9Method *)method)->getJNIProperties() & J9_FAST_JNI_NOT_GC_POINT) != 0; }
bool TR_J9VMBase::jniNoNativeMethodFrame(TR_ResolvedMethod *method) { return (((TR_ResolvedJ9Method *)method)->getJNIProperties() & J9_FAST_NO_NATIVE_METHOD_FRAME) != 0; }
bool TR_J9VMBase::jniNoExceptionsThrown(TR_ResolvedMethod *method) { return (((TR_ResolvedJ9Method *)method)->getJNIProperties() & J9_FAST_JNI_NO_EXCEPTION_THROW) != 0; }
bool TR_J9VMBase::jniNoSpecialTeardown(TR_ResolvedMethod *method) { return (((TR_ResolvedJ9Method *)method)->getJNIProperties() & J9_FAST_JNI_NO_SPECIAL_TEAR_DOWN) != 0; }
bool TR_J9VMBase::jniDoNotWrapObjects(TR_ResolvedMethod *method) { return (((TR_ResolvedJ9Method *)method)->getJNIProperties() & J9_FAST_JNI_DO_NOT_WRAP_OBJECTS) != 0; }
bool TR_J9VMBase::jniDoNotPassReceiver(TR_ResolvedMethod *method) { return (((TR_ResolvedJ9Method *)method)->getJNIProperties() & J9_FAST_JNI_DO_NOT_PASS_RECEIVER) != 0; }
bool TR_J9VMBase::jniDoNotPassThread(TR_ResolvedMethod *method) { return (((TR_ResolvedJ9Method *)method)->getJNIProperties() & J9_FAST_JNI_DO_NOT_PASS_THREAD) != 0; }

UDATA
TR_J9VMBase::thisThreadRememberedSetFragmentOffset()
   {

#if defined(J9VM_GC_REALTIME)
      return offsetof(J9VMThread, staccatoRememberedSetFragment);
#endif
   return 0;
   }

UDATA
TR_J9VMBase::getFragmentParentOffset()
   {

#if defined(J9VM_GC_REALTIME)
    return offsetof(J9VMGCRememberedSetFragment, fragmentParent);
#endif
   return 0;
   }

UDATA
TR_J9VMBase::getRememberedSetGlobalFragmentOffset()
   {
#if defined(J9VM_GC_REALTIME)
      return offsetof(J9VMGCRememberedSet, globalFragmentIndex);
#endif
   return 0;
   }

UDATA
TR_J9VMBase::getLocalFragmentOffset()
   {
#if defined(J9VM_GC_REALTIME)
       return offsetof(J9VMGCRememberedSetFragment, localFragmentIndex);
#endif
    return 0;
   }

UDATA
TR_J9VMBase::thisThreadMemorySpaceOffset()
   {
#if defined(J9VM_GC_REALTIME)
      return offsetof(J9VMThread, memorySpace);
#endif
   return 0;
   }

UDATA
TR_J9VMBase::thisThreadJavaVMOffset()
   {
   return offsetof(J9VMThread, javaVM);
   }

UDATA
TR_J9VMBase::javaVMDefaultMemorySpaceOffset()
   {
   return offsetof(J9JavaVM, defaultMemorySpace);
   }

UDATA
TR_J9VMBase::getMaxObjectSizeForSizeClass()
   {
#if defined(J9VM_GC_REALTIME)
      return J9VMGC_SIZECLASSES_MAX_SMALL_SIZE_BYTES;
#endif
   return 0;
   }

UDATA
TR_J9VMBase::thisThreadAllocationCacheCurrentOffset(uintptrj_t sizeClass)
   {
#if defined(J9VM_GC_REALTIME)
      return offsetof(J9VMThread, segregatedAllocationCache) +
         sizeClass * sizeof(J9VMGCSegregatedAllocationCacheEntry) +
         offsetof(J9VMGCSegregatedAllocationCacheEntry, current);
#endif
   return 0;
   }

UDATA
TR_J9VMBase::thisThreadAllocationCacheTopOffset(uintptrj_t sizeClass)
   {
#if defined(J9VM_GC_REALTIME)
      return offsetof(J9VMThread, segregatedAllocationCache) +
         sizeClass * sizeof(J9VMGCSegregatedAllocationCacheEntry) +
         offsetof(J9VMGCSegregatedAllocationCacheEntry, top);
#endif
   return 0;
   }

UDATA
TR_J9VMBase::getCellSizeForSizeClass(uintptrj_t sizeClass)
   {
#if defined(J9VM_GC_REALTIME)
      J9JavaVM * javaVM = _jitConfig->javaVM;
      return javaVM->realtimeSizeClasses.smallCellSizes[sizeClass];
#endif
   return 0;
   }

UDATA
TR_J9VMBase::getObjectSizeClass(uintptrj_t objectSize)
   {
#if defined(J9VM_GC_REALTIME)
      J9JavaVM * javaVM = _jitConfig->javaVM;
      return javaVM->realtimeSizeClasses.sizeClassIndex[objectSize / sizeof(UDATA)];
#endif
   return 0;
   }

UDATA
TR_J9VMBase::thisThreadMonitorCacheOffset()
   {
#if defined(J9VM_OPT_REAL_TIME_LOCKING_SUPPORT)
   return offsetof(J9VMThread, monitorCache);
#else
   assume(0,"no monitorCache thread slot");
   return 0;
#endif
   }

UDATA
TR_J9VMBase::thisThreadOSThreadOffset()
   {
   return offsetof(J9VMThread, osThread);
   }

UDATA
TR_J9VMBase::getMonitorNextOffset()
   {
#if defined(J9VM_OPT_REAL_TIME_LOCKING_SUPPORT)
   return offsetof(J9ThreadAbstractMonitor, next);
#else
   assume(0,"no next field in J9ThreadAbstractMonitor");
   return 0;
#endif
   }

UDATA
TR_J9VMBase::getMonitorOwnerOffset()
   {
   return offsetof(J9ThreadAbstractMonitor, owner);
   }

UDATA
TR_J9VMBase::getMonitorEntryCountOffset()
   {
   return offsetof(J9ThreadAbstractMonitor, count);
   }

UDATA
TR_J9VMBase::getSmallCellSizesOffset()
   {
#if defined(J9VM_GC_REALTIME)
      return offsetof(J9JavaVM, realtimeSizeClasses) + offsetof(J9VMGCSizeClasses, smallCellSizes);
#endif
   return 0;
   }

UDATA
TR_J9VMBase::getJavaVMSizeClassesOffset()
   {
#if defined(J9VM_GC_REALTIME)
      return offsetof(J9JavaVM, realtimeSizeClasses);
#endif
   return 0;
   }

UDATA
TR_J9VMBase::getSizeClassesIndexOffset()
   {
#if defined(J9VM_GC_REALTIME)
      return offsetof(J9JavaVM, realtimeSizeClasses) + offsetof(J9VMGCSizeClasses, sizeClassIndex);
#endif
   return 0;
   }

UDATA TR_J9VMBase::thisThreadGetPendingAIEOffset()
   {
#if defined(J9VM_OPT_REAL_TIME_EXTENSIONS)
   return offsetof(J9VMThread, pendingAIE);
#else
   assume(0,"no pending AIE thread slot in non-RT builds");
   return 0;
#endif
   }

UDATA TR_J9VMBase::thisThreadGetClinitDepthOffset()
   {
#if defined(J9VM_OPT_REAL_TIME_EXTENSIONS)
   return offsetof(J9VMThread, clinitDepth);
#else
   assume(0,"no clinit depth thread slot in non-RT builds");
   return 0;
#endif
   }

UDATA TR_J9VMBase::thisThreadGetPrivateFlagsOffset()
   {
   return offsetof(J9VMThread, privateFlags);
   }

UDATA TR_J9VMBase::thisThreadGetProfilingBufferCursorOffset()
   {
   return offsetof(J9VMThread, profilingBufferCursor);
   }

UDATA TR_J9VMBase::thisThreadGetProfilingBufferEndOffset()
   {
   return offsetof(J9VMThread, profilingBufferEnd);
   }

UDATA TR_J9VMBase::thisThreadGetOSRBufferOffset()
   {
   return offsetof(J9VMThread, osrBuffer);
   }

UDATA TR_J9VMBase::thisThreadGetOSRScratchBufferOffset()
   {
   return offsetof(J9VMThread, osrScratchBuffer);
   }

UDATA TR_J9VMBase::thisThreadGetOSRFrameIndexOffset()
   {
   return offsetof(J9VMThread, osrFrameIndex);
   }

UDATA TR_J9VMBase::thisThreadGetOSRReturnAddressOffset()
   {
   return offsetof(J9VMThread, osrReturnAddress);
   }

UDATA TR_J9VMBase::thisThreadGetTenantData32Offset()
   {
#if defined(J9VM_OPT_TENANT)
   return offsetof(J9VMThread, tenantData32);
#else
   return 0;
#endif
   }

UDATA TR_J9VMBase::thisThreadGetTenantData64Offset()
   {
#if defined(J9VM_OPT_TENANT)
   return offsetof(J9VMThread, tenantData64);
#else
   return 0;
#endif
   }

UDATA TR_J9VMBase::thisThreadGetTenantDataRefOffset()
   {
#if defined(J9VM_OPT_TENANT)
   return offsetof(J9VMThread, tenantDataObj);
#else
   return 0;
#endif
   }

UDATA TR_J9VMBase::thisThreadGetTenantDataInitOffset()
   {
#if defined(J9VM_OPT_TENANT)
   return offsetof(J9VMThread, tenantDataInitStatus);
#else
   return 0;
#endif
   }

UDATA TR_J9VMBase::thisThreadGetTenantContextOffset()
   {
#if defined(J9VM_OPT_TENANT)
   return offsetof(J9VMThread, tenantObject);
#else
   return 0;
#endif
   }

UDATA TR_J9VMBase::getNoHeapRealTimeThreadMask()
   {
   #if defined(J9VM_OPT_REAL_TIME_EXTENSIONS)
   return J9_PRIVATE_FLAGS_NO_HEAP_REALTIME_THREAD;
   #else
      assume(0,"no NoHeapRealtimeThread mask in non-RT builds");
      return 0;
   #endif
   }


// This query answers whether or not this frontend is capable of generating arraylet
// trees.  This does not imply that it will for this compilation unit: you must ask
// Compilation::generateArraylets() to answer that.
//
bool TR_J9VMBase::canGenerateArraylets()
   {

#if defined(J9VM_GC_COMBINATION_SPEC)

   return TR_Options::getCmdLineOptions()->usesDiscontiguousArraylets();

#else

#if defined(J9VM_GC_ARRAYLETS)
   return true;
#else
   return false;
#endif

#endif
   }

bool TR_J9VMBase::isComboGCSpec()
   {
   #ifdef J9VM_GC_COMBINATION_SPEC
      return true;
   #else
      return false;
   #endif
   }

// This query answers whether or not the commandline option -XCEEHDLR was passed in.
//
bool TR_J9VMBase::CEEHDLREnabled()
   {
#if defined(J9VM_PORT_ZOS_CEEHDLRSUPPORT)
   J9JavaVM * vm = _jitConfig->javaVM;

   if (J9_SIG_ZOS_CEEHDLR == (vm->sigFlags & J9_SIG_ZOS_CEEHDLR))
      return true;
#endif

   return false;
   }


bool TR_J9VMBase::useHybridArraylets()
   {
#if defined(J9VM_GC_COMBINATION_SPEC)
   return TR_Options::getCmdLineOptions()->usesDiscontiguousArraylets();
#else

   #if defined(J9VM_GC_HYBRID_ARRAYLETS)
      return true;
   #else
      return false;
   #endif

#endif
   }


// Answers whether this compilation may need spine checks.  The FE will answer yes
// if true discontiguous arrays could appear at all with this GC policy, but its a
// conservative answer.  The corresponding compilation query may know better for
// this compilation unit.
//
bool TR_J9VMBase::mayRequireSpineChecks()
   {
   return useHybridArraylets();
   }


int32_t TR_J9VMBase::getArraySpineShift(int32_t width)
   {
   assume(canGenerateArraylets(), "not supposed to be generating arraylets!");
   assume(width >= 0, "unexpected arraylet datatype width");

   // for elements larger than bytes, need to reduce the shift because fewer elements
   // fit into each arraylet

   int32_t shift=-1;

   #if defined(J9VM_GC_COMBINATION_SPEC)
   int32_t maxShift = _jitConfig->javaVM->memoryManagerFunctions->j9gc_arraylet_getLeafLogSize(jitConfig->javaVM);
   #else
      #if defined(J9VM_GC_ARRAYLETS)
         int32_t maxShift = _jitConfig->javaVM->memoryManagerFunctions->j9gc_arraylet_getLeafLogSize(jitConfig->javaVM);
      #else
         int32_t maxShift = 0;
         assume(0,"arraylets not supported");
      #endif
   #endif

   switch(width)
      {
      case 1 : shift = maxShift-0; break;
      case 2 : shift = maxShift-1; break;
      case 4 : shift = maxShift-2; break;
      case 8 : shift = maxShift-3; break;
      default: assume(0,"unexpected element width");
      }
   return shift;
   }

int32_t TR_J9VMBase::getArrayletMask(int32_t width)
   {
   assume(canGenerateArraylets(), "not supposed to be generating arraylets!");
   assume(width >= 0, "unexpected arraylet datatype width");
   int32_t mask=(1 << getArraySpineShift(width))-1;
   return mask;
   }

int32_t TR_J9VMBase::getArrayletLeafIndex(int32_t index, int32_t elementSize)
   {
   assume(canGenerateArraylets(), "not supposed to be generating arraylets!");
   assume(elementSize >= 0, "unexpected arraylet datatype width");

   if (index<0)
      return -1;
   int32_t arrayletIndex = (index >> getArraySpineShift(elementSize));
   return  arrayletIndex;
   }

int32_t TR_J9VMBase::getLeafElementIndex(int32_t index , int32_t elementSize)
   {
   assume(canGenerateArraylets(), "not supposed to be generating arraylets!");
   assume(elementSize >= 0, "unexpected arraylet datatype width");

   if (index<0)
      return -1;
   int32_t leafIndex = (index & getArrayletMask(elementSize));
   return leafIndex;
   }

int32_t TR_J9VMBase::getArrayletLeafSize()
   {

#ifdef J9VM_GC_COMBINATION_SPEC
   return (int32_t)(getJ9JITConfig()->javaVM->memoryManagerFunctions->j9gc_arraylet_getLeafSize(getJ9JITConfig()->javaVM));
#else
   #if defined(J9VM_GC_ARRAYLETS)
      return (int32_t)(getJ9JITConfig()->javaVM->memoryManagerFunctions->j9gc_arraylet_getLeafSize(getJ9JITConfig()->javaVM));
   #else
      assume(0, "can't generate arraylets with this VM!");
      return 0;
   #endif
#endif
   }


// Returns the maximum contiguous arraylet size in bytes NOT including the header.
//
int32_t TR_J9VMBase::getMaxContiguousArraySizeInBytes()
   {
   return getArrayletLeafSize() - getContiguousArrayHeaderSizeInBytes();
   }


// 'sizeInBytes' should NOT include the header
//
bool TR_J9VMBase::isDiscontiguousArray(int32_t sizeInBytes)
   {

#if defined(J9VM_GC_ARRAYLETS) || defined(J9VM_GC_COMBINATION_SPEC)
   if (sizeInBytes > getMaxContiguousArraySizeInBytes())
      return true;
   else
      {
      if (useHybridArraylets() && sizeInBytes == 0)
         return true;
      else
         return false;
      }
#else
   return false;
#endif
   }


// 'sizeInElements' should NOT include the header
//
bool TR_J9VMBase::isDiscontiguousArray(int32_t sizeInElements, int32_t elementSize)
   {
#if defined(J9VM_GC_ARRAYLETS) || defined(J9VM_GC_COMBINATION_SPEC)
   int32_t shift = trailingZeroes(elementSize);
   int32_t maxContiguousArraySizeInElements = getMaxContiguousArraySizeInBytes() >> shift;

   if (sizeInElements > maxContiguousArraySizeInElements)
      return true;
   else
      {
      if (useHybridArraylets() && sizeInElements == 0)
         return true;
      else
         return false;
      }
#else
   return false;
#endif
   }


int32_t TR_J9VMBase::getObjectHeaderFixedValue()
   {
   return OBJECT_HEADER_FIXED;
   }

int32_t TR_J9VMBase::getFirstArrayletPointerOffset(TR_Compilation *comp)
   {
   assume(canGenerateArraylets(), "not supposed to be generating arraylets!");

   int32_t headerSize = useHybridArraylets() ?
      sizeof(J9IndexableObjectDiscontiguous) : sizeof(J9IndexableObjectContiguous);

   return (headerSize + getAddressFieldSize()-1) & (-1)*(intptrj_t)(getAddressFieldSize());
   }

int32_t TR_J9VMBase::getArrayletFirstElementOffset(int8_t elementSize, TR_Compilation *comp)
   {
   assume(canGenerateArraylets(), "not supposed to be generating arraylets!");
   assume(elementSize >= 0, "unexpected arraylet element size");

   int32_t offset;
#if defined(J9VM_GC_ARRAYLETS) && defined(J9VM_GC_COMPRESSED_POINTERS)
   offset = (getFirstArrayletPointerOffset(comp) + getAddressFieldSize() + sizeof(UDATA)-1) & (-(int32_t)sizeof(UDATA));
   assume((offset & sizeof(UDATA)-1) == 0, "unexpected alignment for first arraylet element");
#else
   if (elementSize > sizeof(UDATA))
      offset = (getFirstArrayletPointerOffset(comp) + sizeof(UDATA) + elementSize-1) & (-elementSize);
   else
      offset = getFirstArrayletPointerOffset(comp) + sizeof(UDATA);

   assume((offset & (elementSize-1)) == 0, "unexpected alignment for first arraylet element");
#endif

   return offset;
   }

// This is used on Z/OS direct JNI to restore the CAA register before calling to C.
//
int32_t TR_J9VMBase::getCAASaveOffset()
   {
   #if defined(J9TR_CAA_SAVE_OFFSET)
      return J9TR_CAA_SAVE_OFFSET;
   #else
      return 0;
   #endif
   }

uint32_t
TR_J9VMBase::getByteOffsetToGCFlags()
   {
   return _jitConfig->targetLittleEndian ?  getWordOffsetToGCFlags() : getWordOffsetToGCFlags() + 3;
   }

uint32_t
TR_J9VMBase::getWordOffsetToGCFlags()
   {
#if defined(J9VM_INTERP_FLAGS_IN_CLASS_SLOT) && defined(TR_TARGET_64BIT) && !defined(J9VM_INTERP_COMPRESSED_OBJECT_HEADER)
      return getOffsetOfHeaderFlags() + 4;
#else
      return getOffsetOfHeaderFlags();
#endif
   }

uint32_t
TR_J9VMBase::getWriteBarrierGCFlagMaskAsByte()
   {
   return (OBJECT_HEADER_OLD) >> 8; //shift right 8 bits
   }

uint32_t
TR_J9VMBase::getWriteBarrierGCFlagMaskAsWord()
   {
   return OBJECT_HEADER_OLD;
   }

uint32_t
TR_J9VMBase::getObjectHashCodeShift(bool is64BitTarget)
   {
   return 13;
   }


int32_t
TR_J9VMBase::getByteOffsetToLockword(TR_OpaqueClassBlock * clazzPointer)
   {
#if defined (J9VM_THR_LOCK_NURSERY)
   J9JavaVM * jvm = _jitConfig->javaVM;

   if (isAOT() || (clazzPointer == NULL))
      return 0;

   //if (getClassDepth(clazzPointer) == 0)
   //   return 0; // java.lang.Object has non-inheritable lockword, so if this is all we know at compile time, it isn't enough.
   return convertClassOffsetToClassPtr(clazzPointer)->lockOffset;
#else
   return TMP_OFFSETOF_J9OBJECT_MONITOR;
#endif
   }

bool
TR_J9VMBase::isLogSamplingSet()
   {
   return TR_Options::getJITCmdLineOptions() && TR_Options::getJITCmdLineOptions()->getVerboseOption(TR_VerboseSampling);
   }

void
TR_J9VMBase::fatalError(int32_t error)
   {

   }

UDATA
TR_J9VMBase::getOffsetOfIndexableSizeField()
   {
   // This is now a 32-bit load - DJS
   //return offsetof(J9ROMArrayClass, arrayShape) + (jitConfig->targetLittleEndian ? 0 : 2);
   return offsetof(J9ROMArrayClass, arrayShape);
   }

// These two isSafe methods are potentially temporary hacks used by SingleThreadedOpts
// to assert that an unresolved class load or method call won't create a thread.  If
// these methods continue to be exist there use could be extended to assert that the methods
// don't modify statics, etc?
//
bool
TR_J9VMBase::isSafeUnresolvedClassReference(char * name, int32_t len)
   {
   // if getClassLoader == bootstrap loader ??

   switch (len)
      {
      case 40:
         // java/lang/ArrayIndexOutOfBoundsException
         if (!strncmp(name, "java/lang/ArrayIndexOutOfBoundsException", len))
            return true;
      }
   return false;
   }

bool
TR_J9VMBase::isSafeUnresolvedCall(TR_Compilation * comp, TR_Method * vmMethod)
   {
   // assert that all methods of certain class are 'safe'
   //
   int32_t len = vmMethod->classNameLength();
   switch (len)
      {
      case 40:
         {
         char * name = classNameToSignature(vmMethod->classNameChars(), len, comp);
         // java/lang/ArrayIndexOutOfBoundsException
         if (!strncmp(name, "java/lang/ArrayIndexOutOfBoundsException", len))
            return true;
         }
      }

   return false;
   }

bool
TR_J9VMBase::isLimitExceeded(int32_t totalStackAndHeapMemory)
   {
   intptrj_t limit = ((TR_JitPrivateConfig *)_jitConfig->privateConfig)->scratchSpaceLimitKB*1024;

   return (totalStackAndHeapMemory > limit
           /*&&_jitConfig->outOfMemoryJumpBuffer != NULL*/);
   }

void
TR_J9VMBase::setScratchMemoryLimit(int32_t limitKB)
   {
   ((TR_JitPrivateConfig *)_jitConfig->privateConfig)->scratchSpaceLimitKB = limitKB;
   }

extern "C" void loadDebugDLL(J9JavaVM *, J9JITConfig *);

// This method can be called by an app thread in onLoadInternal
TR_DebugBase *
TR_J9VMBase::createDebug(TR_Compilation *comp)
   {
   //bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   if (!_jitConfig->tracingHook)
      loadDebugDLL(_jitConfig->javaVM, jitConfig);


   TR_InternalFunctions * internalFunctions;
   if (comp)
      {
      //internalFunctions object local to the compilation
      internalFunctions = new (comp->trHeapMemory()) TR_InternalFunctions(comp->fe(), comp->trMemory(), comp);
      }
   else
      {
      //Here we are looking for the global internalFunctions object, we want to use one stored in TR_Options!
      internalFunctions = TR_Options::findOrCreateInternalFunctions();
      }
   TR_DebugBase *result = (internalFunctions && _jitConfig->tracingHook) ? ((TR_CreateDebug_t)_jitConfig->tracingHook)(comp, internalFunctions) : 0;
   //releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

TR_InternalFunctions *TR_J9VMBase::createInternalFunctions() 
   {
   TR_InternalFunctions * internalFunctions = new (PERSISTENT_NEW) TR_InternalFunctions(TR_J9VMBase::get(_jitConfig, 0),  _compInfo->scratchTrMemory(), 0);
   return internalFunctions;
   }   
   
TR_ResolvedMethod *
TR_J9VMBase::getDefaultConstructor(TR_Memory * trMemory, TR_OpaqueClassBlock * classPointer)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   List<TR_ResolvedMethod> list(trMemory);
   getResolvedMethods(trMemory, classPointer, &list);
   ListIterator<TR_ResolvedMethod> methods(&list);
   TR_ResolvedMethod * m = methods.getFirst();
   for (; m; m = methods.getNext())
      if (m->isConstructor() && m->signatureLength() == 3 && !strncmp(m->signatureChars(), "()V", 3))
         break;
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return m;
   }

extern "C" J9NameAndSignature newInstancePrototypeNameAndSig;
#if defined(J9ZOS390)
#pragma variable(newInstancePrototypeNameAndSig, NORENT)
#endif

uint32_t
TR_J9VMBase::getNewInstanceImplVirtualCallOffset()
   {
   return offsetof(J9Class, romableAotITable); // should be something like offsetof(J9Class, newInstanceImpl)
   }


U_8 *
TR_J9VMBase::allocateDataCacheRecord(uint32_t numBytes, TR_Compilation *comp,
                                     bool contiguous, bool *shouldRetryAllocation,
                                     uint32_t allocationType, uint32_t *allocatedSizePtr)
   {
   U_8* retValue = NULL;


   
   if (contiguous || ((_jitConfig->runtimeFlags & J9JIT_TOSS_CODE) && comp) )
      {
      // need to allocate space for header too and do some alignment
      uint32_t size = TR_DataCacheManager::alignToMachineWord(numBytes + sizeof(J9JITDataCacheHeader));
      U_8* ptr = NULL;
      assume(comp, "Contiguous allocations require compilation object");
      *shouldRetryAllocation = false;
      // Increment the space needed by this compilation in the data cache,
      // even if the allocation might fail
      //
      comp->incrementTotalNeededDataCacheSpace(size);
      TR_DataCache *dataCache = (TR_DataCache*)comp->getReservedDataCache();
      // If we have a reserved data cache, use that in preference of any other
      if (dataCache)
         {
         ptr = dataCache->allocateDataCacheSpace(size);
         if (!ptr) // designated data cache is not big enough
            {
            TR_DataCacheManager::getManager()->retireDataCache(dataCache);
            // Reserve a new segment that is capable to hold the entire size
            // TODO: find the appropriate vmThread
            dataCache = TR_DataCacheManager::getManager()->reserveAvailableDataCache(_vmThread, comp->getTotalNeededDataCacheSpace());
            comp->setReservedDataCache(dataCache);
            if (dataCache)
               {
               *shouldRetryAllocation = true;
               }
            }
         }
      else // No reserved data cache
         {
         // If we want contiguous allocation we must reserve a data cache now
         dataCache = TR_DataCacheManager::getManager()->reserveAvailableDataCache(_vmThread, comp->getTotalNeededDataCacheSpace());
         comp->setReservedDataCache(dataCache);
         if (dataCache)
            {
            ptr = dataCache->allocateDataCacheSpace(size);
            }
         }
      if (ptr) // I managed to allocate some space
         {
         // Complete the header
         TR_DataCacheManager::getManager()->fillDataCacheHeader((J9JITDataCacheHeader*)ptr, allocationType, size);
         if (allocatedSizePtr)
            *allocatedSizePtr = size - sizeof(J9JITDataCacheHeader); // communicate back the allocated size
          // Return the location after the header
         retValue = (ptr + sizeof(J9JITDataCacheHeader));
         }
      }
   else // not a contiguous allocation, use data cache manager.
      {
      retValue = TR_DataCacheManager::getManager()->allocateDataCacheRecord(numBytes, allocationType, allocatedSizePtr);
      }
#ifdef DATA_CACHE_DEBUG
      fprintf(stderr, "allocateDataCacheRecord: numBytes=%u comp=%p contiguous=%d allocationType=%u size=%u retValue=%p vmThread=%p\n",
              numBytes, comp, contiguous, allocationType, *allocatedSizePtr, retValue, _vmThread);
#endif

   return retValue;
   }



U_8 *
TR_J9VMBase::allocateRelocationData(TR_Compilation * comp, uint32_t numBytes)
   {
   U_8 * relocationData = NULL;
   uint32_t size = 0;
   bool shouldRetryAllocation;
   relocationData = allocateDataCacheRecord(numBytes, comp, isAOT(), &shouldRetryAllocation,
                                            J9_JIT_DCE_RELOCATION_DATA, &size);
   if (!relocationData)
      {
      if (shouldRetryAllocation)
         comp->setErrorCode(COMPILATION_OUT_OF_MEMORY_RELOCATION_DATA); // force a retry
      outOfMemory(comp, "relocation data");
      }
#ifndef TR_MEM_STATS
   if (debug("metaDataStats"))
#endif
      metaDataStats._relocationSize += size;
   return relocationData;
   }


static void switchCodeCache(TR_Compilation *comp, void *oldCache, void *newCache)
   {
   comp->switchCodeCache(newCache);
   // If the old CC had pre-loaded code, the current compilation may have initialized it and will therefore depend on it
   // so we should initialize it in the new CC as well
   // XXX: We could avoid this if we knew for sure that this compile wasn't the one who initialized it
   if (newCache && ((TR_MCCCodeCache*)oldCache)->isCCPreLoadedCodeInitialized())
      ((TR_MCCCodeCache*)newCache)->getCCPreLoadedCodeAddress(TR_numCCPreLoadedCode, comp->cg());
   }

U_8 *
TR_J9VMBase::allocateCodeMemory(TR_Compilation * comp, uint32_t warmCodeSize, uint32_t coldCodeSize, uint8_t ** coldCode, bool isMethodHeaderNeeded)
   {
   void * codeCache = comp->getCurrentCodeCache(); // This is the reserved code cache // MCT

   if (NULL == codeCache)
      {
      if (isAOT())
         comp->setErrorCode(COMPILATION_OUT_OF_MEMORY_RELOCATION_DATA);
#ifdef MCT_DEBUG
      fprintf(stderr, "Aborting compilation %p\n", comp);
#endif
      outOfMemory(comp, "code");
      }

   TR_MCCCodeCache* cc = (TR_MCCCodeCache*)codeCache;
#ifdef MCT_DEBUG
   fprintf(stderr, "comp %p ID=%d codeCache=%p wants to allocate %u bytes of code memory. cache has %d available\n",
           comp, comp->getCompThreadID(), codeCache, warmCodeSize+coldCodeSize, cc->getFreeContiguousSpace());
   if (!((TR_MCCCodeCache*)codeCache)->reserved)
      {
      fprintf(stderr, "Code cache %p belonging to comp %p is not reserved\n", codeCache, comp);
      }
#endif
   assume0(((TR_MCCCodeCache*)codeCache)->isReserved());
   bool hadClassUnloadMonitor;
   bool hadVMAccess = releaseClassUnloadMonitorAndAcquireVMaccessIfNeeded(comp, &hadClassUnloadMonitor);

   U_8 *warmCode = (U_8 *) TR_MCCManager::allocateCodeMemory(warmCodeSize, coldCodeSize, (TR_MCCCodeCache **)&codeCache, coldCode, isAOT(), isMethodHeaderNeeded);

   acquireClassUnloadMonitorAndReleaseVMAccessIfNeeded(comp, hadVMAccess, hadClassUnloadMonitor);

   if (NULL == warmCode)
      {
      if (jitConfig->runtimeFlags & J9JIT_CODE_CACHE_FULL)
         {
         comp->setErrorCode(COMPILATION_CODE_MEMORY_EXHAUSTED);
         }
      else if (isAOT())
         comp->setErrorCode(COMPILATION_OUT_OF_MEMORY_RELOCATION_DATA); // this corresponds to compilationAotCacheFullReloFailure
      else
         comp->setErrorCode(COMPILATION_ALL_CODE_CACHES_RESERVED);
      }
      
   if (codeCache != comp->getCurrentCodeCache())
      {
#ifdef MCT_DEBUG
      fprintf(stderr, "comp %p ID=%d switched cache to %p\n", comp, comp->getCompThreadID(), codeCache);
#endif
      assume(!codeCache || static_cast<TR_MCCCodeCache *>(codeCache)->isReserved(), "Substitute code cache isn't marked as reserved");  // Either we didn't get a code cache, or the one we should get is 
      comp->setAotMethodCodeStart((uint32_t*)warmCode);
      switchCodeCache(comp, comp->getCurrentCodeCache(), codeCache);
      }

   if ( comp->getErrorCode() != COMPILATION_SUCCEEDED )
      {
#ifdef MCT_DEBUG
      fprintf(stderr, "Aborting compilation %p\n", comp);
#endif
      outOfMemory(comp, "code");
      }
   assume( !((warmCodeSize && !warmCode) || (coldCodeSize && !coldCode)), "Allocation failed but didn't call outOfMemory()");
   return warmCode;
   }

void
TR_J9VMBase::resizeCodeMemory(TR_Compilation * comp, U_8 *bufferStart, uint32_t numBytes)
   {
   // I don't see a reason to acquire VM access for this call
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   TR_MCCCodeCache * codeCache = (TR_MCCCodeCache *)comp->getCurrentCodeCache();
   codeCache->resizeCodeMemory(bufferStart, numBytes);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);

#ifndef TR_MEM_STATS
   if (debug("metaDataStats"))
#endif
      {
      metaDataStats._codeSize += numBytes;
      if (numBytes > metaDataStats._maxCodeSize) metaDataStats._maxCodeSize = numBytes;
      }
   }

bool
TR_J9VMBase::supportsCodeCacheSnippets()
   {
#if defined(TR_TARGET_X86) 
   return  (!TR_Options::getCmdLineOptions()->getOption(TR_DisableCodeCacheSnippets));
#else
   return false;
#endif
   }

uintptrj_t
TR_J9VMBase::getAllocationPrefetchCodeSnippetAddress(TR_Compilation * comp)
   {
#if defined(TR_TARGET_X86)
   TR_MCCCodeCache * codeCache = (TR_MCCCodeCache *)comp->getCurrentCodeCache();
   return codeCache->prefetchCodeSnippetAddress;
#else
   assert(0);
   return 0;
#endif
   }

void
TR_J9VMBase::setAllocationPrefetchCodeSnippetAddress(uintptrj_t codeAddress, TR_Compilation * comp)
   {
#if defined(TR_TARGET_X86)
   TR_MCCCodeCache * codeCache = (TR_MCCCodeCache *)comp->getCurrentCodeCache();
   codeCache->prefetchCodeSnippetAddress = codeAddress;
#else
   assert(0);
   return;
#endif
   }

uintptrj_t
TR_J9VMBase::getAllocationNoZeroPrefetchCodeSnippetAddress(TR_Compilation * comp)
   {
#if defined(TR_TARGET_X86)
   TR_MCCCodeCache * codeCache = (TR_MCCCodeCache *)comp->getCurrentCodeCache();
   return codeCache->noZeroPrefetchCodeSnippetAddress;
#else
   assert(0);
   return 0;
#endif
   }

void
TR_J9VMBase::setAllocationNoZeroPrefetchCodeSnippetAddress(uintptrj_t codeAddress, TR_Compilation * comp)
   {
#if defined(TR_TARGET_X86)
   TR_MCCCodeCache * codeCache = (TR_MCCCodeCache *)comp->getCurrentCodeCache();
   codeCache->noZeroPrefetchCodeSnippetAddress = codeAddress;
#else
   assert(0);
   return;
#endif
   }


bool
TR_J9VMBase::splitWarmAndColdBlocks()
   {
   TR_Target target = (TR_Target)jitConfig->targetName;
     if(is390(target))
        {
        return true;
        }
   else
      return false;
   }

U_8 *
TR_J9VMBase::getCodeCacheBase()
   {
   return _jitConfig->codeCache->heapBase;
   }

U_8 *
TR_J9VMBase::getCodeCacheBase(void * codeCache)
   {
   // Used by S390 canUseRelativeLongInstructions to get code cache base.
   return ((TR_MCCCodeCache *)codeCache)->segment->heapBase;
   }

U_8 *
TR_J9VMBase::getCodeCacheTop()
   {
   return _jitConfig->codeCache->heapTop;
   }

U_8 *
TR_J9VMBase::getCodeCacheTop(void * codeCache)
   {
   // Used by S390 canUseRelativeLongInstructions to get code cache top.
   return ((TR_MCCCodeCache *)codeCache)->segment->heapTop;
   }

// This routine may be called on the compilation thread or from a runtime hook
//
void
TR_J9VMBase::releaseCodeMemory(void *startPC, uint8_t bytesToSaveAtStart)
   {
   static char *disableCCR = feGetEnv("TR_DisableCCR");
   if (!disableCCR)
      {
      bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
      J9JavaVM            *vm        = jitConfig->javaVM;
      J9VMThread          *vmContext = vm->internalVMFunctions->currentVMThread(vm);
      J9JITExceptionTable *metaData  = jitConfig->jitGetExceptionTableFromPC(vmContext, (UDATA)startPC);
      TR_MCCManager::addFaintCacheBlock(jitConfig, metaData, bytesToSaveAtStart);
      releaseVMAccessIfNeeded(haveAcquiredVMAccess);
      }
   }

bool
TR_J9VMBase::vmRequiresSelector(uint32_t mask)
   {
   return true;
   }

bool
TR_J9VMBase::classObjectsMayBeCollected()
   {
   return false;
   }

bool
TR_J9VMBase::romClassObjectsMayBeCollected()
   {
   return false;
   }

bool
TR_J9VMBase::callTheJitsArrayCopyHelper()
   {
   return true;
   }

void*
TR_J9VMBase::getReferenceArrayCopyHelperAddress()
   {
   // Get the function descriptor of referenceArrayCopy ( a C routine in the GC module)
   // Each platform will have its own implementation to set up the call to referenceArrayCopy
   J9JavaVM * jvm = _jitConfig->javaVM;
   return (void*) jvm->memoryManagerFunctions->referenceArrayCopy;
   }

bool
TR_J9VMBase::isClassFinal(TR_OpaqueClassBlock * clazz)
   {
   return (getRomClass(clazz)->modifiers & J9AccFinal) ? true : false;
   }

bool
TR_J9VMBase::hasFinalFieldsInClass(TR_OpaqueClassBlock * clazz)
   {
   J9Class *clazzPtr = convertClassOffsetToClassPtr(clazz);
   return (clazzPtr->classDepthAndFlags & J9_JAVA_CLASS_HAS_FINAL_FIELDS)!=0;
   }

static uint32_t offsetOfHotFields() { return offsetof(J9Class, instanceHotFieldDescription); }

class TR_MarkHotField : public TR_SubclassVisitor
   {
public:
   TR_MarkHotField(TR_Compilation * comp, TR_SymbolReference * symRef)
      : TR_SubclassVisitor(comp->fe(), comp->trMemory()), _comp(comp), _symRef(symRef) { }

   void mark(J9Class *, bool);

   virtual bool visitSubclass(TR_PersistentClassInfo *);

private:

   bool markHotField(J9Class * clazz, bool baseClass);

   TR_Compilation *     _comp;
   TR_SymbolReference * _symRef;
   UDATA                _bitValue;
   UDATA                _slotIndex;
   };

void
TR_MarkHotField::mark(J9Class * clazz, bool isFixedClass)
   {
   if (_comp->fe()->isAOT())
      return;

   if ((*(UDATA *)((char *)clazz + offsetOfHotFields()) & 0x1))
      {
      // temorary hack: tenure aligned classes can't have
      // hot fields marked, we need another word for this
      if (_comp->getOption(TR_TraceMarkingOfHotFields))
         {
         J9ROMClass* romClass = ((J9Class *)clazz)->romClass;
         J9UTF8* name = J9ROMCLASS_CLASSNAME(romClass);
         printf("Rejected class %.*s for hot field marking because it's marked for tenured alignment\n", J9UTF8_LENGTH(name), J9UTF8_DATA(name));
         }
      return;
      }

   if (!_symRef->getSymbol()->getShadowSymbol() || _symRef->isUnresolved() || !clazz)
      return;

   if ((uintptrj_t)_symRef->getOffset() < _comp->fe()->getObjectHeaderSizeInBytes())
      return;

   _slotIndex = ((_symRef->getOffset() - _comp->fe()->getObjectHeaderSizeInBytes()) / _comp->fe()->getAddressFieldSize()) + 1; // +1 because low order bit means tenured alignment
   if (_slotIndex > 30) // not 31 because low order bit means tenured alignment
      return;

   _bitValue = (UDATA)1 << _slotIndex;

   if (!markHotField(clazz, true))
      return;

   if (!isFixedClass)
      {
      setTracing(_comp->getOption(TR_TraceMarkingOfHotFields));
      visit(((TR_J9VMBase*)_fe)->convertClassPtrToClassOffset(clazz));
      }
   }

bool
TR_MarkHotField::visitSubclass(TR_PersistentClassInfo * subclassInfo)
   {
   return markHotField(((TR_J9VMBase*)_fe)->convertClassOffsetToClassPtr(subclassInfo->getClassId()), false);
   }

bool
TR_MarkHotField::markHotField(J9Class * clazz, bool rootClass)
   {
   if (_comp->fe()->isAOT())
      return false;

   // If the bit is already marked in the class then we don't need to walk this classes subclasses.
   // Returning false indicates to the visitor to not walk this classes subclasses.
   //
   UDATA noncoldWord= *(UDATA *)((char *)clazz + offsetOfHotFields());
   if (noncoldWord & _bitValue)
      return false;

   UDATA * descriptorPtr = clazz->instanceDescription;
   UDATA descriptorWord;
   if (((UDATA) descriptorPtr) & BCT_J9DescriptionImmediate)
      descriptorWord = ((UDATA) descriptorPtr) >> 1;
   else
      descriptorWord = descriptorPtr[0];

   // Check that the field is a member of the class.  At the time this code was written there were cases
   // when value propation would call this function with the class being Object and the field being String.value
   //
   if (!(descriptorWord & _bitValue))
      return false;

   if (_comp->getOption(TR_TraceMarkingOfHotFields))
      {
      if (rootClass)
         {
         int32_t len; char * s = _symRef->getOwningMethod(_comp)->fieldName(_symRef->getCPIndex(), len, _comp->trMemory());
         printf("hot field %*s with bitValue=%x and slotIndex=%d found while compiling \n   %s\n", len, s, _bitValue, _slotIndex, _comp->signature());
         }

      J9ROMClass* romClass = ((J9Class *)clazz)->romClass;
      J9UTF8* name = J9ROMCLASS_CLASSNAME(romClass);
      printf("%*smarked field as hot in class %.*s\n", depth(), " ", J9UTF8_LENGTH(name), J9UTF8_DATA(name));
      }

   *(UDATA *)((char *)clazz + offsetOfHotFields()) = noncoldWord | _bitValue;

   return true;
   }

void
TR_J9VMBase::markHotField(TR_Compilation * comp, TR_SymbolReference * symRef, TR_OpaqueClassBlock * clazz, bool isFixedClass)
   {
   TR_MarkHotField marker(comp, symRef);
   marker.mark(convertClassOffsetToClassPtr(clazz), isFixedClass);
   }



bool
TR_J9VMBase::scanReferenceSlotsInClassForOffset(TR_Compilation * comp, TR_OpaqueClassBlock * classPointer, int32_t offset)
   {
   if (isAOT())
      return false;
   TR_VMFieldsInfo fields(comp, convertClassOffsetToClassPtr(classPointer), 1);

   if (!fields.getFields()) return false;

   ListIterator <TR_VMField> iter(fields.getFields());
   for (TR_VMField * field = iter.getFirst(); field != NULL; field = iter.getNext())
      {
      if (field->offset > offset)
         return false;

      if (field->isReference())
         {
         char *fieldSignature = field->signature;
         char *fieldName = field->name;

         int32_t fieldOffset = getInstanceFieldOffset(classPointer, fieldName, (uint32_t)strlen(fieldName), fieldSignature, (uint32_t)strlen(fieldSignature));
         if (fieldOffset == offset)
            {
            J9Class *fieldClass = convertClassOffsetToClassPtr(getClassFromSignature(fieldSignature, (int32_t)strlen(fieldSignature), comp->getCurrentMethod()));

            if (fieldClass != NULL)
               {
               UDATA hotWordValue = *(UDATA *)((char *)fieldClass + offsetOfHotFields());

               if (hotWordValue & 0x1) return true;
               }
            }
         }
      }

   return false;
   }

int32_t
TR_J9VMBase::findFirstHotFieldTenuredClassOffset(TR_Compilation *comp, TR_OpaqueClassBlock *opclazz)
   {
   if (!isAOT())
      {
      J9Class *clazz = convertClassOffsetToClassPtr(opclazz);
      UDATA hotFieldsWordValue = *(UDATA *)((char *)clazz + offsetOfHotFields());

      if (!hotFieldsWordValue)
         return -1;

      if (hotFieldsWordValue & 0x1)
         {
         // this class is marked for tenured alignment ignore it
         return -1;
         }

      for (int i = 1; i<31; i++)
         {
         uint32_t flag = (uint32_t)(hotFieldsWordValue & ((UDATA)1<<i));

         // if the field is marked find the
         // class type of this field and see if that class
         // is marked for tenured alignment
         if (flag)
            {
            uint32_t offset = (i-1) * getAddressFieldSize();
            if (scanReferenceSlotsInClassForOffset(comp, opclazz, offset))
               return (int32_t)(offset + getObjectHeaderSizeInBytes());
            }
         }
      }

   return -1;
   }



#if defined(TR_TARGET_X86)
#define CACHE_LINE_SIZE 64
#elif defined(TR_HOST_POWER)
#define CACHE_LINE_SIZE 128
#else
#define CACHE_LINE_SIZE 256
#endif

#define DEFAULT_OBJECT_ALIGNMENT (8)

void
TR_J9VMBase::markClassForTenuredAlignment(TR_Compilation *comp, TR_OpaqueClassBlock *opclazz, uint32_t alignFromStart)
   {
   if (!isAOT())
      {
      J9Class *clazz = convertClassOffsetToClassPtr(opclazz);
      UDATA hotFieldsWordValue = 0x1; // mark for alignment

      assume((alignFromStart % DEFAULT_OBJECT_ALIGNMENT)==0, "alignment undershot should be multiple of 8 bytes");
      assume((alignFromStart < 128), "alignment undershot should be less than 128 (124 max)");

      hotFieldsWordValue |= (((alignFromStart & 0x7f)/DEFAULT_OBJECT_ALIGNMENT) << 1);

      //printf("Class %p, hotFieldsWordValue %p\n", opclazz,  hotFieldsWordValue);

      *(UDATA *)((char *)clazz + offsetOfHotFields()) = hotFieldsWordValue;
      }
   }

char *
TR_J9VMBase::getClassSignature(TR_OpaqueClassBlock * clazz, int32_t & length, TR_Memory * trMemory)
   {
   int32_t   numDims = 0;

   TR_OpaqueClassBlock * myClass = getBaseComponentClass(clazz, numDims);

   int32_t len;
   char * name = getClassNameChars(myClass, len);
   length = len + numDims;
   if (* name != '[')
      length += 2;

   char * sig = (char *)trMemory->allocateStackMemory(length);
   int32_t i;
   for (i = 0; i < numDims; i++)
      sig[i] = '[';
   if (* name != '[')
      sig[i++] = 'L';
   memcpy(sig+i, name, len);
   i += len;
   if (* name != '[')
      sig[i++] = ';';
   return sig;
   }


int32_t
TR_J9VMBase::printTruncatedSignature(char *sigBuf, int32_t bufLen, TR_OpaqueMethodBlock *method)
   {
   // avoid using sampleSignature due to malloc
   J9Method *j9method = (J9Method *)method;
   J9UTF8 * className;
   J9UTF8 * name;
   J9UTF8 * signature;
   getClassNameSignatureFromMethod(j9method, className, name, signature);
   int32_t sigLen = J9UTF8_LENGTH(className) + J9UTF8_LENGTH(name) + J9UTF8_LENGTH(signature)+2;
   if (sigLen < bufLen)
      {
      sigLen = sprintf(sigBuf, "%.*s.%.*s%.*s", J9UTF8_LENGTH(className), utf8Data(className),
                       J9UTF8_LENGTH(name), utf8Data(name),
                       J9UTF8_LENGTH(signature), utf8Data(signature));
      }
   else // truncate some parts of the signature
      {
      if (sigLen - bufLen < J9UTF8_LENGTH(signature)) // classname and methodname can fit
         {
         sigLen = sprintf(sigBuf, "%.*s.%.*s%.*s", J9UTF8_LENGTH(className), utf8Data(className),
                          J9UTF8_LENGTH(name), utf8Data(name),
                          (J9UTF8_LENGTH(signature) - (sigLen-bufLen)), utf8Data(signature));
         }
      else
         {
         int32_t nameLen = MIN(bufLen-3,J9UTF8_LENGTH(name));
         if (nameLen == bufLen-3) // not even the method name can be printed entireley
            sigLen = sprintf(sigBuf, "*.%.*s", nameLen, utf8Data(name));
         else
            sigLen = sprintf(sigBuf, "%.*s.%.*s", MIN(bufLen-2 - nameLen, J9UTF8_LENGTH(className)), utf8Data(className), nameLen, utf8Data(name));
         }
      }
   return sigLen;
   }


int32_t *
TR_J9VMBase::getReferenceSlotsInClass(TR_Compilation * comp, TR_OpaqueClassBlock * classPointer)
   {
   // Get the offsets of all the reference slots in the given class as a
   // zero-terminated array of slot numbers. Return NULL if no reference slots
   // in the class.
   //
   TR_VMFieldsInfo fields(comp, convertClassOffsetToClassPtr(classPointer), 0);
   int32_t * slots = fields.getGCDescriptor();
   if (* slots == 0)
      return NULL;
   return (int32_t*)slots;
   }


int32_t *
TR_J9VMBase::getReferenceSlotsInClassForPackedHeaderRef(TR_Compilation * comp, TR_OpaqueClassBlock * classPointer)
   {
   // Get the offsets of all the reference slots in the given class as a
   // zero-terminated array of slot numbers. Return NULL if no reference slots
   // in the class.
   //
   // null terminated
   int32_t * slots = (int32_t *) comp->trMemory()->allocateHeapMemory((2)*sizeof(int32_t));
   slots[1] = 0;
   int32_t numBytesInSlot = getAddressFieldSize();
   int32_t refSlotOffset = packedDataPointerOffset();
   slots[0] = refSlotOffset/numBytesInSlot;
   return (int32_t*)slots;
   }


bool
TR_J9VMBase::shouldPerformEDO(TR_Block *catchBlock, TR_Compilation * comp)
   {
   assume2(catchBlock->isCatchBlock(), "shouldPerformEDO expected block_%d to be a catch block", catchBlock->getNumber());

   if (isJavaArchetypeSpecimenRequest(comp->ilGenRequest()))
      return false;
   if (comp->getOption(TR_DisableEDO))
      return false;
   if (catchBlock->isOSRCatchBlock()) // Can't currently induce recompilation from an OSR block
      return false;

   static char *disableEDORecomp = feGetEnv("TR_disableEDORecomp");
   if (disableEDORecomp)
      return false;

   TR_Recompilation * recomp = comp->getRecompilationInfo();
   if (  recomp
      && recomp->useSampling()
      && recomp->shouldBeCompiledAgain()
      && comp->getMethodHotness() < hot
      && comp->getNodeCount() < TR_Options::_catchSamplingSizeThreshold)
      {
      return true;
      }
   else
      {
      return false;
      }
   }

bool
TR_J9VMBase::isClassArray(TR_OpaqueClassBlock *klass)
   {
   return J9ROMCLASS_IS_ARRAY(getRomClass(klass)) ? true : false;
   }

char *
TR_J9VMBase::getClassNameChars(TR_OpaqueClassBlock * ramClass, int32_t & length)
   {
   return utf8Data(J9ROMCLASS_CLASSNAME(getRomClass(ramClass)), length);
   }

bool
TR_J9VMBase::isInlineableNative(TR_Compilation * comp, TR_ResolvedMethodSymbol * methodSymbol)
   {
   if (!comp->getOption(TR_DisableInliningOfNatives))
      switch(methodSymbol->getRecognizedMethod())
         {
         //case java_lang_System_identityHashCode:
         case com_ibm_oti_vm_VM_callerClass:
            // return true if these are INLs.  If they've become JNIs as a result of project clear work
            // then we don't know how to handle them.   Specifically identityHashCode is a static JNI
            // so that calls to them get an inserted first argument.  The inliner and other code has
            // trouble matching up parameters to arguments in this case.
            //
            return !methodSymbol->isJNI();
         }

   return false;
   }


TR_Node *
TR_J9VMBase::generateArrayElementShiftAmount(TR_Compilation * comp, TR_Node *object)
   {
   TR_Node                 * shiftAmount;
   TR_SymbolReferenceTable * symRefTab = comp->getSymRefTab();

   shiftAmount = TR_Node::create(comp, TR_iaload,1,object,symRefTab->findOrCreateVftSymbolRef());
   shiftAmount = TR_Node::create(comp, TR_iaload,1,shiftAmount,symRefTab->findOrCreateArrayClassRomPtrSymbolRef());
   shiftAmount = TR_Node::create(comp, TR_iiload,1,shiftAmount,symRefTab->findOrCreateIndexableSizeSymbolRef());
   return shiftAmount;
   }



bool
TR_J9VMBase::populateCharCacheEntry(
   int32_t         charCacheCursor,
   uint8_t        *profiledData,
   int32_t         profiledLengthInUTF16Chars,
   TR_Compilation *comp)
   {
   TR_OpaqueClassBlock *stringClass = comp->getStringClassPointer();
   uintptrj_t *charCacheArrayStaticField = (uintptrj_t *)getStaticFieldAddress(
      stringClass, (unsigned char *)"charCache", 9, (unsigned char *)"[[C", 3);

   // TODO: Check if the popular value is already in the cache.
   //
   uintptrj_t charCacheArrayObject = *((uintptrj_t *)charCacheArrayStaticField);

   if (charCacheArrayObject == 0)
      {
      trace0c(comp, " char cache object is 0\n");
      // The String class has not finished running the clinit yet.
      //
      return false;
      }

   uint32_t headerSize;
   if (canGenerateArraylets() && useHybridArraylets() &&
      isDiscontiguousArray(profiledLengthInUTF16Chars*sizeof(int16_t)))
      {
      headerSize = sizeof(J9IndexableObjectDiscontiguous);
      }
   else
      {
      headerSize = sizeof(J9IndexableObjectContiguous);
      }

   uint32_t charCacheEntryOffset = headerSize + (charCacheCursor * getAddressFieldSize());

   uintptrj_t charCacheEntryElement = charCacheArrayObject + charCacheEntryOffset;
   fj9object_t charCacheEntryBase = *((fj9object_t *)charCacheEntryElement);
#if defined(J9VM_GC_COMPRESSED_POINTERS)
   int32_t result = getCompressedPointersShift();
   uintptrj_t chars = (((uintptrj_t) charCacheEntryBase) << result);
#else
   uintptrj_t chars = (uintptrj_t) charCacheEntryBase;
#endif

   // Populate the cache
   //
   uint8_t *charCacheEntryData = (uint8_t *)((uintptrj_t) chars + (uintptrj_t) headerSize);
   for (int32_t i=0; i<profiledLengthInUTF16Chars*2; i++)
      {
      //printf("populating entry %d value %d\n", i, profiledData[i]);  fflush(stdout);
      charCacheEntryData[i] = profiledData[i];
      }

   return true;
   }



static TR_ILOpCodes udataIndirectLoadOpCode(TR_Compilation * comp)
   {
   if (comp->cg()->is64BitTarget())
      {
      return(TR_ilload);
      }
   else
      {
      return(TR_iiload);
      }
   }

static TR_ILOpCodes udataIndirectStoreOpCode(TR_Compilation * comp)
   {
   if (comp->cg()->is64BitTarget())
      {
      return(TR_ilstore);
      }
   else
      {
      return(TR_iistore);
      }
   }

static TR_ILOpCodes udataConstOpCode(TR_Compilation * comp)
   {
   if (comp->cg()->is64BitTarget())
      {
      return(TR_lconst);
      }
   else
      {
      return(TR_iconst);
      }
   }

static TR_ILOpCodes udataLoadOpCode(TR_Compilation * comp)
   {
   if (comp->cg()->is64BitTarget())
      {
      return(TR_lload);
      }
   else
      {
      return(TR_iload);
      }
   }

static TR_ILOpCodes udataCmpEqOpCode(TR_Compilation * comp)
   {
   if (comp->cg()->is64BitTarget())
      {
      return(TR_lcmpeq);
      }
   else
      {
      return(TR_icmpeq);
      }
   }

TR_TreeTop *
TR_J9VMBase::lowerAsyncCheck(TR_Compilation * comp, TR_Node * root, TR_TreeTop * treeTop)
   {
   // Generate the inline test as a child of the asynccheck node
   //
   TR_SymbolReference * stackOverflowSymRef =
      new (comp->trHeapMemory()) TR_SymbolReference(comp->getSymRefTab(), new (comp->trHeapMemory()) TR_MethodMetaDataSymbol("stackOverflowMark"));

   stackOverflowSymRef->setOffset(offsetof(J9VMThread, stackOverflowMark));

   TR_Node * loadNode  = TR_Node::create(comp, root, udataLoadOpCode(comp), 0, stackOverflowSymRef);
   TR_Node * constNode = TR_Node::create(comp, root, udataConstOpCode(comp), 0, -1);
   if (comp->cg()->is64BitTarget())
      {
      constNode->setLongInt(-1L);
      }
   root->setAndIncChild(0, TR_Node::create(comp, udataCmpEqOpCode(comp), 2, loadNode, constNode));

   // Insert the address of the helper as a symref into the asynccheck node
   root->setSymbolReference(comp->getSymRefTab()->findOrCreateAsyncCheckSymbolRef());
   root->setNumChildren(1);

   return treeTop;
   }


// { RTSJ Support begins
TR_TreeTop *
TR_J9VMBase::lowerAtcCheck(TR_Compilation * comp, TR_Node * root, TR_TreeTop * treeTop)
   {
   assume(comp->getOptions()->atcChecksEnabled(), "about to lower supposedly disabled atccheck");

   // Generate a load for the pendingAIE as first child of the atccheck node
   //
   TR_SymbolReference * pendingAIESymRef = comp->getSymRefTab()->findOrCreatePendingAIESymbolRef();

   TR_Node * pendingAIELoadNode  = TR_Node::create(comp, root, TR_aload, 0, pendingAIESymRef);

   if (comp->getOptions()->realTimeExtensions() && comp->getOptions()->nhrtChecksEnabled())
      {
      TR_SymbolReference *noHeapRealTimeCheckSymRef = comp->getSymRefTab()->findOrCreateNoHeapRealTimeCheckSymbolRef();
      TR_Node *nhrttCheckNode = TR_Node::create(comp, root, TR_NHRTTCHK, 0, noHeapRealTimeCheckSymRef);
      nhrttCheckNode->setAndIncChild(0, pendingAIELoadNode);
      nhrttCheckNode->setNumChildren(1);
      TR_TreeTop *nhrttCheckTT = TR_TreeTop::create(comp, nhrttCheckNode);
      treeTop->getPrevTreeTop()->insertAfter(nhrttCheckTT);
      }

   if (comp->getOptions()->realTimeGC() && comp->getOptions()->readBarriersEnabled())
      pendingAIELoadNode = TR_Node::create(comp, TR_readbar, 1, pendingAIELoadNode, comp->getSymRefTab()->findOrCreateGCForwardingPointerSymbolRef());

   root->setAndIncChild(0, pendingAIELoadNode);

   root->setNumChildren(1);

   return treeTop;
   }

bool
TR_J9VMBase::isMethodEnterTracingEnabled(TR_OpaqueMethodBlock *method)
   {
   J9JavaVM * javaVM = _jitConfig->javaVM;
   J9HookInterface * * vmHooks = javaVM->internalVMFunctions->getVMHookInterface(javaVM);

   return jitMethodEnterTracingEnabled(getCurrentVMThread(), (J9Method *)method);
   }

bool
TR_J9VMBase::isMethodExitTracingEnabled(TR_OpaqueMethodBlock *method)
   {
   J9JavaVM * javaVM = _jitConfig->javaVM;
   J9HookInterface * * vmHooks = javaVM->internalVMFunctions->getVMHookInterface(javaVM);

   return jitMethodExitTracingEnabled(getCurrentVMThread(), (J9Method *)method);
   }

bool
TR_J9VMBase::canMethodEnterEventBeHooked()
   {
   J9JavaVM * javaVM = _jitConfig->javaVM;
   J9HookInterface * * vmHooks = javaVM->internalVMFunctions->getVMHookInterface(javaVM);

   return ((*vmHooks)->J9HookDisable(vmHooks, J9HOOK_VM_METHOD_ENTER) != 0);
   }

bool
TR_J9VMBase::canMethodExitEventBeHooked()
   {
   J9JavaVM * javaVM = _jitConfig->javaVM;
   J9HookInterface * * vmHooks = javaVM->internalVMFunctions->getVMHookInterface(javaVM);

   return ((*vmHooks)->J9HookDisable(vmHooks, J9HOOK_VM_METHOD_RETURN) != 0);
   }

bool
TR_J9VMBase::methodsCanBeInlinedEvenIfEventHooksEnabled()
   {
   J9JavaVM *javaVM = _jitConfig->javaVM;
   return ((javaVM->requiredDebugAttributes & J9VM_DEBUG_ATTRIBUTE_ALLOW_INLINING_WITH_METHOD_ENTER_EXIT) != 0);
   }

TR_TreeTop *
TR_J9VMBase::lowerMethodHook(TR_Compilation * comp, TR_Node * root, TR_TreeTop * treeTop)
   {
   J9Method * j9method = (J9Method *) root->getOwningMethod(comp);
   TR_Node * ramMethod = TR_Node::createPointerNode(comp, root, TR_aconst, j9method);
   ramMethod->setIsMethodPointerConstant(true, comp);

   int32_t event = root->getOpCodeValue() == TR_MethodEnterHook ? J9HOOK_VM_METHOD_ENTER : J9HOOK_VM_METHOD_RETURN;
   bool isTrace = (event == J9HOOK_VM_METHOD_ENTER) ? isMethodEnterTracingEnabled(j9method) : isMethodExitTracingEnabled(j9method);

   TR_Node * methodCall;
   if (root->getNumChildren() == 0)
      methodCall = TR_Node::create(comp, TR_vcall, 1, ramMethod, root->getSymbolReference());
   else
      {
      TR_Node * child = root->getChild(0);
      if (comp->fe()->isAOT() || !isTrace && comp->cg()->getSupportsPartialInlineOfMethodHooks())
         child = child->duplicateTree(comp);

      methodCall = TR_Node::create(comp, TR_vcall, 2, child, ramMethod, root->getSymbolReference());
      root->getChild(0)->recursivelyDecReferenceCount();
      }

   if (comp->fe()->isAOT())
      {
      // Add an ifcmpne branch for AOT
      TR_TreeTop *ifTree = TR_TreeTop::create(comp, TR_Node::create(comp, TR_treetop, 1, methodCall));

      TR_Node *cmp = comp->createAOTGuard(comp, methodCall->getInlinedSiteIndex(), root, 0, TR_MethodEnterExitGuard);
      TR_TreeTop *cmpTree = TR_TreeTop::create(comp, cmp);

      root->setNumChildren(0);

      TR_Block* followOnBlock = NULL;
      TR_Block *enclosingBlock = treeTop->getEnclosingBlock();
      TR_Block *remainderBlock = enclosingBlock->createConditionalBlocksBeforeTree(comp, treeTop, cmpTree, ifTree, 0, comp->getFlowGraph());

      TR_Block *ifBlock = ifTree->getEnclosingBlock();

      bool enableDupTree = comp->getOption(TR_EnableDupRetTree);
      if ((root->getOpCodeValue() == TR_MethodExitHook) && enableDupTree)
         {
         // Remove the goto block
         comp->removeTree(ifBlock->getLastRealTreeTop());

         // remove the edge
         comp->getFlowGraph()->removeEdge(ifBlock, remainderBlock);

         // to create dup tree
         TR_TreeTop *dupTree = remainderBlock->getFirstRealTreeTop()->duplicateTree(comp);

         // append the duplicated tree
         ifBlock->append(dupTree);

         TR_TreeTop *exitTree = remainderBlock->getLastRealTreeTop();
         TR_TreeTop *curTree = remainderBlock->getFirstRealTreeTop();
         TR_TreeTop *nextTree = curTree->getNextRealTreeTop();
         while (curTree != exitTree)
            {
            curTree = nextTree;
            ifBlock->append(curTree->duplicateTree(comp));
            nextTree = curTree->getNextRealTreeTop();
            }

         // mark return block as extension of prev block
         //remainderBlock->setIsExtensionOfPreviousBlock(true);

         // do I need to add edge from ifBlock to out?  what is the Out Block?
         comp->getFlowGraph()->addEdge(ifBlock, comp->getFlowGraph()->getEnd());
         }

      if (methodCall->getNumChildren() != 0)
         {
         //enclosingBlock->getNextBlock()->setIsExtensionOfPreviousBlock();

         TR_Node *child = methodCall->getChild(0);
         if (child->getOpCodeValue() == TR_aRegLoad)
            {
            TR_Node *ifNode = ifTree->getNode();
            ifNode->setNumChildren(3);
            TR_Node *glRegDeps = enclosingBlock->getEntry()->getNode()->getChild(0);

            TR_Node *duplicateGlRegDeps = glRegDeps->duplicateTree(comp);
            TR_Node *originalDuplicateGlRegDeps = duplicateGlRegDeps;
            duplicateGlRegDeps = TR_Node::copy(glRegDeps, comp);
            ifNode->setChild(2, duplicateGlRegDeps);

            for (int32_t i = glRegDeps->getNumChildren() - 1; i >= 0; --i)
               {
               TR_Node * dep = glRegDeps->getChild(i);
               duplicateGlRegDeps->setAndIncChild(i, dep);
               if (dep->getGlobalRegisterNumber() == child->getGlobalRegisterNumber())
                  originalDuplicateGlRegDeps->setAndIncChild(i, child);
               }

            TR_Block *callTreeBlock = ifTree->getEnclosingBlock();
            TR_Node *bbstartNode = callTreeBlock->getEntry()->getNode();
            bbstartNode->setNumChildren(1);
            bbstartNode->setChild(0, originalDuplicateGlRegDeps);
            }
         }

      return cmpTree;
      }
   else if (!isTrace && comp->cg()->getSupportsPartialInlineOfMethodHooks())
      {
      // The method enter and exit hooks must be modified to check to see if the event is hooked
      // in the new interface rather than the old. This is a simple bit test at a known address.
      // The JIT should check the status of the J9HOOK_FLAG_HOOKED bit in the hook interface,
      // rather than the vmThread->eventFlags field.
      //
      // create
      // iand
      //    bu2i
      //      buload &vmThread()->javaVM->hookInterface->flags[J9HOOK_VM_METHOD_ENTER/J9HOOK_VM_METHOD_RETURN];
      //    iconst J9HOOK_FLAG_HOOKED
      //
      TR_StaticSymbol * addressSym = new (comp->trHeapMemory()) TR_StaticSymbol(TR_Address);
      addressSym->setStaticAddress(&vmThread()->javaVM->hookInterface.flags[event]);

      TR_TreeTop * hookedTest =  TR_TreeTop::create(comp,
         TR_Node::createif(comp, TR_ificmpne,
            TR_Node::create(comp, TR_iand, 2,
               TR_Node::create(comp, TR_bu2i, 1,
                  TR_Node::create(comp, root, TR_buload, 0, new (comp->trHeapMemory()) TR_SymbolReference(comp->getSymRefTab(), addressSym))),
               TR_Node::create(comp, root, TR_iconst, 0, J9HOOK_FLAG_HOOKED)),
            TR_Node::create(comp, root, TR_iconst, 0, 0)));

      TR_TreeTop *result = hookedTest;

      TR_TreeTop *callTree = TR_TreeTop::create(comp, TR_Node::create(comp, TR_treetop, 1, methodCall));

      root->setNumChildren(0);

      TR_Block *enclosingBlock = treeTop->getEnclosingBlock();
      if (comp->getOption(TR_EnableSelectiveEnterExitHooks))
         {
         // Mainline test is whether this method has been selected for entry/exit hooks

         TR_StaticSymbol * extendedFlagsSym = new (comp->trHeapMemory()) TR_StaticSymbol(TR_Address);
         extendedFlagsSym->setStaticAddress(fetchMethodExtendedFlagsPointer(j9method));

         TR_TreeTop * selectedTest = TR_TreeTop::create(comp,
            TR_Node::createif(comp, TR_ificmpne,
               TR_Node::create(comp, TR_bu2i, 1,
                  TR_Node::create(comp, root, TR_buload, 0, new (comp->trHeapMemory()) TR_SymbolReference(comp->getSymRefTab(), extendedFlagsSym))),
               TR_Node::create(comp, root, TR_iconst, 0, 0)));

         result = selectedTest;

         enclosingBlock->createConditionalBlocksBeforeTree(comp, treeTop, selectedTest, callTree, 0, comp->getFlowGraph());

         // Test of whether the hook is enabled should instead branch back to the mainline code if it's NOT enabled
         //
         TR_Block *callBlock    = callTree->getEnclosingBlock();
         TR_Block *restartBlock = selectedTest->getEnclosingBlock()->getNextBlock();
         hookedTest->getNode()->setOpCodeValue(reverseBranchOpCodes[hookedTest->getNode()->getOpCodeValue()]);
         hookedTest->getNode()->setBranchDestination(restartBlock->getEntry());
         callTree->insertBefore(hookedTest);
         callBlock->split(callTree, comp->getFlowGraph());
         comp->getFlowGraph()->addEdge(callBlock, enclosingBlock->getNextBlock());
         }
      else
         {
         enclosingBlock->createConditionalBlocksBeforeTree(comp, treeTop, hookedTest, callTree, 0, comp->getFlowGraph());
         }

      if (methodCall->getNumChildren() != 0)
         {
         //enclosingBlock->getNextBlock()->setIsExtensionOfPreviousBlock();

         TR_Node *child = methodCall->getChild(0);
         if (child->getOpCodeValue() == TR_aRegLoad)
            {
            TR_Node *ifNode = hookedTest->getNode();
            ifNode->setNumChildren(3);
            TR_Node *glRegDeps = enclosingBlock->getEntry()->getNode()->getChild(0);

            TR_Node *duplicateGlRegDeps = glRegDeps->duplicateTree(comp);
            TR_Node *originalDuplicateGlRegDeps = duplicateGlRegDeps;
            duplicateGlRegDeps = TR_Node::copy(glRegDeps, comp);
            ifNode->setChild(2, duplicateGlRegDeps);

            for (int32_t i = glRegDeps->getNumChildren() - 1; i >= 0; --i)
               {
               TR_Node * dep = glRegDeps->getChild(i);
               duplicateGlRegDeps->setAndIncChild(i, dep);
               if (dep->getGlobalRegisterNumber() == child->getGlobalRegisterNumber())
                  originalDuplicateGlRegDeps->setAndIncChild(i, child);
               }

            TR_Block *callTreeBlock = callTree->getEnclosingBlock();
            TR_Node *bbstartNode = callTreeBlock->getEntry()->getNode();
            bbstartNode->setNumChildren(1);
            bbstartNode->setChild(0, originalDuplicateGlRegDeps);
            }
         }

      return result;
      }

   // replace mainline Hook node with straight call to the helper

   treeTop->setNode(methodCall);

   return treeTop;
   }

static void lowerContiguousArrayLength(TR_Compilation *comp, TR_Node *root)
   {
   // Array size occupies 4 bytes in ALL header shapes.
   //
   TR_SymbolReferenceTable *symRefTab = comp->getSymRefTab();
   root->setOpCodeValue(TR_iiload);
   root->setSymbolReference(symRefTab->findOrCreateContiguousArraySizeSymbolRef());
   }

static void lowerDiscontiguousArrayLength(TR_Compilation *comp, TR_Node *root)
   {
   // Array size occupies 4 bytes in ALL header shapes.
   //
   TR_SymbolReferenceTable *symRefTab = comp->getSymRefTab();
   root->setOpCodeValue(TR_iiload);
   root->setSymbolReference(symRefTab->findOrCreateDiscontiguousArraySizeSymbolRef());
   }

TR_TreeTop *
TR_J9VMBase::lowerArrayLength(TR_Compilation *comp, TR_Node *root, TR_TreeTop *treeTop)
   {
   // True hybrid arraylet arraylengths must provide their own evaluator because
   // they may involve control flow or instructions that can't be represented in
   // trees.
   //
   if (!useHybridArraylets())
      {
      lowerContiguousArrayLength(comp, root);
      }

   return treeTop;
   }


TR_TreeTop *
TR_J9VMBase::lowerContigArrayLength(TR_Compilation *comp, TR_Node *root, TR_TreeTop *treeTop)
   {
   lowerContiguousArrayLength(comp, root);
   return treeTop;
   }


TR_TreeTop *
TR_J9VMBase::lowerMultiANewArray(TR_Compilation * comp, TR_Node * root, TR_TreeTop * treeTop)
   {
   // Get the number of dimensions
   //
   int32_t dims;
   if (root->getFirstChild()->getOpCode().isLoadConst())
      {
      dims = root->getFirstChild()->getInt();
      } //check is the const is in the literal pool
   else if ((root->getFirstChild()->getSymbolReference()!=NULL) && root->getFirstChild()->getSymbolReference()->isLiteralPoolAddress())
      {
      dims = ((TR_Node *) (root->getFirstChild()->getSymbolReference()->getOffset()))->getInt();
      }
   else assume(0,"Number of dims in multianewarray is not constant");


   // Allocate a temp to hold the array of dimensions
   //
   TR_AutomaticSymbol * temp = new (comp->trHeapMemory()) TR_AutomaticSymbol(TR_Int32, sizeof(int32_t)*dims, comp);
   comp->getMethodSymbol()->addAutomatic(temp);

   // Generate stores of each dimension into the array of dimensions
   // The last dimension is stored first in the array
   //
   int32_t offset = 0;
   for (int32_t i = dims; i > 0; i--)
          {
          TR_SymbolReference * symRef = new (comp->trHeapMemory()) TR_SymbolReference(comp->getSymRefTab(), temp, offset);
          TR_TreeTop::create(comp, treeTop->getPrevTreeTop(),
               TR_Node::create(comp, TR_istore, 1, root->getChild(i),
                               symRef));
          //symRef->setStackAllocatedArrayAccess();
          root->getChild(i)->decReferenceCount();
          offset += sizeof(int32_t);
          }

   // Change the node into a call to the helper with the arguments
   //    1) Pointer to the array of dimensions
   //    2) Number of dimensions
   //    3) Class object for the element
   //
   root->setChild(2,root->getChild(dims+1));
   root->setChild(1,root->getChild(0));
   TR_Node * tempRef = TR_Node::create(comp, root, TR_loadaddr,0,new (comp->trHeapMemory()) TR_SymbolReference(comp->getSymRefTab(), temp));
   root->setAndIncChild(0,tempRef);
   root->setNumChildren(3);
   root->setOpCodeValue(TR_acall);

   return treeTop;
   }

TR_TreeTop *
TR_J9VMBase::lowerToVcall(TR_Compilation * comp, TR_Node * root, TR_TreeTop * treeTop)
   {
   // Change the TR_checkcast node into a call to the jitCheckCast helper
   //
   root->setOpCodeValue(TR_vcall);
   return treeTop;
   }

TR_TreeTop *
TR_J9VMBase::lowerTree(TR_Compilation * comp, TR_Node * root, TR_TreeTop * treeTop)
   {
   switch(root->getOpCodeValue())
      {
      case TR_asynccheck:           return lowerAsyncCheck(comp, root, treeTop);
      case TR_arraylength:          return lowerArrayLength(comp, root, treeTop);
      case TR_contigarraylength:    return lowerContigArrayLength(comp, root, treeTop);
      case TR_discontigarraylength: { lowerDiscontiguousArrayLength(comp, root); return treeTop; }
      case TR_multianewarray:       return lowerMultiANewArray(comp, root, treeTop);
      case TR_athrow:               return lowerToVcall(comp, root, treeTop);
      case TR_MethodEnterHook:      return lowerMethodHook(comp, root, treeTop);
      case TR_MethodExitHook:       return lowerMethodHook(comp, root, treeTop);
      case TR_atccheck:             return lowerAtcCheck(comp, root, treeTop);
      }
   return treeTop;
   }

// Call the following method either with VM access or with classUnloadMonitor in hand
// Call this method only on the frontend that is attached to the calling thread
uint8_t
TR_J9VMBase::getCompilationShouldBeInterruptedFlag()
   {
   // Some assumes to make sure that the code hasn't changed in an incompatible way
   TR_CompilationInfoPerThreadBase *cp = _compInfo->getCompInfoForCompOnAppThread();
   if (cp)
      {
      return cp->compilationShouldBeInterrupted();
      }
   else
      {
#ifdef DEBUG // make sure that what is true today stays true in the future
      J9VMThread *vmThread = _jitConfig->javaVM->internalVMFunctions->currentVMThread(_jitConfig->javaVM);
      TR_CompilationInfoPerThread *compInfoPT = _compInfo->getCompInfoForThread(vmThread);
      assume3(compInfoPT == _compInfoPT, "Discrepancy compInfoPT=%p _compInfoPT=%p\n", compInfoPT, _compInfoPT);
#endif
      return _compInfoPT->compilationShouldBeInterrupted();
      }
   }

bool
TR_J9VMBase::compilationShouldBeInterrupted(TR_Compilation * comp, TR_CallingContext callingContext)
   {
//#ifdef COMP_YIELD_ANALYSIS
   if (comp->getUpdateCompYieldStats())
      comp->updateCompYieldStatistics(callingContext);

   TR_CompilationInfoPerThreadBase * compInfoPT = (TR_CompilationInfoPerThreadBase *)comp->trMemory()->getCompilationInfoPerThread();
   TR_CompilationInfo * compInfo = compInfoPT->getCompilationInfo();
   // The following assume is valid only when compiling on a separate thread
   assume(compInfoPT == _compInfoPT || compInfo->getCompInfoForCompOnAppThread(), "compilationShouldBeInterrupted: mismatch between compInfoPT and _compInfoPT\n");


   if (comp->getOption(TR_EnableYieldVMAccess) &&
       comp->getOption(TR_DisableNoVMAccess) &&
       checkForExclusiveAcquireAccessRequest(comp))
      {
      releaseVMAccess(vmThread());

      if (!compInfo->getCompInfoForCompOnAppThread() && TR_Options::getCmdLineOptions()->realTimeGC()) // no compilation on application thread
         ((TR_CompilationInfoPerThread*)compInfoPT)->waitForGCCycleMonitor(false);

      acquireVMAccessNoSuspend(vmThread());
      }

   if (compInfoPT->compilationShouldBeInterrupted())
      return true;

   if (!comp->getOption(TR_DisableNoVMAccess))
      {
      bool exitClassUnloadMonitor = persistentMemory(_jitConfig)->getPersistentInfo()->GCwillBlockOnClassUnloadMonitor();
      if (TR_Options::getCmdLineOptions()->realTimeGC())
         {
#if defined (J9VM_GC_REALTIME)
         J9JavaVM *vm = _jitConfig->javaVM;
         exitClassUnloadMonitor = exitClassUnloadMonitor || vm->gcCycleOn;
#endif
         }
      if (exitClassUnloadMonitor)
         {
         // release the classUnloadMonitor and then re-aquire it. This will give GC a chance to cut in.
         persistentMemory(_jitConfig)->getPersistentInfo()->resetGCwillBlockOnClassUnloadMonitor();

#if defined(J9VM_GC_DYNAMIC_CLASS_UNLOADING)
         bool hadClassUnloadMonitor = TR_J9MonitorTable::get()->readReleaseClassUnloadMonitor(compInfoPT->getCompThreadId()) >= 0;
         assume(hadClassUnloadMonitor, "Comp thread must hold classUnloadMonitor when compiling without VMaccess");
#else // Class unloading is not possible
         bool hadClassUnloadMonitor = false;
         if (TR_Options::getCmdLineOptions()->getOption(TR_EnableHCR) || TR_Options::getCmdLineOptions()->getOption(TR_FullSpeedDebug))
            {
            hadClassUnloadMonitor = TR_J9MonitorTable::get()->readReleaseClassUnloadMonitor(compInfoPT->getCompThreadId()) >= 0;
            assume(hadClassUnloadMonitor, "Comp thread must hold classUnloadMonitor when compiling without VMaccess");
            }
#endif
         //--- GC CAN INTERVENE HERE ---
         assume((vmThread()->publicFlags & J9_PUBLIC_FLAGS_VM_ACCESS) == 0, "comp thread must not have vm access");
         if (!compInfo->getCompInfoForCompOnAppThread() && TR_Options::getCmdLineOptions()->realTimeGC()) // no compilation on application thread
            ((TR_CompilationInfoPerThread*)compInfoPT)->waitForGCCycleMonitor(false);

         TR_J9MonitorTable::get()->readAcquireClassUnloadMonitor(compInfoPT->getCompThreadId());

         if (compInfoPT->compilationShouldBeInterrupted())
            {
            //fprintf(stderr, "Interrupting compilation of %s because of global flag 2\n", comp->signature());
            return true;
            }
         }
      }

   return false;
   }


bool
TR_J9VMBase::checkForExclusiveAcquireAccessRequest(TR_Compilation *)
   {
   //if (debug("yieldVMAccess"))
   if (vmThread()->publicFlags & J9_PUBLIC_FLAGS_HALT_THREAD_EXCLUSIVE)
      return true;

   return false;
   }


bool
TR_J9VMBase::haveAccess()
   {
   //if (debug("noVMAccess"))
   if (vmThread()->publicFlags &  J9_PUBLIC_FLAGS_VM_ACCESS)
      return true;
   return false;
   }

bool
TR_J9VMBase::haveAccess(TR_Compilation * comp)
   {
   //if (debug("noVMAccess"))
   if (!comp->getOption(TR_DisableNoVMAccess))
      {
      if (vmThread()->publicFlags &  J9_PUBLIC_FLAGS_VM_ACCESS)
         return true;
      return false;
      }
   return true;
   }

void
TR_J9VMBase::acquireAccess(TR_Compilation * comp)
   {
   //if (debug("noVMAccess"))
   if (!comp->getOption(TR_DisableNoVMAccess))
      {
      if (!(vmThread()->publicFlags &  J9_PUBLIC_FLAGS_VM_ACCESS))
         acquireVMAccessNoSuspend(vmThread());
      }
   }

void
TR_J9VMBase::releaseAccess(TR_Compilation * comp)
   {
   //if (debug("noVMAccess"))
   if (!comp->getOption(TR_DisableNoVMAccess))
      {
      if (vmThread()->publicFlags &  J9_PUBLIC_FLAGS_VM_ACCESS)
         releaseVMAccess(vmThread());
      }
   }

bool
TR_J9VMBase::tryToAcquireAccess(TR_Compilation * comp, bool *haveAcquiredVMAccess)
   {
   //if (debug("noVMAccess"))
   bool hasVMAccess;
   *haveAcquiredVMAccess = false;
   if (!comp->getOption(TR_DisableNoVMAccess))
      {
      if (!(vmThread()->publicFlags &  J9_PUBLIC_FLAGS_VM_ACCESS))
         {
         if (!vmThread()->javaVM->internalVMFunctions->internalTryAcquireVMAccessWithMask(vmThread(), J9_PUBLIC_FLAGS_HALT_THREAD_ANY_NO_JAVA_SUSPEND))
            {
            hasVMAccess = true;
            *haveAcquiredVMAccess = true;
            }
         else
            {
            hasVMAccess = false;
            }
         }
      else
         {
         hasVMAccess = true;
         }
      }
   else
      {
      hasVMAccess = true;
      }

   return hasVMAccess;
   }


// The following is called from TR_Options::jitLatePostProcess
bool
TR_J9VMBase::compileMethods(TR_OptionSet *optionSet, void *config)
   {
   J9JITConfig *jitConfig = (J9JITConfig*)config;
   assume(optionSet != NULL && jitConfig != NULL, "Invalid optionSet or jitConfig passed in");

   TR_DebugBase * debug = NULL;
   if (!(debug = TR_Options::getDebug()))
      {
      TR_Options::createDebug();
      debug = TR_Options::getDebug();
      if (!debug)
         {
         return false;
         }
      }

   PORT_ACCESS_FROM_JAVAVM(jitConfig->javaVM);
   J9JavaVM *javaVM = jitConfig->javaVM;
   //TR_FrontEnd * vm = TR_J9VMBase::get(jitConfig, 0);
   TR_CompilationInfo * compInfo = getCompilationInfo(jitConfig);
   J9InternalVMFunctions * intFunc = javaVM->internalVMFunctions;
   J9VMThread *vmThread = intFunc->currentVMThread(javaVM);

   J9Method * newInstanceThunk = NULL;

   int   maxMethodNameLen = 2048;
   char* fullMethodName   = (char*)j9mem_allocate_memory(maxMethodNameLen, J9MEM_CATEGORY_JIT);
   if (!fullMethodName)
      {
      return false;
      }

   TR_SimpleRegex * regex = optionSet->getMethodRegex();

   compInfo->debugPrint(vmThread, "\tcompile methods entering monitor before compile\n");
   compInfo->getCompilationMonitor()->enter();
   compInfo->debugPrint(vmThread, "+CM\n");

   J9ClassWalkState classWalkState;
   J9Class * clazz = javaVM->internalVMFunctions->allLiveClassesStartDo(&classWalkState, javaVM, NULL);
   while (clazz)
      {
      if (!J9ROMCLASS_IS_PRIMITIVE_OR_ARRAY(clazz->romClass))
         {
         if (newInstanceThunk == NULL)
            {
            // this is done inside the loop so that we can survive an early call
            // to this routine before the new instance prototype has been established
            // (and there are no classes loaded) from cmd line processing
            newInstanceThunk = getNewInstancePrototype(vmThread);
            }

         J9ROMMethod * romMethod = (J9ROMMethod *) J9ROMCLASS_ROMMETHODS(clazz->romClass);
         J9Method * ramMethods = (J9Method *) (clazz->ramMethods);
         for (uint32_t m = 0; m < clazz->romClass->romMethodCount; m++)
            {
            J9Method * method = &ramMethods[m];

            if (!(romMethod->modifiers & (J9_JAVA_NATIVE | J9_JAVA_ABSTRACT))
                 && method != newInstanceThunk &&
                 !TR_CompilationInfo::isCompiled(method))
               {
               J9UTF8 *className;
               J9UTF8 *name;
               J9UTF8 *signature;
               getClassNameSignatureFromMethod(method, className, name, signature);
               if (J9UTF8_LENGTH(className) + J9UTF8_LENGTH(name) + J9UTF8_LENGTH(signature) + 1 > maxMethodNameLen)
                  {
                  maxMethodNameLen = J9UTF8_LENGTH(className) + J9UTF8_LENGTH(name) + J9UTF8_LENGTH(signature) + 1;
                  j9mem_free_memory(fullMethodName);
                  fullMethodName   = (char*)j9mem_allocate_memory(maxMethodNameLen, J9MEM_CATEGORY_JIT);
                  if (!fullMethodName)
                     {
                     break;
                     }
                  }

               sprintf(fullMethodName, "%.*s.%.*s%.*s",
                  J9UTF8_LENGTH(className), J9UTF8_DATA(className),
                  J9UTF8_LENGTH(name), J9UTF8_DATA(name),
                  J9UTF8_LENGTH(signature), J9UTF8_DATA(signature));

               if (TR_Debug::matchRegex(regex, fullMethodName))
                  {
                  bool queued = false;
                  TR_MethodEvent event;
                  event._eventType = TR_MethodEvent::InterpreterCounterTripped;
                  event._j9method = method;
                  event._oldStartPC = 0;
                  event._vmThread = vmThread;
                  event._classNeedingThunk = 0;
                  bool newPlanCreated;
                  TR_OptimizationPlan *plan = TR_CompilationController::getCompilationStrategy()->processEvent(&event, &newPlanCreated);
                  // If the controller decides to compile this method, trigger the compilation and wait here
                  if (plan)
                     {
                     TR_ILGenRequestStorage storage;
                     TR_ILGenRequest *request = new (storage) TR_JavaOrdinaryMethodRequest((TR_OpaqueMethodBlock*)method);
                     IDATA result = (IDATA)compInfo->compileMethod(vmThread, request, 0, TR_yes, NULL, &queued, plan);
                     if (!queued && newPlanCreated)
                        TR_OptimizationPlan::freeOptimizationPlan(plan);
                     }
                  else // OOM
                     {
                     break; // OOM, so abort all other compilations
                     }
                  }
               }

            romMethod = nextROMMethod(romMethod);
            }
         }
      clazz = javaVM->internalVMFunctions->allLiveClassesNextDo(&classWalkState);
      }

   javaVM->internalVMFunctions->allLiveClassesEndDo(&classWalkState);

   compInfo->debugPrint(vmThread, "\tcompile methods releasing monitor before compile\n");
   compInfo->debugPrint(vmThread, "-CM\n");
   compInfo->getCompilationMonitor()->exit();

   if (fullMethodName)
      {
      j9mem_free_memory(fullMethodName);
      }

   return true;
   }

// The following is called from TR_Options::jitLatePostProcess
void
TR_J9VMBase::waitOnCompiler(void *config)
   {
   J9JITConfig *jitConfig = (J9JITConfig*)config;
   // There is nothing to wait for if the compilations are not performed asynchronous on separate thread
   if (!isAsyncCompilation())
      return;
   if (!_compInfo)
      return;
   if (_compInfo->getNumCompThreadsActive() == 0)
      return;
  J9JavaVM   *vm        = jitConfig->javaVM;
  J9VMThread *vmContext = vm->internalVMFunctions->currentVMThread(vm);
   releaseVMAccess(vmContext);  // Release VM Access before going to sleep
   _compInfo->acquireCompilationLock();
   while (_compInfo->peekNextMethodToBeCompiled())
      _compInfo->getCompilationMonitor()->wait();
   _compInfo->releaseCompilationLock();
   acquireVMAccess(vmContext); // Reacquire VM access. This is a java thread so it can get suspended
   }

bool
TR_J9VMBase::tossingCode()
   {
   if ((_jitConfig->runtimeFlags & J9JIT_TOSS_CODE) ||
       (_jitConfig->runtimeFlags & J9JIT_TESTMODE))
      return true;
   return false;
   }

TR_Node *
TR_J9VMBase::transformStringIndexOfCall(TR_Compilation * comp, TR_Node * callNode)
   {
   assume(callNode->getNumChildren() == 2, "unexpected numChildren on String.indexOf(String) call");
   TR_ResolvedMethodSymbol * resolvedMethodSymbol =
      callNode->getSymbolReference()->getSymbol()->castToResolvedMethodSymbol();

   TR_Node * needle = callNode->getChild(1);

   if (needle->getOpCodeValue() != TR_aload)
      return callNode;
   if (!needle->getSymbolReference()->getSymbol()->isConstObjectRef())
      return callNode;
   if (needle->getSymbolReference()->isUnresolved())
      return callNode;

   bool haveAcquiredVMAccess;
   if (!tryToAcquireAccess(comp, &haveAcquiredVMAccess))
      return callNode;

   uintptrj_t string = *(uintptrj_t*)needle->getSymbol()->castToStaticSymbol()->getStaticAddress();
   int32_t len = getStringLength(string);
   if (len < 3)
      {
      if (haveAcquiredVMAccess)
         releaseAccess(comp);
      return callNode;
      }

   TR_OpaqueClassBlock * stringClass = comp->getStringClassPointer();
   TR_ScratchList<TR_ResolvedMethod> stringMethods(comp->trMemory());
   getResolvedMethods(comp->trMemory(), stringClass, &stringMethods);
   ListIterator<TR_ResolvedMethod> it(&stringMethods);
   TR_ResolvedMethod * method;
   for (method = it.getCurrent(); method; method = it.getNext())
      {
      char name[] = "indexOf";
      char sig[]  = "(Ljava/lang/String;Ljava/lang/String;IIC)I";
      if (!strncmp(method->nameChars(), name, strlen(name)) &&
          !strncmp(method->signatureChars(), sig, strlen(sig)))
         break;
      }

   if (!method)
      {
      if (haveAcquiredVMAccess)
         releaseAccess(comp);
      return callNode; // fast String.indexOf not supported by the class libraries
      }

   // HACK: if this is first time we are doing this transformation, change the invocation
   // count of the method - otherwise the optimizer will think that no one is calling this
   // method is that this method is cold
   //
   intptrj_t count = getInvocationCount((TR_OpaqueMethodBlock *) method->getPersistentIdentifier());
   if (count == comp->getOptions()->getCmdLineOptions()->getInitialBCount())
      setInvocationCount((TR_OpaqueMethodBlock *) method->getPersistentIdentifier(), count, count / 2);

   // Pattern Matched !

   TR_SymbolReferenceTable * symRefTab = comp->getSymRefTab();
   TR_SymbolReference * newSymRef =
      symRefTab->findOrCreateMethodSymbol(callNode->getSymbolReference()->getOwningMethodIndex(), -1, method, TR_MethodSymbol::Static);

   uint16_t lastChar = getStringCharacter(string, len-1);

   int32_t  cache    = 0;
   int32_t  md2      = len-1;

   int32_t  i;
   for (i = len-1; i >= 0; --i)
      {
      uint16_t thisChar = getStringCharacter(string, i);
      cache |= (1 << (thisChar & 31));
      }

   for (i = len-2; i >= 0; --i)
      {
      uint16_t thisChar = getStringCharacter(string, i);
      if (lastChar == thisChar)
         {
         md2 -= i;
         break;
         }
      }

   TR_Node * haystack = callNode->getChild(0);
   haystack->decReferenceCount();
   needle->decReferenceCount();

   TR_Node * node = TR_Node::create(comp, TR_icall, 5, newSymRef);
   node->setAndIncChild(0, haystack);
   node->setAndIncChild(1, needle);
   node->setAndIncChild(2, TR_Node::create(comp, TR_iconst, 0, cache));
   node->setAndIncChild(3, TR_Node::create(comp, TR_iconst, 0, md2));
   node->setAndIncChild(4, TR_Node::create(comp, TR_iconst, 0, lastChar));

   node->incReferenceCount();

   if (haveAcquiredVMAccess)
      releaseAccess(comp);
   return node;
   }

//
// A few predicates describing shadow symbols that we can reason about at
// compile time.  Note that "final field" here doesn't rule out a pointer to a
// Java object, as long as it always points at the same object.
//
// {{{
//

static bool isFinalFieldOfNativeStruct(TR_SymbolReference *symRef, TR_Compilation *comp)
   {
   switch (symRef->getReferenceNumber() - comp->getSymRefTab()->getNumHelperSymbols())
      {
      case TR_SymbolReferenceTable::isArraySymbol:
      case TR_SymbolReferenceTable::classRomPtrSymbol:
      case TR_SymbolReferenceTable::ramStaticsFromClassSymbol:
      case TR_SymbolReferenceTable::javaLangClassFromClassSymbol:
         assume(symRef->getSymbol()->isShadow(), "isFinalFieldOfNativeStruct expected shadow symbol");
         return true;
      default:
         return false;
      }
   }

static bool isFinalFieldPointingAtNativeStruct(TR_SymbolReference *symRef, TR_Compilation *comp)
   {
   switch (symRef->getReferenceNumber() - comp->getSymRefTab()->getNumHelperSymbols())
      {
      case TR_SymbolReferenceTable::classRomPtrSymbol:
      case TR_SymbolReferenceTable::classFromJavaLangClassSymbol:
      case TR_SymbolReferenceTable::classFromJavaLangClassAsPrimitiveSymbol:
      case TR_SymbolReferenceTable::ramStaticsFromClassSymbol:
      case TR_SymbolReferenceTable::vftSymbol:
         assume(symRef->getSymbol()->isShadow(), "isFinalFieldPointingAtNativeStruct expected shadow symbol");
         return true;
      default:
         return false;
      }
   }

static bool isFinalFieldPointingAtRepresentableNativeStruct(TR_SymbolReference *symRef, TR_Compilation *comp)
   {
   // A "representable native struct" can be turned into a node that is not a field load,
   // such as a const or a loadaddr.  Most native structs are not "representable" because
   // we don't have infrastructure for them such as AOT relocations.
   //
   switch (symRef->getReferenceNumber() - comp->getSymRefTab()->getNumHelperSymbols())
      {
      case TR_SymbolReferenceTable::classFromJavaLangClassSymbol:
      case TR_SymbolReferenceTable::classFromJavaLangClassAsPrimitiveSymbol:
         // Note: We could also do vftSymbol, except replacing those with
         // loadaddres mucks up indirect loads in ways the optimizer/codegen
         // isn't expecting yet
         assume(symRef->getSymbol()->isShadow(), "isFinalFieldPointingAtRepresentableNativeStruct expected shadow symbol");
         return true;
      default:
         return false;
      }
   }

static bool isFinalFieldPointingAtJ9Class(TR_SymbolReference *symRef, TR_Compilation *comp)
   {
   switch (symRef->getReferenceNumber() - comp->getSymRefTab()->getNumHelperSymbols())
      {
      case TR_SymbolReferenceTable::classFromJavaLangClassSymbol:
      case TR_SymbolReferenceTable::classFromJavaLangClassAsPrimitiveSymbol:
      case TR_SymbolReferenceTable::vftSymbol:
         assume(symRef->getSymbol()->isShadow(), "isFinalFieldPointingAtJ9Class expected shadow symbol");
         return true;
      default:
         return false;
      }
   }

static bool isFinalFieldPointingAtUnrepresentableNativeStruct(TR_SymbolReference *symRef, TR_Compilation *comp)
   {
   return isFinalFieldPointingAtNativeStruct(symRef, comp) && !isFinalFieldPointingAtRepresentableNativeStruct(symRef, comp);
   }

static bool isJavaObjectReferenceFieldOfJavaObject(TR_SymbolReference *symRef, TR_Compilation *comp)
   {
   TR_Symbol *symbol = symRef->getSymbol();
   return symbol->isShadow() && symRef->getCPIndex() >= 0 && symbol->isCollectedReference();
   }

static bool isJavaField(TR_SymbolReference *symRef, TR_Compilation *comp)
   {
   TR_Symbol *symbol = symRef->getSymbol();
   if (symbol->isShadow() && symRef->getCPIndex() >= 0)
      {
      switch (symbol->getDataType())
         {
         case TR_Int8:  case TR_Int16: case TR_Int32: case TR_Int64:
         case TR_Float: case TR_Double:
            return true;
         case TR_Address:
            return symbol->isCollectedReference();
         default:
            return false;
         }
      }

   return false;
   }

static bool isFieldOfJavaObject(TR_SymbolReference *symRef, TR_Compilation *comp)
   {
   TR_Symbol *symbol = symRef->getSymbol();
   if (isJavaField(symRef, comp))
      return true;
   else if (symbol->isShadow()) switch (symRef->getReferenceNumber() - comp->getSymRefTab()->getNumHelperSymbols())
      {
      case TR_SymbolReferenceTable::classFromJavaLangClassSymbol:
      case TR_SymbolReferenceTable::classFromJavaLangClassAsPrimitiveSymbol:
      case TR_SymbolReferenceTable::vftSymbol:
         return true;
      default:
         return false;
      }

   return false;
   }

// }}}  (end of predicates)

static bool foldFinalFieldsIn(char *className, int32_t classNameLength, TR_Compilation *comp)
   {
   TR_SimpleRegex *classRegex = comp->getOptions()->getClassesWithFoldableFinalFields();
   if (classRegex)
      return TR_Debug::matchRegex(classRegex, className);
   else if (classNameLength >= 17 && !strncmp(className, "java/lang/invoke/", 17))
      return true; // We can ONLY do this opt to fields that are never victimized by setAccessible
   else
      return false;
   }

bool
TR_J9VMBase::transformDirectLoad(TR_Compilation * comp, TR_Node *node)
   {
   assume3c(node->getOpCode().isLoadVarDirect(), comp, "Expecting direct load; found %s %p", node->getOpCode().getName(), node);
   TR_SymbolReference *symRef = node->getSymbolReference();
   TR_Symbol          *sym    = node->getSymbol();

   // Check for loads of static final primitive fields on classes that have finished initializing.
   //
   if (!symRef->isUnresolved()
      && !isAOT() // Static initializer can produce different values in different runs
      && sym->isStaticField()
      && sym->isFinal())
      {
      J9Class *fieldClass = (J9Class*)symRef->getOwningMethod(comp)->getClassFromFieldOrStatic(comp, symRef->getCPIndex());
      if (!fieldClass)
         return false;

      bool classInitIsFinished = (fieldClass->initializeStatus & J9ClassInitStatusMask) == J9ClassInitSucceeded;
      if (!classInitIsFinished)
         return false;

      int32_t len;
      char * name = getClassNameChars((TR_OpaqueClassBlock*)fieldClass, len);
      if (!strncmp(name, "java/lang/System", len))
         return false; // Keep our hands off out/in/err, which are declared final but aren't really

      if (!foldFinalFieldsIn(name, len, comp))
         return false;

      // Note that the load type can differ from the symbol type, eg. Java uses
      // integer loads for the sub-integer types.  The sub-integer types are
      // included below just for completeness, but we likely never hit them.
      //
      TR_DataTypes loadType = node->getOpCode().getDataType();
      bool typeIsConstible = false;
      bool symrefIsImprovable  = false;
      switch (loadType)
         {
         case TR_Int8:
         case TR_Int16:
         case TR_Int32:
         case TR_Int64:
         case TR_Float:
         case TR_Double:
            typeIsConstible = true;
            break;
         case TR_Address:
            symrefIsImprovable = !symRef->hasKnownObjectIndex();
            break;
         }

      TR_StaticSymbol *staticSym = sym->castToStaticSymbol();
      if (typeIsConstible)
         {
         if (performTransformation3c(comp, "O^O transformDirectLoad: turn [%p] %s %s into load const\n", node, node->getOpCode().getName(), symRef->getName(comp->getDebug())))
            {
            bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
            switch (loadType)
               {
               case TR_Int8:
                  node->setOpCodeValue(TR_bconst);
                  node->setByte(*(int8_t*)staticSym->getStaticAddress());
                  break;
               case TR_Int16:
                  node->setOpCodeValue(TR_sconst);
                  node->setShortInt(*(int16_t*)staticSym->getStaticAddress());
                  break;
               case TR_Int32:
                  node->setOpCodeValue(TR_iconst);
                  node->setInt(*(int32_t*)staticSym->getStaticAddress());
                  break;
               case TR_Int64:
                  node->setOpCodeValue(TR_lconst);
                  node->setLongInt(*(int64_t*)staticSym->getStaticAddress());
                  break;
               case TR_Float:
                  node->setOpCodeValue(TR_fconst);
                  node->setFloat(*(float*)staticSym->getStaticAddress());
                  break;
               case TR_Double:
                  node->setOpCodeValue(TR_dconst);
                  node->setDouble(*(double*)staticSym->getStaticAddress());
                  break;
               default:
                  assume2(0, "Unexpected type %d", loadType);
                  break;
               }
            releaseVMAccessIfNeeded(haveAcquiredVMAccess);
            }
         return true;
         }
      else if (*(uintptrj_t*)staticSym->getStaticAddress() == 0) // Seems ok just to check for a static to be NULL without vm access
         {
         if (performTransformation1c(comp, "O^O transformDirectLoad: [%p] field is null - change to aconst NULL\n", node))
            {
            node->setOpCodeValue(TR_aconst);
            node->setPointer(NULL);
            node->setIsNull   (true,  comp);
            node->setIsNonNull(false, comp);
            return true;
            }
         }
      else if (symrefIsImprovable)
         {
         uintptrj_t *refLocation = (uintptrj_t*)staticSym->getStaticAddress();
         TR_SymbolReference *improvedSymRef = comp->getSymRefTab()->findOrCreateSymRefWithKnownObject(node->getSymbolReference(), refLocation);
         if (improvedSymRef->hasKnownObjectIndex()
            && performTransformation5c(comp, "O^O transformDirectLoad: [%p] use object-specific symref #%d (=obj%d) for %s %s\n",
               node,
               improvedSymRef->getReferenceNumber(),
               improvedSymRef->getKnownObjectIndex(),
               node->getOpCode().getName(),
               symRef->getName(comp->getDebug())))
            {
            node->setSymbolReference(improvedSymRef);
            bool isNull = comp->getKnownObjectTable()->isNull(improvedSymRef->getKnownObjectIndex());
            node->setIsNull    (isNull,  comp);
            node->setIsNonNull (!isNull, comp);
            return true;
            }
         }
      }

   return false; // Indicates we did nothing
   }

static bool verifyFieldAccess(void *curStruct, TR_SymbolReference *field, TR_Compilation *comp)
   {
   if (!curStruct)
      return false;

   if (isJavaField(field, comp))
      {
      // For Java fields, a "verifiable" access is one where we can
      // check whether curStruct is an object of the right type.  If we can't
      // even check that, then we shouldn't be in this function in the first place.

      TR_OpaqueClassBlock *objectClass = comp->fe()->getObjectClass((uintptrj_t)curStruct);
      TR_OpaqueClassBlock *fieldClass  = field->getOwningMethod(comp)->getClassFromFieldOrStatic(comp, field->getCPIndex());
      //
      // TODO: getDeclaringClassFromFieldOrStatic would be better, but it's a
      // little scarier because it's new.  getClassFromFieldOrStatic should be
      // more conservative in this case, so probably more suitable for
      // r13.java.

      TR_YesNoMaybe objectContainsField = comp->fe()->isInstanceOf(objectClass, fieldClass, true);
      return objectContainsField == TR_yes;
      }
   else if (isFieldOfJavaObject(field, comp))
      {
      // For special shadows representing data in Java objects, we need to verify the Java object types.
      TR_OpaqueClassBlock *objectClass = comp->fe()->getObjectClass((uintptrj_t)curStruct);
      switch (field->getReferenceNumber() - comp->getSymRefTab()->getNumHelperSymbols())
         {
         case TR_SymbolReferenceTable::vftSymbol:
            return true; // Every java object has a vft pointer
         case TR_SymbolReferenceTable::classFromJavaLangClassSymbol:
         case TR_SymbolReferenceTable::classFromJavaLangClassAsPrimitiveSymbol:
            return objectClass == comp->fe()->getClassClassPointer(objectClass);
         default:
            assume0("Cannot verify unknown field of java object");
            return false;
         }
      }
   else if (isFinalFieldOfNativeStruct(field, comp))
      {
      // These are implicitly verified by virtue of being verifiable
      //
      return true;
      }
   else
      {
      // Don't know how to verify this
      //
      return false;
      }

   return true;
   }

static void *dereferenceStructPointerChain(void *baseStruct, TR_Node *baseNode, TR_Node *curNode, TR_Compilation *comp)
   {
   // IMPORTANT NOTE on "interpreting" code at compile time:
   //
   // Sometimes we're looking at dead code on impossible paths.  In that case,
   // the node symbol info might not match the actual type of the data it is
   // manipulating.
   //
   // To make this facility easier to use, we have decided to tolerate certain
   // kinds of mismatches between the trees and the actual data being examined.
   // We return NULL to indicate we have been unable to perform the
   // dereference.  (Null is otherwise not a valid result of this function,
   // since the caller of this function is always intending to dereference the
   // result.)
   //
   // The onus is on the caller to make sure baseStruct/baseNode meets certain
   // rules (see below) but beyond that, this function must verify that the
   // actual data structure being walked matches the nodes closely enough to
   // prevent the jit from doing something wrong.
   //
   // This function must return NULL unless we can verify that (1) the
   // structure we're loading from is of the correct type, and (2) the pointer
   // we're returning is itself verifiable.
   //
   // In this narrow usage, "correct type" really means that subsequent
   // dereferences are guaranteed to "work properly" in the sense that they
   // won't crash the jit.  If the data does not match the trees, it is valid
   // to return nonsensical answers (garbage in, garbage out) but it is not
   // valid to crash.
   //
   // "Verifiable" in #2 means that we must be able to perform the verification
   // in #1.  What that means depends on the type of data we're looking at.
   // In the case of a Java object, those are self-describing.  It is enough
   // merely to know that a pointer is the address of a Java object in order to
   // declare it "verifiable", and the verification step can do the rest.  In
   // the case of a C struct, there is generally no way to verify it, so the
   // verification step for a C struct is, of necessity, trivial, and therefore
   // the "verifiable" criterion itself perform the entire verification.  (This
   // corresponds to the fact that Java can do dynamic type checking while C
   // does all type checking statically.)
   //
   // This leads us to the following situation:
   //
   //   For Java object references:
   //    - Verifiable if it is known to point at an object or null
   //    - Verified if it is non-null and points to an object that contains the field being loaded
   //   For C structs pointers:
   //    - Verifiable if it is known to point to a struct of the proper type or null
   //    - Verified trivially by virtue of being verifiable and non null
   //
   // The onus is on the caller to ensure that baseStruct/baseNode is
   // verifiable.  This is why we have separated the notion of "verifiable"
   // from "verified": there is less burden on the caller if it only needs to
   // ensure verifiability.
   //
   if (baseNode == curNode)
      {
      // End of recursion
      //
      return baseStruct;
      //
      // Since baseStruct/baseNode are deemed verifiable by the caller, result
      // is now verifiable a priori.
      }
   else
      {
      uint8_t *curStruct = (uint8_t*)dereferenceStructPointerChain(baseStruct, baseNode, curNode->getFirstChild(), comp);
      if (curStruct)
         {
         if (verifyFieldAccess(curStruct, curNode->getSymbolReference(), comp))
            {
            if (isJavaObjectReferenceFieldOfJavaObject(curNode->getSymbolReference(), comp))
               {
               uintptrj_t fieldAddress = (uintptrj_t)(curStruct + curNode->getSymbolReference()->getOffset());
               TR_J9VM *vm = (TR_J9VM*)(comp->fe());
               return (void*)vm->getReferenceFieldAtAddress(fieldAddress);
               }
            else
               {
               uint8_t **fieldAddress = (uint8_t**)(curStruct + curNode->getSymbolReference()->getOffset());
               return *fieldAddress;
               }
            }
         else
            {
            return NULL;
            }
         }
      else
         {
         return NULL;
         }
      }

   assume(0, "Should never get here");
   return NULL;
   }

TR_Node *
TR_J9VMBase::transformIndirectLoad(TR_Compilation * comp, TR_Node *node)
   {
   // TODO: PATDOYLE
   // This code has lots of does lots of refcount decrementing, which is not
   // safe if we don't anchor the child trees first!
   //
   static char *enableTransformIndirectLoad = feGetEnv("TR_enableTransformIndirectLoad");
   if (!enableTransformIndirectLoad)
      return NULL;

   assume3c(node->getOpCode().isLoadIndirect(), comp, "Expecting indirect load; found %s %p", node->getOpCode().getName(), node);
   TR_SymbolReference *symRef = node->getSymbolReference();
   TR_Symbol          *sym    = node->getSymbol();

   if (!symRef->isUnresolved()
      && sym->isShadow())
      {
      TR_Node *baseObject = node->getFirstChild();

      // Start with a few field-specific goodies
      //
      switch (sym->getRecognizedField())
         {
         case TR_Symbol::Java_lang_invoke_MethodHandle_thunks:
         case TR_Symbol::Java_lang_invoke_DynamicInvokerHandle_site:
            if (!node->isNonNull() && performTransformation1c(comp, "O^O transformIndirectLoad: [%p] recognized field is never null\n", node))
               {
               node->setIsNull   (false, comp);
               node->setIsNonNull(true,  comp);
               }
            break;
         default:
            // Here, we can check a few cases where the "base object" is not
            // actually an object at all.
            //
            if (symRef == comp->getSymRefTab()->findJavaLangClassFromClassSymbolRef()
               && !symRef->hasKnownObjectIndex()
               && baseObject->getOpCodeValue() == TR_loadaddr
         && !baseObject->getSymbolReference()->isUnresolved())
               {
               bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
               uintptrj_t jlClass = (uintptrj_t)J9VM_J9CLASS_TO_HEAPCLASS((J9Class*)baseObject->getSymbol()->castToStaticSymbol()->getStaticAddress());
               assume(jlClass, "java/lang/Class reference from heap class must be non null");
               TR_SymbolReference *improvedSymRef = comp->getSymRefTab()->findOrCreateSymRefWithKnownObject(node->getSymbolReference(), &jlClass);
               releaseVMAccessIfNeeded(haveAcquiredVMAccess);
               if (  improvedSymRef->hasKnownObjectIndex()
                  && performTransformation3c(comp, "O^O transformIndirectLoad: [%p] use object-specific symref #%d (=obj%d) for load of java/lang/Class\n",
                     node, improvedSymRef->getReferenceNumber(), improvedSymRef->getKnownObjectIndex()))
                  {
                  node->setSymbolReference(improvedSymRef);
                  node->setIsNull   (false, comp);
                  node->setIsNonNull(true,  comp);
                  return node;
                  }
               }
            break;
         }

      // Check for loads of final primitive fields on objects that have finished initializing.
      //
      if (1) //sym->isFinal())
         {
         J9Class *fieldClass = (J9Class*)symRef->getOwningMethod(comp)->getClassFromFieldOrStatic(comp, symRef->getCPIndex());
         if (!fieldClass)
            return NULL;
    
         int32_t len;
         char * name = getClassNameChars((TR_OpaqueClassBlock*)fieldClass, len);
         if (!foldFinalFieldsIn(name, len, comp))
            return NULL;
    
         // Note that the load type can differ from the type of the symbol, eg.
         // for sub-integer types.  The sub-integer types are included below
         // just for completeness, but we likely never hit them.
         //
         TR_DataTypes loadType = node->getOpCode().getDataType();
         bool typeIsConstible    = false;
         bool symrefIsImprovable = false;
         switch (loadType)
            {
            case TR_Int32:
            case TR_Int64:
               typeIsConstible = true;
               break;
            case TR_Address:
               symrefIsImprovable = !symRef->hasKnownObjectIndex();
               break;
            }

         TR_Node *firstDereference = NULL;
         TR_Node *secondDereference = NULL;
         if (isFinalFieldOfNativeStruct(symRef, comp))
            {
            // Ok, this is a bit complicated.
            //
            // We have an expression y.z where y is a native struct and z is
            // some final field within that native struct.  Because y is a
            // child node of y.z we can assume it has already been simplified
            // because the optimizer does such things in a bottom-up fashion.
            // If y happens to be a so-called "representable" native struct
            // like a J9Class, then we're in luck because we can just read the
            // field out of y at compile time.
            //
            // However, we also want to handle situations where y is not
            // "representable", meaning it can only appear in the trees as a
            // field load from some other base object x.  In that case, we
            // must handle the whole expression x.y.z in one chunk, because
            // we can't simplify x.y nor y.z independently.
            //
            // Generalizing, we can end up with an arbitrarily long reference
            // chain a.b.c...y.z where b...y are final fields pointing at
            // unrepresentable native structs, and we must collapse this whole
            // chain in one chunk, or not at all.
            //
            // The following logic detects such chains, and finishes with
            // baseObject = a (the representable base expression), and
            // firstDereference = a.b and secondDereference = a.b.c (or NULL,
            // if the expression is just a.b and a is representable).
            // Note that our analysis starts from the "z" dereference, because
            // that is the root TR_Node in this expression tree, and works its
            // way down the tree to "a".

            if (typeIsConstible) // can't yet tell whether the expression chain results in a Java object, so just do primitives
               {
               while (baseObject->getOpCode().isLoadIndirect() && isFinalFieldPointingAtUnrepresentableNativeStruct(baseObject->getSymbolReference(), comp))
                  {
                  secondDereference = firstDereference;
                  firstDereference  = baseObject;
                  baseObject        = firstDereference->getFirstChild();
                  }

               // at this point, baseObject is some representable expression
               J9Class *clazz = NULL;
               if (baseObject->getOpCode().isLoadIndirect())
                  {
                  if (isFinalFieldPointingAtNativeStruct(baseObject->getSymbolReference(), comp))
                     {
                     assume(isFinalFieldPointingAtRepresentableNativeStruct(baseObject->getSymbolReference(), comp), "Base expression pointing at native struct must be representable"); // Good luck to the poor soul doing triage trying to figure out what this means

                     // Don't bother continuing.  Under normal circumstances, a
                     // representable base object expression should already
                     // have gone through this process and been simplified into
                     // a const or loadaddr.
                     dumpOptDetails2c(comp, "Could have transformed %p if representable baseObject %p had already been simplified\n", node, baseObject);
                     return NULL;
                     }
                  else
                     {
                     // baseObject is an expression resulting in a Java object reference.
                     // The logic below handles figuring out the address of the base Java
                     // object.  Proceed with that, then do the dereference below.
                     }
                  }
               else if (baseObject->chkClassPointerConstant())
                  {
                  clazz = (J9Class*)baseObject->getPointer();
                  }
               else if (baseObject->getOpCodeValue() == TR_loadaddr
                     && baseObject->getSymbol()->isClassObject())
                  {
                  clazz = (J9Class*)baseObject->getSymbol()->castToStaticSymbol()->getStaticAddress();
                  }
               else
                  {
                  // TODO: The above checks are too conservative.  There's no
                  // need to check for class pointers explicitly.  Once we know
                  // it's a representable object, we can do getPointer, getInt,
                  // getLongInt, or getStaticAddress depending on the opcode,
                  // and use whatever we get back from that to start dereferencing.
                  dumpOptDetails2c(comp, "Can't yet transform %p based on representable baseObject %p that isn't a J9Class\n", node, baseObject);
                  return NULL;
                  }

               if (clazz && performTransformation3c(comp, "O^O transformIndirectLoad: [%p] evaluate native structure walk into const from %s, based on class pointer node %p\n", node, node->getSymbolReference()->getName(comp->getDebug()), baseObject))
                  {
                  // We have clazz.a.b.c.d.primitiveField
                  // We can simplify this into a const.
                  //
                  uint8_t *structure = (uint8_t*)dereferenceStructPointerChain(clazz, baseObject, node->getFirstChild(), comp);
                  if (verifyFieldAccess(structure, node->getSymbolReference(), comp)) switch (loadType)
                     {
                     case TR_Int32:
                        node = TR_Node::iconst(comp, node, *(int32_t*)(structure + node->getSymbolReference()->getOffset()));
                        break;
                     case TR_Int64:
                        node = TR_Node::lconst(comp, node, *(int64_t*)(structure + node->getSymbolReference()->getOffset()));
                        break;
                     default:
                        assume2(0, "Unexpected native field type %d", node->getDataType());
                        break;
                     }
                  return node;
                  }
               }
            }

         uintptrj_t *baseObjectRefLocation = NULL;
         if (baseObject->getOpCode().hasSymbolReference() && baseObject->getSymbolReference()->hasKnownObjectIndex())
            {
            baseObjectRefLocation = baseObject->getSymbolReference()->getKnownObjectReferenceLocation(comp);
            }
         else if (baseObject->getOpCode().isLoadVarDirect() && baseObject->getSymbol()->isStatic())
            {
            TR_StaticSymbol *baseSym = baseObject->getSymbol()->castToStaticSymbol();
            if (!baseObject->getSymbolReference()->isUnresolved())
               {
               // Note: There is an implicit assumption here that if we can reach
               // a const object, then its constructor must have completed and its
               // final fields will never change again.  Since an object can't be
               // reached through the constant pool or call site table until its
               // constructor completes, there is no known case where this is false,
               // but if one ever arises, we'll need to check for that case here.
               //
               if (baseSym->isFixedObjectRef())
                  baseObjectRefLocation = (uintptrj_t*)baseSym->getStaticAddress();
               }
            }

         if (!baseObjectRefLocation)
            return NULL; // Nothing left we can do

         //
         // Now we know we have an indirect from an identifiable location.
         // We can use this information to simplify loads of final fields.
         //

         if (isFinalFieldOfNativeStruct(symRef, comp))
            {
            if (baseObjectRefLocation && typeIsConstible)
               {
               if (performTransformation3c(comp, "O^O transformIndirectLoad: [%p] evaluate native structure walk into const from %s, based on known object node %p\n", node, node->getSymbolReference()->getName(comp->getDebug()), baseObject))
                  {
                  // We have object.a.b.c.d.primitiveField, where a.b.c.d walks through some native data structure.
                  // We can simplify this into a const.
                  //
                  bool loadFailed = false;
                  uintptrj_t fieldAddress = (*baseObjectRefLocation) + firstDereference->getSymbolReference()->getOffset();
                  bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
                  // First dereference with VM access to dig a pointer out of an object
                  uint8_t *structure;
                  switch (firstDereference->getOpCode().getDataType())
                     {
                     case TR_Int32:
                        {
                        uint32_t *intAddress = (uint32_t*)fieldAddress;
                        structure = (uint8_t*)(uintptr_t)(*intAddress);
                        }
                        break;
                     case TR_Int64:
                        {
                        uint64_t *longAddress = (uint64_t*)fieldAddress;
                        structure = (uint8_t*)(uintptr_t)(*longAddress);
                        }
                        break;
                     case TR_Address:
                        {
                        uintptrj_t *refAddress = (uintptrj_t*)fieldAddress;
                        structure = (uint8_t*)(uintptr_t)(*refAddress);
                        }
                        break;
                     default:
                        loadFailed = true;
                        break;
                     }
                  releaseVMAccessIfNeeded(haveAcquiredVMAccess);
                  assume(!loadFailed, "Expected int, long, or address load");
                  assume2(secondDereference, "Expected expression %p like a.b...c where a is a Java object, b points at a native struct, and c is a field in a native struct", node);
                  if (secondDereference)
                     structure = (uint8_t*)dereferenceStructPointerChain(structure, firstDereference, node->getFirstChild(), comp);

                  if (verifyFieldAccess(structure, node->getSymbolReference(), comp)) switch (loadType)
                     {
                     case TR_Int32:
                        node = TR_Node::iconst(comp, node, *(int32_t*)(structure + node->getSymbolReference()->getOffset()));
                        break;
                     case TR_Int64:
                        node = TR_Node::lconst(comp, node, *(int64_t*)(structure + node->getSymbolReference()->getOffset()));
                        break;
                     default:
                        assume2(0, "Unexpected native field type %d", node->getDataType());
                        break;
                     }
                  return node;
                  }
               }
            }
         else if (isFinalFieldPointingAtJ9Class(symRef, comp)
               && isFinalFieldPointingAtRepresentableNativeStruct(symRef, comp)
               && !isAOT())
            {
            bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
            TR_OpaqueClassBlock *clazz = getClassFromJavaLangClass(*baseObjectRefLocation);
            releaseVMAccessIfNeeded(haveAcquiredVMAccess);
            TR_Node *clazzNode = TR_Node::create(comp, node, TR_loadaddr, 0,
               comp->getSymRefTab()->findOrCreateClassSymbol(comp->getMethodSymbol(), -1, clazz));
            if (performTransformation3c(comp, "O^O transformIndirectLoad: [%p] turn load of J9Class into %s %p\n", node, clazzNode->getOpCode().getName(), clazzNode))
               return clazzNode;
            }
         else if (sym->isFinal() && !isAOT()) // Constructor can set different values for the same field in different runs on AOT
            {
            int32_t fieldOffset = symRef->getOffset() - sizeof(J9Object); // blah
            if (typeIsConstible)
               {
               if (performTransformation3c(comp, "O^O transformIndirectLoad: [%p] turn final %s %s into load const\n", node, node->getOpCode().getName(), symRef->getName(comp->getDebug())))
                  {
                  node->getAndDecChild(0);
                  node->setNumChildren(0);
                  bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
                  switch (loadType)
                     {
                     case TR_Int32:
                        node->setOpCodeValue(TR_iconst);
                        node->setInt(getInt32FieldAt(*baseObjectRefLocation, fieldOffset));
                        break;
                     case TR_Int64:
                        node->setOpCodeValue(TR_lconst);
                        node->setLongInt(getInt64FieldAt(*baseObjectRefLocation, fieldOffset));
                        break;
                     default:
                        assume2(0, "Unexpected type %d", node->getDataType());
                        break;
                     }
                  releaseVMAccessIfNeeded(haveAcquiredVMAccess);
                  return node;
                  }
               }
            else if (symrefIsImprovable)
               {
               bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
               uintptrj_t targetObjectReference = getReferenceFieldAt(*baseObjectRefLocation, fieldOffset);
               TR_SymbolReference *improvedSymRef = comp->getSymRefTab()->findOrCreateSymRefWithKnownObject(node->getSymbolReference(), &targetObjectReference);
               releaseVMAccessIfNeeded(haveAcquiredVMAccess);
               if (targetObjectReference)
                  {
                  if (improvedSymRef->hasKnownObjectIndex()
                     && performTransformation5c(comp, "O^O transformIndirectLoad: [%p] use object-specific symref #%d (=obj%d) for %s of final field %s\n",
                        node, improvedSymRef->getReferenceNumber(), improvedSymRef->getKnownObjectIndex(), node->getOpCode().getName(), symRef->getName(comp->getDebug())))
                     {
                     node->setSymbolReference(improvedSymRef);
                     node->setIsNull   (false, comp);
                     node->setIsNonNull(true,  comp);
                     }
                  }
               else
                  {
                  if (performTransformation1c(comp, "O^O transformIndirectLoad: [%p] field is null - change to aconst NULL\n", node))
                     {
                     node->getAndDecChild(0);
                     node->setNumChildren(0);
                     node->setOpCodeValue(TR_aconst);
                     node->setPointer(NULL);
                     node->setIsNull   (true,  comp);
                     node->setIsNonNull(false, comp);
                     }
                  }
               return node;
               }
            }
         }
      }

   return NULL;
   }

bool
TR_J9VMBase::canDereferenceAtCompileTime(TR_SymbolReference *fieldRef, TR_Compilation *comp)
   {
   // Note that this query only looks at the field shadow symref; it says
   // nothing about the underlying object in which fieldRef is located.  For
   // example, if the field is a java field, this can return true, yet the
   // compiler still can't dereference the field unless the underlying object
   // is known to have finished initialization.
   //
   if (fieldRef->getSymbol()->isShadow())
      {
      if (fieldRef->getReferenceNumber() < comp->getSymRefTab()->getNumPredefinedSymbols())
         {
         return isFinalFieldOfNativeStruct(fieldRef, comp) || isFinalFieldPointingAtNativeStruct(fieldRef, comp);
         }
      else switch (fieldRef->getSymbol()->getRecognizedField())
         {
         case TR_Symbol::Java_lang_invoke_MethodHandle_modifiers:
            return true;
         default:
            return fieldRef->getSymbol()->isFinal();
         }
      }
   else
      {
      return false;
      }
   }

bool
TR_J9VMBase::transformIndirectLoadChainAt(TR_Compilation *comp, TR_Node *node, TR_Node *baseExpression, uintptrj_t *baseReferenceLocation, TR_Node **removedNode)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   bool result = transformIndirectLoadChainImpl(comp, node, baseExpression, (void*)*baseReferenceLocation, removedNode);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

bool
TR_J9VMBase::transformIndirectLoadChain(TR_Compilation *comp, TR_Node *node, TR_Node *baseExpression, TR_KnownObjectTable::Index baseKnownObject, TR_Node **removedNode)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   bool result = transformIndirectLoadChainImpl(comp, node, baseExpression, (void*)comp->getKnownObjectTable()->getPointer(baseKnownObject), removedNode);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

static bool changeIndirectLoadIntoConst(TR_Node *node, TR_ILOpCodes opCode, TR_Node **removedChild, TR_Compilation *comp)
   {
   // Note that this only does part of the job.  Caller must actually set the
   // constant value / symref / anything else that may be necessary.
   //
   TR_ILOpCode opCodeObject; opCodeObject.setOpCodeValue(opCode);
   if (performTransformation3c(comp, "O^O transformIndirectLoadChain: change %s [%p] into %s\n", node->getOpCode().getName(), node, opCodeObject.getName()))
      {
      *removedChild = node->getFirstChild();
      node->setNumChildren(0);
      node->setOpCodeValue(opCode);
      node->setFlags(0);
      return true;
      }
   return false;
   }

bool
TR_J9VMBase::transformIndirectLoadChainImpl(TR_Compilation *comp, TR_Node *node, TR_Node *baseExpression, void *baseAddress, TR_Node **removedNode)
   {
   assume(haveAccess(), "transformIndirectLoadChain requires VM access");
   assume3c(node->getOpCode().isLoadIndirect(), comp, "Expecting indirect load; found %s %p", node->getOpCode().getName(), node);
   assume4c(node->getNumChildren() == 1, comp, "Expecting indirect load %s %p to have one child; actually has %d", node->getOpCode().getName(), node, node->getNumChildren());
   
   if (isAOT())
      {
      return false;
      }
   
   TR_SymbolReference *symRef = node->getSymbolReference();
   if (!canDereferenceAtCompileTime(symRef, comp))
      return false;

   // Dereference the chain starting from baseAddress and ending with the
   // structure (which may be a Java object) that contains the field we want to read.
   //
   uint8_t *structure = (uint8_t*)dereferenceStructPointerChain(baseAddress, baseExpression, node->getFirstChild(), comp);
   if (!verifyFieldAccess(structure, node->getSymbolReference(), comp))
      return false;

   // The last step in the dereference chain is not necessarily an address.
   // Determine what it is and transform node appropriately.
   //
   TR_DataTypes loadType = node->getOpCode().getDataType();
   switch (loadType)
      {
      case TR_Int32:
         {
         int32_t value = *(int32_t*)(structure + symRef->getOffset());
         if (changeIndirectLoadIntoConst(node, TR_iconst, removedNode, comp))
            node->setInt(value);
         else
            return false;
         }
         break;
      case TR_Int64:
         {
         int64_t value = *(int64_t*)(structure + symRef->getOffset());
         if (changeIndirectLoadIntoConst(node, TR_lconst, removedNode, comp))
            node->setLongInt(value);
         else
            return false;
         }
         break;
      case TR_Float:
         {
         float value = *(float*)(structure + symRef->getOffset());
         if (changeIndirectLoadIntoConst(node, TR_fconst, removedNode, comp))
            node->setFloat(value);
         else
            return false;
         }
         break;
      case TR_Double:
         {
         double value = *(double*)(structure + symRef->getOffset());
         if (changeIndirectLoadIntoConst(node, TR_dconst, removedNode, comp))
            node->setDouble(value);
         else
            return false;
         }
         break;
      case TR_Address:
         {
         if (isFinalFieldPointingAtRepresentableNativeStruct(symRef, comp))
            {
            if (isFinalFieldPointingAtJ9Class(symRef, comp))
               {
               if (changeIndirectLoadIntoConst(node, TR_loadaddr, removedNode, comp))
                  {
                  TR_OpaqueClassBlock *value = *(TR_OpaqueClassBlock**)(structure + symRef->getOffset());
                  node->setSymbolReference(comp->getSymRefTab()->findOrCreateClassSymbol(comp->getMethodSymbol(), -1, value));
                  }
               else
                  {
                  return false;
                  }
               }
            else
               {
               // TODO: What about representable native structs that aren't J9Classes?
               // (Are there any?)
               return false;
               }
            }
         else if (isFinalFieldPointingAtNativeStruct(symRef, comp))
            {
            // Representable native structs are handled before now.  All
            // remaining natives are hopeless.
            return false;
            }
         else if (symRef->getSymbol()->isCollectedReference() && !symRef->hasKnownObjectIndex())
            {
            uintptrj_t value = getReferenceFieldAtAddress(structure + symRef->getOffset());
            if (value)
               {
               TR_SymbolReference *improvedSymRef = comp->getSymRefTab()->findOrCreateSymRefWithKnownObject(node->getSymbolReference(), &value);
               if (  improvedSymRef->hasKnownObjectIndex()
                  && performTransformation3c(comp, "O^O transformIndirectLoadChain: %s [%p] is obj%d\n", node->getOpCode().getName(), node, improvedSymRef->getKnownObjectIndex()))
                  {
                  node->setSymbolReference(improvedSymRef);
                  node->setIsNull   (false, comp);
                  node->setIsNonNull(true,  comp);
                  }
               else
                  {
                  return false;
                  }
               }
            else
               {
               if (changeIndirectLoadIntoConst(node, TR_aconst, removedNode, comp))
                  {
                  node->setPointer(NULL);
                  node->setIsNull   (true,  comp);
                  node->setIsNonNull(false, comp);
                  }
               else
                  {
                  return false;
                  }
               }
            }
         else
            {
            return false;
            }
         }
         break;
      default:
         return false;
      }

   // If we don't change the node, we ought to return "false" before this
   // point.  However, "true" is a safe answer whenever the node MIGHT have
   // changed, so that's the best default thing to do here.
   //
   return true;
   }

uint32_t
TR_J9VMBase::getSizeOfArrayElement(TR_Node * node)
   {
   assume2(node->getOpCodeValue() == TR_newarray || node->getOpCodeValue() == TR_anewarray, "getSizeOfArrayElement expects either newarray or anewarray at [%p]", node);

   if (node->getOpCodeValue() == TR_anewarray)
      {
#if defined(J9VM_GC_COMPRESSED_POINTERS)
      return getAddressFieldSize();
#else
      return TR_Symbol::convertTypeToSize(TR_Address, this);
#endif
      }

   assume(node->getSecondChild()->getOpCode().isLoadConst(), "Expecting const child \n");
   switch (node->getSecondChild()->getInt())
      {
      case 4:
         return (uint32_t) getElementSizeOfBooleanArray();
      case 8:
         return 1;
      case 5:
      case 9:
         return 2;
      case 7:
      case 11:
         return 8;
      }
   return 4;
   }

int64_t
TR_J9VMBase::getMaximumArraySizeInElements(TR_Node *newArray, TR_Compilation *comp)
   {
   int64_t result = MAX_LONG;
   switch (newArray->getOpCodeValue())
      {
      case TR_newarray:
      case TR_anewarray:
         result = getMaximumArraySizeInElements(getSizeOfArrayElement(newArray), comp);
         break;
      case TR_multianewarray:
         result = getMaximumArraySizeInElements(getAddressFieldSize(), comp);
         break;
      default:
         assume2(0, "Unexpected node %p in getMaximumArraySizeInElements", newArray);
         break;
      }
   //trace2c(comp, "getMaximumArraySizeInElements(%p) = %lld\n", newArray, result);
   return result;
   }

int64_t
TR_J9VMBase::getMaximumArraySizeInElements(int32_t knownMinElementSize, TR_Compilation *comp)
   {
   int64_t result = MAX_INT; // On Java, indexes are signed 32-bit ints

   // An array can't be larger than the heap.  Limit the index based on that.
   //
   int32_t minElementSize = MAX(1, knownMinElementSize);
   int64_t maxHeapSize = getMaximumHeapSize();
   if (maxHeapSize == -1)
      {
      // getMaximumHeapSize has an irritating habit of returning -1 sometimes,
      // for some reason.  Must compensate for this corner case here.
      //
      if (comp->cg()->is64BitTarget())
         {
         // Ok, in theory it could be MAX_UNSIGNED_LONG.  This isn't worth the
         // hassle of using uint64_t and worrying about signedness.  When the
         // day comes that a Java program needs an array larger than 8 billion
         // gigabytes, our great-grandchildren can switch to int128_t, assuming
         // Testarossa has not yet become self-aware and fixed this limitation
         // by itself.
         //
         maxHeapSize = MAX_LONG;
         }
      else
         {
         maxHeapSize = MAX_UNSIGNED_INT; // Heap can't be larger than the address space
         }
      }

   result = MIN(result, maxHeapSize / minElementSize);

   //trace2c(comp, "getMaximumArraySizeInElements(%d) = %lld\n", knownMinElementSize, result);
   return result;
   }

int64_t
TR_J9VMBase::getMaximumHeapSize()
   {
   J9JavaVM * jvm = _jitConfig->javaVM;
   if (!jvm || isAOT())
      return -1;
   J9MemoryManagerFunctions * mmf = jvm->memoryManagerFunctions;
   return (int64_t) mmf->j9gc_get_maximum_heap_size(jvm);
   }


// Creates a node to initialize the local object flags field
//
TR_Node *
TR_J9VMBase::initializeLocalObjectFlags(TR_Compilation * comp, TR_Node * allocationNode, J9Class * ramClass)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded(); // I don't think this is really needed

#if defined(J9VM_INTERP_FLAGS_IN_CLASS_SLOT)
   int32_t initValue = 0;
#else
   int32_t initValue = ramClass->romClass->instanceShape;
#endif

   if (!TR_Options::getCmdLineOptions()->realTimeGC())
      {
#if !defined(J9VM_GC_REALTIME) || defined (J9VM_GC_COMBINATION_SPEC)
      initValue |= vmThread()->allocateThreadLocalHeap.objectFlags;
#endif
      }

#ifndef J9VM_GC_COMBINATION_SPEC
   if (canGenerateArraylets() && useHybridArraylets() &&
       (allocationNode->getOpCodeValue() == TR_anewarray || allocationNode->getOpCodeValue() == TR_newarray))
      {
      initValue |= J9_GC_ARRAYLET_LAYOUT_DATA_IN_SPINE_BIT;
      }
#endif


   TR_Node * result;
   result = TR_Node::create(comp, allocationNode, TR_iconst, 0, initValue);

   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

bool TR_J9VMBase::hasTwoWordObjectHeader()
  {
#if defined(J9VM_THR_LOCK_NURSERY)
  return true;
#endif
  return false;;
  }

// Create trees to initialize the header of an object that is being created
// on the stack.
//
void
TR_J9VMBase::initializeLocalObjectHeader(TR_Compilation * comp, TR_Node * allocationNode, TR_TreeTop * allocationTreeTop)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   TR_TreeTop * prevTree = allocationTreeTop;

   TR_Node             * classNode = allocationNode->getFirstChild();
   TR_StaticSymbol     * classSym  = classNode->getSymbol()->castToStaticSymbol();
   TR_OpaqueClassBlock * ramClass  = (TR_OpaqueClassBlock *) classSym->getStaticAddress();
   J9ROMClass          * romClass  = getRomClass(ramClass);

   prevTree = initializeClazzFlagsMonitorFields(comp, prevTree, allocationNode, classNode, (J9Class*) ramClass);

   if (isPackedClass(ramClass))
      {
      TR_SymbolReference *pointerSymRef = comp->getSymRefTab()->findOrCreatePackedDataPointerSymbolRef();
      TR_Node *pointerStore = TR_Node::create(comp, TR_iastore, 2, allocationNode, allocationNode, pointerSymRef);
      prevTree = TR_TreeTop::create(comp, prevTree, pointerStore);
      TR_SymbolReference *offsetSymRef = comp->getSymRefTab()->findOrCreatePackedDataOffsetSymbolRef();
      const TR_DataTypes offsetType = (comp->cg()->is64BitTarget() && !comp->useCompressedPointers()) ? TR_Int64 : TR_Int32;
      TR_Node *offsetStore = TR_Node::create(comp, opCodeForIndirectStore(offsetType), 2, allocationNode,
            ((offsetType == TR_Int64) ?
                  TR_Node::lconst(comp, allocationNode, packedObjectSize()) :
                  TR_Node::iconst(comp, allocationNode, packedObjectSize())), offsetSymRef);
      prevTree = TR_TreeTop::create(comp, prevTree, offsetStore);
      }

   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   }


// Create trees to initialize the header of an object that is being created
// on the stack.
//
void
TR_J9VMBase::initializeLocalPackedObjectHeader(TR_Compilation * comp, TR_Node * allocationNode, TR_TreeTop * allocationTreeTop, bool useAllocNode)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   TR_TreeTop * prevTree = allocationTreeTop;

   TR_Node             * classNode = allocationNode->getFirstChild();
   TR_StaticSymbol     * classSym  = classNode->getSymbol()->castToStaticSymbol();
   TR_OpaqueClassBlock * ramClass  = (TR_OpaqueClassBlock *) classSym->getStaticAddress();
   J9ROMClass          * romClass  = getRomClass(ramClass);

   prevTree = initializeClazzFlagsMonitorFields(comp, prevTree, allocationNode, classNode, (J9Class*) ramClass);

   TR_SymbolReference *pointerSymRef = comp->getSymRefTab()->findOrCreatePackedDataPointerSymbolRef();
   TR_Node *pointerStore = TR_Node::create(comp, TR_iastore, 2, allocationNode, useAllocNode ? allocationNode->getSecondChild() : TR_Node::aconst(comp, allocationNode, 0), pointerSymRef);
   prevTree = TR_TreeTop::create(comp, prevTree, pointerStore);
   TR_SymbolReference *offsetSymRef = comp->getSymRefTab()->findOrCreatePackedDataOffsetSymbolRef();
   const TR_DataTypes offsetType = (comp->cg()->is64BitTarget() && !comp->useCompressedPointers()) ? TR_Int64 : TR_Int32;
   TR_Node *offsetStore = TR_Node::create(comp, opCodeForIndirectStore(offsetType), 2, allocationNode, useAllocNode ? 
                                          allocationNode->getChild(2) : 
                                          ((offsetType == TR_Int64) ? 
                                          TR_Node::lconst(comp, allocationNode, 0) : 
                                          TR_Node::iconst(comp, allocationNode, 0)), offsetSymRef);
   prevTree = TR_TreeTop::create(comp, prevTree, offsetStore);

   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   }



// Create trees to initialize the header of an array that is being allocated on the stack
// works for newarray and anewarray allocations
//
void
TR_J9VMBase::initializeLocalArrayHeader(TR_Compilation * comp, TR_Node * allocationNode, TR_TreeTop * allocationTreeTop)
   {
   /*
   assume(!isAOT(), "We create a class symbol with a CPI of -1!\n"
                    "Enabling recognized methods might trigger this code to be executed in AOT!\n"
                    "We need to figure out how to validate and relocate this symbol safely before removing this assertion!\n");
   */
   TR_TreeTop * prevTree = allocationTreeTop;
   TR_ILOpCodes kind = allocationNode->getOpCodeValue();
   J9Class * ramClass = 0;
   bool isPacked = false;  // is only true for TR_panewarray

   switch (kind)
      {
      case TR_newarray:
         {
         assume(allocationNode->getSecondChild()->getOpCode().isLoadConst(), "Expecting const child \n");
         int32_t arrayClassIndex = allocationNode->getSecondChild()->getInt();
         struct J9Class ** arrayClasses = &_jitConfig->javaVM->booleanArrayClass;
         ramClass = arrayClasses[arrayClassIndex - 4];
         }
         break;

      case TR_anewarray:
         {
         TR_Node            * classRef    = allocationNode->getSecondChild();
         TR_SymbolReference * classSymRef = classRef->getSymbolReference();
         TR_StaticSymbol    * classSym    = classSymRef->getSymbol()->getStaticSymbol();
         assume(!classSymRef->isUnresolved(), "Cannot allocate an array with unresolved base class");
         TR_OpaqueClassBlock* clazz = (TR_OpaqueClassBlock*)classSym->getStaticAddress();
         ramClass = convertClassOffsetToClassPtr(clazz);
         ramClass = ramClass->arrayClass;
         }
         break;

      default:
         assume(0, "Expecting TR_newarray or TR_anewarray opcodes only");
      }


   J9ROMClass * romClass = ramClass->romClass;
   TR_Node *classNode = TR_Node::create(comp, allocationNode, TR_loadaddr, 0, comp->getSymRefTab()->findOrCreateClassSymbol(comp->getMethodSymbol(), -1, convertClassPtrToClassOffset(ramClass)));

   prevTree = initializeClazzFlagsMonitorFields(comp, prevTree, allocationNode, classNode, ramClass);

   // -----------------------------------------------------------------------------------
   // Initialize the size field
   // -----------------------------------------------------------------------------------

   int32_t elementSize = getSizeOfArrayElement(allocationNode);
   assume(allocationNode->getFirstChild()->getOpCode().isLoadConst(), "Expecting const child \n");

   int32_t instanceSize = allocationNode->getFirstChild()->getInt();

   TR_SymbolReference *arraySizeSymRef;
   if (canGenerateArraylets() && useHybridArraylets() && isDiscontiguousArray(instanceSize))
      {
      assume(instanceSize == 0, "arbitrary discontiguous stack allocated objects not supported yet");

      // Contiguous size field is zero (mandatory)
      //
      TR_Node* node = TR_Node::create(comp, allocationNode, TR_iconst, 0, instanceSize);
      arraySizeSymRef = comp->getSymRefTab()->findOrCreateContiguousArraySizeSymbolRef();
      node = TR_Node::create(comp, TR_iistore, 2, allocationNode, node, arraySizeSymRef);
      prevTree = TR_TreeTop::create(comp, prevTree, node);

      arraySizeSymRef = comp->getSymRefTab()->findOrCreateDiscontiguousArraySizeSymbolRef();
      }
#if defined(J9VM_GC_COMBINATION_SPEC) && defined(TR_HOST_S390) && defined(TR_TARGET_S390)
   //TODO remove define s390 flags when x and power enable suport for inlining 0 size arrays
   // clean up canGenerateArraylets() && useHybridArraylets() && isDiscontiguousArray(instanceSize) queiries?
   else if (!comp->getOptions()->realTimeGC() && instanceSize == 0)
      {
      // Contiguous size field is zero (mandatory)
      // For J9VM_GC_COMBINATION_SPEC only 0 size discontinous arrays are supported
      TR_Node* node = TR_Node::create(comp, allocationNode, TR_iconst, 0, instanceSize);
      arraySizeSymRef = comp->getSymRefTab()->findOrCreateContiguousArraySizeSymbolRef();
      node = TR_Node::create(comp, TR_iistore, 2, allocationNode, node, arraySizeSymRef);
      prevTree = TR_TreeTop::create(comp, prevTree, node);

      arraySizeSymRef = comp->getSymRefTab()->findOrCreateDiscontiguousArraySizeSymbolRef();
      }
#endif
   else
      {
      arraySizeSymRef = comp->getSymRefTab()->findOrCreateContiguousArraySizeSymbolRef();
      }

   TR_Node* node = TR_Node::create(comp, allocationNode, TR_iconst, 0, instanceSize);
   node = TR_Node::create(comp, TR_iistore, 2, allocationNode, node, arraySizeSymRef);
   prevTree = TR_TreeTop::create(comp, prevTree, node);

   // -----------------------------------------------------------------------------------
   // Initialize the arraylet spine
   // -----------------------------------------------------------------------------------

#ifndef J9VM_GC_COMBINATION_SPEC

   #if defined(J9VM_GC_ARRAYLETS)
   if (!useHybridArraylets())
      {
      node = TR_Node::create(comp, TR_aiadd, 2, allocationNode,
                TR_Node::create(comp, allocationNode, TR_iconst, 0, getArrayletFirstElementOffset(elementSize, comp)));
      node = TR_Node::create(comp, TR_iastore, 2, allocationNode, node,
                comp->getSymRefTab()->findOrCreateArrayletSpineFirstElementSymbolRef(elementSize));
      prevTree = TR_TreeTop::create(comp, prevTree, node);
      }
   #endif

#endif


   if (isPacked)
      {
      TR_SymbolReference *pointerSymRef = comp->getSymRefTab()->findContiguousPackedArrayObjectSymbolRef();
      TR_Node *pointerStore = TR_Node::create(comp, TR_iastore, 2, allocationNode, allocationNode, pointerSymRef);
      prevTree = TR_TreeTop::create(comp, prevTree, pointerStore);
      TR_SymbolReference *offsetSymRef = comp->getSymRefTab()->findContiguousPackedArrayOffsetSymbolRef();
      const TR_DataTypes offsetType = (comp->cg()->is64BitTarget() && !comp->useCompressedPointers()) ? TR_Int64 : TR_Int32;
      TR_Node *offsetStore = TR_Node::create(comp, opCodeForIndirectStore(offsetType), 2, allocationNode,
            ((offsetType == TR_Int64) ?
                  TR_Node::lconst(comp, allocationNode, getContiguousPackedArrayHeaderSizeInBytes()) :
                  TR_Node::iconst(comp, allocationNode, getContiguousPackedArrayHeaderSizeInBytes())), offsetSymRef);
      prevTree = TR_TreeTop::create(comp, prevTree, offsetStore);
      }
   }


TR_TreeTop* TR_J9VMBase::initializeClazzFlagsMonitorFields(TR_Compilation* comp, TR_TreeTop* prevTree,
   TR_Node* allocationNode, TR_Node* classNode, J9Class* ramClass)
   {
   // -----------------------------------------------------------------------------------
   // Initialize the clazz field
   // -----------------------------------------------------------------------------------
   TR_Node* node;
#if !defined(J9VM_INTERP_FLAGS_IN_CLASS_SLOT)
   node = TR_Node::create(comp, TR_iastore, 2, allocationNode, classNode,
      comp->getSymRefTab()->findOrCreateVftSymbolRef());
   
   prevTree = TR_TreeTop::create(comp, prevTree, node);
#endif
   
   // -----------------------------------------------------------------------------------
   // Initialize the flags field
   // -----------------------------------------------------------------------------------

   node = initializeLocalObjectFlags(comp, allocationNode, ramClass);

#if defined(J9VM_INTERP_FLAGS_IN_CLASS_SLOT)
   node = TR_Node::create(comp, TR_aiadd, 2, classNode, node);
   node = TR_Node::create(comp, TR_iastore, 2, allocationNode, node,
                          comp->getSymRefTab()->findOrCreateVftSymbolRef());
#else
   node = TR_Node::create(comp, TR_iistore, 2, allocationNode, node,
                          comp->getSymRefTab()->findOrCreateHeaderFlagsSymbolRef());
#endif

   prevTree = TR_TreeTop::create(comp, prevTree, node);

   // -----------------------------------------------------------------------------------
   // Initialize the monitor field
   // -----------------------------------------------------------------------------------

   int32_t lwOffset = getByteOffsetToLockword((TR_OpaqueClassBlock *)ramClass);
   if (lwOffset > 0)
      {
      // Initialize the monitor field
      //
      if (comp->cg()->is64BitTarget() && comp->cg()->fe()->generateCompressedLockWord())
         {
         node = TR_Node::create(comp, allocationNode, TR_iconst, 0, 0);
         node = TR_Node::create(comp, TR_iistore, 2, allocationNode, node,
            comp->getSymRefTab()->findOrCreateGenericIntNonArrayShadowSymbolReference(lwOffset));
         }
      else
         {
         node = TR_Node::create(comp, TR_iastore, 2, allocationNode, TR_Node::createPointerNode(comp, allocationNode, TR_aconst, (void *)0),
            comp->getSymRefTab()->findOrCreateGenericIntNonArrayShadowSymbolReference(lwOffset));
         }
      prevTree = TR_TreeTop::create(comp, prevTree, node);
      }
   return prevTree;
   }

bool
TR_J9VMBase::tlhHasBeenCleared()
   {
#if defined(J9VM_GC_BATCH_CLEAR_TLH)
   J9JavaVM * jvm = _jitConfig->javaVM;
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   bool result = jvm->memoryManagerFunctions->isAllocateZeroedTLHPagesEnabled(jvm);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
#else
   return false;
#endif
   }

bool
TR_J9VMBase::isStaticObjectFlags()
   {
   J9JavaVM * jvm = _jitConfig->javaVM;
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   bool result = jvm->memoryManagerFunctions->isStaticObjectAllocateFlags(jvm) != 0;
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

uint32_t
TR_J9VMBase::getStaticObjectFlags()
   {
   uint32_t staticFlag;
   assume(isStaticObjectFlags(), "isStaticObjectFlags must be true to invoke getStaticObjectFlags");
   J9JavaVM * jvm = _jitConfig->javaVM;
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   staticFlag = (uint32_t)jvm->memoryManagerFunctions->getStaticObjectAllocateFlags(jvm);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return staticFlag;
   }

uintptrj_t
TR_J9VMBase::getOverflowSafeAllocSize()
   {
   J9JavaVM *jvm = _jitConfig->javaVM;
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   uintptrj_t result = jvm->memoryManagerFunctions->j9gc_get_overflow_safe_alloc_size(jvm);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

void
TR_J9VMBase::unknownByteCode(TR_Compilation * comp, U_8 opcode)
   {
   // an unknown bytecode may be seen if the method contains a breakpoint, in that case we should abort the compile
   //
   outOfMemory(comp, "Unknown bytecode");
   }

TR_ByteCode opcodeToEnum[] =
   {
   /* 0   */ BCnop,
   /* 1   */ BCaconstnull, BCiconstm1,
   /* 3   */ BCiconst0, BCiconst1, BCiconst2, BCiconst3, BCiconst4, BCiconst5,
   /* 9   */ BClconst0, BClconst1,
   /* 11  */ BCfconst0, BCfconst1, BCfconst2,
   /* 14  */ BCdconst0, BCdconst1,
   /* 16  */ BCbipush, BCsipush,
   /* 18  */ BCldc, BCldcw, BCldc2lw,
   /* 21  */ BCiload, BClload, BCfload, BCdload, BCaload,
   /* 26  */ BCiload0, BCiload1, BCiload2, BCiload3,
   /* 30  */ BClload0, BClload1, BClload2, BClload3,
   /* 34  */ BCfload0, BCfload1, BCfload2, BCfload3,
   /* 38  */ BCdload0, BCdload1, BCdload2, BCdload3,
   /* 42  */ BCaload0, BCaload1, BCaload2, BCaload3,
   /* 46  */ BCiaload, BClaload, BCfaload, BCdaload, BCaaload, BCbaload, BCcaload, BCsaload,
   /* 54  */ BCistore, BClstore, BCfstore, BCdstore, BCastore,
   /* 59  */ BCistore0, BCistore1, BCistore2, BCistore3,
   /* 63  */ BClstore0, BClstore1, BClstore2, BClstore3,
   /* 67  */ BCfstore0, BCfstore1, BCfstore2, BCfstore3,
   /* 71  */ BCdstore0, BCdstore1, BCdstore2, BCdstore3,
   /* 75  */ BCastore0, BCastore1, BCastore2, BCastore3,
   /* 79  */ BCiastore, BClastore, BCfastore, BCdastore, BCaastore, BCbastore, BCcastore, BCsastore,
   /* 87  */ BCpop, BCpop2,
   /* 89  */ BCdup, BCdupx1, BCdupx2, BCdup2, BCdup2x1, BCdup2x2,
   /* 95  */ BCswap,
   /* 96  */ BCiadd, BCladd, BCfadd, BCdadd,
   /* 100 */ BCisub, BClsub, BCfsub, BCdsub,
   /* 104 */ BCimul, BClmul, BCfmul, BCdmul,
   /* 108 */ BCidiv, BCldiv, BCfdiv, BCddiv,
   /* 112 */ BCirem, BClrem, BCfrem, BCdrem,
   /* 116 */ BCineg, BClneg, BCfneg, BCdneg,
   /* 120 */ BCishl, BClshl, BCishr, BClshr, BCiushr, BClushr,
   /* 126 */ BCiand, BCland,
   /* 128 */ BCior, BClor, BCixor, BClxor,
   /* 132 */ BCiinc,
   /* 133 */ BCi2l, BCi2f, BCi2d,
   /* 136 */ BCl2i, BCl2f, BCl2d,
   /* 139 */ BCf2i, BCf2l, BCf2d,
   /* 142 */ BCd2i, BCd2l, BCd2f,
   /* 145 */ BCi2b, BCi2c, BCi2s,
   /* 148 */ BClcmp, BCfcmpl, BCfcmpg, BCdcmpl, BCdcmpg,
   /* 153 */ BCifeq, BCifne, BCiflt, BCifge, BCifgt, BCifle,
   /* 159 */ BCificmpeq, BCificmpne, BCificmplt, BCificmpge, BCificmpgt, BCificmple, BCifacmpeq, BCifacmpne,
   /* 167 */ BCgoto, BCunknown, BCunknown,
   /* 170 */ BCtableswitch, BClookupswitch,
   /* 172 */ BCgenericReturn, BCgenericReturn, BCgenericReturn,
   /* 175 */ BCgenericReturn, BCgenericReturn, BCgenericReturn,
   /* 178 */ BCgetstatic, BCputstatic, BCgetfield, BCputfield,
   /* 182 */ BCinvokevirtual, BCinvokespecial, BCinvokestatic, BCinvokeinterface, BCinvokedynamic,
   /* 187 */ BCnew, BCnewarray, BCanewarray,
   /* 190 */ BCarraylength,
   /* 191 */ BCathrow,
   /* 192 */ BCcheckcast,
   /* 193 */ BCinstanceof,
   /* 194 */ BCmonitorenter, BCmonitorexit,
   /* 196 */ BCunknown,
   /* 197 */ BCmultianewarray,
   /* 198 */ BCifnull, BCifnonnull,
   /* 200 */ BCgotow, BCunknown,
   /* 202 */ BCunknown,
   /* 203 */ BCiloadw, BClloadw, BCfloadw, BCdloadw, BCaloadw,
   /* 208 */ BCistorew, BClstorew, BCfstorew, BCdstorew, BCastorew,
   /* 213 */ BCiincw, BCunknown,
   /* 215 - JBaload0getfield */ BCaload0,
   /* 216 - JBnewdup */ BCnew,
   /* 217 */ BCunknown, BCunknown,
   /* 219 */ BCunknown, BCunknown, BCunknown, BCunknown, BCunknown,
   /* 224 */ BCunknown, BCunknown, BCunknown, BCunknown,
   /* 228 */ BCgenericReturn, BCgenericReturn, BCunknown, BCnop,
   /* 232 */ BCinvokehandle, BCinvokehandlegeneric,
   /* 234 */ BCunknown, BCunknown, BCunknown, BCunknown, BCunknown,
   /* 239 */ BCunknown, BCunknown, BCunknown, BCunknown, BCunknown,
   /* 244 */ BCunknown, BCunknown, BCunknown, BCunknown, BCunknown,
   /* 249 */ BCldc2dw,
   /* 250 */ BCunknown, BCunknown, BCunknown, BCunknown, BCunknown, BCunknown
   };

#if defined(TR_TARGET_X86) && !defined(J9HAMMER) && !(defined(TR_MULTI_CODEGEN_BUILD) && TR_HOST_POWER)
extern "C" void * jitExitInterpreterX; /* SSE float */
extern "C" void * jitExitInterpreterY; /* SSE double */

void VMprepareForSSELinkage(TR_Compilation * comp)
   {
   ((TR_J9VMBase *)comp->fe())->_jitConfig->javaVM->floatJITExitInterpreter = &jitExitInterpreterX;
   ((TR_J9VMBase *)comp->fe())->_jitConfig->javaVM->doubleJITExitInterpreter = &jitExitInterpreterY;
   }
#endif // defined(TR_TARGET_X86) && !defined(J9HAMMER) && !(defined(TR_MULTI_CODEGEN_BUILD) && TR_HOST_POWER)

// Temporary logic for @TenantScope fields which are only supported in the interpreter.
bool TR_J9VMBase::canGenerateStaticFieldAccesses()
   {
#if defined(J9VM_OPT_TENANT)
   if (!TR_Options::getCmdLineOptions()->getOption(TR_DisableMultiTenancy))
      return true;
   J9JavaVM * javaVM = _jitConfig->javaVM;
   if (NULL != javaVM->tenantGlobals)
      {
         if (0 != (javaVM->tenantGlobals->flags & J9_TENANT_TENANT_SCOPE_FIELD_ENABLED) ||
        0 != (javaVM->tenantGlobals->flags & J9_TENANT_ISOLATE_ENABLED))
         {
            // Temporary logic for @TenantScope fields which are only supported in the interpreter.
            return false;
         }
      }
#endif
   return true;
   }

bool TR_J9VMBase::isMultiTenantEnabled() 
   {
#if defined(J9VM_OPT_TENANT)
   J9JavaVM * javaVM = _jitConfig->javaVM;
   if (NULL == javaVM->tenantGlobals)
      {
      return false;
      }

   return (javaVM->tenantGlobals->flags & J9_TENANT_TENANT_SCOPE_FIELD_ENABLED) || 
          (javaVM->tenantGlobals->flags & J9_TENANT_ISOLATE_ENABLED);
#else
   return false;
#endif
   }

extern "C" BOOLEAN jitTestOSForSSESupport(void);

extern TR_X86CPUIDBuffer *queryX86TargetCPUID(void * javaVM);

const char *
TR_J9VMBase::getX86ProcessorVendorId()
   {
   assume(_jitConfig, "jitConfig is not defined!");
   assume(_jitConfig->javaVM, "jitConfig->javaVM is not defined!");

   static char buf[13];

   // Terminate the vendor ID with NULL before returning.
   //
   strncpy(buf, queryX86TargetCPUID(_jitConfig->javaVM)->_vendorId, 12);
   buf[12] = '\0';

   return buf;
   }

uint32_t
TR_J9VMBase::getX86ProcessorSignature()
   {
   assume(_jitConfig, "jitConfig is not defined!");
   assume(_jitConfig->javaVM, "jitConfig->javaVM is not defined!");
   return queryX86TargetCPUID(_jitConfig->javaVM)->_processorSignature;
   }

uint8_t
TR_J9VMBase::getX86ProcessorBrandId()
   {
   assume(_jitConfig, "jitConfig is not defined!");
   assume(_jitConfig->javaVM, "jitConfig->javaVM is not defined!");
   return queryX86TargetCPUID(_jitConfig->javaVM)->_brandIdEtc & 0xFF;
   }

uint32_t
TR_J9VMBase::getX86ProcessorFeatureFlags()
   {
   assume(_jitConfig, "jitConfig is not defined!");
   assume(_jitConfig->javaVM, "jitConfig->javaVM is not defined!");
   return queryX86TargetCPUID(_jitConfig->javaVM)->_featureFlags;
   }

uint32_t
TR_J9VMBase::getX86ProcessorFeatureFlags2()
   {
   assume(_jitConfig, "jitConfig is not defined!");
   assume(_jitConfig->javaVM, "jitConfig->javaVM is not defined!");
   return queryX86TargetCPUID(_jitConfig->javaVM)->_featureFlags2;
   }

uint32_t
TR_J9VMBase::getX86ProcessorFeatureFlags8()
   {
   assume(_jitConfig, "jitConfig is not defined!");
   assume(_jitConfig->javaVM, "jitConfig->javaVM is not defined!");
   return queryX86TargetCPUID(_jitConfig->javaVM)->_featureFlags8;
   }

uint32_t
TR_J9VMBase::getX86L1InstructionCacheDescription()
   {
   assume(_jitConfig, "jitConfig is not defined!");
   assume(_jitConfig->javaVM, "jitConfig->javaVM is not defined!");
   return queryX86TargetCPUID(_jitConfig->javaVM)->_cacheDescription.l1instr;
   }

uint32_t
TR_J9VMBase::getX86L1DataCacheDescription()
   {
   assume(_jitConfig, "jitConfig is not defined!");
   assume(_jitConfig->javaVM, "jitConfig->javaVM is not defined!");
   return queryX86TargetCPUID(_jitConfig->javaVM)->_cacheDescription.l1data;
   }

uint32_t
TR_J9VMBase::getX86L2CacheDescription()
   {
   assume(_jitConfig, "jitConfig is not defined!");
   assume(_jitConfig->javaVM, "jitConfig->javaVM is not defined!");
   return queryX86TargetCPUID(_jitConfig->javaVM)->_cacheDescription.l2;
   }

uint32_t
TR_J9VMBase::getX86L3CacheDescription()
   {
   assume(_jitConfig, "jitConfig is not defined!");
   assume(_jitConfig->javaVM, "jitConfig->javaVM is not defined!");
   return queryX86TargetCPUID(_jitConfig->javaVM)->_cacheDescription.l3;
   }
   
bool
TR_J9VMBase::testOSForSSESupport()
   {
   return false;
   }
   
bool
TR_J9VMBase::getX86OSSupportsSSE()
   {
   // If the FXSR (bit 24) and SSE (bit 25) bits in the feature flags are not set, SSE is unavailable
   // on this processor.
   //

   uint32_t flags = getX86ProcessorFeatureFlags();

   // Note do not move above call within the if statement below as sh-4
   // compiler aborts at high opt
   if ((flags & 0x03000000) != 0x03000000)
      return false;

   return testOSForSSESupport();
   }

bool
TR_J9VMBase::getX86OSSupportsSSE2()
   {
   // If the FXSR (bit 24) and SSE2 (bit 26) bits in the feature flags are not set, SSE is unavailable
   // on this processor.
   //
         uint32_t flags = getX86ProcessorFeatureFlags();
   if ((flags & 0x05000000) != 0x05000000)
      return false;

   return testOSForSSESupport();
   }

bool
TR_J9VMBase::getX86SupportsTM()
   {
   uint32_t flags8 = getX86ProcessorFeatureFlags8();
   if ((flags8 & TR_RTM) != 0x00000000)  
      return true;
   else return false; 
   }   

bool
TR_J9VMBase::getX86SupportsHLE()
   {
   uint32_t flags8 = getX86ProcessorFeatureFlags8();
   if ((flags8 & TR_HLE) != 0x00000000)  
      return true;
   else return false; 
   }

J9VMThread * getJ9VMThreadFromTR_VM(void * fe)
   {
   return ((TR_J9VMBase *)fe)->_vmThread;
   }

J9JITConfig * getJ9JitConfigFromFE(void * fe)
   {
   return ((TR_FrontEnd *)fe)->getJ9JITConfig();
   }

extern TR_Monitor * vpMonitor;

void acquireVPMutex()
   {
   vpMonitor->enter();
   }

void releaseVPMutex()
   {
   vpMonitor->exit();
   }

void *
TR_J9VMBase::getJ2IThunk(char *signatureChars, uint32_t signatureLength, TR_Compilation *comp)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   void * result = j9ThunkLookupSignature(_jitConfig, signatureLength, signatureChars);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

void *
TR_J9VMBase::setJ2IThunk(char *signatureChars, uint32_t signatureLength, void *thunkptr, TR_Compilation *comp)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   if (j9ThunkNewSignature(_jitConfig, signatureLength, signatureChars, thunkptr))
      {
      releaseVMAccessIfNeeded(haveAcquiredVMAccess);
      outOfMemory(comp, "J9Thunk new signature");
      }

   if (J9_EVENT_IS_HOOKED(jitConfig->javaVM->hookInterface, J9HOOK_VM_DYNAMIC_CODE_LOAD) && !isAOT())
      ALWAYS_TRIGGER_J9HOOK_VM_DYNAMIC_CODE_LOAD(jitConfig->javaVM->hookInterface, jitConfig->javaVM->internalVMFunctions->currentVMThread(jitConfig->javaVM), NULL, (void *) thunkptr, *((uint32_t *)thunkptr - 2), "JIT virtual thunk", NULL);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return thunkptr;
   }

void *
TR_J9VMBase::findPersistentJ2IThunk(char *signatureChars)
   {
   return findPersistentThunk(signatureChars, strlen(signatureChars));
   }

void *
TR_J9VMBase::findPersistentThunk(char *signatureChars, uint32_t signatureLength)
   {
#if defined(J9VM_INTERP_AOT_COMPILE_SUPPORT) && defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390) || defined(TR_HOST_ARM))
   J9SharedDataDescriptor firstDescriptor;
   J9VMThread *curThread = getCurrentVMThread();
   firstDescriptor.address = NULL;

   _jitConfig->javaVM->sharedClassConfig->findSharedData(curThread, signatureChars, signatureLength,
                                                         J9SHR_DATA_TYPE_AOTTHUNK, false, &firstDescriptor, NULL);
   return firstDescriptor.address;
#else
   return NULL;
#endif
   }

void *
TR_J9VMBase::persistJ2IThunk(void *thunk)
   {
   return NULL;  // only needed for AOT compilations
   }

static char *
getJ2IThunkSignature(char *invokeHandleSignature, uint32_t signatureLength, int argsToSkip, char *description, TR_Compilation *comp)
   {
   char *argsToCopy;
   for (argsToCopy = invokeHandleSignature+1; argsToSkip > 0; argsToSkip--)
      argsToCopy = nextSignatureArgument(argsToCopy);
   uint32_t lengthToCopy = signatureLength - (argsToCopy - invokeHandleSignature);

   char *resultBuf = (char*)comp->trMemory()->allocateMemory(2+lengthToCopy, stackAlloc);
   sprintf(resultBuf, "(%.*s", lengthToCopy, argsToCopy);

   if (comp->getOption(TR_TraceCG))
      trace4c(comp, "JSR292: j2i-thunk signature for %s of '%.*s' is '%s'\n", description, signatureLength, invokeHandleSignature, resultBuf);
   return resultBuf;
   }

static TR_Node *
getEquivalentVirtualCallNode(TR_Node *callNode, int argsToSkip, char *description, TR_Compilation *comp)
   {
   TR_Node *j2iThunkCall = TR_Node::create(comp, callNode, callNode->getOpCodeValue(), callNode->getNumChildren() - argsToSkip + 1, callNode->getSymbolReference());
   j2iThunkCall->setChild(0, callNode->getFirstChild()); // first child should be vft pointer but we don't have one
   for (int32_t i = argsToSkip; i < callNode->getNumChildren(); i++) // Skip target address, vtable index
      j2iThunkCall->setChild(i-argsToSkip+1, callNode->getChild(i));
   if (comp->getOption(TR_TraceCG))
      {
      trace2c(comp, "JSR292: j2i-thunk call node for %s is %p:\n", description, j2iThunkCall);
      comp->getDebug()->print(comp->getOutFile(), j2iThunkCall, 2, true);
      }
   return j2iThunkCall;
   }

char *
TR_J9VMBase::getJ2IThunkSignatureForDispatchDirect(char *invokeHandleSignature, uint32_t signatureLength, TR_Compilation *comp)
   {
   // Skip MethodHandle.  Target address is not in the signature for dispatchDirect
   return getJ2IThunkSignature(invokeHandleSignature, signatureLength, 1, "dispatchDirect", comp);
   }

TR_Node *
TR_J9VMBase::getEquivalentVirtualCallNodeForDispatchDirect(TR_Node *callNode, TR_Compilation *comp)
   {
   // Skip target address, keep MethodHandle
   return getEquivalentVirtualCallNode(callNode, 1, "dispatchDirect", comp);
   }

char *
TR_J9VMBase::getJ2IThunkSignatureForDispatchVirtual(char *invokeHandleSignature, uint32_t signatureLength, TR_Compilation *comp)
   {
   // Skip target address, vtable index, eventual receiver
   return getJ2IThunkSignature(invokeHandleSignature, signatureLength, 3, "dispatchVirtual", comp);
   }

TR_Node *
TR_J9VMBase::getEquivalentVirtualCallNodeForDispatchVirtual(TR_Node *callNode, TR_Compilation *comp)
   {
   // Skip target address, vtable index, but leave the ultimate receiver
   return getEquivalentVirtualCallNode(callNode, 2, "dispatchVirtual", comp);
   }

bool
TR_J9VMBase::needsInvokeExactJ2IThunk(TR_Node *callNode, TR_Compilation *comp)
   {
   assume2(callNode->getOpCode().isCall(), "needsInvokeExactJ2IThunk expects call node; found %s", callNode->getOpCode().getName());

   TR_MethodSymbol *methodSymbol = callNode->getSymbol()->castToMethodSymbol();
   TR_Method       *method       = methodSymbol->getMethod();
   if (  methodSymbol->isComputed()
      && (  method->getMandatoryRecognizedMethod() == java_lang_invoke_MethodHandle_invokeExact
         || method->isArchetypeSpecimen()))
      {
      if (isAOT()) // While we're here... we need an AOT relocation for this call
         comp->cg()->addAOTRelocation(new (comp->trHeapMemory()) TR_64BitExternalRelocation(NULL, (uint8_t *)callNode, (uint8_t *)methodSymbol->getMethod()->signatureChars(), TR_J2IThunks, comp->cg()), __FILE__, __LINE__, callNode);

      // We need a j2i thunk when this call executes, in case the target MH has
      // no invokeExact thunk yet.

      TR_J2IThunkTable *thunkTable = comp->trMemory()->getPersistentInfo()->getInvokeExactJ2IThunkTable();
      TR_J2IThunk      *thunk      = thunkTable->findThunk(methodSymbol->getMethod()->signatureChars(), this);
      return (thunk == NULL);
      }
   else
      return false;
   }

uintptrj_t
TR_J9VMBase::methodHandle_thunkableSignature(uintptrj_t methodHandle)
   {
   assume(haveAccess(), "methodHandle_thunkableSignature requires VM access");
   return getReferenceField(getReferenceField(
      methodHandle,
      "thunks",             "Ljava/lang/invoke/ThunkTuple;"),
      "thunkableSignature", "Ljava/lang/String;");
   }

uintptrj_t
TR_J9VMBase::methodHandle_type(uintptrj_t methodHandle)
   {
   assume(haveAccess(), "methodHandle_type requires VM access");
   return getReferenceField(
      methodHandle,
      "type", "Ljava/lang/invoke/MethodType;");
   }

uintptrj_t
TR_J9VMBase::methodType_descriptor(uintptrj_t methodType)
   {
   assume(haveAccess(), "methodType_descriptor requires VM access");
   return getReferenceField(
      methodType,
      "methodDescriptor", "Ljava/lang/String;");
   }

static uint8_t *bypassBaseAddress(uintptrj_t mutableCallSite, TR_J9VMBase *fe)
   {
   /* The location of the JNI global ref is actually stored in the form of an
      offset from the ramStatics area of whatever object was returned by
      getUnsafe().staticFieldBase(MutableCallSite.class).  This extra complexity
      is necessary because the java code uses Unsafe.putObject to update the
      global ref, so it must have a "base object" and "offset" just as though
      it were a static field.
   */
   uintptrj_t *fieldAddress = (uintptrj_t*)fe->getStaticFieldAddress(
         fe->getObjectClass(mutableCallSite),
         (unsigned char*)"bypassBase", 10, (unsigned char*)"Ljava/lang/Object;", 18
         );
   uintptrj_t bypassBaseObject = *fieldAddress; // Statics are not compressed refs
   TR_OpaqueClassBlock *bypassClass = fe->getClassFromJavaLangClass(bypassBaseObject);
   return (uint8_t*)fe->convertClassOffsetToClassPtr(bypassClass)->ramStatics;
   }

uintptrj_t *
TR_J9VMBase::mutableCallSite_bypassLocation(uintptrj_t mutableCallSite)
   {
   assume(haveAccess(), "mutableCallSite_bypassLocation requires VM access");

   int64_t bypassOffset = getInt64Field(getReferenceField(
      mutableCallSite,
      "globalRefCleaner", "Ljava/lang/invoke/GlobalRefCleaner;"),
      "bypassOffset");
   if (bypassOffset == 0)
      return NULL;

   uint8_t *baseAddress = bypassBaseAddress(mutableCallSite, this);
   bypassOffset &= -2L; // mask off low tag if present
   return (uintptrj_t*)(baseAddress + bypassOffset);
   }

uintptrj_t *
TR_J9VMBase::mutableCallSite_findOrCreateBypassLocation(uintptrj_t mutableCallSite)
   {
   assume(haveAccess(), "mutableCallSite_bypassLocation requires VM access");

   uintptrj_t cleaner   = getReferenceField(mutableCallSite, "globalRefCleaner", "Ljava/lang/invoke/GlobalRefCleaner;");
   uint32_t fieldOffset = getInstanceFieldOffset(getObjectClass(cleaner), "bypassOffset", "J");
   int64_t bypassOffset = getInt64FieldAt(cleaner, fieldOffset);
   if (bypassOffset == 0)
      {
      uintptrj_t target = getReferenceField(mutableCallSite, "target", "Ljava/lang/invoke/MethodHandle;");
      jobject handleRef = vmThread()->javaVM->internalVMFunctions->j9jni_createGlobalRef((JNIEnv*)vmThread(), (j9object_t)target, false);
      uint8_t *baseAddress = bypassBaseAddress(mutableCallSite, this);
      bypassOffset = ((int64_t)handleRef) - ((int64_t)baseAddress);
      bypassOffset |= 1; // Low tag to pretend it's a static field
      if (!compareAndSwapInt64FieldAt(cleaner, fieldOffset, 0, bypassOffset))
         {
         // Another thread beat us to it
         vmThread()->javaVM->internalVMFunctions->j9jni_deleteGlobalRef((JNIEnv*)vmThread(), handleRef, false);
         }
      }
   return mutableCallSite_bypassLocation(mutableCallSite);
   }

static TR_OpaqueMethodBlock *findClosestArchetype(TR_OpaqueClassBlock *clazz, char *name, char *signature, char *currentArgument, TR_FrontEnd *fe, J9VMThread *vmThread)
   {
   // NOTE: signature will be edited in-place

   bool details = TR_Options::getCmdLineOptions()->getVerboseOption(TR_VerboseMethodHandleDetails);

   if (currentArgument[1] == ')')
      {
      assume(!strncmp(currentArgument, "I)", 2), "Must be pointing at the placeholder arg");
      }
   else
      {
      // We're not yet pointing at the last (placeholder) argument,
      // so let's see if we can get away with deleting fewer arguments
      //
      TR_OpaqueMethodBlock *result = findClosestArchetype(clazz, name, signature, nextSignatureArgument(currentArgument), fe, vmThread);
      if (result)
         return result;

      // Otherwise, truncate the argument list
      //
      *currentArgument++ = 'I';
      char *tail = strchr(currentArgument, ')');
      while ((*currentArgument++ = *tail++));
      }

   TR_OpaqueMethodBlock *result = fe->getMethodFromClass(clazz, name, signature);
   if (result)
      {
      TR_OpaqueClassBlock *methodClass = fe->getClassFromMethodBlock(result);
      int32_t methodClassNameLength;
      char *methodClassName = fe->getClassNameChars(methodClass, methodClassNameLength);
      U_16 numTemps = J9_TEMP_COUNT_FROM_ROM_METHOD(J9_ROM_METHOD_FROM_RAM_METHOD((J9Method*)result));
      if (numTemps > 0)
         {
         // Compjazz 201055: We can't yet trust archetypes with locals.
         //
         result = NULL;
         if (details)
           TR_VerboseLog::writeLineLocked(TR_Vlog_MHD, "%p   - Ignoring archetype with local variables %.*s.%s%s", vmThread, methodClassNameLength, methodClassName, name, signature);
         }
      else if (methodClass == clazz)
         {
         if (details)
            TR_VerboseLog::writeLineLocked(TR_Vlog_MHD, "%p   - Found matching archetype %.*s.%s%s", vmThread, methodClassNameLength, methodClassName, name, signature);
         }
      else
         {
         // It's generally dangerous to use an inherited archetype because
         // usually the reason for creating a new MethodHandle class is that
         // its semantics are different, so the inherited archetypes are likely
         // to be unsuitable.  We'd rather revert to interpreter than compile
         // an unsuitable thunk.
         //
         result = NULL;
         if (details)
           TR_VerboseLog::writeLineLocked(TR_Vlog_MHD, "%p   - Ignoring inherited archetype %.*s.%s%s", vmThread, methodClassNameLength, methodClassName, name, signature);
         }
      }
   return result;
   }

TR_OpaqueMethodBlock *
TR_J9VMBase::lookupArchetype(TR_OpaqueClassBlock *clazz, char *name, char *signature)
   {
   // Find the best match for the signature.  Start by appending an "I"
   // placeholder argument.  findClosestArchetype will progressively truncate
   // the other arguments until the best match is found.
   //
   char *truncatedSignature = (char*)alloca(strlen(signature)+2); // + 'I' + null terminator
   strcpy(truncatedSignature, signature);
   char toInsert = 'I';
   char *cur;
   for (cur = strrchr(truncatedSignature, ')'); toInsert; cur++)
      {
      char toInsertNext = *cur;
      *cur = toInsert;
      toInsert = toInsertNext;
      }
   *cur = 0;
   return findClosestArchetype(clazz, name, truncatedSignature, truncatedSignature+1, this, getCurrentVMThread());
   }

TR_OpaqueMethodBlock *
TR_J9VMBase::lookupMethodHandleThunkArchetype(uintptrj_t methodHandle)
   {
   assume(haveAccess(), "methodHandle_jitInvokeExactThunk requires VM access");

   // Compute thunk's asignature and archetype's name
   //
   uintptrj_t thunkableSignatureString = methodHandle_thunkableSignature((uintptrj_t)methodHandle);
   intptrj_t  thunkableSignatureLength = getStringUTF8Length(thunkableSignatureString);
   char *thunkSignature = (char*)alloca(thunkableSignatureLength+1);
   getStringUTF8(thunkableSignatureString, thunkSignature, thunkableSignatureLength+1);

   char *archetypeSpecimenSignature = (char*)alloca(thunkableSignatureLength+20);
   strcpy(archetypeSpecimenSignature, thunkSignature);
   char *returnType = (1+strchr(archetypeSpecimenSignature, ')'));
   switch (returnType[0])
      {
      case '[':
      case 'L':
         // The thunkable signature might return some other class, but archetypes
         // returning a reference are always declared to return Object.
         //
         sprintf(returnType, "Ljava/lang/Object;");
         break;
      }
   char methodName[50];
   sprintf(methodName, "invokeExact_thunkArchetype_%c", returnType[0]);

   TR_OpaqueMethodBlock *result = lookupArchetype(getObjectClass((uintptrj_t)methodHandle), methodName, archetypeSpecimenSignature);
   if (!result)
      {
      strcpy(returnType, "I");
      result = lookupArchetype(getObjectClass((uintptrj_t)methodHandle), "invokeExact_thunkArchetype_X", archetypeSpecimenSignature);
      }
   return result;
   }

TR_ResolvedMethod *
TR_J9VMBase::createMethodHandleArchetypeSpecimen(TR_Memory *trMemory, TR_OpaqueMethodBlock *archetype, uintptrj_t *methodHandleLocation, TR_ResolvedMethod *owningMethod)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   assume(archetype, "Explicitly provided archetype must not be null");
   assume(archetype == lookupMethodHandleThunkArchetype(*methodHandleLocation), "Explicitly provided archetype must be the right one");
   uintptrj_t signatureString = getReferenceField(getReferenceField(
      *methodHandleLocation,
      "thunks",             "Ljava/lang/invoke/ThunkTuple;"),
      "thunkableSignature", "Ljava/lang/String;");
   intptrj_t length = getStringUTF8Length(signatureString);
   char *thunkableSignature = (char*)trMemory->allocateStackMemory(length+1);
   getStringUTF8(signatureString, thunkableSignature, length+1);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   TR_ResolvedMethod *result = createResolvedMethodWithSignature(trMemory, archetype, NULL, thunkableSignature, length, owningMethod);
   result->convertToMethod()->setArchetypeSpecimen();
   result->setMethodHandleLocation(methodHandleLocation);
   return result;
   }

TR_ResolvedMethod *
TR_J9VMBase::createMethodHandleArchetypeSpecimen(TR_Memory *trMemory, uintptrj_t *methodHandleLocation, TR_ResolvedMethod *owningMethod)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   TR_OpaqueMethodBlock *archetype = lookupMethodHandleThunkArchetype(*methodHandleLocation);
   TR_ResolvedMethod *result;
   if (archetype)
      result = createMethodHandleArchetypeSpecimen(trMemory, archetype, methodHandleLocation, owningMethod);
   else
      result = NULL;
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

uintptrj_t TR_J9VMBase::mutableCallSiteCookie(uintptrj_t mutableCallSite, uintptrj_t potentialCookie)
   {
   uintptrj_t result=0;
   if (potentialCookie && compareAndSwapInt64Field(mutableCallSite, "invalidationCookie", 0, potentialCookie))
      result =  potentialCookie;
   else
      result = (uintptrj_t)getInt64Field(mutableCallSite, "invalidationCookie");
   return result;
   }

bool
TR_J9VMBase::hasMethodTypesSideTable()
   {
#if defined(J9VM_OPT_REMOVE_CONSTANT_POOL_SPLITTING)
   return true;
#else
   return false;
#endif
   }

void *
TR_J9VMBase::methodHandle_jitInvokeExactThunk(uintptrj_t methodHandle)
   {
   assume(haveAccess(), "methodHandle_jitInvokeExactThunk requires VM access");
   return (void*)(intptrj_t)getInt64Field(getReferenceField(
      methodHandle,
      "thunks", "Ljava/lang/invoke/ThunkTuple;"),
      "invokeExactThunk");
   }

bool
TR_J9VMBase::getStringFieldByName(TR_Compilation * comp, TR_SymbolReference * stringRef, TR_SymbolReference * fieldRef, void * & pResult)
   {
   bool haveAcquiredVMAccess;
   if (!tryToAcquireAccess(comp, &haveAcquiredVMAccess))
      return false;

   assume(!stringRef->isUnresolved(), "don't handle unresolved constant strings yet");

   j9object_t string = *(j9object_t*)stringRef->getSymbol()->castToStaticSymbol()->getStaticAddress();


   TR_Symbol::RecognizedField   field = fieldRef->getSymbol()->getRecognizedField();


   if (field == TR_Symbol::Java_lang_String_count)
      pResult = (U_8*)string + J9VMJAVALANGSTRING_LENGTH_OFFSET(jitConfig->javaVM);
   else if (field == TR_Symbol::Java_lang_String_offset)
      pResult = (U_8*)string + J9VMJAVALANGSTRING_OFFSET_OFFSET(jitConfig->javaVM);
   else if (field == TR_Symbol::Java_lang_String_hashCode)
      {
      if (J9VMJAVALANGSTRING_HASHCODE(jitConfig->javaVM, string) == 0)
         {
         // If not already computed, compute and clobber
         //
         int32_t sum   = 0;
         int32_t scale = 1;

         for (int32_t i = J9VMJAVALANGSTRING_LENGTH(jitConfig->javaVM, string) - 1; i >= 0; --i, scale *= 31)
            {
            uint16_t thisChar = getStringCharacter((uintptrj_t)string, i);
            sum += thisChar * scale;
            }

         J9VMJAVALANGSTRING_SET_HASHCODE(jitConfig->javaVM, string, sum);
         }
      pResult = (U_8*)string + J9VMJAVALANGSTRING_HASHCODE_OFFSET(jitConfig->javaVM);
      }
   else if (field == TR_Symbol::Java_lang_String_value)
      pResult = (U_8*)string + J9VMJAVALANGSTRING_BYTES_OFFSET(jitConfig->javaVM);
   else
      {
      if (haveAcquiredVMAccess)
         releaseAccess(comp);
      return false;
      }

   if (haveAcquiredVMAccess)
      releaseAccess(comp);
   return true;
   }

uintptrj_t
TR_J9VMBase::getFieldOffset(TR_Compilation * comp, TR_SymbolReference* classRef, TR_SymbolReference* fieldRef)
   {
   TR_ResolvedMethod* method = classRef->getOwningMethod(comp);
   TR_StaticSymbol* classSym = classRef->getSymbol()->castToStaticSymbol();
   J9VMJavaLangString* classString = *(J9VMJavaLangString **)classSym->getStaticAddress();
   TR_StaticSymbol* fieldSym = fieldRef->getSymbol()->castToStaticSymbol();
   J9VMJavaLangString* fieldString = *(J9VMJavaLangString **)fieldSym->getStaticAddress();

   int32_t len, res;
   len = (int32_t)jitConfig->javaVM->internalVMFunctions->getStringUTF8Length(jitConfig->javaVM, classString);
   U_8* u8ClassString = (U_8*)comp->trMemory()->allocateStackMemory(len + 1);
   res = (int32_t)jitConfig->javaVM->internalVMFunctions->copyStringToUTF8(jitConfig->javaVM, classString, J9_STR_XLAT, u8ClassString, len+1);

   /**
   //fprintf(stderr,"name is (res is %d) classString is %p\n",res, classString); fflush(stderr);
   for (int i =0; i<len; i++)
      {
      fprintf(stderr,"%c",u8ClassString[i]);
      }
   fprintf(stderr,"  (len is %d)\n",len);fflush(stderr);
   **/

   if (res) return 0;
   char* classSignature = classNameToSignature((char*)u8ClassString, len, comp);

   /**
   fprintf(stderr,"classSignature is \n");
   for (int i =0; i <len; i++){
      fprintf(stderr,"%c",classSignature[i]);
   }
   fprintf(stderr,"  (len is %d)\n",len);
   **/

   TR_OpaqueClassBlock * j9ClassPtr = comp->fe()->getClassFromSignature(classSignature, len, method);
   //fprintf(stderr,"Class looked up to be %p \n", j9ClassPtr);

   if (!j9ClassPtr) return 0;

   TR_VMFieldsInfo fields(comp, (J9Class*)j9ClassPtr, 1);

   len = (int32_t)jitConfig->javaVM->internalVMFunctions->getStringUTF8Length(jitConfig->javaVM, fieldString);
   U_8* u8FieldString = (U_8*)comp->trMemory()->allocateStackMemory(len + 1);
   res = (int32_t)jitConfig->javaVM->internalVMFunctions->copyStringToUTF8(jitConfig->javaVM, fieldString, 0, u8FieldString, len+1);

   ListIterator<TR_VMField> itr(fields.getFields()) ;
   TR_VMField* field;
   uint32_t offset = 0;
   for (field = itr.getFirst(); field != NULL; field= itr.getNext())
      {
      // fprintf(stderr, "fieldName %s fieldOffset %d fieldSig %s\n",field->name, field->offset, field->signature);
      if (!strncmp(field->name, (const char*)u8FieldString, len+1))
         {
         offset = (uint32_t)(field->offset + getObjectHeaderSizeInBytes());
         // Do we Need this?
         // offset = getInstanceFieldOffset(j9ClassPtr, field->name, strlen(field->name), field->signature, strlen(field->signature),
         //                               J9_RESOLVE_FLAG_NO_THROW_ON_FAIL);

         // fprintf(stderr,">>>>> Offset for %s determined to be : %d\n", field->name,offset);
         return (uintptrj_t)offset;
         }
      }

   void * staticAddr = 0;
   itr = fields.getStatics() ;
   for (field = itr.getFirst(); field != NULL; field=itr.getNext())
      {
      // fprintf(stderr, "staticName %s staticSig %s\n",field->name, field->signature);
      if (!strncmp(field->name, (const char*)u8FieldString, len+1))
         {

         // Do we Need to acquire VM Access? getInstanceFieldOffset does it?
         bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
         staticAddr = jitConfig->javaVM->internalVMFunctions->staticFieldAddress(_vmThread,
                          (J9Class*)j9ClassPtr, u8FieldString, len,  (U_8*)field->signature, (UDATA)strlen(field->signature),
                          NULL, NULL, J9_RESOLVE_FLAG_NO_THROW_ON_FAIL, NULL);
         releaseVMAccessIfNeeded(haveAcquiredVMAccess);
         // fprintf(stderr,">>>>> Static addr for %s determined to be : %p\n", field->name, staticAddr);
         }
      }


   return (uintptrj_t)staticAddr;
   }

bool
TR_J9VMBase::isString(TR_OpaqueClassBlock *clazz)
   {
   return (J9Class*)clazz == J9VMJAVALANGSTRING(jitConfig->javaVM);
   }

bool
TR_J9VMBase::isString(uintptrj_t objectPointer)
   {
   assume(haveAccess(), "isString requires VM access");
   return isString(getObjectClass(objectPointer));
   }

int32_t
TR_J9VMBase::getStringLength(uintptrj_t objectPointer)
   {
   assume(haveAccess(), "getStringLength requires VM access");
   return J9VMJAVALANGSTRING_LENGTH(jitConfig->javaVM, (j9object_t)objectPointer);
   }

int32_t
TR_J9VMBase::getStringOffsetWithinCharArray(uintptrj_t objectPointer)
   {
   assume(haveAccess(), "getStringOffsetWithinCharArray requires VM access");
   return J9VMJAVALANGSTRING_OFFSET(jitConfig->javaVM, (j9object_t)objectPointer);
   }

uint16_t
TR_J9VMBase::getStringCharacter(uintptrj_t objectPointer, int32_t index)
   {
   assume(haveAccess(), "getStringCharacter requires VM access");

   j9object_t string = (j9object_t)objectPointer;
   uintptrj_t dataStart = (uintptrj_t)J9VMJAVALANGSTRING_BYTES(jitConfig->javaVM, string);

   bool hasSpine = canGenerateArraylets() && !useHybridArraylets();

   // Check string length in bytes.
   //
   int32_t arrayLengthInElements = getArrayLengthInElements(dataStart);
   if (canGenerateArraylets() && useHybridArraylets() && isDiscontiguousArray(arrayLengthInElements * sizeof(int16_t)))
      {
      dataStart += sizeof(J9IndexableObjectDiscontiguous);
      hasSpine = true;
      }
   else
      {
      dataStart += sizeof(J9IndexableObjectContiguous);
      }

   int32_t indexInArray = index + getStringOffsetWithinCharArray(objectPointer);

   if (hasSpine)
      {
      uint16_t *arraylet;
#if defined(J9VM_GC_COMPRESSED_POINTERS)
      uint32_t *spine = (uint32_t*)dataStart;
      uintptrj_t compressedArrayletPtr = (uintptrj_t)(spine[indexInArray >> getArraySpineShift(2)]);
      arraylet = (uint16_t*)((compressedArrayletPtr << getCompressedPointersShift()) + getHeapBaseAddress());
#else
      uintptrj_t *spine = (uintptrj_t*)dataStart;
      arraylet = (uint16_t *)(spine[indexInArray >> getArraySpineShift(2)]);
#endif
      return arraylet[indexInArray & getArrayletMask(2)];
      }
   else
      {
      uint16_t *charArray = (uint16_t*)dataStart;
      return charArray[indexInArray];
      }

   }

intptrj_t
TR_J9VMBase::getStringUTF8Length(uintptrj_t objectPointer)
   {
   assume(haveAccess(), "Must have VM access to call getStringUTF8Length");
   assume0(objectPointer);
   return vmThread()->javaVM->internalVMFunctions->getStringUTF8Length(vmThread()->javaVM, (J9VMJavaLangString*)objectPointer);
   }

char *
TR_J9VMBase::getStringUTF8(uintptrj_t objectPointer, char *buffer, intptrj_t bufferSize)
   {
   assume(haveAccess(), "Must have VM access to call getStringAscii");
   assume0(objectPointer && buffer);
   assume(bufferSize >= 1+getStringUTF8Length(objectPointer), "getStringUTF8 buffer must be large enough");
   UDATA end = vmThread()->javaVM->internalVMFunctions->copyFromStringIntoUTF8(vmThread()->javaVM, (J9VMJavaLangString*)objectPointer, buffer);
   buffer[end] = 0;
   return buffer;
   }

// set a 32 bit field that will be printed if the VM crashes
// typically, this should be used to represent the state of the
// compilation
//
void
TR_J9VMBase::reportILGeneratorPhase()
   {
   if (!_vmThread)
      return;

   enum { DEFAULT_LOW_BYTE=0x80 };

   vmThread()->vmState = J9VMSTATE_JIT_CODEGEN | (DEFAULT_LOW_BYTE & 0xFF);
   }

void
TR_J9VMBase::reportOptimizationPhase(Optimizations opts)
   {
   if (!_vmThread)
      return;

   vmThread()->vmState = J9VMSTATE_JIT_CODEGEN | ((((int32_t) opts & 0xFF) << 8)|0xFF);
   }

void
TR_J9VMBase::reportOptimizationPhaseForSnap(Optimizations opts)
   {
   if (!_vmThread)
      return;

   if (TrcEnabled_Trc_JIT_optimizationPhase)
      Trc_JIT_optimizationPhase(vmThread(), TR_OptimizerImpl::getOptimizationName(opts));
   }

void
TR_J9VMBase::reportCodeGeneratorPhase(TR_CodeGeneratorPhase codegenPhase)
   {
   if (!_vmThread)
      return;

   vmThread()->vmState = J9VMSTATE_JIT_CODEGEN | codegenPhase | 0xFF00;

   if (TrcEnabled_Trc_JIT_codeGeneratorPhase)
      Trc_JIT_codeGeneratorPhase(vmThread(), TR_CodeGenerator::getCodeGeneratorPhaseName(codegenPhase));
   }
   
void
TR_J9VMBase::reportPrexInvalidation(void * startPC)
   {
   if (!_vmThread)
      return;
   // Generate a trace point
   Trc_JIT_MethodPrexInvalidated(vmThread(), startPC);
   }
   

void
TR_J9VMBase::traceAssumeFailure(int32_t line, char * file)
   {
   //Trc_JIT_assumeFailure(vmThread(), line, file);
   }


// Math functions
float
TR_J9VMBase::floatAddFloat(float a, float b)
   {
   return helperCFloatPlusFloat(a, b);
   }

float
TR_J9VMBase::floatSubtractFloat(float a, float b)
   {
   return helperCFloatMinusFloat(a, b);
   }

float
TR_J9VMBase::floatMultiplyFloat(float a, float b)
   {
   return helperCFloatMultiplyFloat(a, b);
   }

float
TR_J9VMBase::floatDivideFloat(float a, float b)
   {
   return helperCFloatDivideFloat(a, b);
   }
float
TR_J9VMBase::floatRemainderFloat(float a, float b)
   {
   return helperCFloatRemainderFloat(a, b);
   }

float
TR_J9VMBase::floatNegate(float a)
   {
   float c;
   helperNegateFloat(&a, &c);
   return c;
   }

double
TR_J9VMBase::doubleAddDouble(double a, double b)
   {
   return helperCDoublePlusDouble(a, b);
   }

double
TR_J9VMBase::doubleSubtractDouble(double a, double b)
   {
   return helperCDoubleMinusDouble(a, b);
   }

double
TR_J9VMBase::doubleMultiplyDouble(double a, double b)
   {
   return helperCDoubleMultiplyDouble(a, b);
   }

double
TR_J9VMBase::doubleDivideDouble(double a, double b)
   {
   return helperCDoubleDivideDouble(a, b);
   }
double
TR_J9VMBase::doubleRemainderDouble(double a, double b)
   {
   return helperCDoubleRemainderDouble(a, b);
   }

double
TR_J9VMBase::doubleNegate(double a)
   {
   double c;
   helperNegateDouble(&a, &c);
   return c;
   }

double
TR_J9VMBase::floatToDouble(float a)
   {
   return helperCConvertFloatToDouble(a);
   }

float
TR_J9VMBase::doubleToFloat(double a)
   {
   return helperCConvertDoubleToFloat(a);
   }

int64_t
TR_J9VMBase::longRemainderLong(int64_t a, int64_t b)
   {
   return helperCLongRemainderLong(a, b);
   }

int64_t
TR_J9VMBase::longDivideLong(int64_t a, int64_t b)
   {
   return helperCLongDivideLong(a, b);
   }

int64_t
TR_J9VMBase::longMultiplyLong(int64_t a, int64_t b)
   {
   return helperCLongMultiplyLong(a, b);
   }


// Multiple codeCache support

void
TR_J9VMBase::setHasFailedCodeCacheAllocation()
   {
   if (!_compInfo->getRampDownMCT())
      {
      _compInfo->setRampDownMCT();
      if (TR_Options::getCmdLineOptions()->getVerboseOption(TR_VerboseCompilationThreads))
         {
         TR_VerboseLog::writeLineLocked(TR_Vlog_INFO,"t=%u setRampDownMCT", (uint32_t)_compInfo->getPersistentInfo()->getElapsedTime());
         }
      }
   }

void *
TR_J9VMBase::getDesignatedCodeCache(TR_Hotness hotness, TR_Compilation *comp) // MCT
   {
   int32_t numReserved;
   int32_t compThreadID = comp ? comp->getCompThreadID() : -1;
   assume(comp->trMemory()->getCompilationInfoPerThread()==_compInfoPT || _compInfo->getCompInfoForCompOnAppThread(), "discrepancy in getDesignatedCodeCache");

   bool hadClassUnloadMonitor;
   bool hadVMAccess = releaseClassUnloadMonitorAndAcquireVMaccessIfNeeded(comp, &hadClassUnloadMonitor);

   void * preferred = NULL;
#ifdef TR_HOST_POWER
   // Try to place this method in the same codecache as it's most frequent caller
   TR_PersistentMethodInfo *methodInfo = TR_PersistentMethodInfo::get(comp);
   if (methodInfo && methodInfo->_sampledCallers)
      {
      TR_SampledCallerInfo *sampledCallerInfo = methodInfo->_sampledCallers;
      TR_SampledCallerInfo *hottestCaller = sampledCallerInfo;
      uint32_t              hottestCallerNumSamples = hottestCaller->_samples;
      while (sampledCallerInfo)
         {
         if (sampledCallerInfo->_samples > hottestCallerNumSamples)
            {
            hottestCaller = sampledCallerInfo;
            hottestCallerNumSamples = sampledCallerInfo->_samples;
            }
         sampledCallerInfo = sampledCallerInfo->getNext();
         }
      preferred = ((J9Method *)hottestCaller->_methodInfo->getMethodInfo())->extra;

      if (TR_Options::getCmdLineOptions()->getOption(TR_VerboseHWProfiling))
         {
         J9Method *j9method = (J9Method *)hottestCaller->_methodInfo->getMethodInfo();
         J9UTF8   *className = J9ROMCLASS_CLASSNAME(J9_CLASS_FROM_METHOD(j9method)->romClass);
         J9UTF8   *methName = J9ROMMETHOD_NAME(J9_ROM_METHOD_FROM_RAM_METHOD(j9method));
         J9UTF8   *methSig  = J9ROMMETHOD_SIGNATURE(J9_ROM_METHOD_FROM_RAM_METHOD(j9method));
         TR_VerboseLog::writeLineLocked(TR_Vlog_HWPROFILER, "Compiling %s, placing together with hottest caller %.*s.%.*s%.*s (%u samples), preferred=%p.",
                                        comp->signature(),
                                        J9UTF8_LENGTH(className), utf8Data(className),
                                        J9UTF8_LENGTH(methName), utf8Data(methName),
                                        J9UTF8_LENGTH(methSig), utf8Data(methSig),
                                        hottestCallerNumSamples, preferred);
         }
      }
#endif
   void * result = (void *)TR_MCCManager::reserveCodeCache(hotness, false, 0, compThreadID, &numReserved, preferred);
#ifdef TR_HOST_POWER
   if (TR_Options::getCmdLineOptions()->getOption(TR_VerboseHWProfiling) && preferred && result)
      TR_VerboseLog::writeLineLocked(TR_Vlog_HWPROFILER, "%s placed in preferred codecache.", comp->signature());
#endif

   acquireClassUnloadMonitorAndReleaseVMAccessIfNeeded(comp, hadVMAccess, hadClassUnloadMonitor);
   if (!result)
      {
      // If this is a temporary condition due to all code caches being reserved for the moment
      // we should retry this compilation
      if (!(jitConfig->runtimeFlags & J9JIT_CODE_CACHE_FULL))
         {
         // If this is a temporary condition due to all code caches being reserved for the moment
         // we should retry this compilation
         if (numReserved > 0)
            {
            // set an error code so that the compilation is retried
            if (comp && !comp->isWCodeMethod())
               {
               comp->setErrorCode(COMPILATION_ALL_CODE_CACHES_RESERVED);
               j9OutOfMemory(_jitConfig, comp, "Cannot reserve code cache");
               }
            }
         }
      }
   return result;
   }

void *
TR_J9VMBase::getCCPreLoadedCodeAddress(void *codeCache, TR_CCPreLoadedCode h, TR_CodeGenerator *cg)
   {
   TR_MCCCodeCache *mcc = (TR_MCCCodeCache *)codeCache;
   return mcc->getCCPreLoadedCodeAddress(h, cg);
   }

void
TR_J9VMBase::reserveTrampolineIfNecessary(TR_Compilation * comp, TR_SymbolReference * symRef, bool inBinaryEncoding)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded(); // TODO: move this down as much as possible
   TR_MCCCodeCache *curCache = static_cast<TR_MCCCodeCache *>(comp->getCurrentCodeCache());
   bool isRecursive = false;

   if (NULL == curCache)
      {
      releaseVMAccessIfNeeded(haveAcquiredVMAccess);

      if (isAOT())
         comp->setErrorCode(COMPILATION_OUT_OF_MEMORY_RELOCATION_DATA);
#ifdef MCT_DEBUG
      fprintf(stderr, "Aborting compilation no space for trampoline area %p\n", comp);
#endif
      outOfMemory(comp, "code");
      }

   assume0(curCache->isReserved()); // MCT

   if (!symRef->isUnresolved() && !comp->isDLT())
      {
      TR_ResolvedMethod *resolvedMethod = symRef->getSymbol()->castToResolvedMethodSymbol()->getResolvedMethod();
      isRecursive = resolvedMethod->isSameMethod(comp->getCurrentMethod());
      }

   TR_MCCCodeCache *newCache = curCache; // optimistically assume that we will manage to allocate trampoline from current code cache
   if (isAOT() && isRecursive)
      {
      J9JITDataCacheHeader *aotMethodHeader = (J9JITDataCacheHeader *)comp->getAotMethodDataStart();
      TR_AOTMethodHeader *aotMethodHeaderEntry =  (TR_AOTMethodHeader *)(aotMethodHeader + 1);
      aotMethodHeaderEntry->flags |= TR_AOTMethodHeader_NeedsRecursiveMethodTrampolineReservation; // Set flag in TR_AOTMethodHeader
      //newCache = curCache; // done above
      }
   else if (symRef->isUnresolved() || isAOT())
      {
      void *cp = (void *)symRef->getOwningMethod(comp)->constantPool();
      I_32 cpIndex = symRef->getCPIndex();

#if 0
      if (isAOT() && (comp->getOption(TR_TraceRelocatableDataCG) || comp->getOption(TR_TraceRelocatableDataDetailsCG)) )
         {
          trace0c(comp, "<relocatableDataTrampolinesCG>\n");
          trace1c(comp, "%s\n", comp->signature());
          trace1c(comp, "%-8s", "cpIndex");
          trace0c(comp, "cp\n");
          trace1c(comp, "%-8x", cpIndex);
          trace1c(comp, "%x\n", cp);
          trace0c(comp, "</relocatableDataTrampolinesCG>\n");
         }
#endif

      bool hadClassUnloadMonitor;
      bool hadVMAccess = releaseClassUnloadMonitorAndAcquireVMaccessIfNeeded(comp, &hadClassUnloadMonitor);

      int32_t retValue = curCache->reserveUnresolvedTrampoline(cp, cpIndex, !isAOT());
      if (retValue != TR_MCCErrorCode::ERRORCODE_SUCCESS)
         {
         // We couldn't allocate trampoline in this code cache
         curCache->unreserve(); // delete the old reservation
         if (retValue == TR_MCCErrorCode::ERRORCODE_INSUFFICIENTSPACE && !inBinaryEncoding) // code cache full, allocate a new one
            {
            // Allocate a new code cache and try again
            newCache = TR_MCCManager::getNewCodeCache(comp->getCompThreadID(), comp->getDeFactoHotness(), isAOT()); // class unloading may happen here
            if (newCache)
               {
               // check for class unloading that can happen in getNewCodeCache
               if (((TR_CompilationInfoPerThreadBase*)(comp->trMemory()->getCompilationInfoPerThread()))->compilationShouldBeInterrupted())
                  {
                  ((TR_MCCCodeCache*)newCache)->unreserve(); // delete the reservation
                  newCache = NULL; // This will fail the compilation
                  comp->setErrorCode(COMPILATION_INTERRUPTED); // Set the error code so compilation could be retried
                  }
               else
                  {
                  retValue = ((TR_MCCCodeCache*)newCache)->reserveUnresolvedTrampoline(cp, cpIndex, !isAOT());
                  if (retValue != TR_MCCErrorCode::ERRORCODE_SUCCESS)
                     {
                     ((TR_MCCCodeCache*)newCache)->unreserve(); // delete the reservation
                     newCache = NULL; // This will fail the compilation
                     comp->setErrorCode(COMPILATION_RESERVE_UNRESOLVED_TRAMPOLINE_FATAL_ERROR);
                     }
                  }
               }
            else // cannot allocate a new code cache
               {
               comp->setErrorCode(COMPILATION_RESERVE_UNRESOLVED_TRAMPOLINE_INSUFFICIENT_SPACE);
               }
            }
         else
            {
            newCache = 0;
            if (inBinaryEncoding)
               comp->setErrorCode(COMPILATION_RESERVE_UNRESOLVED_TRAMPOLINE_ERROR_INBINARYENCODING); // RAS only
            else
               comp->setErrorCode(COMPILATION_RESERVE_UNRESOLVED_TRAMPOLINE_ERROR); // RAS only
            }
         }
      acquireClassUnloadMonitorAndReleaseVMAccessIfNeeded(comp, hadVMAccess, hadClassUnloadMonitor);
      }
   else // asking for resolved trampoline
      {
      newCache = getResolvedTrampoline(comp, curCache, (J9Method *)symRef->getSymbol()->castToResolvedMethodSymbol()->getResolvedMethod()->getPersistentIdentifier(), inBinaryEncoding);
      }
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);

   if (newCache != curCache)
      {
      switchCodeCache(comp, curCache, newCache);
      }
   assume0(newCache->isReserved()); // MCT
   }

intptrj_t
TR_J9VMBase::indexedTrampolineLookup(int32_t helperIndex, void * callSite)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   void * tramp = (void *)TR_MCCManager::findHelperTrampoline(callSite, helperIndex);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   assume(tramp!=NULL, "Error: CodeCache is not initialized properly.\n");
   return (intptrj_t)tramp;
   }

void
TR_J9VMBase::reserveNTrampolines(TR_Compilation * comp, int32_t n, bool inBinaryEncoding)
   {
#if defined(TR_TARGET_X86) && defined(TR_TARGET_64BIT)
   // Don't do anything if method trampolines are not needed
   if (!needsMethodTrampolines())
      return; 
   bool hadClassUnloadMonitor;
   bool hadVMAccess = releaseClassUnloadMonitorAndAcquireVMaccessIfNeeded(comp, &hadClassUnloadMonitor);

   TR_MCCCodeCache *curCache = static_cast<TR_MCCCodeCache *>(comp->getCurrentCodeCache());
   TR_MCCCodeCache *newCache = curCache;
   TR_MCCErrorCode::ErrorCode status = TR_MCCErrorCode::ERRORCODE_SUCCESS;

   assume0(curCache->isReserved()); // MCT

   if (!isAOT())
      {
      status = curCache->reserveNTrampolines(n);
      if (status != TR_MCCErrorCode::ERRORCODE_SUCCESS)
         {
         // Current code cache is no good. Must unreserve
         curCache->unreserve();
         newCache = 0;
         if (!inBinaryEncoding)
            {
            newCache = TR_MCCManager::getNewCodeCache(comp->getCompThreadID(), comp->getDeFactoHotness(), isAOT());
            if (newCache)
               {
               status = newCache->reserveNTrampolines(n);
               assume(status == TR_MCCErrorCode::ERRORCODE_SUCCESS, "Failed to reserve trampolines in fresh code cache.");
               }
            }
         }
      }

   acquireClassUnloadMonitorAndReleaseVMAccessIfNeeded(comp, hadVMAccess, hadClassUnloadMonitor);
   
   if (!newCache)
      {
      comp->setErrorCode(COMPILATION_RESERVE_NTRAMPOLINES_INSUFFICIENT_SPACE);
      }

   if (newCache != curCache)
      {
      // We keep track of number of IPIC trampolines that are present in the current code cache
      // If the code caches have been switched we have to reset this number, the setCodeCacheSwitched helper called
      // in switchCodeCache resets the count
      // If we are in binaryEncoding we will kill this compilation anyway
      switchCodeCache(comp, curCache, newCache);
      }
   else
      {
      comp->setNumReservedIPICTrampolines(comp->getNumReservedIPICTrampolines()+n);
      }

   assume0(newCache->isReserved());
#endif
   }

bool TR_J9VMBase::needsMethodTrampolines()
   {
   return TR_MCCManager::getMCCManager()->needsMethodTrampolines();
   }

// Interface Method Table support
//
bool
TR_J9VMBase::canUseIMT()
   {
   return false; //getIMTBuilder()->canBuildIMT();
   }

TR_IMTBuilder *
TR_J9VMBase::getIMTBuilder()
   {
   return 0;
   #if 0
   if (!_imtBuilder)
      {
      if (!((TR_JitPrivateConfig*)(jitConfig->privateConfig))->imtBuilder)
         ((TR_JitPrivateConfig*)(jitConfig->privateConfig))->imtBuilder = TR_IMTBuilder::allocate(jitConfig);
      _imtBuilder = ((TR_JitPrivateConfig*)(jitConfig->privateConfig))->imtBuilder;
      }
   return _imtBuilder;
   #endif
   }

int32_t
TR_J9VMBase::getIMTSize()
   {
   return 0;//getIMTBuilder()->getIMTSize();
   }

int32_t
TR_J9VMBase::getIMTOffset()
   {
   return 0;//getIMTBuilder()->getIMTOffset();
   }

int32_t
TR_J9VMBase::getMinIITSize()
   {
   return 0;//getIMTBuilder()->getMinIITSize();
   }

int32_t
TR_J9VMBase::getIITOffset()
   {
   return 0;//getIMTBuilder()->getIITOffset();
   }

int32_t
TR_J9VMBase::getIMTSlotOffset(int32_t index)
   {
   return 0;//getIMTBuilder()->getIMTSlotOffset(index);
   }

int32_t
TR_J9VMBase::getInterfaceIndex(TR_OpaqueClassBlock *cl)
   {
   return 0;//getIMTBuilder()->getInterfaceIndex(cl);
   }

int32_t
TR_J9VMBase::getInterfaceMethodIndex(void *constantPool, int32_t cpIndex)
   {
   return 0;//getIMTBuilder()->getInterfaceMethodIndex(constantPool, cpIndex);
   }

int32_t
TR_J9VMBase::getInterfaceMethodIndex(TR_OpaqueMethodBlock *method)
   {
   return 0;//getIMTBuilder()->getInterfaceMethodIndex(method);
   }

// interpreter profiling support
TR_IProfiler *
TR_J9VMBase::getIProfiler()
   {
   return NULL;
   }

bool
TR_J9VMBase::isClassLibraryMethod(TR_OpaqueMethodBlock *method, bool vettedForAOT)
   {
   if (!_vmThread || !_vmThread->javaVM)
      return false;
   J9ClassLoader *classLoader = convertClassOffsetToClassPtr((TR_OpaqueClassBlock*)J9_CLASS_FROM_METHOD(((J9Method *)method)))->classLoader;
   if (((J9JavaVM *)_vmThread->javaVM)->systemClassLoader == classLoader)
      return true;

   return false;
   }

//currently suppoorted in x86 codegen
bool
TR_J9VMBase::getSupportsRecognizedMethods()
   {
   return  !isAOT() || isX86(TR_Options::getCmdLineOptions()->getTarget());
   }


int32_t
TR_J9VMBase::getMaxCallGraphCallCount()
   {
   TR_IProfiler *profiler = getIProfiler();

   if (!profiler)
      return -1;

   return profiler->getMaxCallCount();
   }

int32_t
TR_J9VMBase::getIProfilerMaxCount()
   {
   TR_IProfiler *profiler = getIProfiler();
   if (profiler)
      return profiler->getMaxCount(isAOT());

   return -1;
   }

void
TR_J9VMBase::getNumberofCallersAndTotalWeight(TR_OpaqueMethodBlock *method, uint32_t *count, uint32_t *weight)
   {
   TR_IProfiler *profiler = getIProfiler();

   if (!profiler)
   {
      //printf("No Profiler!\n");
      return; // -1;
   }

   profiler->getNumberofCallersAndTotalWeight(method,count,weight);
   return;
   }

uint32_t
TR_J9VMBase::getOtherBucketWeight(TR_OpaqueMethodBlock *method)
   {
   TR_IProfiler *profiler = getIProfiler();

   if (!profiler)
   {
      //printf("No Profiler!\n");
      return 0; // -1;
   }

   return profiler->getOtherBucketWeight(method);
   }

bool
TR_J9VMBase::getCallerWeight(TR_OpaqueMethodBlock *calleeMethod, TR_OpaqueMethodBlock *callerMethod , uint32_t *weight)
   {
   TR_IProfiler *profiler = getIProfiler();

   if (!profiler)
      return 0;

   return profiler->getCallerWeight(calleeMethod,callerMethod, weight);
   }
bool
TR_J9VMBase::getCallerWeight(TR_OpaqueMethodBlock *calleeMethod, TR_OpaqueMethodBlock *callerMethod , uint32_t *weight, uint32_t pcIndex)
   {
   TR_IProfiler *profiler = getIProfiler();

   if (!profiler)
      return 0;

   return profiler->getCallerWeight(calleeMethod,callerMethod, weight, pcIndex);
   }


int32_t
TR_J9VMBase::getIProfilerCallCount(TR_OpaqueMethodBlock *caller, int32_t bcIndex, TR_Compilation * comp)
   {
   TR_IProfiler *profiler = getIProfiler();
   if (profiler)
      return profiler->getCallCount(caller, bcIndex, comp);

   return -1;
   }

int32_t
TR_J9VMBase::getIProfilerCallCount(TR_ByteCodeInfo &bcInfo, TR_Compilation *comp)
   {
   TR_IProfiler *profiler = getIProfiler();
   if (profiler)
      return profiler->getCallCount(bcInfo, comp);

   return 0;
   }

void
TR_J9VMBase::setIProfilerCallCount(TR_OpaqueMethodBlock *caller, int32_t bcIndex, int32_t count, TR_Compilation * comp)
   {
   TR_IProfiler *profiler = getIProfiler();
   if (profiler)
      profiler->setCallCount(caller, bcIndex, count, comp);
   }

void
TR_J9VMBase::setIProfilerCallCount(TR_ByteCodeInfo &bcInfo, int32_t count, TR_Compilation *comp)
   {
   TR_IProfiler *profiler = getIProfiler();
   if (profiler)
      profiler->setCallCount(bcInfo, count, comp);
   }

int32_t
TR_J9VMBase::getCGEdgeWeight(TR_Node *callerNode, TR_OpaqueMethodBlock *callee, TR_Compilation *comp)
   {
   TR_IProfiler *profiler = getIProfiler();
   if (profiler)
      return profiler->getCGEdgeWeight(callerNode, callee, comp);

   return 0;
   }

bool
TR_J9VMBase::isCallGraphProfilingEnabled()
   {
   if (getIProfiler())
      {
      return getIProfiler()->isCallGraphProfilingEnabled();
      }

   return false;
   }

void
TR_J9VMBase::updateCGEdgeWeight(TR_Node *callerNode, TR_OpaqueMethodBlock *callee, int32_t weight, TR_Compilation *comp)
   {
   TR_IProfiler *profiler = getIProfiler();
   if (profiler)
      profiler->updateCGEdgeWeight(callerNode, callee, weight, comp);
   }

uintptrj_t
TR_J9VMBase::getInterpreterProfilingData(TR_Node *node, TR_Compilation *comp)
   {
   TR_IProfiler *iProfiler = getIProfiler();
   if (iProfiler == NULL) return (uintptrj_t)NULL;
   return iProfiler->getProfilingData (node->getByteCodeInfo(), comp);
   }

uintptrj_t
TR_J9VMBase::getInterpreterProfilingData(TR_ByteCodeInfo &bcInfo, TR_Compilation *comp)
   {
   TR_IProfiler *iProfiler = getIProfiler();
   if (iProfiler == NULL) return (uintptrj_t)NULL;
   return iProfiler->getProfilingData (bcInfo, comp);
   }

TR_AbstractInfo *
TR_J9VMBase::createIProfilingValueInfo( TR_ByteCodeInfo &bcInfo, TR_Compilation *comp)
   {
   TR_IProfiler *iProfiler = getIProfiler();
   if (iProfiler == NULL) return NULL;
   return iProfiler->createIProfilingValueInfo (bcInfo, comp);
   }

TR_ValueProfileInfo *
TR_J9VMBase::getValueProfileInfoFromIProfiler(TR_ByteCodeInfo & bcInfo, TR_Compilation *comp)
   {
   TR_IProfiler *iProfiler = getIProfiler();
   if (iProfiler == NULL) return NULL;
   return iProfiler->getValueProfileInfo (bcInfo, comp);
   }

uint32_t *
TR_J9VMBase::getAllocationProfilingDataPointer(TR_ByteCodeInfo &bcInfo, TR_OpaqueClassBlock *clazz, TR_OpaqueMethodBlock *method, TR_Compilation *comp)
   {
   TR_IProfiler *iProfiler = getIProfiler();
   if (iProfiler == NULL) return NULL;
   return iProfiler->getAllocationProfilingDataPointer(bcInfo, clazz, method, comp);
   }

uint32_t *
TR_J9VMBase::getGlobalAllocationDataPointer()
   {
   TR_IProfiler *iProfiler = getIProfiler();
   if (iProfiler == NULL) return NULL;
   return iProfiler->getGlobalAllocationDataPointer(isAOT());
   }

TR_ExternalProfiler *
TR_J9VMBase:: hasIProfilerBlockFrequencyInfo(TR_Compilation& comp)
   {
   TR_IProfiler *iProfiler = getIProfiler();
   if (iProfiler == NULL) return NULL;
   return iProfiler->canProduceBlockFrequencyInfo(comp);
   }

// HWProfiler support
TR_HWProfiler *
TR_J9VMBase::getHWProfiler()
   {
   return NULL;
   }

bool
TR_J9VMBase::registerInstructionWithHWProfiler(uint8_t *instr, TR_Node *node, TR_Compilation *comp)
   {
   return false;
   }
   
uint32_t
TR_J9VMBase::getMethodSize(TR_OpaqueMethodBlock *method)
   {
   J9ROMMethod *romMethod = J9_ROM_METHOD_FROM_RAM_METHOD((J9Method *)method);

   return (uint32_t)(J9_BYTECODE_END_FROM_ROM_METHOD(romMethod) -
                     J9_BYTECODE_START_FROM_ROM_METHOD(romMethod));
   }

uintptrj_t
TR_J9VMBase::getMethodStart(TR_OpaqueMethodBlock *method)
   {
   J9ROMMethod *romMethod = J9_ROM_METHOD_FROM_RAM_METHOD((J9Method *)method);

   return (uintptrj_t)(J9_BYTECODE_START_FROM_ROM_METHOD(romMethod));
   }


int32_t TR_J9VMBase::getLineNumber(TR_OpaqueMethodBlock *method, int32_t bcIndex)
   {
   return isAOT() ? -1 : (int32_t)getLineNumberForROMClass(_jitConfig->javaVM, (J9Method *) method, bcIndex);
   }


bool
TR_J9VMBase::isJavaOffloadEnabled()
   {
   #if defined(J9VM_OPT_JAVA_OFFLOAD_SUPPORT)
      return (vmThread()->javaVM->javaOffloadSwitchOnWithReasonFunc != NULL);
   #else
      return false;
   #endif
   }

bool
TR_J9VMBase::isThrowException(UDATA value)
   {
   return value == (UDATA) &jitThrowException ? true : false;
   }

intptrj_t
TR_J9VMBase::getInvocationCount(TR_OpaqueMethodBlock * methodInfo)
   {
   J9Method * method = (J9Method*)methodInfo;
   if (((UDATA) method->constantPool) & J9_STARTPC_JNI_NATIVE)
      return -1;
   intptrj_t count = (intptrj_t) method->extra;
   if (count < 0 || !(count&1))
      return -1;
   return count >> 1;
   }

bool
TR_J9VMBase::setInvocationCount(TR_OpaqueMethodBlock * methodInfo, intptrj_t oldCount, intptrj_t newCount)
   {
   J9Method * method = (J9Method*)methodInfo;
   oldCount = oldCount*2 + 1;
   newCount = newCount*2 + 1;
   if (newCount < 0)
      return false;
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   UDATA result = jitUpdateCount(vmThread(), method, oldCount, newCount);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return (result != 0);
   }

bool
TR_J9VMBase::startAsyncCompile(TR_OpaqueMethodBlock * method, void *oldStartPC, bool *queued, TR_OptimizationPlan *optimizationPlan)
   {
   if (_compInfo && _compInfo->useSeparateCompilationThread())
      {
      bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
      TR_ILGenRequestStorage storage;
      TR_ILGenRequest *request = new (storage) TR_JavaOrdinaryMethodRequest(method);
      _compInfo->compileMethod(vmThread(), request, oldStartPC, TR_yes, NULL, queued, optimizationPlan);
      releaseVMAccessIfNeeded(haveAcquiredVMAccess);
      return true;
      }
   return false;
   }

bool
TR_J9VMBase::isCompiledMethod(TR_OpaqueMethodBlock *method)
   {
   if (tossingCode())
      return false;
   return TR_CompilationInfo::isCompiled((J9Method *)method);
   }

bool
TR_J9VMBase::isBeingCompiled(TR_OpaqueMethodBlock * method, void * startPC)
   {
   return _compInfo->isQueuedForCompilation((J9Method *)method, startPC);
   }

U_32
TR_J9VMBase:: virtualCallOffsetToVTableSlot(U_32 offset)
   {
   return J9JIT_INTERP_VTABLE_OFFSET - offset;
   }

void *
TR_J9VMBase:: addressOfFirstClassStatic(TR_OpaqueClassBlock * j9Class)
   {
   return (void *)(convertClassOffsetToClassPtr(j9Class)->ramStatics);
   }

U_32
TR_J9VMBase::offsetOfIsOverriddenBit()
   {
   return J9_STARTPC_METHOD_IS_OVERRIDDEN;
   }
void *
TR_J9VMBase::getStaticFieldAddress(TR_OpaqueClassBlock * clazz, unsigned char * fieldName, uint32_t fieldLen,
                                   unsigned char * sig, uint32_t sigLen)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   void * result = vmThread()->javaVM->internalVMFunctions->staticFieldAddress(
    vmThread(), convertClassOffsetToClassPtr(clazz), fieldName, fieldLen, sig, sigLen, NULL, NULL, J9_RESOLVE_FLAG_NO_THROW_ON_FAIL,  NULL);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

#ifdef JNI_INLINING

// these were in this file before, just not in this location
typedef struct J9JNINameAndSignature {
        char* name;
        char* signature;
        U_32 nameLength;
        U_32 signatureLength;
} J9JNINameAndSignature;

// these were in this file before, just not in this location
#define LOOKUP_OPTION_JNI 1024
#define LOOKUP_OPTION_NO_CLIMB 32
#define LOOKUP_OPTION_NO_THROW 8192

struct J9JNIID*
TR_J9VMBase::getMethOrFieldID(TR_OpaqueClassBlock * classReference, unsigned char *name, unsigned char *signature, bool isMethod, bool isStatic)    {
   void *env = NULL;
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   // version is :  0x00010002 = 655538
   vmThread()->javaVM->internalVMFunctions->GetEnv((JavaVM *)vmThread()->javaVM, &env, 65538 );
   assume(env, "TR_J9VMBase::getMethOrFieldID failed to get JNIEnv *");
   jclass cls = (jclass) &classReference;

   /*From j9.h
     #define J9JNIID_METHOD  0
     #define J9JNIID_FIELD  1
     #define J9JNIID_STATIC  2*/

   struct J9JNIID * result;
   if (isStatic)
      {
      if (!isMethod)
         {
         result = vmThread()->javaVM->internalVMFunctions->getMethodOrFieldID
            ((JNIEnv *)env, cls, (const char *)name, (const char *)signature, J9JNIID_STATIC | J9JNIID_FIELD);
         }
      else
         {
         result = vmThread()->javaVM->internalVMFunctions->getMethodOrFieldID
            ((JNIEnv *)env, cls, (const char *)name, (const char *)signature, J9JNIID_STATIC | J9JNIID_METHOD);
         }
      }
   else // !isStatic
      {
      if (!isMethod)
         {
         result =  vmThread()->javaVM->internalVMFunctions->getMethodOrFieldID
            ((JNIEnv *)env, cls, (const char *)name, (const char *)signature, J9JNIID_FIELD);
         }
      else
         {
         result = vmThread()->javaVM->internalVMFunctions->getMethodOrFieldID
            ((JNIEnv *)env, cls, (const char *)name, (const char *)signature, J9JNIID_METHOD);
         }
      }
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

void *
TR_J9VMBase::getStaticFieldAddress(TR_OpaqueClassBlock * clazz, unsigned char * fieldName, uint32_t fieldLen,
                                   unsigned char * sig, uint32_t sigLen)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   void * result = vmThread()->javaVM->internalVMFunctions->staticFieldAddress(
     vmThread(), (J9Class *)clazz, fieldName, fieldLen, sig, sigLen, 0, (J9Class **)NULL, NULL,1, (J9Class *) NULL);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

uint32_t
TR_J9VMBase::getInstanceFieldOffset(TR_OpaqueClassBlock * clazz, unsigned char * fieldName, uint32_t fieldLen,
                                    unsigned char * sig, uint32_t sigLen, UDATA options)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   uint32_t result = vmThread()->javaVM->internalVMFunctions->instanceFieldOffset(
      vmThread(), (J9Class *)clazz, fieldName, fieldLen, sig, sigLen, 0, (J9Class **)NULL, NULL, options);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   if (result == FIELD_OFFSET_NOT_FOUND)
      {
      assume5("Class %p has no field '%.*s' of type '%.*s'", clazz, fieldLen, fieldName, sigLen, sig);
      return -1;
      }
   return result;
   }

uint32_t
TR_J9VMBase::getInstanceFieldOffset(TR_OpaqueClassBlock * clazz, unsigned char * fieldName, uint32_t fieldLen,
   {
   return getInstanceFieldOffset(clazz, fieldName, fieldLen, sig, sigLen, J9_RESOLVE_FLAG_NO_THROW_ON_FAIL);
   }

#endif
int32_t
TR_J9VMBase::getInterpreterVTableSlot(TR_OpaqueMethodBlock * mBlock, TR_OpaqueClassBlock * clazz)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   int32_t result =  vmThread()->javaVM->internalVMFunctions->getVTableIndexForMethod((J9Method*)mBlock, (J9Class*)clazz, vmThread());
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }
int32_t
TR_J9VMBase::getVTableSlot(TR_OpaqueMethodBlock * mBlock, TR_OpaqueClassBlock * clazz)
   {
   return J9JIT_INTERP_VTABLE_OFFSET - getInterpreterVTableSlot(mBlock, clazz);
   }
uint64_t
TR_J9VMBase::getUSecClock()
   {
   PORT_ACCESS_FROM_JITCONFIG(_jitConfig);
   return j9time_usec_clock();
   }

uint64_t
TR_J9VMBase::getHighResClock()
   {
   PORT_ACCESS_FROM_JITCONFIG(_jitConfig);
   return j9time_hires_clock();
   }

uint64_t
TR_J9VMBase::getHighResClockResolution()
   {
   PORT_ACCESS_FROM_JITCONFIG(_jitConfig);
   return j9time_hires_frequency();
   }

bool 
TR_J9VMBase::hasBackwardBranches(TR_OpaqueMethodBlock *method)
   {
   J9ROMMethod * romMethod = J9_ROM_METHOD_FROM_RAM_METHOD((J9Method *)method);
   return (romMethod->modifiers & J9AccMethodHasBackwardBranches) != 0; 
   }


TR_JitPrivateConfig *
TR_J9VMBase::getPrivateConfig()
   {
   return (TR_JitPrivateConfig*) _jitConfig->privateConfig;
   }

TR_JitPrivateConfig *
TR_FrontEnd::getPrivateConfig(void *jitConfig)
   {
   return (TR_JitPrivateConfig*) ((J9JITConfig*)jitConfig)->privateConfig;
   }

void *
TR_J9VMBase::getStartAddressFromPersistentIdentifier(TR_OpaqueMethodBlock * id)
   {
   return ((J9Method *)id)->extra;
   }

void
TR_J9VMBase::revertToInterpreted(TR_OpaqueMethodBlock * method)
   {
   revertMethodToInterpreted((J9Method*)method);
   }

extern "C" {
void revertMethodToInterpreted(J9Method * method)
   {
   TR_CompilationInfo * compInfo = getCompilationInfo(jitConfig);
   compInfo->acquireCompilationLock();
   J9JIT_REVERT_METHOD_TO_INTERPRETED(jitConfig->javaVM, method);
   compInfo->releaseCompilationLock();
   }
}

// Which recognized methods are known to have no valid null checks
//
static RecognizedMethod canSkipNullChecks[] =
   {
   // NOTE!! add methods whose checks can be skipped by sov library to the beginning of the list (see stopMethod below)
   java_util_ArrayList_add,
   java_util_ArrayList_ensureCapacity,
   java_util_ArrayList_get,
   java_lang_String_trim,
   java_lang_String_replace,
   java_lang_String_charAt,
   java_lang_String_hashCode,
   //java_lang_String_toLowerCase,
   //java_lang_String_toUpperCase,
   java_lang_String_equalsIgnoreCase,
   java_lang_String_indexOf_fast,
   java_util_Hashtable_clone,
   java_util_Hashtable_contains,
   java_util_HashtableHashEnumerator_hasMoreElements,
   java_util_HashtableHashEnumerator_nextElement,
   //java_util_Vector_addElement,
   java_math_BigDecimal_longString1C,
   java_math_BigDecimal_longString2,
   java_util_TreeMapUnboundedValueIterator_next,
   java_util_TreeMapSubMap_setFirstKey,
   java_util_TreeMapSubMap_setLastKey,
   java_lang_String_init_int_String_int_String_String,
   unknownMethod
   };


bool
TR_J9VMBase::skipNullChecks(TR_MethodSymbol * method)
   {
   RecognizedMethod methodId = method->getRecognizedMethod();
   if (methodId == unknownMethod)
      return false;

   for (int i = 0; canSkipNullChecks[i] != unknownMethod; ++i)
      if (canSkipNullChecks[i] == methodId)
         return true;

   return false;
   }


// Which recognized methods are known to have no valid bound checks
//
static RecognizedMethod canSkipBoundChecks[] =
   {
   // NOTE!! add methods whose checks can be skipped by sov library to the beginning of the list (see stopMethod below)
   //java_util_ArrayList_add,
   //java_util_ArrayList_remove,
   //java_util_ArrayList_ensureCapacity,
   //java_util_ArrayList_get,
   java_lang_String_trim,
   java_lang_String_charAt,
   java_lang_String_indexOf_String,
   java_lang_String_indexOf_String_int,
   java_lang_String_indexOf_native,
   java_lang_String_indexOf_fast,
   java_lang_String_replace,
   java_lang_String_compareTo,
   java_lang_String_lastIndexOf,
   java_lang_String_toLowerCase,
   java_lang_String_toUpperCase,
   java_lang_String_regionMatches,
   java_lang_String_regionMatches_bool,
   java_lang_String_regionMatches,
   java_lang_String_equalsIgnoreCase,
   java_lang_String_compareToIgnoreCase,
   java_lang_String_hashCode,
   java_util_Hashtable_get,
   java_util_Hashtable_put,
   java_util_Hashtable_clone,
   java_util_Hashtable_rehash,
   java_util_Hashtable_remove,
   java_util_Hashtable_contains,
   java_util_Hashtable_getEntry,
   java_util_HashtableHashEnumerator_hasMoreElements,
   java_util_HashtableHashEnumerator_nextElement,
   java_util_TreeMap_all,
   java_util_TreeMap_rbInsert,
   java_math_BigDecimal_longString1C,
   java_math_BigDecimal_longString2,
   java_util_TreeMapUnboundedValueIterator_next,
   java_util_TreeMapSubMap_setFirstKey,
   java_util_TreeMapSubMap_setLastKey,
   java_util_HashMap_get,
   java_util_HashMap_findNonNullKeyEntry,
   java_util_HashMap_putImpl,
   java_lang_String_init_int_String_int_String_String,
   unknownMethod
   };

bool
TR_J9VMBase::skipBoundChecks(TR_MethodSymbol * method)
   {
   RecognizedMethod methodId = method->getRecognizedMethod();
   if (methodId == unknownMethod)
      return false;

   for (int i = 0; canSkipBoundChecks[i] != unknownMethod; ++i)
      if (canSkipBoundChecks[i] == methodId)
         return true;

   return false;
   }


// Which recognized methods are known to have no valid div checks
//
static RecognizedMethod canSkipDivChecks[] =
   {
   java_util_Hashtable_get,
   java_util_Hashtable_put,
   java_util_Hashtable_rehash,
   java_util_Hashtable_remove,
   java_util_Hashtable_getEntry,
   unknownMethod
   };

bool
TR_J9VMBase::skipDivChecks(TR_MethodSymbol * method)
   {
   RecognizedMethod methodId = method->getRecognizedMethod();
   if (methodId == unknownMethod)
      return false;

   for (int i = 0; canSkipDivChecks[i] != unknownMethod; ++i)
      if (canSkipDivChecks[i] == methodId)
         return true;

   return false;
   }


// Which recognized methods are known to have no valid checkcasts
//
static RecognizedMethod canSkipCheckCasts[] =
   {
   java_util_ArrayList_ensureCapacity,
   java_util_Hashtable_clone,
   java_util_Hashtable_putAll,
   //ali// removed: no longer has checkcasts in it, and to fix jartester
   //java_util_Hashtable_rehash,
   unknownMethod
   };

bool
TR_J9VMBase::skipCheckCasts(TR_MethodSymbol * method)
   {
   RecognizedMethod methodId = method->getRecognizedMethod();
   if (methodId == unknownMethod)
      return false;

   for (int i = 0; canSkipCheckCasts[i] != unknownMethod; ++i)
      if (canSkipCheckCasts[i] == methodId)
         return true;

   return false;
   }


// Which recognized methods are known to have no valid array store checks
//
static RecognizedMethod canSkipArrayStoreChecks[] =
   {
   java_util_ArrayList_add,
   java_util_ArrayList_ensureCapacity,
   java_util_Hashtable_put,
   java_util_Hashtable_clone,
   java_util_Hashtable_rehash,
   java_util_Hashtable_remove,
   unknownMethod
   };

bool
TR_J9VMBase::skipArrayStoreChecks(TR_MethodSymbol * method)
   {
   RecognizedMethod methodId = method->getRecognizedMethod();
   if (methodId == unknownMethod)
      return false;

   for (int i = 0; canSkipArrayStoreChecks[i] != unknownMethod; ++i)
      if (canSkipArrayStoreChecks[i] == methodId)
         return true;

   return false;
   }


// Which recognized methods are known to require no checking when lowering to TR_arraycopy
//
static RecognizedMethod canSkipChecksOnArrayCopies[] =
   {
   // NOTE!! add methods whose checks can be skipped by sov library to the beginning of the list (see stopMethod below)
   //java_util_ArrayList_ensureCapacity,
   //java_util_ArrayList_remove,   /* ArrayList is NOT synchronized and therefore it's unsafe to remove checks! */
   java_lang_String_concat,
   java_lang_String_replace,
   java_lang_String_toLowerCase,
   java_lang_String_toUpperCase,
   java_lang_String_toCharArray,
   java_lang_StringBuffer_append,
   java_lang_StringBuffer_ensureCapacityImpl,
   java_lang_String_init_int_String_int_String_String,
   unknownMethod
   };

bool
TR_J9VMBase::skipChecksOnArrayCopies(TR_MethodSymbol * method)
   {
   RecognizedMethod methodId = method->getRecognizedMethod();
   if (methodId == unknownMethod)
      return false;

   for (int i = 0; canSkipChecksOnArrayCopies[i] != unknownMethod; ++i)
      if (canSkipChecksOnArrayCopies[i] == methodId)
         return true;

   return false;
   }

// Which recognized methods are known to not require zero initialization of arrays
//
static RecognizedMethod canSkipZeroInitializationOnNewarrays[] =
   {
   java_lang_String_init,
   java_lang_String_concat,
   java_lang_String_replace,
   java_lang_String_toCharArray,
   java_lang_String_toLowerCase,
   //java_lang_String_toUpperCase,
   java_math_BigDecimal_toString,
   java_lang_StringCoding_encode,
   java_lang_StringCoding_decode,
   //java_lang_StringBuilder_ensureCapacityImpl,
   //java_lang_StringBuffer_ensureCapacityImpl,
   //java_util_Arrays_copyOf,
   unknownMethod
   };


bool
TR_J9VMBase::skipZeroInitializationOnNewarrays(TR_MethodSymbol * method)
   {
   RecognizedMethod methodId = method->getRecognizedMethod();
   if (methodId == unknownMethod)
      return false;

   for (int i = 0; canSkipZeroInitializationOnNewarrays[i] != unknownMethod; ++i)
      if (canSkipZeroInitializationOnNewarrays[i] == methodId)
         return true;

   return false;
   }


// See if a call argument can escape the current method via the call
//

struct TrustedClass
   {
   char   * name;
   int32_t length;
   int32_t argNum;
   };

static TrustedClass trustedClasses[] =
   {
   "java/lang/String",       16, -1, // trusted in both jclMax and J2SE
   "java/lang/StringBuffer", 22, -1, // trusted in both jclMax and J2SE
   "java/util/Hashtable",    19,  0, // trusted in both jclMax and J2SE
   "java/util/Vector",       16,  0, // trusted in both jclMax and J2SE
   "java/io/DataInputStream",23,  0, // not yet trusted in J2SE
   "java/io/File",           12,  0, // not yet trusted in J2SE
   "java/net/URL",           12,  0, // not yet trusted in J2SE
   "java/util/Stack",        15,  0, // not yet trusted in J2SE
   NULL,                      0,  0  // Mark end of trusted classes
   };

struct TrustedMethod
   {
   RecognizedMethod method;
   int32_t                                   argNum;
   };

static TrustedMethod trustedMethods[] =
   {
   java_util_Vector_contains,  1,
   unknownMethod,             -1    // Mark end of trusted methods
   };

static TrustedMethod untrustedMethods[] =
   {
   java_util_Vector_subList,  -1,
   unknownMethod,             -1    // Mark end of untrusted methods
   };

bool
TR_J9VMBase::argumentCanEscapeMethodCall(TR_MethodSymbol * method, int32_t argIndex)
   {
   int32_t numberOfTrustedClasses = MAX_INT;
   if (_jitConfig->javaVM->j2seVersion != 0)
      numberOfTrustedClasses = 4;

   int32_t i;
   RecognizedMethod methodId = method->getRecognizedMethod();

   // See if the method is a member of a trusted class. If so the argument
   // will not escape via this call, unless it is in the list of untrusted
   // methods.
   //
   char * className    = method->getMethod()->classNameChars();
   int32_t nameLength = method->getMethod()->classNameLength();

   for (i = 0; trustedClasses[i].name && i < numberOfTrustedClasses; ++i)
      {
      if (nameLength == trustedClasses[i].length &&
          !strncmp(className, trustedClasses[i].name, nameLength))
         {
         if (trustedClasses[i].argNum < 0 ||
             trustedClasses[i].argNum == argIndex)
            {
            if (methodId == unknownMethod)
               return false;

            for (i = 0; untrustedMethods[i].method != unknownMethod; ++i)
               {
               if (untrustedMethods[i].method == methodId)
                  {
                  if (untrustedMethods[i].argNum < 0 ||
                      untrustedMethods[i].argNum == argIndex)
                     {
                     return true;
                     }
                  }
               }
            return false;
            }
         }
      }

   // See if the method is a trusted method for the argument in this position.
   // If so, the argument will not escape via this call.
   //
   // Note that an argument position of "-1" in the list of trusted methods
   // means that any argument is OK.
   //
   if (methodId == unknownMethod)
      return true;

   for (i = 0; trustedMethods[i].method != unknownMethod; ++i)
      {
      if (trustedMethods[i].method == methodId)
         {
         if (trustedMethods[i].argNum < 0 ||
             trustedMethods[i].argNum == argIndex)
            {
            return false;
            }
         }
      }

   return true;
   }

#define BDCLASSLEN 20
const char * recognizedBigDecimalClasses [] =
   {
   "java/math/BigDecimal" //length = BDCLASSLEN
   };

bool
TR_J9VMBase::isBigDecimalClass(J9UTF8 * className)
   {
   return (J9UTF8_LENGTH(className) == BDCLASSLEN &&
           !strcmp(utf8Data(className), recognizedBigDecimalClasses[0]));
   }

#ifdef ENABLE_INTRINSICS
bool
TR_J9VMBase::isBigDecimalConvertersClass(J9UTF8 * className)
   {
   return (J9UTF8_LENGTH(className) == 32 &&
           !strcmp(utf8Data(className), "com/ibm/BigDecimalConverters"));
   }
#endif


J9Class *
TR_J9VMBase::convertClassOffsetToClassPtr(TR_OpaqueClassBlock *clazzOffset)
   {
   //return (J9Class*)clazzOffset;
   // NOTE : We could pass down vmThread() in the call below if the conversion
   // required the VM thread. Currently it does not. If we did change that
   // such that the VM thread was reqd, we would need to handle AOT where the
   // TR_FrontEnd is created with a NULL J9VMThread object.
   //
   return J9JitMemory::convertClassOffsetToClassPtr(clazzOffset);
   }

TR_OpaqueClassBlock *
TR_J9VMBase::convertClassPtrToClassOffset(J9Class *clazzPtr)
   {
   //return (TR_OpaqueClassBlock*)clazzPtr;
   // NOTE : We could pass down vmThread() in the call below if the conversion
   // required the VM thread. Currently it does not. If we did change that
   // such that the VM thread was reqd, we would need to handle AOT where the
   // TR_FrontEnd is created with a NULL J9VMThread object.
   //
   return J9JitMemory::convertClassPtrToClassOffset(clazzPtr);
   }

const char *
TR_J9VMBase::getJ9MonitorName(J9ThreadMonitor* monitor) { return monitor->name; }

/////////////////////////////////////////////////////
// TR_J9VM
/////////////////////////////////////////////////////

TR_J9VM::TR_J9VM(J9JITConfig * jitConfig, J9VMThread * vmThread)
   : TR_J9VMBase(jitConfig,vmThread,
                 opCodesForStore, opCodesForLoad,
                 opCodesForIndirectStore, opCodesForIndirectLoad,
                 opCodesForIndirectArrayStore, opCodesForIndirectArrayLoad,
                 opCodesForConst,
                 opCodesForRegisterStore, opCodesForRegisterLoad,
                 opCodesForCompareEquals, opCodesForCompareNotEquals)
   {
   }

J9ROMClass *
TR_J9VM::getRomClass(TR_OpaqueClassBlock * clazz)
   {
   return convertClassOffsetToClassPtr(clazz)->romClass;
   }

//d169771 [2177]
uintptrj_t
TR_J9VM::getPersistentClassPointerFromClassPointer(TR_OpaqueClassBlock * clazz)
   {
   return (uintptrj_t)getRomClass(clazz);
   }

uint32_t
TR_J9VM::getTenantStatusIndex(TR_OpaqueClassBlock * clazz)
   {
#if defined(J9VM_OPT_TENANT)
   return convertClassOffsetToClassPtr(clazz)->tenantInitStatus;
#else
   return 0;
#endif
   }

// get J9Class tenant status row index and col index
uint32_t
TR_J9VM::getTenantStatusRowIndex(TR_OpaqueClassBlock * clazz)
   {
#if defined(J9VM_OPT_TENANT)
   return convertClassOffsetToClassPtr(clazz)->tenantInitStatus >> TR_StaticSymbol::tenantRowIndexBits;
#else
   return 0;
#endif
   }
   
uint32_t
TR_J9VM::getTenantStatusColIndex(TR_OpaqueClassBlock * clazz)
   {
#if defined(J9VM_OPT_TENANT)
   return convertClassOffsetToClassPtr(clazz)->tenantInitStatus & TR_StaticSymbol::tenantColIndexMask;
#else
   return 0;
#endif
   }

bool
TR_J9VM::isClassIsolated(TR_OpaqueClassBlock *clazz)
   {
#if defined(J9VM_OPT_TENANT)
   if (!clazz)
      return false;

   J9Class *clazzPtr = convertClassOffsetToClassPtr(clazz);
   return clazzPtr->classDepthAndFlags & J9AccClassNeedsPerTenantInitialization;
#else
   return false;
#endif
   }

bool 
TR_J9VM::isClassSharedBetweenTenants(TR_OpaqueClassBlock *clazz)
   {
#if defined(J9VM_OPT_TENANT)
   if (!clazz)
      return false;
   return (((J9ClassLoader *)getClassLoader(clazz))->flags & J9CLASSLOADER_AUTO_TENANT_SCOPE_FIELD_ENABLED) != 0;
#else
   return false;
#endif
   }

/*
 * check if an isolated class is initialized in containingClass's tenant.
 * containingClass must be none shared class.
 * in this case, it can remove tenant init check in containingClass's method
 */
bool
TR_J9VM::isClassInitializedInTenant(TR_OpaqueClassBlock *containingClass, TR_OpaqueClassBlock *checkingClass)
   {
#if defined(J9VM_OPT_TENANT)
   bool classIsInitialized = (((convertClassOffsetToClassPtr(checkingClass))->initializeStatus & J9ClassInitStatusMask) == J9ClassInitTenant);
   if (!classIsInitialized)
      return false;

   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   int32_t tenantStatus = (int32_t)jitCTClassInitializedInTenant(vmThread(),
                                                                 (J9ClassLoader *)(getClassLoader(containingClass)),
                                                                 convertClassOffsetToClassPtr(checkingClass));
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   // turn on when running in mode 751, this should always true.
   //assume2(tenantStatus == J9ClassInitSucceeded, "isolated class is not initilaized, class is %p\n", checkingClass);
   return tenantStatus == J9ClassInitSucceeded;
#else
   return false;
#endif
   }

bool
TR_J9VM::isClassPreTenantInitialized(TR_OpaqueClassBlock *clazz)
   {
#if defined(J9VM_OPT_TENANT)
   // check optional flag J9ROMCLASS->optionalFlags = J9_ROMCLASS_OPTINFO_IS_HOT_CLASS
   J9ROMClass* romClass = ((J9Class *)clazz)->romClass;
   return romClass->optionalFlags & J9_ROMCLASS_OPTINFO_IS_HOT_CLASS;
#else
   return false;
#endif
   }

UDATA
TR_J9VM::getClassDepth(TR_OpaqueClassBlock * clazzPointer)
   {
   return (convertClassOffsetToClassPtr(clazzPointer)->classDepthAndFlags) & J9_JAVA_CLASS_DEPTH_MASK;
   }

UDATA
TR_J9VM::getClassInstanceSize(TR_OpaqueClassBlock * clazzPointer)
   {
   return convertClassOffsetToClassPtr(clazzPointer)->totalInstanceSize;
   }

void *
TR_J9VM::getClassLoader(TR_OpaqueClassBlock * classPointer)
   {
   return (void *) (convertClassOffsetToClassPtr(classPointer)->classLoader);
   }

void *
TR_J9VM::getLocationOfClassLoaderObjectPointer(TR_OpaqueClassBlock *classPointer)
   {
   return (void *)&(TMP_J9CLASSLOADER_CLASSLOADEROBJECT(convertClassOffsetToClassPtr(classPointer)->classLoader));
   }

TR_OpaqueClassBlock *
TR_J9VM::getClassOfMethod(TR_OpaqueMethodBlock *method)
   {
   return (TR_OpaqueClassBlock *)J9_CLASS_FROM_METHOD(((J9Method *)method));
   }

UDATA
TR_J9VM::getHeapBaseAddress()
   {
   return 0;
   }


void TR_J9VM::getCompInfo(char *processorName)
   {
   _jitConfig->jitLevelName;
   
   TR_Target target = (TR_Target)jitConfig->targetName;
   
   if(isPPC(target))
      {
      switch(portLibCall_getPPCProcessorType())
         {
         case TR_PPCpwr604:
            strcpy(processorName, "PPCPWR604");
            break;
            
         case TR_PPCpwr630:
            strcpy(processorName, "PPCpwr630 ");
            break;
            
         case TR_PPCgp:
            strcpy(processorName, "PPCgp");
            break;
            
         case TR_PPCgr:
            strcpy(processorName, "PPCgr");
            break;
            
         case TR_PPCp6:
            strcpy(processorName, "PPCp6");
            break;

         case TR_PPCp7:
            strcpy(processorName, "PPCp7");
            break;

         case TR_PPCp8:
            strcpy(processorName, "PPCp8");
            break;
            
         case TR_PPCpulsar:
            strcpy(processorName, "PPCpulsar");
            break;
            
         case TR_PPCnstar:
            strcpy(processorName, "PPCnstar");
            break;
            
         case TR_PPCpwr403:
            strcpy(processorName, "PPCPWR403");
            break;
            
         case TR_PPCpwr601:
            strcpy(processorName, "PPCPWR601");
            break;
            
         case TR_PPCpwr603:
            strcpy(processorName, "PPCPWR603");
            break;
            
         case  TR_PPC82xx:
            strcpy(processorName, "PPCP82xx");
            break;
            
         case  TR_PPC7xx:
            strcpy(processorName, "PPC7xx");
            break;
            
         case TR_PPCpwr440:
            strcpy(processorName, "PPCPWR440");
            break;
            
         default:
            strcpy(processorName, "Unknown PPC processor");
            break;
         }
      return;
      }
   
   if(isARM(target))
      {
      strcpy(processorName, "Unknown ARM processor");
      return;
      }

#if defined(TR_TARGET_S390)
   if((is390(target)))
      {
      /* Definitions in codegen/Runtime.hpp
         #define G5                              (9672)  // Not Supported as of Java 1.5
         #define MULTIPRISE7000                  (7060)  // Not Supported as of Java 1.5
         #define FREEWAY                         (2064)
         #define Z800                            (2066)
         #define MIRAGE                          (1090)
         #define TREX                            (2084)
         #define Z890                            (2086)
         #define GOLDEN_EAGLE                    (2094)
         #define DANU_GA2                        (2094)  // type doesn't change from GOLDEN_EAGLE
         #define Z9BC                            (2096)
         #define Z10                             (2097)
         #define Z10BC                           (2098)  // zMR
         #define ZG                              (2817)
         #define ZGMR                            (2818)
         #define ZG_RESERVE                      (2819)  // reserve for zGryphon
         #define ZGP                             (2827)
         #define ZGPMR                           (2828)
         #define ZGP_RESERVE                     (2829)  // reserve for zHelix
         #define ZH                              (2458)  // reserve for zHybrid
         #define DATAPOWER                       (2462)  // reserve for DataPwoer on zHybrid
         #define ZH_RESERVE1                     (2459)  // reserve for zHybrid
         #define ZH_RESERVE2                     (2461)  // reserve for zHybrid
      */
      
      if(isZOS(target))
         {
         getS390MachineName(portLib_get390zOSMachineType(), processorName);
         }
      else
         {
         getS390MachineName(portLib_get390zLinuxMachineType(), processorName);
         }
      return;
      }
#endif

   if(isX86(target))
      {
      switch(_compInfo->getProcessor())
         {
         case TR_X86ProcessorIntelPentium:
            strcpy(processorName,"X86 Intel Pentium");
            break;
            
         case TR_X86ProcessorIntelP6:
            strcpy(processorName,"X86 Intel P6");
            break;
            
         case TR_X86ProcessorIntelPentium4:
            strcpy(processorName,"X86 Intel Netburst Microarchitecture");
            break;
            
         case TR_X86ProcessorIntelCore2:
            strcpy(processorName,"X86 Intel Core2 Microarchitecture");
            break;
            
         case TR_X86ProcessorIntelTulsa:
            strcpy(processorName,"X86 Intel Tulsa");
            break;
            
         case TR_X86ProcessorAMDK5:
            strcpy(processorName,"X86 AMDK5");
            break;
            
         case TR_X86ProcessorAMDAthlonDuron:
            strcpy(processorName,"X86 AMD Athlon-Duron");
            break;
            
         case TR_X86ProcessorAMDOpteron:
            strcpy(processorName,"X86 AMD Opteron");
            break;
            
         default:
            strcpy(processorName,"Unknown X86 Processor");
            break;
         }
      return;
      
      }
   strcpy(processorName,"Unknown Processor");
   return;
   }


I_32
TR_J9VMBase::getAddressFieldSize()
   {
#if defined(J9VM_GC_COMPRESSED_POINTERS)
   return sizeof(fj9object_t);
#else
   return sizeof(uintptrj_t);
#endif
   }

// used by both j2se and sharedcache vms
//
I_32
TR_J9VM::getCompressedPointersShift()
   {
#if defined(J9VM_GC_COMPRESSED_POINTERS)
   J9JavaVM *jvm = _jitConfig->javaVM;
   if (!jvm)
      return 0;
   //bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   J9VMThread *vmThread = jvm->internalVMFunctions->currentVMThread(jvm);
   J9MemoryManagerFunctions * mmf = jvm->memoryManagerFunctions;
   I_32 result = mmf->j9gc_objaccess_compressedPointersShift((J9VMToken *)vmThread);
   //releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
#else
   return 0;
#endif

   }

UDATA
TR_J9VM::getShadowHeapProperties(bool getBase)
   {
   // return either the heap base or the heap top
   //
#if defined(J9VM_GC_COMPRESSED_POINTERS)
   J9JavaVM *jvm = _jitConfig->javaVM;
   if (!jvm || isAOT())
      return 0;
   //bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   UDATA result = 0;
   J9MemoryManagerFunctions * mmf = jvm->memoryManagerFunctions;
   J9VMThread *vmThread = jvm->internalVMFunctions->currentVMThread(jvm);
#if 0
   if (getBase)
      result = mmf->j9gc_objaccess_compressedPointersShadowHeapBase((J9VMToken *)vmThread);
   else
      result = mmf->j9gc_objaccess_compressedPointersShadowHeapTop((J9VMToken *)vmThread);
#endif
   //releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
#else
      return 0;
#endif
   }

TR_ResolvedMethod *
TR_J9VM::getObjectNewInstanceImplMethod(TR_Memory * trMemory)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   J9Method * protoMethod;
   J9InternalVMFunctions * intFunc = vmThread()->javaVM->internalVMFunctions;

   // make sure that internal functions return true class pointers
   J9Class * jlObject = intFunc->internalFindKnownClass(vmThread(), J9KnownClassJLObject, J9_FINDKNOWNCLASS_FLAG_EXISTING_ONLY);
   protoMethod = (J9Method *) intFunc->javaLookupMethod(vmThread(), jlObject, (J9ROMNameAndSignature *) &newInstancePrototypeNameAndSig, NULL, J9_LOOK_DIRECT_NAS | J9_LOOK_VIRTUAL | J9_LOOK_NO_THROW);
   protoMethod->constantPool = (J9ConstantPool *) ((UDATA) protoMethod->constantPool | J9_STARTPC_METHOD_IS_OVERRIDDEN);
   TR_ResolvedMethod * result = createResolvedMethod(trMemory, (TR_OpaqueMethodBlock *) protoMethod, 0);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

uintptrj_t
TR_J9VMBase::getBytecodePC(TR_OpaqueMethodBlock *method, TR_ByteCodeInfo &bcInfo)
   {
   uintptrj_t methodStart = getMethodStart(method);
   return methodStart + (uintptrj_t)(bcInfo.getByteCodeIndex());
   }

void *
TR_J9VMBase::generateMethodCookie(void *ramMethod, TR_Compilation *comp)
   {
   assume(0, "Should never try to create method cookie in non AOT!");
   return (void*)0xBAD11BAD;
   }

//Given a class signature classSig, find the symbol references of all classSig's methods
//whose signatures are given in methodSig. methodCount is the number of methods.
//Returns the number of methods found in the class
//This function handles static and virtual functions only.
int TR_J9VMBase::findOrCreateMethodSymRef(TR_Compilation* comp, TR_ResolvedMethodSymbol* owningMethodSym, char* classSig, char** methodSig, TR_SymbolReference** symRefs, int methodCount) 
   {
   TR_OpaqueClassBlock *c = getClassFromSignature(classSig, 
                                                  strlen(classSig), 
                                                  comp->getCurrentMethod());
   if (!c)
      { 
      if (comp->getOption(TR_TraceILGen)) 
         trace1c(comp, "class %s not found\n", classSig); 
      return 0;
      }
   void* stackMark = comp->trMemory()->markStack();
   TR_ScratchList<TR_ResolvedMethod> methods(comp->trMemory());
   getResolvedMethods(comp->trMemory(), c, &methods);
   ListIterator<TR_ResolvedMethod> it(&methods);
   int numMethodsFound = 0;
   //I couldn't make "new (comp->trStackMemory()) int[methodCount]" to compile so I 
   //wrote the following ugly code. Please fix this if you know how to do that
   int *methodSigLen = (int*)comp->trMemory()->allocateStackMemory(sizeof(int)*methodCount);
   for (int i = 0; i < methodCount; i++)
      {
      methodSigLen[i] = strlen(methodSig[i]);
      if (symRefs[i])
         numMethodsFound++;
      }
   for (TR_ResolvedMethod *method = it.getCurrent(); method && numMethodsFound < methodCount; method = it.getNext())
      {
      if (method->isConstructor()) continue;
      const char *sig = method->signature(comp->trMemory());
      int32_t len = strlen(sig);
      for (int i = 0; i < methodCount; i++) 
         {
         if (!symRefs[i] && !strncmp(sig, methodSig[i], methodSigLen[i]))
            {
            if (method->isStatic()) 
               {
               symRefs[i] = comp->getSymRefTab()->
                  findOrCreateMethodSymbol(owningMethodSym
                                           ?owningMethodSym->getResolvedMethodIndex()
                                           :JITTED_METHOD_INDEX, 
                                           -1, method, TR_MethodSymbol::Static);
               }
            else
               {
               symRefs[i] = comp->getSymRefTab()->
                  findOrCreateMethodSymbol(owningMethodSym
                                           ?owningMethodSym->getResolvedMethodIndex()
                                           :JITTED_METHOD_INDEX, 
                                           -1, method, TR_MethodSymbol::Virtual);
               symRefs[i]->setOffset(getVTableSlot(method->getPersistentIdentifier() ,c));
               }
            numMethodsFound++;
            }
         }
      }
   comp->trMemory()->releaseStack(stackMark);
   return numMethodsFound;
   }

//Given a class signature and a method signature returns a symref for that method.
//If the method is not resolved or doesn't exist in that class, it returns NULL
//This function handles static and virtual functions only.
TR_SymbolReference* TR_J9VMBase::findOrCreateMethodSymRef(TR_Compilation* comp, TR_ResolvedMethodSymbol* owningMethodSym, char* classSig, char* methodSig) 
   {
   TR_SymbolReference* symRef = NULL;
   int numMethodsFound = findOrCreateMethodSymRef(comp, owningMethodSym, classSig, &methodSig, &symRef, 1);
   return ( numMethodsFound == 1)? symRef : NULL;
   }

//Given a complete method signature (i.e., including the full class name)
//returns a symref for that method. If the method is not resolved or doesn't exist
//in that class, it returns NULL
//This function handles static and virtual functions only.
TR_SymbolReference* TR_J9VMBase::findOrCreateMethodSymRef(TR_Compilation* comp, TR_ResolvedMethodSymbol* owningMethodSym, char* methodSig) {
   int methodSigLen = strlen(methodSig);
   char* classSig = (char*)comp->trMemory()->allocateStackMemory(sizeof(char)*methodSigLen);
   char* separator = strchr(methodSig, '.');
   assume(separator, ". not found in method name");
   int classSigLen = separator - methodSig;
   strncpy(classSig, methodSig, classSigLen);
   classSig[classSigLen] = 0;
   TR_SymbolReference* result = findOrCreateMethodSymRef(comp, owningMethodSym, classSig, methodSig);
   return result;
}

//Given an array of full method signatures (i.e., incluing full class name), this method
//gives symrefs for those methods. The number of input methods are given in methodCount.
//The function returns the number of methods found
//this function handles static and virtual functions only
int TR_J9VMBase::findOrCreateMethodSymRef(TR_Compilation* comp, TR_ResolvedMethodSymbol* owningMethodSym, char** methodSig, TR_SymbolReference** symRefs, int methodCount) {
   int numMethodsFound = 0;
   for (int i = 0; i < methodCount; i++) {
      if (!methodSig[i]) continue;
      symRefs[i] = findOrCreateMethodSymRef(comp, owningMethodSym, methodSig[i]);
      if (symRefs[i])
	 numMethodsFound++;
   }
   return numMethodsFound;
}

//returns true if a subclass of java.util.concurrent.locks.AbstractOwnableSynchronizer
bool 
TR_J9VMBase::isOwnableSyncClass(TR_OpaqueClassBlock *clazz)
   {
   J9Class* j9class = convertClassOffsetToClassPtr(clazz);
   return ((j9class->classDepthAndFlags & J9_JAVA_CLASS_OWNABLE_SYNCHRONIZER) != 0);
   }

void
TR_J9VM::getResolvedMethods(TR_Memory * trMemory, TR_OpaqueClassBlock * classPointer, List<TR_ResolvedMethod> * resolvedMethodsInClass)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   J9Method * resolvedMethods = (J9Method *) getMethods(classPointer);
   uint32_t i;
   uint32_t numMethods = getNumMethods(classPointer);
   for (i=0;i<numMethods;i++)
      {
      resolvedMethodsInClass->add(createResolvedMethod(trMemory, (TR_OpaqueMethodBlock *) &(resolvedMethods[i]), 0));
      }
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   }

void *
TR_J9VM::getMethods(TR_OpaqueClassBlock * classPointer)
   {
   return (J9Method *) convertClassOffsetToClassPtr(classPointer)->ramMethods;
   }

uint32_t
TR_J9VM::getNumMethods(TR_OpaqueClassBlock * classPointer)
   {
   return getRomClass(classPointer)->romMethodCount;
   }

uint32_t
TR_J9VM::getNumInnerClasses(TR_OpaqueClassBlock * classPointer)
   {
   return getRomClass(classPointer)->innerClassCount;
   }

#define SMALL_METHOD_SIZE 15
#define DEBUG_RESERVATION_SCAN 0
void
TR_J9VM::scanClassForReservation (TR_OpaqueClassBlock *classPointer, TR_Compilation *comp)
   {
   J9Method * resolvedMethods = (J9Method *) getMethods(classPointer);
   TR_PersistentClassInfo * persistentClassInfo =
      comp->getPersistentInfo()->getPersistentCHTable()->findClassInfoAfterLocking(classPointer, comp);
   uint32_t i;
   uint32_t numMethods = getNumMethods(classPointer);
   int32_t  numSynchronizedMethods  = 0;
   int32_t  synchronizedMethodsSize = 0;
   int32_t  numNonSynchronizedMethods = 0; // excluding constructors
   int32_t  numSmallNonSynchronizedMethods = 0;
   int32_t  numSmallSynchronizedMethods = 0;

   if (!persistentClassInfo || persistentClassInfo->isScannedForReservation())
      return;

   bool candidateClass = false;
   for (i=0;i<numMethods;i++)
      {
      J9Method *resolvedMethod = &(resolvedMethods[i]);
      int32_t methodSize = getMethodSize((TR_OpaqueMethodBlock *)resolvedMethod);
      J9ROMMethod * romMethod = J9_ROM_METHOD_FROM_RAM_METHOD((J9Method *)resolvedMethod);

      if (romMethod->modifiers & J9AccSynchronized)
         {
         numSynchronizedMethods ++;
         synchronizedMethodsSize += methodSize;
         if (methodSize < SMALL_METHOD_SIZE)
            numSmallSynchronizedMethods ++;
         }
      else
         {
         J9UTF8 *name = J9ROMMETHOD_GET_NAME(J9_CLASS_FROM_METHOD(resolvedMethod)->romClass, J9_ROM_METHOD_FROM_RAM_METHOD(resolvedMethod));

         if (J9UTF8_LENGTH(name) == 6)
            {
            char s[10];
            sprintf(s, "%.*s",
                    J9UTF8_LENGTH(name), J9UTF8_DATA(name));
            if (strncmp(s, "<init>", 6))
               {
               numNonSynchronizedMethods ++;
               if (methodSize < SMALL_METHOD_SIZE)
                  numSmallNonSynchronizedMethods ++;
               }
            }
         else
            {
            numNonSynchronizedMethods ++;
            if (methodSize < SMALL_METHOD_SIZE)
               numSmallNonSynchronizedMethods ++;
            }
         }

      if (DEBUG_RESERVATION_SCAN && candidateClass)
         {
         char s[4096];
         J9UTF8 *name;
         J9UTF8 *signature;
         J9UTF8 *className;
         getClassNameSignatureFromMethod(resolvedMethod, className, name, signature);


         sprintf(s, "%.*s%.*s",
                 J9UTF8_LENGTH(name), J9UTF8_DATA(name),
                 J9UTF8_LENGTH(signature), J9UTF8_DATA(signature));
         fprintf(stderr, "Method: %s [%s] size %d\n", s,
                (romMethod->modifiers & J9AccSynchronized) ? "synchronized" : "",
                 methodSize);
         }
      }

   if (DEBUG_RESERVATION_SCAN && candidateClass)
      {
      fprintf(stderr, "Total number of methods: %d\n", numMethods);
      fprintf(stderr, "Total number of synchronized methods: %d\n", numSynchronizedMethods);
      fprintf(stderr, "Total number of non-synchronized methods: %d\n", numNonSynchronizedMethods);
      fprintf(stderr, "Total number of small synchronized methods: %d\n", numSmallSynchronizedMethods);
      fprintf(stderr, "Total number of small non-synchronized methods: %d\n", numSmallNonSynchronizedMethods);
      fprintf(stderr, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
      fprintf(stderr, "\n\n");
      }

   persistentClassInfo->setScannedForReservation();
   int lwOffset = getByteOffsetToLockword(classPointer);

   if ((numSynchronizedMethods>0) &&
       (lwOffset > 0))
      {
      char s[4096];
      J9UTF8 * className = J9ROMCLASS_CLASSNAME(((J9Class *)classPointer)->romClass);
      int32_t len = J9UTF8_LENGTH(className);
      sprintf(s, "%.*s", J9UTF8_LENGTH(className), utf8Data(className));

      if (!strncmp(s, "java/util/Random", 16) ||
          (numNonSynchronizedMethods >0 &&
           ((numSmallNonSynchronizedMethods == 0) ||
            (numSmallSynchronizedMethods >= numSmallNonSynchronizedMethods))))
         persistentClassInfo->setReservable();

      // if lockReserveClass is used, set all the specified classes to be reservable
      TR_SimpleRegex * regex = comp->getOptions()->getLockReserveClass();
      if(regex && TR_Debug::matchRegex(regex, s))
        persistentClassInfo->setReservable();

      if (DEBUG_RESERVATION_SCAN && persistentClassInfo->isReservable())
         {
         char s[4096];
         J9UTF8 * className = J9ROMCLASS_CLASSNAME(((J9Class *)classPointer)->romClass);
         int32_t len = J9UTF8_LENGTH(className);
         sprintf(s, "%.*s", J9UTF8_LENGTH(className), utf8Data(className));
         printf("*****!*!*!*!*!*!*  Reservable class %s\n", s);
         }

      }
   }

uint32_t
TR_J9VM::getInstanceFieldOffset(TR_OpaqueClassBlock * clazz, char * fieldName, uint32_t fieldLen,
                               char * sig, uint32_t sigLen, UDATA options)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   uint32_t result = (uint32_t) vmThread()->javaVM->internalVMFunctions->instanceFieldOffset(
                        vmThread(), (J9Class *)clazz, (unsigned char *) fieldName,
                        fieldLen, (unsigned char *)sig, sigLen, (J9Class **)&clazz, (UDATA *)NULL, options);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   if (result == FIELD_OFFSET_NOT_FOUND)
      return ~0;
   return result;
   }

uint32_t
TR_J9VM::getInstanceFieldOffset(TR_OpaqueClassBlock * clazz, char * fieldName, uint32_t fieldLen,
                                    char * sig, uint32_t sigLen)
   {
   return getInstanceFieldOffset(clazz, fieldName, fieldLen, sig, sigLen, J9_RESOLVE_FLAG_NO_THROW_ON_FAIL);
   }

void
TR_J9VM::initializeProcessorType()
   {
   assume(_compInfo,"compInfo not defined");

   TR_Target target = (TR_Target)jitConfig->targetName;
   if(is390(target))
      {
      // TODO AOT: When AOT is implemented on S390, must ensure that we set processor type to a safe default and/or add compatibility checks
      #if defined(TR_HOST_S390) && !defined(SUPPORT_WCODE)
      if(isZLinux(target))
         {
         _compInfo->setS390MachineType(portLib_get390zLinuxMachineType());
         initializeS390zLinuxProcessorFeatures();
         }
      else
         {
         _compInfo->setS390MachineType(portLib_get390zOSMachineType());
         initializeS390zOSProcessorFeatures();
#if defined(J9ZOS390)
         // Cache whether current process is running in Supervisor State (i.e. Control Region of WAS).
         if (!_isPSWInProblemState())
             _compInfo->setIsInZOSSupervisorState();
#endif
         }
      #else       // if cross-compiling for 390
      _compInfo->setS390MachineType(isZLinux(target) ? TR_TREX : TR_DEFAULT_S390_MACHINE);
      _compInfo->setS390SupportsZArchitecture();
      _compInfo->setS390SupportsZ6();
      _compInfo->setS390SupportsTrex();
      _compInfo->setS390SupportsGoldenEagle();
      if (!TR_Options::getCmdLineOptions()->getOption(TR_DisableDFP))
         _compInfo->setS390SupportsDFP();
      if (!TR_Options::getCmdLineOptions()->getOption(TR_DisableFPE))
         _compInfo->setS390SupportsFPE();
      #endif

      // For AOT shared classes cache processor compatibility purposes, the following
      // processor settings should not be modified.
      if (_compInfo->getS390SupportsZHelix())
         {
         _compInfo->setProcessor(TR_s370gp10);
         }
      else if (_compInfo->getS390SupportsZGryphon())
         {
         _compInfo->setProcessor(TR_s370gp9);
         }
      else if (_compInfo->getS390SupportsZ6())
         {
         _compInfo->setProcessor(TR_s370gp8);
         }
      else if (_compInfo->getS390SupportsGoldenEagle())
         {
         _compInfo->setProcessor(TR_s370gp7);
         }
      else if (_compInfo->getS390SupportsTrex())
         {
         _compInfo->setProcessor(TR_s370gp6);
         }
      else
         {
         _compInfo->setProcessor(TR_s370gp5);
         }

      assume(_compInfo->getProcessor() >= TR_First390Processor
               && _compInfo->getProcessor() <= TR_Last390Processor, "Not a valid 390 Processor Type");
      }
   else if(isARM(target))
      {
      _compInfo->setProcessor(portLibCall_getARMProcessorType());
      assume(_compInfo->getProcessor() >= TR_FirstARMProcessor
             && _compInfo->getProcessor() <= TR_LastARMProcessor, "Not a valid ARM Processor Type");
      }
   else if(isPPC(target))
      {
      //We can have a "reported" processor and a "underlying" processsor.
      //See design 28980
      TR_Processor tp_reported;
      TR_Processor tp_underlying;

      if( isAOT() )
         _compInfo->setProcessor( TR_DefaultPPCProcessor );
      else
         {
         tp_reported    = portLibCall_getProcessorType();
         tp_underlying  = portLibCall_getPhysicalProcessorType();
         _compInfo->setProcessor(tp_reported);
         _compInfo->setSchedulingProcessor(tp_underlying);
         }

      assume(_compInfo->getProcessor() >= TR_FirstPPCProcessor
             && _compInfo->getProcessor() <= TR_LastPPCProcessor, "Not a valid PPC Processor Type");

      #if defined(DEBUG)
      _compInfo->setProcessorByDebugOption();
      #endif
      }
   else if(isX86(target))
      {
      const char *vendor = getX86ProcessorVendorId();
      uint32_t processorSignature = getX86ProcessorSignature();

      _compInfo->setProcessor(portLibCall_getX86ProcessorType(vendor, processorSignature));
      assume(_compInfo->getProcessor() >= TR_FirstX86Processor
             && _compInfo->getProcessor() <= TR_LastX86Processor, "Not a valid X86 Processor Type");
      }
   else
      {
      assume(0,"Unknown target");
      }
   }

void
TR_J9VM::initializeHasResumableTrapHandler()
   {
   assume(_compInfo,"compInfo not defined");
    if(!TR_Options::getCmdLineOptions()->getOption(TR_NoResumableTrapHandler) &&
      portLibCall_sysinfo_has_resumable_trap_handler())
      {
      _compInfo->setHasResumableTrapHandler();
      }
    else
      {
      // In the case where the user specified -Xrs initializeHasResumableTrapHandler
      // will be called twice and we must unset the flag as it will have been set on
      // during the first call.
      _compInfo->setHasResumableTrapHandler(false);
      }
   }

void
TR_J9VM::initializeHasFixedFrameC_CallingConvention()
   {
   assume(_compInfo,"compInfo not defined");
   /*
   J9PortLibrary * portLib = jitConfig->javaVM->portLibrary;

   if(portLib->sysinfo_has_fixed_frame_C_calling_convention(portLib))
      {
      _compInfo->setHasFixedFrameC_CallingConvention();
      }
   */
   if(portLibCall_sysinfo_has_fixed_frame_C_calling_convention())
      {
      _compInfo->setHasFixedFrameC_CallingConvention();
      }
   }

void
TR_J9VM::initializeHasFPU()
   {
   assume(_compInfo,"compInfo not defined");
   /*
   J9PortLibrary * portLib = jitConfig->javaVM->portLibrary;
   if(portLib->sysinfo_has_floating_point_unit(portLib))
      {
     _compInfo->setHasFPU();
      }
   */
   if(portLibCall_sysinfo_has_floating_point_unit())
      {
      _compInfo->setHasFPU();
      }
   }

void
TR_J9VM::initializeNumberBytesInaccessible()
   {
   assume(_compInfo,"compInfo not defined");
   /*
   J9PortLibrary * portLib = jitConfig->javaVM->portLibrary;
   _compInfo->setNumberBytesReadInaccessible(portLib->sysinfo_number_bytes_read_inaccessible(portLib));
   _compInfo->setNumberBytesWriteInaccessible(portLib->sysinfo_number_bytes_write_inaccessible(portLib));
   */

   if (TR_Options::getCmdLineOptions()->getOption(TR_NoResumableTrapHandler))
      {
      _compInfo->setNumberBytesReadInaccessible(0);
      _compInfo->setNumberBytesWriteInaccessible(0);
      }
   else
      {
      _compInfo->setNumberBytesReadInaccessible(portLibCall_sysinfo_number_bytes_read_inaccessible());
      _compInfo->setNumberBytesWriteInaccessible(portLibCall_sysinfo_number_bytes_write_inaccessible());
      }
   }

void
TR_J9VM::initializeSupportsScaledIndexAddressing()
   {
   assume(_compInfo,"compInfo not defined");
   /*
   J9PortLibrary * portLib = jitConfig->javaVM->portLibrary;

   if(portLib->portLibCall_sysinfo_supports_scaled_index_addressing(portLib))
      {
      _compInfo->setSupportsScaledIndexAddressing();
      }
   */
   if(portLibCall_sysinfo_supports_scaled_index_addressing())
      {
      _compInfo->setSupportsScaledIndexAddressing();
      }
   }

#if defined(TR_TARGET_S390)
void
TR_J9VM::initializeS390zLinuxProcessorFeatures()
   {
   TR_Target target = (TR_Target)jitConfig->targetName;
   assume(_compInfo,"compInfo not defined");
   assume(is390(target) && isZLinux(target),"Only valid for s390/zLinux\n");

// The following conditionals are dependent on each other and must occur in this order

   _compInfo->setS390SupportsZArchitecture(); // always support z/Arch

   // Check for Facility bits, which can detect z6/z10 or higher.
   // If STFLE is supported, we must rely on these bits.
   // zVM can spoof as a newer machine model, without really providing the support.
   // However, the facility bits don't lie.
   if (_compInfo->getS390SupportsZArchitecture() &&
      portLibCall_get390zLinux_supportsStoreExtendedFacilityList())
      {
      // Check facility bits for support of hardware
      if (portLibCall_get390_supportsZHelix())
         _compInfo->setS390SupportsZHelix();
      if (portLibCall_get390_supportsZGryphon())
         _compInfo->setS390SupportsZGryphon();
      if (portLibCall_get390_supportsZ6())
         _compInfo->setS390SupportsZ6();

      // z9 (DANU) supports DFP in millicode - so do not check
      // for DFP support unless z10 or higher.
      if (_compInfo->getS390SupportsZ6() && portLibCall_supportsDFP())
         _compInfo->setS390SupportsDFP();
      if (portLibCall_get390_supportsFPE())
         _compInfo->setS390SupportsFPE();

      // If we support z10, we also support z9 and z990.
      if (_compInfo->getS390SupportsZ6())
         {
         _compInfo->setS390SupportsGoldenEagle();
         _compInfo->setS390SupportsTrex();
         }

      }

   // Either z6/z10 wasn't detected or STFLE is not supported.
   // Use machine model.
   if (!_compInfo->getS390SupportsZ6() || !portLibCall_get390zLinux_supportsStoreExtendedFacilityList())
      {
      TR_S390MachineType machineType = _compInfo->getS390MachineType();

      if ( ( (machineType == TR_TREX) || (machineType == TR_MIRAGE ) || (machineType == TR_MIRAGE2) || (machineType == TR_Z890) ) &&
          _compInfo->getS390SupportsZArchitecture())
         {
         _compInfo->setS390SupportsTrex();
         }

      if ( ( (machineType == TR_GOLDEN_EAGLE) || (machineType == TR_DANU_GA2) || (machineType == TR_Z9BC) ) &&
          _compInfo->getS390SupportsZArchitecture())
         {
         _compInfo->setS390SupportsGoldenEagle();
         _compInfo->setS390SupportsTrex();
         }

      // For z6/z10 or higher, only use machine model if STFLE is not supported.  Otherwise,
      // we should have detected it above.
      if (!portLibCall_get390zLinux_supportsStoreExtendedFacilityList())
         {
         if ( ( (machineType == TR_Z10) || (machineType == TR_Z10BC) ) &&
              _compInfo->getS390SupportsZArchitecture())
            {
            _compInfo->setS390SupportsZ6();
            _compInfo->setS390SupportsGoldenEagle();
            _compInfo->setS390SupportsTrex();
            }

         if ( ( (machineType == TR_ZG) || (machineType == TR_ZGMR) || (machineType == TR_ZG_RESERVE) ) &&
              _compInfo->getS390SupportsZArchitecture() )
            {
            _compInfo->setS390SupportsZGryphon();
            _compInfo->setS390SupportsZ6();
            _compInfo->setS390SupportsGoldenEagle();
            _compInfo->setS390SupportsTrex();
            }

         if ( ( (machineType == TR_ZEC12) || (machineType == TR_ZEC12MR) || (machineType == TR_ZEC12_RESERVE) ||
                (machineType == TR_ZNEXT) || (machineType == TR_ZNEXTBC) ) &&
              _compInfo->getS390SupportsZArchitecture() )
            {
            _compInfo->setS390SupportsZHelix();
            _compInfo->setS390SupportsZGryphon();
            _compInfo->setS390SupportsZ6();
            _compInfo->setS390SupportsGoldenEagle();
            _compInfo->setS390SupportsTrex();
            }
         }
      }

   // Only SLES11 SP1 supports HPR debugging facilities, which is
   // required for RAS support for HPR (ZGryphon or higher).
   if (_compInfo->getS390SupportsZGryphon() && portLibCall_get390zLinux_supportsHPRDebug())
      _compInfo->setS390SupportsHPRDebug();

   // Check for OS support of TM.
   if (_compInfo->getS390SupportsZHelix() && portLib_get390zLinux_SupportTM())
      _compInfo->setS390SupportsTM();
   }

void
TR_J9VM::initializeS390zOSProcessorFeatures()
   {
   TR_Target target = (TR_Target)jitConfig->targetName;
   assume(is390(target) && isZOS(target),"Only valid for s390/zOS\n");
   assume(_compInfo,"compInfo not defined");

   if(!portLibCall_get390zOS_N3Support() || !portLibCall_get390zOS_ZArchSupport())
      {
      PORT_ACCESS_FROM_ENV(jitConfig->javaVM);
      j9nls_printf(jitConfig->javaVM->portLibrary, J9NLS_ERROR, J9NLS_J9JIT_390_UNSUPPORTED_HARDWARE, TR_G5);
      assume1(0,"Hardware is not supported.");
      outOfMemory(0, "Hardware is not supported");
      }

    // JIT pre-req's zArchitecture (and N3).
    _compInfo->setS390SupportsZArchitecture();

   if(portLibCall_get390zOS_TrexSupport())
      _compInfo->setS390SupportsTrex();

   if(portLibCall_get390zOS_GoldenEagleSupport())
      {
      _compInfo->setS390SupportsGoldenEagle();
      _compInfo->setS390SupportsTrex();
      }

   // On z10 or higher architectures, we should check for facility bits.
   if(_compInfo->getS390SupportsZArchitecture() &&
      portLibCall_get390zOS_supportsStoreExtendedFacilityList())
      {
      if (portLibCall_get390_supportsZ6())
         _compInfo->setS390SupportsZ6();
      if (portLibCall_get390_supportsZGryphon())
         _compInfo->setS390SupportsZGryphon();
      if (portLibCall_get390_supportsZHelix())
         _compInfo->setS390SupportsZHelix();


      // z9 (DANU) supports DFP in millicode - so do not check
      // for DFP support unless z10 or higher.
      if (_compInfo->getS390SupportsZ6() && portLibCall_supportsDFP())
            _compInfo->setS390SupportsDFP();

      if (portLibCall_get390_supportsFPE())
         _compInfo->setS390SupportsFPE();

      // zOS supports HPR debugging facilities on ZGryphon or higher.
      if (_compInfo->getS390SupportsZGryphon())
         _compInfo->setS390SupportsHPRDebug();

      if (_compInfo->getS390SupportsZHelix())
         {
         // check for zOS TM support
         U_8 * cvtptr = (U_8 *)(*(uint32_t *)16);   // pointer to CVT is at offset +16 of PSA (at address 0)
         U_8 cvttxj = *(cvtptr+0x17b);              // CVTTX and CVTTXC are at offset +0x17B of CVT, bits 8 and 4, indicating full support
         if ((cvttxj & 0xc) == 0xc)
            {
            _compInfo->setS390SupportsTM();
            }
         // check for zOS RI support
         if ((cvttxj & 0x2) == 0x2)                  // CVTRI bit is X'02' bit at byte X'17B' off CVT
            {
            _compInfo->setS390SupportsRI();

            //TR_Options::getCmdLineOptions()->setOption(TR_EnableHardwareProfiling);
            }
         }

      }
   }
#endif

static bool
portLibCall_sysinfo_has_resumable_trap_handler()
   {
   // remove TR_HOST_ and TR_TARGET_ defines once code is moved to each architectures port library

   /* PPC */
   #if defined(TR_HOST_POWER) && defined(TR_TARGET_POWER)
      #if defined(AIXPPC) || defined(LINUX)
         return true;
      #else
         return false;
      #endif
   #endif

   /* X86 */
   #if defined(TR_HOST_X86) && defined(TR_TARGET_X86)
      #if defined(J9HAMMER) || defined(WIN32) || defined(LINUX) // LINUX_POST22
         return true;
      #else
         return false;
      #endif
   #endif

   /* ARM */
   #if defined(TR_HOST_ARM) && defined(TR_TARGET_ARM)
      #ifdef LINUX
         return true;
      #else
         return false;
      #endif
   #endif

   /* 390 */
   #if defined(TR_HOST_S390) && defined(TR_TARGET_S390)
#if defined(J9ZOS390)
    if (!_isPSWInProblemState())
       {
       return false;
       }
#endif
      return true;
   #endif

   }

static bool
portLibCall_sysinfo_has_fixed_frame_C_calling_convention()
   {
   // remove TR_HOST_ and TR_TARGET_ defines once code is moved to each architectures port library

   /* PPC */
  #if defined(TR_HOST_POWER) && defined(TR_TARGET_POWER)
      #if defined(AIXPPC) || defined(LINUX) || defined(OSX)
         return true;
      #else
         return false;
      #endif
   #endif

   /* S390 */
   #if defined(TR_HOST_S390) && defined(TR_TARGET_S390)
       return true;
   #endif

   /* ARM */
   #if defined(TR_HOST_ARM) && defined(TR_TARGET_ARM)
      return false;
   #endif

   /* X86 */
   #if defined(TR_HOST_X86) && defined(TR_TARGET_X86)
      return false;
   #endif

   }

static bool
portLibCall_sysinfo_has_floating_point_unit()
   {
   #if defined(J9VM_ENV_HAS_FPU)
      return true;
   #else
      #if defined(TR_HOST_ARM) && defined(TR_TARGET_ARM) && defined(__VFP_FP__) && !defined(__SOFTFP__)
         return true;
      #else
         return false;
      #endif
   #endif
   }

static uint32_t
portLibCall_sysinfo_number_bytes_read_inaccessible()
   {
   // remove TR_HOST_ and TR_TARGET_ defines once code is moved to each architectures port library

   /* PPC */
   #if defined(TR_HOST_POWER) && defined(TR_TARGET_POWER)
      return 0;
   #endif

   /* X86 */
   #if defined(TR_HOST_X86) && defined(TR_TARGET_X86)
      #if defined(J9HAMMER) || defined(WIN32) || defined(LINUX) // LINUX_POST22
         return 4096;
      #else
         return 0;
      #endif
   #endif

   /* ARM */
   #if defined(TR_HOST_ARM) && defined(TR_TARGET_ARM)
      return 0;
   #endif

   /* 390 */
   #if defined(TR_HOST_S390) && defined(TR_TARGET_S390)
      return 4096;
   #endif

   }

static uint32_t
portLibCall_sysinfo_number_bytes_write_inaccessible()
   {
   // remove TR_HOST_ and TR_TARGET_ defines once code is moved to each architectures port library

   /* PPC */
   #if defined(TR_HOST_POWER) && defined(TR_TARGET_POWER)
      return 4096;
   #endif

   /* X86 */
   #if defined(TR_HOST_X86) && defined(TR_TARGET_X86)
      #if defined(J9HAMMER) || defined(WIN32) || defined(LINUX) // LINUX_POST22
         return 4096;
      #else
         return 0;
      #endif
   #endif

   /* ARM */
   #if defined(TR_HOST_ARM) && defined(TR_TARGET_ARM)
      return 0;
   #endif

   /* 390 */
   #if defined(TR_HOST_S390) && defined(TR_TARGET_S390)
      return 4096;
   #endif

   }

static bool
portLibCall_sysinfo_supports_scaled_index_addressing()
   {
   // remove TR_HOST_ and TR_TARGET_ defines once code is moved to each architectures port library

   /* X86 */
   #if defined(TR_HOST_X86) && defined(TR_TARGET_X86)
      return true;
   #endif

   /* PPC */
   #if defined(TR_HOST_POWER) && defined(TR_TARGET_POWER)
      return false;
   #endif

   /* S390 */
   #if defined(TR_HOST_S390) && defined(TR_TARGET_S390)
       return false;
   #endif

   /* ARM */
   #if defined(TR_HOST_ARM) && defined(TR_TARGET_ARM)
      return false;
   #endif

   }

TR_OpaqueClassBlock *
TR_J9VM::getSuperClass(TR_OpaqueClassBlock * classPointer)
   {
   J9Class * clazz = convertClassOffsetToClassPtr(classPointer);
   UDATA classDepth = (clazz->classDepthAndFlags & J9_JAVA_CLASS_DEPTH_MASK) - 1;
   return convertClassPtrToClassOffset(classDepth >= 0 ? clazz->superclasses[classDepth]: 0);
   }

List<TR_OpaqueClassBlock> *
TR_J9VM::getInterfacesImplementedByClass(TR_OpaqueClassBlock *clazz, List<TR_OpaqueClassBlock> *interfaces )
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   for (J9ITable * iTableEntry = (J9ITable *) convertClassOffsetToClassPtr(clazz)->iTable; iTableEntry; iTableEntry = iTableEntry->next)
      {
      J9Class *interfaceCl = iTableEntry->interfaceClass;
      //while (interfaceCl)
         {
         TR_OpaqueClassBlock * classOffset = convertClassPtrToClassOffset(interfaceCl);
         if (!interfaces->find(classOffset))
            interfaces->add(classOffset);
         //classOffset = getSuperClass(classOffset);
         //interfaceCl = convertClassOffsetToClassPtr(classOffset);
         }
      }
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return interfaces;
   }


bool
TR_J9VM::sameClassLoaders(TR_OpaqueClassBlock * class1, TR_OpaqueClassBlock * class2)
   {
   return (getClassLoader(class1) == getClassLoader(class2));
   }

bool 
TR_J9VM::isUnloadAssumptionRequired(TR_OpaqueClassBlock * clazzPointer, TR_ResolvedMethod * methodBeingCompiled)
   {
   TR_OpaqueClassBlock *classOfMethod = methodBeingCompiled->classOfMethod();
   
   if (clazzPointer == classOfMethod || 
       getClassLoader(clazzPointer) == getSystemClassLoader() ||
       sameClassLoaders(clazzPointer, classOfMethod)
      )
      return false;
      
   return true;
   }

bool
TR_J9VM::classHasBeenExtended(TR_OpaqueClassBlock * clazzPointer)
   {
   return (convertClassOffsetToClassPtr(clazzPointer)->classDepthAndFlags & J9_JAVA_CLASS_HAS_BEEN_OVERRIDDEN) != 0;
   }

bool
TR_J9VM::classHasBeenReplaced(TR_OpaqueClassBlock * clazzPointer)
   {
   return (convertClassOffsetToClassPtr(clazzPointer)->classDepthAndFlags & J9_JAVA_CLASS_HOT_SWAPPED_OUT) != 0;
   }

bool
TR_J9VM::isGetImplInliningSupported()
   {
   J9JavaVM * jvm = _jitConfig->javaVM;
   return jvm->memoryManagerFunctions->j9gc_modron_isFeatureSupported(jvm, j9gc_modron_feature_inline_reference_get) != 0;
   }

bool
TR_J9VM::isInterfaceClass(TR_OpaqueClassBlock * clazzPointer)
   {
   return (getRomClass(clazzPointer)->modifiers & J9AccInterface) ? true : false;
   }

bool
TR_J9VM::isAbstractClass(TR_OpaqueClassBlock * clazzPointer)
   {
   if (isInterfaceClass(clazzPointer))
      return false;
   return (getRomClass(clazzPointer)->modifiers & J9AccAbstract) ? true : false;
   }

bool
TR_J9VM::isPackedObjectClass(TR_OpaqueClassBlock * clazzPointer)
   {
#ifdef J9VM_OPT_PACKED
   if (getClassDepth(clazzPointer) != 0)
      return false;
   return (getRomClass(clazzPointer)->modifiers & J9AccClassPackedObject) ? true : false;
#else
   return false;
#endif
   }

bool
TR_J9VM::isPublicClass(TR_OpaqueClassBlock * clazz)
   {
   return (getRomClass(clazz)->modifiers & J9AccPublic) ? true : false;
   }

bool
TR_J9VM::hasFinalizer(TR_OpaqueClassBlock * classPointer)
   {
   return (convertClassOffsetToClassPtr(classPointer)->classDepthAndFlags & (J9_JAVA_CLASS_FINALIZE | J9_JAVA_CLASS_OWNABLE_SYNCHRONIZER)) != 0;
   }

uintptrj_t
TR_J9VM::getClassDepthAndFlagsValue(TR_OpaqueClassBlock * classPointer)
   {
   return (convertClassOffsetToClassPtr(classPointer)->classDepthAndFlags);
   }

uintptrj_t
TR_J9VM::getClassFlagsValue(TR_OpaqueClassBlock * classPointer)
   {
   return (convertClassOffsetToClassPtr(classPointer)->classFlags);
   }

#ifdef JNI_INLINING
TR_OpaqueMethodBlock *
TR_J9VM::getMethodFromClass(TR_OpaqueClassBlock * methodClass, char * methodName, char * signature)
   {
   J9JNINameAndSignature nameAndSig;
   nameAndSig.name = methodName;
   nameAndSig.nameLength = (U_32)strlen(methodName);
   nameAndSig.signature = signature;
   nameAndSig.signatureLength = (U_32) strlen(signature);
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   TR_OpaqueMethodBlock *result = (TR_OpaqueMethodBlock *) vmThread()->javaVM->internalVMFunctions->javaLookupMethod(vmThread(), 
      (J9Class *)methodClass, (J9ROMNameAndSignature *) &nameAndSig, NULL, LOOKUP_OPTION_JNI | LOOKUP_OPTION_NO_CLIMB | LOOKUP_OPTION_NO_THROW);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

TR_OpaqueMethodBlock *
TR_J9VM::getMethodFromName(char *className, char *methodName, char *signature)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   J9Class *methodClass = jitGetClassInClassloaderFromUTF8(vmThread(), (J9ClassLoader *) vmThread()->javaVM->systemClassLoader, className, strlen(className));
   TR_OpaqueMethodBlock * result = methodClass ? getMethodFromClass((TR_OpaqueClassBlock *)methodClass, methodName, signature) : 0;
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

#else
#define LOOKUP_OPTION_JNI 1024
#define LOOKUP_OPTION_NO_CLIMB 32
#define LOOKUP_OPTION_NO_THROW 8192

TR_OpaqueMethodBlock *
TR_J9VM::getMethodFromName(char *className, char *methodName, char *signature, TR_OpaqueMethodBlock *callingMethod)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   J9Class *methodClass = 0;
   if (callingMethod)
      {
      J9ConstantPool * constantPool = (J9ConstantPool *) (J9_CP_FROM_METHOD((J9Method*)callingMethod));
      methodClass = jitGetClassFromUTF8(vmThread(), constantPool, className, strlen(className));
      }
   if (!methodClass) // try the system class loader
      {
      methodClass = jitGetClassInClassloaderFromUTF8(vmThread(),
         (J9ClassLoader *) vmThread()->javaVM->systemClassLoader,
         className, strlen(className));
      }
   TR_OpaqueMethodBlock * result = NULL;
   if (methodClass)
      result = (TR_OpaqueMethodBlock *)getMethodFromClass((TR_OpaqueClassBlock *)methodClass, methodName, signature);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

TR_OpaqueMethodBlock *
TR_J9VM::getMethodFromClass(TR_OpaqueClassBlock * methodClass, char * methodName, char * signature)
   {
   J9JNINameAndSignature nameAndSig;
   nameAndSig.name = methodName;
   nameAndSig.nameLength = (U_32)strlen(methodName);
   nameAndSig.signature = signature;
   nameAndSig.signatureLength = (U_32) strlen(signature);
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   TR_OpaqueMethodBlock *result = (TR_OpaqueMethodBlock *) vmThread()->javaVM->internalVMFunctions->javaLookupMethod(vmThread(),
      (J9Class *)methodClass, (J9ROMNameAndSignature *) &nameAndSig, NULL, J9_LOOK_JNI | J9_LOOK_NO_THROW);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

#endif

const char *
TR_J9VM::sampleSignature(TR_OpaqueMethodBlock * aMethod, char *buf, int32_t bufLen, TR_Memory *memory)
   {
   J9UTF8 * className;
   J9UTF8 * name;
   J9UTF8 * signature;
   getClassNameSignatureFromMethod(((J9Method *)aMethod), className, name, signature);

   int32_t len = J9UTF8_LENGTH(className)+J9UTF8_LENGTH(name)+J9UTF8_LENGTH(signature)+3;
   char * s = len <= bufLen ? buf : (memory ? (char*)memory->allocateHeapMemory(len) : NULL);
   if (s)
      sprintf(s, "%.*s.%.*s%.*s", J9UTF8_LENGTH(className), utf8Data(className), J9UTF8_LENGTH(name), utf8Data(name), J9UTF8_LENGTH(signature), utf8Data(signature));
   return s;
   }

bool
TR_J9VM::isPrimitiveClass(TR_OpaqueClassBlock * clazz)
   {
   return J9ROMCLASS_IS_PRIMITIVE_TYPE(getRomClass(clazz)) ? true : false;
   }

bool
TR_J9VM::isClassInitialized(TR_OpaqueClassBlock *clazz)
   {
   if (!clazz)
      return false;
   
   return (convertClassOffsetToClassPtr(clazz)->initializeStatus == 1);
   }

TR_OpaqueClassBlock *
TR_J9VM::getComponentClassFromArrayClass(TR_OpaqueClassBlock * arrayClass)
   {
   return convertClassPtrToClassOffset(((J9ArrayClass*)convertClassOffsetToClassPtr(arrayClass))->componentType);
   }

TR_OpaqueClassBlock *
TR_J9VM::getArrayClassFromComponentClass(TR_OpaqueClassBlock * componentClass)
   {
   return convertClassPtrToClassOffset(convertClassOffsetToClassPtr(componentClass)->arrayClass);
   }

TR_OpaqueClassBlock *
TR_J9VM::getLeafComponentClassFromArrayClass(TR_OpaqueClassBlock * arrayClass)
   {
   return convertClassPtrToClassOffset(((J9ArrayClass*)convertClassOffsetToClassPtr(arrayClass))->leafComponentType);
   }

TR_OpaqueClassBlock *
TR_J9VM::getClassFromNewArrayType(int32_t arrayType)
   {
   struct J9Class ** arrayClasses = &_jitConfig->javaVM->booleanArrayClass;
   return convertClassPtrToClassOffset(arrayClasses[arrayType - 4]);
   }

uint32_t
TR_J9VM::getPrimitiveArrayOffsetInJavaVM(uint32_t arrayType)
   {
   assume(arrayType >= 4 && arrayType <= 11, "primitive array types must be between 4 and 11");
   return offsetof(J9JavaVM, booleanArrayClass) + (arrayType-4) * sizeof(J9Class*);
   }

bool
TR_J9VM::isReferenceArray(TR_OpaqueClassBlock *klass)
   {
   return isClassArray(klass) && !isPrimitiveArray(klass);
   }

TR_OpaqueClassBlock *
TR_J9VM::getSystemClassFromClassName(char * name, int32_t length)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   TR_OpaqueClassBlock * result = convertClassPtrToClassOffset(jitGetClassInClassloaderFromUTF8(vmThread(), (J9ClassLoader*)vmThread()->javaVM->systemClassLoader, name, length));
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

void *
TR_J9VM::getSystemClassLoader()
   {
   return vmThread()->javaVM->systemClassLoader;
   }

bool
TR_J9VM::acquireClassTableMutex()
   {
   // Get VM access before acquiring the ClassTableMutex and keep it until after
   // we release the ClassTableMutex
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   jitAcquireClassTableMutex(vmThread());
   return haveAcquiredVMAccess;
   }

void
TR_J9VM::releaseClassTableMutex(bool releaseVMAccess)
   {
   jitReleaseClassTableMutex(vmThread());
   releaseVMAccessIfNeeded(releaseVMAccess);
   }

bool
TR_J9VM::jitFieldsAreSame(TR_ResolvedMethod * method1, I_32 cpIndex1, TR_ResolvedMethod * method2, I_32 cpIndex2, int32_t isStatic)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   bool result = false;

   bool sigSame = true;
   if (method1->fieldsAreSame(cpIndex1, method2, cpIndex2, sigSame))
      result = true;
   else
      {
      if (sigSame)
         result = jitFieldsAreIdentical(vmThread(), (J9ConstantPool *)method1->constantPool(), cpIndex1, (J9ConstantPool *)method2->constantPool(), cpIndex2, isStatic) != 0;
      }

   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

bool
TR_J9VM::jitStaticsAreSame(TR_ResolvedMethod *method1, I_32 cpIndex1, TR_ResolvedMethod *method2, I_32 cpIndex2)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   bool result = false;
   bool sigSame = true;
   if (method1->staticsAreSame(cpIndex1, method2, cpIndex2, sigSame))
      result = true;
   else
      {
      if (sigSame)
         result = jitFieldsAreIdentical(vmThread(), (J9ConstantPool *)method1->constantPool(), cpIndex1, (J9ConstantPool *)method2->constantPool(), cpIndex2, true) != 0;
      }

   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }


TR_MCCCodeCache *
TR_J9VM::getResolvedTrampoline(TR_Compilation *comp, TR_MCCCodeCache* curCache, J9Method * method, bool inBinaryEncoding)
   {
   bool hadClassUnloadMonitor;
   bool hadVMAccess = releaseClassUnloadMonitorAndAcquireVMaccessIfNeeded(comp, &hadClassUnloadMonitor);
   TR_MCCCodeCache* newCache = curCache; // optimistically assume as can allocate from current code cache

   int32_t retValue = curCache->reserveResolvedTrampoline(method, inBinaryEncoding);
   if (retValue != TR_MCCErrorCode::ERRORCODE_SUCCESS)
      {
      curCache->unreserve();  // delete the old reservation
      if (retValue == TR_MCCErrorCode::ERRORCODE_INSUFFICIENTSPACE && !inBinaryEncoding) // code cache full, allocate a new one
         {
         // Allocate a new code cache and try again
         newCache = TR_MCCManager::getNewCodeCache(comp->getCompThreadID(), comp->getDeFactoHotness(), isAOT()); // class unloading may happen here
         if (newCache)
            {
            // check for class unloading that can happen in getNewCodeCache
            if (((TR_CompilationInfoPerThreadBase*)(comp->trMemory()->getCompilationInfoPerThread()))->compilationShouldBeInterrupted())
               {
               newCache->unreserve(); // delete the reservation
               newCache = NULL; // this will fail the compilation
               comp->setErrorCode(COMPILATION_INTERRUPTED); // this will allow retrial of the compilation
               }
            else
               {
               int32_t retValue = newCache->reserveResolvedTrampoline(method, inBinaryEncoding);
               if (retValue != TR_MCCErrorCode::ERRORCODE_SUCCESS)
                  {
                  newCache->unreserve(); // delete the reservation
                  newCache = NULL; // this will fail the compilation
                  comp->setErrorCode(COMPILATION_RESERVE_RESOLVED_TRAMPOLINE_FATAL_ERROR);
                  }
               }
            }
         else // cannot allocate a new code cache to reserve trampolines
            {
            comp->setErrorCode(COMPILATION_RESERVE_RESOLVED_TRAMPOLINE_INSUFFICIENT_SPACE);
            }
         }
      else
         {
         newCache = NULL; // fail this compilation
         if (inBinaryEncoding)
            comp->setErrorCode(COMPILATION_RESERVE_RESOLVED_TRAMPOLINE_ERROR_INBINARYENCODING);
         else
            comp->setErrorCode(COMPILATION_RESERVE_RESOLVED_TRAMPOLINE_ERROR);
         }
      }
   acquireClassUnloadMonitorAndReleaseVMAccessIfNeeded(comp, hadVMAccess, hadClassUnloadMonitor);

   return newCache;
   }

intptrj_t
TR_J9VM::methodTrampolineLookup(TR_Compilation *comp, TR_SymbolReference * symRef, void * callSite)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   assume(!symRef->isUnresolved(), "No need to lookup trampolines for unresolved methods.\n");
   J9Method * method = (J9Method *)symRef->getSymbol()->castToResolvedMethodSymbol()->getResolvedMethod()->getPersistentIdentifier();

   void *tramp;
   TR_MethodSymbol *methodSym = symRef->getSymbol()->castToMethodSymbol();
   switch (methodSym->getMandatoryRecognizedMethod())
      {
      case java_lang_invoke_ComputedCalls_dispatchJ9Method:
         {
         // TODO:JSR292: Get the proper helper based on the target j9method (or select it dynamically inside the thunk).
         // This is a hack, and it appears more than once.  Search for PROPER_DISPATCH_J9METHOD.
         //
         TR_RuntimeHelper vmCallHelper = TR_MethodSymbol::getVMCallHelperFor(methodSym->getMethod()->returnType(), methodSym->isSynchronised(), false, comp);
         tramp = (void *)TR_MCCManager::findHelperTrampoline(callSite, vmCallHelper);
         }
         break;
      default:
         tramp = (void *)TR_MCCManager::findMethodTrampoline(method, callSite);
         break;
      }

   assume(tramp!=NULL, "It should not fail since it is reserved first.\n");
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return (intptrj_t)tramp;
   }

TR_OpaqueClassBlock *
TR_J9VM::getBaseComponentClass(TR_OpaqueClassBlock * clazz, int32_t & numDims)
   {
   J9Class * myClass = convertClassOffsetToClassPtr(clazz);
   while (J9ROMCLASS_IS_ARRAY(myClass->romClass))
      {
      J9Class * componentClass = (J9Class *)(((J9ArrayClass*)myClass)->componentType);
      if (J9ROMCLASS_IS_PRIMITIVE_TYPE(componentClass->romClass))
         break;
      numDims++;
      myClass = componentClass;
      }
   return convertClassPtrToClassOffset(myClass);
   }




TR_YesNoMaybe
TR_J9VM::isInstanceOf(TR_OpaqueClassBlock * a, TR_OpaqueClassBlock *b, bool objectTypeIsFixed, bool castTypeIsFixed, bool optimizeForAOT)
   {
   TR_YesNoMaybe result = TR_maybe;
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   while (isClassArray(a) && isClassArray(b))
      {
      // too many conversions back and forth in this loop
      // maybe we should have specialized methods that work on J9Class pointers
      a = getComponentClassFromArrayClass(a);
      b = getComponentClassFromArrayClass(b);
      }
   J9Class * objectClass   = convertClassOffsetToClassPtr(a);
   J9Class * castTypeClass = convertClassOffsetToClassPtr(b);
   bool objectClassIsInstanceOfCastTypeClass = jitCTInstanceOf(objectClass, castTypeClass) != 0;

   if (castTypeIsFixed && objectClassIsInstanceOfCastTypeClass)
      result = TR_yes;
   else if (objectTypeIsFixed && !objectClassIsInstanceOfCastTypeClass)
      result = TR_no;
   else if (!isInterfaceClass(b) && !isInterfaceClass(a) &&
            !objectClassIsInstanceOfCastTypeClass &&
            !jitCTInstanceOf(castTypeClass, objectClass))
      result = TR_no;
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

bool
TR_J9VM::isPrimitiveArray(TR_OpaqueClassBlock *klass)
   {
   J9Class * j9class = convertClassOffsetToClassPtr(klass);
   if (!J9ROMCLASS_IS_ARRAY(j9class->romClass))
      return false;
   j9class = (J9Class *)(((J9ArrayClass*)j9class)->componentType);
   return J9ROMCLASS_IS_PRIMITIVE_TYPE(j9class->romClass) ? true : false;
   }

TR_OpaqueClassBlock *
TR_J9VM::getClassFromSignature(char * sig, int32_t sigLength, TR_ResolvedMethod * method, bool isVettedForAOT)
   {
   return getClassFromSignature(sig, sigLength, (TR_OpaqueMethodBlock *)method->getPersistentIdentifier(), isVettedForAOT);
   }

TR_OpaqueClassBlock *
TR_J9VM::getClassFromSignature(char * sig, int32_t sigLength, TR_OpaqueMethodBlock * method, bool isVettedForAOT)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   J9ConstantPool * constantPool = (J9ConstantPool *) (J9_CP_FROM_METHOD((J9Method*)method));
   J9Class * j9class = NULL;
   TR_OpaqueClassBlock * returnValue = NULL;

   // For a non-array class type, strip off the first 'L' and last ';' of the
   // signature
   //
   if (* sig == 'L' && sigLength > 2)
      {
      sig += 1;
      sigLength -= 2;
      }


   j9class = jitGetClassFromUTF8(vmThread(), constantPool, sig, sigLength);

   if (j9class == NULL)
      {
      // special case -- classes in java.* and jit helper packages MUST be defined in the bootstrap class loader
      // however callers must account for the possibility that certain ClassLoaders may refuse to
      // allow these classes to be loaded at all. i.e. a non-NULL return from this helper DOES NOT
      // mean that a resolve can be safely removed
      // TODO: this will not find arrays
      if ((sigLength > 5 && strncmp(sig, "java/", 5) == 0) || 
         (sigLength == 31 && strncmp(sig, "com/ibm/jit/DecimalFormatHelper", 31) == 0) ||
         (sigLength > 21 && strncmp(sig, "com/ibm/jit/JITHelpers", 22) == 0))
         {
         returnValue = getSystemClassFromClassName(sig, sigLength);
         }
      }
   else
      {
         returnValue = convertClassPtrToClassOffset(j9class);
      }
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return returnValue; // 0 means failure
   }

void
TR_J9VM::transformJavaLangClassIsArrayOrIsPrimitive(TR_Compilation * comp, TR_Node * callNode, TR_TreeTop * treeTop, int32_t andMask)
   {
   // Original for J2SE (jclmax may be different):
   //
   // treetop (may be resolve check and/or null check)
   //   iicall                   <= callNode
   //     aload <parm 1>         <= vftField
   //
   //
   // Final:
   //
   // treetop
   //   i2b                      <= callNode
   //     ishr                   <= shiftNode
   //       iand                 <= andNode
   //         iiload             <= isArrayField
   //           iaload
   // if (generateClassesOnHeap())
   //             iaload
   // endif
   //               aload <parm 1> <= vftField
   //         iconst <andMask>   <= andConstNode
   //       iconst 16
   //
   TR_Node * isArrayField, * vftField, * andConstNode;
   TR_SymbolReferenceTable *symRefTab = comp->getSymRefTab();

   vftField = callNode->getFirstChild();

   TR_Node * vftFieldInd;

   if (comp->fe()->generateClassesOnHeap())
      {
      vftFieldInd = TR_Node::create(comp, TR_iaload, 1, vftField, comp->getSymRefTab()->findOrCreateClassFromJavaLangClassSymbolRef());
      isArrayField = TR_Node::create(comp, TR_iaload,1,vftFieldInd,symRefTab->findOrCreateClassRomPtrSymbolRef());
      }
   else
      {
      isArrayField = TR_Node::create(comp, TR_iaload,1,vftField,symRefTab->findOrCreateClassRomPtrSymbolRef());
      vftFieldInd  = isArrayField; //pkalle
      }

   if (treeTop->getNode()->getOpCode().isNullCheck())
      {
      // Original tree had a null check before we transformed it, so we still
      // need one after
      //
      TR_ResolvedMethodSymbol *owningMethodSymbol = treeTop->getNode()->getSymbolReference()->getOwningMethodSymbol(comp);
      TR_TreeTop::create(comp, treeTop->getPrevTreeTop(),
         TR_Node::create(comp, TR_NULLCHK, 1, vftFieldInd, symRefTab->findOrCreateNullCheckSymbolRef(owningMethodSymbol)));
      }

   callNode->setOpCodeValue(TR_idiv);
   callNode->setNumChildren(2);

   isArrayField = TR_Node::create(comp, TR_iiload,1, isArrayField, comp->getSymRefTab()->findOrCreateClassIsArraySymbolRef());
   andConstNode = TR_Node::create(comp, isArrayField, TR_iconst, 0, andMask);

   TR_Node * andNode   = TR_Node::create(comp, TR_iand, 2, isArrayField, andConstNode);

   callNode->setAndIncChild(0, andNode);
   callNode->setAndIncChild(1, TR_Node::create(comp, TR_iconst, 0, andMask));
   treeTop->getNode()->setOpCodeValue(TR_treetop);

   vftField->decReferenceCount();
   }

// Hack markers
//
#define HELPERS_ARE_NEITHER_RESOLVED_NOR_UNRESOLVED (1)

TR_Node *
TR_J9VM::inlineNativeCall(TR_Compilation * comp, TR_TreeTop * callNodeTreeTop, TR_Node * callNode)
   {
   // Mandatory recognized methods: if we don't handle these specially, we
   // generate incorrect code.
   //
   RecognizedMethod mandatoryMethodID = callNode->getSymbol()->castToResolvedMethodSymbol()->getMandatoryRecognizedMethod();
   switch (mandatoryMethodID)
      {
      case java_lang_invoke_ComputedCalls_dispatchDirect:
      case java_lang_invoke_ComputedCalls_dispatchVirtual:
         {
         // The first argument to these calls is actually the target address masquerading as a long argument
         TR_MethodSymbol *methodSymbol = callNode->getSymbol()->castToMethodSymbol();
         methodSymbol->setMethodKind(TR_MethodSymbol::ComputedStatic);
         callNode->setOpCodeValue(methodSymbol->getMethod()->indirectCallOpCode());
         return callNode;
         }
      case java_lang_invoke_ComputedCalls_dispatchJ9Method:
         {
         TR_MethodSymbol *sym = callNode->getSymbol()->castToMethodSymbol();
         sym->setVMInternalNative(false);
         sym->setInterpreted(false);
         TR_Method *method = sym->getMethod();

         // TODO:JSR292: Get the proper helper based on the target j9method (or select it dynamically inside the thunk).
         // This is a hack, and it appears more than once.  Search for PROPER_DISPATCH_J9METHOD.
         //
         TR_RuntimeHelper vmCallHelper = TR_MethodSymbol::getVMCallHelperFor(method->returnType(), sym->isSynchronised(), false, comp);
         TR_SymbolReference *helperSymRef = comp->getSymRefTab()->findOrCreateRuntimeHelper(vmCallHelper, true, true, false);
         sym->setMethodAddress(helperSymRef->getMethodAddress());
         return callNode;
         }
      case java_lang_invoke_MethodHandle_invokeWithArgumentsHelper:
         {
         // This method is implemented as a VM helper.  It must alter the stack
         // frame shape insuch bizarre ways that it needs special treatment.
         //
         TR_SymbolReference *helperSymRef = comp->getSymRefTab()->findOrCreateRuntimeHelper(TR_icallVMprJavaSendInvokeWithArguments, true, true, false);
         if (HELPERS_ARE_NEITHER_RESOLVED_NOR_UNRESOLVED)
            {
            // Can't just use the the helper symbol because those aren't
            // unresolved yet don't use TR_ResolvedMethodSymbol.  That confuses
            // everyone.  We ought to fix that.  Instead, for now, transmute
            // the existing symbol so it acts like the helper.
            //
            TR_MethodSymbol *sym = callNode->getSymbol()->castToMethodSymbol();
            sym->setVMInternalNative(false);
            sym->setJITInternalNative(false);
            sym->setInterpreted(false);
            sym->setMethodAddress(helperSymRef->getMethodAddress());
            }
         else
            {
            callNode->setSymbolReference(helperSymRef);
            }
         return callNode;
         }

      }


   TR_ResolvedMethod *resolvedMethod = callNode->getSymbol()->castToResolvedMethodSymbol()->getResolvedMethod();

   //this is commented out to test recognized methods for AOT
   //if (isAnyMethodTracingEnabled(resolvedMethod->getPersistentIdentifier()))
   //   return NULL;

   // Ordinary recognized methods: these are for performance, and can be
   // disabled without harming correctness.
   //
   RecognizedMethod methodID = callNode->getSymbol()->castToResolvedMethodSymbol()->getRecognizedMethod();
   switch (methodID)
      {
      case java_lang_Class_isArray:
         transformJavaLangClassIsArrayOrIsPrimitive(comp, callNode, callNodeTreeTop, J9AccClassArray);
         return callNode;
      case java_lang_Class_isPrimitive:
         transformJavaLangClassIsArrayOrIsPrimitive(comp, callNode, callNodeTreeTop, J9AccClassInternalPrimitiveType);
         return callNode;
      case java_lang_Class_isInstance:
         if (  comp->cg()->supportsInliningOfIsInstance()
            && performTransformation2c(comp, "O^O inlineNativeCall: generate instanceof [%p] for Class.isInstance at bytecode 0x%x\n", callNode, callNode->getByteCodeInfo().getByteCodeIndex()) )
            {
            if (comp->getOption(TR_TraceILGen) && comp->getDebug())
               {
               TR_BitVector nodeChecklistBeforeDump(comp->getNodeCount(), comp->trMemory(), stackAlloc, growable);
               trace0c(comp, "   /--- Class.isInstance call tree --\n");

               comp->getDebug()->saveNodeChecklist(nodeChecklistBeforeDump);
               comp->getDebug()->dumpSingleTreeWithInstrs(callNodeTreeTop, NULL, true, false, true, false);
               comp->getDebug()->restoreNodeChecklist(nodeChecklistBeforeDump);

               trace0c(comp, "\n");
               }

            assume(!callNode->getOpCode().isIndirect(), "Expecting direct call to Class.isInstance");

            TR_Node *jlClass = callNode->getChild(0);
            TR_Node *object  = callNode->getChild(1);

            // If there's a NULLCHK on the j/l/Class, we must retain that.  We
            // do this by pulling the call out from under the nullchk before
            // attempting to transform the call.
            //
            if (callNodeTreeTop->getNode()->getOpCode().isNullCheck())
               {
               // Put the call under its own tree after the NULLCHK
               //
               TR_TreeTop::create(comp, callNodeTreeTop, TR_Node::create(comp, TR_treetop, 1, callNode));

               // Replace the call under the nullchk with a PassThrough of the jlClass
               //
               TR_Node *nullchk = callNodeTreeTop->getNode();
               nullchk->getAndDecChild(0);
               nullchk->setAndIncChild(0, TR_Node::create(comp, TR_PassThrough, 1, jlClass));
               }

            // Transmute the call into an instanceof.
            // Note that the arguments on the call are backward.
            //
            TR_Node *j9class = TR_Node::create(comp, TR_iaload, 1, jlClass, 
                                                comp->getSymRefTab()->findOrCreateClassFromJavaLangClassSymbolRef());
            callNode->setOpCodeValue(TR_instanceof);
            callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateInstanceOfSymbolRef(comp->getMethodSymbol()));
            callNode->getAndDecChild(0); // jlClass
            callNode->setChild(0, object);
            callNode->setAndIncChild(1, j9class);

            return callNode;
            }
         else
            {
            return NULL;
            }
      case java_lang_Object_getClass:
         callNode->setOpCodeValue(TR_iaload);
         callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateVftSymbolRef());
         if (comp->fe()->generateClassesOnHeap())
            {
            callNode = TR_Node::create(comp, TR_iaload, 1, callNode, comp->getSymRefTab()->findOrCreateJavaLangClassFromClassSymbolRef());
            }
         return callNode;
#if 1 // Java 6 SR1 code
      // Note: these cases are not tested and thus are commented out:
      // - com.ibm.oti.vm.VM.getStackClass(int)
      // - com.ibm.oti.vm.VM.getStackClassLoader(int)
      // - java/lang/Class.getStackClass(int)

      //case com_ibm_oti_vm_VM_getStackClass:
      //   // A() -> B() -> c/i/o/v/VM.getStackClass(1) => A()
      //   // 1      0      -1
      //   // fall through intended
      //case com_ibm_oti_vm_VM_getStackClassLoader:
      //   // A() -> B() -> c/i/o/v/VM.getStackClassLoader(1) => A()
      //   // 1      0      -1
      //   // fall through intended
      //case java_lang_Class_getStackClass:
      //   // A() -> B() -> j/l/C.getStackClass(1) => A()
      //   // 1      0      -1
      //   // fall through intended

      case java_lang_ClassLoader_getStackClassLoader:
         // A() -> B() -> j/l/CL.getStackClassLoader(1) => A()
         // 1      0      -1
         // fall through intended
         return 0;   // TW FIXME:: disabled for now
      case java_lang_invoke_MethodHandles_getStackClass:
      case sun_reflect_Reflection_getCallerClass:
         // A() -> B() -> s/r/R.getCallerClass(1) => B()
         // 2      1      0
         {
         //we need to bail out since we create a class pointer const with cpIndex of -1
         if (isAOT() || ((!(vmThread()->javaVM->extendedRuntimeFlags &  J9_EXTENDED_RUNTIME_ALLOW_GET_CALLER_CLASS)) &&
	         methodID == sun_reflect_Reflection_getCallerClass))
            {
            return 0;
            }
         // char *caseName = (methodID == sun_reflect_Reflection_getCallerClass) ? "s/r/R.getCallerClass" : "j/l/CL.getStackClassLoader";
         TR_Node *iconstNode = callNode->getFirstChild();
         if (iconstNode->getOpCodeValue() == TR_iconst)
            {
            int32_t stackDepth = iconstNode->getInt();
            if (stackDepth <= 0)
               return 0;   // getCallerClass is meaningless when invoked with depth <= 0
            int32_t callerIndex = callNode->getByteCodeInfo().getCallerIndex();
            int32_t inlineDepth;
            J9Class *callerClass;
            J9Method *callerMethod;

            if (methodID == sun_reflect_Reflection_getCallerClass)
               inlineDepth = 1;
            else
               inlineDepth = 0;   // traverse one frame more than getCallerClass

            //printf("\nTW: trying to inline %s at node 0x%p for jitted method %s\n", caseName, callNode, comp->signature());
            bool skipFrame;  // the logic of computing skipFrame should match with what vm does
            while (true)
               {
               if (callerIndex != -1)
                  {
                  callerMethod = (J9Method *) comp->getInlinedCallSite(callerIndex)._methodInfo;
                  if (isAOT())
                     {
                     callerClass = (J9Class*) ((TR_AOTMethodInfo *)callerMethod)->resolvedMethod->containingClass();
                     }
                  else
                     {
                     callerClass = J9_CLASS_FROM_METHOD(callerMethod);
                     }
                  //#define SIG_SZ 150
                  //char sig[SIG_SZ];  // hopefully the size is good for most cases
                  //printTruncatedSignature(sig, SIG_SZ, (TR_OpaqueMethodBlock*)callerMethod);
                  //printf("   TW%d: caller method signature = %s\n", inlineDepth, sig);
                  //#undef SIG_SZ
                  }
               else
                  {
                  TR_ResolvedMethodSymbol *callerMethodSymbol = comp->getJittedMethodSymbol();
                  callerMethod = (J9Method *) callerMethodSymbol->getResolvedMethod()->getPersistentIdentifier();
                  callerClass = (J9Class*) callerMethodSymbol->getResolvedMethod()->classOfMethod();
                  //#define SIG_SZ 150
                  //char sig[SIG_SZ];  // hopefully the size is good for most cases
                  //printTruncatedSignature(sig, SIG_SZ, (TR_OpaqueMethodBlock*)callerMethod);
                  //printf("   TW%d: caller method signature = %s\n", inlineDepth, sig);
                  //#undef SIG_SZ
                  }

              bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();

               if (vmThread()->javaVM->jlrMethodInvoke == NULL)
                  {
                  releaseVMAccessIfNeeded(haveAcquiredVMAccess);
                     return 0;
                  }

               skipFrame = false;
               skipFrame = (vmThread()->javaVM->jlrMethodInvoke == callerMethod);
               if (!skipFrame)
                  skipFrame = (vmThread()->javaVM->jlrAccessibleObject != NULL) &&
                               isInstanceOf((TR_OpaqueClassBlock*) callerClass,
                                            (TR_OpaqueClassBlock*) J9VM_J9CLASS_FROM_JCLASS(vmThread(), vmThread()->javaVM->jlrAccessibleObject),
                                            false);
#if defined(J9VM_OPT_SIDECAR)
               if (!skipFrame)
                  skipFrame = (vmThread()->javaVM->srMethodAccessor != NULL) &&
                               isInstanceOf((TR_OpaqueClassBlock*) callerClass,
                                            (TR_OpaqueClassBlock*) J9VM_J9CLASS_FROM_JCLASS(vmThread(), vmThread()->javaVM->srMethodAccessor),
                                            false);
               if (!skipFrame)
                  skipFrame = (vmThread()->javaVM->srConstructorAccessor != NULL) &&
                               isInstanceOf((TR_OpaqueClassBlock*) callerClass,
                                            (TR_OpaqueClassBlock*) J9VM_J9CLASS_FROM_JCLASS(vmThread(), vmThread()->javaVM->srConstructorAccessor),
                                            false);

               releaseVMAccessIfNeeded(haveAcquiredVMAccess);
#endif // J9VM_OPT_SIDECAR
#if defined(J9VM_OPT_REAL_TIME_EXTENSIONS)
               if ((J9Method*)vmThread()->javaVM->jxrMAnewInstance1 == NULL || (J9Method*)vmThread()->javaVM->jxrMAnewInstance2 == NULL)
                     return 0;

               if (!skipFrame)
                  skipFrame = ((J9Method*)vmThread()->javaVM->jxrMAnewInstance1 == callerMethod ||
                               (J9Method*)vmThread()->javaVM->jxrMAnewInstance2 == callerMethod);
#endif // J9VM_OPT_REAL_TIME_EXTENSIONS
               //printf("   TW%d: skipFrame is %s\n", inlineDepth, skipFrame ? "true" : "false");

               if (!skipFrame && inlineDepth == stackDepth)
                  break;
               if (callerIndex == -1)
                  break;
               if (!skipFrame)
                  inlineDepth++;
               callerIndex = comp->getInlinedCallSite(callerIndex)._byteCodeInfo.getCallerIndex();
               }

            if (!skipFrame && (inlineDepth == stackDepth))
               {
               bool returnClassLoader;
               switch (methodID)
                  {
                  case sun_reflect_Reflection_getCallerClass:
                  case java_lang_invoke_MethodHandles_getStackClass:
                  //case com_ibm_oti_vm_VM_getStackClass:
                  //case java_lang_Class_getStackClass:
                     returnClassLoader = false;
                     break;
                  default:
                     returnClassLoader = true;
                     break;
                  }

               //printf("   TW: sucess to inline at stackDepth=%d (inlineDepth=%d)\n", stackDepth, inlineDepth);
               if (returnClassLoader)
                  {
                  if (performTransformation3c(comp, "O^O inlineNativeCall: inline classloader load [%p] for '%s' at bytecode 0x%x\n",
                        callNode, callNode->getSymbolReference()->getName(comp->getDebug()), callNode->getByteCodeInfo().getByteCodeIndex()))
                     {
                     callNode->setOpCodeValue(TR_aload);
                     callNode->removeAllChildren();
                     TR_SymbolReference *callerClassLoaderSymRef = comp->getSymRefTab()->findOrCreateClassLoaderSymbolRef(comp->getMethodSymbol()->getResolvedMethod());
                     callNode->setSymbolReference(callerClassLoaderSymRef);
                     }
                  }
               else
                  {
                  int32_t classNameLength;
                  char *className = comp->fe()->getClassNameChars((TR_OpaqueClassBlock *) callerClass, classNameLength);
                  if (performTransformation5c(comp, "O^O inlineNativeCall: inline class load [%p] of %.*s for '%s' at bytecode 0x%x\n", callNode,
                        classNameLength, className,
                        callNode->getSymbolReference()->getName(comp->getDebug()), callNode->getByteCodeInfo().getByteCodeIndex()))
                     {
                     callNode->setOpCodeValue(TR_loadaddr);
                     callNode->removeAllChildren();
                     TR_SymbolReference *callerClassSymRef = comp->getSymRefTab()->findOrCreateClassSymbol(comp->getMethodSymbol(), -1, convertClassPtrToClassOffset(callerClass));
                     callNode->setSymbolReference(callerClassSymRef);
                     if (comp->fe()->generateClassesOnHeap())
                        {
                        callNode = TR_Node::create(comp, TR_iaload, 1, callNode, comp->getSymRefTab()->findOrCreateJavaLangClassFromClassSymbolRef());
                        }
                     }
                  }
               }
            else
               {
               //printf("   TW: fail to inline: inlineDepth=%d, stackDepth=%d\n", inlineDepth, stackDepth);
               return 0;
               }
            }
         else
            {
            //printf("TW: fail to inline %s at node 0x%p for jitted method %s\n", caseName, callNode, comp->signature());
            return 0;
            }
         //fflush(stdout);
         }
         return callNode;
#endif // Java 6 SR1 code
      case java_lang_Thread_currentThread:
         if (comp->cg()->getGRACompleted())
            {
            return 0;
            }
         else
            {
            comp->cg()->setInlinedGetCurrentThreadMethod();

            if (comp->cg()->getSupportsVMThreadGRA())
               comp->cg()->resetSupportsVMThreadGRA();

            callNode->setOpCodeValue(TR_aload);
            callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateCurrentThreadSymbolRef());

            return callNode;
            }
      case java_lang_Float_intBitsToFloat:
         if (comp->cg()->getSupportsInliningOfTypeCoersionMethods())
            callNode->setOpCodeValue(TR_ibits2f);
         return callNode;
      case java_lang_Float_floatToIntBits:
         if (comp->cg()->getSupportsInliningOfTypeCoersionMethods())
            {
            callNode->setOpCodeValue(TR_fbits2i);
            callNode->setNormalizeNanValues(true, comp);
            }
         return callNode;
      case java_lang_Float_floatToRawIntBits:
         if (comp->cg()->getSupportsInliningOfTypeCoersionMethods())
            {
            callNode->setOpCodeValue(TR_fbits2i);
            callNode->setNormalizeNanValues(false, comp);
            }
         return callNode;
      case java_lang_Double_longBitsToDouble:
         if (comp->cg()->getSupportsInliningOfTypeCoersionMethods())
            {
            callNode->setOpCodeValue(TR_lbits2d);
            }
         return callNode;
      case java_lang_Double_doubleToLongBits:
         if (comp->cg()->getSupportsInliningOfTypeCoersionMethods())
            {
            callNode->setOpCodeValue(TR_dbits2l);
            callNode->setNormalizeNanValues(true, comp);
            }
         return callNode;
      case java_lang_Double_doubleToRawLongBits:
         if (comp->cg()->getSupportsInliningOfTypeCoersionMethods())
            {
            callNode->setOpCodeValue(TR_dbits2l);
            callNode->setNormalizeNanValues(false, comp);
            }
         return callNode;
      case java_lang_ref_Reference_getImpl:
         if (comp->getGetImplInlineable())
            {
            // Retrieve the offset of the instance "referent" field from Reference class
            TR_SymbolReference * callerSymRef = callNode->getSymbolReference();
            TR_ResolvedMethod * owningMethod = callerSymRef->getOwningMethod(comp);
            int32_t len = owningMethod->classNameLength();
            char * s = classNameToSignature(owningMethod->classNameChars(), len, comp);
            TR_OpaqueClassBlock * ReferenceClass = comp->fe()->getClassFromSignature(s, len, owningMethod);
            // defect 143867, ReferenceClass == 0 and crashed later in findFieldInClass()
            if (!ReferenceClass)
               return 0;
            int32_t offset =
               comp->fe()->getInstanceFieldOffset(ReferenceClass, REFERENCEFIELD, REFERENCEFIELDLEN, REFERENCERETURNTYPE, REFERENCERETURNTYPELEN, J9_RESOLVE_FLAG_INIT_CLASS);
            offset += (int32_t)comp->fe()->getObjectHeaderSizeInBytes();  // size of a J9 object header to move past it

            // Generate reference symbol
            TR_SymbolReference * symRefField = comp->getSymRefTab()->findOrCreateJavaLangReferenceReferentShadowSymbol(callerSymRef->getOwningMethodSymbol(comp),
                                                                                                              true, TR_Address, offset, false);

            // This pointer
            TR_Node * thisNode = callNode->getFirstChild();

            // Generate indirect load of referent into the callNode
            callNode->setOpCodeValue(comp->fe()->opCodeForIndirectLoad(TR_Address));
            callNode->setSymbolReference(symRefField);
            callNode->removeAllChildren();
            callNode->setNumChildren(1);
            callNode->setAndIncChild(0, thisNode);
            }
            return callNode;
      case java_lang_J9VMInternals_rawNewArrayInstance:
      	{
      	callNode->setOpCodeValue(TR_variableNewArray);
      	callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateANewArraySymbolRef(callNode->getSymbol()->castToResolvedMethodSymbol()));
      	TR_Node *newNode = TR_Node::create(comp, callNode, TR_iaload, 1, comp->getSymRefTab()->findOrCreateArrayComponentTypeSymbolRef());
      	TR_Node *newNodeChild = TR_Node::create(comp, callNode, TR_iaload, 1, comp->getSymRefTab()->findOrCreateClassFromJavaLangClassAsPrimitiveSymbolRef());
      	newNode->setAndIncChild(0, newNodeChild);
      	newNode->setNumChildren(1);
      	newNodeChild->setChild(0, callNode->getChild(1));
      	newNodeChild->setNumChildren(1);
      	callNode->setAndIncChild(1,newNode);
      	return callNode;
      	}
      case java_lang_J9VMInternals_getArrayLengthAsObject:
      	{
      	callNode->setOpCodeValue(TR_iiload);
      	callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateContiguousArraySizeSymbolRef());
      	return callNode;
      	}
      case java_lang_J9VMInternals_rawNewInstance:
      	{
      	 callNode->setOpCodeValue(TR_variableNew);
      	 callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateNewObjectSymbolRef(callNode->getSymbol()->castToResolvedMethodSymbol()));
         TR_Node *newNode = TR_Node::create(comp, callNode, TR_iaload, 1, comp->getSymRefTab()->findOrCreateClassFromJavaLangClassAsPrimitiveSymbolRef());
      	 newNode->setChild(0, callNode->getFirstChild());
      	 newNode->setNumChildren(1);
      	 callNode->setAndIncChild(0,newNode);
      	 return callNode;
      	}
      case java_lang_J9VMInternals_defaultClone:
      	{
        TR_OpaqueClassBlock *bdClass = getClassFromSignature("java/lang/Object", 16, comp->getCurrentMethod());
        TR_ScratchList<TR_ResolvedMethod> bdMethods(comp->trMemory());
        getResolvedMethods(comp->trMemory(), bdClass, &bdMethods);
        ListIterator<TR_ResolvedMethod> bdit(&bdMethods);

        TR_ResolvedMethod *method = null;
        for (method = bdit.getCurrent(); method; method = bdit.getNext())
        {
         const char *sig = method->signature(comp->trMemory());
         int32_t len = strlen(sig);
         if(!strncmp(sig, "java/lang/Object.clone()Ljava/lang/Object;", len))
            {
            callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateMethodSymbol(callNode->getSymbolReference()->getOwningMethodIndex(), -1, method, TR_MethodSymbol::Special));
            break;
            }
          }
        return callNode;
      	}
      case java_lang_J9VMInternals_isClassModifierPublic:
      	{
        callNode->setOpCodeValue(TR_iand);
      	callNode->setAndIncChild(1, TR_Node::create(comp, TR_iconst, 0, J9AccPublic));
        callNode->setNumChildren(2);
      	return callNode;
      	}
      case com_ibm_jit_JITHelpers_is32Bit:
         {
         // instance method so receiver object is first child, and other nodes may hold onto it so need to decrement its ref count
         // following code hammers this call node into an iconst which disconnects the child
    	 callNode->getFirstChild()->decReferenceCount();
    	 // intentionally fall through to next case
         }
      case java_lang_J9VMInternals_is32Bit:
      case com_ibm_oti_vm_ORBVMHelpers_is32Bit:
         {
         // these methods are static so there are no child to worry about
         int32_t intValue = 0;
         if (!comp->cg()->is64BitTarget())
            intValue = 1;
         callNode->setOpCodeValue(TR_iconst);
         callNode->setNumChildren(0);
         callNode->setInt(intValue);
         if (callNodeTreeTop && 
               callNodeTreeTop->getNode()->getOpCode().isResolveOrNullCheck())
            callNodeTreeTop->getNode()->setOpCodeValue(TR_treetop);
         return callNode;
         }
      case java_lang_J9VMInternals_getNumBitsInReferenceField:
      case com_ibm_oti_vm_ORBVMHelpers_getNumBitsInReferenceField:
      case com_ibm_jit_JITHelpers_getNumBitsInReferenceField:
         {
         int32_t intValue = 8*getAddressFieldSize();
         callNode->setOpCodeValue(TR_iconst);
         callNode->setNumChildren(0);
         callNode->setInt(intValue);
         return callNode;
         }
      case java_lang_J9VMInternals_getNumBytesInReferenceField:
      case com_ibm_oti_vm_ORBVMHelpers_getNumBytesInReferenceField:
      case com_ibm_jit_JITHelpers_getNumBytesInReferenceField:
         {
         int32_t intValue = getAddressFieldSize();
         callNode->setOpCodeValue(TR_iconst);
         callNode->setNumChildren(0);
         callNode->setInt(intValue);
         return callNode;
         }
      case java_lang_J9VMInternals_getNumBitsInDescriptionWord:
      case com_ibm_oti_vm_ORBVMHelpers_getNumBitsInDescriptionWord:
      case com_ibm_jit_JITHelpers_getNumBitsInDescriptionWord:
         {
         int32_t intValue = 8*sizeof(UDATA);
         callNode->setOpCodeValue(TR_iconst);
         callNode->setNumChildren(0);
         callNode->setInt(intValue);
         return callNode;
         }
      case java_lang_J9VMInternals_getNumBytesInDescriptionWord:
      case com_ibm_oti_vm_ORBVMHelpers_getNumBytesInDescriptionWord:
      case com_ibm_jit_JITHelpers_getNumBytesInDescriptionWord:
         {
         int32_t intValue = sizeof(UDATA);
         callNode->setOpCodeValue(TR_iconst);
         callNode->setNumChildren(0);
         callNode->setInt(intValue);
         return callNode;
         }
      case java_lang_J9VMInternals_getNumBytesInJ9ObjectHeader:
      case com_ibm_oti_vm_ORBVMHelpers_getNumBytesInJ9ObjectHeader:
      case com_ibm_jit_JITHelpers_getNumBytesInJ9ObjectHeader:
         {
         int32_t intValue = sizeof(J9Object);
         callNode->setOpCodeValue(TR_iconst);
         callNode->setNumChildren(0);
         callNode->setInt(intValue);
         return callNode;
         }
      case java_lang_J9VMInternals_getJ9ClassFromClass32:
      case com_ibm_oti_vm_ORBVMHelpers_getJ9ClassFromClass32:
         {
         callNode->setOpCodeValue(TR_iiload);
         callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateClassFromJavaLangClassAsPrimitiveSymbolRef());

         return callNode;
         }
      case java_lang_J9VMInternals_getInstanceShapeFromJ9Class32:
      case com_ibm_oti_vm_ORBVMHelpers_getInstanceShapeFromJ9Class32:
         {
         callNode->setOpCodeValue(TR_iiload);
         callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateInstanceShapeSymbolRef());
         return callNode;
         }
      case java_lang_J9VMInternals_getInstanceDescriptionFromJ9Class32:
      case com_ibm_oti_vm_ORBVMHelpers_getInstanceDescriptionFromJ9Class32:
      case com_ibm_jit_JITHelpers_getInstanceDescriptionFromJ9Class32:
         {
         callNode->setOpCodeValue(TR_iiload);
         callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateInstanceDescriptionSymbolRef());
         return callNode;
         }
      case java_lang_J9VMInternals_getDescriptionWordFromPtr32:
      case com_ibm_oti_vm_ORBVMHelpers_getDescriptionWordFromPtr32:
      case com_ibm_jit_JITHelpers_getDescriptionWordFromPtr32:
         {
         callNode->setOpCodeValue(TR_iiload);
         callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateDescriptionWordFromPtrSymbolRef());
         return callNode;
         }
      case java_lang_J9VMInternals_getJ9ClassFromClass64:
      case com_ibm_oti_vm_ORBVMHelpers_getJ9ClassFromClass64:
         {
         callNode->setOpCodeValue(TR_ilload);
         callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateClassFromJavaLangClassAsPrimitiveSymbolRef());
         return callNode;
         }
      case java_lang_J9VMInternals_getInstanceShapeFromJ9Class64:
      case com_ibm_oti_vm_ORBVMHelpers_getInstanceShapeFromJ9Class64:
         {
         callNode->setOpCodeValue(TR_ilload);
         callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateInstanceShapeSymbolRef());
         return callNode;
         }
      case java_lang_J9VMInternals_getInstanceDescriptionFromJ9Class64:
      case com_ibm_oti_vm_ORBVMHelpers_getInstanceDescriptionFromJ9Class64:
      case com_ibm_jit_JITHelpers_getInstanceDescriptionFromJ9Class64:
         {
         callNode->setOpCodeValue(TR_ilload);
         callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateInstanceDescriptionSymbolRef());
         return callNode;
         }
      case java_lang_J9VMInternals_getDescriptionWordFromPtr64:
      case com_ibm_oti_vm_ORBVMHelpers_getDescriptionWordFromPtr64:
      case com_ibm_jit_JITHelpers_getDescriptionWordFromPtr64:
         {
         callNode->setOpCodeValue(TR_ilload);
         callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateDescriptionWordFromPtrSymbolRef());
         return callNode;
         }
      case java_lang_J9VMInternals_getSuperclass:
         {
         TR_OpaqueClassBlock *jitHelpersClass = comp->getJITHelpersClassPointer();
         ///trace1c(comp, "jithelpersclass = %p\n", jitHelpersClass);
         if (jitHelpersClass && isClassInitialized(jitHelpersClass))
            {
            // fish for the getSuperclass method in JITHelpers
            //
            const char *callerSig = comp->signature();
            const char *getHelpersSig = "jitHelpers";
            int32_t getHelpersSigLength = 10;
            TR_SymbolReference *getSuperclassSymRef = NULL;
            TR_SymbolReference *getHelpersSymRef = NULL;
            TR_ScratchList<TR_ResolvedMethod> helperMethods(comp->trMemory());
            getResolvedMethods(comp->trMemory(), jitHelpersClass, &helperMethods);
            ListIterator<TR_ResolvedMethod> it(&helperMethods);
            for (TR_ResolvedMethod *m = it.getCurrent(); m; m = it.getNext())
               {
               char *sig = m->nameChars();
               if (!strncmp(sig, "getSuperclass", 13))
                  {
                  ///newMethodSymRef = comp->getSymRefTab()->findOrCreateMethodSymbol(JITTED_METHOD_INDEX, -1, m, TR_MethodSymbol::Static);
                  getSuperclassSymRef = comp->getSymRefTab()->findOrCreateMethodSymbol(JITTED_METHOD_INDEX, -1, m, TR_MethodSymbol::Virtual);
                  getSuperclassSymRef->setOffset(getVTableSlot(m->getPersistentIdentifier(), jitHelpersClass));
                  ///break;
                  }
               else if (!strncmp(sig, getHelpersSig, getHelpersSigLength))
                  {
                  getHelpersSymRef = comp->getSymRefTab()->findOrCreateMethodSymbol(JITTED_METHOD_INDEX, -1, m, TR_MethodSymbol::Static);
                  }
               }

            // redirect the call to the new Java implementation 
            // instead of calling the native
            //
            if (getSuperclassSymRef && getHelpersSymRef)
               {
               // first get the helpers object 
               // acall getHelpers
               //
               TR_Method *method = getHelpersSymRef->getSymbol()->castToMethodSymbol()->getMethod();
               TR_Node *helpersCallNode = TR_Node::create(comp, callNode, method->directCallOpCode(), 0, getHelpersSymRef);
               TR_TreeTop *helpersCallTT = TR_TreeTop::create(comp, TR_Node::create(comp, TR_treetop, 1, helpersCallNode));
               callNodeTreeTop->insertBefore(helpersCallTT);
              
               //TR_Node *vftLoad = TR_Node::create(comp, TR_iaload, 1, helpersCallNode, comp->getSymRefTab()->findOrCreateVftSymbolRef());
               method = getSuperclassSymRef->getSymbol()->castToMethodSymbol()->getMethod();
               callNode->setOpCodeValue(method->directCallOpCode());
               TR_Node *firstChild = callNode->getFirstChild();
               firstChild->decReferenceCount();
               callNode->setNumChildren(2);
               //callNode->setAndIncChild(0, vftLoad);
               callNode->setAndIncChild(0, helpersCallNode);
               callNode->setAndIncChild(1, firstChild);
               ///trace1c(comp, "replaced call node for getSuperclass at node = %p\n", callNode);
               callNode->setSymbolReference(getSuperclassSymRef);
               }
            }
         return callNode;
         }
      case com_ibm_jit_JITHelpers_getBackfillOffsetFromJ9Class32:
      case com_ibm_jit_JITHelpers_getBackfillOffsetFromJ9Class64:
      case com_ibm_jit_JITHelpers_getRomClassFromJ9Class32:
      case com_ibm_jit_JITHelpers_getRomClassFromJ9Class64:
      case com_ibm_jit_JITHelpers_getArrayShapeFromRomClass32:
      case com_ibm_jit_JITHelpers_getArrayShapeFromRomClass64:
      case com_ibm_jit_JITHelpers_getModifiersFromRomClass32:
      case com_ibm_jit_JITHelpers_getModifiersFromRomClass64:
      case com_ibm_jit_JITHelpers_getSuperClassesFromJ9Class32:
      case com_ibm_jit_JITHelpers_getSuperClassesFromJ9Class64:
         {
         // TODO: These methods should really have proper symrefs
         TR_Node *firstChild = callNode->getFirstChild();
         TR_Node *secondChild = callNode->getSecondChild();
         firstChild->decReferenceCount();
         secondChild->decReferenceCount();
         TR_Node *konstNode = NULL;
         TR_ILOpCodes addOp;
         TR_DataTypes type;
         switch (methodID)
            {
            case com_ibm_jit_JITHelpers_getBackfillOffsetFromJ9Class32:
               {
               int32_t konst = (int32_t)getOffsetOfBackfillOffsetField();
               konstNode = TR_Node::iconst(comp, callNode, konst);
               addOp = TR_iadd;
               type = TR_Int32;
               break;
               }
            case com_ibm_jit_JITHelpers_getBackfillOffsetFromJ9Class64:
               {
               int64_t konst = getOffsetOfBackfillOffsetField();
               konstNode = TR_Node::lconst(comp, callNode, konst);
               addOp = TR_ladd;
               type = TR_Int64;
               break;
               }
            case com_ibm_jit_JITHelpers_getRomClassFromJ9Class32:
               {
               int32_t konst = (int32_t)getOffsetOfClassRomPtrField();
               konstNode = TR_Node::iconst(comp, callNode, konst);
               addOp = TR_iadd;
               type = TR_Int32;
               break;
               }
            case com_ibm_jit_JITHelpers_getRomClassFromJ9Class64:
               {
               int64_t konst = getOffsetOfClassRomPtrField();
               konstNode = TR_Node::lconst(comp, callNode, konst);
               addOp = TR_ladd;
               type = TR_Int64;
               break;
               }
            case com_ibm_jit_JITHelpers_getArrayShapeFromRomClass32:
            case com_ibm_jit_JITHelpers_getArrayShapeFromRomClass64:
               {
               int32_t konst = (int32_t)getOffsetOfIndexableSizeField();
               konstNode = TR_Node::iconst(comp, callNode, konst);
               addOp = TR_iadd;
               type = TR_Int32;
               break;
               }
            case com_ibm_jit_JITHelpers_getModifiersFromRomClass32:
            case com_ibm_jit_JITHelpers_getModifiersFromRomClass64:
               {
               int32_t konst = (int32_t)getOffsetOfIsArrayFieldFromRomClass();
               konstNode = TR_Node::iconst(comp, callNode, konst);
               addOp = TR_iadd;
               type = TR_Int32;
               break;
               }
            case com_ibm_jit_JITHelpers_getSuperClassesFromJ9Class32:
               {
               int32_t konst = (int32_t)getOffsetOfSuperclassesInClassObject();
               konstNode = TR_Node::iconst(comp, callNode, konst);
               addOp = TR_iadd;
               type = TR_Int32;
               break;
               }
            case com_ibm_jit_JITHelpers_getSuperClassesFromJ9Class64:
               {
               int64_t konst = getOffsetOfSuperclassesInClassObject();
               konstNode = TR_Node::lconst(comp, callNode, konst);
               addOp = TR_ladd;
               type = TR_Int64;
               break;
               }
            }
         secondChild = TR_Node::create(comp, addOp, 2, secondChild, konstNode);
         TR_SymbolReference *newSymRef = comp->getSymRefTab()->findOrCreateUnsafeSymbolRef(type);
         callNode->setOpCodeValue(opCodeForIndirectArrayLoad(type));
         // now remove the receiver (firstChild)
         //
         callNode->setSymbolReference(newSymRef);
         callNode->setAndIncChild(0, secondChild);
         callNode->setNumChildren(1);
         if (callNodeTreeTop && callNodeTreeTop->getNode()->getOpCode().isCheck())
            callNodeTreeTop->getNode()->setOpCodeValue(TR_treetop);

         return callNode;
         }
      case com_ibm_jit_JITHelpers_getJ9ClassFromClass32:
      case com_ibm_jit_JITHelpers_getJ9ClassFromClass64:
      case com_ibm_jit_JITHelpers_getClassFromJ9Class32:
      case com_ibm_jit_JITHelpers_getClassFromJ9Class64:
      case com_ibm_jit_JITHelpers_getClassFlagsFromJ9Class32:
      case com_ibm_jit_JITHelpers_getClassFlagsFromJ9Class64:
      case com_ibm_jit_JITHelpers_getClassDepthAndFlagsFromJ9Class32:
      case com_ibm_jit_JITHelpers_getClassDepthAndFlagsFromJ9Class64:
      case com_ibm_jit_JITHelpers_getPackedDataSizeFromJ9Class32:
      case com_ibm_jit_JITHelpers_getPackedDataSizeFromJ9Class64:
      case com_ibm_jit_JITHelpers_getComponentTypeFromJ9Class32:
      case com_ibm_jit_JITHelpers_getComponentTypeFromJ9Class64:
      case com_ibm_jvm_packed_PackedObject_getPackedOffset32Impl:
      case com_ibm_jvm_packed_PackedObject_getPackedOffset64Impl:
      case com_ibm_jvm_packed_PackedObject_getPackedTargetImpl:
      case com_ibm_jvm_packed_PackedObject_getPackedIndexableContiguousOffset32Impl:
      case com_ibm_jvm_packed_PackedObject_getPackedIndexableContiguousOffset64Impl:
      case com_ibm_jvm_packed_PackedObject_getPackedIndexableContiguousTargetImpl:
         {
         TR_Node *firstChild = callNode->getFirstChild();
         TR_Node *secondChild = callNode->getSecondChild();
         firstChild->decReferenceCount();
         secondChild->decReferenceCount();
         TR_ILOpCodes loadOp;
         TR_SymbolReference *newSymRef;

         switch (methodID)
            {
         case com_ibm_jit_JITHelpers_getJ9ClassFromClass32:
            {
            newSymRef = comp->getSymRefTab()->findOrCreateClassFromJavaLangClassAsPrimitiveSymbolRef();
            loadOp = TR_iiload;
            break;
            }
         case com_ibm_jit_JITHelpers_getJ9ClassFromClass64:
            {
            newSymRef = comp->getSymRefTab()->findOrCreateClassFromJavaLangClassAsPrimitiveSymbolRef();
            loadOp = TR_ilload;
            break;
            }
         case com_ibm_jit_JITHelpers_getClassFromJ9Class32:
         case com_ibm_jit_JITHelpers_getClassFromJ9Class64:
            {
            newSymRef = comp->getSymRefTab()->findOrCreateJavaLangClassFromClassSymbolRef();
            loadOp = TR_iaload;
            break;
            }
         case com_ibm_jit_JITHelpers_getClassFlagsFromJ9Class32:
         case com_ibm_jit_JITHelpers_getClassFlagsFromJ9Class64:
            {
            loadOp = TR_iiload;
            newSymRef = comp->getSymRefTab()->findOrCreateClassFlagsSymbolRef();
            break;
            }
         case com_ibm_jit_JITHelpers_getClassDepthAndFlagsFromJ9Class32:
            {
            loadOp = TR_iiload;
            newSymRef = comp->getSymRefTab()->findOrCreateClassAndDepthFlagsSymbolRef();
            break;
            }
         case com_ibm_jit_JITHelpers_getClassDepthAndFlagsFromJ9Class64:
            {
            loadOp = TR_ilload;
            newSymRef = comp->getSymRefTab()->findOrCreateClassAndDepthFlagsSymbolRef();
            break;
            }
         case com_ibm_jit_JITHelpers_getPackedDataSizeFromJ9Class32:
            {
            loadOp = TR_ilload;
            newSymRef = comp->getSymRefTab()->findOrCreatePackedDataOffsetSymbolRef();
            break;
            }
         case com_ibm_jit_JITHelpers_getPackedDataSizeFromJ9Class64:
            {
            loadOp = TR_ilload;
            newSymRef = comp->getSymRefTab()->findOrCreatePackedDataOffsetSymbolRef();
            break;
            }
         case com_ibm_jit_JITHelpers_getComponentTypeFromJ9Class32:
            {
            loadOp = TR_ilload;
            newSymRef = comp->getSymRefTab()->findOrCreateArrayComponentTypeAsPrimitiveSymbolRef();
            break;
            }
         case com_ibm_jit_JITHelpers_getComponentTypeFromJ9Class64:
            {
            loadOp = TR_ilload;
            newSymRef = comp->getSymRefTab()->findOrCreateArrayComponentTypeAsPrimitiveSymbolRef();
            break;
            }
         case com_ibm_jvm_packed_PackedObject_getPackedOffset32Impl:
            {
            loadOp = TR_iiload;
            newSymRef = comp->getSymRefTab()->findPackedDataOffsetSymbolRef();
            break;
            }
         case com_ibm_jvm_packed_PackedObject_getPackedOffset64Impl:
            {
            loadOp = TR_ilload;
            newSymRef = comp->getSymRefTab()->findPackedDataOffsetSymbolRef();
            break;
            }
         case com_ibm_jvm_packed_PackedObject_getPackedTargetImpl:
            {
            loadOp = TR_iaload;
            newSymRef = comp->getSymRefTab()->findPackedDataPointerSymbolRef();
            break;
            }
         case com_ibm_jvm_packed_PackedObject_getPackedIndexableContiguousOffset32Impl:
            {
            loadOp = TR_iiload;
            newSymRef = comp->getSymRefTab()->findContiguousPackedArrayOffsetSymbolRef();
            break;
            }
         case com_ibm_jvm_packed_PackedObject_getPackedIndexableContiguousOffset64Impl:
            {
            loadOp = TR_ilload;
            newSymRef = comp->getSymRefTab()->findContiguousPackedArrayOffsetSymbolRef();
            break;
            }
         case com_ibm_jvm_packed_PackedObject_getPackedIndexableContiguousTargetImpl:
            {
            loadOp = TR_iaload;
            newSymRef = comp->getSymRefTab()->findContiguousPackedArrayObjectSymbolRef();
            break;
            }
            }
         // now remove the receiver (firstChild)
         //
         callNode->setOpCodeValue(loadOp);
         callNode->setSymbolReference(newSymRef);
         callNode->setAndIncChild(0, secondChild);
         callNode->setNumChildren(1);
         return callNode;
         }
       case com_ibm_jit_Crypto_expandAESKeyInHardware:
         {
         if (comp->cg()->enableAESInHardwareTransformations())
            {
            // Virtual final method.  Remove the receiver child and shift the position of each of
            // the remaining children.
            //
            TR_Node *firstChild = callNode->getFirstChild();
            firstChild->decReferenceCount();

            callNode->setChild(0, callNode->getChild(1));
            callNode->setChild(1, callNode->getChild(2));
            callNode->setChild(2, callNode->getChild(3));
            callNode->setNumChildren(3);
            return callNode;
            }
         else
            {
            return 0;
            }
         }
      case com_ibm_jit_Crypto_doAESInHardware:
         {
         if (comp->cg()->enableAESInHardwareTransformations())
            {
            // Virtual final method.  Remove the receiver child and shift the position of each of
            // the remaining children.
            //
            TR_Node *firstChild = callNode->getFirstChild();
            firstChild->decReferenceCount();

            callNode->setChild(0, callNode->getChild(1));
            callNode->setChild(1, callNode->getChild(2));
            callNode->setChild(2, callNode->getChild(3));
            callNode->setChild(3, callNode->getChild(4));
            callNode->setChild(4, callNode->getChild(5));
            callNode->setChild(5, callNode->getChild(6));
            callNode->setChild(6, callNode->getChild(7));
            callNode->setChild(7, callNode->getChild(8));
            callNode->setNumChildren(8);
            return callNode;
            }
         else
            {
            return 0;
            }
         }
	  case com_ibm_tenant_InternalTenantContext_getCurrentImpl:
	     {
         callNode->setOpCodeValue(TR_aload);
         callNode->setSymbolReference(comp->getSymRefTab()->findOrCreateTenantContextSymbolRef());
         return callNode;
         }
      }

   return 0;
   }


TR_OpaqueClassBlock *
TR_J9VM::getClassClassPointer(TR_OpaqueClassBlock *objectClassPointer)
   {
   J9Class *j9class;
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   j9class = convertClassOffsetToClassPtr(objectClassPointer);

   if (generateClassesOnHeap())
      {
      void *javaLangClass = J9VM_J9CLASS_TO_HEAPCLASS(convertClassOffsetToClassPtr(objectClassPointer));

      // j9class points to the J9Class corresponding to java/lang/Object
      if (generateCompressedObjectHeaders())
         j9class = (J9Class *) *((uint32_t *) ((uintptrj_t) javaLangClass + (uintptrj_t) getOffsetOfObjectVftField()));
      else
         j9class = (J9Class *)(*((J9Class **) ((uintptrj_t) javaLangClass + (uintptrj_t) getOffsetOfObjectVftField())));
      }
   else
      j9class = (J9Class *)(*((J9Class **) ((uintptrj_t)j9class + (uintptrj_t) getOffsetOfObjectVftField())));

   j9class = (J9Class *)((uintptrj_t)j9class & getMaskOfObjectVftField());

   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return convertClassPtrToClassOffset(j9class);
   }



TR_OpaqueClassBlock *
TR_J9VM::getClassFromStatic(void *p)
   {
   J9Class *j9class;

   if (generateCompressedObjectHeaders())
      j9class = (J9Class *) *((uint32_t *) ((uintptrj_t) *((void ***)p) + (uintptrj_t) getOffsetOfObjectVftField()));
   else
      j9class = (J9Class *)(*((J9Class **) ((uintptrj_t) *((void ***)p) + (uintptrj_t) getOffsetOfObjectVftField())));

   j9class = (J9Class *)((uintptrj_t)j9class & getMaskOfObjectVftField());
   return convertClassPtrToClassOffset(j9class);
   }

bool
TR_J9VM::canInlineAllocateClass(TR_OpaqueClassBlock *block, bool avoidSpecialObjects)
   {
   if (block == NULL)
      return false;

   J9Class *clazz = (J9Class *) block;

   // Can't inline the allocation if the class is special (finalizable,
   // reference object, etc.)
   //
   if (clazz->classDepthAndFlags & (
		     J9_JAVA_CLASS_REFERENCE_WEAK
		   | J9_JAVA_CLASS_REFERENCE_SOFT
		   | J9_JAVA_CLASS_FINALIZE
		   | J9_JAVA_CLASS_OWNABLE_SYNCHRONIZER
		   ))
       {
       if (avoidSpecialObjects)
          {
          return false;
          }
       }

   // Can't inline the allocation if the class is not fully initialized
   //
   if (clazz->initializeStatus != 1)
      return false;

   // Can't inline the allocation if the class is an interface or abstract
   //
   J9ROMClass * romClass = clazz->romClass;
   if (romClass->modifiers & (J9_JAVA_ABSTRACT | J9_JAVA_INTERFACE))
      return false;

   return true;
   }

// See if it is OK to remove this allocation node to e.g. merge it with others
// or allocate it locally on a stack frame.
// If so, return the allocation size if the size is constant, or zero if the
// size is variable.
// If not, return -1.
//
int32_t
TR_J9VM::canSkipAllocationNode(TR_Compilation * comp, TR_Node * node, TR_OpaqueClassBlock *&classInfo, bool avoidSpecialObjects)
   {
   if (isAOT())
      return -1;
   return canInlineAllocation(comp, node, classInfo, avoidSpecialObjects);
   }

// This code was previously in canSkipAllocationNode. However, it is required by code gen to
// inline heap allocations. The only difference, for now, is that inlined heap allocations
// are being enabled for AOT, but stack allocations are not (yet).
//
int32_t
TR_J9VM::canInlineAllocation(TR_Compilation * comp, TR_Node * node, TR_OpaqueClassBlock *&classInfo, bool avoidSpecialObjects)
   {

#ifdef TR_RESOLVED_CLASSES_INSTRUMENTATION_FOR_AOT
   if (node->getOpCodeValue() == TR_new && isAOT())
      {
      TR_SymbolReference *classSymRef = node->getFirstChild()->getSymbolReference();
      TR_StaticSymbol *classSym = classSymRef->getSymbol()->getStaticSymbol();

      // The point of the relocation record is to count how many classes are resolved
      // and can be inlined allocated after compile time, that are also resolved at
      // relocation after load time.
      //
      J9ConstantPool *cp = (J9ConstantPool *) classSymRef->getOwningMethod(comp)->constantPool();
      int32_t cpIndex = classSymRef->getCPIndex();
      comp->cg()->addAOTRelocation
         (new (comp->cg()->trHeapMemory()) TR_32BitExternalRelocation(0,
                                                                      (uint8_t *) cp,
                                                                      (uint8_t *) cpIndex,
                                                                      TR_VerifyClassObjectForAlloc,
                                                                      comp->cg()),
         __FILE__, __LINE__, node->getFirstChild());
      }
#endif

   // Can't skip the allocation if we are generating JVMPI hooks, since
   // JVMPI needs to know about the allocation.
   //
   if (comp->suppressAllocationInlining() || !supportAllocationInlining(comp, node))
      return -1;

   // Pending inline allocation support on platforms for variable new
   //
   if (node->getOpCodeValue() == TR_variableNew || node->getOpCodeValue() == TR_variableNewArray)
   	   return -1;

   int32_t              size;
   TR_Node            * classRef;
   TR_SymbolReference * classSymRef;
   TR_StaticSymbol    * classSym;
   J9Class            * clazz;

   bool isRealTimeGC = comp->getOptions()->realTimeGC();

   bool generateArraylets = comp->generateArraylets();

   #if !defined (PUBLIC_BUILD)
   if (node->getOpCodeValue() == TR_newStructRef)
      {
      classRef    = node->getFirstChild();
      if (classRef->getOpCodeValue() == TR_loadaddr)
         {
         classSymRef = classRef->getSymbolReference();
         clazz       = getClassForAllocationInlining(comp, classSymRef);
         classInfo   = getClassOffsetForAllocationInlining(clazz);
         }
      //else classRef->getOpCodeValue() == call
      return comp->fe()->packedObjectSize();
      }

   if (node->getOpCodeValue() == TR_anewarrayStructRef)
      {
      classRef    = node->getFirstChild();
      TR_Node* length = node->getChild(3);
      if (length->getOpCode().isIntegralConst() && 0 != length->get64bitIntegralValue(comp))
         // Only inline allocate packed derived arrays with known size
         // Cannot inline allocate a 'discontiguous' derived packed array
         // Because anewarraystructref is used for nested array fields, this is the
         // only expected shape, except for icall helper call for unresolved class
         return -1;
      if (classRef->getOpCodeValue() == TR_loadaddr)
         {
         classSymRef = classRef->getSymbolReference();
         clazz       = getClassForAllocationInlining(comp, classSymRef);
         classInfo   = getClassOffsetForAllocationInlining(clazz);
         }
      //else classRef->getOpCodeValue() == call
      return comp->fe()->getContiguousPackedArrayHeaderSizeInBytes();
      }
#endif //PUBLIC_BUILD

   if (node->getOpCodeValue() == TR_new)
      {

      classRef    = node->getFirstChild();
      classSymRef = classRef->getSymbolReference();

      classSym    = classSymRef->getSymbol()->getStaticSymbol();

      // Check if the class can be inlined allocation.
      // The class has to be resolved, initialized, concrete, etc.
      clazz = getClassForAllocationInlining(comp, classSymRef);
      if (!canInlineAllocateClass((TR_OpaqueClassBlock *)clazz, avoidSpecialObjects))
         return -1;

      classInfo = getClassOffsetForAllocationInlining(clazz);

      int32_t objectSize = getAllocationSize(classSym, (TR_OpaqueClassBlock *)clazz);
      return objectSize;
      }

   int32_t elementSize;
   bool isPackedArray = false;

   if (node->getOpCodeValue() == TR_newarray)
      {
      assume(node->getSecondChild()->getOpCode().isLoadConst(), "Expecting const child \n");

      int32_t arrayClassIndex = node->getSecondChild()->getInt();
      struct J9Class ** arrayClasses = &_jitConfig->javaVM->booleanArrayClass;
      clazz = arrayClasses[arrayClassIndex - 4];

      if (node->getFirstChild()->getOpCodeValue() != TR_iconst)
         {
         classInfo = getPrimitiveArrayAllocationClass(clazz);
         return 0;
         }

      // Make sure the number constant of elements requested is within reasonable bounds
      //
      assume(node->getFirstChild()->getOpCode().isLoadConst(), "Expecting const child \n");
      size = node->getFirstChild()->getInt();
      if (size < 0 || size > 0x000FFFFF)
         return -1;

      classInfo = getPrimitiveArrayAllocationClass(clazz);

      elementSize = getSizeOfArrayElement(node);
      }
   else if (node->getOpCodeValue() == TR_anewarray || node->getOpCodeValue() == TR_panewarray)
      {
      isPackedArray = node->getOpCodeValue() == TR_panewarray;
      classRef      = node->getSecondChild();
      classSymRef   = classRef->getSymbolReference();

      // Need to have a concrete packed class for packed array allocation
      if (isPackedArray && classRef->getOpCodeValue() != TR_loadaddr)
         return -1;

      // Can't skip the allocation if the class is unresolved
      //
      clazz = getClassForAllocationInlining(comp, classSymRef);
      if (clazz == NULL)
         return -1;

      // Can't skip the allocation if the array class is not yet created
      //
      if (!isPackedArray)
         clazz = clazz->arrayClass;
      if (!clazz)
         return -1;

      if (node->getFirstChild()->getOpCodeValue() != TR_iconst)
         {
         classInfo = getClassOffsetForAllocationInlining(clazz);
         return 0;
         }

      // Make sure the number of elements requested is in reasonable bounds
      //
      assume(node->getFirstChild()->getOpCode().isLoadConst(), "Expecting const child \n");
      size = node->getFirstChild()->getInt();
      if (size < 0 || size > 0x000FFFFF)
         return -1;

      classInfo = getClassOffsetForAllocationInlining(clazz);

      if (isPackedArray && size == 0)
         return -1;

      if (isPackedArray)
         elementSize = getArrayPackedDataSize(convertClassPtrToClassOffset(clazz));
      else if (comp->useCompressedPointers())
         elementSize = comp->fe()->getAddressFieldSize();
      else
         elementSize = (int32_t)comp->cg()->sizeOfJavaPointer();
      }

   assume(node->getOpCodeValue() == TR_newarray ||
          node->getOpCodeValue() == TR_anewarray ||
          node->getOpCodeValue() == TR_panewarray, "unexpected allocation node");

   size *= elementSize;

   if (useHybridArraylets() && isDiscontiguousArray(size))
      {
      if (comp->getOption(TR_TraceCG))
         trace2c(comp, "cannot inline array allocation @ node %p because size %d is discontiguous\n", node, size);
      return -1;
      }
#ifdef J9VM_GC_COMBINATION_SPEC
   else if (!isRealTimeGC && size == 0)
      {
#if (defined(TR_HOST_S390) && defined(TR_TARGET_S390)) || (defined(TR_TARGET_POWER) && defined(TR_HOST_POWER))
      if (isPackedArray)
         {
         if (comp->getOption(TR_TraceCG))
            trace1c(comp, "cannot inline array allocation @ node %p because size 0 is discontiguous\n", node);
         return -1;

         }
      size = getDiscontiguousArrayHeaderSizeInBytes();
      if (comp->getOption(TR_TraceCG))
         trace1c(comp, "inline array allocation @ node %p for size 0\n", node);
#else
      if (comp->getOption(TR_TraceCG))
         trace1c(comp, "cannot inline array allocation @ node %p because size 0 is discontiguous\n", node);
      return -1;
#endif
      }
#endif

   else if (isPackedArray)
      {
      size += getContiguousPackedArrayHeaderSizeInBytes();
      size = (size + DEFAULT_OBJECT_ALIGNMENT -1) & (-DEFAULT_OBJECT_ALIGNMENT);
      }
   else if (generateArraylets)
      {
      size += getArrayletFirstElementOffset(elementSize, comp);
      }
   else
      {
      size += getContiguousArrayHeaderSizeInBytes();
      }

   if (node->getOpCodeValue() == TR_newarray || comp->useCompressedPointers())
      {
      size = (int32_t)((size+(comp->cg()->sizeOfJavaPointer()-1)) & ~(comp->cg()->sizeOfJavaPointer()-1));
      }

   if (isRealTimeGC &&
       ((size < 0) || (size > comp->fe()->getMaxObjectSizeForSizeClass())))
      return -1;

   assume (size != -1, "unexpected array size");

   return size >= J9_GC_MINIMUM_OBJECT_SIZE ? size : J9_GC_MINIMUM_OBJECT_SIZE;
   }

// for replay
//
static U_32 getNumInlinedCalls(J9JITExceptionTable * methodMetaData)
   {
   U_32 sizeOfInlinedCallSites, numInlinedCallSites = 0;
   U_32 numExceptionRanges = methodMetaData->numExcptionRanges & 0x3FFF;
   U_32 fourByteExceptionRanges = methodMetaData->numExcptionRanges & 0x8000;

   if (methodMetaData->inlinedCalls)
      {
      sizeOfInlinedCallSites = (U_32) ((UDATA) methodMetaData->gcStackAtlas - (UDATA) methodMetaData->inlinedCalls);

      U_32 sizeOfElement = sizeof(TR_InlinedCallSite) + ((J9JITStackAtlas *)methodMetaData->gcStackAtlas)->numberOfMapBytes;
      numInlinedCallSites = sizeOfInlinedCallSites / sizeOfElement;
      }

   return numInlinedCallSites;
   }

static void printInlinedCalls(J9JITExceptionTable * methodMetaData)
   {
   U_32 i = 0;
   U_32 numCalls = getNumInlinedCalls(methodMetaData);
   printf("Found inlinedMethods are in methodMetadata [%p] = %d \n", methodMetaData, numCalls);
   printf("callerIndex\t byteCodeIndex\t j9method\n");

   for (i=0; i<numCalls; i++)
      {
      U_32 sizeOfElement = sizeof(TR_InlinedCallSite) + ((J9JITStackAtlas *)methodMetaData->gcStackAtlas)->numberOfMapBytes;
      U_8 *inlinedCallSite = (U_8*)methodMetaData->inlinedCalls + (i * sizeOfElement);
      J9Method *inlinedMethod = (J9Method *)(((TR_InlinedCallSite*)inlinedCallSite)->_vmMethodInfo);
      printf("%d\t       %x\t            %p\n", ((TR_InlinedCallSite*)inlinedCallSite)->_byteCodeInfo.getCallerIndex(), ((TR_InlinedCallSite*)inlinedCallSite)->_byteCodeInfo.getByteCodeIndex(), inlinedMethod);
      if ((UDATA)inlinedMethod & 0x1)
         ;//printf("and it is patched");
      else
         {
         J9UTF8 * name;
         J9UTF8 * className;
         J9UTF8 * sig;
         getClassNameSignatureFromMethod(inlinedMethod, className, name, sig);
         ///printf("%.*s.%.*s.%.*s ", (U_32)J9UTF8_LENGTH(className), J9UTF8_DATA(className), (U_32)J9UTF8_LENGTH(name), J9UTF8_DATA(name), (U_32)J9UTF8_LENGTH(sig), J9UTF8_DATA(sig));
         }
      ///printf("\n");
      }
   }

static void setupCHTableForReplay(TR_J9VM *vm, TR_PersistentInfo *corePersistentInfo, J9JITExceptionTable * methodMetaData)
   {
   U_32 i = 0;
   if (methodMetaData)
      {
      printf("found methodMetadata at [%p]\n", methodMetaData);
      U_32 numCalls = getNumInlinedCalls(methodMetaData);
      for (i = 0; i < numCalls; i++)
         {
         U_32 sizeOfElement = sizeof(TR_InlinedCallSite) + ((J9JITStackAtlas *)methodMetaData->gcStackAtlas)->numberOfMapBytes;
         U_8 *inlinedCallSite = (U_8*)methodMetaData->inlinedCalls + (i * sizeOfElement);
         J9Method *inlinedMethod = (J9Method *)(((TR_InlinedCallSite*)inlinedCallSite)->_vmMethodInfo);
         TR_PersistentCHTable * table = corePersistentInfo->getPersistentCHTable();
         if (!((UDATA)inlinedMethod & 0x1))
            {
            J9Class *inlinedClass = J9_CLASS_FROM_METHOD(inlinedMethod);
            UDATA classDepth = (inlinedClass->classDepthAndFlags & J9_JAVA_CLASS_DEPTH_MASK) - 1;
            if (classDepth >= 0)
               {
               J9Class * superClass = inlinedClass->superclasses[classDepth];
               bool isAbstract = vm->isAbstractClass((TR_OpaqueClassBlock *)superClass);
               ///printf("got superClass %p for class %p isabstract %d\n", superClass, inlinedClass, isAbstract);
               bool result = table->classInAssumptionsList(corePersistentInfo->getRuntimeAssumptionTable(), (TR_OpaqueClassBlock *)superClass);
               ///printf("it is %d in assumptions\n", result);
               if (!result && isAbstract)
                  {
                  // either profiled or abstract guard
                  printf("Patching CHTable for class %p superClass %p\n", inlinedClass, superClass);
                  table->fixUpSubClasses((TR_OpaqueClassBlock*)superClass, (TR_OpaqueClassBlock*)inlinedClass);
                  }
               }
            }
         }
      }
   }

// setup the IProfiler hash table entries
//
static void setupIProfilerEntries(J9JITConfig *coreJitConfig, J9JITConfig *curJitConfig)
   {
   printf("going to access iprofiler from config %p\n", coreJitConfig);fflush(stdout);
   printf("coreJitConfig->privateConfig:\n");fflush(stdout);
   printf("coreJitConfig->privateConfig: %p\n", (coreJitConfig->privateConfig));fflush(stdout);
   TR_IProfiler *coreIProfiler = ((TR_JitPrivateConfig*)(coreJitConfig->privateConfig))->iProfiler;
   printf("found coreIProfiler: %p\n", coreIProfiler);fflush(stdout);
   if (!coreIProfiler)
      {
      printf("no iprofiler found in the coredump!\n");fflush(stdout);
      return;
      }
   TR_IProfiler *curIProfiler = ((TR_JitPrivateConfig*)(curJitConfig->privateConfig))->iProfiler;
   printf("going to setup entries in the iprofiler table\n");
   coreIProfiler->setupEntriesInHashTable(curIProfiler);
   }

// setup the JIT value profile info lists
//
static void setupJitValueProfileInfo(TR_PersistentProfileInfo *ppi, TR_PersistentCHTable * table)
   {
   printf("setting up jit value profile info in ppi [%p]\n", ppi);fflush(stdout);
   if (!ppi)
      {
      printf("no jit value profile info found\n");fflush(stdout);
      return;
      }

   TR_ValueProfileInfo *coreVPInfo = ppi->getValueProfileInfo();
   if (!coreVPInfo)
      {
      printf("no value profile info found\n");fflush(stdout);
      return;
      }
   int32_t valueInfoSize = sizeof(TR_ValueInfo);
   int32_t addressInfoSize = sizeof(TR_AddressInfo);
   int32_t warmCompilePICSize = sizeof(TR_WarmCompilePICAddressInfo);
   // initialize the various abstractInfo classes
   // so we can use the vfts of these for whacking
   //
   TR_ValueInfo *vInfo = new (PERSISTENT_NEW) TR_ValueInfo();
   TR_AddressInfo *aInfo = new (PERSISTENT_NEW) TR_AddressInfo();
   TR_WarmCompilePICAddressInfo *wInfo = new (PERSISTENT_NEW) TR_WarmCompilePICAddressInfo();

   printf("vi = %d ai = %d w = %d\n", valueInfoSize, addressInfoSize, warmCompilePICSize);fflush(stdout);

   TR_AbstractInfo *prevInfo = NULL;
   for (TR_AbstractInfo *valueInfo = coreVPInfo->getValues(); valueInfo; valueInfo = valueInfo->_next)
      {
      printf("found valueInfo %p %d\n", valueInfo, sizeof(*valueInfo));fflush(stdout);
      uintptrj_t v = ((TR_AddressInfo*)valueInfo)->_value1;
      void *oldvft = *(void**)(valueInfo);
      bool isClass = table->classInCHTable((TR_OpaqueClassBlock *)v);
      printf("value v = %p isclass = %d\n", v, isClass);fflush(stdout);
      if (v == 0xdeadf00d)
         {
         printf("unloaded or uninitialized??\n");fflush(stdout);
         ; //unloaded/uninitialized??
         //remove it from our list
         if (prevInfo)
            prevInfo->_next = valueInfo->_next;
         else
            coreVPInfo->setValues(valueInfo->_next);
         }
      else if (!isClass)
         {
         void *newvft = *(void**)(vInfo);
         *(void**)(valueInfo) = newvft;
         printf("not isClass\n");fflush(stdout);
         prevInfo = valueInfo;
         }
      else
         {
         void *newvft = *(void**)(aInfo);
         *(void**)(valueInfo) = newvft;
         printf("isClass\n");fflush(stdout);
         prevInfo = valueInfo;
         }
      /*
      if (v == 0xdeadf00d)
         {
         void *newvft = *(void**)(wInfo);
         *(void**)(valueInfo) = newvft;
         ;
         }
      else if (v < 0x1869f)
         {
         void *newvft = *(void**)(vInfo);
         *(void**)(valueInfo) = newvft;
         }
      else
         {
         void *newvft = *(void**)(aInfo);
         *(void**)(valueInfo) = newvft;
         }
      */

      printf("got value %p\n", v);fflush(stdout);
      }
   printf("finished setting up jit value profile info in ppi [%p]\n", ppi);fflush(stdout);
   }


// setup the JIT persistent profile info
//
static void setupJITProfilers(TR_Compilation *comp, TR_OpaqueMethodBlock *corej9method, TR_PersistentCHTable * table)
   {
   if (comp->getCoreCompilation())
      {
      printf("crash during compile %p\n", comp->getCoreCompilation());fflush(stdout);
      // crash during compile
      //
      TR_Compilation *coreCompilation = comp->getCoreCompilation();
      if (coreCompilation->getRecompilationInfo() &&
            coreCompilation->getRecompilationInfo()->getMethodInfo())
         {
         TR_PersistentProfileInfo *corePPI = coreCompilation->getRecompilationInfo()->getMethodInfo()->getProfileInfo();
         setupJitValueProfileInfo(corePPI, table);
         comp->getRecompilationInfo()->getMethodInfo()->setProfileInfo(corePPI);
         printf("persistent profile info [%p] set for method [%p]\n", corePPI, corej9method);
         }
      else
         printf("1persistent method info not found for method [%p]\n", corej9method);
      }
   else
      {
      void *startPC = (void *)(((J9Method *)corej9method)->extra);
      printf("fishing for persistent jitted bodyinfo startpc [%p] for method [%p]\n", startPC, corej9method);fflush(stdout);
      TR_PersistentJittedBodyInfo *corePJBI = TR_Recompilation::getJittedBodyInfoFromPC(startPC);
      if (corePJBI)
         {
         printf("persistent jitted bodyinfo [%p] for method [%p]\n", corePJBI, corej9method);
         TR_PersistentMethodInfo *corePMI = corePJBI->getMethodInfo();
         printf("persistent method info [%p] for method [%p]\n", corePMI, corej9method);fflush(stdout);
         if (corePMI)
            {
            setupJitValueProfileInfo(corePMI->getProfileInfo(), table);
            printf("setting up persistent profile info [%p] for method [%p]\n", corePMI, corej9method);fflush(stdout);
            comp->getRecompilationInfo()->getMethodInfo()->setProfileInfo(corePMI->getProfileInfo());
            printf("setup persistent profile info [%p] for method [%p]\n", corePMI->getProfileInfo(), corej9method);
            }
         else
            printf("persistent method info not found for method [%p]\n", corej9method);

         // set it up as a profiling compilation if required
         if (corePJBI->getIsProfilingBody())
            {
            comp->getRecompilationInfo()->setIsProfilingCompilation(true);
            printf("setup method [%p] as profiling compilation\n", corej9method);fflush(stdout);
            }
         }
      else
         printf("persistent jitted bodyinfo not found for method [%p]\n", corej9method);
      }
   printf("finished setting up JIT profiler entries\n");
   }

TR_YesNoMaybe
TR_J9VM::isMethodInInlinedCallSites(TR_ResolvedMethod *calleeResolvedMethod,
                                    TR_ByteCodeInfo &bcInfo,
                                    TR_Compilation *comp)
   {
   U_32 i = 0;
   J9JITExceptionTable * methodMetaData = (J9JITExceptionTable *)comp->getCurMethodMetadata();
   if (methodMetaData)
      {
      U_32 numCalls = getNumInlinedCalls(methodMetaData);
      //printf("Found inlinedMethods are: \n");
      trace2c(comp, "looking for bcInfo.getCallerIndex() = %d bcInfo.getByteCodeIndex() = %d\n", bcInfo.getCallerIndex(), bcInfo.getByteCodeIndex());
      trace1c(comp, "current inline depth = %d\n", comp->getInlineDepth());
      trace1c(comp, "current inline site index = %d\n", comp->getCurrentInlinedSiteIndex());
      if (calleeResolvedMethod)
         trace1c(comp, "current j9method = %p\n", calleeResolvedMethod->getPersistentIdentifier());
      else
         trace0c(comp, "calleeResolvedMethod is null\n");

      for (i=0; i<numCalls; i++)
         {
         U_32 sizeOfElement = sizeof(TR_InlinedCallSite) + ((J9JITStackAtlas *)methodMetaData->gcStackAtlas)->numberOfMapBytes;
         TR_InlinedCallSite *ics = (TR_InlinedCallSite *)((U_8*)methodMetaData->inlinedCalls + (i * sizeOfElement));
         ///J9Method *inlinedMethod = (J9Method *)(((TR_InlinedCallSite*)inlinedCallSite)->_vmMethodInfo);
         trace2c(comp, "ics->_byteCodeInfo.getCallerIndex() = %d ics->_byteCodeInfo.getByteCodeIndex() = %d\n", ics->_byteCodeInfo.getCallerIndex(), ics->_byteCodeInfo.getByteCodeIndex());
         if ((ics->_byteCodeInfo.getCallerIndex() == bcInfo.getCallerIndex()) &&
               (ics->_byteCodeInfo.getByteCodeIndex() == bcInfo.getByteCodeIndex()))
            {
            TR_OpaqueClassBlock *thisType = getClassFromMethodBlock(getInlinedCallSiteMethod(ics));
            trace2c(comp, "inliner: found target method [%p] class [%p]\n", (TR_OpaqueMethodBlock *) ics->_vmMethodInfo, thisType);
            trace2c(comp, "inliner: _callerIndex [%d] getByteCodeIndex() [%x]\n", ics->_byteCodeInfo.getCallerIndex(), ics->_byteCodeInfo.getByteCodeIndex());
            return TR_yes;
            }
         }
      return TR_no;
      }

   return TR_maybe;
   }

void
TR_J9VM::setUpForReplay(TR_OpaqueMethodBlock *corej9method, TR_Compilation *comp, bool cleanUp)
   {
   if (cleanUp)
      {
      // do the clean up
      TR_JitMemory::setupMemoryHeader(NULL, comp->getCurMemoryHeader());
      return;
      }

   static char *szJ9JavaVM    = feGetEnv("TR_REPLAY_J9VM");
   static char *szMetaData    = feGetEnv("TR_REPLAY_JITMETADATA");
   static char *szCompilation = feGetEnv("TR_REPLAY_COMPILATION");
   J9JavaVM *coreVM = (J9JavaVM *)strtoul( szJ9JavaVM, NULL, 16);
   J9JITExceptionTable * coreMethodMetadata = NULL;
   int32_t numInlinedCalls = -1;
   // TODO: find it from the j9method->extra (startPC) by looking for the eyecatcher
   // not set if the crash was during a compile
   if (szMetaData)
      {
      coreMethodMetadata = (J9JITExceptionTable *)strtoul( szMetaData, NULL, 16);
      numInlinedCalls = getNumInlinedCalls(coreMethodMetadata);
      printInlinedCalls(coreMethodMetadata);
      }

   //printf("setupJitConfig J9JavaVM %p JitConfig %p J9Method %p methodMetadata %p numInlinedCalls %d\n", coreVM, coreVM->jitConfig, corej9method, coreMethodMetadata, numInlinedCalls);
   J9JITConfig *coreConfig = coreVM->jitConfig;
   TR_PersistentMemory * coreSegment = (TR_PersistentMemory *)coreConfig->scratchSegment;
   TR_PersistentInfo *corePersistentInfo = coreSegment->_persistentInfo;
   //printf("setupJitConfig corePersistentInfo %p\n", corePersistentInfo);
   printf("Found CHTable at %p!\n", corePersistentInfo->getPersistentCHTable());

   // massage the CHTable to get the same inlined calls
   //
   if (coreMethodMetadata)
      {
      //setupCHTableForReplay(this, corePersistentInfo, coreMethodMetadata);
      printf("set current method metadata to %p\n", coreMethodMetadata);fflush(stdout);
      comp->setCurMethodMetadata(coreMethodMetadata);
      }
   else
      { // else crash during compile
      // walk inline call sites array hung off the compilation object
      //TR_Compilation *coreCompilation = ((TR_J9VMBase*)coreConfig->compilationInfo)->_compInfo->getCompilation();
      TR_Compilation *coreCompilation = NULL;
      if (szCompilation)
         coreCompilation = (TR_Compilation *)strtoul(szCompilation, NULL, 16);
      if (coreCompilation)
         {
         printf("Printing inlinedcalls from the compilation object: \n");
         printf("calleeIndex\t callerIndex\t byteCodeIndex\t j9method\n");
         for (uint32_t i = 0; i < coreCompilation->getNumInlinedCallSites(); ++i)
            {
            TR_InlinedCallSite & ics = coreCompilation->getInlinedCallSite(i);
            printf("    %4d       %4d       %4x        %p\n", i, ics._byteCodeInfo.getCallerIndex(),
                       ics._byteCodeInfo.getByteCodeIndex(), ics._methodInfo);
            }
         printf("Printing virtual guards from the compilation object : \n");
         List<TR_VirtualGuard> &vguards = coreCompilation->getVirtualGuards();
         ListIterator<TR_VirtualGuard> vguardsIt(&vguards);
         printf("   calleeIndex       byteCodeIndex      kind\n"); fflush(stdout);
         for (TR_VirtualGuard *info = vguardsIt.getFirst(); info; info = vguardsIt.getNext())
            {
            printf("   %4d        %4x       %4d\n", info->getCalleeIndex(), info->getByteCodeIndex(), info->getKind()); fflush(stdout);
            }
         comp->setCoreCompilation(coreCompilation);
         }
      }


   printf("setting up chtable\n");fflush(stdout);
   // now setup the CHTable in the persistent memory so that it is
   // available to the compile
   comp->setCurMemoryHeader(TR_JitMemory::getMemoryHeader());
   printf("patching memoryHeader with jitconfig %p\n", coreConfig);
   TR_JitMemory::setupMemoryHeader(coreConfig->scratchSegment, NULL);
   printf("chtable setup in persistent memory\n");fflush(stdout);

   // setup iprofiler entries
   setupIProfilerEntries(coreConfig, _jitConfig);
   // setup JIT profilers
   //
   setupJITProfilers(comp, corej9method, corePersistentInfo->getPersistentCHTable());
   corePersistentInfo->getPersistentCHTable()->fixUpClassLookAheadInfo();

   return;
   }

TR_OpaqueClassBlock *
TR_J9VM::getClassFromMethodBlock(TR_OpaqueMethodBlock *mb)
   {
   J9Class *ramClass = J9_CLASS_FROM_METHOD((J9Method *)mb);
   return (TR_OpaqueClassBlock *)ramClass;
   }

bool
TR_J9VM::testOSForSSESupport()
   {
   #if defined(TR_HOST_X86) && !defined(TR_CROSS_COMPILE_ONLY)
      return jitTestOSForSSESupport() != 0;
   #else
      return TR_J9VMBase::testOSForSSESupport();
   #endif
   }

uint32_t
TR_J9VM::getMethodSize(TR_OpaqueMethodBlock *method)
   {
   J9ROMMethod *romMethod = J9_ROM_METHOD_FROM_RAM_METHOD((J9Method *)method);

   return (uint32_t)(J9_BYTECODE_END_FROM_ROM_METHOD(romMethod) -
                     J9_BYTECODE_START_FROM_ROM_METHOD(romMethod));
   }

uintptrj_t
TR_J9VM::getMethodStart(TR_OpaqueMethodBlock *method)
   {
   J9ROMMethod *romMethod = J9_ROM_METHOD_FROM_RAM_METHOD((J9Method *)method);

   return (uintptrj_t)(J9_BYTECODE_START_FROM_ROM_METHOD(romMethod));
   }

// interpreter profiling support
TR_IProfiler *
TR_J9VM::getIProfiler()
   {
   if (!_iProfiler)
      {
      // This used to use a global variable called 'jitConfig' instead of the local object's '_jitConfig'.  In early out of memory scenarios, the global jitConfig may be NULL.
      _iProfiler = ((TR_JitPrivateConfig*)(_jitConfig->privateConfig))->iProfiler;
      }
   return _iProfiler;
   }

// HWProfiler support
TR_HWProfiler *
TR_J9VM::getHWProfiler()
   {
   if (!_hwProfiler)
      {
      _hwProfiler = ((TR_JitPrivateConfig*)(_jitConfig->privateConfig))->hwProfiler;
      }
   return _hwProfiler;
   }
   
bool
TR_J9VM::registerInstructionWithHWProfiler(uint8_t *instr, TR_Node *node, TR_Compilation *comp)
   {
   getHWProfiler()->registerInstruction(instr,getHWProfiler()->getPCFromBCInfo(node,comp));
   return true;
   }
   
TR_OpaqueClassBlock *
TR_J9VM::getClassOffsetForAllocationInlining(J9Class *clazzPtr)
   {
   return convertClassPtrToClassOffset(clazzPtr);
   }

J9Class *
TR_J9VM::getClassForAllocationInlining(TR_Compilation *comp, TR_SymbolReference *classSymRef)
   {
   if (!classSymRef->isUnresolved())
      return convertClassOffsetToClassPtr((TR_OpaqueClassBlock *)classSymRef->getSymbol()->getStaticSymbol()->getStaticAddress());

   return NULL;
   }


uint32_t
TR_J9VM::getAllocationSize(TR_StaticSymbol *classSym, TR_OpaqueClassBlock * opaqueClazz)
   {
   // We are asking for size of entire object. For packed classes, this only makes sense
   // for top level (i.e. of the nesting) object. Nested objects will only have pointers to
   // them in form of header-only (TR_newStructRef)
   J9Class * clazz = (J9Class * ) opaqueClazz;
   int32_t headerSize = sizeof(J9Object);
#ifdef J9VM_OPT_PACKED
   if (isPackedClass(opaqueClazz))
      headerSize = sizeof(J9PackedObject);
#endif
   int32_t objectSize = (int32_t)clazz->totalInstanceSize + headerSize;

   // gc requires objects to have
   // header + size >= 16
   objectSize = objectSize >= J9_GC_MINIMUM_OBJECT_SIZE ? objectSize : J9_GC_MINIMUM_OBJECT_SIZE;
   return objectSize;
   }

bool
TR_J9VM::supportAllocationInlining(TR_Compilation *comp, TR_Node *node)
   {
   return true;
   }

TR_OpaqueClassBlock *
TR_J9VM::getPrimitiveArrayAllocationClass(J9Class *clazz)
   {
   return (TR_OpaqueClassBlock *) clazz;
   }



//////////////////////////////////////////////////////////
// TR_J9SharedCacheVM
//////////////////////////////////////////////////////////

#if defined(J9VM_INTERP_AOT_COMPILE_SUPPORT)

bool
TR_J9SharedCacheVM::isMethodEnterTracingEnabled(TR_OpaqueMethodBlock *method)
   {
   // We want to return the same answer as TR_J9VMBase unless we want to force it to allow tracing
   return TR_J9VMBase::isMethodEnterTracingEnabled(method) || TR_Options::getAOTCmdLineOptions()->getOption(TR_EnableAOTMethodEnter);
   }

bool
TR_J9SharedCacheVM::isMethodExitTracingEnabled(TR_OpaqueMethodBlock *method)
   {
   return TR_J9VMBase::isMethodExitTracingEnabled(method) || TR_Options::getAOTCmdLineOptions()->getOption(TR_EnableAOTMethodExit);
   }

bool
TR_J9SharedCacheVM::traceableMethodsCanBeInlined()
   {
   return true;
   }

bool
TR_J9SharedCacheVM::canMethodEnterEventBeHooked()
   {
   // We want to return the same answer as TR_J9VMBase unless we want to force it to allow tracing
   return TR_J9VMBase::canMethodEnterEventBeHooked() || TR_Options::getAOTCmdLineOptions()->getOption(TR_EnableAOTMethodEnter);
   }

bool
TR_J9SharedCacheVM::canMethodExitEventBeHooked()
   {
   // We want to return the same answer as TR_J9VMBase unless we want to force it to allow tracing
   return TR_J9VMBase::canMethodExitEventBeHooked() || TR_Options::getAOTCmdLineOptions()->getOption(TR_EnableAOTMethodExit);
   }

bool
TR_J9SharedCacheVM::methodsCanBeInlinedEvenIfEventHooksEnabled()
   {
   return true;
   }

uint32_t
TR_J9SharedCacheVM::getInstanceFieldOffset(TR_OpaqueClassBlock * classPointer, char * fieldName, uint32_t fieldLen,
                                    char * sig, uint32_t sigLen, UDATA options)
   {

   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   
   if (((TR_ResolvedRelocatableJ9Method*) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class*)classPointer))
      return this->TR_J9VM::getInstanceFieldOffset (classPointer, fieldName, fieldLen, sig, sigLen, options);

   return ~0;	
   }

TR_OpaqueClassBlock *
TR_J9SharedCacheVM::getClassOfMethod(TR_OpaqueMethodBlock *method)
   {
   TR_OpaqueClassBlock *classPointer = TR_J9VM::getClassOfMethod(method);

   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) classPointer))
      return classPointer;

   return NULL;
   }

TR_OpaqueClassBlock *
TR_J9SharedCacheVM::getSuperClass(TR_OpaqueClassBlock * classPointer)
   {
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) classPointer))
      return TR_J9VM::getSuperClass(classPointer);

   return NULL;
   }

List<TR_OpaqueClassBlock> *
TR_J9SharedCacheVM::getInterfacesImplementedByClass(TR_OpaqueClassBlock *classPointer, List<TR_OpaqueClassBlock> *interfaces)
   {
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) classPointer))
      return TR_J9VM::getInterfacesImplementedByClass(classPointer, interfaces);

   return NULL;
   }


void
TR_J9SharedCacheVM::getResolvedMethods(TR_Memory *trMemory, TR_OpaqueClassBlock * classPointer, List<TR_ResolvedMethod> * resolvedMethodsInClass)
   {
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) classPointer))
      TR_J9VM::getResolvedMethods(trMemory, classPointer, resolvedMethodsInClass);
   }

TR_OpaqueMethodBlock *
TR_J9SharedCacheVM::getMethodFromName(char *className, char *methodName, char *signature, TR_OpaqueMethodBlock *callingMethod)
   {
   TR_OpaqueMethodBlock *omb = this->TR_J9VM::getMethodFromName(className, methodName, signature, callingMethod);
   if (omb)
      {
      J9Class* methodClass = (J9Class*)getClassFromMethodBlock(omb);

      TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
      assume (comp, "Should be called only within a compilation");
      if (!((TR_ResolvedRelocatableJ9Method*) comp->getCurrentMethod())->validateArbitraryClass(comp, methodClass))
         omb = NULL;
      }

   return omb;
   }

bool
TR_J9SharedCacheVM::isClassLibraryMethod(TR_OpaqueMethodBlock *method, bool vettedForAOT)
   {
   assume (vettedForAOT, "The TR_J9SharedCacheVM version of this method is expected to be called only from isClassLibraryMethod.\n"
                         "Please consider whether you call is vetted for AOT!");
   if (getSupportsRecognizedMethods())
      return this->TR_J9VM::isClassLibraryMethod(method, vettedForAOT);

   return false;
   }

TR_OpaqueMethodBlock *
TR_J9SharedCacheVM::getMethodFromClass(TR_OpaqueClassBlock * methodClass, char * methodName, char * signature)
   {
   TR_OpaqueMethodBlock* omb = this->TR_J9VM::getMethodFromClass(methodClass, methodName, signature);
   if (omb)
      {
      TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
      assume (comp, "Should be called only within a compilation");
      if (!((TR_ResolvedRelocatableJ9Method*) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class*)methodClass))
         omb = NULL;
      }

   return omb;
   }

bool
TR_J9SharedCacheVM::supportAllocationInlining(TR_Compilation *comp, TR_Node *node)
   {
   if (comp->getOptions()->realTimeGC())
      return false;

   if ((isX86((TR_Target)jitConfig->targetName) ||
        isPPC((TR_Target)jitConfig->targetName) ||
        is390((TR_Target)jitConfig->targetName)) &&
       !comp->getOption(TR_DisableAllocationInlining))
      return true;

   return false;
   }

bool
TR_J9SharedCacheVM::sameClassLoaders(TR_OpaqueClassBlock * class1, TR_OpaqueClassBlock * class2)
   {
   return false; // conservative answer: need a relocation to validate
   }

bool
TR_J9SharedCacheVM::isPublicClass(TR_OpaqueClassBlock * classPointer)
   {
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) classPointer))
      return TR_J9VM::isPublicClass(classPointer);

   return false;
   }

bool
TR_J9SharedCacheVM::hasFinalizer(TR_OpaqueClassBlock * classPointer)
   {
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) classPointer))
      return TR_J9VM::hasFinalizer(classPointer);

   return true;
   }

uintptrj_t
TR_J9SharedCacheVM::getClassDepthAndFlagsValue(TR_OpaqueClassBlock * classPointer)
   {
   return 0;
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) classPointer))
      return TR_J9VM::getClassDepthAndFlagsValue(classPointer);

   return 0;
   }


bool
TR_J9SharedCacheVM::isPrimitiveClass(TR_OpaqueClassBlock * classPointer)
   {
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) classPointer))
      return TR_J9VM::isPrimitiveClass(classPointer);

   return false;
   }

TR_OpaqueClassBlock *
TR_J9SharedCacheVM::getComponentClassFromArrayClass(TR_OpaqueClassBlock * arrayClassPointer)
   {
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) arrayClassPointer))
      return TR_J9VM::getComponentClassFromArrayClass(arrayClassPointer);

   return NULL;
   }

TR_OpaqueClassBlock *
TR_J9SharedCacheVM::getArrayClassFromComponentClass(TR_OpaqueClassBlock * componentClassPointer)
   {
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) componentClassPointer))
      return TR_J9VM::getArrayClassFromComponentClass(componentClassPointer);

   return NULL;
   }

TR_OpaqueClassBlock *
TR_J9SharedCacheVM::getLeafComponentClassFromArrayClass(TR_OpaqueClassBlock * arrayClassPointer)
   {
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) arrayClassPointer))
      return TR_J9VM::getLeafComponentClassFromArrayClass(arrayClassPointer);

   return NULL;
   }

TR_OpaqueClassBlock *
TR_J9SharedCacheVM::getBaseComponentClass(TR_OpaqueClassBlock * classPointer, int32_t & numDims)
   {
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) classPointer))
      return TR_J9VM::getBaseComponentClass(classPointer, numDims);

   return classPointer;  // suspicious original return value
   }

TR_OpaqueClassBlock *
TR_J9SharedCacheVM::getClassFromNewArrayType(int32_t arrayType)
   {
   return NULL;
   }

bool
TR_J9SharedCacheVM::isPrimitiveArray(TR_OpaqueClassBlock *classPointer)
   {
   return false;
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) classPointer))
      return TR_J9VM::isPrimitiveArray(classPointer);

   return false;
  }

bool
TR_J9SharedCacheVM::isReferenceArray(TR_OpaqueClassBlock *classPointer)
   {
   return false;
   TR_Compilation* comp = _compInfo->getCompInfoForCompOnAppThread() ? _compInfo->getCompInfoForCompOnAppThread()->getCompilation() : _compInfoPT->getCompilation();
   assume (comp, "Should be called only within a compilation");
   if (((TR_ResolvedRelocatableJ9Method *) comp->getCurrentMethod())->validateArbitraryClass(comp, (J9Class *) classPointer))
      return TR_J9VM::isReferenceArray(classPointer);

   return false;
  }

TR_OpaqueMethodBlock *
TR_J9SharedCacheVM::getInlinedCallSiteMethod(TR_InlinedCallSite *ics)
   {
   return (TR_OpaqueMethodBlock *)((TR_AOTMethodInfo *)(ics->_vmMethodInfo))->resolvedMethod->getPersistentIdentifier();
   }

TR_YesNoMaybe
TR_J9SharedCacheVM::isInstanceOf(TR_OpaqueClassBlock * a, TR_OpaqueClassBlock *b, bool objectTypeIsFixed, bool castTypeIsFixed, bool optimizeForAOT)
   {
   if (optimizeForAOT)
      {
      return TR_J9VM::isInstanceOf(a, b, objectTypeIsFixed, castTypeIsFixed, optimizeForAOT);
      }
   return TR_maybe;
   }

TR_OpaqueClassBlock *
TR_J9SharedCacheVM::getClassFromSignature(char * sig, int32_t sigLength, TR_ResolvedMethod * method, bool isVettedForAOT)
   {
   if (isVettedForAOT)
      {
      return TR_J9VM::getClassFromSignature(sig, sigLength, method, true);
      }
   return NULL;
   }

TR_OpaqueClassBlock *
TR_J9SharedCacheVM::getClassFromSignature(char * sig, int32_t sigLength, TR_OpaqueMethodBlock * method, bool isVettedForAOT)
   {
   if (isVettedForAOT)
      {
      return TR_J9VM::getClassFromSignature(sig, sigLength, method, true);
      }
   return NULL;
   }



// Answer these conservatively until we know how to do better
bool
TR_J9SharedCacheVM::isUnloadAssumptionRequired(TR_OpaqueClassBlock *, TR_ResolvedMethod *)   
   {
   return true;
   }

bool
TR_J9SharedCacheVM::classHasBeenExtended(TR_OpaqueClassBlock * classPointer)
   {
   return true;
   }

#if defined(J9VM_INTERP_AOT_COMPILE_SUPPORT)
bool
TR_J9SharedCacheVM::classHasBeenReplaced(TR_OpaqueClassBlock * classPointer)
   {
   return TR_J9VM::classHasBeenReplaced(classPointer);
   }
#endif

bool
TR_J9SharedCacheVM::isGetImplInliningSupported()
   {
   return false;
   }

TR_ResolvedMethod *
TR_J9SharedCacheVM::getObjectNewInstanceImplMethod(TR_Memory *)
   {
   return NULL;
   }

void
TR_J9SharedCacheVM::scanClassForReservation(TR_OpaqueClassBlock *classPointer, TR_Compilation *comp)
   {
   }

//////////////////

UDATA
TR_J9SharedCacheVM::getHeapBaseAddress()
   {
   return 0;
   }

UDATA
TR_J9SharedCacheVM::getShadowHeapProperties(bool getBase)
   {
   return 0;
   }

#if defined(TR_TARGET_S390)
void
TR_J9SharedCacheVM::initializeS390zLinuxProcessorFeatures()
   {
   return;
   }

void
TR_J9SharedCacheVM::initializeS390zOSProcessorFeatures()
   {
   return;
   }
#endif

TR_OpaqueClassBlock *
TR_J9SharedCacheVM::getSystemClassFromClassName(char * name, int32_t length)
   {
   return NULL;
   }

void *
TR_J9SharedCacheVM::getSystemClassLoader()
   {
   return NULL;
   }

bool
TR_J9SharedCacheVM::jitFieldsAreSame(TR_ResolvedMethod * method1, I_32 cpIndex1, TR_ResolvedMethod * method2, I_32 cpIndex2, int32_t isStatic)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   bool result = false;

   bool sigSame = true;
   if (method1->fieldsAreSame(cpIndex1, method2, cpIndex2, sigSame))
      result = true;
   else
      {
      if (sigSame)
         result = jitFieldsAreIdentical(vmThread(), (J9ConstantPool *)method1->ramConstantPool(), cpIndex1, (J9ConstantPool *)method2->ramConstantPool(), cpIndex2, isStatic) != 0;
      }

   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

bool
TR_J9SharedCacheVM::jitStaticsAreSame(TR_ResolvedMethod *method1, I_32 cpIndex1, TR_ResolvedMethod *method2, I_32 cpIndex2)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   bool result = false;
   bool sigSame = true;
   if (method1->staticsAreSame(cpIndex1, method2, cpIndex2, sigSame))
      result = true;
   else
      {
      if (sigSame)
         result = jitFieldsAreIdentical(vmThread(), (J9ConstantPool *)method1->ramConstantPool(), cpIndex1, (J9ConstantPool *)method2->ramConstantPool(), cpIndex2, true) != 0;
      }

   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return result;
   }

TR_MCCCodeCache *
TR_J9SharedCacheVM::getResolvedTrampoline(TR_Compilation *comp, TR_MCCCodeCache *, J9Method * method, bool inBinaryEncoding)
   {
   return 0;
   }

intptrj_t
TR_J9SharedCacheVM::methodTrampolineLookup(TR_Compilation *comp, TR_SymbolReference * symRef, void * callSite)
   {
   assume(0, "methodTrampolineLookup not implemented for AOT");
   return 0;
   }


TR_OpaqueClassBlock *
TR_J9SharedCacheVM::getClassClassPointer(TR_OpaqueClassBlock *objectClassPointer)
   {
   // AOTVM doesn't handle this query well, so for now
   return NULL;
   }

void TR_J9SharedCacheVM::getCompInfo(char *processorName)
   {
   return;
   }

void
TR_J9SharedCacheVM::setUpForReplay(TR_OpaqueMethodBlock *corej9method, TR_Compilation *comp, bool cleanUp)
   {
   //printf("this wont work yet for AOT\n");
   return;
   }


extern U_8 *align(U_8 *ptr, U_32 alignment);

// Multiple codeCache support
void *
TR_J9SharedCacheVM::getDesignatedCodeCache(TR_Hotness hotness, TR_Compilation *comp)
   {
   int32_t numReserved;
   int32_t compThreadID = comp ? comp->getCompThreadID() : -1;
   bool hadClassUnloadMonitor;
   bool hadVMAccess = releaseClassUnloadMonitorAndAcquireVMaccessIfNeeded(comp, &hadClassUnloadMonitor);
   TR_MCCCodeCache * mccCodeCache = TR_MCCManager::reserveCodeCache(hotness, true, 0, compThreadID, &numReserved);
   acquireClassUnloadMonitorAndReleaseVMAccessIfNeeded(comp, hadVMAccess, hadClassUnloadMonitor);
   // For AOT we need some alignment
   if (mccCodeCache)
      {
      mccCodeCache->warmCodeAlloc = align((U_8*)mccCodeCache->warmCodeAlloc, _jitConfig->codeCacheAlignment - 1);

      // For AOT we must install the beginning of the code cache
      comp->setAotMethodCodeStart((uint32_t *)mccCodeCache->warmCodeAlloc);
      }
   else
      {
      // If this is a temporary condition due to all code caches being reserved for the moment
      // we should retry this compilation
      if (!(jitConfig->runtimeFlags & J9JIT_CODE_CACHE_FULL))
         {
         if (numReserved > 0)
            {
            // set an error code so that the compilation is retried
            if (comp && !comp->isWCodeMethod())
               {
               comp->setErrorCode(COMPILATION_ALL_CODE_CACHES_RESERVED);
               j9OutOfMemory(_jitConfig, comp, "Cannot reserve code cache");
               }
            }
         }
      }
   return (void*)mccCodeCache;
   }


void *
TR_J9SharedCacheVM::getJ2IThunk(char *signatureChars, uint32_t signatureLength, TR_Compilation *comp)
   {
   return (void *)findPersistentThunk(signatureChars, signatureLength);
   }

void *
TR_J9SharedCacheVM::setJ2IThunk(char *signatureChars, uint32_t signatureLength, void *thunkptr, TR_Compilation *comp)
   {
   return persistThunk(signatureChars, signatureLength, (U_8*)thunkptr - 8 /* start of thunk */, *((U_32 *)((U_8*)thunkptr -8)) + 8 /* size of thunk */);
   }

void *
TR_J9SharedCacheVM::persistJ2IThunk(void *thunk)
   {
   return persistThunk(((TR_J2IThunk *)thunk)->terseSignature(), strlen(((TR_J2IThunk *)thunk)->terseSignature()), (uint8_t*)thunk, ((TR_J2IThunk *)thunk)->totalSize());
   }

void *
TR_J9SharedCacheVM::persistThunk(char *signatureChars, uint32_t signatureLength, uint8_t *thunkStart, uint32_t totalSize)
   {
#if defined(J9VM_INTERP_AOT_COMPILE_SUPPORT) && defined(J9VM_OPT_SHARED_CLASSES) && (defined(TR_HOST_X86) || defined(TR_HOST_POWER) || defined(TR_HOST_S390) || defined(TR_HOST_ARM))
   J9SharedDataDescriptor dataDescriptor;
   J9VMThread *curThread = getCurrentVMThread();

   dataDescriptor.address = (U_8*)thunkStart;
   dataDescriptor.length = totalSize;
   dataDescriptor.type = J9SHR_DATA_TYPE_AOTTHUNK;
   dataDescriptor.flags = 0;

   if (TR_Options::getAOTCmdLineOptions()->getOption(TR_TraceRelocatableDataDetailsCG))
      {
      TR_VerboseLog::write("<relocatableDataThunksDetailsCG>\n");

      TR_VerboseLog::write("%.*s\n", signatureLength, signatureChars);
      TR_VerboseLog::write("thunkAddress: %p, thunkSize: %x\n", dataDescriptor.address, dataDescriptor.length);
      TR_VerboseLog::write("thunkStart: %p\n", thunkStart);
      TR_VerboseLog::write("</relocatableDataThunksDetailsCG>\n");

      }

   const void* store= _jitConfig->javaVM->sharedClassConfig->storeSharedData(curThread, signatureChars, signatureLength, &dataDescriptor);
   if (!store) /* Store failed */
      outOfMemory(0, "Store failed");

#endif
   return thunkStart;
   }

void *
TR_J9SharedCacheVM::generateMethodCookie(void *ramMethod, TR_Compilation *comp)
   {
   return (void*)aotSharedGenerateCookie(jitConfig, (J9Method*)ramMethod, comp->trMemory());
   }

//////////////////////////////////////////////////////////
// TR_J9SharedCacheVM
//////////////////////////////////////////////////////////

#if defined(J9VM_OPT_SHARED_CLASSES)

TR_J9SharedCacheVM::TR_J9SharedCacheVM(J9JITConfig * jitConfig, J9VMThread * vmThread)
   : TR_J9VM(jitConfig,vmThread)
   {
   }

bool
TR_J9SharedCacheVM::registerInstructionWithHWProfiler(uint8_t *instr, TR_Node *node, TR_Compilation *comp)
   {
   return false;
   }
   
//d169771 [2177]
J9Class *
TR_J9SharedCacheVM::getClassForAllocationInlining(TR_Compilation *comp, TR_SymbolReference *classSymRef)
   {
   bool haveAcquiredVMAccess = acquireVMAccessIfNeeded();
   J9Class * resolvedClass;
   resolvedClass = _vmFunctionTable->resolveClassRef(vmThread(),
                                                     (J9ConstantPool *) classSymRef->getOwningMethod(comp)->ramConstantPool(),
                                                     classSymRef->getCPIndex(),
                                                     J9_RESOLVE_FLAG_JIT_COMPILE_TIME);
   releaseVMAccessIfNeeded(haveAcquiredVMAccess);
   return resolvedClass;
   }

#endif // defined J9VM_OPT_SHARED_CLASSES

#endif // defined J9VM_INTERP_AOT_COMPILE_SUPPORT


char * getClassNameChars(TR_Compilation * comp, TR_SymbolReference * symRef, int32_t & length) // this should be a method of TR_SymbolReference
   {
   TR_Symbol * sym = symRef->getSymbol();
   if (sym == NULL || !sym->isClassObject() || symRef->getCPIndex() <= 0)
      {
      if (!symRef->isUnresolved() && (sym->isClassObject() || sym->isAddressOfClassObject()))
         {
         void * classObject = (void *) (sym->castToStaticSymbol()->getStaticAddress());
         if (sym->isAddressOfClassObject())
            classObject = *(void**)classObject;
         return comp->fe()->getClassNameChars((TR_OpaqueClassBlock *) classObject, length);
         }

      length = 0;
      return NULL;
      }

   if (sym->addressIsCPIndexOfStatic())
      return symRef->getOwningMethod(comp)->classNameOfFieldOrStatic(symRef->getCPIndex(), length);

   TR_ResolvedMethod * method = symRef->getOwningMethod(comp);
   int32_t cpIndex = symRef->getCPIndex();

   assume(cpIndex != -1, "cpIndex shouldn't be -1");

   uint32_t len;
   char * n = method->getClassNameFromConstantPool(cpIndex, len);
   length = len;
   return n;
   }



// Native method bodies
//

JIT_HELPER(_initialInvokeExactThunkGlue);

JNIEXPORT jlong JNICALL Java_java_lang_invoke_ThunkTuple_initialInvokeExactThunk
   (JNIEnv *env, jclass clazz)
   {
#if defined(J9ZOS390)
   return (jlong)TOC_UNWRAP_ADDRESS(_initialInvokeExactThunkGlue);
#else
   return (jlong)_initialInvokeExactThunkGlue;
#endif
   }

JNIEXPORT jint JNICALL Java_java_lang_invoke_InterfaceHandle_convertITableIndexToVTableIndex
  (JNIEnv *env, jclass InterfaceMethodHandle, jlong interfaceArg, jint itableIndex, jlong receiverClassArg)
   {
   J9Class  *interfaceClass = (J9Class*)(intptrj_t)interfaceArg;
   J9Class  *receiverClass  = (J9Class*)(intptrj_t)receiverClassArg;
   J9ITable *itableEntry;
   for (itableEntry = (J9ITable*)receiverClass->iTable; itableEntry; itableEntry = itableEntry->next)
      if (itableEntry->interfaceClass == interfaceClass)
         break;
   assume(itableEntry, "Shouldn't call convertITableIndexToVTableIndex without first ensuring the receiver implements the interface");
   UDATA *itableArray = (UDATA*)(itableEntry+1);
   return (itableArray[itableIndex] - sizeof(J9Class))/sizeof(intptrj_t);
#if 0 // TODO:JSR292: We probably want to do something more like this instead, so it will properly handle exceptional cases
   struct
      {
      UDATA interfaceClass;
      UDATA methodIndex;
      } indexAndLiterals =
      {
      (UDATA)interfaceClass,
      (UDATA)itableIndex
      };
   J9Object *receiver = (J9Object*)receiverArg;
   return (jint)jitLookupInterfaceMethod(receiver->clazz, &indexAndLiterals, 0);
#endif
   }


extern "C" {
JNIEXPORT void JNICALL Java_java_lang_invoke_MutableCallSite_invalidate
  (JNIEnv *env, jclass MutableCallSite, jlongArray cookieArrayObject);
}

JNIEXPORT void JNICALL Java_java_lang_invoke_MutableCallSite_invalidate
  (JNIEnv *env, jclass MutableCallSite, jlongArray cookieArrayObject)
   {
   J9VMThread          *vmThread  = (J9VMThread*)env;
   J9JITConfig         *jitConfig = vmThread->javaVM->jitConfig;
   TR_FrontEnd         *fe        = TR_J9VMBase::get(jitConfig, vmThread);
   TR_RuntimeAssumptionTable *rat = TR_CompilationInfo::get(jitConfig)->getPersistentInfo()->getRuntimeAssumptionTable();

   bool verbose = TR_Options::getCmdLineOptions()->getVerboseOption(TR_VerboseHooks);
   bool details = TR_Options::getCmdLineOptions()->getVerboseOption(TR_VerboseHookDetails);
   int threadID = (int)(intptr_t)vmThread;

   if (verbose)
      TR_VerboseLog::writeLineLocked(TR_Vlog_HK, "%x hook %s vmThread=%p ", threadID, "MutableCallSite.invalidate", vmThread);

   jint numSites = env->GetArrayLength(cookieArrayObject);
   if (numSites <= 0)
      {
      if (verbose)
         TR_VerboseLog::writeLineLocked(TR_Vlog_HK, "%x   finished -- nothing to do", threadID);
      return;
      }

   jlong *cookies = (jlong*)alloca(numSites * sizeof(cookies[0]));
   env->GetLongArrayRegion(cookieArrayObject, 0, numSites, cookies);
   bool hasAcquiredVMAccess = fe->acquireClassTableMutex();
   for (int32_t i=0; i < numSites; i++)
      {
      jlong cookie = cookies[i];
      if (cookie)
         {
         if (details)
            TR_VerboseLog::writeLineLocked(TR_Vlog_HD, "%x     notifying cookies[%3d] " UINT64_PRINTF_FORMAT_HEX, threadID, i, cookie);
         rat->notifyMutableCallSiteChangeEvent(fe, cookie);
         }
      else
         {
         if (details)
            TR_VerboseLog::writeLineLocked(TR_Vlog_HD, "%x     skipping nonexistent cookies[%3d]", threadID, i);
         }
      }
   fe->releaseClassTableMutex(hasAcquiredVMAccess);
   if (verbose)
      TR_VerboseLog::writeLineLocked(TR_Vlog_HK, "%x   finished %d CallSites", threadID, numSites);
   }

// Log file handling
//
extern "C" {
FEFILE fefopen(TR_FrontEnd * fe, char * fileName, const char * mode, bool encrypt, bool suffix, char *suffixFormat)
   {
      char tmp[1025];
      fileName = fe->getFormattedName(tmp, 1025, fileName, suffixFormat, suffix);

   J9JITConfig * config = getJ9JitConfigFromFE(fe);
#if USE_J9_IO
   return j9jit_fopen(config, (char*)fileName, mode, true, encryp);
#else
   return j9jit_fopen(config, (char*)fileName, mode, false, encrypt);
#endif
   }

void fefclose(TR_FrontEnd * fe, FEFILE fileId)
   {
   J9JITConfig * config = getJ9JitConfigFromFE(fe);
   j9jit_fclose(config, (TR_File *)fileId);
   }

void fefseek(TR_FrontEnd * fe, FEFILE fileId, IDATA offset, I_32 whence)
   {
#ifdef USE_J9_IO
   // NOT IMPLEMENTED
   assume0(0);
#else
   fseek(((TR_File*)fileId)->_stream, (long)offset, whence);
#endif
   }

long feftell(TR_FrontEnd * fe, FEFILE fileId)
   {
#ifdef USE_J9_IO
   // NOT IMPLEMENTED
   assume0(0);
#else
   return ftell(((TR_File*)fileId)->_stream);
#endif
   }

void feLockRTlog(TR_FrontEnd * fe)
   {
   J9JITConfig * config = getJ9JitConfigFromFE(fe);
   j9jitrt_lock_log(config);
   }

void feUnlockRTlog(TR_FrontEnd * fe)
   {
   J9JITConfig * config = getJ9JitConfigFromFE(fe);
   j9jitrt_unlock_log(config);
   }

int fevprintf(TR_FrontEnd * fe, const char * format, va_list args)
   {
   J9JITConfig * config = getJ9JitConfigFromFE(fe);
   return j9jit_vprintf(config, (char*)format, args);
   }

void TR_VerboseLog::vlogAcquire() 
   {
   j9jit_lock_vlog(_config);
   }

void TR_VerboseLog::vlogRelease()
   {
   j9jit_unlock_vlog(_config);
   }
   
void TR_VerboseLog::vwrite(const char *format, va_list args) 
   {
   j9jit_vprintf((J9JITConfig *)_config, (char*)format, args);
   }

int fefprintf(TR_FrontEnd * fe, FEFILE fileId, const char * format, ...)
   {
   va_list args;
   J9JITConfig * config = getJ9JitConfigFromFE(fe);
   va_start(args, format);
   int length=j9jit_vfprintf(config, (TR_File*)fileId, (char*)format, args);
   va_end(args);
   return length;
   }

int fevfprintf(TR_FrontEnd * fe, FEFILE fileId, const char * format, va_list args)
   {
   J9JITConfig * config = getJ9JitConfigFromFE(fe);
   return j9jit_vfprintf(config, (TR_File*)fileId, (char*)format, args);
   }


void fefflush(TR_FrontEnd * fe, FEFILE fileId)
   {
   J9JITConfig * config = getJ9JitConfigFromFE(fe);
   j9jit_fflush(config, (TR_File *)fileId);
   }

void feDebugBreak()
   {
#if defined(LINUX)
   raise(SIGTRAP);
#elif defined(_MSC_VER)
   DebugBreak();
#else
   assert(0);
#endif
   }

//PLEASE DO NOT MOVE THE FOLLOWING CODE
//On zOS, fread maps to atoe_fread which converts read data from ebcdic to ascii,
// use fefread to read text files, and fefreadBin to read from binary files

IDATA fefread(TR_FrontEnd * fe, FEFILE fileId, void *buf, IDATA nbytes)
   {
#ifdef USE_J9_IO
   // NOT IMPLEMENTED
   assume0(0);
   return 0;
#else
   return fread(buf, 1, nbytes, ((TR_File*)fileId)->_stream); // returns # bytes read
#endif
   }

#if defined(J9ZOS390)
#undef fread
#endif

//On zOS, fread maps to atoe_fread which converts read data from ebcdic to ascii
//The above undef undoes the mapping and calls original fread from system library.
// so use fefreadBin to read binary data to ensure corect behaviour on zOS

IDATA fefreadBin(TR_FrontEnd * fe, FEFILE fileId, void *buf, IDATA nbytes)
   {
#ifdef USE_J9_IO
   // NOT IMPLEMENTED
   assume0(0);
   return 0;
#else
   return fread(buf, 1, nbytes, ((TR_File*)fileId)->_stream); // returns # bytes read
#endif
   }
//PLEASE DO NOT ADD ANY CODE THAT USES fread AFTER THIS

}

