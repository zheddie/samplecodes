# include <pmapi.h>
main()
{
       pm_info2_t pminfo;
       pm_prog_t prog;
       pm_data_t data;
       pm_groups_info_t pmginfo; 

       int filter = PM_VERIFIED; /* use only verified events */

       pm_initialize(filter, &pminfo, &pmginfo, PM_CURRENT );
       //pm_initialize(filter, &pminfo, &pmginfo, PM_CURRENT );
 
       prog.mode.w       = 0;  /* start with clean mode */
       prog.mode.b.user  = 1;  /* count only user mode */
       prog.mode.b.is_group  = 1;  /* specify event group */

       for (int i = 0; i < pminfo.maxpmcs; i++)
                prog.events[i] = COUNT_NOTHING;

       prog.events[0]    = 1;  /* count event 1 in first counter */
       prog.events[1]    = 2;  /* count event 2 in second counter */

       pm_set_program_mythread(&prog);
       pm_start_mythread();

//(1)    ... usefull work ....
	printf("zg.in mythread()\n");

       pm_stop_mythread();
       pm_get_data_mythread(&data);

       //... print results ...
	printf("zg. exiting...\n");
}
