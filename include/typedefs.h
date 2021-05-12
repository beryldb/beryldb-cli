/*
 * Emerald - A POSIX client for BerylDB.
 * http://www.beryldb.com
 *
 * Copyright (C) 2015-2021 Carlos F. Ferry <cferry@beryldb.com>
 * 
 * This file is part of BerylDB. BerylDB is free software: you can
 * redistribute it and/or modify it under the terms of the BSD License
 * version 3.
 *
 * More information about our licensing can be found at https://docs.beryl.dev
 */

#pragma once

#define MSG_LIMIT 65536
#define CHA_MAX 200
#define INS_MAX 200
#define HIS_MAX 50

#define HIST_FILE ".hist.cli"

struct Param 
{
	char  *prefix;
	char  *suffix;
	char  *message;
	char  *nickname;
	char  *command;
	char  *channel;
	char  *params;
	size_t offset;
	size_t maxcols;
	int nicklen;
};

struct State 
{
	char  *prompt;     /* Prompt to display. */
	char  *buf;        /* Edited line buffer. */
	size_t buflen;     /* Edited line buffer size. */
	size_t plenb;       /* Prompt length. */
	size_t plenu8;       /* Prompt length. */
	size_t posb;        /* Current cursor position. */
	size_t reserverd_position;        /* Current cursor position. */
	size_t oldposb;     /* Previous refresh cursor position. */
	size_t oldreserverd_position;     /* Previous refresh cursor position. */
	size_t lenb;        /* Current edited line length. */
	size_t lenu8;        /* Current edited line length. */
	size_t cols;       /* Number of columns in terminal. */
	int history_id; /* Current line in the edit history */
};

struct abuf 
{
	char *b;
	int len;
};

enum KeyAction
{
	KEY_NULL = 0,	    /* NULL */
	CTRL_A = 1,         /* Ctrl+a */
	CTRL_B = 2,         /* Ctrl-b */
	CTRL_C = 3,         /* Ctrl-c */
	CTRL_D = 4,         /* Ctrl-d */
	CTRL_E = 5,         /* Ctrl-e */
	CTRL_F = 6,         /* Ctrl-f */
	CTRL_H = 8,         /* Ctrl-h */
	TAB = 9,            /* Tab */
	CTRL_K = 11,        /* Ctrl+k */
	CTRL_L = 12,        /* Ctrl+l */
	ENTER = 13,         /* Enter */
	CTRL_N = 14,        /* Ctrl-n */
	CTRL_P = 16,        /* Ctrl-p */
	CTRL_T = 20,        /* Ctrl-t */
	CTRL_U = 21,        /* Ctrl+u */
	CTRL_W = 23,        /* Ctrl+w */
	ESC = 27,           /* Escape */
	BACKSPACE =  127    /* Backspace */
};