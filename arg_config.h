#pragma once

#include "wcap_config.h"

#include <stdlib.h>
#include <wchar.h>

#define TITLE_SIZE 1024

typedef struct
{
	Config wcap;
	wchar_t title[TITLE_SIZE];
	wchar_t* filepath;
}
CmdConfig;

static void PrintUsage(const wchar_t* ProgramName)
{
	wprintf(L"Usage: %ls [options] outputname\n", ProgramName);
	puts("Program options:");
	puts("  -i  --input <TARGET>             The recording target, the title of the window.");
	puts("  -m  --mouse                      Draw mouse in the recording video.");
	puts("  -r  --framerate <FRAME_PER_SEC>  Specify frame per second of the recording video.");
	puts("  -t  --time <DURATION>            Specify duration of the recording video.");
	puts("  -w  --width <WIDTH>              The max width of resulting video.");
	puts("  -h  --height <HEIGHT>            The max height of resulting video.");
	puts("      --h264                       Use h264 encoder for video (default).");
	puts("      --h265                       Use h265 (hevc) encoder for video.");
	puts("  -b  --bitrate <BITRATE>          Specify average bitrate in kbps (default 8000).");
	puts("  -?  --help                       Show this message.");
}

static BOOL CmdConfig__GetOptionValue(
	int argc, wchar_t* argv[], int* Index, const wchar_t* Argument,
	const wchar_t* LongName, wchar_t ShortName, const wchar_t** Value)
{
	size_t LongLength = wcslen(LongName);
	BOOL MatchesLong = wcscmp(Argument, LongName) == 0;
	BOOL MatchesShort = Argument[0] == L'-' && Argument[1] == ShortName;

	if (!MatchesLong && wcsncmp(Argument, LongName, LongLength) == 0 && Argument[LongLength] == L'=')
	{
		*Value = Argument + LongLength + 1;
		return TRUE;
	}

	if (MatchesShort && Argument[2] != L'\0')
	{
		*Value = Argument + 2;
		return TRUE;
	}

	if (!MatchesLong && !(MatchesShort && Argument[2] == L'\0'))
	{
		return FALSE;
	}

	*Value = ++(*Index) < argc ? argv[*Index] : NULL;
	return TRUE;
}

static BOOL ParseArgs(int argc, wchar_t* argv[], CmdConfig* Config)
{
	Config_Defaults(&Config->wcap);
	Config->wcap.MouseCursor = FALSE;
	Config->wcap.VideoMaxFramerate = 25;
	Config->wcap.OpenFolder = FALSE;
	Config->title[0] = L'\0';
	Config->filepath = NULL;

	BOOL OptionsEnabled = TRUE;
	for (int Index = 1; Index < argc; Index++)
	{
		const wchar_t* Argument = argv[Index];
		const wchar_t* Value;

		if (OptionsEnabled && wcscmp(Argument, L"--") == 0)
		{
			OptionsEnabled = FALSE;
			continue;
		}
		if (OptionsEnabled && (wcscmp(Argument, L"-?") == 0 || wcscmp(Argument, L"--help") == 0))
		{
			return FALSE;
		}
		if (OptionsEnabled && (wcscmp(Argument, L"-m") == 0 || wcscmp(Argument, L"--mouse") == 0))
		{
			Config->wcap.MouseCursor = TRUE;
			continue;
		}
		if (OptionsEnabled && wcscmp(Argument, L"--h264") == 0)
		{
			Config->wcap.VideoCodec = CONFIG_VIDEO_H264;
			Config->wcap.VideoProfile = CONFIG_VIDEO_HIGH;
			continue;
		}
		if (OptionsEnabled && wcscmp(Argument, L"--h265") == 0)
		{
			Config->wcap.VideoCodec = CONFIG_VIDEO_H265;
			Config->wcap.VideoProfile = CONFIG_VIDEO_MAIN;
			continue;
		}

		if (OptionsEnabled && CmdConfig__GetOptionValue(argc, argv, &Index, Argument, L"--input", L'i', &Value))
		{
			if (!Value) return FALSE;
			wcsncpy_s(Config->title, TITLE_SIZE, Value, _TRUNCATE);
			continue;
		}
		if (OptionsEnabled && CmdConfig__GetOptionValue(argc, argv, &Index, Argument, L"--framerate", L'r', &Value))
		{
			if (!Value) return FALSE;
			Config->wcap.VideoMaxFramerate = _wtoi(Value);
			continue;
		}
		if (OptionsEnabled && CmdConfig__GetOptionValue(argc, argv, &Index, Argument, L"--width", L'w', &Value))
		{
			if (!Value) return FALSE;
			Config->wcap.VideoMaxWidth = _wtoi(Value);
			continue;
		}
		if (OptionsEnabled && CmdConfig__GetOptionValue(argc, argv, &Index, Argument, L"--height", L'h', &Value))
		{
			if (!Value) return FALSE;
			Config->wcap.VideoMaxHeight = _wtoi(Value);
			continue;
		}
		if (OptionsEnabled && CmdConfig__GetOptionValue(argc, argv, &Index, Argument, L"--time", L't', &Value))
		{
			if (!Value) return FALSE;
			int Length = _wtoi(Value);
			Config->wcap.EnableLimitLength = Length > 0;
			Config->wcap.LimitLength = Length;
			continue;
		}
		if (OptionsEnabled && CmdConfig__GetOptionValue(argc, argv, &Index, Argument, L"--bitrate", L'b', &Value))
		{
			if (!Value) return FALSE;
			int Bitrate = _wtoi(Value);
			if (Bitrate > 0)
			{
				Config->wcap.VideoBitrate = Bitrate;
			}
			continue;
		}

		if ((OptionsEnabled && Argument[0] == L'-') || Config->filepath)
		{
			return FALSE;
		}
		Config->filepath = argv[Index];
	}

	return Config->filepath != NULL;
}
