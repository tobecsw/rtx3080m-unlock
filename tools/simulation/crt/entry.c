#include <stdlib.h>
__declspec(dllimport) int __getmainargs(int *,char ***,char ***,int,void *);
int main(int,char **);
void HostEntry(void){int n=0,mode=0;char **args=0,**env=0;__getmainargs(&n,&args,&env,0,&mode);exit(main(n,args));}
