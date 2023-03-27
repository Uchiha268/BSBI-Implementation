#include<bits/stdc++.h>
#define A 10 //No. of representatives chosen from each posting list
#define K 10 //Top K elements of potential docs
#define PRESENT_IN_LIMIT 3
using namespace std;

static const string DOC_PREFIX = "./DOCS/";

class Trie {
private:
    struct trieNode{
        trieNode* alp[128];
        bool isEnd;
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
    
    void insert(string word) {
        trieNode* curr = root;
        for(int i = 0;i < word.length();i++){
            if(curr->alp[int(word[i])] == nullptr){
                trieNode* newNode = createNode();
                curr->alp[int(word[i])] = newNode;
            }
            curr = curr->alp[int(word[i])];
        }
        curr->isEnd = true;
    }
    
    bool search(string word) {
        trieNode* curr = root;
        for(int i = 0;i<word.length();i++){
            if(curr->alp[int(word[i])] == nullptr){
                return false;
            }
            curr = curr->alp[int(word[i])];
        }
        return curr->isEnd;
    }

    set<string> startsWith(string prefix) {
    trieNode* curr = root;
    set<string> wordSet;
    int letterASCII;
    for(int i = 0; i < prefix.length(); i++){
        if(curr->alp[int(prefix[i])] == nullptr){
            return {};
        }
        curr = curr->alp[int(prefix[i])];
    }
    string currWord = prefix;
    startsWithHelper(curr, wordSet, currWord);
    return wordSet;
    }

    void startsWithHelper(trieNode* curr, set<string>& wordSet, string& currWord) {
        if(curr == nullptr){
            return;
        }
        if(curr->isEnd){
            wordSet.insert(currWord);
        }
        for(int i = 0; i < 128; i++){
            if(curr->alp[i]!=nullptr){
                currWord = currWord + char(i);
                startsWithHelper(curr->alp[i], wordSet, currWord);
                currWord = currWord.substr(0, currWord.length() - 1);
            }
        }
    }
};

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

string getFirstWord(string line){
    string word;
    for (char c : line) {
        if (c == ' ' || c == '.') {break;}
        word += c;
    }
    return word;
}
string reverseString(string str) {
    int n = str.length();
    for (int i = 0; i < n / 2; i++) {
        char temp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = temp;
    }
    return str;
}

set<string> queryLocator(string query,Trie* standard,Trie* reverse){
    set<string> queryOut;
    if(query[0] == '*'){
        set<string> querySub;
        query = query.substr(1);
        querySub = reverse->startsWith(reverseString(query));
        for(auto i:querySub){
            queryOut.insert(reverseString(i));
        }
    }
    else if(query[query.length()-1] == '*'){
        query.pop_back();
        queryOut = standard->startsWith(query);
    }
    else{
        vector<string> querySplit = split(query,"*");
        set<string> querySub;
        set<string> queryLeft;
        set<string> queryRight;
        querySub = reverse->startsWith(reverseString(querySplit[1])); 
        for(auto i:querySub){
            queryRight.insert(reverseString(i));
        }
        queryLeft = standard->startsWith(querySplit[0]);
        set_intersection(begin(queryLeft),end(queryLeft),begin(queryRight),end(queryRight),inserter(queryOut, end(queryOut))); 
    }
    return queryOut;
}

vector<Trie*> getStandardAndReverseTrie(string filename){
    vector<Trie*> trieVec(2,nullptr);
    trieVec[0] = new Trie();
    trieVec[1] = new Trie();
    ifstream fp("dictionary.txt");
    string line;
    while(getline(fp,line)){
        string word = getFirstWord(line);
        trieVec[0]->insert(word);
        trieVec[1]->insert(reverseString(word));
    }
    return trieVec;
}

class myIDFComparator
{
	public:
		int operator() (const pair<string, float> &p1, const pair<string, float> &p2)
		{
			return p1.second < p2.second;
		}
};

class myComparator
{
	public:
		int operator() (const pair<int, double> &p1, const pair<int, double> &p2)
		{
			return p1.second < p2.second;
		}
};

string mystringtolower(string word)
{
	for(int i = 0; i < word.size(); i++)
	{
		if(isupper(word[i]))
		{
			word[i] = tolower(word[i]);
		}
	}
	return word;
}

unordered_set<string> get_stop_words(string file)
{
	ifstream fp;
	fp.open(file);
	string word;
	unordered_set<string> *stop_words = new unordered_set<string>;
	while(fp >> word)
	{
		(*stop_words).insert(word);
	}
	return (*stop_words);
}

vector<int> get_docs_from_posting_list(string term, ifstream *index, map< string, pair< int, long int> > dictionary)
{
	vector<int> docs;
	(*index).seekg(dictionary[term].second);
	string line;
	getline((*index), line);
	vector<string> lineSplit = split(line, " ");
	int i = 2;
	while(i < lineSplit.size())
	{
		docs.push_back(stoi(lineSplit[i]));
		i += 2;
		int term_freq = stoi(lineSplit[i]);
		i++;
		for(int j = 0; j < term_freq; j++, i++){}
	}
	return docs;
}

vector<int> boolean_and(vector<int> l1, vector<int> l2)
{
	int p1 = 0, p2 = 0;
	vector<int> result;
	while(p1 < l1.size() && p2 < l2.size())
	{
		if(l1[p1] == l2[p2])
		{
			result.push_back(l1[p1]);
			p1++; p2++;
		}
		else if(l1[p1] < l2[p2])
		{
			p1++;
		}
		else
		{
			p2++;
		}
	}
	return result;
}

vector<int> boolean_or(vector<int> l1, vector<int> l2)
{
	int p1 = 0, p2 = 0;
	vector<int> result;
	while(p1 < l1.size() && p2 < l2.size())
	{
		if(l1[p1] == l2[p2])
		{
			result.push_back(l1[p1++]);
			p2++;
		}
		else if(l1[p1] < l2[p2])
		{
			result.push_back(l1[p1++]);
		}
		else
		{
			result.push_back(l2[p2++]);
		}
	}
	while(p1 < l1.size())
	{
		result.push_back(l1[p1++]);
	}
	while(p2 < l2.size())
	{
		result.push_back(l2[p2++]);
	}
	return result;
}

vector<int> boolean_not(vector<int> l1, vector<int> l2)
{
	int p1 = 0, p2 = 0;
	vector<int> result;
	while(p1 < l1.size() && p2 < l2.size())
	{
		if(l1[p1] == l2[p2])
		{
			p1++; p2++;
		}
		else if(l1[p1] < l2[p2])
		{
			result.push_back(l1[p1++]);
		}
		else
		{
			p2++;
		}
	}
	if(p2 >= l2.size())
	{
		while(p1 < l1.size())
		{
			result.push_back(l1[p1++]);
		}
	}
	return result;
}



map< string, pair< int, long int > > load_dictionary(string dict_file)
{
	ifstream fp;
	fp.open(dict_file);
	map< string, pair< int, long int> > *dict = new map< string, pair< int, long int> >;
	string line;
	while(getline(fp, line))
	{
		vector<string> lineSplit = split(line, " ");
		(*dict)[lineSplit[0]] = make_pair(stoi(lineSplit[1]), stol(lineSplit[2]));
	}
	return (*dict);
}

void get_scores(vector<string> terms, unordered_map<int, double> &pot_docs, map<string, pair <int, long int> > dictionary, ifstream *index, priority_queue< pair<string, float>, vector<pair<string, float> >, myIDFComparator> &idfpq)
{
	for(auto term : terms)
	{
		if(dictionary.find(term) != dictionary.end())
		{
			(*index).seekg(dictionary[term].second);
			string index_entry;
			getline((*index), index_entry);
			vector<string> index_entry_split = split(index_entry, " ");
			int i = 1, j = A;
			float idf = stof(index_entry_split[i]);
			i++;
			idfpq.push(make_pair(term, idf));
			while(i < index_entry_split.size() && j--)
			{
				int doc_id = stoi(index_entry_split[i]);
				i++;
				float tf = stof(index_entry_split[i]);
				i++;
				int term_freq = stoi(index_entry_split[i]);
				i++;
				for(int k = 0; k < term_freq; k++, i++){}
				double score = idf * (1 + (double)log(term_freq));
				if(pot_docs.find(doc_id) == pot_docs.end())
				{
					pot_docs[doc_id] = 0;
				}
				pot_docs[doc_id] += score;
			}
		}
	}
}

priority_queue< pair<int, double>, vector<pair<int, double> >, myComparator > rank_docs(unordered_map<int, double> pot_docs)
{
	priority_queue< pair<int, double>, vector<pair<int, double> >, myComparator> *pq = new priority_queue< pair<int, double>, vector<pair<int, double> >, myComparator>;
	for(auto i : pot_docs)
	{
		(*pq).push(make_pair(i.first, i.second));
	}
	return *(pq);
}

void print_results(priority_queue< pair<int, double>, vector<pair<int, double> >, myComparator > pq, priority_queue< pair<string, float>, vector<pair<string, float> >, myIDFComparator> idfpq, ifstream *index, map< string, pair< int, long int> > dictionary)
{
	int k = K;
	vector<pair<string, float> > top_idf;
	while(!idfpq.empty())
	{
		top_idf.push_back(idfpq.top());
		idfpq.pop();
	}
	vector< pair<int, double> >top_k;
	while(!pq.empty() && k--)
	{
		top_k.push_back(pq.top());
		pq.pop();
	}
	unordered_map<string, map<int, long int> > first_locs;
	for(auto i : top_idf)
	{
		string term = i.first;
		string line;
		(*index).seekg(dictionary[term].second);
		getline((*index), line);
		vector<string> lineSplit = split(line, " ");
		int j = 2, l = K;
		while(j < lineSplit.size() && l)
		{
			int doc_id = stoi(lineSplit[j]);
			j++;
			float tf = stof(lineSplit[j]);
			j++;
			int term_freq = stoi(lineSplit[j]);
			j++;
			long int first_loc = stol(lineSplit[j]);
			for(int m = 0; m < term_freq; m++, j++){}

			int flag = 0;
			for(auto doc_entry : top_k)
			{
				if(doc_entry.first == doc_id)
				{
					flag = 1;
					break;
				}
			}
			if(flag)
			{
				first_locs[term].insert(make_pair(doc_id, first_loc));
				l--;
			}
		}
	}
	for(auto i : top_k)
	{
		cout << "document" << i.first << ".txt" << endl;
		cout << "Query terms present in - " << endl << endl;
		int present_limit = PRESENT_IN_LIMIT;
		vector<long int> prev_locs;
		for(auto j : top_idf)
		{
			if(present_limit)
			{
				if(first_locs[j.first].find(i.first) != first_locs[j.first].end())
				{
					int flag = 1;
					for(auto prev : prev_locs)
					{
						long int cur_loc = first_locs[j.first][i.first];
						if(cur_loc >= prev - 30 && cur_loc <= prev + 30)
						{
							flag = 0;
						}
					}
					if(flag)
					{
						ifstream fin;
						fin.open(DOC_PREFIX + "document" + to_string(i.first) + ".txt");
						fin.seekg(first_locs[j.first][i.first] - 20);
						char c;
						int word_count = 10;
						string word;
						while(fin >> word && word_count--)
						{
							cout << word << " ";
						}
						cout << endl;
						present_limit--;
						prev_locs.push_back(first_locs[j.first][i.first]);
					}
				}
			}
			else
			{
				break;
			}
		}
		cout << endl << endl;
	}
}

int main()
{
	ifstream index;
	index.open("sorted_tf_idf.txt");
	map< string, pair< int, long int> > dictionary = load_dictionary("dictionary.txt");
	unordered_set<string> stop_words = get_stop_words("stop_words.txt");
	unordered_map< int, double > potential_docs;
	vector<Trie*> trieVec = getStandardAndReverseTrie("dictionary.txt");

	while(true)
	{
		string choice;
		cout << "What kind of query do you want to make?" << endl << "1)Phrase" << endl << "2)Boolean" << endl << "3)WildCard" << endl << "4)End" << endl << endl << "Enter your choice - ";
		getline(cin, choice);

		char ch = choice[0];
		switch(ch)
		{
			case '1' :	{
							string query;
							cout << "Enter your query - " << endl;
							getline(cin, query);
							query = mystringtolower(query);

							vector<string> query_terms_with_stop_words = split(query, " ");
							vector<string> query_terms;
							for(auto i : query_terms_with_stop_words)
							{
								if(stop_words.find(i) == stop_words.end())
								{
									query_terms.push_back(i);
								}
							}
							vector<string> query_bi_word_terms;
							for(int i = 1; i < query_terms.size(); i++)
							{
								query_bi_word_terms.push_back((query_terms[i - 1] + "." + query_terms[i]));
							}
							priority_queue< pair<string, float>, vector<pair<string, float> >, myIDFComparator> idfpq;
							get_scores(query_bi_word_terms, potential_docs, dictionary, &index, idfpq);
							get_scores(query_terms, potential_docs, dictionary, &index, idfpq);
							priority_queue< pair<int, double>, vector<pair<int, double> >, myComparator > pq = rank_docs(potential_docs);
							cout << endl << endl << "Results are - " << endl << endl;
							print_results(pq, idfpq, &index, dictionary);
							cout << endl << endl << "Enter the search results (1 - 10) which seemed relevant to you separated by spaces (eg: 1 2 6). If you are satisfied with the output, then enter any other character - " << endl;
							string relev_results;
							getline(cin, relev_results);
							vector<string> relev_results_split = split(relev_results, " ");
							if(isdigit(relev_results_split[0][0]))
							{
								vector<int> top_k;
								int k = K;
								while(!pq.empty() && k--)
								{
									top_k.push_back(pq.top().first);
									pq.pop();
								}
								for(auto i : relev_results_split)
								{
									int doc_id = top_k[stoi(i) - 1];
									ifstream doc_fp;
									doc_fp.open(DOC_PREFIX + "document" + to_string(doc_id) + ".txt");
									string prev_word = "";
									string word;
									int wc = 5;
									while(doc_fp >> word && wc)
									{
										word = mystringtolower(word);
										if(stop_words.find(word) == stop_words.end() && dictionary.find(word) != dictionary.end())
										{
											query_terms.push_back(word);
											query_bi_word_terms.push_back((prev_word + "." + word));
											prev_word = word;
											wc--;
										}
									}
								}
								potential_docs.clear();
								priority_queue< pair<string, float>, vector<pair<string, float> >, myIDFComparator> new_idfpq;

								get_scores(query_bi_word_terms, potential_docs, dictionary, &index, new_idfpq);
								get_scores(query_terms, potential_docs, dictionary, &index, new_idfpq);
								priority_queue< pair<int, double>, vector<pair<int, double> >, myComparator > new_pq = rank_docs(potential_docs);
								cout << endl << endl << "Results after feedback are - " << endl << endl;
								print_results(new_pq, new_idfpq, &index, dictionary);
								potential_docs.clear();


							}
							break;
						}

			case '2' : {
							ifstream doc_id_sorted_index;
							doc_id_sorted_index.open("tf_idf.txt");
							string query;
							cout << "Enter your query with necessary AND, OR and NOT syntax - " << endl;
							getline(cin, query);
							int in_dict = 1;
							query = mystringtolower(query);
							vector<string> query_terms_with_stop_words = split(query, " ");
							string prev_term;
							vector<int> prev_doc_list;
							vector<int> next_doc_list;
							if(dictionary.find(query_terms_with_stop_words[0]) != dictionary.end())
							{
								prev_term = query_terms_with_stop_words[0];
								prev_doc_list = get_docs_from_posting_list(prev_term, &doc_id_sorted_index, dictionary);
							}
							else
							{
								in_dict = 0;
							}
							for(int i = 1; i < query_terms_with_stop_words.size() && in_dict; i += 2)
							{
								string operation = query_terms_with_stop_words[i];
								string next_term = query_terms_with_stop_words[i + 1];
								if(dictionary.find(next_term) != dictionary.end())
								{
									next_doc_list = get_docs_from_posting_list(next_term, &doc_id_sorted_index, dictionary);

									if(operation == "and")
									{
										prev_doc_list = boolean_and(prev_doc_list, next_doc_list);
									}
									else if(operation == "or")
									{
										prev_doc_list = boolean_or(prev_doc_list, next_doc_list);
									}
									else if(operation == "not")
									{
										prev_doc_list = boolean_not(prev_doc_list, next_doc_list);
									}
									else
									{
										in_dict = 0;
									}
								}
								else
								{
									in_dict = 0;
								}

							}
							if(in_dict)
							{
								cout << endl << "The total matching documents are - " << prev_doc_list.size() << endl;
								cout << endl << "The resulting docs are - " << endl << endl;
								for(auto i : prev_doc_list)
								{
									cout << "document" << to_string(i) << ".txt" << endl;
								}
								cout << endl;
							}
							else
							{
								cout << endl << "Invalid boolean query or the terms mentioned are not in the dictionary..." << endl;
							}
							break;
					   }

			case '3' : {
							//to be implemented by Karthik
							string query;
							cout << "Enter your query - " << endl;
							getline(cin, query);
							query = mystringtolower(query);
							set<string> queryOut = queryLocator(query,trieVec[0],trieVec[1]);
							vector<string> query_terms;
							for(auto i : queryOut)
							{
								query_terms.push_back(i);
							}
							priority_queue< pair<string, float>, vector<pair<string, float> >, myIDFComparator> idfpq;
							get_scores(query_terms, potential_docs, dictionary, &index, idfpq);
							priority_queue< pair<int, double>, vector<pair<int, double> >, myComparator > pq = rank_docs(potential_docs);
							cout << endl << endl << "Results are - " << endl << endl;
							print_results(pq, idfpq, &index, dictionary);
							potential_docs.clear();
							break;
					   }

			case '4' : {
							cout << endl << "Thank you for using our program" << endl;
							exit(0);
							break;
					   }

			default  : {
							cout << endl << "Invalid choice..." << endl;
							break;
					   }
		}
	}

	

	return 0;
}