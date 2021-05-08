
#pragma once

class Methods
{
    public:
        
        /* Quits server. */
        
        static void Quit();
        
        static void Command(const std::string& cmd);

        static void Publish(const std::string& dest, const std::string& text);

        static void Join(const std::string& chan);

        static void Part(const std::string& chan);

        static void LogIn(const std::string& session, const std::string& login, const std::string& pass);

        static void use(const std::string& value);

        static void rkey();

        static void Set(const std::string& key, const std::string& value);

        static void HSet(const std::string& key, const std::string& hesh, const std::string& value);

        static void HGet(const std::string& key, const std::string& hesh);
        
        static void Get(const std::string& key);

        static void LPush(const std::string& key, const std::string& value);

        static void LGet(const std::string& key);

        static void Find(const std::string& key);
        
};