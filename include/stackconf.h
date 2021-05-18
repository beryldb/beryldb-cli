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

#pragma once

struct parse_stack
{
        std::vector<std::string> streaming;
        brld::flat_map<std::string, std::string, engine::insensitive_swo> input;
        ConfigMap& output;
        cached_config& file_content;
        std::stringstream& error_msg;

        parse_stack(Configuration* conf) : output(conf->config_data), file_content(conf->Files), error_msg(conf->errstr)
        {
                input["newline"] = "\n";
                input["nl"]      = "\n";

                input["amp"]  = "&";
                input["apos"] = "'";
                input["gt"]   = ">";
                input["lt"]   = "<";
                input["quot"] = "\"";
        }

        bool process_conf_buffer(const std::string& name, int flags, const std::string& required_tag = std::string(), bool isexec = false);

        void include_file(config_rule* includeTag, int flags);

        void load_file_tobuff(const std::string& key, const std::string& file, int flags, bool exec);
};

