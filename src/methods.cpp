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

void Methods::Future(const unsigned int seconds, const std::string& key, const std::string& value)
{
       Server::SendData("future %s %s \"%s\"\r\n", convto_string(seconds).c_str(), key.c_str(), value.c_str());
}

void Methods::rkey()
{
        Server::SendData("rkey\r\n");
}

void Methods::Set(const std::string& key, const std::string& value)
{
        Server::SendData("set %s \"%s\"\r\n", key.c_str(), value.c_str());
}

void Methods::HSet(const std::string& key, const std::string& hesh, const std::string& value)
{
        Server::SendData("hset %s %s \"%s\"\r\n", key.c_str(), hesh.c_str(), value.c_str());
}

void Methods::HGet(const std::string& key, const std::string& hesh)
{
        Server::SendData("hget %s %s\r\n", key.c_str(), hesh.c_str());
}

void Methods::Get(const std::string& key)
{
        Server::SendData("get %s\r\n", key.c_str());
}

void Methods::LPush(const std::string& key, const std::string& value)
{
        Server::SendData("lpush %s \"%s\"\r\n", key.c_str(), value.c_str());
}

void Methods::LGet(const std::string& key)
{
        Server::SendData("lget %s\r\n", key.c_str());
}

void Methods::Find(const std::string& key)
{
        Server::SendData("find %s\r\n", key.c_str());
}

void Methods::use(const std::string& value)
{
        Server::SendData("use %s\r\n", value.c_str());
}

void Methods::LogIn(const std::string& session, const std::string& login, const std::string& pass)
{
        if (!Kernel->Config->select.empty() && Kernel->Config->select != "1")
        {
             Methods::use(Kernel->Config->select);
        }

        Server::SendData("agent %s\r\n", session.c_str());
        Server::SendData("auth %s\r\n", pass.c_str());
        Server::SendData("login %s\r\n", login.c_str());
}

void Methods::Command(const std::string& cmd)
{
    Server::SendData("%s \r\n", cmd.c_str());
}

void Methods::Publish(const std::string& dest, const std::string& text)
{
    Server::SendData("PUBLISH %s :%s\r\n", dest.c_str(), text.c_str());
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
        
        Server::SendData("JOIN %s\r\n", server.c_str());   
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

        Server::SendData("PART %s\r\n", server.c_str());   
    }
}
