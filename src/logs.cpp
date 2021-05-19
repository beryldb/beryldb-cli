/*
 * BerylDB - A modular database.
 * http://www.beryldb.com
 *
 * Copyright (C) 2021 - Carlos F. Ferry <cferry@beryldb.com>
 * 
 * This file is part of BerylDB. BerylDB is free software: you can
 * redistribute it and/or modify it under the terms of the BSD License
 * version 3.
 *
 * More information about our licensing can be found at https://docs.beryl.dev
 */

#include "emerald.h"

#include <fstream>

const std::string LogStream::StreamHead = "Log head for " + VERSION;

LogHandler::LogHandler() : Locked(false)
{

}
	
LogHandler::~LogHandler()
{

}

void LogHandler::OpenLogs()
{
        if (Kernel->Config->usercmd.forcedebug)
        {
                Kernel->Config->RawLog = true;
                return;
        }

	std::map<std::string, FileHandler*> logmap;
	MultiTag tags = Kernel->Config->GetTags("log");
	
	for (config_iterator i = tags.first; i != tags.second; ++i)
	{
		config_rule* tag = i->second;
		std::string method = tag->as_string("method");
	
		if (!stdhelpers::string::equalsci(method, "file"))
		{
			continue;
		}
		
		std::string type = tag->as_string("type");
		std::string level = tag->as_string("level");
		log_level loglevel = LOG_DEFAULT;
		
		if (stdhelpers::string::equalsci(level, "rawio"))
		{
			loglevel = LOG_RAW;
			Kernel->Config->RawLog = true;
		}
		else if (stdhelpers::string::equalsci(level, "debug"))
		{
			loglevel = LOG_DEBUG;
		}
		else if (stdhelpers::string::equalsci(level, "verbose"))
		{
			loglevel = LOG_VERBOSE;
		}
		else if (stdhelpers::string::equalsci(level, "default"))
		{
			loglevel = LOG_DEFAULT;
		}
		else if (stdhelpers::string::equalsci(level, "sparse"))
		{
			loglevel = LOG_MISC;
		}
		else if (stdhelpers::string::equalsci(level, "none"))
		{
			loglevel = LOG_NONE;
		}
		
		FileHandler* fw;
		std::string target = Kernel->Config->Paths.PrependLog(tag->as_string("target"));
		std::map<std::string, FileHandler*>::iterator fwi = logmap.find(target);
		
		if (fwi == logmap.end())
		{
			char realtarget[256];
			time_t time = Kernel->Now();
			struct tm *mytime = gmtime(&time);
			strftime(realtarget, sizeof(realtarget), target.c_str(), mytime);
			FILE* f = fopen(realtarget, "a");
			fw = new FileHandler(f);
			logmap.insert(std::make_pair(target, fw));
		}
		else
		{
			fw = fwi->second;
		}
		
		OutStream* fls = new OutStream(loglevel, fw);
		fls->WriteLog(LOG_MISC, "INIT", LogStream::StreamHead);
		AttachTypes(type, fls, true);
	}
}

void LogHandler::CloseLogs()
{
	ActiveStreams.clear();
	GeneralStreams.clear();

	for (std::map<LogStream*, int>::iterator i = AllStreams.begin(); i != AllStreams.end(); ++i)
	{
		delete i->first;
	}

	AllStreams.clear();
}

void LogHandler::AttachTypes(const std::string &types, LogStream* l, bool autoclose)
{
	engine::space_node_stream tlist(types);
	std::string token;
	std::vector<std::string> excludes;
	
	while (tlist.items_extract(token))
	{
		if (token.empty())
		{
			continue;
		}
		
		if (token.at(0) == '-')
		{
			
			excludes.push_back(token.substr(1));
		}
		else
		{
			AttachType(token, l, autoclose);
		}
	}

	for (std::vector<std::string>::iterator i = excludes.begin(); i != excludes.end(); ++i)
	{
		if (*i == "*")
		{
			DeleteStream(l);
			return;
		}
		
		DeleteType(*i, l);
	}

	std::map<LogStream *, std::vector<std::string> >::iterator gi = GeneralStreams.find(l);

	if (gi != GeneralStreams.end())
	{
		gi->second.swap(excludes); 
	}
}

bool LogHandler::AttachType(const std::string &type, LogStream *l, bool autoclose)
{
	ActiveStreams[type].push_back(l);

	if (type == "*")
	{
		GeneralStreams.insert(std::make_pair(l, std::vector<std::string>()));
	}

	if (autoclose)
	{
		AllStreams[l]++;
	}

	return true;
}

void LogHandler::DeleteStream(LogStream* l)
{
	for (std::map<std::string, std::vector<LogStream*> >::iterator i = ActiveStreams.begin(); i != ActiveStreams.end(); ++i)
	{
		while (stdhelpers::erase(i->second, l))
		{

		}
	}

	GeneralStreams.erase(l);

	std::map<LogStream*, int>::iterator ai = AllStreams.begin();

	if (ai == AllStreams.end())
	{
		return; 
	}

	delete ai->first;
	AllStreams.erase(ai);
}

bool LogHandler::DeleteType(const std::string &type, LogStream *l)
{
	std::map<std::string, std::vector<LogStream *> >::iterator i = ActiveStreams.find(type);

	if (type == "*")
	{
		GeneralStreams.erase(l);
	}

	if (i != ActiveStreams.end())
	{
		if (stdhelpers::erase(i->second, l))
		{
			if (i->second.size() == 0)
			{
				ActiveStreams.erase(i);
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	std::map<LogStream*, int>::iterator ai = AllStreams.find(l);

	if (ai == AllStreams.end())
	{
		return true;
	}

	if ((--ai->second) < 1)
	{
		AllStreams.erase(ai);
		delete l;
	}

	return true;
}

void LogHandler::Log(const std::string &type, log_level loglevel, const char *fmt, ...)
{
	if (Locked)
	{
		return;
	}

	std::string buf;
	SCHEME(buf, fmt, fmt);
	this->Log(type, loglevel, buf);
}

void LogHandler::Log(const std::string &type, log_level loglevel, const std::string &msg)
{
	if (Locked)
	{
		return;
	}

	Locked = true;

	for (std::map<LogStream *, std::vector<std::string> >::iterator gi = GeneralStreams.begin(); gi != GeneralStreams.end(); ++gi)
	{
		if (stdhelpers::isin(gi->second, type))
		{
			continue;
		}
		gi->first->WriteLog(loglevel, type, msg);
	}

	std::map<std::string, std::vector<LogStream *> >::iterator i = ActiveStreams.find(type);

	if (i != ActiveStreams.end())
	{
		for (std::vector<LogStream *>::iterator it = i->second.begin(); it != i->second.end(); ++it)
		{
			(*it)->WriteLog(loglevel, type, msg);
		}
	}

	Locked = false;
}

FileHandler::FileHandler(FILE* logfile) : log(logfile)
{

}

void FileHandler::AppendLine(const std::string &line)
{
	if (log == NULL)
	{
		return;
	}

	fputs(line.c_str(), log);
	fflush(log);
}

FileHandler::~FileHandler()
{
	if (log)
	{
		fflush(log);
		fclose(log);
		log = NULL;
	}
}

OutStream::OutStream(log_level loglevel, FileHandler *fwrite) : LogStream(loglevel), file(fwrite)
{
        Kernel->Logs->AddLogReference(fwrite);
}

OutStream::~OutStream()
{
        Kernel->Logs->RemoveLogReference(this->file);
}

void OutStream::WriteLog(log_level loglevel, const std::string &type, const std::string &text)
{
        static std::string TIMESTR;
        static time_t LAST = 0;

        if (loglevel < this->loglvl)
        {
                return;
        }

        if (Kernel->Now() != LAST)
        {
                TIMESTR = Daemon::HumanEpochTime(Kernel->Now());
                LAST = Kernel->Now();
        }

        this->file->AppendLine("[" + TIMESTR + "|" + type + "]: " + text + "\n");
}
