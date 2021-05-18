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
#include "stackconf.h"
#include "configprocess.h"
#include "exit.h"

Configuration::ServerPaths::ServerPaths(config_rule* tag)
        : Config(tag->as_string("configdir", CONFIG_PATH, 1))
        , Runtime(tag->as_string("runtimedir", RUNTIME_PATH, 1))
        , Log(tag->as_string("logdir", LOG_PATH, 1))
        
{

}


static config_rule* crate_empty_rule()
{
        file_config_items* items;
        return config_rule::create("empty", "<auto>", 0, items);
}



Configuration::Configuration() : EmptyTag(crate_empty_rule())
                               , Paths(EmptyTag)
                               , RawLog(false)
{

}

config_rule* Configuration::GetConf(const std::string &tag)
{
        MultiTag found = config_data.equal_range(tag);
        
        if (found.first == found.second)
        {
                return EmptyTag;
        }
        
        config_rule* rv = found.first->second;
        found.first++;
        
        if (found.first != found.second)
        {

        }
        
        return rv;
}


void Configuration::Load()
{
        parse_stack stack(this);
        
        try
        {
                this->Ready = stack.process_conf_buffer(Kernel->GetConfigFile(), 0);
        }
        catch (KernelException& err)
        {
                this->Ready = false;
                errstr << err.get_reason() << std::endl;
        }
}

void Configuration::SetAll()
{
        this->Ready = true;

        try
        {
                Fill();

        }
        catch (KernelException &ce)
        {
                errstr << ce.get_reason() << std::endl;
        }
        
        this->Ready = errstr.str().empty();

        if (!this->Ready)
        {
                bprint(ERROR, "Errors found in your configuration file.");
        }
        
        while (errstr.good())
        {
                std::string line;
                getline(errstr, line, '\n');

                if (line.empty())
                {
                        continue;
                }

                bprint(ERROR, "%s", line.c_str());
        }

        errstr.clear();
        errstr.str(std::string());

        if (!this->Ready)
        {
                Kernel->Exit(EXIT_CODE_CONFIG);
        }

        if (!this->Ready)
        {
                return;
        }
}


void Configuration::CheckCMD()
{
        if (!Kernel->Config->usercmd.login.empty())
        {
           bprint(INFO, "Using login: %s", Kernel->Config->usercmd.login.c_str());
           Kernel->Config->login = Kernel->Config->usercmd.login;
           
           if (!Kernel->Engine->ValidLogin(login))
           {
                bprint(ERROR, "Invalid login: Must be between 3 and 15 characters.");
                Kernel->Exit(EXIT_CODE_CONFIG, true);
           }
           
        }

        if (!Kernel->Config->usercmd.pass.empty())
        {
           bprint(INFO, "Using pass: %s", Kernel->Config->usercmd.pass.c_str());
           Kernel->Config->pass = Kernel->Config->usercmd.pass;

       if (Kernel->Config->usercmd.pass.length() < 3 || Kernel->Config->usercmd.pass.length() > 30)
       {
            bprint(ERROR, "Invalid password: Must be between 3 and 30 characters.");
            Kernel->Exit(EXIT_CODE_CONFIG, true);
       }
           

        }
                
        if (!Kernel->Config->usercmd.host.empty())
        {       
               bprint(INFO, "Using host: %s", Kernel->Config->usercmd.host.c_str());
                Kernel->Config->host = Kernel->Config->usercmd.host;
        }
        
        if (!Kernel->Config->usercmd.port.empty())
        {
               bprint(INFO, "Using port: %s", Kernel->Config->usercmd.port.c_str());
               Kernel->Config->port = Kernel->Config->usercmd.port;
                
               int intport = convto_num<int>(Kernel->Config->usercmd.port);

               if (intport < 0 || intport > 65353)
               {
                   bprint(ERROR, "Invalid port.");
                   Kernel->Exit(EXIT_CODE_CONFIG, true);
               }
               
        }

}


void Configuration::Fill()
{
       int select_usig = GetConf("select")->as_int("use", 1);
       
       if (select_usig < 1 || select_usig > 100)
       {
             bprint(ERROR, "use tag must be a value between 1 and 100");
             Kernel->Exit(EXIT_CODE_CONFIG, true);
       }
       
       select = convto_string(select_usig);
       
       config_rule* connect = GetConf("connect");
       
       login = connect->as_string("login", "root");
       
       if (!Kernel->Engine->ValidLogin(login))
       {
            bprint(ERROR, "Invalid login: Must be between 3 and 15 characters.");
            Kernel->Exit(EXIT_CODE_CONFIG, true);
       }
       
       host = connect->as_string("host", "localhost");
       pass = connect->as_string("pass", "default");
 
       if (pass.length() < 3 || pass.length() > 30)
       {
            bprint(ERROR, "Invalid password: Must be between 3 and 30 characters.");
            Kernel->Exit(EXIT_CODE_CONFIG, true);
       }
       
       int intport = connect->as_int("port", 6378);
       
       if (intport < 0 || intport > 65353)
       {
             bprint(ERROR, "Invalid port.");
             Kernel->Exit(EXIT_CODE_CONFIG, true);
       }

       port = convto_string(intport);
       
       config_rule* settings = GetConf("settings");
       changeserv = settings->as_bool("changeserv", true);
       customdisplay = settings->as_string("customdisplay", "");
       notifyflags = settings->as_bool("notifyflags", true);
       
       clear = settings->as_bool("clear", false);

       CheckCMD();

}


MultiTag Configuration::GetTags(const std::string& tag)
{
        return config_data.equal_range(tag);
}

