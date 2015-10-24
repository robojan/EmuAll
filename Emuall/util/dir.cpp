#include "memDbg.h"

#include "dir.h"

#include <windows.h>

std::vector<std::string> GetDirFiles(std::string folder, std::string extension)
{
	std::vector<std::string> result;
	
	WIN32_FIND_DATAA ffd;
	HANDLE hfind = INVALID_HANDLE_VALUE;
	std::string searchPath = folder;
	if (!extension.empty())
	{
		searchPath += "\\*.";
		searchPath += extension;
	}

	hfind = FindFirstFileA(searchPath.c_str(), &ffd);
	if (INVALID_HANDLE_VALUE == hfind)
	{
		return result;
	}

	do
	{
		result.push_back(folder.append("\\").append(ffd.cFileName));
	} while (FindNextFileA(hfind, &ffd) != 0);
	FindClose(hfind);
	return result;
}
