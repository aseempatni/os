#include <algorithm>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>
using namespace std;

void Tokenize(const string& str,
        vector<string>& tokens,
        const string& delimiters) {
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos = str.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters. Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

int fifo(string reference_string, int nframes) {
    vector<int> v;
    int page_fault=0;
    vector<string> tokens;
    Tokenize(reference_string,tokens," ");
    vector<int> vint;
    int i;
    for (i=0; i<tokens.size(); i++ ) {
        vint.push_back(atoi(tokens[i].c_str()));
    }
    for(i=0; i<vint.size(); i++) {
        if(find(v.begin(), v.end(),vint[i]) != v.end()) {
            /* v contains x */
        } else {
            /* v does not contain x */
            if(v.size()==nframes) {
                v.erase(v.begin());
            }
            v.push_back(vint[i]);
            page_fault++;
        }
    }
    return page_fault;
}

int lru(string reference_string, int nframes) {
    vector<int> v;
    int page_fault=0;
    vector<string> tokens;
    Tokenize(reference_string,tokens," ");
    vector<int> vint;
    int i;
    for (i=0; i<tokens.size(); i++ ) {
        vint.push_back(atoi(tokens[i].c_str()));
    }
    for(i=0; i<vint.size(); i++) {
        std::vector<int>::iterator it;
        it = find(v.begin(), v.end(),vint[i]);
        if(it != v.end()) {
            /* v contains x */
            v.erase(it);
        } else {
            /* v does not contain x */
            if(v.size()==nframes) {
                v.erase(v.begin());
            }
            page_fault++;
        }
        v.push_back(vint[i]);
    }
    return page_fault;
}

struct lfu_struct {
    lfu_struct (int page_no): page_no (page_no){frequency=1;};
    int page_no;
    int frequency;
};

int lfu (string reference_string, int nframes) {
    vector<int> v;
    int page_fault=0;
    vector<string> tokens;
    Tokenize(reference_string,tokens," ");
    vector<int> vint;
    vector <lfu_struct> loaded_pages;
    int i;
    for (i=0; i<tokens.size(); i++ ) {
        vint.push_back(atoi(tokens[i].c_str()));
    }
    for(i=0; i<vint.size(); i++) {
        std::vector<lfu_struct>::iterator it;
        int found = 0, min_freq, min_freq_index;
        if(loaded_pages.size() >0 ) min_freq = loaded_pages[0].frequency;
        for(it = loaded_pages.begin(); it < loaded_pages.end(); it++) {
            if(it->frequency <= min_freq) {
                min_freq = it->frequency;
                min_freq_index = it-loaded_pages.begin();
            }
            if(it->page_no==vint[i]) {
                // already present
                found = 1;
                it->frequency++;
            }
        }
        if (found==0) {
            // not already present
            page_fault++;
            if(loaded_pages.size() == nframes) {
                vector<lfu_struct>::iterator nth = loaded_pages.begin() + min_freq_index;
                loaded_pages.erase(nth);
            }
            loaded_pages.push_back(*new lfu_struct(vint[i]));
        }
    }
    return page_fault;
}

int second_chance (string reference_string, int nframes) {
    vector<int> v;
    vector<bool> reference_bit;
    int page_fault=0;
    vector<string> tokens;
    Tokenize(reference_string,tokens," ");
    vector<int> vint;
    int i;
    for (i=0; i<tokens.size(); i++ ) {
        vint.push_back(atoi(tokens[i].c_str()));
    }
    for(i=0; i<vint.size(); i++) {
        std::vector<int>::iterator it;
        it = find(v.begin(), v.end(),vint[i]);
        if(it != v.end()) {
            /* v contains x */
            reference_bit[it-v.begin()] =true;
        } else {
            /* v does not contain x */
            if(v.size()==nframes) {
                for(int i=0; i<v.size(); i++) {
                    if (reference_bit[i]==true) reference_bit[i] = false;
                    else {
                        v.erase(v.begin()+i);
                        reference_bit.erase(reference_bit.begin()+i);
                        break;
                    }
                }
                if(it==v.end()) {
                    v.erase(v.begin());
                    reference_bit.erase(reference_bit.begin());
                }
            }
            v.push_back(vint[i]);
            reference_bit.push_back(true);
            page_fault++;
        }
    }
    return page_fault;
}

string generate_reference_string(int n, int d) {
    stringstream os;
    int i, j, k;
    for (i=0; i<n; i++ ) {
        for(j=0; j<n; j++ ) {
            for(k=0; k<n; k++) {
                os << 101+(i*n+k)/d << " " << 501+(k*n+j)/d << " ";
            }
            os << 1001+(i*n+j)/d << " ";
        }
    }
    return os.str();
}

int main() {
    int n, d;
    cout << "n = ";
    cin >> n;
    cout << "d = ";
    cin >> d;
    string reference_string = generate_reference_string(n,d);
    //cout << reference_string;
    int no_of_frames, fault_count;
    int i;
    cout << endl;
    cout << "frames\tfifo\tlru\tlfu\tsecond chance" << endl;
    for(i=1; i<3*n*n/d+d; i++) {
        no_of_frames = i;
        cout << i << "\t";
        fault_count = fifo(reference_string, no_of_frames);
        cout << fault_count << "\t";
        fault_count = lru(reference_string, no_of_frames);
        cout << fault_count << "\t";
        fault_count = lfu(reference_string, no_of_frames);
        cout << fault_count << "\t";
        fault_count = second_chance(reference_string, no_of_frames);
        cout << fault_count << "\t";
        cout << endl;
    }
/*    
    while (1) {
        cout << endl << "Select a page replacement algorithm." << endl;
        cout << "No of frames: ";
        cin >> no_of_frames;
        fault_count = fifo(s,no_of_frames);
        cout << fault_count << endl;
        fault_count = lru(s,no_of_frames);
        cout << fault_count << endl;

        //getchar();
    }
*/
}
