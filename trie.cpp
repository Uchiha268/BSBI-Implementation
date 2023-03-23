#include<bits/stdc++.h>
using namespace std;

class Trie {
private:
    struct trieNode{
        trieNode* alp[128];
        bool isEnd;
        map<int,int> docFreq;
    };
    trieNode* root;
    trieNode* createNode(){
        trieNode* newnode=new trieNode();
        for(int i=0; i<128; i++){
            newnode->alp[i]=nullptr;
        }
        newnode->isEnd=false;
        return newnode;
    }
public:

    Trie() {
        root = createNode();
    }

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

    void insert(string line) {
        trieNode* curr = root;
        vector<string> lineSplit = split(line," ");
        string word = lineSplit[0];
        for(int i = 0;i < word.length();i++){
            if(curr->alp[int(word[i])] == nullptr){
                trieNode* newNode = createNode();
                curr->alp[int(word[i])] = newNode;
            }
            curr = curr->alp[int(word[i])];
        }
        curr->isEnd = true;
        for(int i = 1;i+1 < lineSplit.size();i+=2){
            curr->docFreq[stoi(lineSplit[i])] = stoi(lineSplit[i+1]);
        }
    }
    
    map<int,int> search(string word) {
        trieNode* curr = root;
        for(int i = 0;i<word.length();i++){
            if(curr->alp[int(word[i])] == nullptr){
                return {};
            }
            curr = curr->alp[int(word[i])];
        }
        if(curr->isEnd){
            return curr->docFreq;
        }
        return {};
    }
};

int main(){
    Trie* obj = new Trie();
    string line;
    ifstream invInd("inverted_index.txt");
    while(getline(invInd,line)){
        obj->insert(line);
    }
    int input;
    while(input!=2){
        cout << "Enter 1: Search" << endl << "Enter 2: Quit" << endl;
        cin >> input;
        if(input == 1){
            string word;
            getline(cin,word);
            map<int,int> docIDF = obj->search(word);
            for(auto i : docIDF){
                cout << "DocID:" << i.first << "Freq:" << i.second << endl;
            }
        }
    }
    return 1;
}