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
#include "server.h"

void Methods::rkey()
{
        Server::raw("rkey\r\n");
}

void Methods::Set(const std::string& key, const std::string& value)
{
        Server::raw("set %s \"%s\"\r\n", key.c_str(), value.c_str());
}

void Methods::HSet(const std::string& key, const std::string& hesh, const std::string& value)
{
        Server::raw("hset %s %s \"%s\"\r\n", key.c_str(), hesh.c_str(), value.c_str());
}

void Methods::HGet(const std::string& key, const std::string& hesh)
{
        Server::raw("hget %s %s\r\n", key.c_str(), hesh.c_str());
}

void Methods::Get(const std::string& key)
{
        Server::raw("get %s\r\n", key.c_str());
}

void Methods::LPush(const std::string& key, const std::string& value)
{
        Server::raw("lpush %s \"%s\"\r\n", key.c_str(), value.c_str());
}

void Methods::LGet(const std::string& key)
{
        Server::raw("lget %s\r\n", key.c_str());
}

void Methods::Find(const std::string& key)
{
        Server::raw("find %s\r\n", key.c_str());
}

void Methods::use(const std::string& value)
{
        Server::raw("use %s\r\n", value.c_str());
}

void Methods::LogIn(const std::string& session, const std::string& login, const std::string& pass)
{
        if (!Kernel->Config->select.empty() && Kernel->Config->select != "1")
        {
             Methods::use(Kernel->Config->select);
        }

        Server::raw("agent %s\r\n", session.c_str());
        Server::raw("auth %s\r\n", pass.c_str());
        Server::raw("login %s\r\n", login.c_str());
}

void Methods::Command(const std::string& cmd)
{
    Server::raw("%s \r\n", cmd.c_str());
}

void Methods::Publish(const std::string& dest, const std::string& text)
{
    Server::raw("PUBLISH %s :%s\r\n", dest.c_str(), text.c_str());
}

void Methods::Join(const std::string& chan)
{
    engine::space_node_stream CMD(chan);
    std::string server;
    
    while (CMD.items_extract(server))
    {
        if (server.at(0) == ' ')
        {
            continue;
        }
        
        if (server.at(0) != '#')
        {
              server = "#" + server;
        } 
        
        Server::raw("JOIN %s\r\n", server.c_str());   
    }    
}

void Methods::Part(const std::string& chan)
{
    engine::space_node_stream CMD(chan);
    std::string server;
    
    while (CMD.items_extract(server))
    {
        if (server.at(0) == ' ')
        {
            continue;
        }
        
        if (server.at(0) != '#')
        {
              server = "#" + server;
        } 

        Server::raw("PART %s\r\n", server.c_str());   
    }
}
