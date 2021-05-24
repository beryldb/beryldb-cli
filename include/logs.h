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

#pragma once

enum log_level
{
	LOG_RAW   = 5,
	LOG_DEBUG   = 10,
	LOG_VERBOSE = 20,
	LOG_DEFAULT = 30,
	LOG_MISC  = 40,
	LOG_NONE    = 50
};

class FileHandler
{
  protected:
		
  	FILE* log;

  public:
	
	/* Constructor for a log file. */
		
	FileHandler(FILE* logfile);

	/* Appends a line to a log file. */
		
	void AppendLine(const std::string &line);

		
	virtual ~FileHandler();
};

class LogStream
{
 protected:

	log_level loglvl;

 public:

	/* A log stream header. */

	static const std::string StreamHead;

	LogStream(log_level loglevel) : loglvl(loglevel)
	{
	
	}

	virtual ~LogStream() 
	{ 
	
	}

	/* Changes log level. */
	
	void ChangeLevel(log_level lvl) 
	{ 
		this->loglvl = lvl; 
	}

	virtual void WriteLog(log_level loglevel, const std::string &type, const std::string &msg) = 0;
};

typedef std::map<FileHandler*, int> FileLogMap;

class LogHandler : public safecast<LogHandler>
{
 private:

	/* Current activity. */

	bool Locked;


	std::map<std::string, std::vector<LogStream *> > ActiveStreams;


	std::map<LogStream *, int> AllStreams;


	std::map<LogStream *, std::vector<std::string> > GeneralStreams;

	/* Map of active file logs. */

	FileLogMap FileLogs;

 public:

	/* Constructor. */

	LogHandler();

	/* Destructor. */
	
	~LogHandler();

	void AddLogReference(FileHandler* fw)
	{
		FileLogMap::iterator i = FileLogs.find(fw);
		if (i == FileLogs.end())
		{
			FileLogs.insert(std::make_pair(fw, 1));
		}
		else
		{
			++i->second;
		}
	}

	void RemoveLogReference(FileHandler* fw)
	{
			FileLogMap::iterator i = FileLogs.find(fw);

			if (i == FileLogs.end()) 
			{
				return;
			} 
			
			if (--i->second < 1)
			{
				delete i->first;
				FileLogs.erase(i);
			}
	}

	/* Opens log files from configuration file */
	
	void OpenLogs();

	/* Closes all logs. */
	
	void CloseLogs();

	void AttachTypes(const std::string &type, LogStream *l, bool autoclose);

	bool AttachType(const std::string &type, LogStream *l, bool autoclose);

	void DeleteStream(LogStream* l);

	bool DeleteType(const std::string &type, LogStream *l);

	void Log(const std::string &type, log_level loglevel, const std::string &msg);

	void Log(const std::string &type, log_level loglevel, const char *fmt, ...) BERYL_PRINTF(4, 5);
};

class OutStream : public LogStream
{
   private:
   
        FileHandler *file;

   public:
        
        OutStream(log_level loglevel, FileHandler *fwrite);

        virtual ~OutStream();

        void WriteLog(log_level loglevel, const std::string& type, const std::string& msg);
};


