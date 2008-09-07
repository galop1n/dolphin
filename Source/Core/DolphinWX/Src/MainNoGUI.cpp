#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
//#include <curses.h>
#else
#endif

#include "Globals.h"
#include "Common.h"
#include "ISOFile.h"
#include "CPUDetect.h"

#include "BootManager.h"
void* g_pCodeWindow = NULL;
void* main_frame = NULL;
bool wxPanicAlert(const char* text, bool /*yes_no*/)
{
	return(true);
}


void Host_BootingStarted(){}


void Host_BootingEnded(){}


// OK, this thread boundary is DANGEROUS on linux
// wxPostEvent / wxAddPendingEvent is the solution.
void Host_NotifyMapLoaded(){}


void Host_UpdateLogDisplay(){}


void Host_UpdateDisasmDialog(){}


void Host_UpdateMainFrame(){}

void Host_UpdateBreakPointView(){}


void Host_UpdateMemoryView(){}


void Host_SetDebugMode(bool){}


void Host_SetWaitCursor(bool enable){}


void Host_CreateDisplay(){}


void Host_CloseDisplay(){}

void Host_UpdateStatusBar(const char* _pText){}

// Include SDL header so it can hijack main().
#include <SDL.h>
#include "cmdline.h"

int main(int argc, char* argv[])
{
	gengetopt_args_info args_info;

	if (cmdline_parser (argc, argv, &args_info) != 0)
		return(1);

	if (args_info.inputs_num < 1)
	{
		fprintf(stderr, "Please supply at least one argument - the ISO to boot.\n");
		return(1);
	}
	std::string bootFile(args_info.inputs[0]);

	DetectCPU();
	BootManager::BootCore(bootFile);
	usleep(2000 * 1000 * 1000);
	//while (!getch()) {
	//	usleep(20);
	//}

	cmdline_parser_free (&args_info);
	return(0);
}
