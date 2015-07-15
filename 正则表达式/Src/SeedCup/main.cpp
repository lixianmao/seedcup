#include <iostream>
#include <fstream>
#include <cstdio>
#include <stdexcept>
#include "RegexpToNfa.h"

struct filestr
{
    std::string r;
    std::string t;
};

std::vector<filestr> fstr;

std::string readTestText(std::string filename)
{
        std::string temp;
        std::string text = "";
        std::ifstream infile(filename.c_str());
        while(getline(infile,temp)) {
            text = text + temp +'\n';
        }
        text.erase(text.length()-1);
        return text;
}

void readdata()
{
    std::ifstream rege("regex.txt");
    std::vector<filestr>::iterator iter;
    filestr files;
    int i=0;
    char reg[1025],filename[100];
    std::string txt="";
    while(rege.getline(reg,1025))
    {
        if(reg[0]=='\0')
            break;
        std::string str(reg);
        files.r=str;
        i++;
        sprintf(filename,"text%d.txt",i);
        files.t = readTestText(std::string(filename));
        fstr.push_back(files);
    }
    return;
}

int main(void)
{
    int i=1;
    readdata();
    for(std::vector<filestr>::iterator iter = fstr.begin();iter != fstr.end();++iter) {
        const char *str = (*iter).t.c_str();
        std::string regular = (*iter).r;
        RegexpToNfa regexp(str,regular);
        FILE* output;
        char filename[100];
        sprintf(filename,"output%d.txt",i++);
        output = fopen(filename,"w");
        try {
             fprintf(output,"%s",regexp.getResult().data());
        } catch (std::runtime_error &e) {
            std::cout<<e.what()<<std::endl;
        }
        fclose(output);
    }
    system("PAUSE");
    return 0;
}
