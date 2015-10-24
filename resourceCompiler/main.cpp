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
	cerr << "Usage: " << progname << " [-l listFile] [-p working_path] -o output resource1 resource2 resourceN" << endl;
}

int main(int argc, char *argv[])
{
	string outputfile;
	list<string> inputFiles;
	ifstream listfile;

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
			case 'l':
				if (argc == i + 1)
				{
					usage(argv[0]);
					return -1;
				}
				i++;
				listfile.open(argv[i], ios::in);
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
				outputfile = argv[i];
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

	if (outputfile.empty() || inputFiles.empty())
	{
		usage(argv[0]);
		return -1;
	}
	ofstream resourceFile(outputfile, ios::out | ios::trunc);
	if (!resourceFile.is_open())
	{
		cerr << "Error: Could not open \"" << outputfile << "\"" << endl;
		return -2;
	}

	char timestr[80];
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timestr, sizeof(timestr), "%c", timeinfo);

	// Print header
	resourceFile << "/******************************************************" << endl;
	resourceFile << " * This is an automatically generated resource file.  *" << endl;
	resourceFile << " * Do not modify this file. All changes will be lost! *" << endl;
	resourceFile << " * Resource file creater made by Robbert-Jan de Jager.*" << endl;
	resourceFile << " ******************************************************" << endl;
	resourceFile << " * Generated on: " << timestr << endl;
	resourceFile << " * Input files: " << endl;
	list<string>::iterator it;
	for (it = inputFiles.begin(); it != inputFiles.end(); ++it)
	{
		resourceFile << " *   " << *it << endl;
	}
	resourceFile << " */" << endl;
	resourceFile << endl;
	string outname = outputfile;
	transform(outname.begin(), outname.end(), outname.begin(), ::toupper);
	outname.insert(outname.begin(), '_');
	outname.append("_");
	replace(outname.begin(), outname.end(), '.', '_');
	resourceFile << "#ifndef " << outname << endl;
	resourceFile << "#define " << outname << endl;
	resourceFile << endl;
	resourceFile << "#ifdef __cplusplus" << endl;
	resourceFile << "extern \"C\" { " << endl;
	resourceFile << "#endif" << endl;




	for (it = inputFiles.begin(); it != inputFiles.end(); ++it)
	{
		// Add all the resource files

		// Open the file
		ifstream input(*it, ios::in|ios::binary);
		if (!input.is_open())
		{
			cerr << "Warning: Could not open input file \"" << *it << "\"" << endl; 
			continue;
		}
		resourceFile << "// Resource: \"" << *it << "\"" << endl;
		string resourceName = "resource_";
		resourceName.append(*it);
		replace(resourceName.begin(), resourceName.end(), '.', '_');
		resourceFile << "static const unsigned char " << resourceName << "[] = " << endl;
		resourceFile << "{" << endl << "\t";
		char data;
		input.read(&data, 1);
		while (input.good())
		{
			resourceFile << "0x" << toHex(data) << ", ";
			input.read(&data, 1);
		}
		resourceFile << endl << "};" << endl;

		resourceFile << endl;
		input.close();
	}

	resourceFile << "#ifdef __cplusplus" << endl;
	resourceFile << "}" << endl;
	resourceFile << "#endif" << endl;
	resourceFile << "#endif" << endl;

	resourceFile.close();
	return 0;
}