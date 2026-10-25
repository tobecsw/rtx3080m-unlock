#define main CoreFixtureMain
#include "test_ga104_simulation.c"
#undef main
#include "../../gop/patch.h"
#include "../../gop/full.h"
#include <IndustryStandard/Acpi.h>
#include <Protocol/DevicePath.h>
extern GU_RUNTIME_PROFILE GuProfile;
static EFI_SYSTEM_TABLE SystemTable;
static GC_CONTEXT Context;
static EFI_PCI_IO_PROTOCOL Amd;
static EFI_GRAPHICS_OUTPUT_PROTOCOL Gop;
static EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE GopMode;
static EFI_GRAPHICS_OUTPUT_MODE_INFORMATION GopInfo;
static EFI_DEVICE_PATH_PROTOCOL GopPath;
static BOOLEAN Claimed;
static int Failure;
static EFI_GUID PciId=EFI_PCI_IO_PROTOCOL_GUID,GopId=EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID,PathId=EFI_DEVICE_PATH_PROTOCOL_GUID;
#define TARGET ((EFI_HANDLE)(UINTN)1)
#define AMD ((EFI_HANDLE)(UINTN)2)
#define GOP ((EFI_HANDLE)(UINTN)3)
#define IMAGE ((EFI_HANDLE)(UINTN)4)
static BOOLEAN Log(const char *f,...){return Failure!=3;}
BOOLEAN GuLogReady(VOID){return Failure!=3;}
VOID GuIoSetQuiet(BOOLEAN q){}
EFI_STATUS GuIoOpenRun(EFI_HANDLE i,EFI_SYSTEM_TABLE *t,UINTN n){return EFI_SUCCESS;}
EFI_STATUS GuIoOpen(EFI_HANDLE i,EFI_SYSTEM_TABLE *t){return EFI_SUCCESS;}
EFI_STATUS GuIoClose(VOID){return EFI_SUCCESS;}
EFI_STATUS GuLoadAssets(VOID){return EFI_SUCCESS;}
VOID GuReleaseAssets(VOID){}
VOID GuConsole(const char *s){}
EFI_STATUS GvClockInit(GC_CONTEXT *c){return EFI_SUCCESS;}
BOOLEAN GvMark(GC_CONTEXT *c,const char *s){return TRUE;}
static EFI_TPL EFIAPI Raise(EFI_TPL t){return TPL_APPLICATION;}
static VOID EFIAPI Restore(EFI_TPL t){}
static EFI_STATUS EFIAPI Location(EFI_PCI_IO_PROTOCOL *p,UINTN *s,UINTN *b,UINTN *d,UINTN *f){*s=*d=*f=0;*b=p==&pci?1:8;return EFI_SUCCESS;}
static EFI_STATUS EFIAPI Bar(EFI_PCI_IO_PROTOCOL *p,UINT8 i,UINT64 *supports,VOID **out){
 EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *r=NULL;EFI_STATUS s=allocate_pool(EfiLoaderData,sizeof(*r)+2,(VOID**)&r);if(EFI_ERROR(s))return s;
 r->Desc=ACPI_ADDRESS_SPACE_DESCRIPTOR;r->Len=sizeof(*r)-3;r->ResType=0;r->AddrSpaceGranularity=32;r->AddrRangeMin=0xfb000000;r->AddrRangeMax=0xfbffffff;r->AddrLen=0x1000000;((UINT8*)(r+1))[0]=ACPI_END_TAG_DESCRIPTOR;*out=r;return EFI_SUCCESS;
}
static EFI_STATUS EFIAPI ReadConfigMany(EFI_PCI_IO_PROTOCOL *p,EFI_PCI_IO_PROTOCOL_WIDTH w,UINT32 off,UINTN n,VOID *v){
 if(p==&Amd){CHECK(n==3);UINT32 *a=v;a[0]=0x16381002;a[1]=3;a[2]=0x03000000;return EFI_SUCCESS;}
 for(UINTN i=0;i<n;i++){EFI_STATUS s=read_config(p,w,off+(UINT32)(i<<w),1,(UINT8*)v+(i<<w));if(EFI_ERROR(s))return s;}return EFI_SUCCESS;
}
static EFI_STATUS EFIAPI Locate(EFI_LOCATE_SEARCH_TYPE type,EFI_GUID *g,VOID *key,UINTN *n,EFI_HANDLE **h){
 CHECK(!memcmp(g,&GopId,sizeof(*g)));allocate_pool(EfiLoaderData,sizeof(EFI_HANDLE),(VOID**)h);(*h)[0]=GOP;*n=1;return EFI_SUCCESS;
}
static EFI_STATUS EFIAPI Handle(EFI_HANDLE h,EFI_GUID *g,VOID **p){
 if(h==GOP && !memcmp(g,&GopId,sizeof(*g))){*p=&Gop;return EFI_SUCCESS;}
 if(h==GOP && !memcmp(g,&PathId,sizeof(*g))){*p=&GopPath;return EFI_SUCCESS;}
 if(h==AMD && !memcmp(g,&PciId,sizeof(*g))){*p=&Amd;return EFI_SUCCESS;}
 if(h==TARGET && !memcmp(g,&PciId,sizeof(*g))){*p=&pci;return EFI_SUCCESS;}
 *p=NULL;return EFI_UNSUPPORTED;
}
static EFI_STATUS EFIAPI Resolve(EFI_GUID *g,EFI_DEVICE_PATH_PROTOCOL **p,EFI_HANDLE *h){*h=(Failure==1 || (Failure==6 && fake.sec2_starts))?TARGET:AMD;return EFI_SUCCESS;}
static EFI_STATUS EFIAPI Protocols(EFI_HANDLE h,EFI_GUID ***p,UINTN *n){
 CHECK(h==TARGET);allocate_pool(EfiLoaderData,sizeof(EFI_GUID*),(VOID**)p);(*p)[0]=&PciId;*n=1;return EFI_SUCCESS;
}
static EFI_STATUS EFIAPI Opens(EFI_HANDLE h,EFI_GUID *g,EFI_OPEN_PROTOCOL_INFORMATION_ENTRY **e,UINTN *n){
 *e=NULL;*n=0;if(!Claimed && Failure!=2)return EFI_SUCCESS;
 allocate_pool(EfiLoaderData,sizeof(**e),(VOID**)e);*n=1;
 (*e)->Attributes=Claimed?EFI_OPEN_PROTOCOL_EXCLUSIVE:EFI_OPEN_PROTOCOL_BY_DRIVER;
 (*e)->AgentHandle=(Claimed && !(Failure==7 && fake.sec2_starts))?IMAGE:(EFI_HANDLE)(UINTN)9;(*e)->ControllerHandle=NULL;(*e)->OpenCount=1;return EFI_SUCCESS;
}
static EFI_STATUS EFIAPI Open(EFI_HANDLE h,EFI_GUID *g,VOID **p,EFI_HANDLE image,EFI_HANDLE controller,UINT32 attr){
 CHECK(h==TARGET && image==IMAGE && !controller && attr==EFI_OPEN_PROTOCOL_EXCLUSIVE && !Claimed);
 if(Failure==4)return EFI_ACCESS_DENIED;Claimed=TRUE;*p=&pci;return EFI_SUCCESS;
}
static EFI_STATUS EFIAPI Close(EFI_HANDLE h,EFI_GUID *g,EFI_HANDLE image,EFI_HANDLE controller){
 CHECK(Claimed);if(Failure==5)return EFI_DEVICE_ERROR;Claimed=FALSE;return EFI_SUCCESS;
}
static VOID setup_app(enum scenario m,int failure){
 setup(m);Failure=failure;Claimed=FALSE;memset(&SystemTable,0,sizeof(SystemTable));SystemTable.BootServices=&bs;
 SystemTable.ConsoleOutHandle=(EFI_HANDLE)(UINTN)10;
 bs.RaiseTPL=Raise;bs.RestoreTPL=Restore;bs.LocateHandleBuffer=Locate;bs.HandleProtocol=Handle;bs.LocateDevicePath=Resolve;
 bs.ProtocolsPerHandle=Protocols;bs.OpenProtocolInformation=Opens;bs.OpenProtocol=Open;bs.CloseProtocol=Close;
 pci.GetLocation=Location;pci.GetBarAttributes=Bar;pci.Pci.Read=ReadConfigMany;
 memset(&Amd,0,sizeof(Amd));Amd.GetLocation=Location;Amd.Pci.Read=ReadConfigMany;
 memset(&GopInfo,0,sizeof(GopInfo));GopInfo.HorizontalResolution=1920;GopInfo.VerticalResolution=1080;GopInfo.PixelsPerScanLine=1920;GopInfo.PixelFormat=PixelBlueGreenRedReserved8BitPerColor;
 memset(&GopMode,0,sizeof(GopMode));GopMode.Info=&GopInfo;GopMode.SizeOfInfo=sizeof(GopInfo);GopMode.FrameBufferBase=0xfc10000000ULL;GopMode.FrameBufferSize=0x7e9000;Gop.Mode=&GopMode;
 memset(&Context,0,sizeof(Context));Context.table=&SystemTable;Context.image=IMAGE;Context.target=TARGET;Context.pci=&pci;Context.identity=0x249c10de;Context.subsystem=0x16021043;Context.class_revision=0x030000a1;Context.log=Log;
}
int main(int argc,char **argv){
 CHECK(argc==4);size_t bytes;UINT8 *profile=read_file(argv[3],&bytes);CHECK(GuProfileDecode(profile,bytes,&GuProfile));free(profile);
 load_fixture_assets(argv[1],argv[2]);
 int passed=0;
 for(int m=0;m<SCENARIO_COUNT;m++){
  if(m==DISPLAY_CHANGED)continue; /* Old fixture's callback is replaced by real independent checks. */
  setup_app((enum scenario)m,0);EFI_STATUS s=GxPrepare(&Context);if(EFI_ERROR(s)){fprintf(stderr,"preflight %d failed %#llx\n%s",m,(unsigned long long)s,fake.log);return 1;}
  GF_PLAN plan;CHECK(GfPrepare(&Context,&plan)==EFI_SUCCESS);GC_RESULT result={0};s=GfRun(&Context,&plan,&result);
  BOOLEAN ok=m==SUCCESS || m==FWSEC_RUNNING || m==PIO_UNAVAILABLE;
  if(ok){if(EFI_ERROR(s))fprintf(stderr,"app transaction %d failed: %#llx\n%s",m,(unsigned long long)s,fake.log);CHECK(s==EFI_SUCCESS && result.restored && !Claimed && GxSafe());CHECK(GxFinish()==EFI_SUCCESS);CHECK(!fake.pool_live);}
  else {CHECK(EFI_ERROR(s) && !result.restored);}
  CHECK(fake.flrs<=1);CHECK(!(config[4]&4));passed++;printf("APP chain mode=%s accepted=%u status=%#llx PASS\n",case_names[m],ok,(unsigned long long)s);dispose_host_fixture();
 }
 for(int failure=1;failure<=10;failure++){
  setup_app(SUCCESS,failure);EFI_STATUS s=GxPrepare(&Context);GF_PLAN plan;GC_RESULT result={0};
  if(!EFI_ERROR(s))s=GfPrepare(&Context,&plan);
  if(failure==8)registers[SS0/4]^=1;
  if(failure==9)registers[SS1/4]^=1;
  if(failure==10)config[0x10]^=0x10;
  if(!EFI_ERROR(s))s=GfRun(&Context,&plan,&result);
  CHECK(EFI_ERROR(s));if(failure<=4 || failure>=8)CHECK(!fake.mmio_writes && !fake.flrs);
  if(failure==6 || failure==7)CHECK(!fake.flrs && Gu.retained && !result.restored);passed++;printf("APP fault=%d status=%#llx PASS\n",failure,(unsigned long long)s);dispose_host_fixture();
 }
 printf("PASS %d actual preflight + independent display/ownership + transaction + finish chains; services/firmware simulated.\n",passed);
 return 0;
}
