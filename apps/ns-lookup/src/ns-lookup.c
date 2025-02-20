#include "ns-lookup.h"

int start(int argc, char** argv)
{
	char name[256];

	memset(name, 0, sizeof(name));

	if (argc >= 2)
	{
		strcpy(name, argv[1]);
	}
	else
	{
		printf("Enter dns name:");
		int ret = scanf("%s", name);
		if (ret < 1)
		{
			return -3;
		}
	}

	return resolve_dns(name);
}

int resolve_dns(const char* name)
{
	char ip[64];
	
	memset(ip, 0, sizeof(ip));

	resolve_address(name, ip);

	printf("Host: %s\n", name);

	printf("IPv4 address: %s\n", ip);

	return 0;
}