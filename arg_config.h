#pragma once

#include "wcap_config.h"
#include "getopt.h"

#include <wchar.h>

#define TITLE_SIZE 1024

typedef struct CmdConfig
{
	struct Config wcap;

	char title[TITLE_SIZE];

	const char* filepath;
} CmdConfig;

void printUsage(const char* progName)
{
	printf("Usage: %s [options] outputname\n", progName);
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
	{"help",      no_argument,       NULL, '?'},
	{"mouse",     no_argument,       NULL, 'm'},
	{"width",     required_argument, NULL, 'w'},
	{"height",    required_argument, NULL, 'h'},
	{"input",     required_argument, NULL, 'i'},
	{"framerate", required_argument, NULL, 'r'},
	{"time",      required_argument, NULL, 't'},
	{"bitrate",   required_argument, NULL, 'b'},
	{"h264",      no_argument,       &codec_h265_flag, 0},
	{"h265",      no_argument,       &codec_h265_flag, 1},
	{0,0,0,0}
};

BOOL parseArgs(int argc, char* argv[], CmdConfig* config)
{
	int c;

	config->title[0] = '\0';
	Config_Defaults(&(config->wcap));
	config->wcap.MouseCursor = FALSE;
	config->wcap.VideoMaxFramerate = 25;
	config->wcap.OpenFolder = FALSE;
	config->wcap.ShowNotifications = FALSE;

	// start parse argument with getopt_log
	for (;;)
	{
		int optionIndex = 0;
		c = getopt_long(argc, argv, "?mw:h:r:i:t:b:", longOptions, &optionIndex);

		if (c == -1)
		{
			break;
		}

		switch (c)
		{
		case 0:
			break;
		case '?':
			return FALSE;
		case 'm':
			config->wcap.MouseCursor = TRUE;
			break;
		case 'r':
			config->wcap.VideoMaxFramerate = atoi(optarg);
			break;
		case 'w':
			config->wcap.VideoMaxWidth = atoi(optarg);
			break;
		case 'h':
			config->wcap.VideoMaxHeight = atoi(optarg);
			break;
		case 't':
		{
			int len = atoi(optarg);
			config->wcap.EnableLimitLength = (len > 0);
			config->wcap.LimitLength = len;
			break;
		}
		case 'b':
		{
			int rate = atoi(optarg);
			if (rate > 0)
			{
				config->wcap.VideoBitrate = rate;
			}
			break;
		}
		case 'i':
			strncpy(config->title, optarg, TITLE_SIZE);
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
