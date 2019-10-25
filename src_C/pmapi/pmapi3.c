#include <stdio.h>
#include <stdlib.h>
#include <pmapi.h>

int main()
{
	int rc = 0;
	pm_info2_t events;
	pm_groups_info_t groups;
	
	pm_events2_t *ev_ptr = NULL;
	
	int filter = 0;
	/*
	* Get the events and groups supported for POWER4.
  * To get the events and groups supported for the current processor,
  * use PM_CURRENT.
	*/
	//int processor_type = PM_POWER4;
	int processor_type = PM_CURRENT;

	int group_idx = 0;
	int counter_idx = 0;
	int ev_count = 0;
	int event_found = 0;
	
	/*
	 * PM_VERIFIED 	  - To get list of verified events
	 * PM_UNVERIFIED	- To get list of unverified events
	 * PM_CAVEAT		  - To get list of events that are usable but with caveats
	 */
	filter |= PM_VERIFIED | PM_UNVERIFIED | PM_CAVEAT;	  
	
	/* Get list of events-groups */
	filter |= PM_GET_GROUPS;  
	
	 if ((rc = pm_initialize(filter, &events, &groups, processor_type)) != 0)
	{
 		pm_error("pm_initialize", rc);
 		exit(-1);
 	}
for(group_idx = 0; group_idx < groups.maxgroups; group_idx++)
	{
		printf("================================\n");
		printf("Group ID: %d.\n", groups.event_groups[group_idx].group_id);
		printf("Group Name: %s.\n", groups.event_groups[group_idx].short_name);
		printf("Group Long Name: %s.\n", groups.event_groups[group_idx].long_name);
		printf("Group Description: %s.\n", groups.event_groups[group_idx].description);
		printf("Events in this Group: \n");
		for(counter_idx = 0; counter_idx < events.maxpmcs; counter_idx++)
		{
			event_found = 0;
			for(ev_ptr = events.list_events[counter_idx], ev_count = 0;
				ev_count < events.maxevents[counter_idx]; 
				ev_ptr++, ev_count++)
			{
				/* If the event ID in "groups" matches with event ID supported
				 * in the counter */
				if(groups.event_groups[group_idx].events[counter_idx] == ev_count)
				{
					printf("\tCounter ID: %d.\n", counter_idx+1);
					printf("\tEvent ID: %d.\n", ev_count);
					printf("\tEvent Name: %s.\n", ev_ptr->short_name);
					event_found = 1;
					break;
				}
        /* We have found the event for this counter. Move on to
				 * next counter. */
				if(event_found) break;
			}
		}
		printf("\n");
	}
	return 0;
}
