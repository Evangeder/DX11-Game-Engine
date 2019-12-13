#pragma once
// Target Windows 7 or later
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>
// Defines below disable a lot of unused windows things.
// On errors, try commenting/removing some or every of those.
#ifndef FULL_WINTARD
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCOMM
#define NOCTLMGR
#define NODEFERWINDOWPOS
#define NODRAWTEXT
#define NOGDICAPMASKS
#define NOHELP
#define NOICONS
#define NOIMAGE
#define NOKANJI
#define NOKERNEL
#define NOMCX
#define NOMEMMGR
#define NOMENUS
#define NOMETAFILE
#define NONLS
#define NOOPENFILE
#define NOPROFILER
#define NOPROXYSTUB
#define NORASTEROPS
#define NORPC
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOTAPE
#define NOTEXTMETRIC
#define NOWH
#define OEMRESOURCE
#define WIN32_LEAN_AND_MEAN
#endif

#define NOMINMAX
#define STRICT

#include <Windows.h>