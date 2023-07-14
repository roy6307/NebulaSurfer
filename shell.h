#pragma once

#ifndef NEBULASURFER_SHELL_H
#define NEBULASURFER_SHELL_H
#endif

#ifndef IMGUI_VERSION
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#endif

#ifndef LIBSSH2_H
#include "libssh2/include/libssh2.h"
#endif

#ifndef NEBULASURFER_NETWORK_H
#include "network.h"
#endif

#ifndef _VECTOR_
#include <vector>
#endif

#ifndef _STRING_
#include <string>
#endif

#ifndef _REGEX_
#include <regex>
#endif

#ifndef _UTILITY_
#include <utility>
#endif

#ifndef _IOSTREAM_
#include <iostream>
#endif

#ifndef _SSTREAM_
#include <sstream>
#endif

#define ESC '\033'

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#erase-functions
/*
ESC[J	erase in display (same as ESC[0J)
ESC[0J	erase from cursor until end of screen
ESC[1J	erase from cursor to beginning of screen
ESC[2J	erase entire screen
ESC[3J	erase saved lines
ESC[K	erase in line (same as ESC[0K)
ESC[0K	erase from cursor to end of line
ESC[1K	erase start of line to the cursor
ESC[2K	erase the entire line
*/

//https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#colors--graphics-mode
// ABGR , SGR           ESC[1;34;{...}m
#define ANSI_COLOR_BLACK	0xFF000000
#define ANSI_COLOR_RED		0xFF0000FF
#define ANSI_COLOR_GREEN	0xFF00FF00
#define ANSI_COLOR_YELLOW	0xFFFFFF00
#define ANSI_COLOR_BLUE		0xFFFF0000
#define ANSI_COLOR_MAGENTA	0xFFFF00FF
#define ANSI_COLOR_CYAN		0xFF00FFFF
#define ANSI_COLOR_WHITE	0xFFFFFFFF
#define ANSI_COLOR_DEFAULT	0x0			// reset colors
#define ANSI_COLOR_RESET	0x1			// reset all colors, text effects

// https://github.com/ocornut/imgui/issues/1566
struct AnsiStr
{
	unsigned int Fg_Col = ANSI_COLOR_WHITE;
	unsigned int Bg_Col = ANSI_COLOR_BLACK;
	std::string text = "";
};

enum AnsiDataType
{
	PLAIN_TEXT,
	TEXT_FORMAT,
	CURSOR_CONTROL,
	ERASE_DISPLAY
};

struct AnsiData
{
	AnsiDataType type;
	std::string parameter;
};

namespace NebulaSurfer
{
	namespace SHELL
	{
		void parseANSICodes(const std::string &);
		void Print();
		void Render(const char *);
	}
}