#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <ctime>
#include <list>
#include <map>
using namespace std;

int indentlevel = 0;

map<string,int>paramTypes;

/**
 * Crummy auto indenting.
 */
string indent()
{
    string tab;
    for(int i=0; i<indentlevel; i++)
    {
        tab = tab + "\t";
    }

    return tab;
}

/**
 * Sets the value of a variable.
 */
void setParam(string* var, string lookFor, string line)
{
    string lower;
    lower = line;
    std::transform(lower.begin(),lower.end(),lower.begin(), (int (*)(int))tolower);

    size_t found = lower.find(lookFor);
    if (found!=std::string::npos)
    {
        size_t seperator = line.find(":");
        size_t whitespace = line.find(" ",seperator);
        string value = line.substr(seperator+(whitespace-seperator)+1);
        cout << lookFor << " is " << value << std::endl;
        *var = value;
    }
}

/**
 * Adds lilypond includes.
 */
void addIncludes(string* var, string line)
{
    string lower;
    lower = line;
    std::transform(lower.begin(),lower.end(),lower.begin(), (int (*)(int))tolower);

    size_t found = lower.find("include");
    if (found!=std::string::npos)
    {
        size_t seperator = line.find(":");
        size_t whitespace = line.find(" ",seperator);
        string value = line.substr(seperator+(whitespace-seperator)+1);
        *var = *var + "\\include \"" + value + "\"\n";
    }
}

/**
 * Adds lilypond paper params.
 */
string addPaperParams(list<string>* paperList, string line)
{
    string lower;
    lower = line;
    std::transform(lower.begin(),lower.end(),lower.begin(), (int (*)(int))tolower);

    size_t found = lower.find("paper");
    if (found!=std::string::npos)
    {
        size_t seperator = line.find(":");
        size_t whitespace = line.find(" ",seperator);
        string value = line.substr(seperator+(whitespace-seperator)+1);
//        *paperList->push_back(value);
        return value;
    }
    return "";
}

/**
 * Generates lilypond output files from an input template file.
 */
int main(int argc, char* argv[])
{
    paramTypes["title"] = 0;
    paramTypes["subtitle"] = 1;
    paramTypes["composer"] = 2;
    paramTypes["include"] = 3;
    paramTypes["paper"] = 4;

    // get the current date/time
    time_t t = time(0);
    struct tm * now = localtime( & t );

    // global vars
    int year = (now->tm_year + 1900);
    int month = (now->tm_mon + 1);
    int day = now->tm_mday;

    string title = "Untitled";
    string subtitle = "";
    string composer = "anon";

    string includes;
    list<string> paper;

    // read input file
    string line;
    string lower;
    ifstream inputFile(argv[1]);
    if(inputFile.is_open())
    {
        while(inputFile.good())
        {
            getline(inputFile,line);

            string lower;
            lower = line;
            std::transform(lower.begin(),lower.end(),lower.begin(), (int (*)(int))tolower);
    
            size_t seperator = lower.find(":");
            string param = lower.substr(0,seperator);

            switch(paramTypes[param])
            {
                case 0:
                    setParam(&title,"title",line);
                    break;
                case 1:
                    setParam(&subtitle,"sub",line);
                    break;
                case 2:
                    setParam(&composer,"composer",line);
                    break;
                case 3:
                    addIncludes(&includes,line);
                    break;
                case 4:
                    paper.push_back(addPaperParams(&paper,line));
                    break;
                
            }
        }
    }
    inputFile.close();

    cout << title << std::endl;
    cout << "© " << composer << " " << year << std::endl;

    // write output to files
    ofstream outputFile;
    outputFile.open ("score.ly");
    outputFile << "% Created on " << month << "/" << day << "/" << year << std::endl;
    outputFile << "\\version \"2.16.1\"" << std::endl << std::endl;
    outputFile << includes << std::endl;
    outputFile << "#(ly:set-option 'point-and-click #f)" << std::endl << std::endl;

    outputFile << "% Full Score" << std::endl;
    outputFile << "\\book {" << std::endl; // start book block

        indentlevel++;
        outputFile << indent() << "\\bookOutputName \"" << title << " - Score\"" << std::endl  << std::endl;

        outputFile << indent() << "\\paper {" << std::endl; // start paper block

            indentlevel++;

//            outputFile << paper << std::endl;
            for (list<string>::iterator it = paper.begin(); it != paper.end(); it++)
            {
                outputFile << indent() << *it << std::endl;
            }
            
            indentlevel--;

        outputFile << indent() << "}" << std::endl << std::endl; // end paper block

        outputFile << indent() << "\\header {" << std::endl; // start header block

            indentlevel++;
            outputFile << indent() << "title = \"" << title << "\"" << std::endl;
            outputFile << indent() << "subtitle = \"" << subtitle << "\"" << std::endl;
            outputFile << indent() << "composer = \"" << composer << "\"" << std::endl;
            outputFile << indent() << "date = \"" << year << "\"" << std::endl;
            outputFile << indent() << "copyright = \"© " << composer << " " << year << "\"" << std::endl;
            outputFile << indent() << "tagline = ##f" << std::endl;
            
            indentlevel--;

        outputFile << indent() << "}" << std::endl << std::endl; // end header block

        outputFile << indent() << "\\score {" << std::endl; // start score block

            indentlevel++;
            outputFile << indent() << std::endl;

        indentlevel--;
        outputFile << indent() << "}" << std::endl << std::endl; // end score block

    indentlevel--;
    outputFile << indent() << "}" << std::endl << std::endl; // end book block

    outputFile.close();
    
    return 0;
}
