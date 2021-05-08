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

#pragma once

/* Incorrect agent format. */

#define WRONG_AGENT "Your agent is not valid."

/* Valid agent. */

#define VALID_AGENT "Agent must be in the 3 to 15 length range."

/* Default ping interval. */

#define PING_INTVL 20

%target include/constants.h
