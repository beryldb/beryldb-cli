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

/* Incorrect agent format. */

const std::string WRONG_AGENT = "Your agent is not valid.";

/* Valid agent. */

const std::string VALID_AGENT = "Agent must be in the 3 to 15 length range.";

/* Default ping interval. */

const unsigned int PING_INTVL = 20;

/* Bcrypt rounds. */

const unsigned int DEFAULT_BROUNDS = 12;

%target include/constants.h
