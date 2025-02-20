#pragma once

#include "stdio.h"
#include "string.h"
#include "net-utils.h"
//#include "winsock2.h"
//#include "windows.h"


int start(int argc, char* argv[]);

int resolve_dns(const char* name);