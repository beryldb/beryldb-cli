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

class FileLoader
{
  private:
  
	std::vector<std::string> lines;

	unsigned long Size;

  public:
  
	FileLoader() : Size(0) 
	{ 
	
	}
	
	FileLoader(const std::string& filename);
	
	static void Dump(const std::string& filename);

	void Load(const std::string& filename);

	std::string as_string() const;

	const std::vector<std::string>& GetVector() const 
	{ 
		return lines; 
	}

	unsigned long TotalSize() const 
	{ 
		return this->Size; 
	}
};

class  FileSystem
{

 private:

	FileSystem() 
	{ 
	
	}

 public:
	
	static std::string get_real_path(const std::string& base, const std::string& fragment);

	
	static bool Exists(const std::string& path);

	
	static std::string GetName(const std::string& path);

	
	static bool AsFileList(const std::string& directory, std::vector<std::string>& entries, const std::string& match = "*");

};
