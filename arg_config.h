#pragma once

#include "wcap_config.h"
#include "getoptw.h"

#include <wchar.h>

#define TITLE_SIZE 1024

typedef struct CmdConfig
{
	struct Config wcap;

	// title of the target window
	wchar_t title[TITLE_SIZE];

	// output file path.
	wchar_t* filepath;
} CmdConfig;

void printUsage(const wchar_t* progName)
{
	wprintf(L"Usage: %s [options] outputname\n", progName);
	puts("Program options:");
	puts("  -i  --input <TARGET>             The recording target, the title of the window.");
	puts("  -m  --mouse                      Draw mouse in the recording video.");
	puts("  -r  --framerate <FRAME_PER_SEC>  Specify frame per second of the recording video.");
	puts("  -t  --time <DURATION>            Specify duration of the recording video");
	puts("  -w  --width <WIDTH>              The max width of resulting video.");
	puts("  -h  --height <HEIGHT>            The max height of resulting video.");
	puts("      --h264                       Use h264 encoder for video (default).");
	puts("      --h265                       Use h265 (hevc) encoder for video.");
	puts("  -b  --bitrate <BITRATE>          Specify average bitrate of the recording video, in kbps (default 8000)");
	puts("  -?  --help                       Show this message.");
}

static int codec_h265_flag = 0;

static struct option longOptions[] =
{
	{L"help",      no_argument,       NULL, L'?'},
	{L"mouse",     no_argument,       NULL, L'm'},
	{L"width",     required_argument, NULL, L'w'},
	{L"height",    required_argument, NULL, L'h'},
	{L"input",     required_argument, NULL, L'i'},
	{L"framerate", required_argument, NULL, L'r'},
	{L"time",      required_argument, NULL, L't'},
	{L"bitrate",   required_argument, NULL, L'b'},
	{L"h264",      no_argument,       &codec_h265_flag, 0},
	{L"h265",      no_argument,       &codec_h265_flag, 1},
	{0,0,0,0}
};

BOOL parseArgs(int argc, wchar_t* argv[], CmdConfig* config)
{
	int c;

	config->title[0] = '\0';
	Config_Defaults(&(config->wcap));
	config->wcap.MouseCursor = FALSE;
	config->wcap.VideoMaxFramerate = 25;
	config->wcap.OpenFolder = FALSE;

	// start parse argument with getopt_log
	for (;;)
	{
		int optionIndex = 0;
		c = getopt_long(argc, argv, L"?mw:h:r:i:t:b:", longOptions, &optionIndex);

		if (c == -1)
		{
			break;
		}

		switch (c)
		{
		case 0:
			break;
		case L'?':
			return FALSE;
		case L'm':
			config->wcap.MouseCursor = TRUE;
			break;
		case L'r':
			config->wcap.VideoMaxFramerate = _wtoi(optarg);
			break;
		case L'w':
			config->wcap.VideoMaxWidth = _wtoi(optarg);
			break;
		case L'h':
			config->wcap.VideoMaxHeight = _wtoi(optarg);
			break;
		case L't':
		{
			int len = _wtoi(optarg);
			config->wcap.EnableLimitLength = (len > 0);
			config->wcap.LimitLength = len;
			break;
		}
		case L'b':
		{
			int rate = _wtoi(optarg);
			if (rate > 0)
			{
				config->wcap.VideoBitrate = rate;
			}
			break;
		}
		case L'i':
			wcsncpy(config->title, optarg, TITLE_SIZE);
			config->title[TITLE_SIZE - 1] = '\0';
			break;
		default:
			abort();
		}
	}

	if (codec_h265_flag)
	{
		config->wcap.VideoCodec = CONFIG_VIDEO_H265;
	}

	// there should be exactly 1 argument after parsing.
	if (optind + 1 != argc)
	{
		return FALSE;
	}

	config->filepath = argv[optind];

	return TRUE;
}
