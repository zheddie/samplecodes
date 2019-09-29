#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <grp.h>
#include <errno.h>

main(int argc, char *argv[])
{
  if(1 == argc) {
    printf("Please input a group number\n");
    exit(0);
  }

  int group_number = atoi(argv[1]);

  struct passwd *pd;
  gid_t groups[10];
  int ngroups=0;

  short int lp;
  struct group newgroup;
  struct group * grpptr=&newgroup;
  struct group * tempGrpPtr;
  char grpbuffer[200];
  int  grplinelen = sizeof(grpbuffer);
  memset(grpptr,0,sizeof(struct group));
  printf("zg.groupid=%d\n",group_number);
  
  if ((getgrgid_r(group_number,grpptr,grpbuffer,grplinelen,&tempGrpPtr))!=0) { 
     perror("getgrgid_r() error.");
     printf("errno=%d\n",errno);

   }
  else
  {
     printf("\nThe group name is: %s\n", newgroup.gr_name);
     printf("The gid        is: %u\n", newgroup.gr_gid);
     for (lp = 1; NULL != *(newgroup.gr_mem); lp++, (newgroup.gr_mem)++)
        printf("Group Member %d is: %s\n", lp, *(newgroup.gr_mem));
  }


  struct group *grp;
  setgrent();
  while ((grp = getgrent())) {
     printf("current grp is non null\n");
    if(! grp->gr_mem[0])
      printf("The first group: grp->gr_mem[0] is null\n");
    printf("id %d, member[0] %s\n", grp->gr_gid,grp->gr_mem[0]);   
    for (int i = 0; grp->gr_mem[i]; i++) {
        printf("id %d, member[%d] %s\n", grp->gr_gid,i, grp->gr_mem[i]); 
        
      }
  }
  
  if(!grp)
    printf("There is no extra groups\n");
out:
  endgrent(); 

}