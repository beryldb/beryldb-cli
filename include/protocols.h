/*
 * BerylDB - A lightweight database.
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

enum BRLD_PROTOCOL
{
	/* Beryldb internal protocol codes. */
	
	BRLD_CURRENT_TIME  		=		100,
	BRLD_SERVERNAME  		=		101,
	BRLD_CREATED  			=		102,
        BRLD_PONG                       =               103,
        BRLD_PUBLISH                    =               104,
        BRLD_ALERT                      =               105,
        BRLD_LOCAL_TIME                 =               106,
	BRLD_MYINFO 			=		107,
	BRLD_CONNECTED  	    	=		108, 
        BRLD_PING                       =               110,

        BRLD_LINKS                      =               111,
        BRLD_ENDOFLINKS                 =               112,
        BRLD_END_OF_SLIST               =               113,
        BRLD_INFO                       =               114,
	BRLD_MAP    		        = 		115, 
	BRLD_ENDMAP             	= 		116, 
	BRLD_MAPUSERS                   = 		117, 
	BRLD_STATUS                     = 		118,
	
	BRLD_TOTAL_VIEW                 = 		120,
	BRLD_LUSEROP                    = 		121,
	BRLD_CHANS_VIEW               	= 		122,
	BRLD_LOCAL_VIEW                 = 		123,
	BRLD_LOCAL_PEAK                 = 		124,
	BRLD_GLOBAL_VIEW                = 		125,
	BRLD_CLIENTSERVER               = 		126,
	BRLD_ENDOFCLIENT                = 		127,
	BRLD_FINGER_LIST                = 		128,
	BRLD_CHANNELCREATED             = 		130,
	 
        BRLD_MODLIST 			= 		131,
        BRLD_END_OF_MODLIST 		= 		132,
	BRLD_VERSION                    = 		133,
	BRLD_SLIST_REPLY                = 		134,
        BRLD_SYNTAX                     = 		135, 
        
        BRLD_FINGER_END                 = 		140,
        BRLD_DBQUERY_OK                 = 		141,
        BRLD_ERROR              	=       	142,
	BRLD_ENDOFINFO                  = 		143,
	BRLD_JOIN			=		145,
	BRLD_QUIT			=		146,
	BRLD_PART			=		147,
	BRLD_VIS_HOST          		= 		148,
	BRLD_UPTIME			=		149,
	BRLD_END_STATUS			=		150,
	
	BRLD_CPU_USE 			=		151,
	BRLD_ALLOCATION			=		152,
	BRLD_SIGNALS			=		153,
	BRLD_PFAULTS			=		154,
	BRLD_SWAPS			=		155,
	BRLD_CSW			=		156,
	BRLD_CPU_NOW			=		157,
	BRLD_QUITTING			=		158,
	BRLD_USER_ADD			=		159,
	BRLD_DB_ADD			=		160,
	
	BRLD_NEW_USE			=		161,
	BRLD_COMMANDS_START 		= 		162,
	BRLD_COMMANDS_END 		= 		163,
	BRLD_QUERY_OK			=		164,
	BRLD_CURRENT_USE		=		165,
	BRLD_ADD_FLAGS			=		166,
	BRLD_LOGIN_CHPASS		=		167,
	BRLD_LOGIN_DEL			=		168,
	BRLD_EXPIRE_ADD			=		170,
	
	BRLD_TTL			=		171,
	BRLD_PERSIST			=		172,
	BRLD_EXP_COUNT			=		173,
        BRLD_SHARED_MEM                 =       	174, 
        BRLD_COUNT			=		175,
        BRLD_USER_ITEM			=		176,
        BRLD_USER_LIST_END		=		177,
        BRLD_VIEW_INFO			=		178,
        BRLD_EXPIRE_ITEM		=		179,
        BRLD_EXPIRE_END			=		180,
        
        BRLD_EXPIRE_BEGIN		=		181,
        BRLD_INFO_EXP_DEL		=		182,
        BRLD_EXP_DELETED		=		183,
        BRLD_FINGER_BEGIN		=		184,
	BRLD_LOGIN_FLAGS		=		185,
	BRLD_YOUR_FLAGS			=		186,
	BRLD_BEGIN_OF_MODLIST		=		187,
	BRLD_HSEARCH_BEGIN		=		188,
	BRLD_HSEARCH_END		=		189,
	BRLD_HSEARCH_ITEM		=		190,
	
	BRLD_FIND_BEGIN			=		194,
	BRLD_FIND_END			=		195,
	BRLD_ITEM			=		196,
	BRLD_QUERY_OK_DONE		=		197,
	BRLD_CONF_DEFAULTS		=		198,
	BRLD_CONF_VALUE			=		199,
	BRLD_CONF_OK			=		200,
	
	BRLD_CURRENT_DIR		=		201,
	BRLD_CURRENT_CONF		=		202,
	BRLD_CURRENT_LOG		=		203,
	BRLD_FLAG_INFO			=		204,
	BRLD_LENGTH			=		206,
	BRLD_IN_CACHE			=		207,
	BRLD_RESET_CACHE		=		208,
	BRLD_DB_RESET			=		209,	
	BRLD_DB_RESET_TIMED		=		210,
	
	BRLD_LOGINS_BEGIN		=		211,
	BRLD_LOGINS_END			=		212,
	BRLD_LOGINS_ITEM		=		213,
	BRLD_INSTANCE			=		214,
	BRLD_SUBS_LIST			=		215,
	BRLD_SUBS_LIST_END		=		216,
	BRLD_SUBS_LIST_BEGIN		=		217,
	BRLD_MOD_UNLOAD			=		218,
	BRLD_MOD_LOADED			=		219,
	BRLD_SHUTDOWN			=		220,
	
	BRLD_RESTART			=		221,
	BRLD_TYPE_RAND			=		222,
	BRLD_AGENT_SET			=		223,
	BRLD_SFLUSH			=		225,
	BRLD_SFLUSHED			=		225,
	BRLD_PARTALL			=		226,
	BRLD_LPOS 			=		227,
	BRLD_SEARCH_ITEM		=		228,
	BRLD_SEARCH_END			=		229,
	BRLD_SEARCH_BEGIN		=		230,
	
	BRLD_HASH			=		231,
	BRLD_LSEARCH_BEGIN		=		232,
	BRLD_LSEARCH_ITEM		=		233,
	BRLD_LSEARCH_END		=		234,
	BRLD_LFIND_ITEM			=		235,
	BRLD_LFIND_END			=		236,
	BRLD_LFIND_BEGIN		=		237,
	BRLD_PREPARE_OK			=		238,
	BRLD_HOVER_OK			=		240,
	
	BRLD_HLIMITS_OK			=		241,
	BRLD_END_MASS_JOIN		=		243,
	BRLD_MASS_JOIN			=		244,
	BRLD_MONITOR			=		245,
	BRLD_NOW_MONITORING		=		246,
 	BRLD_STOP_MONITOR		=		247,
	BRLD_RESET_MONITOR		=		248,
	BRLD_FUTURE_ADD			=		249,
	BRLD_MONITOR_LIST		=		250,
		
	BRLD_MONITOR_USER		=		251,
	BRLD_TTE			=		252,
	BRLD_FUTURE_EXEC		=		253,
	BRLD_INFO_FUT_DEL		=		255,
	BRLD_FUTURE_DELETED		=		256,
	BRLD_MY_CHANS			=		257,
	BRLD_CHANS			=		258,
	BRLD_USING			=		259,
	BRLD_WHOAMI			=		260,
	BRLD_PAUSED			=		261,
	BRLD_RESUMED			=		262,
	BRLD_IDLE			=		263,
	BRLD_FUTURE_VALUE		=		264,
        BRLD_FUTURE_BEGIN		=		265,
        BRLD_FUTURE_ITEM		=		266,
	BRLD_FUTURE_END			=		267,        
	BRLD_AGENT			=		268,
	BRLD_LOCAL_EPOCH		=		269,
	BRLD_TEST			=		270,
	
	BRLD_DB_NAME			=		272,
	BRLD_USER_LIST_BEGIN		=		273,
	BRLD_ADMIN_LIST_BEGIN		=		274,
	BRLD_ADMIN_LIST_END		=		275,
	BRLD_END_L			=		276,
	BRLD_NOTIFICATION		=		277,
	BRLD_RESET_NRESET		=		278,
	BRLD_NOW_NOTIFYING		=		279,
	BRLD_STOP_NOTIFY		=		280,
	BRLD_NRESET			=		281,
	BRLD_START_LIST			=		282,
	BRLD_END_LIST			=		283,
        BRLD_START_UNQ_LIST             =               284,
        BRLD_END_UNQ_LIST               =               285,
        BRLD_CORES			=		286,
        BRLD_FIRSTOF			=		287,	
        BRLD_FLUSHED			=		288,
        BRLD_RESTART_OK			=		289,
        BRLD_COMMAND_ITEM		=		290,
        BRLD_LOGGED_OUT			=		291,
        BRLD_SHUTTING_DOWN		=		292,
        BRLD_DB_CHANGED			=		293,
        BRLD_DB_BEGIN        		=		294,
        BRLD_DB_END			=		295,
        BRLD_DB_ITEM			=		296,
        BRLD_DB_CREATED			=		297,
        BRLD_DB_DELETED			=		298,
	BRLD_MULTI_OK			=		299,
	
	BRLD_QUEUED			=		300,	
	BRLD_DB_DEFAULT			=		301,
	BRLD_ADMIN_ITEM 		=		302,
	BRLD_DEFAULT_CHANGED		=		303,
	BRLD_BEGIN_MONITOR_LIST		=		304,
	BRLD_END_MONITOR_LIST		=		305,
	
	BRLD_EXPIRES			=		306,
	BRLD_FUTURES			=		307,	
	BRLD_LOGIN			=		308,
	
	/* Error protocol codes. */
	
	ERR_NO_INSTANCE               	= 		500,
	ERR_NOSUCHSERVER                = 		501,
	ERR_NO_CHAN               	= 		502, 
	ERR_UNLOAD_MOD          	=               503, 
	ERR_NORECIPIENT                 = 		505,
	ERR_PUB_NOT_PROVIDED            = 		506,
	ERR_DBQUERY_FAILED              = 		507,
	ERR_CANNOTSENDTOCHAN            = 		508,
	ERR_EXCESS_SUBS             	= 		509,
	ERR_NOSUCHSERVICE               = 		510,
	 
	ERR_CMD_NOFND              	= 		511,
	ERR_ALREADY_LOGGED              = 		512,
	ERR_INVALID_LOGIN            	= 		513,
	ERR_INSTANCE_NOT_SUBS           = 		514,
	ERR_NOT_SUBSCRIBED              = 		515,
	ERR_CANTSETLOGIN                = 		516, 
        ERR_INVALID_CHAN                = 		517,
        ERR_CHANFULL                    =		518,
        ERR_LOGIN_NO_EXISTS		=		519,

	ERR_NO_AUTH               	=	 	520,
	ERR_MISS_PARAMS			=		521,
	ERR_AGENT_IS_SET           	= 		522,
	ERR_CHANOPRIVSNEEDED            = 		527, 
	ERR_USERSDONTMATCH              = 		529,
	ERR_CANTSENDTOINSTANCE          = 		530, 
	ERR_LOGIN_EXISTS		=		531,
	ERR_BAD_AGENT			=		534,
	ERR_DB_EXISTS			=		535,
	ERR_WRONG_PASS			=		536,
	ERR_MUST_BE_POS_INT		=		537,
	ERR_QUERY			=		538,
	ERR_NO_FLAGS			=		539,
	ERR_PROTECTED_LOGIN		=		540,
	
	ERR_INVALID_PASS		=		541,
	ERR_INVALID_PARAM		=		542,
	ERR_EXPIRE			=		543,
	ERR_NOT_EXPIRE			=		544,
	ERR_PERSIST			=		545,
	ERR_WRONG_SYNTAX		=		546,
	ERR_NOT_ACCEPTING_C		=		547,
	ERR_USE				=		548,
	ERR_INVALID_RANGE		=		549,
	ERR_UNABLE_FLUSH		=		550,
	
	ERR_CONFIG_NOT_FOUND		=		551,
        ERR_MUST_BE_DOUBLE              =               552,
        ERR_MUST_BE_INT                 =               553,
        ERR_UNABLE_DBRESET		=		554,
        ERR_NO_ORIGIN			=		555,
        ERR_EXCEED_BUFFER        	=		556,
        ERR_LOGIN_NO_FLAGS		=		557,
        ERR_SFLUSH			=		558,
        ERR_PARTALL			=		559,
	ERR_UNKNOWN_HASH		=		560,
	ERR_HASH_NOT_FOUND		=		561,
	ERR_GREAT_ZERO			=		563,
	ERR_HUNABLE_RESET		=		564,
	ERR_WRONG_HPARAMS		=		565,
	ERR_WRONG_MONITOR		=		566,
	ERR_NO_MONITOR			=		567,
	ERR_FUTURE			=		568,
	ERR_INVALID_MONITORLVL		=		569,
	ERR_NOT_FUTURE			=		570,
	ERR_INVALID_NOTLVL		=		572,
	ERR_NO_NOTIFY			=		573,
	ERR_NOT_VALID_COORDINATE	=		574,
	ERR_NOT_NUM			=		575,
	ERR_NOT_FOUND			=		576,
	ERR_NO_CHANS			=		577,
	ERR_DB_NOT_SET			=		578,
	ERR_DB_NOT_FOUND		=		579,
	ERR_DB_NOT_EXISTS		=		580,
	ERR_MULTI			=		581,
	ERR_NO_DEFAULT			=		582,
	ERR_LOGIN_NOT_EXISTS		=		583,
	ERR_DB_BUSY			=		584
};
