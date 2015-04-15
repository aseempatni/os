#include "utils.hpp"
#include "threads.hpp"
#include "cond.hpp"
#include "bar.hpp"
#include <map>
using namespace std;

#define Tthread 5

queue<string> todo;
queue<string> todonext;
map<string,pair<uint,int> > done;
int level,mlevel;
int Nthread;
int someid;

class URLFetcher:public Thread
{
private:
    Mutex *m;
    Cond *c;
    Bar *b1;
    Bar *b2;
public:
    URLFetcher(Mutex *m_,Cond *c_,Bar *b1_,Bar *b2_):m(m_),c(c_),b1(b1_),b2(b2_)
    {}
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
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8L);
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

            linklist = getlinks(buffer,url);
            return split(linklist,',');
        }
        curl_global_cleanup();
        return vector<string>();
    }

    void copy()
    {
        while(!todonext.empty())
        {
            if(!done.count(todonext.front()))
                todo.push(todonext.front());
            todonext.pop();
        }
    }

    void levelincrease1()
    {
        m->lock();
        Nthread++;
        if(Nthread==Tthread)
        {
            Nthread = 0;
            copy();
            level++;
            printf("Level: %d\n",level);
            c->broadcast();
        }
        else
            c->wait();
    }

    void levelincrease2()
    {
        b1->wait();
        if(id()==someid)
        {
            copy();
            level++;
            printf("Level: %d\n",level);
        }
        b2->wait();

    }

    void run()
    {
        someid = id();
        while(1)
        {
            m->lock();
            if(todo.empty())
            {
                m->unlock();
                printf("Thread number %u found to-do queue empty\n",id());
                levelincrease1();
                if(level > mlevel)
                {
                    cout << "Coming out\n";
                    break;
                }
            }
            string a = todo.front();
            todo.pop();
            printf("Deque Thread:%u URL:%s\n",id(),a.c_str());
            m->unlock();

            // Time consuming part
            vector<string> linklist = fetch(a);

            m->lock();
            done[a]=make_pair(id(),level);
            for(auto a:linklist)
                todonext.push(a);
            m->unlock();
        }
    }
};

int main(int argc, char* argv[])
{
    // initialize
    if(argc !=3)
    {
        printf("./crawl <url> <level>\n");
        exit(0);
    }
    todo.push(argv[1]);
    mlevel = atoi(argv[2]);
    level = 1;
    Nthread = 0;
    URLFetcher *u[Tthread];
    Mutex m;
    Cond c(&m);
    Bar b1(Tthread),b2(Tthread);
    for(int i=0;i<Tthread;i++)
    {
        u[i] = new URLFetcher(&m,&c,&b1,&b2);
        u[i]->start();
    }

    for(int i=0;i<Tthread;i++)
        u[i]->wait();

    cout << "<depth>\t<thread>\t<url>\n";
    for(auto a:done)
        cout << a.second.second << " " << a.second.first << " " << a.first << endl;

    while(!todo.empty())
    {
        cout << level << " 0 " << todo.front() << endl;
        todo.pop();
    }
    return 0;
}
