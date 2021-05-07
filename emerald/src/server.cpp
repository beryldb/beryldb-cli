/*
 * BerylDB - A modular database.
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

/*
 * server.cpp is largely based in Michael Czigler's IRC client: kirc.
 *
 * Many credits goes to the kirc team (Copyright 2021 - MIT license),
 * Michael and his great work. 
 */

#include <sys/stat.h> 

#include "emerald.h"
#include "protocols.h"
#include "converter.h"
#include "typedefs.h"
#include "server.h"
#include "methods.h"
#include "nodes.h"
#include "exit.h"

static char  cdef[MSG_LIMIT] = "";
static int   conn;                      /* connection socket */
static int   Reserved = 0; 

static struct termios orig;
static int rawmode = 0;
static int atexit_registered = 0;

static int history_length = 0;
static char **history = NULL;

static void FreeHistory(void) 
{
	if (history) 
	{
		int j;

		for (j = 0; j < history_length; j++) 
		{
			free(history[j]);
		}

		free(history);
	}
}

static void DisableRawMode(int fd) 
{
	if (rawmode && tcsetattr(fd, TCSAFLUSH, &orig) != -1) 
	{
		rawmode = 0;
	}
}

static void AtExit(void)
{
   	DisableRawMode(STDIN_FILENO);
	FreeHistory();	
}

static int EnableRawMode(int fd) 
{
	if (!isatty(STDIN_FILENO)) 
	{
		goto fatal;
	}

	if (!atexit_registered) 
	{
		atexit(AtExit);
		atexit_registered = 1;
	}
	
	if (tcgetattr(fd,&orig) == -1) 
	{
		goto fatal;
	}
	
	{
		struct termios raw = orig;
		raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
		raw.c_oflag &= ~(OPOST);
		raw.c_cflag |= (CS8);
		raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
		raw.c_cc[VMIN] = 1;
		raw.c_cc[VTIME] = 0;

		if (tcsetattr(fd,TCSAFLUSH,&raw) < 0) 
		{
			goto fatal;
		}
		
		rawmode = 1;
		return 0;
	}
fatal:
	errno = ENOTTY;
	return -1;
}

static int get_cursor_pos(int ifd, int ofd) 
{
	char buf[32];
	int cols, rows;
	unsigned int i = 0;

	if (write(ofd, "\x1b[6n", 4) != 4) 
	{
		return -1;
	}

	while (i < sizeof(buf) - 1) 
	{
		if (read(ifd, buf + i, 1) != 1) 
		{
			break;
		}

		if (buf[i] == 'R') 
		{
			break;
		}
		i++;
	}

	buf[i] = '\0';

	if (buf[0] != 27 || buf[1] != '[') 
	{
		return -1;
	}

	if (sscanf(buf+2, "%d;%d", &rows, &cols) != 2) 
	{
		return -1;
	}

	return cols;
}

static int get_columns(int ifd, int ofd) 
{
	struct winsize ws;

	if (ioctl(1, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) 
	{
		int start = get_cursor_pos(ifd, ofd);

		if (start == -1) 
		{
			return 80;
		}
		if (write(ofd,"\x1b[999C",6) != 6) 
		{
			return 80;
		}

		int cols = get_cursor_pos(ifd, ofd);

		if (cols == -1) 
		{
			return 80;
		}

		if (cols > start) 
		{
			char seq[32];
			snprintf(seq, sizeof(seq), "\x1b[%dD", cols - start);
		
			if (write(ofd, seq, strnlen(seq, MSG_LIMIT)) == -1) 
			{
			
			}
		}
		
		return cols;

	} 
	else
	 {
		return ws.ws_col;
	}
}

static void buffer_pos_move(struct State *l, ssize_t dest, ssize_t src, size_t size) 
{
	memmove(l->buf + l->posb + dest, l->buf + l->posb + src, size);
}

static void move_buffer_pos_end(struct State *l, ssize_t dest, ssize_t src) 
{
	buffer_pos_move(l, dest, src, l->lenb - (l->posb + src) + 1);
}

static int start_u8set(char c) 
{
	int ret = 1;

	if (Reserved != 0) 
	{
		ret = (c & 0x80) == 0x00 || (c & 0xC0) == 0xC0;
	}
	
	return ret;
}

static int size_u8set(char c) 
{
	int ret = 1;

	if (Reserved != 0) 
	{
		int size = 0;
	
		while (c & (0x80 >> size)) 
		{
			size++;
		}

		ret = (size != 0) ? size : 1;
	}

	return ret;
}

static size_t u8_length(const char *s) 
{
	size_t lenu8 = 0;

	while (*s != '\0') 
	{
		lenu8 += start_u8set(*(s++));
	}

	return lenu8;
}

static size_t u8Prev(const char *s, size_t posb) 
{
	if (posb != 0) 
	{
		do 
		{
			posb--;
		} while ((posb > 0) && !start_u8set(s[posb]));
	}

	return posb;
}

static size_t ReservedNext(const char *s, size_t posb) 
{
	if (s[posb] != '\0') 
	{
		do 
		{
			posb++;
		} while((s[posb] != '\0') && !start_u8set(s[posb]));
	}
	
	return posb;
}

static int SetReserved(int ifd, int ofd) 
{
	if (Reserved) 
	{
		return 0;
	}
	
	if (write(ofd, "\r", 1) != 1) 
	{
		return -1;
	}

	if (get_cursor_pos(ifd, ofd) != 1) 
	{
		return -1;
	}
	const char* testChars[] = 
	{
		"\xe1\xbb\xa4",
		NULL
	};

	for (const char** it = testChars; *it; it++)
	{
		if (write(ofd, *it, strlen(*it)) != (ssize_t) strlen(*it)) 
		{
			return -1;
		}

		int pos = get_cursor_pos(ifd, ofd);

		if (write(ofd, "\r", 1) != 1) 
		{
			return -1;
		}

		for (int i = 1; i < pos; i++) 
		{
			if (write(ofd, " ", 1) != 1) 
			{
				return -1;
			}
		}

		if (write(ofd, "\r", 1) != 1) 
		{
			return -1;
		}

		if (pos != 2) 
		{
			return 0;
		}
	}

	Reserved = 1;
	return 0;
}

void Server::ClearScreen(void) 
{
    if (write(STDOUT_FILENO,"\x1b[H\x1b[2J",7) <= 0) 
    {
    
    }
}
static void abInit(struct abuf *ab) 
{
	ab->b = NULL;
	ab->len = 0;
}

static void A_BufferAppend(struct abuf *ab, const char *s, int len) 
{
	char *New = (char*)realloc(ab->b, ab->len + len);

	if (New == NULL) 
	{
		return;
	}
	
	memcpy(New + ab->len, s, len);
	ab->b = New;
	ab->len += len;
}

static void ABufferFree(struct abuf *ab) 
{
	free(ab->b);
}

static void UpdateLine(struct State *l) 
{
	char seq[64];
	size_t plenu8 = 0;

	if (!Kernel->display_select)
	{
        	plenu8 = strlen(Kernel->displayserver.c_str()) + 2;
        }
        else
        {
        	plenu8 = strlen(Kernel->displayserver.c_str()) + strlen("|") + strlen(Kernel->select.c_str()) + 2;
        }

	int fd = STDOUT_FILENO;
	char *buf = l->buf;
	size_t lenb = l->lenb;
	size_t lenu8 = l->lenu8;
	size_t reserverd_position = l->reserverd_position;
	struct abuf ab;

	l->cols = get_columns(STDIN_FILENO, STDOUT_FILENO);

	while ((plenu8 + reserverd_position) >= l->cols) 
	{
		size_t movedBy = ReservedNext(buf, 0);
		buf += movedBy;
		lenb += movedBy;
		lenu8--;
		reserverd_position--;
	}

	while ((plenu8 + lenu8) > l->cols) 
	{
		lenu8--;
		lenb = u8Prev(buf, lenb);
	}

	abInit(&ab);

	snprintf(seq, sizeof(seq), "\r");
	A_BufferAppend(&ab, seq, strnlen(seq, MSG_LIMIT));
	A_BufferAppend(&ab,l->prompt, l->plenb);
	
	if (!Kernel->display_select)
	{
        	A_BufferAppend(&ab, Kernel->displayserver.c_str(), strlen(Kernel->displayserver.c_str()));
	}
	else
	{
    		std::string format = Kernel->displayserver + "|" + Kernel->select;
    		A_BufferAppend(&ab, format.c_str(), strlen(format.c_str()));
        }
	
	A_BufferAppend(&ab, "> ", 2);
	A_BufferAppend(&ab, buf, lenb);

	snprintf(seq, sizeof(seq), "\x1b[0K");

	A_BufferAppend(&ab, seq, strnlen(seq, MSG_LIMIT));

	if (reserverd_position + plenu8) 
	{
		snprintf(seq, sizeof(seq), "\r\x1b[%dC", (int)(reserverd_position + plenu8));
	} 
	else 
	{
		snprintf(seq, sizeof(seq), "\r");
	}

	A_BufferAppend(&ab, seq, strlen(seq));

	if (write(fd, ab.b, ab.len) == -1) 
	{
	
	}
	
	ABufferFree(&ab);
}

static int UpdateInsert(struct State *l, char *c) 
{
	size_t clenb = strlen(c);

	if ((l->lenb + clenb) < l->buflen) 
	{
		if (l->lenu8 == l->reserverd_position) 
		{
			strcpy(l->buf + l->posb, c);
			l->reserverd_position++;
			l->lenu8++;
			l->posb += clenb;
			l->lenb += clenb;

			if ((l->plenu8 + l->lenu8) < l->cols) 
			{
				if (write(STDOUT_FILENO, c, clenb) == -1) 
				{
					return -1;
				}
			} 
			else 
			{
				UpdateLine(l);
			}

		} 
		else 
		{
			move_buffer_pos_end(l, clenb, 0);
			memmove(l->buf + l->posb, c, clenb);
			l->reserverd_position++;
			l->lenu8++;
			l->posb += clenb;
			l->lenb += clenb;
			UpdateLine(l);
		}
	}
	
	return 0;
}

static void MoveLeft(struct State *l) 
{
	if (l->posb > 0) 
	{
		l->posb = u8Prev(l->buf, l->posb);
		l->reserverd_position--;
		UpdateLine(l);
	}
}

static void MoveRight(struct State *l) 
{
	if (l->reserverd_position != l->lenu8) 
	{
		l->posb = ReservedNext(l->buf, l->posb);
		l->reserverd_position++;
		UpdateLine(l);
	}
}

static void MoveHome(struct State *l) 
{
	if (l->posb != 0) 
	{
		l->posb = 0;
		l->reserverd_position = 0;
		UpdateLine(l);
	}
}

static void MoveEnd(struct State *l) 
{
	if (l->reserverd_position != l->lenu8) 
	{
		l->posb = l->lenb;
		l->reserverd_position = l->lenu8;
		UpdateLine(l);
	}
}

static void Delete(struct State *l) 
{
	if ((l->lenu8 > 0) && (l->reserverd_position < l->lenu8)) 
	{
		size_t this_size = ReservedNext(l->buf, l->posb) - l->posb;
		move_buffer_pos_end(l, 0, this_size);
		l->lenb -= this_size;
		l->lenu8--;
		UpdateLine(l);
	}
}

static void Backspace(struct State *l) 
{
	if ((l->reserverd_position > 0) && (l->lenu8 > 0)) 
	{
		size_t prev_size = l->posb - u8Prev(l->buf, l->posb);
		move_buffer_pos_end(l, (ssize_t)-prev_size, 0);
		l->posb -= prev_size;
		l->lenb -= prev_size;
		l->reserverd_position--;
		l->lenu8--;
		UpdateLine(l);
	}
}

static void DeletePrevWord(struct State *l) 
{
	size_t old_posb = l->posb;
	size_t old_reserverd_position = l->reserverd_position;

	while ((l->posb > 0) && (l->buf[l->posb - 1] == ' ')) 
	{
		l->posb--;

		l->reserverd_position--;
	}

	while ((l->posb > 0) && (l->buf[l->posb - 1] != ' ')) 
	{
		if (start_u8set(l->buf[l->posb - 1])) 
		{

			l->reserverd_position--;
		}

		l->posb--;
	}

	size_t diffb = old_posb - l->posb;
	size_t diffu8 = old_reserverd_position - l->reserverd_position;
	move_buffer_pos_end(l, 0, diffb);
	l->lenb -= diffb;
	l->lenu8 -= diffu8;
	UpdateLine(l);
}

static void DeleteWholeLine(struct State *l) 
{
	l->buf[0] = '\0';
	l->posb = l->lenb = l->reserverd_position = l->lenu8 = 0;
	UpdateLine(l);
}

static void DeleteLineToEnd(struct State *l) 
{
	l->buf[l->posb] = '\0';
	l->lenb = l->posb;
	l->lenu8 = l->reserverd_position;
	UpdateLine(l);
}

static void SwapCharWithPrev(struct State *l) 
{
	if (l->reserverd_position > 0 && l->reserverd_position < l->lenu8) 
	{
		char aux[8];
		ssize_t prev_size = l->posb - u8Prev(l->buf, l->posb);
		ssize_t this_size = ReservedNext(l->buf, l->posb) - l->posb;
		memmove(aux, l->buf + l->posb, this_size);
		buffer_pos_move(l, -prev_size + this_size, -prev_size, prev_size);
		memmove(l->buf + l->posb - prev_size, aux, this_size);
		
		if (l->reserverd_position != l->lenu8-1)
		{
			l->reserverd_position++;
			l->posb += this_size;
		}
		
		UpdateLine(l);
	}
}

static void History(struct State *l, int dir) 
{
	if (history_length > 1) 
	{
		free(history[history_length - (1 + l->history_id)]);
		history[history_length - (1 + l->history_id)] = strdup(l->buf);
		l->history_id += (dir == 1) ? 1 : -1;
	
		if (l->history_id < 0) 
		{
			l->history_id = 0;
			return;

		} 
		else if (l->history_id >= history_length) 
		{
			l->history_id = history_length - 1;
			return;
		}

		strncpy(l->buf, history[history_length - (1 + l->history_id)], l->buflen);
		l->buf[l->buflen - 1] = '\0';
		l->lenb = l->posb = strlen(l->buf);
		l->lenu8 = l->reserverd_position = u8_length(l->buf);
		UpdateLine(l);
	}
}

static int push_history(const char *line) 
{
	char *linecopy;
	
	if (HIS_MAX == 0) 
	{
		return 0;
	}

	if (history == NULL) 
	{
		history = (char**)malloc(sizeof(char*)*HIS_MAX);

		if (history == NULL) 
		{
			return 0;
		}

		memset(history, 0, (sizeof(char*)*HIS_MAX));
	}

	if (history_length && !strcmp(history[history_length-1], line)) 
	{
		return 0;
	}

	linecopy = strdup(line);

	if (!linecopy) 
	{
		return 0;
	}

	if (history_length == HIS_MAX) 
	{
		free(history[0]);
		memmove(history, history + 1, sizeof(char*)*(HIS_MAX - 1));
		history_length--;
	}

	history[history_length] = linecopy;
	history_length++;
	return 1;
}

static void UpdateEnter(void) 
{
	history_length--;
	free(history[history_length]);
}

static void EscSequence(struct State *l, char seq[3]) 
{
	if (read(STDIN_FILENO, seq, 1) == -1) return;
	if (read(STDIN_FILENO, seq + 1, 1) == -1) return;

	if (seq[0] == '[') 
	{
		if ((seq[1] >= '0') && (seq[1] <= '9')) 
		{
			if (read(STDIN_FILENO, seq + 2, 1) == -1) 
			{
				return;
			}

			if ((seq[2] == '~') && (seq[1] == 3)) 
			{
				Delete(l);	
			}
		} 
		else 
		{
			switch(seq[1]) 
			{
				case 'A': History(l, 1); break; /* Up */
				case 'B': History(l, 0); break; /* Down */
				case 'C': MoveRight(l);  break; /* Right */
				case 'D': MoveLeft(l);   break; /* Left */
				case 'H': MoveHome(l);   break; /* Home */
				case 'F': MoveEnd(l);	break; /* End*/
			}
		}
	} 
	else if (seq[0] == 'O') 
	{ 
		switch(seq[1]) 
		{
			case 'H': MoveHome(l); break; /* Home */
			case 'F': MoveEnd(l);  break; /* End*/
		}
	}
}

static int edit(struct State *l) 
{
	char c, seq[3];
	ssize_t stream_read = read(STDIN_FILENO, &c, 1);
	
	if (stream_read <= 0) 
	{
		return 1;
	}
	
	switch(c) 
	{
		case CTRL_L: Server::ClearScreen(); 	 UpdateLine(l); break;
		case ENTER: UpdateEnter();          return 1; /* enter */
		case CTRL_C: 
		{
		
			errno = EAGAIN;
                        std::string buf = l->buf;
                                
                        if (buf == "")
                        {
                        	Daemon::sprint(DTYPE_R, " ", " ");                     
                        }
                        else
                        {
                                Daemon::sprint(DTYPE_R, "%s", buf.c_str());                     
                        }
		        
			Kernel->Exit(EXIT_CODE_OK, false);
			return -1; /* ctrl-c */
		}
		case BACKSPACE:                                /* backspace */
		case CTRL_H:  Backspace(l);        break; /* ctrl-h */
		case CTRL_B:  MoveLeft(l);         break; /* ctrl-b */
		case CTRL_F:  MoveRight(l);        break; /* ctrl-f */
		case CTRL_A:  MoveHome(l);         break; /* Ctrl+a */
		case CTRL_E:  MoveEnd(l);          break; /* ctrl+e */
		case CTRL_W: DeletePrevWord(l);   break; /* ctrl+w */
		case CTRL_U: DeleteWholeLine(l);  break; /* Ctrl+u */
		case CTRL_K: DeleteLineToEnd(l);  break; /* Ctrl+k */
		case CTRL_N: History(l, 0);       break; /* Ctrl+n */
		case CTRL_P: History(l, 1);       break; /* Ctrl+p */
		case CTRL_T: SwapCharWithPrev(l); break; /* ctrl-t */
		case ESC: EscSequence(l, seq); break; /* escape sequence */
		case CTRL_D:                                  /* ctrl-d */
		{

                        std::string buf = l->buf;
                                
                        if (buf == "")
                        {
                                Daemon::sprint(DTYPE_R, " ", " ");                     
                        }
                        else
                        {
                                Daemon::sprint(DTYPE_R, "%s", buf.c_str());                     
                        }
                        
                        Kernel->Exit(EXIT_CODE_OK, false);

			break;
		}
		default:
		{
			if (start_u8set(c)) 
			{
				char aux[8];
				aux[0] = c;
				int size = size_u8set(c);
				
				for (int i = 1; i < size; i++) 
				{
					stream_read = read(STDIN_FILENO, aux + i, 1);
					
					if ((aux[i] & 0xC0) != 0x80) 
					{
						break;
					}
				}
				
				aux[size] = '\0';
				
				if (UpdateInsert(l, aux)) 
				{	
					return -1;
				}
			}
			
			break;
		}
	}
	
	return 0;
}

static void ResetState(struct State *l) 
{
	l->plenb = strlen(l->prompt);
	l->plenu8 = u8_length(l->prompt);
	l->oldposb = l->posb = l->oldreserverd_position = l->reserverd_position = l->lenb = l->lenu8 = 0;
	l->history_id = 0;
	l->buf[0] = '\0';
	l->buflen--;
	push_history("");
}

void Server::raw(char *fmt, ...) 
{
	va_list ap;
	char *cmd_str = (char*)malloc(MSG_LIMIT);

	if (!cmd_str) 
	{
		perror("malloc");
		Kernel->Exit(EXIT_CODE_SOCKETSTREAM);
	}

	va_start(ap, fmt);
	vsnprintf(cmd_str, MSG_LIMIT, fmt, ap);
	va_end(ap);

	if (write(conn, cmd_str, strnlen(cmd_str, MSG_LIMIT)) < 0) 
	{
		perror("write");
		Kernel->Exit(EXIT_CODE_SOCKETSTREAM);
	}

	free(cmd_str);
}

static int InitConnection(void) 
{
	int gai_status;

	struct addrinfo *res, hints = 
	{
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM
	};

	if ((gai_status = getaddrinfo(Kernel->Config->host.c_str(), Kernel->Config->port.c_str(), &hints, &res)) != 0) 
	{
		bprint(ERROR, "Unable to connect: %s", gai_strerror(gai_status));
		Kernel->Exit(EXIT_CODE_SOCKETSTREAM, true);
		return -1;
	}

	struct addrinfo *p;

	for (p = res; p != NULL; p = p->ai_next) 
	{
		if ((conn = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
		{
			Kernel->Exit(EXIT_CODE_SOCKETSTREAM, true);
			continue;
		}

		if (connect(conn, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(conn);
			bprint(ERROR, "Unable to connect to %s:%s", Kernel->Config->host.c_str(), Kernel->Config->port.c_str());
			Kernel->Exit(EXIT_CODE_SOCKETSTREAM, true);
			continue;
		}

		break;
	}

	freeaddrinfo(res);

	if (p == NULL) 
	{
		fputs("Failed to connect\n", stderr);
		return -1;
	}

	int flags = fcntl(conn, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(conn, F_SETFL, flags);

	return 0;
}

static void UnknownMessage(struct Param *p) 
{
	if (!p->message) 
	{
		return;
	}

    	Daemon::sprint(DTYPE_R, "%s", p->message);
}

static void CommandParser(char *request) 
{
	const std::string modified = convto_string(request);

	if (!strncmp(request, convto_string(BRLD_PING).c_str(), 3)) 
	{	
		Kernel->Link.raw("PONG %s\r\n", request);
		return;
	}

	if (request[0] != ':' || (strnlen(request, MSG_LIMIT) < 4)) 
	{
		return;
	}

	printf("\r\x1b[0K");

	struct Param p;

	p.prefix =   strtok(request, " ") + 1;
	p.suffix =   strtok(NULL, ":");
	p.message =  strtok(NULL, "\r");
	p.nickname = strtok(p.prefix, "!");
	p.command =  strtok(p.suffix, "#& ");
	p.channel =  strtok(NULL, " \r");
	p.params =   strtok(NULL, ":\r");
	p.maxcols = get_columns(STDIN_FILENO, STDOUT_FILENO);
	p.nicklen = (p.maxcols / 3 > INS_MAX ? INS_MAX : p.maxcols / 3);
	p.offset = 0;

	std::string cmd = convto_string(strtok(p.suffix, "#& "));
	std::string msg = convto_string(p.message);

	std::string protocol = cmd.substr(0, 3);
	unsigned int brld_protocol = 0;
	
	if (cmd == "PUBLISH")
	{
		brld_protocol = BRLD_PUBLISH;	
	}
	else 
	{
	
		if (!protocol.empty())
		{
				brld_protocol = convto_num<unsigned int>(protocol);
		}
	}

	std::string response;

	engine::space_node_stream CMD(modified);
	std::string server;
        std::vector<std::string> params;

	unsigned int counter = 0;


	while (CMD.items_extract(server))
	{   
		std::string param = server;

		if (counter == 0)
		{
			    param.erase(0, 1);
		}
		else 
		{

		}

		params.push_back(param);

		counter++;
	}

	switch (brld_protocol)
	{

			case BRLD_CONNECTED:

				Kernel->Handler.OnConnected(params);
				
			break;
			
			case ERR_CMD_NOFND:
				
				response = Kernel->Handler.OnNotFnd(params);
			break;
			
			case  ERR_WRONG_PASS:
			
				Kernel->Handler.OnWrongPass();
			break;

			case BRLD_MYINFO:
					
				Kernel->Handler.MyInfo(params);

			break;

			case BRLD_JOIN:

				Kernel->Handler.OnJoin(params);
				return;
			break;
			
			case BRLD_ERROR:
			
				Kernel->Handler.OnError(params, msg);
				return;
			break;
			
			case BRLD_QUITTING:
				
				Kernel->Exit();
			break;

			case BRLD_PUBLISH:

				Kernel->Handler.OnPublish(params, msg);

			break;
			
			case BRLD_NEW_USE:
			
                                Kernel->Handler.OnMyUse(params);
                                return;
				
			break;

			case BRLD_ENDOFLINKS:
				
				return;

			break;

			case BRLD_ENDOFINFO:
				
				return;

			break;	
			
			case BRLD_YOUR_FLAGS:
				
                                Kernel->Handler.OnYourFlags(params);
                                return;
				
			break;

			case BRLD_QUIT:

				Kernel->Handler.OnQuit(params);

			break;

			case BRLD_UPTIME:

				response = Kernel->Handler.OnUptime(msg);

			break;

			case BRLD_SLIST_REPLY:
			
				Kernel->Handler.OnSlist(params, msg);
				return;
				
			break;


			case BRLD_END_OF_SLIST:
			
				return;
			break;

			case BRLD_END_STATUS:
			
				return;
			break;

			case BRLD_PART:	
					
				Kernel->Handler.OnPart(params);
				return;
					
			break;

			default:
			{
			 	UnknownMessage(&p);
				return;
			}

	}

	if (!response.empty())
	{	
			Daemon::sprint(DTYPE_R, "%s", response.c_str());
	} 
	else 
	{
		return;
	}
	
}

static char buffered_message[MSG_LIMIT + 1];
static size_t message_end = 0;

static int HandleMessage(void) 
{
	for (;;) 
	{
		ssize_t stream_read = read(conn, &buffered_message[message_end], MSG_LIMIT - message_end);
		if (stream_read == -1) 
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) 
			{
				return 0;
			} 
			else 
			{
				Kernel->Exit(EXIT_CODE_SOCKETSTREAM, false);
				return -2;
			}
		}

		if (stream_read == 0) 
		{
			printf("\x1b[0m\r");

			bprint(INFO, "Server has exited.");
                        printf("\x1b[0m\r");

			Kernel->Exit(EXIT_CODE_SOCKETSTREAM, false);
			return -1;
		}

		size_t i, old_message_end = message_end;
		message_end += stream_read;

		for (i = old_message_end; i < message_end; ++i) 
		{
			if (i != 0 && buffered_message[i - 1] == '\r' && buffered_message[i] == '\n') 
			{
				char saved_char = buffered_message[i + 1];
				buffered_message[i + 1] = '\0';
				CommandParser(buffered_message);
				buffered_message[i + 1] = saved_char;
				memmove(&buffered_message, &buffered_message[i + 1],
						message_end - i - 1);
				message_end = message_end - i - 1;
				i = 0;
			}
		}

		if (message_end == MSG_LIMIT) 
		{
			message_end = 0;
		}
	}
}

static void UserInput(struct State *l) 
{
	if (l->buf == NULL) 
	{
		return;
	}
	
	size_t msg_len = strnlen(l->buf, MSG_LIMIT);

	if (msg_len > 0 && l->buf[msg_len - 1] == '\n') 
	{
		l->buf[msg_len - 1] = '\0';
	}

	printf("\r\x1b[0K");

	switch (l->buf[0]) 
	{
		case '/':
		{	
			std::string buffer = l->buf + 1;
			Handlers::Local(buffer);
			break;
                	
		}
		
		
		default : 

			{
				std::string buf = l->buf;
				
				if (buf == "")
				{
                                        Daemon::sprint(DTYPE_R, " ", " ");                     
                                        return;
				}
				
			        engine::space_node_stream CMD(buf);
			        std::string server;
				std::vector<std::string> CommandList;
				
			        while (CMD.items_extract(server))
				{
			        	CommandList.push_back(server);	
			        }   
			        
			        if (Kernel->Link.CheckCmd(CommandList))
			        {
					Server::raw("%s\r\n", l->buf);
					Daemon::sprint(DTYPE_R, "%s", l->buf);					
					
				}
			
			}
	}
}

bool Server::CheckCmd(const std::vector<std::string>& CommandList)
{
	if (!CommandList.empty())
	{
		if (CommandList[0] == "lping")
		{
			Daemon::sprint(DTYPE_R, "alive.");
			return false;
		}
		
	}
	
	return true;
}

Server::Server()
{

}

void Server::Shutdown()
{
	Kernel->Connected = false;
	shutdown(conn, 1);
}

int Server::HistoryWrite() 
{
    mode_t old_umask = umask(S_IXUSR|S_IRWXG|S_IRWXO);
    FILE *fp;

    fp = fopen(HIST_FILE, "w");
    umask(old_umask);

    if (fp == NULL) 
    {
      	return -1;
    }

    chmod(HIST_FILE, S_IRUSR|S_IWUSR);
    
    for (int j = 0; j < history_length; j++)
    {
        fprintf(fp, "%s\n", history[j]);
    }
    
    fclose(fp);
    return 0;
}

int HistoryLoad() 
{
    FILE *fp = fopen(HIST_FILE, "r");
    char buffer[MSG_LIMIT];

    if (fp == NULL) 
    {
    	return -1;
    }   

    while (fgets(buffer, MSG_LIMIT,fp) != NULL) 
    {
        char *p;
        p = strchr(buffer, '\r');
        
        if (!p)
        {	
        	 p = strchr(buffer,'\n');
        }
	
        if (p) 
        {
        	*p = '\0';
	}
	
        push_history(buffer);
    }
    
    fclose(fp);
    return 0;
}

void Server::ResetCache()
{
	 history_length = 0;
	 *history = NULL;
}

void Server::PrepareExit()
{
   	DisableRawMode(STDIN_FILENO);
}

unsigned int Server::CountHistory()
{
	return history_length;
}

int Server::Initialize()
{
        HistoryLoad();

	if (InitConnection() != 0) 
	{
		return 1;
	}
	
	Methods::LogIn(SESSION_DEFAULT, Kernel->Config->login, Kernel->Config->pass);
	
	fds[0].fd = STDIN_FILENO;
	fds[1].fd = conn;
	fds[0].events = POLLIN;
	fds[1].events = POLLIN;
	
	char usrin[MSG_LIMIT];
	
	cstate.buf = usrin;
	cstate.buflen = MSG_LIMIT;
	cstate.prompt = cdef;
	ResetState(&cstate);
	
	int rc, ReturnFlag = 0;

	if (EnableRawMode(STDIN_FILENO) == -1) 
	{
		return 1;
	}

	if (SetReserved(STDIN_FILENO, STDOUT_FILENO) == -1) 
	{
		return 1;
	}

	for (;;) 
	{ 
		Kernel->Refresh();
		
		if (poll(fds, 2, -1) != -1) 
		{
			if (fds[0].revents & POLLIN) 
			{
				ReturnFlag = edit(&cstate);

				if (ReturnFlag > 0) 
				{
					push_history(cstate.buf);
					UserInput(&cstate);
					ResetState(&cstate);
				} 
				else if (ReturnFlag < 0) 
				{
 				        printf("\r\n");
 				        return 0;
				}

				UpdateLine(&cstate);
			}
			
			if (fds[1].revents & POLLIN) 
			{
				rc = HandleMessage();

				if (rc != 0) 
				{
					if (rc == -2) 
					{
						return 1;
					}

					return 0;
				}

				UpdateLine(&cstate);
			}
		} 
		else 
		{
			if (errno == EAGAIN) 
			{
				continue;
			}

			Kernel->Exit(EXIT_CODE_SOCKETSTREAM, false);
			return 1;
		}
	}
	
        Kernel->Exit(EXIT_CODE_SOCKETSTREAM, false);
}

