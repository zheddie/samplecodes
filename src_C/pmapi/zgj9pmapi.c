# include <pmapi.h>

main()
{
       pm_info2_t pminfo;
       pm_prog_t prog;
       pm_data_t data;
       pm_groups_info_t pmginfo; 

       //int filter = PM_VERIFIED; /* use only verified events */
       int filter = PM_VERIFIED | PM_UNVERIFIED | PM_CAVEAT |PM_GET_GROUPS;
       pm_initialize(filter, &pminfo, &pmginfo, PM_CURRENT );
   
   memset(&prog, 0, sizeof(pm_prog_t));
   prog.mode.b.user = 1;
   prog.mode.b.is_group = 1;
   prog.mode.b.runlatch = 1;
   prog.mode.b.nointerrupt = 1;
   prog.mode.b.no_inherit = 1;
   //prog.events[0] = configs[currentConfig].group;
      prog.events[0]    = 1;  /* count event 1 in first counter */

       // for (int i = 0; i < pminfo.maxpmcs; i++)
       //          prog.events[i] = COUNT_NOTHING;

       pm_set_program_mythread(&prog);
       pm_start_mythread();

//(1)    ... usefull work ....
	printf("zg.in mythread()\n");

       pm_stop_mythread();
       pm_get_data_mythread(&data);

       //... print results ...
	printf("zg. exiting...\n");
}
