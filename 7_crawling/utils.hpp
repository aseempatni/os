#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <regex.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <queue>
#include <set>

#define BUFFER_SIZE 1000000

using namespace std;

string getlinks(string buffer,string url)
{
    int c[2];
    int p[2];
    pipe(p);
    pipe(c);
    if(!fork())
    {
        // child
        close(p[1]);
        dup2(p[0],0); // stdin
        close(c[0]);
        dup2(c[1],1); //stdout
        
        execl("./getlinks.py","");
        exit(0);
    }
    close(p[0]);
    close(c[1]);
    
    char temp[BUFFER_SIZE];
    url += "\n";
    strcpy(temp,url.c_str());
    write(p[1],temp,url.length());

    buffer+="\nENDFILE\n";
    strcpy(temp,buffer.c_str());
    write(p[1],temp,sizeof(temp));
    
    int t = read(c[0],temp,sizeof(temp));
    
    string result(temp,t);
    return result;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

static int writer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
  if (writerData == NULL)
    return 0;

  writerData->append(data, size*nmemb);

  return size * nmemb;
}