#include<bits/stdc++.h>
#define DOC_COUNT 30
using namespace std;

vector<string> split(string str, string delimiter){
    vector<string> v;
    if (!str.empty()) {
        int start = 0;
        do{
            int idx = str.find(delimiter, start);
            if (idx == string::npos) {
                break;
            }
            int length = idx - start;
            v.push_back(str.substr(start, length));
            start += (length + delimiter.size());
        } while (true);
        v.push_back(str.substr(start));
    }
    return v;
}

vector<int> getTotalTermsPerDoc(string file){
    vector<int> tots(DOC_COUNT,0);
    ifstream fp;
    fp.open(file);
    string line;
    while(getline(fp,line)){
        vector<string> lineSplit = split(line," ");
        // for(int i = 1;i+1 < lineSplit.size();i+=2){
        //     tots[stoi(lineSplit[i])-1]+=stoi(lineSplit[i+1]);
        // }
        if(lineSplit[0] == "abstraction")
        {
            int x = 0;
        }
        int i = 1;
        while(i < lineSplit.size())
        {
            tots[stoi(lineSplit[i]) - 1] += stoi(lineSplit[i + 1]);
            i++;
            int term_freq = stoi(lineSplit[i]);
            for(int j = 0; j < term_freq; j++, i++){}
            i++;
        }
    }
    return tots;
    
}

double getIDF(vector<string> s){
    // int wordDocs = (s.size()-1)/2;
    int wordDocs = 0;
    int i = 1;
    while(i < s.size())
    {
        wordDocs++;
        i++;
        int term_freq = stoi(s[i]);
        for(int j = 0; j < term_freq; j++, i++){}
        i++;
    }
    return log10(DOC_COUNT/wordDocs);
}

map<int,double> getTF(vector<string> s,vector<int> docTot){
    map<int,double> tfDoc;
    // for(int i = 1;i+1 < s.size();i+=2){
    //     tfDoc[stoi(s[i])] = stod(s[i+1])/docTot[stoi(s[i])-1];
    // }
    int i = 1;
    while(i < s.size())
    {
        tfDoc[stoi(s[i])] = stod(s[i + 1]) / docTot[stoi(s[i]) - 1];
        i++;
        int term_freq = stoi(s[i]);
        for(int j = 0; j < term_freq; j++, i++){}
        i++;
    }
    return tfDoc;
}


int main(){
    vector<int> docTot = getTotalTermsPerDoc("inverted_index.txt");

    // for(int i = 0;i < DOC_COUNT;i++){
    //     cout << docTot[i] << endl;
    // }

    // cout << getIDF({"hello","5","6"});

    // vector<string> s = {"hello","5","1234","4","10000"};
    // map <int,double> tfDoc = getTF(s,docTot);
    // for(auto i:tfDoc){
    //     cout << i.first << " " << i.second << endl;
    // }

    ofstream fout;
    fout.open("tf_idf.txt");
    ifstream fp;
    fp.open("inverted_index.txt");
    string line;
    while(getline(fp,line)){
        vector<string> lineSplit = split(line," ");
        double IDF = getIDF(lineSplit);
        map<int,double> TF = getTF(lineSplit,docTot);
        int i = 0;
        fout << lineSplit[i] << " " << IDF;
        i++;
        for(auto j : TF){
			fout << " " << j.first << " " << j.second;
            i++;
            int term_freq = stoi(lineSplit[i]);
            fout << " " << term_freq;
            i++;
            for(int k = 0; k < term_freq; k++, i++)
            {
                fout << " " << lineSplit[i];
            }
		}
		fout << endl;
    }
    fout.close();

    return 0;
}