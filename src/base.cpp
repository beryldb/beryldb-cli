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
#include "base.h"

static void* last_iter = NULL;

void* refcountbase::operator new(size_t size)
{
        last_iter = ::operator new(size);
        return last_iter;
}

void refcountbase::operator delete(void* obj)
{
        if (last_iter == obj)
        {
                last_iter = NULL;
        }
        
        ::operator delete(obj);
}

refcountbase::refcountbase() : refcount(0)
{
        if (this != last_iter)
        {
                throw KernelException("Reference allocate on the stack!");
        }
}

refcountbase::~refcountbase()
{

}

