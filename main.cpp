#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;

void addDataToFile(string filename, string value) {
    ofstream outfile;
    outfile.open(filename, ios::app);
    outfile << value << endl;
    outfile.close();
}

void deleteLineInFile(string filename, string line_value) {
    ifstream infile;
    infile.open(filename);
    string line;
    stringstream ss;
    while (getline(infile, line)) {
        if (line.find(line_value) != string::npos) {
            continue;
        }
        ss << line << endl;
    }
    infile.close();
    ofstream outfile;
    outfile.open(filename, ios::out);
    outfile << ss.str() << endl;
    outfile.close();
}

bool compareAscendingOrder(string a, string b) {
    return a < b;
}

void readDataFromFile(string filename, bool printDiseasesOnly = false) {
    ifstream infile;
    infile.open(filename);
    string line;
    vector<string> foundFileContent{};

    while (getline(infile, line)) {
        if (printDiseasesOnly) {
            vector<string> caseParts{};
            size_t pos = 0;
            while((pos = line.find(",")) != string::npos) {
                caseParts.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            string disease = (caseParts.size() > 1 ? caseParts.at(1) : "");
            if (disease.empty()) {
                continue;
            }
            if (!count(foundFileContent.begin(), foundFileContent.end(), disease)) {
                foundFileContent.push_back(disease);
            }
        } else {
            foundFileContent.push_back(line);
        }
    }
    infile.close();
    sort(foundFileContent.begin(), foundFileContent.end(), compareAscendingOrder);
    for (auto value : foundFileContent) {
        cout << value << endl;
    }
}

bool isDuplicateRecordInFile(string filename, string line_value) {
    ifstream infile;
    infile.open(filename);
    string line;
    while (getline(infile, line)) {
        if (line.compare(line_value) == 0) {
            return true;
        }
    }
    infile.close();
    return false;
}

string changeStringCase(string str, bool toLower = false) {
    for_each(str.begin(), str.end(), [toLower](char& c) {
        c = toLower ? c = ::tolower(c) : c = ::toupper(c);
    });
    return str;
}

vector<string> findRecordsByDisease(string filename, string disease) {
    vector<string> recordResults{};
    ifstream infile;
    infile.open(filename);
    string line;

    while(getline(infile, line)) {
        if (line.find(disease) != string::npos) {
            recordResults.push_back(line.substr(0, line.find(",")));
        }
    }
    infile.close();
    return recordResults;
}

int countNumberOfDiseaseCases(string filename, string disease, string location) {
    int cases = 0;
    string line;
    ifstream infile;
    infile.open(filename);

    while (getline(infile, line)) {
        if (
            line.find(disease) != string::npos && 
            (location.empty() ? true : line.find(location) != string::npos)
        ) {
            size_t pos = 0;
            while ((pos = line.find(",")) != string::npos) {
                line.erase(0, pos + 1);
            }
            cases += stoi(line);
        }
    }
    infile.close();
    return cases;
}

int helpMenu() {
    cout << "Need any help? Type 'help' then press Enter key."<<endl;

    string command;

    do {
        cout << "Console >";
        getline(cin, command);

        for_each(command.begin(), command.end(), [](char& c) {
            c = ::tolower(c);
        });
        // do this with a for loop
        // for (char c : command) {
        //     c = ::tolower(c);
        // }

        system("cls"); // for windows and linux use ("clear")

        string commandCopy = command;
        string space_delimiter = " ";
        vector<string> splitCommands{};

        size_t pos = 0;
        while ((pos = commandCopy.find(space_delimiter)) != string::npos) {
            splitCommands.push_back(commandCopy.substr(0, pos));
            commandCopy.erase(0, pos + space_delimiter.length());
        }
        splitCommands.push_back(commandCopy);

        // print the commands to execute
        // for (const auto& value : splitCommands) {
        //     cout << value + " ";
        // }

        if (splitCommands.at(0).compare("add") == 0) {
            if (isDuplicateRecordInFile("locations.csv", changeStringCase(splitCommands.at(1)))) {
                cout << "This location already exists" << endl;
                continue;
            }
            addDataToFile("locations.csv", changeStringCase(splitCommands.at(1)));
            cout << "Location " << changeStringCase(splitCommands.at(1)) << " is successfully added." << endl;
        } else if (splitCommands.at(0).compare("delete") == 0) {
            if (!isDuplicateRecordInFile("locations.csv", changeStringCase(splitCommands.at(1)))) {
                cout << "This location doesn't exist" << endl;
                continue;
            }
            deleteLineInFile("locations.csv", changeStringCase(splitCommands.at(1)));
            deleteLineInFile("cases.csv", changeStringCase(splitCommands.at(1)));
            cout << "Location " << changeStringCase(splitCommands.at(1)) << " is successfully deleted." << endl;
        } else if (splitCommands.at(0).compare("record") == 0) {
            string record = changeStringCase(splitCommands.at(1)).append(",")
                .append(changeStringCase(splitCommands.at(2))).append(",")
                .append(splitCommands.at(3));
            if (isDuplicateRecordInFile("cases.csv", record)) {
                cout << "This record already exists!";
                continue;
            }
            addDataToFile("cases.csv", record);
            cout << "Record " << changeStringCase(splitCommands.at(1)) 
                << " " << splitCommands.at(2) 
                << " " << splitCommands.at(3) << endl;
        } else if (splitCommands.at(0).compare("list") == 0) {
            if (splitCommands.at(1).compare("locations") == 0) {
                readDataFromFile("locations.csv");
            } else if (splitCommands.at(1).compare("diseases") == 0) {
                readDataFromFile("cases.csv", true);
            }
        } else if (splitCommands.at(0).compare("where") == 0) {
            vector<string> result = findRecordsByDisease("cases.csv", changeStringCase(splitCommands.at(1)));
            for (const auto& data : result) {
                cout << data << endl;
            }
            if (result.size() == 0) {
                cout << "No location with this disease" << endl;
            }
        } else if (splitCommands.at(0).compare("cases") == 0) {
            if (splitCommands.size() == 2) {
                cout << "Total cases of '" << changeStringCase(splitCommands.at(1)) 
                    << "' = " 
                    << countNumberOfDiseaseCases("cases.csv", changeStringCase(splitCommands.at(1)), "") << endl;
            } else if (splitCommands.size() == 3) {
                cout << "Cases of " << changeStringCase(splitCommands.at(2)) 
                    << " at " << changeStringCase(splitCommands.at(1)) << " are: " 
                    << countNumberOfDiseaseCases(
                        "cases.csv", changeStringCase(splitCommands.at(2)), changeStringCase(splitCommands.at(1))
                    ) << endl;
            }
        } else if (splitCommands.at(0).compare("help") == 0) {
            cout << "================================================================================"<<endl;
            cout << "*                            H E L P     M E N U                               *"<<endl;
            cout << "================================================================================"<<endl;
            cout << "add <Location>                              :Add a new location"<<endl;
            cout << "delete <Location>                           :Delete an existing location"<<endl;
            cout << "record <Location> <disease> <cases>         :Record a disease and its cases"<<endl;
            cout << "list locations                              :List all existing locations"<<endl;
            cout << "list diseases                               :List existing diseases in locations"<<endl;
            cout << "where <disease>                             :Find where disease exists"<<endl;
            cout << "cases <Location> <disease>                  :Find cases of a disease in location"<<endl;
            cout << "cases <disease>                             :Find total cases of a given disease"<<endl;
            cout << "help                                        :Prints user manual"<<endl;
            cout << "Exit                                        :Exit the program"<<endl;
            cout << "================================================================================"<<endl;
        } else if (splitCommands.at(0).compare("exit") == 0){
            return 0;
        } else {
            cout << "Invalid command !"<<endl;
        }

    } while (command.compare("exit") != 0);
    return 0;
}

int main() {
    cout << "================================================================================" << endl;
    cout << "*                  Welcome To Disease Cases Reporting System!                  *" << endl;
    cout << "********************************************************************************" << endl;
    cout << "*     Developed by ABIJURU Seth as practical preparation for end of Year 3     *" << endl;
    cout << "********************************************************************************" << endl;
    cout << "================================================================================" << endl;

    helpMenu();

    cout << "================================================================================" << endl;
    cout << "*                                     Bye!!                                    *" << endl;
    cout << "================================================================================" << endl;

    return 0;
}
