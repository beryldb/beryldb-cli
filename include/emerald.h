
#pragma once

#include <stdarg.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <algorithm>
#include <memory>
#include <iostream>
#include <vector>

extern std::unique_ptr<Emerald> Kernel;

class Emerald
{

};
