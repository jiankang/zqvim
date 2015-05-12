//
//  main.c
//  zqvim
//
//  Created by Jim Wang on 5/11/15.
//  Copyright (c) 2015 cisco-webex-mac-team. All rights reserved.
//

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "base64.h"

#define MAX_PATH 1024
char* g_cmdName = NULL;
void getFullPath(char* fileName, char buffer[], int len = MAX_PATH);
void showfile(const char* filePath);
void convert(const char* filePath);
void rconvert(const char* filePath);

void usage(void)
{
	printf("usage: %s [filePath]\n",g_cmdName);
}

void checkPasswd(void)
{
	char password[10];
	printf("Enter password!\n");
	scanf("%s",password);
	std::string str = "123456";
	if(0!=strcasecmp(str.c_str(), password))
	{
		printf("password error!\n");
		exit(1);
	}
}

void showfile(const char* filePath)
{
	system("clear");
	printf("\n============%s===========\n",filePath);
	FILE* f = fopen(filePath, "rb");
	if(f && fseek(f, 0, SEEK_END) == 0)
	{
		long filelen = ftell(f);
		char* fbuffer = new char[filelen];
		memset_s(fbuffer, filelen, 0, filelen);
		fseek(f, 0, SEEK_SET);
		fread(fbuffer, 1, filelen, f);
		printf("%s",fbuffer);
		printf("\n=======================\n");
		fclose(f);
		delete[] fbuffer;
	}
}

void convert(const char* filePath)
{
	if(filePath == NULL)
	{
		printf("%s:error parameters\n",__FUNCTION__);
		return;
	}
	printf("encoding %s....\n",filePath);
	FILE* f = fopen(filePath, "rb+");
	if(f && fseek(f, 0, SEEK_END) == 0)
	{
		long filelen = ftell(f);
		char* fbuffer = new char[filelen];
		memset_s(fbuffer, filelen, 0, filelen);
		fseek(f, 0, SEEK_SET);
		fread(fbuffer, filelen, 1, f);
		//printf("%s",fbuffer);
		unsigned long base64_len = GetBase64EncodeSize(filelen);
		char *base64code = new char[base64_len];
		memset_s(base64code, base64_len, 0, base64_len);
		Base64Encode(base64code, fbuffer);
		printf("\n=======================\n");
		printf("%s",base64code);
		printf("\n=======================\n");
		fseek(f, 0, SEEK_SET);
		fwrite(base64code, base64_len, 1, f);
		fflush(f);
		fclose(f);
		delete[] fbuffer;
		delete[] base64code;
	}
}

void rconvert(const char* filePath)
{
	if(filePath == NULL)
	{
		printf("%s:error parameters\n",__FUNCTION__);
		return;
	}
	printf("decoding %s....\n",filePath);
	FILE* f = fopen(filePath, "rb+");
	if(f && fseek(f, 0, SEEK_END) == 0)
	{
		long base64len = ftell(f);
		char* base64code = new char[base64len];
		memset_s(base64code, base64len, 0, base64len);
		fseek(f, 0, SEEK_SET);
		fread(base64code, 1, base64len, f);
		unsigned long decodeLen = GetBase64DecodeSize(base64len);
		char* fbuffer = new char[decodeLen];
		memset_s(fbuffer, decodeLen, 0, decodeLen);
		Base64Decode(fbuffer, base64code);
		printf("\n=======================\n");
		printf("%s",fbuffer);
		printf("\n=======================\n");
		
		fseek(f, 0, SEEK_SET);
		fwrite(fbuffer, decodeLen, 1, f);
		fflush(f);
		fclose(f);
		delete[] fbuffer;
		delete[] base64code;
	}
}

void getFullPath(char* fileName, char buffer[], int len)
{
	if(fileName == NULL || buffer == NULL)
	{
		printf("%s:error parameters\n",__FUNCTION__);
		return;
	}
	std::string strFileName = fileName;
	if(strFileName.at(0) == '/')
	{
		strncpy(buffer, fileName, strlen(fileName));
		return;
	}
	
	getcwd(buffer, len);
	if(errno == ERANGE)
	{
		printf("exceed the char buffer.\n");
		exit(1);
	}
	
	std::string dir = buffer;
	std::string::size_type s = dir.find_last_of("/");
	if(s != dir.length()-1)
	{
		dir += "/";
	}
	
	dir += strFileName;
	strncpy(buffer, dir.c_str(), dir.length());
}

int main(int argc, char * const argv[]) {
	g_cmdName = new char(strlen(argv[0]));
	strcpy(g_cmdName, argv[0]);
	//checkPasswd();
	char fullname[MAX_PATH] = {0};
//	int ch;
//	while (( ch = getopt(argc, argv, "c:r:")) != -1)
//	{
//		switch (ch) {
//			case 'c':
//				{
//					getFullPath(optarg,fullname);
//					convert(fullname);
//				}
//				break;
//			case 'r':
//				{
//					getFullPath(optarg,fullname);
//					rconvert(fullname);
//				}
//				break;
//			case '?':
//			default:
//				usage();
//		}
//	}
//	argc -= optind;
//	argv += optind;
	
	char* const fileName = *argv;
	if(fileName)
	{
		getFullPath(fileName,fullname);
		showfile(fullname);
	}
	
    return 0;
}
