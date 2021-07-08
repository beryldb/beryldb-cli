/*
 * Emerald - A POSIX client for BerylDB.
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
#include "methods.h"
#include "engine.h"
#include "exit.h"

std::unique_ptr<Emerald> Kernel = nullptr;

EmeraldStart
{
        Kernel = std::make_unique<Emerald>(argc, argv);
        return 1;
}

Emerald::Emerald(int argc, char** argv) : ConfigFile(DEFAULT_CONFIG)
{        
        Kernel = std::unique_ptr<Emerald>(this);
        
        /* Updates time. */
        
        this->Refresh();
        
        /* Startup time. */
        
        this->startup = this->Now();
        
        /* Configuration init. */
        
        this->Config = std::make_unique<Configuration>();

        /* We must verify if a provided file actually exists. */
        
        if (!Daemon::ConfigFileExists(this->ConfigFile))
        {
                bprint(ERROR, "Unable to open config file: %s", Daemon::Welcome(this->ConfigFile).c_str());
                this->Exit(EXIT_CODE_CONFIG, true);
        }

        /* Checks if we are running with a root */
        
        Daemon::CheckRoot();
        
        /* Keep a copy of arguments from main() */

        this->Config->usercmd.argv = argv;

        /* We must pass both, argv and argc */

        this->Config->usercmd.argc = argc;
        
        /* Reads command line. */
        
        this->CommandLine();

        /* Load configuration file. */

        this->Config->Load();
        
        /* Sets configuration vars */
        
        this->Config->SetAll();

        /* Opens all logs as provided in the configuration file. */
        
        this->Logs->OpenLogs();
        
        /* We are not connected by default. */
        
        this->Connected = false;
       
        /* Clear screen on startup. */
         
        if (this->Config->clear)
        {
                Link->ClearScreen();
        }

        Emerald::Signalizers();

        /* Inits client connection. */
        
        Link->Initialize();
}

void Emerald::Exit(int code, bool skip, const std::string& exitmsg)
{
        this->Link->Reset();
        this->Link->Shutdown();
        
        /* If clear option was provided, we must clean before exiting. */
        
        if (this->Config->clear)
        {
                Link->ClearScreen();
        }

        if (!skip)
        {
                realserver.clear();
                displayserver.clear();
                this->Link->HistoryWrite();
                this->Link->ResetCache();
                this->Link->PrepareExit();
        }
        
        if (exitmsg.empty())
        {
                slog("EXIT", LOG_DEFAULT, "Exiting: %s", ExitMap[code]);
        }
        else
        {
                slog("EXIT", LOG_DEFAULT, "Exiting: %s", exitmsg.c_str());
        }

        /* Configuration class is not needed anymore, so we reset it. */

        this->Config.reset();

        /* Close logs. */
        
        this->Logs->CloseLogs();

        /* Cleans up config file. */

        this->ConfigFile.clear();
        
        this->displayserver.clear();

        this->Config = nullptr;

        memset(&this->TIME, 0, sizeof(timespec));

        memset(&this->startup, 0, sizeof(time_t));

        Kernel = nullptr;

        exit(code);
}

void Emerald::Signalizers()
{
        signal(SIGALRM, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);
        signal(SIGPIPE, SIG_IGN);
        signal(SIGXFSZ, SIG_IGN);
        signal(SIGINT, Emerald::Signalizer);
        signal(SIGTERM, Emerald::Signalizer);
        signal(SIGKILL, Emerald::Signalizer);
}

void Emerald::SignalManager(int signal)
{
        if (signal == SIGTERM)
        {
                Kernel->Link->QuickExit();
        }
        else if (signal == SIGINT || signal == SIGKILL)
        {
                Kernel->Link->QuickExit();
        }
}
