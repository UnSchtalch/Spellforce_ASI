#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "asi/sf_asi.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

	switch (ul_reason_for_call)
	{
    case DLL_PROCESS_ATTACH:
    {
       	if (!ASI::Init(hModule))
       	{
           	return FALSE;
       	}
       	if (!ASI::CheckSFVersion(ASI::SF_BETA))
       	{
           	return FALSE;
       	}
       	ASI::MemoryRegion loot_mreg(ASI::AddrOf(0x22314D), 4);
  		ASI::BeginRewrite(loot_mreg);
       		*(unsigned char*)(ASI::AddrOf(0x223150)) = 0x01;
    	ASI::EndRewrite(loot_mreg);
	   	break;
    }
    case DLL_PROCESS_DETACH:
    	    break;
	}
    return TRUE;
}