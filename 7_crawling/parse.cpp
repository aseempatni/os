#include "utils.hpp"
#include "threads.hpp"
using namespace std;

queue<string> todo;
queue<string> todonext;
vector<pair<string,uint> > done;
int level;

void levelincrease()
{

}

class URLFetcher:public Thread
{
private:
public:
    vector<string> fetch(string url)
    {
        string buffer;
        string linklist;
        CURL *curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if(curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if(res != CURLE_OK)
              fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
            /* always cleanup */
            curl_easy_cleanup(curl);
            linklist = getlinks(buffer);
            return split(linklist,',');
        }
        curl_global_cleanup();
        return vector<string>();
    }
    void run()
    {
        while(1)
        {
            if(todo.empty())
                levelincrease();
            string a = todo.front();
            todo.pop();
            vector<string> linklist = fetch(a);

            for(auto a:linklist)
                todonext.push(a);
        }
    }
};

int main(int argc, char* argv[])
{
    // initialize
    todo.push(argv[1]);
    level = 1;
    URLFetcher u[5];
    for(int i=0;i<5;i++)
        u[i].start();
    while(1)
    {}
    return 0;
}