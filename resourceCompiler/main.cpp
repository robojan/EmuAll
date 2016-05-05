#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <algorithm>
#include <Windows.h>
#include <ctime>

using namespace std;

string toHex(char c)
{
	char buffer[5];
	sprintf_s(buffer, sizeof(buffer), "%02X", c);
	return buffer;
}

void usage(const char *progname)
{
	cerr << "Usage: " << progname << " [-l listFile] [-p working_path] [-u] -o output resource1 resource2 resourceN" << endl;
}

void printInfo(ostream &out, list<string> &inFiles) {
	char timestr[80];
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timestr, sizeof(timestr), "%c", timeinfo);

	// Print header
	out << "/******************************************************" << endl;
	out << " * This is an automatically generated resource file.  *" << endl;
	out << " * Do not modify this file. All changes will be lost! *" << endl;
	out << " * Resource file creater made by Robbert-Jan de Jager.*" << endl;
	out << " ******************************************************" << endl;
	out << " * Generated on: " << timestr << endl;
	out << " * Input files: " << endl;
	list<string>::iterator it;
	for (it = inFiles.begin(); it != inFiles.end(); ++it)
	{
		out << " *   " << *it << endl;
	}
	out << " */" << endl;
}

void writeHeader(ostream &out, list<string> &inFiles, string fileName) {
	printInfo(out, inFiles);
	out << endl;

	// add header declarations
	string outname = fileName;
	transform(outname.begin(), outname.end(), outname.begin(), ::toupper);
	outname.insert(outname.begin(), '_');
	outname.append("_");
	replace(outname.begin(), outname.end(), '.', '_');
	out << "#ifndef " << outname << endl;
	out << "#define " << outname << endl;
	out << endl;
	out << "#ifdef __cplusplus" << endl;
	out << "extern \"C\" { " << endl;
	out << "#endif" << endl;
	out << endl;

	for (auto &fileName : inFiles) {
		out << "// Resource: \"" << fileName << "\"" << endl;
		string resourceName = "resource_";
		resourceName.append(fileName);
		replace(resourceName.begin(), resourceName.end(), '.', '_');
		replace(resourceName.begin(), resourceName.end(), '-', '_');
		out << "extern const unsigned char " << resourceName << "[];" << endl;
		resourceName.append("_len");
		out << "extern const unsigned int " << resourceName << ";" << endl;
		out << endl;
	}
	out << "#ifdef __cplusplus" << endl;
	out << "}" << endl;
	out << "#endif" << endl;
	out << "#endif" << endl;


}

void writeData(ostream &out, list<string> &inFiles, string headerFileName) {
	printInfo(out, inFiles);
	out << endl;
	out << "#include \"" << headerFileName << "\"" << endl;
	unsigned char buffer[512];


	for (auto &fileName : inFiles) {
		out << "// Resource: \"" << fileName << "\"" << endl;
		string resourceName = "resource_";
		resourceName.append(fileName);
		replace(resourceName.begin(), resourceName.end(), '.', '_');
		replace(resourceName.begin(), resourceName.end(), '-', '_');
		string lenName = resourceName;
		lenName.append("_len");

		// Open the file
		ifstream input(fileName, ios::in | ios::binary);
		if (!input.is_open())
		{
			cerr << "Warning: Could not open input file \"" << fileName << "\"" << endl;
			out << "//Resource could not be opened";
			out << "const unsigned int " << lenName << " = 0;" << endl;
			out << "const unsigned char * const " << resourceName << " = (void *)0; " << endl;
			out << endl;
			continue;
		}

		int fileSize = 0;
		input.seekg(0, fstream::end);
		fileSize = (int)input.tellg();
		input.seekg(0, fstream::beg);

		out << hex;
		out << "const unsigned int " << lenName << " = 0x" << fileSize << ";" << endl;
		out << "const unsigned char " << resourceName << "[0x" << fileSize <<"] = { " << endl;
		do {
			input.read((char *)buffer, 512);
			int read = (int)input.gcount();
			for (int i = 0; i < read; i++) {
				out << "0x" << (int)buffer[i] << ",";
			}
		} while (input.good());
		out << endl << "};" << endl;
		out << endl;
	}
}

uint64_t GetFileModificationTime(const string &file) {
	HANDLE fhandle = CreateFile(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (fhandle == INVALID_HANDLE_VALUE) {
		return 0;
	}
	FILETIME fileTime;
	if (!GetFileTime(fhandle, NULL, NULL, &fileTime)) {
		CloseHandle(fhandle);
		return 0;
	}

	ULARGE_INTEGER intCast;
	intCast.LowPart = fileTime.dwLowDateTime;
	intCast.HighPart = fileTime.dwHighDateTime;
	uint64_t time = intCast.QuadPart;
	CloseHandle(fhandle);
	return time;
}

int CompareFileTimes(uint64_t a, uint64_t b) {
	FILETIME fta, ftb;
	ULARGE_INTEGER intcast;
	intcast.QuadPart = a;
	fta.dwHighDateTime = intcast.HighPart;
	fta.dwLowDateTime = intcast.LowPart;
	intcast.QuadPart = b;
	ftb.dwHighDateTime = intcast.HighPart;
	ftb.dwLowDateTime = intcast.LowPart;
	return CompareFileTime(&fta, &ftb);
}

void UpdateFileTime(const string &file) {
	FILETIME ft;
	SYSTEMTIME st;
	HANDLE fhandle = CreateFile(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (fhandle == INVALID_HANDLE_VALUE) {
		return;
	}
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
	SetFileTime(fhandle, NULL, NULL, &ft);
	CloseHandle(fhandle);
}

int main(int argc, char *argv[])
{
	string outFileName;
	list<string> inputFiles;
	ifstream listfile;
	bool update = true;
	string listFileName;

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (strlen(argv[i]) != 2)
			{
				usage(argv[0]);
				return -1;
			}
			// Starts with - so option
			switch (argv[i][1])
			{
			case 'u': // Update resource file
				update = false;
				break;
			case 'l':
				if (argc == i + 1)
				{
					usage(argv[0]);
					return -1;
				}
				i++;
				listFileName = argv[i];
				listfile.open(listFileName, ios::in);
				if (listfile.is_open())
				{
					while (!listfile.eof())
					{
						string infile;
						getline(listfile, infile);
						inputFiles.push_back(infile);
					}
				}
				listfile.close();
				break;
			case 'o':
				if (argc == i + 1)
				{
					usage(argv[0]);
					return -1;
				}
				i++;
				outFileName = argv[i];
				break;
			case 'p':
				if (argc == i + 1)
				{
					usage(argv[0]);
					return -1;
				}
				i++;
				if (!SetCurrentDirectory(argv[i]))
				{
					cerr << "Error: Could not set working directory to \"" << argv[i] << "\"" << endl;
					return -3;
				}
				break;
			default:
				usage(argv[0]);
				return -1;
			}
		}
		else {
			inputFiles.push_back(argv[i]);
		}
	}

	if (outFileName.empty() || inputFiles.empty())
	{
		usage(argv[0]);
		return -1;
	}

	string headerFileName = outFileName;
	string dataFileName = outFileName;
	headerFileName.append(".h");
	dataFileName.append(".c");

	if (!update) {
		// Check if the input files are newer than the list file
		uint64_t outTime = GetFileModificationTime(dataFileName);
		uint64_t outTime2 = GetFileModificationTime(headerFileName);
		if (CompareFileTimes(outTime, outTime2) < 0) outTime = outTime2;
		uint64_t listTime = GetFileModificationTime(listFileName);
		auto inFile = inputFiles.cbegin();
		uint64_t newestInputFile = GetFileModificationTime(*inFile);
		while (++inFile != inputFiles.cend()) {
			uint64_t modTime = GetFileModificationTime(*inFile);
			if (CompareFileTimes(modTime, newestInputFile) > 0) {
				newestInputFile = modTime;
			}
		}
		if (CompareFileTimes(outTime, newestInputFile) > 0 && CompareFileTimes(outTime, listTime) > 0) {
			cerr << "No update necessary" << endl;
			return 0;
		}
	}


	ofstream headerFile(headerFileName, ios::out | ios::trunc);
	if (!headerFile.is_open()) {
		cerr << "Error: Could not open \"" << headerFileName << "\"" << endl;
		return -2;
	}
	ofstream dataFile(dataFileName, ios::out | ios::trunc);
	if (!dataFile.is_open())
	{
		cerr << "Error: Could not open \"" << dataFileName << "\"" << endl;
		return -2;
	}
	
	writeHeader(headerFile, inputFiles, headerFileName);
	writeData(dataFile, inputFiles, headerFileName);

	headerFile.close();
	dataFile.close();
	
	return 0;
}