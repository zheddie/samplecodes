changequote({,})dnl
define({J9CONST},{{$1}ifelse($2,,,{{($2}ifelse($3,,,{,$3}){)}})})dnl
define({ASM_J9VM_INTERP_ATOMIC_FREE_JNI},{J9CONST(1,$1,$2)})dnl
define({ASM_J9VM_JIT_NEW_DUAL_HELPERS},{J9CONST(1,$1,$2)})dnl
define({ASM_J9VM_ENV_LITTLE_ENDIAN},{J9CONST(1,$1,$2)})dnl
define({ASM_J9VM_ENV_DATA64},{J9CONST(1,$1,$2)})dnl
define({ASM_J9VM_INTERP_COMPRESSED_OBJECT_HEADER},{J9CONST(1,$1,$2)})dnl
define({ASM_J9VM_THR_LOCK_NURSERY},{J9CONST(1,$1,$2)})dnl
define({ASM_J9VM_INTERP_SMALL_MONITOR_SLOT},{J9CONST(1,$1,$2)})dnl
define({ASM_J9VM_GC_COMPRESSED_POINTERS},{J9CONST(1,$1,$2)})dnl
define({ASM_J9VM_GC_TLH_PREFETCH_FTA},{J9CONST(1,$1,$2)})dnl
define({ASM_J9VM_GC_DYNAMIC_CLASS_UNLOADING},{J9CONST(1,$1,$2)})dnl
define({OMR_GC_CONCURRENT_SCAVENGER},{J9CONST(1,$1,$2)})dnl
define({J9TR_cframe_sizeof},{J9CONST(280,$1,$2)})dnl
define({J9TR_cframe_vmStruct},{J9CONST(0,$1,$2)})dnl
define({J9TR_cframe_machineBP},{J9CONST(8,$1,$2)})dnl
define({J9TR_cframe_jitGPRs},{J9CONST(16,$1,$2)})dnl
define({J9TR_cframe_jitFPRs},{J9CONST(144,$1,$2)})dnl
define({J9TR_cframe_rax},{J9CONST(16,$1,$2)})dnl
define({J9TR_cframe_rbx},{J9CONST(24,$1,$2)})dnl
define({J9TR_cframe_rcx},{J9CONST(32,$1,$2)})dnl
define({J9TR_cframe_rdx},{J9CONST(40,$1,$2)})dnl
define({J9TR_cframe_rdi},{J9CONST(48,$1,$2)})dnl
define({J9TR_cframe_rsi},{J9CONST(56,$1,$2)})dnl
define({J9TR_cframe_rbp},{J9CONST(64,$1,$2)})dnl
define({J9TR_cframe_rsp},{J9CONST(72,$1,$2)})dnl
define({J9TR_cframe_r8},{J9CONST(80,$1,$2)})dnl
define({J9TR_cframe_r9},{J9CONST(88,$1,$2)})dnl
define({J9TR_cframe_r10},{J9CONST(96,$1,$2)})dnl
define({J9TR_cframe_r11},{J9CONST(104,$1,$2)})dnl
define({J9TR_cframe_r12},{J9CONST(112,$1,$2)})dnl
define({J9TR_cframe_r13},{J9CONST(120,$1,$2)})dnl
define({J9TR_cframe_r14},{J9CONST(128,$1,$2)})dnl
define({J9TR_cframe_r15},{J9CONST(136,$1,$2)})dnl
define({J9TR_runtimeFlags_PatchingFenceRequired},{J9CONST(67108864,$1,$2)})dnl
define({J9TR_runtimeFlags_PatchingFenceType},{J9CONST(134217728,$1,$2)})dnl
define({J9TR_VMThread_machineSP},{J9CONST(2432,$1,$2)})dnl
define({J9TR_machineSP_machineBP},{J9CONST(8,$1,$2)})dnl
define({J9TR_machineSP_vmStruct},{J9CONST(0,$1,$2)})dnl
define({J9TR_VMThreadCurrentException},{J9CONST(72,$1,$2)})dnl
define({J9TR_VMThread_floatTemp1},{J9CONST(264,$1,$2)})dnl
define({J9TR_VMThread_floatTemp2},{J9CONST(272,$1,$2)})dnl
define({J9TR_VMThread_floatTemp3},{J9CONST(280,$1,$2)})dnl
define({J9TR_VMThread_floatTemp4},{J9CONST(288,$1,$2)})dnl
define({J9TR_VMThread_stackOverflowMark},{J9CONST(80,$1,$2)})dnl
define({J9TR_VMThread_heapAlloc},{J9CONST(96,$1,$2)})dnl
define({J9TR_VMThread_heapTop},{J9CONST(104,$1,$2)})dnl
define({J9TR_VMThread_tlhPrefetchFTA},{J9CONST(112,$1,$2)})dnl
define({J9TR_VMThread_publicFlags},{J9CONST(152,$1,$2)})dnl
define({J9TR_VMThread_privateFlags},{J9CONST(424,$1,$2)})dnl
define({J9TR_VMThread_privateFlags2},{J9CONST(2336,$1,$2)})dnl
define({J9TR_VMThreadJavaVM},{J9CONST(8,$1,$2)})dnl
define({J9TR_VMThread_javaVM},{J9CONST(8,$1,$2)})dnl
define({J9TR_VMThread_sp},{J9CONST(32,$1,$2)})dnl
define({J9TR_VMThread_pc},{J9CONST(40,$1,$2)})dnl
define({J9TR_VMThread_literals},{J9CONST(48,$1,$2)})dnl
define({J9TR_VMThread_arg0EA},{J9CONST(16,$1,$2)})dnl
define({J9TR_VMThread_codertTOC},{J9CONST(344,$1,$2)})dnl
define({J9TR_VMThread_debugEventData1},{J9CONST(496,$1,$2)})dnl
define({J9TR_VMThread_debugEventData2},{J9CONST(504,$1,$2)})dnl
define({J9TR_VMThread_debugEventData3},{J9CONST(512,$1,$2)})dnl
define({J9TR_VMThread_debugEventData4},{J9CONST(520,$1,$2)})dnl
define({J9TR_VMThread_debugEventData5},{J9CONST(528,$1,$2)})dnl
define({J9TR_VMThread_debugEventData6},{J9CONST(536,$1,$2)})dnl
define({J9TR_VMThread_debugEventData7},{J9CONST(544,$1,$2)})dnl
define({J9TR_VMThread_debugEventData8},{J9CONST(552,$1,$2)})dnl
define({J9TR_VMThread_tempSlot},{J9CONST(248,$1,$2)})dnl
define({J9TR_VMThread_jitReturnAddress},{J9CONST(256,$1,$2)})dnl
define({J9TR_VMThread_returnValue},{J9CONST(296,$1,$2)})dnl
define({J9TR_VMThread_returnValue2},{J9CONST(304,$1,$2)})dnl
define({J9TR_VMThread_entryLocalStorage},{J9CONST(616,$1,$2)})dnl
define({J9TR_VMThread_stackWalkState},{J9CONST(608,$1,$2)})dnl
define({J9TR_J9StackWalkState_restartPoint},{J9CONST(224,$1,$2)})dnl
define({J9TR_JavaVMJitConfig},{J9CONST(6488,$1,$2)})dnl
define({J9TR_JavaVM_runtimeFlags},{J9CONST(224,$1,$2)})dnl
define({J9TR_JavaVM_cInterpreter},{J9CONST(184,$1,$2)})dnl
define({J9TR_JavaVM_bytecodeLoop},{J9CONST(192,$1,$2)})dnl
define({J9TR_JavaVM_extendedRuntimeFlags},{J9CONST(228,$1,$2)})dnl
define({J9TR_JavaVMInternalFunctionTable},{J9CONST(0,$1,$2)})dnl
define({J9TR_JavaVM_memoryManagerFunctions},{J9CONST(320,$1,$2)})dnl
define({J9TR_JavaVM_compressedPointersShift},{J9CONST(18376,$1,$2)})dnl
define({J9TR_J9MemoryManagerFunctions_J9ReadBarrier},{J9CONST(512,$1,$2)})dnl
define({J9TR_J9MemoryManagerFunctions_referenceArrayCopy},{J9CONST(448,$1,$2)})dnl
define({J9TR_ELS_jitGlobalStorageBase},{J9CONST(8,$1,$2)})dnl
define({J9TR_ELS_jitFPRegisterStorageBase},{J9CONST(48,$1,$2)})dnl
define({J9TR_ELS_machineSPSaveSlot},{J9CONST(88,$1,$2)})dnl
define({J9TR_J9VMJITRegisterState_jit_fpr0},{J9CONST(128,$1,$2)})dnl
define({J9TR_J9Class_classLoader},{J9CONST(40,$1,$2)})dnl
define({J9TR_J9Class_lastITable},{J9CONST(96,$1,$2)})dnl
define({J9TR_J9Class_lockOffset},{J9CONST(200,$1,$2)})dnl
define({J9TR_ArrayClass_componentType},{J9CONST(88,$1,$2)})dnl
define({J9TR_ITableOffset},{J9CONST(24,$1,$2)})dnl
define({J9TR_J9ITable_interfaceClass},{J9CONST(0,$1,$2)})dnl
define({J9TR_MethodFlagsOffset},{J9CONST(8,$1,$2)})dnl
define({J9TR_MethodPCStartOffset},{J9CONST(24,$1,$2)})dnl
define({J9TR_JitConfig_runtimeFlags},{J9CONST(1168,$1,$2)})dnl
define({J9TR_JitConfig_loadPreservedAndBranch},{J9CONST(2896,$1,$2)})dnl
define({J9TR_JitConfig_pseudoTOC},{J9CONST(1904,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitNewObject},{J9CONST(8,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitNewObject},{J9CONST(16,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitNewObjectNoZeroInit},{J9CONST(24,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitNewObjectNoZeroInit},{J9CONST(32,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitANewArray},{J9CONST(40,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitANewArray},{J9CONST(48,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitANewArrayNoZeroInit},{J9CONST(56,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitANewArrayNoZeroInit},{J9CONST(64,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitNewArray},{J9CONST(72,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitNewArray},{J9CONST(80,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitNewArrayNoZeroInit},{J9CONST(88,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitNewArrayNoZeroInit},{J9CONST(96,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitAMultiNewArray},{J9CONST(104,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitStackOverflow},{J9CONST(112,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveString},{J9CONST(120,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitAcquireVMAccess},{J9CONST(128,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitReleaseVMAccess},{J9CONST(136,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitCheckAsyncMessages},{J9CONST(144,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitCheckCast},{J9CONST(152,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitCheckCast},{J9CONST(160,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitCheckCastForArrayStore},{J9CONST(168,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitCheckCastForArrayStore},{J9CONST(176,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitCheckIfFinalizeObject},{J9CONST(184,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitCollapseJNIReferenceFrame},{J9CONST(192,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitHandleArrayIndexOutOfBoundsTrap},{J9CONST(200,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitHandleIntegerDivideByZeroTrap},{J9CONST(208,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitHandleNullPointerExceptionTrap},{J9CONST(216,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitHandleInternalErrorTrap},{J9CONST(224,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitInstanceOf},{J9CONST(232,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitLookupInterfaceMethod},{J9CONST(240,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitLookupInterfaceMethod},{J9CONST(248,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitMethodIsNative},{J9CONST(256,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitMethodIsSync},{J9CONST(264,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitMethodMonitorEntry},{J9CONST(272,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitMethodMonitorEntry},{J9CONST(280,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitMonitorEntry},{J9CONST(288,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitMonitorEntry},{J9CONST(296,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitMethodMonitorExit},{J9CONST(304,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitMethodMonitorExit},{J9CONST(312,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowIncompatibleReceiver},{J9CONST(320,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitMonitorExit},{J9CONST(328,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitMonitorExit},{J9CONST(336,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitReportMethodEnter},{J9CONST(344,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitReportStaticMethodEnter},{J9CONST(352,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitReportMethodExit},{J9CONST(360,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveClass},{J9CONST(368,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveClassFromStaticField},{J9CONST(376,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitResolvedFieldIsVolatile},{J9CONST(384,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveField},{J9CONST(392,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveFieldSetter},{J9CONST(400,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveFieldDirect},{J9CONST(408,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveFieldSetterDirect},{J9CONST(416,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveStaticField},{J9CONST(424,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveStaticFieldSetter},{J9CONST(432,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveStaticFieldDirect},{J9CONST(440,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveStaticFieldSetterDirect},{J9CONST(448,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveInterfaceMethod},{J9CONST(456,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveSpecialMethod},{J9CONST(464,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveStaticMethod},{J9CONST(472,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveVirtualMethod},{J9CONST(480,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveMethodType},{J9CONST(488,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveMethodHandle},{J9CONST(496,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveInvokeDynamic},{J9CONST(504,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveConstantDynamic},{J9CONST(512,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitResolveHandleMethod},{J9CONST(520,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitRetranslateCaller},{J9CONST(528,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitRetranslateCallerWithPreparation},{J9CONST(536,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitRetranslateMethod},{J9CONST(544,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowCurrentException},{J9CONST(552,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowException},{J9CONST(560,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowAbstractMethodError},{J9CONST(568,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowArithmeticException},{J9CONST(576,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowArrayIndexOutOfBounds},{J9CONST(584,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowArrayStoreException},{J9CONST(592,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowArrayStoreExceptionWithIP},{J9CONST(600,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowExceptionInInitializerError},{J9CONST(608,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowIllegalAccessError},{J9CONST(616,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowIncompatibleClassChangeError},{J9CONST(624,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowInstantiationException},{J9CONST(632,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowNullPointerException},{J9CONST(640,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitThrowWrongMethodTypeException},{J9CONST(648,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitTypeCheckArrayStoreWithNullCheck},{J9CONST(656,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitTypeCheckArrayStoreWithNullCheck},{J9CONST(664,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitTypeCheckArrayStore},{J9CONST(672,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitTypeCheckArrayStore},{J9CONST(680,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitWriteBarrierBatchStore},{J9CONST(688,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitWriteBarrierBatchStoreWithRange},{J9CONST(696,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitWriteBarrierJ9ClassBatchStore},{J9CONST(704,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitWriteBarrierJ9ClassStore},{J9CONST(712,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitWriteBarrierStore},{J9CONST(720,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitWriteBarrierStoreGenerational},{J9CONST(728,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitWriteBarrierStoreGenerationalAndConcurrentMark},{J9CONST(736,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitWriteBarrierClassStoreMetronome},{J9CONST(744,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitWriteBarrierStoreMetronome},{J9CONST(752,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitCallJitAddPicToPatchOnClassUnload},{J9CONST(760,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitCallCFunction},{J9CONST(768,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitPreJNICallOffloadCheck},{J9CONST(776,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitPostJNICallOffloadCheck},{J9CONST(784,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitObjectHashCode},{J9CONST(792,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitInduceOSRAtCurrentPC},{J9CONST(800,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitInduceOSRAtCurrentPCAndRecompile},{J9CONST(808,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitInterpretNewInstanceMethod},{J9CONST(816,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitNewInstanceImplAccessCheck},{J9CONST(824,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitTranslateNewInstanceMethod},{J9CONST(832,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitReportFinalFieldModified},{J9CONST(840,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitReportInstanceFieldRead},{J9CONST(1064,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitReportInstanceFieldWrite},{J9CONST(1072,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitReportStaticFieldRead},{J9CONST(1080,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_jitReportStaticFieldWrite},{J9CONST(1088,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitNewObject},{J9CONST(848,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitNewObjectNoZeroInit},{J9CONST(856,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitANewArray},{J9CONST(864,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitANewArrayNoZeroInit},{J9CONST(872,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitNewArray},{J9CONST(880,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitNewArrayNoZeroInit},{J9CONST(888,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitCheckCast},{J9CONST(896,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitCheckCastForArrayStore},{J9CONST(904,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitMethodMonitorEntry},{J9CONST(912,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitMonitorEntry},{J9CONST(920,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitMethodMonitorExit},{J9CONST(928,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitMonitorExit},{J9CONST(936,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitTypeCheckArrayStore},{J9CONST(944,$1,$2)})dnl
define({J9TR_JitConfig_fast_jitTypeCheckArrayStoreWithNullCheck},{J9CONST(952,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitVolatileReadLong},{J9CONST(960,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitVolatileWriteLong},{J9CONST(968,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitVolatileReadDouble},{J9CONST(976,$1,$2)})dnl
define({J9TR_JitConfig_old_fast_jitVolatileWriteDouble},{J9CONST(984,$1,$2)})dnl
define({J9TR_JitConfig_old_slow_icallVMprJavaSendPatchupVirtual},{J9CONST(992,$1,$2)})dnl
define({J9TR_JitConfig_c_jitDecompileOnReturn},{J9CONST(1000,$1,$2)})dnl
define({J9TR_JitConfig_c_jitDecompileAtExceptionCatch},{J9CONST(1008,$1,$2)})dnl
define({J9TR_JitConfig_c_jitReportExceptionCatch},{J9CONST(1016,$1,$2)})dnl
define({J9TR_JitConfig_c_jitDecompileAtCurrentPC},{J9CONST(1024,$1,$2)})dnl
define({J9TR_JitConfig_c_jitDecompileBeforeReportMethodEnter},{J9CONST(1032,$1,$2)})dnl
define({J9TR_JitConfig_c_jitDecompileBeforeMethodMonitorEnter},{J9CONST(1040,$1,$2)})dnl
define({J9TR_JitConfig_c_jitDecompileAfterAllocation},{J9CONST(1048,$1,$2)})dnl
define({J9TR_JitConfig_c_jitDecompileAfterMonitorEnter},{J9CONST(1056,$1,$2)})dnl
define({J9TR_InternalFunctionTableReleaseVMAccess},{J9CONST(312,$1,$2)})dnl
define({J9TR_InternalFunctionTableExitVMToJNI},{J9CONST(2208,$1,$2)})dnl
define({J9TR_J9Object_class},{J9CONST(0,$1,$2)})dnl
define({J9TR_ObjectHeader_class},{J9CONST(0,$1,$2)})dnl
define({J9TR_IndexableObjectContiguous_objectData},{J9CONST(8,$1,$2)})dnl
define({J9TR_J9SFJNICallInFrame_exitAddress},{J9CONST(0,$1,$2)})dnl
define({J9TR_J9JITWatchedInstanceFieldData_method},{J9CONST(0,$1,$2)})dnl
define({J9TR_J9JITWatchedInstanceFieldData_location},{J9CONST(8,$1,$2)})dnl
define({J9TR_J9JITWatchedInstanceFieldData_offset},{J9CONST(16,$1,$2)})dnl
define({J9TR_J9JITWatchedStaticFieldData_method},{J9CONST(0,$1,$2)})dnl
define({J9TR_J9JITWatchedStaticFieldData_location},{J9CONST(8,$1,$2)})dnl
define({J9TR_J9JITWatchedStaticFieldData_fieldAddress},{J9CONST(16,$1,$2)})dnl
define({J9TR_J9JITWatchedStaticFieldData_fieldClass},{J9CONST(24,$1,$2)})dnl
define({J9TR_bcloop_execute_bytecode},{J9CONST(0,$1,$2)})dnl
define({J9TR_bcloop_handle_pop_frames},{J9CONST(10,$1,$2)})dnl
define({J9TR_bcloop_i2j_transition},{J9CONST(16,$1,$2)})dnl
define({J9TR_bcloop_j2i_invoke_exact},{J9CONST(12,$1,$2)})dnl
define({J9TR_bcloop_j2i_transition},{J9CONST(11,$1,$2)})dnl
define({J9TR_bcloop_j2i_virtual},{J9CONST(13,$1,$2)})dnl
define({J9TR_bcloop_jump_bytecode_prototype},{J9CONST(2,$1,$2)})dnl
define({J9TR_bcloop_load_preserved_and_branch},{J9CONST(14,$1,$2)})dnl
define({J9TR_bcloop_return_from_jit},{J9CONST(17,$1,$2)})dnl
define({J9TR_bcloop_return_from_jit_ctor},{J9CONST(18,$1,$2)})dnl
define({J9TR_bcloop_run_exception_handler},{J9CONST(4,$1,$2)})dnl
define({J9TR_bcloop_run_jni_native},{J9CONST(5,$1,$2)})dnl
define({J9TR_bcloop_run_method},{J9CONST(1,$1,$2)})dnl
define({J9TR_bcloop_run_method_compiled},{J9CONST(3,$1,$2)})dnl
define({J9TR_bcloop_run_method_handle},{J9CONST(7,$1,$2)})dnl
define({J9TR_bcloop_run_method_handle_compiled},{J9CONST(8,$1,$2)})dnl
define({J9TR_bcloop_run_method_interpreted},{J9CONST(9,$1,$2)})dnl
define({J9TR_bcloop_stack_overflow},{J9CONST(6,$1,$2)})dnl
define({J9TR_bcloop_throw_current_exception},{J9CONST(15,$1,$2)})dnl
define({J9TR_bcloop_enter_method_monitor},{J9CONST(19,$1,$2)})dnl
define({J9TR_bcloop_report_method_enter},{J9CONST(20,$1,$2)})dnl
define({J9TR_bcloop_exit_interpreter},{J9CONST(22,$1,$2)})dnl
define({J9TR_MethodNotCompiledBit},{J9CONST(1,$1,$2)})dnl
define({J9TR_InterpVTableOffset},{J9CONST(336,$1,$2)})dnl
define({J9TR_RequiredClassAlignment},{J9CONST(256,$1,$2)})dnl
define({J9TR_RequiredClassAlignmentInBits},{J9CONST(8,$1,$2)})dnl
define({J9TR_pointerSize},{J9CONST(8,$1,$2)})dnl
define({J9TR_ELSSize},{J9CONST(96,$1,$2)})dnl
define({J9TR_J9_EXTENDED_RUNTIME_DEBUG_MODE},{J9CONST(16384,$1,$2)})dnl
define({J9TR_J9_EXTENDED_RUNTIME_USE_VECTOR_REGISTERS},{J9CONST(1,$1,$2)})dnl
define({J9TR_J9_INLINE_JNI_MAX_ARG_COUNT},{J9CONST(32,$1,$2)})dnl
define({J9TR_J9_ITABLE_OFFSET_DIRECT},{J9CONST(1,$1,$2)})dnl
define({J9TR_J9_ITABLE_OFFSET_VIRTUAL},{J9CONST(2,$1,$2)})dnl
define({J9TR_J9_ITABLE_OFFSET_TAG_BITS},{J9CONST(3,$1,$2)})dnl
define({J9TR_J9_VTABLE_INDEX_DIRECT_METHOD_FLAG},{J9CONST(1,$1,$2)})dnl
