/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* bos71F src/bos/usr/sbin/perf/pmapi/libpmapi/pmapi.h 1.32.1.2           */
/*                                                                        */
/* Licensed Materials - Property of IBM                                   */
/*                                                                        */
/* Restricted Materials of IBM                                            */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 1999,2011              */
/* All Rights Reserved                                                    */
/*                                                                        */
/* US Government Users Restricted Rights - Use, duplication or            */
/* disclosure restricted by GSA ADP Schedule Contract with IBM Corp.      */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
/* @(#)41     1.32.1.2  src/bos/usr/sbin/perf/pmapi/libpmapi/pmapi.h, pmapi, bos71F, 1139A_71F 8/25/11 04:23:17 */
#ifndef _H_PMAPI
#define _H_PMAPI

/*
 *   COMPONENT_NAME:  PMAPI
 *
 *   ORIGINS: 27
 *
 *                    -- (                            when
 *   combined with the aggregated modules for this product)
 *   OBJECT CODE ONLY SOURCE MATERIALS
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1999, 2002.
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/corralid.h>	/* cid_t */

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_COUNTERS		 8  /* maximum number of supported counters */
#define MIN_THRESH_VALUE	 0  /* minimum threshold value */
#define MAX_THRESH_VALUE	63  /* maximum threshold value */

/* Each processor has a different event number for the "count-nothing event". 
 * To shield this from the user the following define is used.
 */
#define COUNT_NOTHING   -1

/*
 * defines for pm_initialize proctype argument
 */
#define	PM_CURRENT		-1
#define	PM_PowerPC604		0
#define	PM_PowerPC604e		1
#define	PM_RS64_II		2
#define	PM_POWER3		3
#define	PM_RS64_III		4
#define	PM_POWER3_II		5
#define	PM_POWER4		6
#define	PM_MPC7450	        7
#define	PM_POWER4_II		8
#define	PM_POWER5		9
#define	PM_PowerPC970		10
#define	PM_POWER5_II		11
#define	PM_POWER6		12
#define	PM_PowerPC970MP		13
#define	PM_POWERCOMPAT		14
#define PM_POWER7           15
#define PM_MAXPROCTYPE      PM_POWER7

/* Defines for event filters */
#define PM_MAX_FILTER	4	/* # filters */
#define PM_VERIFIED     1       /* events that have been verified */
#define PM_UNVERIFIED   2       /* events that have not been verified*/
#define PM_CAVEAT       4       /* events that work with caveats */
#define PM_GET_GROUPS   8       /* want to receive group info */

typedef	uint	ptid_t;

typedef struct {		/* structure for available events */
  int		event_id;	/* event number */
  char		status;		/* verified/unverified/... */
  char		threshold;	/* thresholdable event */
  char		*short_name;	/* mnemonic name */
  char		*long_name;	/* long name */
  char		*description;	/* full description */
} pm_events_t;

typedef union {			/* event status and characteristics */
	int	w;
	struct {
	    unsigned unverified	  :1;
	    unsigned verified	  :1;
	    unsigned caveat	  :1;
	    unsigned broken	  :1;
	    unsigned spare1	  :12;	/* reserved for future usage          */
	    unsigned group_only	  :1;	/* grouped only event                 */
	    unsigned threshold	  :1;	/* thresholdable event                */
	    unsigned shared	  :1;	/* shared event core level            */
	    unsigned support_mode :1;	/* support, user, kernel,hypervisor and
	    				   proctree counting modes applied    */
	    unsigned overflow	  :1;	/* generate an interrupt on overflow  */
	    unsigned marked       :1;   /* marked event                       */
	    unsigned sharedchip   :1;	/* shared event chip level            */
	    unsigned spare2	  :9;	/* reserved for future usage          */
	} b;
} pm_status_t;

typedef struct {		/* structure for available events */
  int		event_id;	/* event number     */
  pm_status_t	status ;	/* event status	    */
  char		*short_name;	/* mnemonic name    */
  char		*long_name;	/* long name        */
  char		*description;	/* full description */
  long		reserved;	/* reserved for future usage */
} pm_events2_t;

typedef struct {                /* structure returned by pminit */
  int           maxpmcs;	/* number of available counters */
  int           *maxevents;     /* number of events for each hw counter */
  pm_events_t   **list_events;  /* list of available events */
  int           thresholdmult;  /* threshold multiplier */
  char		*proc_name;	/* processor name */
  int 		hthresholdmult;	/* upper threshold multiplier */
} pm_info_t;

typedef union {			/* features supported by processor */
	int	w;
	struct {
	    unsigned spare	:27;	/* reserved for future usage */
	    unsigned compatibility	:1;	/* Compatibility mode is enabled */
	    unsigned spurr	:1;	/* SPURR supported */
	    unsigned purr	:1;	/* PURR supported */
	    unsigned runlatch	:1;	/* runlatch mode */
	    unsigned hypervisor	:1;	/* hypervisor counting mode  */
	} b;
} pm_feature_t;

typedef struct {                /* structure returned by pminitialize */
  int           maxpmcs;	/* number of available counters */
  int           *maxevents;     /* number of events for each hw counter */
  pm_events2_t **list_events;   /* list of available events */
  int           thresholdmult;  /* threshold multiplier */
  int 		hthresholdmult;	/* upper threshold multiplier */
  int 		Hthresholdmult;	/* hyper threshold multiplier */
  int 	        reserved;	/* reserved for future usage */
  char		*proc_name;	/* processor name */
  pm_feature_t	proc_feature;	/* processor feature list */
  int 	        reserved2;	/* reserved for future usage */
} pm_info2_t;

typedef struct {		/* structure for available events */
  int		group_id;	/* group number */
  int 	        *events;	/* events in this group, by ID # */
  char		*short_name;	/* mnemonic name */
  char		*long_name;	/* long name */
  char		*description;	/* full description */
} pm_groups_t;

typedef struct {
  int            maxgroups;      /* number of available groups */
  pm_groups_t    *event_groups;  /* list of event groups */
} pm_groups_info_t;

typedef union {			 /* defines mode of operations */
  int	w;
  struct {	  
    unsigned threshold   :6;	/* threshold value */
    unsigned thresh_res  :1;	/* use upper threshold multiplier if set */
    unsigned runlatch    :1;	/* runlatch enable/disable */
    unsigned hypervisor  :1;	/* turns hypervisor counting on */
    unsigned thresh_hres :1;	/* use hyper threshold multiplier if set */
    unsigned nointerrupt :1;	/* interruption counting disabled */
    unsigned wpar_all    :1;	/* collect per-WPAR data */
    unsigned spare       :14;
    unsigned is_group    :1;	/* is an event group */
    unsigned process     :1;	/* process level group indicator */
    unsigned kernel      :1;	/* turns kernel mode counting on */
    unsigned user        :1;	/* turns user mode counting on */
    unsigned count       :1;	/* counting state */
    unsigned proctree    :1;	/* turns process tree counting on */
  } b;
} pm_mode_t;

typedef struct {			/* structure for PM programing */
  pm_mode_t	mode;			/* mode of operation */
  int		events[MAX_COUNTERS];	/* list of counted events */
  long		reserved;		/* reserved for future use */
} pm_prog_t;

typedef struct {
  int	members;	 		/* number of threads in group */
  union {				/* group characteristics */
    int	w;
    struct {			
      unsigned spare      :29;
      unsigned direct     :1; /* hardware counters read in direct mode */
      unsigned process    :1; /* process level group indicator */
      unsigned consistent :1; /* group data consistent with members */
    } b;
  } flags;
} pm_ginfo_t;

typedef struct {			/* structure for PM data */
  pm_ginfo_t	ginfo;			/* group information */
  long long	accu[MAX_COUNTERS];	/* accumulated data */
  long		reserved;		/* reserved for future use */
} pm_data_t;

typedef struct {
  timebasestruct_t	accu_timebase;	/* accumulated time base  */
  timebasestruct_t	accu_purr;	/* accumulated PURR time  */
  timebasestruct_t	accu_spurr;	/* accumulated SPURR time */
} pm_accu_time_t;

/* counters multiplexing support */
typedef int		pm_events_prog_t[MAX_COUNTERS];
typedef unsigned int	pm_mmcrs_prog_t[MAX_COUNTERS];

typedef struct {			/* structure for PM programing       */
  pm_mode_t		mode;		/* mode of operation                 */
  int			slice_duration;	/* duration of each time slice in ms */
  int			nb_events_prog;	/* number of events_set              */
  pm_events_prog_t	*events_set;	/* list of counted events            */
} pm_prog_mx_t;

typedef struct {
  timebasestruct_t	accu_time;	/* accumulated time      */
  timebasestruct_t	accu_purr;	/* accumulated PURR time */
  timebasestruct_t	accu_spurr;	/* accumulated SPURR time */
  long long	accu_data[MAX_COUNTERS];/* accumulated data      */
} pm_accu_mx_t;

typedef struct {			/* structure for PM data */
  pm_ginfo_t	ginfo;			/* group information     */
  int		nb_accu_mx;		/* number of accu_set    */
  int		nb_mx_round;		/* number of loops on all the event
					   sets */
  pm_accu_mx_t	*accu_set;		/* accumulated data      */
} pm_data_mx_t; 

/* multi-mode programmation support */
typedef struct {		/* structure for PM programing       */
  int		slice_duration;	/* duration of each time slice in ms */
  int		nb_set_prog;	/* number of events_set              */
  pm_prog_t	*prog_set;	/* list of counted events            */
} pm_prog_mm_t;

/* unique WPAR opaque identifier */
typedef unsigned long long pm_wp_handle_t;
#define PM_INVALID_WP_HANDLE	((pm_wp_handle_t)-1)

typedef struct {
  cid_t			cid;			/* corral identifier */
  char			name[MAXWPARNAMELEN + 1];	/* WPAR name */
  pm_wp_handle_t	hwpar;			/* WPAR opaque identifier */
} pm_wpar_ctx_info_t;

/* function prototypes */
extern double	pm_cycles(void);

extern void	pm_error(char *where, int error);
extern char	*pm_strerror(char *where, int error);

extern int	pm_init(int filter, pm_info_t *pminfo,
			pm_groups_info_t *pmgroups);
extern int	pm_initialize(int filter, pm_info2_t *pminfo,
			pm_groups_info_t *pmgroups, int proctype);

extern int	pm_set_program(pm_prog_t *prog);
extern int	pm_set_program_mx(pm_prog_mx_t *prog);
extern int	pm_set_program_mm(pm_prog_mm_t *prog);
extern int	pm_set_program_wp(cid_t, pm_prog_t *);
extern int	pm_set_program_wp_mm(cid_t, pm_prog_mm_t *);
extern int	pm_get_program(pm_prog_t *prog);
extern int	pm_get_program_mx(pm_prog_mx_t *prog);
extern int	pm_get_program_mm(pm_prog_mm_t *prog);
extern int	pm_get_program_wp(cid_t, pm_prog_t *);
extern int	pm_get_program_wp_mm(cid_t, pm_prog_mm_t *);
extern int	pm_start(void);
extern int	pm_tstart(timebasestruct_t *time);
extern int	pm_start_wp(cid_t);
extern int	pm_tstart_wp(cid_t, timebasestruct_t *);
extern int	pm_stop(void);
extern int	pm_tstop(timebasestruct_t *time);
extern int	pm_stop_wp(cid_t);
extern int	pm_tstop_wp(cid_t, timebasestruct_t *);
extern int	pm_reset_data(void);
extern int	pm_reset_data_wp(cid_t);
extern int	pm_get_data(pm_data_t *data);
extern int	pm_get_tdata(pm_data_t *data, timebasestruct_t *time);
extern int	pm_get_Tdata(pm_data_t *data, pm_accu_time_t *times);
extern int	pm_get_data_mx(pm_data_mx_t *data);
extern int	pm_get_tdata_mx(pm_data_mx_t *data, timebasestruct_t *time);
extern int	pm_get_data_lcpu(int lcpuid, pm_data_t *data);
extern int	pm_get_data_cpu(int cpuid, pm_data_t *data);
extern int	pm_get_tdata_lcpu(int lcpuid, pm_data_t *data,
			timebasestruct_t *time);
extern int	pm_get_tdata_cpu(int cpuid, pm_data_t *data,
			timebasestruct_t *time);
extern int	pm_get_Tdata_lcpu(int lcpuid, pm_data_t *data,
			pm_accu_time_t *time);
extern int	pm_get_Tdata_cpu(int cpuid, pm_data_t *data,
			pm_accu_time_t *time);
extern int	pm_get_data_lcpu_mx(int lcpuid, pm_data_mx_t *data);
extern int	pm_get_data_cpu_mx(int cpuid, pm_data_mx_t *data);
extern int	pm_get_tdata_lcpu_mx(int lcpuid, pm_data_mx_t *data,
			timebasestruct_t *time);
extern int	pm_get_tdata_cpu_mx(int cpuid, pm_data_mx_t *data,
			timebasestruct_t *time);
extern int	pm_get_data_wp(pm_wp_handle_t, pm_data_t *);
extern int	pm_get_tdata_wp(pm_wp_handle_t, pm_data_t *,
			timebasestruct_t *);
extern int	pm_get_Tdata_wp(pm_wp_handle_t, pm_data_t *, pm_accu_time_t *);
extern int	pm_get_data_wp_mx(pm_wp_handle_t, pm_data_mx_t *);
extern int	pm_get_tdata_wp_mx(pm_wp_handle_t, pm_data_mx_t *,
			timebasestruct_t *);
extern int	pm_get_data_lcpu_wp(pm_wp_handle_t, int, pm_data_t *);
extern int	pm_get_tdata_lcpu_wp(pm_wp_handle_t, int, pm_data_t *,
			timebasestruct_t *);
extern int	pm_get_Tdata_lcpu_wp(pm_wp_handle_t, int, pm_data_t *,
			pm_accu_time_t *);
extern int	pm_get_data_lcpu_wp_mx(pm_wp_handle_t, int, pm_data_mx_t *);
extern int	pm_get_tdata_lcpu_wp_mx(pm_wp_handle_t, int, pm_data_mx_t *,
			timebasestruct_t *);
extern int	pm_delete_program(void);
extern int	pm_delete_program_wp(cid_t);
extern int	pm_get_wplist(const char *, pm_wpar_ctx_info_t *, int *);

extern int	pm_set_program_mythread(pm_prog_t *prog);
extern int	pm_set_program_mythread_mx(pm_prog_mx_t *prog);
extern int	pm_get_program_mythread(pm_prog_t *prog);
extern int	pm_get_program_mythread_mx(pm_prog_mx_t *prog);
extern int	pm_tstart_mythread(timebasestruct_t *time);
extern int	pm_start_mythread(void);
extern int	pm_tstop_mythread(timebasestruct_t *time);
extern int	pm_stop_mythread(void);
extern int	pm_reset_data_mythread(void);
extern int	pm_get_data_mythread(pm_data_t *data);
extern int	pm_get_tdata_mythread(pm_data_t *data, timebasestruct_t *time);
extern int	pm_get_Tdata_mythread(pm_data_t *data, pm_accu_time_t *time);
extern int	pm_get_data_mythread_mx(pm_data_mx_t *data);
extern int	pm_get_tdata_mythread_mx(pm_data_mx_t *data,
			timebasestruct_t *time);
extern int	pm_delete_program_mythread(void);

extern int	pm_set_program_thread(pid_t pid, tid_t tid, pm_prog_t *prog);
extern int	pm_set_program_thread_mx(pid_t pid, tid_t tid,
							pm_prog_mx_t *prog);
extern int	pm_set_program_thread_mm(pid_t pid, tid_t tid,
							pm_prog_mm_t *prog);
extern int	pm_get_program_thread(pid_t pid, tid_t tid, pm_prog_t *prog);
extern int	pm_get_program_thread_mx(pid_t pid, tid_t tid,
							pm_prog_mx_t *prog);
extern int	pm_get_program_thread_mm(pid_t pid, tid_t tid,
							pm_prog_mm_t *prog);
extern int	pm_start_thread(pid_t pid, tid_t tid);
extern int	pm_tstart_thread(pid_t pid, tid_t tid, timebasestruct_t *time);
extern int	pm_stop_thread(pid_t pid, tid_t tid);
extern int	pm_tstop_thread(pid_t pid, tid_t tid, timebasestruct_t *time);
extern int	pm_reset_data_thread(pid_t pid, tid_t tid);
extern int	pm_get_data_thread(pid_t pid, tid_t tid, pm_data_t *data);
extern int	pm_get_tdata_thread(pid_t pid, tid_t tid, pm_data_t *data,
			timebasestruct_t *time);
extern int	pm_get_Tdata_thread(pid_t pid, tid_t tid, pm_data_t *data,
			pm_accu_time_t *time);
extern int	pm_get_data_thread_mx(pid_t pid, tid_t tid,
							pm_data_mx_t *data);
extern int	pm_get_tdata_thread_mx(pid_t pid, tid_t tid,
			pm_data_mx_t *data, timebasestruct_t *time);
extern int	pm_delete_program_thread(pid_t pid, tid_t tid);

extern int	pm_set_program_pthread(pid_t pid, tid_t tid, ptid_t ptid,
							 pm_prog_t *prog);
extern int	pm_set_program_pthread_mx(pid_t pid, tid_t tid, ptid_t ptid,
							 pm_prog_mx_t *prog);
extern int	pm_set_program_pthread_mm(pid_t pid, tid_t tid, ptid_t ptid,
							 pm_prog_mm_t *prog);
extern int	pm_get_program_pthread(pid_t pid, tid_t tid, ptid_t ptid,
							 pm_prog_t *prog);
extern int	pm_get_program_pthread_mx(pid_t pid, tid_t tid, ptid_t ptid,
							 pm_prog_mx_t *prog);
extern int	pm_get_program_pthread_mm(pid_t pid, tid_t tid, ptid_t ptid,
							 pm_prog_mm_t *prog);
extern int	pm_start_pthread(pid_t pid, tid_t tid, ptid_t ptid);
extern int	pm_tstart_pthread(pid_t pid, tid_t tid, ptid_t ptid,
			timebasestruct_t *time);
extern int	pm_stop_pthread(pid_t pid, tid_t tid, ptid_t ptid);
extern int	pm_tstop_pthread(pid_t pid, tid_t tid, ptid_t ptid,
			timebasestruct_t *time);
extern int	pm_reset_data_pthread(pid_t pid, tid_t tid, ptid_t ptid);
extern int	pm_get_data_pthread(pid_t pid, tid_t tid, ptid_t ptid,
							 pm_data_t *data);
extern int	pm_get_tdata_pthread(pid_t pid, tid_t tid, ptid_t ptid,
				 pm_data_t *data, timebasestruct_t *time);
extern int	pm_get_Tdata_pthread(pid_t pid, tid_t tid, ptid_t ptid,
				 pm_data_t *data, pm_accu_time_t *time);
extern int	pm_get_data_pthread_mx(pid_t pid, tid_t tid, ptid_t ptid,
							 pm_data_mx_t *data);
extern int	pm_get_tdata_pthread_mx(pid_t pid, tid_t tid, ptid_t ptid,
				 pm_data_mx_t *data, timebasestruct_t *time);
extern int	pm_delete_program_pthread(pid_t pid, tid_t tid, ptid_t ptid);

extern int	pm_set_program_mygroup(pm_prog_t *prog);
extern int	pm_set_program_mygroup_mx(pm_prog_mx_t *prog);
extern int	pm_set_program_mygroup_mm(pm_prog_mm_t *prog);
extern int	pm_get_program_mygroup(pm_prog_t *prog);
extern int	pm_get_program_mygroup_mx(pm_prog_mx_t *prog);
extern int	pm_get_program_mygroup_mm(pm_prog_mm_t *prog);
extern int	pm_start_mygroup(void);
extern int	pm_tstart_mygroup(timebasestruct_t *time);
extern int	pm_stop_mygroup(void);
extern int	pm_tstop_mygroup(timebasestruct_t *time);
extern int	pm_reset_data_mygroup(void);
extern int	pm_get_data_mygroup(pm_data_t *data);
extern int	pm_get_tdata_mygroup(pm_data_t *data, timebasestruct_t *time);
extern int	pm_get_Tdata_mygroup(pm_data_t *data, pm_accu_time_t *time);
extern int	pm_get_data_mygroup_mx(pm_data_mx_t *data);
extern int	pm_get_tdata_mygroup_mx(pm_data_mx_t *data,
			timebasestruct_t *time);
extern int	pm_delete_program_mygroup(void);

extern int	pm_set_program_group(pid_t pid, tid_t tid, pm_prog_t *prog);
extern int	pm_set_program_group_mx(pid_t pid, tid_t tid,
							pm_prog_mx_t *prog);
extern int	pm_set_program_group_mm(pid_t pid, tid_t tid,
							pm_prog_mm_t *prog);
extern int	pm_get_program_group(pid_t pid, tid_t tid, pm_prog_t *prog);
extern int	pm_get_program_group_mx(pid_t pid, tid_t tid,
							pm_prog_mx_t *prog);
extern int	pm_get_program_group_mm(pid_t pid, tid_t tid,
							pm_prog_mm_t *prog);
extern int	pm_start_group(pid_t pid, tid_t tid);
extern int	pm_tstart_group(pid_t pid, tid_t tid, timebasestruct_t *time);
extern int	pm_stop_group(pid_t pid, tid_t tid);
extern int	pm_tstop_group(pid_t pid, tid_t tid, timebasestruct_t *time);
extern int	pm_reset_data_group(pid_t pid, tid_t tid);
extern int	pm_get_data_group(pid_t pid, tid_t tid, pm_data_t *data);
extern int	pm_get_tdata_group(pid_t pid, tid_t tid, pm_data_t *data,
			timebasestruct_t *time);
extern int	pm_get_Tdata_group(pid_t pid, tid_t tid, pm_data_t *data,
			pm_accu_time_t *time);
extern int	pm_get_data_group_mx(pid_t pid, tid_t tid, pm_data_mx_t *data);
extern int	pm_get_tdata_group_mx(pid_t pid, tid_t tid, pm_data_mx_t *data,
			timebasestruct_t *time);
extern int	pm_delete_program_group(pid_t pid, tid_t tid);

extern int	pm_set_program_pgroup(pid_t pid, tid_t tid, ptid_t ptid,
							pm_prog_t *prog);
extern int	pm_set_program_pgroup_mx(pid_t pid, tid_t tid, ptid_t ptid,
							pm_prog_mx_t *prog);
extern int	pm_set_program_pgroup_mm(pid_t pid, tid_t tid, ptid_t ptid,
							pm_prog_mm_t *prog);
extern int	pm_get_program_pgroup(pid_t pid, tid_t tid, ptid_t ptid,
							pm_prog_t *prog);
extern int	pm_get_program_pgroup_mx(pid_t pid, tid_t tid, ptid_t ptid,
							pm_prog_mx_t *prog);
extern int	pm_get_program_pgroup_mm(pid_t pid, tid_t tid, ptid_t ptid,
							pm_prog_mm_t *prog);
extern int	pm_start_pgroup(pid_t pid, tid_t tid, ptid_t ptid);
extern int	pm_tstart_pgroup(pid_t pid, tid_t tid, ptid_t ptid,
			timebasestruct_t *time);
extern int	pm_stop_pgroup(pid_t pid, tid_t tid, ptid_t ptid);
extern int	pm_tstop_pgroup(pid_t pid, tid_t tid, ptid_t ptid,
			timebasestruct_t *time);
extern int	pm_reset_data_pgroup(pid_t pid, tid_t tid, ptid_t ptid);
extern int	pm_get_data_pgroup(pid_t pid, tid_t tid, ptid_t ptid,
							pm_data_t *data);
extern int	pm_get_tdata_pgroup(pid_t pid, tid_t tid, ptid_t ptid,
				pm_data_t *data, timebasestruct_t *time);
extern int	pm_get_Tdata_pgroup(pid_t pid, tid_t tid, ptid_t ptid,
				pm_data_t *data, pm_accu_time_t *time);
extern int	pm_get_data_pgroup_mx(pid_t pid, tid_t tid, ptid_t ptid,
							pm_data_mx_t *data);
extern int	pm_get_tdata_pgroup_mx(pid_t pid, tid_t tid, ptid_t ptid,
				pm_data_mx_t *data, timebasestruct_t *time);
extern int	pm_delete_program_pgroup(pid_t pid, tid_t tid, ptid_t ptid);

#ifdef __cplusplus
}
#endif

#endif
