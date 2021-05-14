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

#include <fstream>

#include "emerald.h"
#include "stackconf.h"
#include "exit.h"

enum parse_options
{
        OPTION_USE_COMPAT = 1,

        OPTION_NO_EXECUTE = 2,

        OPTION_NO_INCLUDE = 4,

        OPTION_NO_ENV = 8
};

struct file_position
{
        std::string name;

        unsigned int line;

        unsigned int column;

        file_position(const std::string& Name) : name(Name), line(1), column(1)
        {

        }

        std::string str()
        {
                return name + ":" + convto_string(line) + ":" + convto_string(column);
        }
};

class FileStream
{
  private:

        bool close_with_pclose;

        FILE* const file;

  public:
  
        FileStream(FILE* File, bool CloseWithPClose = false) : close_with_pclose(CloseWithPClose), file(File)
        {

        }

        operator bool() { return (file != NULL); }

        operator FILE*() 
        { 
                        return file; 
        }

        ~FileStream()
        {
                if (!file)
                {
                        return;
                }

                if (close_with_pclose)
                {
                        pclose(file);
                }
                else
                {
                        fclose(file);
                }
        }
};


struct ConfFile
{
	parse_stack& stack;
	int flags;
	FILE* const file;
	file_position current;
	file_position last_tag;
	reference<config_rule> tag;
	int ungot;
	std::string required_tag;

	ConfFile(parse_stack& me, int myflags, FILE* conf, const std::string& name, const std::string& must_have_tag)
		: stack(me), flags(myflags), file(conf), current(name), last_tag(name), ungot(-1), required_tag(must_have_tag)
	{ 

	}

	int next(bool eof_ok = false)
	{
		if (ungot != -1)
		{
			int ch = ungot;
			ungot = -1;
			return ch;
		}

		int ch = fgetc(file);

		if (ch == EOF && !eof_ok)
		{
			throw KernelException("Unexpected end-of-file");
		}
		else if (ch == '\n')
		{
			current.line++;
			current.column = 0;
		}
		else
		{
			current.column++;
		}
		
		return ch;
	}

	void unget(int ch)
	{
		if (ungot != -1)
		{
			throw KernelException("INTERNAL ERROR: cannot unget twice");
		}
		
		ungot = ch;
	}

	void process_comment()
	{
		while (1)
		{
			int ch = next();

			if (ch == '\n')
			{
				return;
			}
		}
	}

	bool wordchar(char ch)
	{
		return isalnum(ch)
			|| ch == '-'
			|| ch == '.'
			|| ch == '_';
	}

	void iternext(std::string& rv)
	{
		int ch = next();
		while (isspace(ch))
			ch = next();
		while (wordchar(ch))
		{
			rv.push_back(ch);
			ch = next();
		}
		
		unget(ch);
	}

	bool kv(file_config_items* items)
	{
		std::string key;
		iternext(key);
		int ch = next();

		if (ch == '>' && key.empty())
		{
			return false;
		}
		else if (ch == '#' && key.empty())
		{
			process_comment();
			return true;
		}
		else if (ch != '=')
		{
			throw KernelException("Invalid character " + std::string(1, ch) + " in key (" + key + ")");
		}

		std::string value;
		ch = next();

		if (ch != '"')
		{
			throw KernelException("Invalid character in value of <" + tag->tag + ":" + key + ">");
		}
		while (1)
		{
			ch = next();

			if (ch == '&' && !(flags & OPTION_USE_COMPAT))
			{
				std::string tag_name;
				
				while (1)
				{
					ch = next();
					if (wordchar(ch) || (tag_name.empty() && ch == '#'))
					{
						tag_name.push_back(ch);
					}
					else if (ch == ';')
					{
						break;
					}
					else
					{
						stack.error_msg << "Invalid XML entity name in value of <" + tag->tag + ":" + key + ">\n"
							<< "To include an ampersand or quote, use &amp; or &quot;\n";
						throw KernelException("Parse error");
					}
				}

				if (tag_name.empty())
					throw KernelException("Empty XML entity reference");
				else if (tag_name[0] == '#' && (tag_name.size() == 1 || (tag_name.size() == 2 && tag_name[1] == 'x')))
					throw KernelException("Empty numeric character reference");
				else if (tag_name[0] == '#')
				{
					const char* ctag_name = tag_name.c_str();
					char* endptr;
					unsigned long lvalue;
					if (ctag_name[1] == 'x')
						lvalue = strtoul(ctag_name + 2, &endptr, 16);
					else
						lvalue = strtoul(ctag_name + 1, &endptr, 10);
					if (*endptr != '\0' || lvalue > 255)
						throw KernelException("Invalid numeric character reference '&" + tag_name + ";'");
					value.push_back(static_cast<char>(lvalue));
				}
				else if (tag_name.compare(0, 4, "env.") == 0)
				{
					if (flags & OPTION_NO_ENV)
					{
						throw KernelException("XML environment entity reference in file included with noenv=\"yes\"");
					}

					const char* envstr = getenv(tag_name.c_str() + 4);

					if (!envstr)
					{
						throw KernelException("XML environment entity reference '&" + tag_name + ";'");
					}

					value.append(envstr);
				}
				else
				{
					brld::flat_map<std::string, std::string>::iterator var = stack.input.find(tag_name);

					if (var == stack.input.end())
					{
						throw KernelException("XML entity reference '&" + tag_name + ";'");
					}
					
					value.append(var->second);
				}
			}
			else if (ch == '\\' && (flags & OPTION_USE_COMPAT))
			{
				int esc = next();
				if (esc == 'n')
					value.push_back('\n');
				else if (isalpha(esc))
					throw KernelException("Unknown character \\" + std::string(1, esc));
				else
					value.push_back(esc);
			}
			else if (ch == '"')
			{
				break;
			}
			else if (ch != '\r')
			{
				value.push_back(ch);
			}
		}

		if (items->find(key) != items->end())
		{
			throw KernelException("Key '" + key + "' found twice.");
		}

		(*items)[key] = value;
		return true;
	}

	void real_parse()
	{
		last_tag = current;
		std::string name;
		iternext(name);

		int spc = next();

		if (spc == '>')
		{
			unget(spc);
		}
		else if (!isspace(spc))
		{
			throw KernelException("Tag contains an invalid character.");
		}

		if (name.empty())
		{
			throw KernelException("Empty tag name");
		}

		file_config_items* items;
		tag = config_rule::create(name, current.name, current.line, items);

		while (kv(items))
		{
		}

		if (name == required_tag)
		{
			required_tag.clear();
		}

		if (stdhelpers::string::equalsci(name, "include"))
		{
			stack.include_file(tag, flags);
		}
		else if (stdhelpers::string::equalsci(name, "files"))
		{
			for(file_config_items::iterator i = items->begin(); i != items->end(); i++)
			{
				stack.load_file_tobuff(i->first, i->second, flags, false);
			}
		}
		else if (stdhelpers::string::equalsci(name, "execfiles"))
		{
			for(file_config_items::iterator i = items->begin(); i != items->end(); i++)
			{
				stack.load_file_tobuff(i->first, i->second, flags, true);
			}
		}
		else if (stdhelpers::string::equalsci(name, "define"))
		{
			if (flags & OPTION_USE_COMPAT)
			{
				throw KernelException("<define> tags may only be used in XML format (add <config format=\"xml\">)");
			}

			std::string tag_name = tag->as_string("name");
			std::string value = tag->as_string("value");

			if (tag_name.empty())
			{
				throw KernelException("Variable define must include variable name");
			}

			stack.input[tag_name] = value;
		}
		else if (stdhelpers::string::equalsci(name, "config"))
		{
			std::string format = tag->as_string("format");

			if (stdhelpers::string::equalsci(format, "xml"))
			{
				flags &= ~OPTION_USE_COMPAT;
			}
			else if (stdhelpers::string::equalsci(format, "compat"))
			{
				flags |= OPTION_USE_COMPAT;
			}
			else if (!format.empty())
			{
				throw KernelException("Unknown configuration format " + format);
			}
		}
		else
		{
			stack.output.insert(std::make_pair(name, tag));
		}
		tag = NULL;
	}

	bool out_tag_parse()
	{
		try
		{
			while (1)
			{
				int ch = next(true);

				switch (ch)
				{
					case EOF:

						if (!required_tag.empty())
						{
							throw KernelException("Required tag \"" + required_tag + "\" not found.");
						}

						return true;
					case '#':
						process_comment();
						break;
					case '<':
						real_parse();
						break;
					case ' ':
					case '\r':
					case '\t':
					case '\n':
						break;
					case 0xFE:
					case 0xFF:
						stack.error_msg << "Configuration files must be written in UTF8.\n";
						break;
						
					default:
						throw KernelException("Tag start expected.");
				}
			}
		}
		catch (KernelException& err)
		{
			stack.error_msg << err.get_reason() << " at " << current.str();

			if (tag)
			{
				stack.error_msg << " (inside tag " << tag->tag << " at line " << tag->src_line << ")\n";
			}
			else
			{
				stack.error_msg << " (last tag was on line " << last_tag.line << ")\n";
			}
		}

		return false;
	}
};

void parse_stack::include_file(config_rule* tag, int flags)
{
	if (flags & OPTION_NO_INCLUDE)
	{
		throw KernelException("Invalid <include> tag in file included with noinclude=\"yes\"");
	}

	std::string must_have_tag;
	tag->read_string("must_have_tag", must_have_tag);

	std::string name;

	if (tag->read_string("file", name))
	{
		if (tag->as_bool("noinclude", false))
		{
			flags |= OPTION_NO_INCLUDE;
		}
	
		if (tag->as_bool("noexec", false))
		{
			flags |= OPTION_NO_EXECUTE;
		}

		if (tag->as_bool("noenv", false))
		{
			flags |= OPTION_NO_ENV;
		}

		if (!process_conf_buffer(Kernel->Config->Paths.PrependConfig(name), flags, must_have_tag))
		{
			throw KernelException("Included");
		}
	}
	else if (tag->read_string("directory", name))
	{
		if (tag->as_bool("noinclude", false))
		{
			flags |= OPTION_NO_INCLUDE;
		}

		if (tag->as_bool("noexec", false))
		{
			flags |= OPTION_NO_EXECUTE;
		}

		if (tag->as_bool("noenv", false))
		{
			flags |= OPTION_NO_ENV;
		}

		const std::string includedir = Kernel->Config->Paths.PrependConfig(name);
		std::vector<std::string> files;
	
		if (!FileSystem::AsFileList(includedir, files, "*.conf"))
		{
			throw KernelException("Unable to read directory for include: " + includedir);
		}

		std::sort(files.begin(), files.end());

		for (std::vector<std::string>::const_iterator iter = files.begin(); iter != files.end(); ++iter)
		{
			const std::string path = includedir + '/' + *iter;

			if (!process_conf_buffer(path, flags, must_have_tag))
			{
				throw KernelException("Included");
			}
		}
	}
	else if (tag->read_string("executable", name))
	{
		if (flags & OPTION_NO_EXECUTE)
		{
			throw KernelException("Invalid <include:executable> tag in file included with noexec=\"yes\"");
		}

		if (tag->as_bool("noinclude", false))
		{
			flags |= OPTION_NO_INCLUDE;
		}

		if (tag->as_bool("noexec", true))
		{
			flags |= OPTION_NO_EXECUTE;
		}

		if (tag->as_bool("noenv", true))
		{
			flags |= OPTION_NO_ENV;
		}

		if (!process_conf_buffer(name, flags, must_have_tag, true))
		{
			throw KernelException("Included");
		}
	}
}

void parse_stack::load_file_tobuff(const std::string& key, const std::string& name, int flags, bool exec)
{
	if (flags & OPTION_NO_INCLUDE)
	{
		throw KernelException("Invalid <files> tag in file included with noinclude=\"yes\"");
	}

	if (exec && (flags & OPTION_NO_EXECUTE))
	{
		throw KernelException("Invalid <execfiles> tag in file included with noexec=\"yes\"");
	}

	std::string path = Kernel->Config->Paths.PrependConfig(name);
	FileStream file(exec ? popen(name.c_str(), "r") : fopen(path.c_str(), "r"), exec);

	if (!file)
	{
		throw KernelException("Could not read \"" + path + "\" for \"" + key + "\" file");
	}

	file_cache& cache = file_content[key];
	cache.clear();

	char buffer[5120];

	while (fgets(buffer, sizeof(buffer), file))
	{
		size_t len = strlen(buffer);

		if (len)
		{
			if (buffer[len-1] == '\n')
			{
				len--;
			}

			cache.push_back(std::string(buffer, len));
		}
	}
}

bool parse_stack::process_conf_buffer(const std::string& path, int flags, const std::string& required_tag, bool isexec)
{
	if (stdhelpers::isin(streaming, path))
	{
		throw KernelException((isexec ? "Executable " : "File ") + path + " is included recursively (looped inclusion)");
	}
	
	FileStream file((isexec ? popen(path.c_str(), "r") : fopen(path.c_str(), "r")), isexec);

	if (!file)
	{
		throw KernelException("Could not read \"" + path + "\" for include");
	}

	streaming.push_back(path);
	ConfFile p(*this, flags, file, path, required_tag);
	bool ok = p.out_tag_parse();
	streaming.pop_back();
	return ok;
}

bool config_rule::read_string(const std::string& key, std::string& value, bool allow_lf)
{
	for (file_config_items::iterator j = items.begin(); j != items.end(); ++j)
	{
		if (j->first != key)
		{
			continue;
		}

		value = j->second;

 		if (!allow_lf && (value.find('\n') != std::string::npos))
		{
			for (std::string::iterator n = value.begin(); n != value.end(); n++)
				if (*n == '\n')
					*n = ' ';
		}

		return true;
	}

	return false;
}

std::string config_rule::as_string(const std::string& key, const std::string& default_value, const STR1::function<bool(const std::string&)>& validator)
{
	std::string res;

	if (!read_string(key, res))
	{
		return default_value;
	}

	if (!validator(res))
	{
		return default_value;
	}

	return res;
}

std::string config_rule::as_string(const std::string& key, const std::string& default_value, size_t minlen, size_t maxlen)
{
	std::string res;

	if (!read_string(key, res))
	{
		return default_value;
	}

	if (res.length() < minlen || res.length() > maxlen)
	{
		bprint(INFO, "The length of <%s:%s> is not between %ld and %ld; value set to %s.", tag.c_str(), key.c_str(), minlen, maxlen, default_value.c_str());
		return default_value;
	}

	return res;
}

namespace
{

	template <typename Numeric> void readable_magnitude(const std::string& tag, const std::string& key, const std::string& val, Numeric& num, Numeric default_value, const char* tail)
	{
		if (!*tail)
		{
			return;
		}

		switch (toupper(*tail))
		{
			case 'K':
				num *= 1024;
				return;

			case 'M':
				num *= 1024 * 1024;
				return;

			case 'G':
				num *= 1024 * 1024 * 1024;
				return;
		}

		const std::string message = "WARNING: <" + tag + ":" + key + "> value of " + val + " contains an invalid magnitude specifier '"
			+ tail + "'; value set to " + convto_string(default_value) + ".";
		num = default_value;
	}

	template <typename Numeric> void readable_range(const std::string& tag, const std::string& key, Numeric& num, Numeric default_value, Numeric min, Numeric max)
	{
		if (num >= min && num <= max)
		{
			return;
		}

		const std::string message = "WARNING: <" + tag + ":" + key + "> value of " + convto_string(num) + " is not between "
			+ convto_string(min) + " and " + convto_string(max) + "; value set to " + convto_string(default_value) + ".";

		num = default_value;
	}
}

long config_rule::as_int(const std::string &key, long default_value, long min, long max)
{
	std::string result;

	if(!read_string(key, result))
	{
		return default_value;
	}

	const char* res_cstr = result.c_str();
	char* res_tail = NULL;
	long res = strtol(res_cstr, &res_tail, 0);

	if (res_tail == res_cstr)
	{
		return default_value;
	}

	readable_magnitude(tag, key, result, res, default_value, res_tail);
	readable_range(tag, key, res, default_value, min, max);
	return res;
}

unsigned long config_rule::as_uint(const std::string& key, unsigned long default_value, unsigned long min, unsigned long max, bool force)
{
	std::string result;

	if (!read_string(key, result))
	{
		return default_value;
	}

	const char* res_cstr = result.c_str();
	char* res_tail = NULL;
	unsigned long res = strtoul(res_cstr, &res_tail, 0);

	if (res_tail == res_cstr)
	{
		return default_value;
	}

	readable_magnitude(tag, key, result, res, default_value, res_tail);
	readable_range(tag, key, res, default_value, min, max);
	
        if (force && (convto_num<unsigned int>(result) > max || convto_num<unsigned int>(result) < min))
        {
                bprint(ERROR, "Configuration error: %s", this->tag.c_str());
                bprint(ERROR, "%s must have a valid range. Max: %lu, Min: %lu", key.c_str(), max, min);
                Kernel->Exit(EXIT_CODE_CONFIG, false);
        }
	
	return res;
}

unsigned long config_rule::get_duration(const std::string& key, unsigned long default_value, unsigned long min, unsigned long max)
{
	std::string duration;

	if (!read_string(key, duration))
	{
		return default_value;
	}

	unsigned long ret;

	if (!Daemon::Duration(duration, ret))
	{
		bprint(INFO, "Value of <" + tag + ":" + key + "> at " + get_tag_location() + " is not a duration; value set to " + convto_string(default_value) + ".");
		return default_value;
	}

	readable_range(tag, key, ret, default_value, min, max);
	return ret;
}

double config_rule::as_float(const std::string& key, double default_value, double min, double max)
{
	std::string result;

	if (!read_string(key, result))
	{
		return default_value;
	}

	double res = strtod(result.c_str(), NULL);
	readable_range(tag, key, res, default_value, min, max);
	return res;
}

bool config_rule::as_bool(const std::string &key, bool default_value)
{
	std::string result;

	if(!read_string(key, result))
	{
		return default_value;
	}

	if (stdhelpers::string::equalsci(result, "yes") || stdhelpers::string::equalsci(result, "true") || stdhelpers::string::equalsci(result, "on") || result == "1")
	{
		return true;
	}

	if (stdhelpers::string::equalsci(result, "no") || stdhelpers::string::equalsci(result, "false") || stdhelpers::string::equalsci(result, "off") || result == "0")
	{
		return false;
	}

	bprint(INFO, "Not valid value: <" + tag + ":" + key + "> at " + get_tag_location() + ". Will ignore.");
	return default_value;
}

std::string config_rule::get_tag_location()
{
	return src_name + ":" + convto_string(src_line);
}

config_rule* config_rule::create(const std::string& Tag, const std::string& file, int line, file_config_items*& Items)
{
	config_rule* rv = new config_rule(Tag, file, line);
	Items = &rv->items;
	return rv;
}

config_rule::config_rule(const std::string& Tag, const std::string& file, int line) : tag(Tag), src_name(file), src_line(line)
{

}
