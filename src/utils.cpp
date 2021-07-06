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

#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <iomanip>

#include "emerald.h"
#include "utils.h"
#include "converter.h"
#include "exit.h"

static void ShowHelp()
{
        std::cout << std::endl;

        printf("Usage: ./beryl-cli <arguments>\n\n");

        printf ("%4s %-28s %29s\n", "", "--login <username>,", "Login with a different username.");
        printf ("%4s %-28s %29s\n", "", "--host <host>,", "Connect to provided host.");
        printf ("%4s %-28s %29s\n", "", "--password <password>,", "Use a different password than default.");
        printf ("%4s %-28s %29s\n", "", "--port <port>,", "Use a differnt port than 6378");
        printf ("%4s %-28s %29s\n", "", "--use <use>,", "Use a different select than 1.");
        printf ("%4s %-28s %29s\n", "", "--join <channels>,", "Join channels upon connection.");
        printf ("%4s %-28s %19s\n", "", "--test,", "Run tests and exit.");
        printf ("%4s %-28s %19s\n", "", "--version,", "Display version and exit.");
        printf("\n");
        Kernel->Exit(EXIT_CODE_ARGV, true);
}

const char* ExitMap[] =
{
        "No error",
        "Error with Emerald.",
        "Config file error",
        "Logfile error",
        "Bad commandline parameters",
        "Problem with SocketPool",
        "Refusing to start up as root",
        "Received SIGTERM",
        "SIG INT received."
};

void Emerald::CommandLine()
{
        int do_version = 0, do_debug = 0;
        int do_tests = 0;
        
        /* Stands for remove history. */
        
        int do_rhist = 0;
        
        struct option longopts[] =
        {
                        { "version",  no_argument,       &do_version,    1 },
                        { "debug",    no_argument,       &do_debug,     1   },
                        { "port",     required_argument, NULL,   	'p' },
                        { "host",     required_argument, NULL,   	'h' },
                        { "pass",     required_argument, NULL,   	'd'},
                        { "login",    required_argument, NULL,          'l'},
                        { "use",      required_argument, NULL,          'u'},
                        { "join",     required_argument, NULL,          'j'},
                        { "test",     no_argument,       &do_tests,     't'},
                        { "rhist",    no_argument,       &do_rhist, 	'r' },
                        { 0, 0, 0, 0 }
        };
        
        char** argv = this->Config->usercmd.argv;
        int value;

        while ((value = getopt_long(this->Config->usercmd.argc, argv, ":p:h:d:l:u:j:t:r", longopts, NULL)) != -1)
        {
                      switch (value)
                      {
                           case 0:
                       
                                  break;
                                  
                           case 'h':
                           
                                  Kernel->Config->usercmd.host = optarg;
                                  break;
                              
                           case 'p':
                              
                                  Kernel->Config->usercmd.port = optarg;
                                  break;
                                  
                           case 'd':
                                  
                                  Kernel->Config->usercmd.pass = optarg;
                                  break;
                                
                          case 'l':
                                
                                  Kernel->Config->usercmd.login = optarg;
                                  break;
                                  
                          case 'j':

                                  Kernel->Config->usercmd.join = optarg;
                                  break;
                                  
                                  
                          case 'u':
                                         
                                  Kernel->Config->select = optarg;
                                  break;
                                
                           default:
                                
                                 ShowHelp();
                      }       
        }
        
        if  (!Kernel->Config->usercmd.join.empty())
        {
               engine::comma_node_stream CMD(Kernel->Config->usercmd.join);
               std::string server;
               
               unsigned int counter = 0;
               
               while (CMD.items_extract(server))
               {
                     if (!Kernel->Engine->ValidChannel(server))
                     {
                            bprint(ERROR, "Invalid channel: %s", server.c_str());
                            Kernel->Exit(EXIT_CODE_CONFIG, true);
                     }
                     
                     counter++;
              }
              
              bprint(INFO, "Joining %u channels.", counter);
        }
        
        if (do_rhist)
        {
                if (FileSystem::Exists(HIST_FILE))
                {
                        std::remove(HIST_FILE);
                        bprint(DONE, "History removed.");
                }
                else
                {
                        bprint(ERROR, "No history found.");
                }
                
                Kernel->Exit(EXIT_CODE_CONFIG, true);
        }
        
        if (do_tests)
        {
                Kernel->Config->usercmd.do_tests = true;
        }
        else
        {
                Kernel->Config->usercmd.do_tests = false;
        }
        
        if (do_version)
        {
              std::cout << VERSION << std::endl;
              Kernel->Exit(EXIT_CODE_OK, true);        
        }
        
        this->Config->usercmd.forcedebug = !!do_debug;
}

void Emerald::Refresh()
{

#if defined HAS_CLOCK_GETTIME

        clock_gettime(CLOCK_REALTIME, &TIME);

#else

        struct timeval tv;
        gettimeofday(&tv, NULL);

        TIME.tv_sec = tv.tv_sec;
        TIME.tv_nsec = tv.tv_usec * 1000;

#endif
}

std::string Utils::GenRandom(const int len) 
{
        
        std::string tmp_s;
        static const char alphanum[] = "0123456789"
                                       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                       "abcdefghijklmnopqrstuvwxyz";
        
        srand( (unsigned) time(NULL) * getpid());

        tmp_s.reserve(len);

        for (int i = 0; i < len; ++i) 
        {
            tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        
        return tmp_s;
}

std::string Utils::GetStdoutFromCommand(std::string cmd) 
{
        cmd = "echo \"$(timeout 2 " + cmd + ")\"";

        std::string data;
        FILE * stream;
        const int max_buffer = 256;
        char buffer[max_buffer];
        cmd.append(" 2>&1");

        stream = popen(cmd.c_str(), "r");

        if (stream) 
        {
            while (!feof(stream))

            if (fgets(buffer, max_buffer, stream) != NULL) 
            {
                        data.append(buffer);
            }

            pclose(stream);
        }

        return data;
}

