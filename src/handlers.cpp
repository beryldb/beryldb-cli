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

#include <iostream>

#include "emerald.h"
#include "exit.h"
#include "handlers.h"
#include "nodes.h"
#include "server.h"
#include "methods.h"
#include "utils.h"

std::vector<std::string> explode(std::string const & s, char delim)
{
      std::vector<std::string> result;
      std::istringstream iss(s);

      for (std::string token; std::getline(iss, token, delim); )
      {
          result.push_back(std::move(token));
      }

      return result;
}

Handlers::Handlers()
{

}

void Handlers::OnError(std::vector<std::string>& cmd, std::string& original)
{
    if (cmd.empty())
    {
        return;
    }
    
    bprint(INFO, Daemon::Format("Disconnected: %s", original.c_str()));
    printf("\x1b[0m\r");
    
    Kernel->Exit(EXIT_CODE_SOCKETSTREAM, false);
}

void Handlers::OnPart(std::vector<std::string>& cmd)
{   
        std::vector<std::string> vec = explode(cmd[0], '!');
        std::string chan = cmd[2];
        chan.erase(0, 1);
        chan.erase(std::remove(chan.begin(), chan.end(), '\n'), chan.end());

        std::string user;
        
        if (vec[0].size())
        {
                user = vec[0];
        }
        
        if (user == Kernel->myself)
        {
            Daemon::csprint(DTYPE_R, "%s|%s|%s> %s", Kernel->displayserver.c_str(), "PART", "You", chan.c_str());
        }
        else 
        {
            Daemon::csprint(DTYPE_R, "%s|%s|%s> %s", Kernel->displayserver.c_str(), "PART", user.c_str(), chan.c_str());
        }
}

void Handlers::OnQuit(std::vector<std::string>& cmd)
{
        std::vector<std::string> vec = explode(cmd[0], '!');
        std::string user = vec[0];
        std::string output = Kernel->displayserver + "|QUIT> " + user;
        Daemon::csprint(DTYPE_R, "%s", output.c_str());
}

std::string Handlers::OnUptime(std::string& original)
{
     return original;
}

void Handlers::OnPublish(std::vector<std::string>& cmd, std::string& original)
{
        std::vector<std::string> vec = explode(cmd[0], '!');
        std::string user = vec[0];
        std::string chan = cmd[2];

        std::string output = Kernel->displayserver;

        if (chan != Kernel->myself)
        {
            output += "|PUB|" + user + "|" + chan + "> " +  original;   
        }
        else
        {
        output += "|PUB|" + user + "> " +  original;   
        }

        Daemon::csprint(DTYPE_R, "%s", output.c_str());
}

void Handlers::OnSlist(std::vector<std::string>& cmd, std::string& original)
{
        std::vector<std::string> vec = explode(cmd[0], '!');
        std::string chan = cmd[4];
        chan.erase(0, 1);
        chan.erase(std::remove(chan.begin(), chan.end(), '\n'), chan.end());

        engine::space_node_stream CMD(original);
        std::string server;
        std::string users;

        unsigned int counter = 0;
        bool onemore = false;

        while (CMD.items_extract(server))
        {       
                users.append("[" + server + "]");
                users.append(" ");
                counter++;

                if (counter % 2 == 0)
                {
                     std::cout << Kernel->displayserver.c_str() << "|#" << chan << "> " << users;
                     printf("\x1b[0m\r\n");
                     onemore  = true;
                     users.clear();
                }
                else
                {
                     onemore = false;
                }
        }

        if (!onemore)
        {
             std::cout << Kernel->displayserver.c_str() << "|#" << chan << "> " << users;
             printf("\x1b[0m\r\n");
        }

        std::cout << Kernel->displayserver.c_str() << "|#" << chan << "|total|> " << counter;
        printf("\x1b[0m\r\n");
}

void Handlers::OnJoin(std::vector<std::string>& cmd)
{   
        std::vector<std::string> vec = explode(cmd[0], '!');
        std::string chan = cmd[2];
        chan.erase(0, 1);
        chan.erase(std::remove(chan.begin(), chan.end(), '\n'), chan.end());

        std::string user;

        if (vec[0].size())
        {
                user = vec[0];
        }

        if (user == Kernel->myself)
        {
            Daemon::csprint(DTYPE_R, "%s|%s|%s> %s", Kernel->displayserver.c_str(), "JOIN", "You", chan.c_str());
        }
        else 
        {
            Daemon::csprint(DTYPE_R, "%s|%s|%s> %s", Kernel->displayserver.c_str(), "JOIN", user.c_str(), chan.c_str());
        }
}


static bool InternalTest()
{
    int x = 500000;
    
    for (int i = 0; i < x; i++)
    {
         std::string to = "hello" + convto_string(i);
         Methods::Set(to, to);
         //Methods::Get(to);
    }
    
    return false;
}

void Handlers::Test()
{
    if (!InternalTest())
    {
        return;
    }

    slog("TESTS", LOG_DEFAULT, "Calling Handlers::Test()");
       
    int x = 50;

    Methods::Set("hello", "world");
    Methods::Set("hello2", "world");
    Methods::Set("hello3", "world");
    Methods::Set("hello4", "world!");
    
    for (int i = 0; i < x; i++)
    {
         /* Requests 100 random keys. */
        
         Methods::rkey();         
    }
    
    Methods::Get("hello4");
    
    Methods::HSet("hello", "pretty", "world");
    Methods::HGet("hello", "pretty");
        
   // Methods::Find("*");
}

void Handlers::OnMyUse(std::vector<std::string>& cmd)
{
     const std::string select = cmd[3];
     Kernel->select = select;
     
     /* Only display select if different than 1 */
     
     if (select != "1")
     {
             Kernel->display_select = true;
     }
     else
     {
             Kernel->display_select = false;
     }
     
     if (Kernel->Connected)
     {
         Daemon::sprint(DTYPE_R, "OK");             
     }
}

void Handlers::OnYourFlags(std::vector<std::string>& cmd)
{
     std::string mine = cmd[4].erase(0, 1);
     mine.erase(std::remove(mine.begin(), mine.end(), '\n'), mine.end());
     Kernel->myflags = mine;
    
     if (Kernel->Config->notifyflags)
     {
         Daemon::sprint(DTYPE_R, "Your flags are now: %s", mine.c_str());
    }
}

void Handlers::OnConnected(std::vector<std::string>& cmd)
{          
        Kernel->Connected = true;
        
        Kernel->server_time = convto_num<time_t>(cmd[4]);
        
        std::string version = cmd[3];
        Kernel->server_version = version.erase(0, 1);
        
        if (!Kernel->Config->select.empty() && Kernel->Config->select != "1")
        {
                /* Nothing to do. */
        }
        else
        {
             Kernel->select = "1";
             Kernel->display_select = false;
        }
        
        std::string server = cmd[0];
        Kernel->realserver = server;
        Kernel->myself = cmd[2];
        
        if (!Kernel->Config->customdisplay.empty())
        {
            Kernel->displayserver = Kernel->Config->customdisplay;
        }
        else
        {
            if (Kernel->Config->changeserv)
            {
                 Kernel->displayserver = server + ":" + Kernel->Config->port;
            }
            else
            {
                 Kernel->displayserver = Kernel->Config->host;
            }
        }
        
        if (Kernel->Config->usercmd.do_tests)
        {
             Handlers::Test();
        }
}

void Handlers::MyInfo(std::vector<std::string>& cmd)
{
    Kernel->myself = cmd[2];
}

void Handlers::OnWrongPass()
{
    slog("STARTUP", LOG_DEFAULT, "%s: Incorrect login.", Kernel->Config->host.c_str());

    bprint(ERROR, "%s: Incorrect login.", Kernel->Config->host.c_str());    
    printf("\x1b[0m\r");

    /* User has been disconnected at this point, we may exit. */
    
    Kernel->Exit();
}

std::string Handlers::OnNotFnd(std::vector<std::string>& cmd)
{
    Daemon::sprint(DTYPE_R, "%s is not a command", cmd[3].c_str());
    return "";   
}

void Handlers::Local(std::string& buffer)
{
        const std::string input(buffer);
        engine::space_node_stream CMD(input);
        int total = CMD.count();
        int counter = 0;
        std::string server;
        std::string first;
        std::string remaining;
        
        while (CMD.items_extract(server))
        {
            if (counter == 0)
            {
                first = server;
        }
        else
        {
            remaining.append(server);
        
            if (total != counter)
            {
                remaining.append(" ");
            }
        }

        counter++;
        }
        
    Daemon::sprint(DTYPE_R, "/%s", input.c_str());

    if (first.compare("me") == 0)
    {
            Daemon::sprint(DTYPE_R, "Your instance ID: %s", Kernel->myself.c_str());
            
    }
    else if (first.compare("test") == 0)
    {
            Handlers::Test();
    }
    else if (first.compare("version") == 0)
    {
	  Daemon::sprint(DTYPE_R, "%s", convto_string(VERSION).c_str());    
    }
    else if (first.compare("rh") == 0)
    {
            std::remove(HIST_FILE);
            Kernel->Link->ResetCache();
            
            Daemon::sprint(DTYPE_R, "History file removed.");
    }
    else if (first.compare("clear") == 0)
    {
             Server::ClearScreen();   
    }
    else if (first.compare("hc") == 0)
    {
            std::remove(HIST_FILE);
            Daemon::sprint(DTYPE_R, "History items: %d", Kernel->Link->CountHistory());
            Daemon::sprint(DTYPE_R, "History file removed.");
    }
    
    else if (first.compare("flags") == 0)
    {
            Daemon::sprint(DTYPE_R, "Your flags: %s", Kernel->myflags.c_str());
    }
#ifndef _WIN32
    else if (first.compare("exec") == 0)
    {
            std::string response = Utils::GetStdoutFromCommand(remaining.c_str());
            engine::newline_node_stream resp(response);

            while (resp.items_extract(server))
            {
                std::cout << server << std::endl;
                printf("\x1b[0m\r");
            }   
    }
    else if (first.compare("execsend") == 0)
    {
            std::string response = Utils::GetStdoutFromCommand(remaining.c_str());
            engine::newline_node_stream resp(response);

            while (resp.items_extract(server))
            {
                    Daemon::sprint(DTYPE_R, "%s", server.c_str());
                    Server::raw("%s\r\n", server.c_str());
            }   
    }
#endif
    else 
    {
        Daemon::sprint(DTYPE_R, "Uknown local command: %s", input.c_str());
    }
}