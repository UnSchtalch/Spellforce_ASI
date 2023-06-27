#include <windows.h>
#include "asi/sf_asi.h"
#include "asi/SF_SpellManager.h"
#include "asi/SF_SpellData.h"
#include "asi/SF_GdFigure.h"
#include "asi/SF_XData.h"
#include "asi/SF_Unknown.h"
#include "asi/SF_GdEffect.h"


unsigned int JOB_SELECT_RET;
unsigned int JOB_SELECT_CONT;

void __declspec(naked) worker_miner_hook (){
    asm(
    "movzbl 0x1a(%%ecx), %%eax\n\t"
    "dec %%eax\n\t"
    "cmpb $0x2, %%al\n\t" //are we elves
    "jne 1f\n\t" //if not -- jump away
    "movl $0x14, %%ebx\n\t"//we are! looking for our small HQ
    "jmp *%0\n\t"  //jump away to look for
    "1: cmpb $0x3, %%al\n\t" //are we trolls?
    "je 2f\n\t"//if we are - jump forward
    "jmp *%1\n\t" //we are not, let's get back
    "2: movl $0x49, %%ebx\n\t"//we are! looking for our small HQ
    "jmp *%0\n\t":  //jump away to look for
    : "o"(JOB_SELECT_RET),"m"(JOB_SELECT_CONT)
    );
}


void hookModernVersion()
{
	JOB_SELECT_RET = ASI::AddrOf(0x34874C);
	JOB_SELECT_CONT = ASI::AddrOf(0x3486EE);
	ASI::MemoryRegion mreg(ASI::AddrOf(0x3486E9), 5); //building selector

	ASI::BeginRewrite(mreg);
        *(unsigned char*)(ASI::AddrOf(0x3486E9)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x3486EA)) = (int)(&worker_miner_hook) - ASI::AddrOf(0x3486EE);
    ASI::EndRewrite(mreg);
}

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
            return FALSE;
        //!ASI::CheckSFVersion(ASI::SF_154) &&
        if (!ASI::CheckSFVersion(ASI::SF_161))
        {
            return FALSE;
        }
        //game_window = *(HWND*)(ASI::AddrOf(ASI::WINDOW_OFFSET));
  

        /*if (ASI::CheckSFVersion(ASI::SF_154))
        {
                hookLegacyVersion();
                break;
        }*/
        if (ASI::CheckSFVersion(ASI::SF_161))
        {
                hookModernVersion();
                break;
        }
     break;
    }
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}