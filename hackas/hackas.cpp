#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <istream>
#include <cmath>
#include <map>
using namespace std;
inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

string replaceAll(const string &str, const string &pattern, const string &replace)
{
	string result = str;
	string::size_type pos = 0;
	string::size_type offset = 0;

	while((pos = result.find(pattern, offset)) != string::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}

	return result;
}

class Assembler {
    private:
        string asmFileName;
        string targetFileName;
        string asmSource;
        string machineCode;
        vector< pair<string,unsigned short> > symbolTable;
        
    short lookupSymbolTable(string symName){
        for(int i=0; i<this->symbolTable.size(); i++){
            if(this->symbolTable[i].first == symName){
                return this->symbolTable[i].second;
            }
        }
        return -1;
    }

    void addSymbol(string symName, short symValue) {
        this->symbolTable.push_back(make_pair(symName, symValue));
    }
        

    public:
    Assembler() {
        this->asmFileName = "";
        
        this->targetFileName = "";
        return;
    }

    void initOpts(int argc, char *argv[]) {
        string targetFileName = ""; 
        int c;

        while((c=getopt(argc, argv, "o:")) != -1) {
            switch(c){
                case 'o':
                    targetFileName = string(optarg);
                    break;
                case '?':
                    exit(-1);
            }
        }

        if(optind == argc){
            // no asm file given
            cerr << "No input file given" << endl;
            exit(-2);
        }
        this->asmFileName = string(argv[optind]);
        if(!ends_with(this->asmFileName, ".asm")) {
            cerr << this->asmFileName << ": input file name should end with .asm" << endl;
            exit(-3);
        }
        if(targetFileName.length() == 0) {
            // targetFileName not given, defaulting to asFileName[:-4](replace ".asm" => ".hack")
            targetFileName = this->asmFileName.substr(0, this->asmFileName.length()-4) + ".hack";
        }
        if(!ends_with(targetFileName, ".hack")) {
            cerr << targetFileName << ": output file name should end with .hack" << endl;
            exit(-3);
        }
        this->targetFileName = targetFileName;
    }
    void loadFile() {
        ifstream f(this->asmFileName);
        stringstream buf;
        buf << f.rdbuf();
        this->asmSource = buf.str();
    }
    void preprocess() {
        string oldSource = this->asmSource;
        string newSource = "";

        for (int i=0; i < oldSource.length(); ++i) {
            // 3. remove comments (whenever meets '//', consume until next '\n')
            if (oldSource[i] == '/' && oldSource[i+1] == '/') {
                while(oldSource[i] != '\n') {
                    ++i;
                }
                newSource += "\n";
            }
            // 4. consume whitespaces
            else if(oldSource[i] != ' ' && oldSource[i] != '\t') {
                newSource += oldSource[i];
            }
        }
        // 1. replace \r\n with \n
        newSource = replaceAll(newSource, "\r\n", "\n");
        // 2. remove all redundant \n
        while(newSource.find("\n\n") != -1)
            newSource = replaceAll(newSource, "\n\n", "\n");
        if(newSource[0] == '\n')
            newSource = newSource.substr(1, newSource.length());
        if(newSource[newSource.length()-1])
            newSource = newSource.substr(0, newSource.length()-1);
        this->asmSource = newSource;
    }
   void initSymbols() {
        // vector of pair(symbol, value)
        auto symbolTable = &this->symbolTable;
        // put r0 ~ r15
        for(int i=0; i<16; ++i) {
            string regName = "R" + to_string(i);
            this->addSymbol(regName, i);
        }
        this->addSymbol("SCREEN", 0x4000);
        this->addSymbol("KBD", 0x6000);
        this->addSymbol("SP", 0);
        this->addSymbol("LCL", 1);
        this->addSymbol("ARG", 2);
        this->addSymbol("THIS", 3);
        this->addSymbol("THAT", 4);
    }

    void firstPass() {
        istringstream f(this->asmSource);
        string line;
        int lineNo = -1;
        
        // firstPass-1. process LABELs
        while (getline(f, line)) {
            // if line starts with opening parenthesis '(', lookup in the symbol table
            // if cur symbol doesn't exist in the symbol table, add new
            
            ++lineNo;
            //cout << "line["<<lineNo<<"] : " << line << endl;
            if(line[0] != '(')
                continue;
            
            string curSym = "";
            // check if there are more than one opening/closing parenthesis
            if(count(line.begin(), line.end(), '(') > 1){
                cerr << "Syntax Error: Opening parenthesis '(' should only occur once!" << endl;
                cerr << "line " << lineNo << " => " << line << endl;
                exit(1);
            }
            if(count(line.begin(), line.end(), ')') > 1){
                cerr << "Syntax Error: Closing parenthesis ')' should only occur once!" << endl;
                cerr << "line " << lineNo << " => " << line << endl;
                exit(1);
            }
            // line.length should be > 2, no empty label declaration allowed
            if(line.length() <= 2) {
                cerr << "Syntax Error: Empty label declaration not allowed" << endl;
                cerr << "line " << lineNo << " => " << line << endl;
                exit(2);
            }

            for(int i=1; i<line.length(); ++i){
                if(line[i] == ')'){
                    break;
                }
                else{
                    curSym += line[i];
                }
            }
            if(this->lookupSymbolTable(curSym) == -1){
                this->addSymbol(curSym, lineNo);
                --lineNo;
            }
        }
        unsigned int lastVarAddr = 15;
        // re-init stream
        f.str(this->asmSource);
        f.clear();
        while(getline(f, line)){
            // firstPass-2. process VARIABLEs
            if(line[0] != '@')
                continue;
            // if var starts with number -> it's constant
            if(line[1] >= '0' && line[1] <= '9')
                continue;
            if(line.length() <= 1) {
                cerr << "Syntax Error: Empty variable declaration not allowed" << endl;
                cerr << "line " << lineNo << " => " << line << endl;
                exit(3);
            }
            string curSym = line.substr(1, line.length());
            if(this->lookupSymbolTable(curSym) == -1){
                this->addSymbol(curSym, lastVarAddr+1);
                ++lastVarAddr;
            }
        }
    }

    void assemble() {
        istringstream f(this->asmSource);
        string line;
        while(getline(f, line)) {
            string machineLine = "";
            // skip LABEL declaration
            if(line[0] == '(')
                continue;
            
            // process A-inst
            if(line[0] == '@') {
                machineLine += "0";
                unsigned short aValue = 0;
                if (line[1] >= '0' && line[1] <= '9')
                    aValue = atoi(line.substr(1, line.length()).c_str());
                else
                    aValue = this->lookupSymbolTable(line.substr(1, line.length()));
                
                // convert decimal aConst to binary aConst
                for(int p=14; p>=0; --p) {
                    if (aValue >= pow(2, p)) {
                        aValue -= pow(2, p);
                        machineLine += "1";
                    }
                    else {
                        machineLine += "0";
                    }
                }
                this->machineCode += machineLine + "\n";
                continue;
            }

            // process C-inst
            else{
                // if line.find("=") != -1 : dest is not null
                // if line.find(";") != -1 : jump is not null
                machineLine += "111";
                string comp, dest, jump;
                string compM, destM="000", jumpM="000"; // machine codes for each
                if (line.find("=") != -1){
                    dest = line.substr(0, line.find("="));
                    if(dest.find("A") != -1)
                        destM[0] = '1';
                    if(dest.find("D") != -1)
                        destM[1] = '1';
                    if(dest.find("M") != -1)
                        destM[2] = '1';
                }
                if (line.find(";") != -1){
                    jump = line.substr(line.find(";")+1, line.length());
                    if(jump == "JGT") jumpM = "001";
                    else if(jump == "JEQ") jumpM = "010";
                    else if(jump == "JGE") jumpM = "011";
                    else if(jump == "JLT") jumpM = "100";
                    else if(jump == "JNE") jumpM = "101";
                    else if(jump == "JLE") jumpM = "110";
                    else if(jump == "JMP") jumpM = "111";
                    else jumpM = "000";
                }

                int compStart = line.find("=") == -1 ? 0 : line.find("=")+1;
                int compEnd = line.find(";") == -1 ? line.length() : line.find(";");
                comp = line.substr(compStart, compEnd);
                map<string, string> compTable = {
                    {"0","0101010"}, {"1","0111111"}, {"-1", "0111010"}, {"D", "0001100"},
                    {"A","0110000"}, {"M","1110000"}, {"!D", "0001101"}, {"!A", "0110001"},
                    {"!M","1110001"}, {"-D","0001111"}, {"-A", "0110011"}, {"-M", "1110011"},
                    {"D+1","0011111"}, {"A+1","0110111"}, {"M+1", "1110111"}, {"D-1", "0001110"},
                    {"A-1","0110010"}, {"M-1","1110010"}, {"D+A", "0000010"}, {"D+M", "1000010"},
                    {"D-A","0010011"}, {"D-M","1010011"}, {"A-D", "0000111"}, {"M-D", "1000111"},
                    {"D&A","0000000"}, {"D&M","1000000"}, {"D|A", "0010101"}, {"D|M", "1010101"}
                };
                compM = compTable[comp];
                machineLine += compM + destM + jumpM;
                //cout << "orig: " << line << " compM: " << compM << " destM: " << destM << " jumpM: " << jumpM << " machine: " << machineLine << endl ;
                this->machineCode += machineLine + "\n";
                continue;
            }
        }
    }

    void writeToFile() {
        ofstream f(this->targetFileName);
        f << this->machineCode;
        f.close();
    }
};

int main(int argc, char *argv[]){
    Assembler as;
    as.initOpts(argc, argv);
    as.loadFile();
    as.preprocess(); // remove comments, whitespace
    as.initSymbols(); // generate symbol tables and initialize with predefines(Screen, R0, etc.)
    as.firstPass(); // generate symbol table
    as.assemble(); // let's go!
    as.writeToFile();
    
}
