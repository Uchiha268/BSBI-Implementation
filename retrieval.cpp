#include<bits/stdc++.h>
#define A 10 //No. of representatives chosen from each posting list
#define K 10 //Top K elements of potential docs
#define PRESENT_IN_LIMIT 3
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
		long int prev_loc = -1;
		for(auto j : top_idf)
		{
			if(present_limit)
			{
				if(first_locs[j.first].find(i.first) != first_locs[j.first].end() && first_locs[j.first][i.first] != prev_loc)
				{
					ifstream fin;
					fin.open("./DOCS/document" + to_string(i.first) + ".txt");
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
					prev_loc = first_locs[j.first][i.first];
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

	string choice;
	cout << "What kind of query do you want to make?" << endl << "1)Phrase" << endl << "2)Boolean" << endl << "3)WildCard" << endl << endl << "Enter your choice - ";
	getline(cin, choice);
	// string query;
	// 				cout << "Enter your query - " << endl;
	// 				getline(cin, query);
	// 				query = mystringtolower(query);
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
						break;
					}

		case '2' : {
						//to be implemented by Shreyash
						break;
				   }

		case '3' : {
						//to be implemented by Karthik
						break;
				   }

		default  : {
						cout << endl << "Invalid choice..." << endl;
						break;
				   }
	}

	

	return 0;
}