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

template <typename T> class reference
{
 private:
 
        T* value;
        
 public:
        
        reference() : value(0) 
        { 
        
        }
        
        reference(T* v) : value(v) 
        { 
                if (value) 
                {
                        value->refcount_inc(); 
                }
        }
        
        reference(const reference<T>& v) : value(v.value) { if (value) value->refcount_inc(); }
        reference<T>& operator=(const reference<T>& other)
        {
                if (other.value)
                        other.value->refcount_inc();
                this->reference::~reference();
                value = other.value;
                return *this;
        }

        ~reference()
        {
                if (value && value->refcount_dec())
                        delete value;
        }

        inline reference<T>& operator=(T* other)
        {
                if (value != other)
                {
                        if (value && value->refcount_dec())
                                delete value;
                        value = other;
                        if (value)
                                value->refcount_inc();
                }

                return *this;
        }

        inline operator bool() const { return (value != NULL); }
        inline operator T*() const { return value; }
        inline T* operator->() const { return value; }
        inline T& operator*() const { return *value; }
        inline bool operator<(const reference<T>& other) const { return value < other.value; }
        inline bool operator>(const reference<T>& other) const { return value > other.value; }
        static inline void* operator new(size_t, void* m) { return m; }

 private:

        static void* operator new(size_t);
        static void operator delete(void*);
};

class refcountbase
{
 private:
 
        mutable unsigned int refcount;
        
 public:

        refcountbase();
        virtual ~refcountbase();
        inline unsigned int GetReferenceerenceCount() const { return refcount; }
        static inline void* operator new(size_t, void* m) { return m; }
        static void* operator new(size_t);
        static void operator delete(void*);
        inline void refcount_inc() const { refcount++; }
        inline bool refcount_dec() const { refcount--; return !refcount; }
 private:

        refcountbase(const refcountbase&);
        void operator=(const refcountbase&);
};


class KernelException : public std::exception
{
        protected:
                
                const std::string error;
                
                const std::string source;

        public:
                
                KernelException(const std::string &message) : error(message), source("Kernel throw") 
                {

                }
                
                KernelException(const std::string &message, const std::string &src) : error(message), source(src) 
                {

                }
                
                virtual ~KernelException() throw() 
                {

                }
                
                const std::string& get_reason() const 
                { 
                        return this->error; 
                }
                
                const std::string& GetSource() const 
                { 
                        return this->source; 
                }
};

