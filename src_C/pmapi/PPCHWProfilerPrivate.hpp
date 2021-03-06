/*
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2000, 2016 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * This file is part of product 5655-W32.
 *
 */

#ifndef PPCHWPROFILERPRIVATE_INCL
#define PPCHWPROFILERPRIVATE_INCL
   #define OMR_LIKELY(expr)   ((expr) == 1)
   #define OMR_UNLIKELY(expr) ((expr) == 0)
#define SIER            768
#define PMC1            771
#define PMC2            772
#define PMC3            773
#define PMC4            774
#define PMC5            775
#define PMC6            776
#define MMCR0           779
#define MMCR0_FC        0x80000000
#define MMCR0_PMAE      0x04000000
#define MMCR0_PMAO      0x00000080
#define MMCR2           769
#define MMCR2_FCnP      128ULL
#define MMCRA           770
#define SIAR            780
#define SDAR            781
#define BESCRS          800
#define BESCRSU         801
#define BESCRR          802
#define BESCRRU         803
#define BESCRU_GE       0x80000000
#define BESCRU_PME      0x00000001
#define BESCRU_LME      0x00000004
#define BESCR_PMEO      0x00000001
#define BESCR_LMEO      0x00000004
#define EBBHR           804
#define EBBRR           805
#define BESCR           806

#define BHRBE_EA_MASK   (~3L)
#define BHRBE_T_MASK    2L
#define BHRBE_POWER_MASK    1L

extern "builtin" void __mtspr(int, unsigned long);

#if defined(__xlC__) || defined(__ibmxl__)
#define MTSPR(spr, src)         __mtspr(spr, src)
#define MFSPR(dst, spr)         ((dst) = __mfspr(spr))
#elif defined(__GNUC__)
#define MTSPR(spr, src)         asm("mtspr      %0, %1  \n\t" : : "i" (spr), "r" (src))
#define MFSPR(dst, spr)         asm("mfspr      %0, %1  \n\t" : "=r" (dst) : "i" (spr))
#endif /* GNUC */


typedef unsigned char boolean;
/* ANSI C++ compilers have keywords "bool," "true" and "false."  For
   non-ANSI compiles, we define the same identifiers with the same meanings. */
#if !((defined(__USLC__) && defined(__cplusplus)) || \
      (defined(__IBMCPP__) && (__IBMCPP__ >= 400)) || \
      (defined(__GNUC__) && defined(__cplusplus))) || \
    defined(ON390)
typedef unsigned char bool;    /* use this one */
#ifdef __cplusplus
enum { false = 0, true = 1};
#else
#define false 0
#define true  1
#endif
#endif

#undef FALSE
#define FALSE 0
#undef TRUE
#define TRUE  1

#define VERBOSE(...)                                                                    \
   do                                                                                   \
      {                                                                                 \
      if (OMR_UNLIKELY(TR::Options::isAnyVerboseOptionSet(TR_VerboseHWProfiler)))        \
         {                                                                              \
         TR_VerboseLog::writeLineLocked(TR_Vlog_HWPROFILER, __VA_ARGS__);               \
         }                                                                              \
      }                                                                                 \
   while (0)

#if defined(AIXPPC)
#define FUNCTION_ADDRESS(x) (*(void**)(x))
#elif defined(LINUXPPC64) && !defined(__LITTLE_ENDIAN__)
#define FUNCTION_ADDRESS(x) (*(uintptr_t*)(x))
#elif defined(LINUXPPC) || defined(__LITTLE_ENDIAN__)
#define FUNCTION_ADDRESS(x) ((uintptr_t)x)
#endif /* LINUXPPC || __LITTLE_ENDIAN__ */

#define MAX_PMCS 6
#define MAX_BHRBES 32
#define INVALID_SAMPLERATE 0xFFFFFFFF

enum TR_PPCHWProfilingConfigs
   {
   MethodHotness,       // PMU samples instructions executed, BHRB samples BLs
   BlockHotness,        // PMU samples instructions executed, BHRB samples conditional branches
   FieldHotness,        // PMU samples LSU instructions executed, loads that missed cache
   NumProfilingConfigs
   };

struct TR_PPCHWProfilerEBBContext;
struct J9VMThread;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef int (*zgfuncp)(char *);
bool x;

typedef int32_t (*pmBaseEventHandlerFunc)(struct TR_PPCHWProfilerEBBContext *);
typedef int32_t (*lmEventHandlerFunc)(struct TR_PPCHWProfilerEBBContext *);
typedef int32_t (*pmEventHandlerFunc)(struct TR_PPCHWProfilerEBBContext *, int32_t);


struct TR_PPCHWProfilerEBBContext
   {
   // If you change the layout of this struct, make sure you account for it in ebb.s
   uintptr_t              savedRegs[32];
   uintptr_t              lr, xer, ebbrr;
   pmBaseEventHandlerFunc eventHandler;
   lmEventHandlerFunc     lmEventHandler;
   struct J9VMThread      *vmThread;
   uint32_t               eventCounts[MAX_PMCS];
   uint32_t               sampleRates[MAX_PMCS];
   uint32_t               unknownEventCount;
   bool                   lostPMU;

   TR_PPCHWProfilingConfigs currentConfig;

   struct
      {
      void         *start;
      uint32_t      spaceLeft;
      } buffers[MAX_PMCS];
#ifdef LINUXPPC
   int           fds[MAX_PMCS];
#endif /* LINUXPPC */
   
   char                    counterBitMask;
   };

struct TR_PPCMethodHotnessSample
   {
   uintptr_t sia;                       // Address of the sampled instruction
#ifdef HAVE_BHRBES
   uintptr_t calls[MAX_BHRBES];         // Address of BLs from the BHRB
#endif /* HAVE_BHRBES */
   };

struct TR_PPCBlockHotnessSample
   {
   uintptr_t  sia;                      // Address of the sampled instruction
#ifdef HAVE_BHRBES
   uintptr_t  branches[MAX_BHRBES];     // Address of branches from BHRB
#endif /* HAVE_BHRBES */
   };

struct TR_PPCFieldHotnessSample
   {
   uintptr_t ia;                        // Address of the sampled load/store instruction
   uintptr_t dia;                       // Address of the data that was accessed
   };

enum tr_pm_bhrb_ifm_t
   {
   BHRB_IFM0 = 0,                       // All branches
   BHRB_IFM1,                           // All branches with LK=1
   BHRB_IFM2,                           // Conditional branches only, targets of unconditional indirect branches
   BHRB_IFM3                            // Same as IFM2, except exclude conditional branches with static hints and branches that don't depend on CR
   };
#define MMCR0           779
#define MMCR0_FC        0x80000000
#define MMCR0_PMAE      0x04000000

struct TR_PPCHWProfilerPMUConfig
   {
   const pmBaseEventHandlerFunc eventHandler;
   const pmEventHandlerFunc     eventHandlerTable[MAX_PMCS];
#ifdef AIXPPC                   
   const char* const            config[MAX_PMCS];
   int32_t                      group;
#elif defined(LINUXPPC)         
   const uint32_t               config[MAX_PMCS];
#endif /* LINUXPPC */           
   const tr_pm_bhrb_ifm_t       bhrbFilter;
   const uint32_t               bufferSize[MAX_PMCS];
   const uint32_t               bufferElementSize[MAX_PMCS];
   const uint32_t               sampleRates[MAX_PMCS];
   const uint32_t               vmFlags;

   static TR_PPCHWProfilerPMUConfig* getPMUConfigs();
   static void calcMMCR2ForConfig(uint64_t &mmcr2, TR_PPCHWProfilingConfigs c);
   static void calcMMCR2ToDisablePMC(uint64_t &mmcr2, int32_t pmcNumber);
   };
static int32_t baseEventHandler(TR_PPCHWProfilerEBBContext *context);
static int32_t methodHotnessEventHandler(TR_PPCHWProfilerEBBContext *context, int32_t pmu);


static struct TR_PPCHWProfilerPMUConfig configs[NumProfilingConfigs] =
   {
      { // MethodHotness
         baseEventHandler,
         {
            NULL,
            NULL,
            NULL,
            NULL,
            methodHotnessEventHandler,
            NULL
         },
#ifdef AIXPPC
         {
            NULL,
            NULL,
            NULL,
            NULL,
            "PM_RUN_INST_CMPL",
            NULL
         },
         -1,
#elif defined(LINUXPPC)
         {
            0x0,
            0x0,
            0x0,
            0x0,
            0xFA,       // PM_RUN_INST_CMPL
            0x0
         },
#endif /* LINUXPPC */
         BHRB_IFM1,     // Calls
         {0, 0, 0, 0, 512, 0},
         {0, 0, 0, 0, sizeof(TR_PPCMethodHotnessSample), 0},
         {INVALID_SAMPLERATE, INVALID_SAMPLERATE, INVALID_SAMPLERATE, INVALID_SAMPLERATE, 1000000, INVALID_SAMPLERATE},
         0 // XXX: Re-enable when intepreter is fixed //J9_JIT_TOGGLE_RI_ON_TRANSITION
      },
      { // BlockHotness
         NULL,
         {NULL},
#ifdef AIXPPC
         {NULL},
         0,
#elif defined(LINUXPPC)
         {0},
#endif /* LINUXPPC */
         BHRB_IFM0,
         {0},
         {0},
         {0},
         0
      },
      { // FieldHotness
         NULL,
         {NULL},
#ifdef AIXPPC
         {NULL},
         0,
#elif defined(LINUXPPC)
         {0},
#endif /* LINUXPPC */
         BHRB_IFM0,
         {0},
         {0},
         {0},
         0
      }
   };
   static int32_t updatePMC(int32_t pmcNumber, uint32_t pmc[])
   {
   switch (pmcNumber)
      {
      case 1: return tr_pmc_write(PMC1, &pmc[0]);
      case 2: return tr_pmc_write(PMC2, &pmc[1]);
      case 3: return tr_pmc_write(PMC3, &pmc[2]);
      case 4: return tr_pmc_write(PMC4, &pmc[3]);
      case 5: return tr_pmc_write(PMC5, &pmc[4]);
      case 6: return tr_pmc_write(PMC6, &pmc[5]);
      default: return 1;
      }
   }

static int32_t baseEventHandler(TR_PPCHWProfilerEBBContext *context)
   {
   uint32_t pmc[MAX_PMCS];

#if defined(DEBUG) || defined(PPCRI_VERBOSE)
   bool counterNegative = false;
#endif /* DEBUG || PPCRI_VERBOSE */

   if (OMR_UNLIKELY(tr_pmc_read_1to4(&pmc[0]))) goto lostpmu;
   if (OMR_UNLIKELY(tr_pmc_read_5to6(&pmc[4]))) goto lostpmu;

   uint64_t mmcr[3];
   if (OMR_UNLIKELY(tr_mmcr_read(mmcr))) goto lostpmu;

   for (int32_t i = 0; i < MAX_PMCS; i++)
      {
      if ((context->counterBitMask & (1 << i)) &&
          pmc[i] >= 0x80000000)
         {
         if (configs[context->currentConfig].eventHandlerTable[i])
            {
#if defined(DEBUG) || defined(PPCRI_VERBOSE)
            ++context->eventCounts[i];
#endif
            
            pmc[i] = 0x80000000 - context->sampleRates[i];

            if (configs[context->currentConfig].eventHandlerTable[i](context, i)) goto lostpmu;

            if (updatePMC(i+1, pmc)) goto lostpmu;

            // If the buffer is full we want to disable the current counter, so we freeze it through MMCR2
            if (OMR_UNLIKELY(!context->buffers[i].spaceLeft))
               {
               context->counterBitMask &= (0xFF ^ (1 << i));
               TR_PPCHWProfilerPMUConfig::calcMMCR2ToDisablePMC(mmcr[1], i);
               if (OMR_UNLIKELY(tr_mmcr_write(MMCR2, &mmcr[1]))) goto lostpmu;
               }
            }
         else
            {
#if defined(DEBUG) || defined(PPCRI_VERBOSE)
            ++context->eventCounts[i];
            counterNegative = true;
#endif /* DEBUG || PPCRI_VERBOSE */
            pmc[i] = context->sampleRates[i] != INVALID_SAMPLERATE ? 0x80000000 - context->sampleRates[i] : 0;
            if (updatePMC(i+1, pmc)) goto lostpmu;
            }
         }
      }

#if defined(DEBUG) || defined(PPCRI_VERBOSE)
      if (OMR_UNLIKELY(!counterNegative))
         ++context->unknownEventCount;
#endif /* DEBUG || PPCRI_VERBOSE */

   // In order to re-enable EBBs and restart the PMU we need to clear MMCR0[PMAO|FC] and BESCR[PMEO] and set MMCR0[PMAE] and BESCR[GE|PME]
   // however the order is important for a couple of reasons. First, the BESCR bits can't be manipulated until MMCR0[PMAO] is cleared.
   // Second, we have to unfreeze the PMU before returning from the EBB handler, but we don't want instructions that are executed as part of the
   // EBB handler to be counted. To minimize the number of EBB instructions counted we clear MMCR0[FC] as the last step before returning from the
   // assembly EBB handler.

   // First clear MMCR0[PMAO] and set MMCR[PMAE], but leave MMCR[FC] set
   // goto bypasses decl of mmcr0
      {
      const uint64_t mmcr0 = MMCR0_FC | MMCR0_PMAE;
      if (OMR_UNLIKELY(tr_mmcr_write(MMCR0, &mmcr0))) goto lostpmu;
      }

   // Then clear BESCR[PMEO] and set BESCR[PME]
   MTSPR(BESCRR, BESCR_PMEO);
   MTSPR(BESCRSU, BESCRU_PME);

   // MMCR0[FC] will be cleared by the assembly EBB handler
   return 0;

lostpmu:
   // If we've lost access to the PMU while servicing the EBB we can't re-initialize it until we
   // get access back so we set a flag and deal with it later
   context->lostPMU = true;
   return 1;
   }

static int32_t methodHotnessEventHandler(TR_PPCHWProfilerEBBContext *context, int32_t pmu)
   {
   if (OMR_LIKELY(context->buffers[pmu].spaceLeft))
      {
      TR_PPCMethodHotnessSample *buffer = (TR_PPCMethodHotnessSample *)context->buffers[pmu].start;
      TR_PPCMethodHotnessSample *sample = &buffer[configs[context->currentConfig].bufferSize[pmu] - context->buffers[pmu].spaceLeft];
      sample->sia = context->ebbrr;
#ifdef HAVE_BHRBES
      readBHRB(sample->calls);
      CLRBHRB();
#endif /* HAVE_BHRBES */
      --context->buffers[pmu].spaceLeft;
      }

   return 0;
   }
// ASM Handler, see ebb.s
//extern "C" void ebbHandler(void);




#ifdef TR_HOST_64BIT

#define MTSPR64(spr, src)       MTSPR(spr, src)
#define MFSPR64(dst, spr)       MFSPR(dst, spr)

#else /* TR_HOST_32BIT */

// Accessing 64-bit SPRs in 32-bit mode requires special attention.
// This is mostly required for MMCR2, which has bits in the upper 32
// that we want to access.

#ifdef AIXPPC
// Can't use simple ldarx/stdcx on AIX because accessing PMU regs
// can cause an interrupt if AIX has temporarily blocked problem-state
// access to the PMU.
// If stdcx fails once, retry; if it fails again, assume it's because
// we don't have PMU access and an interrupt occured and carry on.
#define MTSPR64(spr, src)                       \
   do                                           \
      {                                         \
      int tries = 2;                            \
      asm("0:                           \n\t"   \
          "ldarx           0, %y1, 1    \n\t"   \
          "mtspr           %2, 0        \n\t"   \
          "stdcx.          0, %y1       \n\t"   \
          "beq+            1f           \n\t"   \
          "cmplwi          %0, 1        \n\t"   \
          "addi            %0, %0, -1   \n\t"   \
          "bgt             0b           \n\t"   \
          "1:                           \n\t"   \
          : "+b" (tries)                        \
          : "Z" (src), "i" (spr)                \
          : "r0", "cr0"                         \
         );                                     \
      }                                         \
   while (0)

#define MFSPR64(dst, spr)                       \
   do                                           \
      {                                         \
      int tries = 2;                            \
      asm("0:                           \n\t"   \
          "ldarx           0, %y1, 1    \n\t"   \
          "mfspr           0, %2        \n\t"   \
          "stdcx.          0, %y1       \n\t"   \
          "beq+            1f           \n\t"   \
          "cmplwi          %0, 1        \n\t"   \
          "addi            %0, %0, -1   \n\t"   \
          "bgt             0b           \n\t"   \
          "1:                           \n\t"   \
          : "+b" (tries)                        \
          : "Z" (dst), "i" (spr)                \
          : "r0", "cr0"                         \
         );                                     \
      }                                         \
   while (0)

#elif defined(LINUXPPC)

#define MTSPR64(spr, src)                       \
   asm("0:                              \n\t"   \
       "ldarx           0, %y0, 1       \n\t"   \
       "mtspr           %1, 0           \n\t"   \
       "stdcx.          0, %y0          \n\t"   \
       "bne-            0b              \n\t"   \
       :                                        \
       : "Z" (src), "i" (spr)                   \
       : "r0", "cr0"                            \
      )
#define MFSPR64(dst, spr)                       \
   asm("0:                              \n\t"   \
       "ldarx           0, %y0, 1       \n\t"   \
       "mfspr           0, %1           \n\t"   \
       "stdcx.          0, %y0          \n\t"   \
       "bne-            0b              \n\t"   \
       :                                        \
       : "Z" (dst), "i" (spr)                   \
       : "r0", "cr0"                            \
      )

#endif /* LINUXPPC */

#endif /* TR_HOST_32BIT */

#ifdef TR_HOST_64BIT
#define laddr "ld"
#define staddr "std"
#else /* TR_HOST_32BIT */
#define laddr "lwz"
#define staddr "stw"
#endif /* TR_HOST_32BIT */

#ifdef AIXPPC
#define MFBHRBE(target, entry)                          \
   asm(".long 0x7C00025C | %0 < 21 | %1 < 11    \n\t"   \
       : "=r" (target)                                  \
       : "i" (entry)                                    \
      )
#elif defined(LINUXPPC)
#define MFBHRBE(target, entry)                          \
   asm(".long 0x7C00025C | %0 << 21 | %1 << 11  \n\t"   \
       : "=r" (target)                                  \
       : "i" (entry)                                    \
      )
#endif /* LINUXPPC */

#define GET_BHRBE(entry, buffer)                        \
   do                                                   \
      {                                                 \
      uintptr_t val;                                    \
      MFBHRBE(val, entry);                              \
      (buffer)[entry] = val;                            \
      if (!val) return;                                 \
      }                                                 \
   while (0)

static inline
__attribute__((always_inline))
void readBHRB(uintptr_t *bhrb)
   {
   GET_BHRBE(0, bhrb);
   GET_BHRBE(1, bhrb);
   GET_BHRBE(2, bhrb);
   GET_BHRBE(3, bhrb);
   GET_BHRBE(4, bhrb);
   GET_BHRBE(5, bhrb);
   GET_BHRBE(6, bhrb);
   GET_BHRBE(7, bhrb);
   GET_BHRBE(8, bhrb);
   GET_BHRBE(9, bhrb);
   GET_BHRBE(10, bhrb);
   GET_BHRBE(11, bhrb);
   GET_BHRBE(12, bhrb);
   GET_BHRBE(13, bhrb);
   GET_BHRBE(14, bhrb);
   GET_BHRBE(15, bhrb);
   GET_BHRBE(16, bhrb);
   GET_BHRBE(17, bhrb);
   GET_BHRBE(18, bhrb);
   GET_BHRBE(19, bhrb);
   GET_BHRBE(20, bhrb);
   GET_BHRBE(21, bhrb);
   GET_BHRBE(22, bhrb);
   GET_BHRBE(23, bhrb);
   GET_BHRBE(24, bhrb);
   GET_BHRBE(25, bhrb);
   GET_BHRBE(26, bhrb);
   GET_BHRBE(27, bhrb);
   GET_BHRBE(28, bhrb);
   GET_BHRBE(29, bhrb);
   GET_BHRBE(30, bhrb);
   GET_BHRBE(31, bhrb);
   }

#define CLRBHRB()         asm(".long 0x7C00035C \n\t")

// AIX provides these functions as millicode routines and we have to use them
// so we provide the same API here for Linux to make it easier to write common code.
// We don't have to use these in Linux-specific code.
// On AIX these will return non-zero if the PMU has been taken away by the OS, in
// addition to when the _write functions are called with invalid SPRs.
// On Linux the PMU will (currently) never be taken away.

#if defined(AIXPPC)

// Defined by later versions of pmapi.h
#pragma mc_func tr_pmc_read_1to4 { "48008493" }
#pragma mc_func tr_pmc_read_5to6 { "480084D3" }
#pragma mc_func tr_mmcr_read { "48008503" }
#pragma mc_func tr_sampled_regs_read { "48008533" }
#pragma mc_func tr_pmc_write { "48008563" }
#pragma mc_func tr_mmcr_write { "480085F3" }

// XXX: However these are defined incorrectly in some versions of pmapi.h
#pragma reg_killed_by tr_pmc_read_1to4 gr3, gr4, lr
#pragma reg_killed_by tr_pmc_read_5to6 gr3, gr4, lr
#pragma reg_killed_by tr_mmcr_read gr3, gr4, lr
#pragma reg_killed_by tr_sampled_regs_read gr3, gr4, lr
#pragma reg_killed_by tr_pmc_write gr3, gr5, cr0, lr
#pragma reg_killed_by tr_mmcr_write gr3, gr5, cr0, lr

#elif defined(LINUXPPC)

static inline
__attribute__((always_inline))
int tr_pmc_read_1to4(uint32_t buf[4])
   {
   MFSPR(buf[0], PMC1);
   MFSPR(buf[1], PMC2);
   MFSPR(buf[2], PMC3);
   MFSPR(buf[3], PMC4);
   return 0;
   }

static inline
__attribute__((always_inline))
int tr_pmc_read_5to6(uint32_t buf[2])
   {
   MFSPR(buf[0], PMC5);
   MFSPR(buf[1], PMC6);
   return 0;
   }

static inline
__attribute__((always_inline))
int tr_mmcr_read(uint64_t buf[3])
   {
   MFSPR64(buf[0], MMCR0);
   MFSPR64(buf[1], MMCR2);
   MFSPR64(buf[2], MMCRA);
   return 0;
   }

static inline
__attribute__((always_inline))
int tr_sampled_regs_read(uint64_t buf[3])
   {
   MFSPR64(buf[0], SIER);
   MFSPR64(buf[1], SIAR);
   MFSPR64(buf[2], SDAR);
   return 0;
   }

static inline
__attribute__((always_inline))
int tr_pmc_write(uint32_t pmc, const uint32_t *value)
   {
   switch (pmc)
      {
      case PMC1:
         MTSPR(PMC1, *value);
         break;
      case PMC2:
         MTSPR(PMC2, *value);
         break;
      case PMC3:
         MTSPR(PMC3, *value);
         break;
      case PMC4:
         MTSPR(PMC4, *value);
         break;
      case PMC5:
         MTSPR(PMC5, *value);
         break;
      case PMC6:
         MTSPR(PMC6, *value);
         break;
      default:
         return -1;
      }
   return 0;
   }

static inline
__attribute__((always_inline))
int tr_mmcr_write(uint32_t mmcr, const uint64_t *value)
   {
   switch (mmcr)
      {
      case MMCR2:
         MTSPR64(MMCR2, *value);
         break;
      case MMCRA:
         MTSPR64(MMCRA, *value);
         break;
      case MMCR0:
         MTSPR64(MMCR0, *value);
         break;
      default:
         return -1;
      }
   return 0;
   }

#endif /* LINUXPPC */

// XXX: Should be added to oti/jitprotos.h
//extern "C" UDATA
//updateJITRuntimeInstrumentationFlags(J9VMThread *targetThread, UDATA newFlags);
void TR_PPCHWProfilerPMUConfig::calcMMCR2ForConfig(uint64_t &mmcr2, TR_PPCHWProfilingConfigs c)
   {
   uint64_t fcnp = 0;
   for (int i = 0; i < MAX_PMCS; ++i)
      {
      if (configs[c].sampleRates[i] == INVALID_SAMPLERATE)
         fcnp |= MMCR2_FCnP << ((5 - i) * 9 + 10);
      }
   mmcr2 = fcnp;
   }

void TR_PPCHWProfilerPMUConfig::calcMMCR2ToDisablePMC(uint64_t &mmcr2, int32_t pmcNumber)
   {
   
   uint64_t fcnp = 0;
   fcnp |= MMCR2_FCnP << ((5 - pmcNumber) * 9 + 10);
   mmcr2 |= fcnp;
   }
TR_PPCHWProfilerPMUConfig* TR_PPCHWProfilerPMUConfig::getPMUConfigs()
   {
   return configs;
   }

#endif /* PPCHWPROFILERPRIVATE_INCL */

