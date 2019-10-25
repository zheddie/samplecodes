# include <pmapi.h>
main()
{
       pm_info2_t        pminfo;
       pm_prog_t        prog; 
       pm_groups_info_t pmginfo; 
       int i=0;
       int filter = PM_VERIFIED;  /* get list of verified events */
 
       pm_initialize(filter, &pminfo, &pmginfo, PM_CURRENT );
 
       prog.mode.w           = 0;  /* start with clean mode */
       prog.mode.b.user      = 1;  /* count only user mode */ 
       prog.mode.b.is_group  = 1;  /* specify event group */
 
       for (i = 0; i < pminfo.maxpmcs; i++)
                prog.events[i] = COUNT_NOTHING;
 
       prog.events[0]    = 1;  /* count events in group 1 */ 
	printf("zg.exiting\n");
} 
