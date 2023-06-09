#include<bits/stdc++.h>
#define DOC_COUNT 8823
#define WORDS_PER_BLOCK 100000
#define ITEMS_PER_BLOCK_IN_MEM 10000
using namespace std;

static const string DOC_PREFIX = "./DOCS/";
static const string BLOCK_PREFIX = "./BLOCKS/";

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

string word_pre_processor(string word)
{
	string new_word = "";
	int i = 0;
	while(i < word.size() && ((word[i] < 'A' || word[i] > 'Z') && (word[i] < 'a' || word[i] > 'z')))
	{
		i++;
	}
	while(i < word.size() && ((word[i] >= 'A' && word[i] <= 'Z') || (word[i] >= 'a' && word[i] <= 'z') || word[i] == '-'))
	{
		new_word += word[i];
		i++;
	}
	return new_word;
}


void fill_queue(queue< pair<string, map<int, pair<int, vector<long int> > > > > &q, ifstream *fp)
{
	int lc = 0;
	string line;
	while(lc++ < ITEMS_PER_BLOCK_IN_MEM && getline(*fp, line))
	{
		stringstream ss(line);
		string word;
		ss >> word;
		string term = word;
		map<int, pair<int, vector<long int> > > *temp = new map<int, pair<int, vector<long int> > >;
		while(ss >> word)
		{
			int doc_id = stoi(word);
			ss >> word;
			int term_freq = stoi(word);
			(*temp)[doc_id].first = term_freq;
			for(int i = 0; i < term_freq; i++)
			{
				ss >> word;
				(*temp)[doc_id].second.push_back(stoi(word));
			}
		}
		q.push(make_pair(term, *temp));
	}

}

class myComparator
{
	public:
		int operator() (const pair<string, int> &p1, const pair<string, int> &p2)
		{
			return p1.first > p2.first;
		}
};

int main()
{
	string doc = "document";
	unordered_set<string> stop_words = get_stop_words("stop_words.txt");
	map<string, map<int, pair<int, vector<long int> > > > term_doc_pairs;
	int flag = 0, block_count = 0;
	int wc = 0;
	for(int i = 0; i < DOC_COUNT; i++)
	{
		ifstream f;
		string word, prev_word = "";
		long int prev_word_loc = 0;
		f.open(DOC_PREFIX + "document" + to_string(i + 1) + ".txt");
		while(f >> word)
		{
			if(!flag)
			{
				// if(word.find(",") != string::npos || word.find(";") != string::npos || word.find(":") != string::npos || word.find(".") != string::npos)
				// {
				// 	word = word.substr(0, word.size() - 1);
				// }
				long int loc = (long int)f.tellg() - word.size();
				string old_word = word; //we maintain this so we know the size of original string which is needed for prev_word_size to accruately calculate loc for biwords
				word = word_pre_processor(word);
				word = mystringtolower(word);
				if(word != "" && stop_words.find(word) == stop_words.end())
				{
					string bi_word = prev_word + "." + word;
					vector<long int> *temp_single = new vector<long int>;
					vector<long int> *temp_bi_word = new vector<long int>;
					if(term_doc_pairs.find(word) == term_doc_pairs.end())
					{
						term_doc_pairs[word].insert(make_pair(i + 1, make_pair(0, (*temp_single))));
					}
					else if(term_doc_pairs[word].find(i + 1) == term_doc_pairs[word].end())
					{
						term_doc_pairs[word][i + 1].first = 0;
					}
					term_doc_pairs[word][i + 1].first++;
					term_doc_pairs[word][i + 1].second.push_back(loc);

					if(term_doc_pairs.find(bi_word) == term_doc_pairs.end())
					{
						term_doc_pairs[bi_word].insert(make_pair(i + 1, make_pair(0, (*temp_bi_word))));
					}
					else if(term_doc_pairs[bi_word].find(i + 1) == term_doc_pairs[bi_word].end())
					{
						term_doc_pairs[bi_word][i + 1].first = 0;
					}
					term_doc_pairs[bi_word][i + 1].first++;
					term_doc_pairs[bi_word][i + 1].second.push_back(prev_word_loc); //problem is that the words skipped in middle are not counted here

					wc += 2;
					if(wc >= WORDS_PER_BLOCK)
					{
						flag = 1;
					}
					prev_word = word;
					prev_word_loc = loc; //+1 to accomodate the space in between the two words of biword
				}
			}
			else
			{
				ofstream f1;
				f1.open(BLOCK_PREFIX + "block" + to_string(++block_count) + ".txt");
				for(auto i : term_doc_pairs)
				{
					f1 << i.first;
					for(auto j : term_doc_pairs[i.first])	
					{
						f1 << " " << j.first;
						f1 << " " << j.second.first;
						for(auto k : j.second.second)
						{
							f1 << " " << k;
						}
					}
					f1 << endl;
				}

				f1.close();
				term_doc_pairs.clear();
				wc = 0;
				flag = 0;
			}
			
		}

	}
	if(!term_doc_pairs.empty())
	{
		ofstream f1;
		f1.open(BLOCK_PREFIX + "block" + to_string(++block_count) + ".txt");
		for(auto i : term_doc_pairs)
		{
			f1 << i.first;
			for(auto j : term_doc_pairs[i.first])
			{
				f1 << " " << j.first;
				f1 << " " << j.second.first;
				for(auto k : j.second.second)
				{
					f1 << " " << k;
				}
			}
			f1 << endl;
		}

		f1.close();
		term_doc_pairs.clear();
	}
	vector < queue< pair<string, map<int, pair<int, vector<long int> > > > > > block_queues;
	vector < ifstream * > block_pointers;
	priority_queue< pair<string, int> , vector < pair<string, int> >, myComparator> pq;
	for(int i = 0; i < block_count; i++)
	{
		queue< pair<string, map<int, pair<int, vector<long int> > > > > temp;
		block_queues.push_back(temp) ;
		ifstream *fp = new ifstream;
		(*fp).open(BLOCK_PREFIX + "block" + to_string(i + 1) + ".txt");
		block_pointers.push_back(fp);
		fill_queue(block_queues[i], fp);
		pq.push(make_pair(block_queues[i].front().first, i));
	}
	ofstream fout;
	fout.open("inverted_index.txt");
	while(!pq.empty())
	{
		pair<string, int> cur = pq.top();
		pq.pop();
		map< int, pair<int, vector<long int> > > cur_dict_entry;

		for(int i = 0; i < block_count; i++)
		{
			if(!block_queues[i].empty() && block_queues[i].front().first == cur.first)
			{
				pair< string, map<int, pair<int, vector<long int> > > > block_entry = block_queues[i].front();
				block_queues[i].pop();

				for(auto j : block_entry.second)
				{
					if(cur_dict_entry.find(j.first) == cur_dict_entry.end())
					{
						cur_dict_entry[j.first].first = j.second.first; //edited till here
						for(auto k : j.second.second)
						{
							cur_dict_entry[j.first].second.push_back(k);
						}
					}
					else
					{
						cur_dict_entry[j.first].first += j.second.first;
						for(auto k : j.second.second)
						{
							cur_dict_entry[j.first].second.push_back(k);
						}
					}
				}
				if(block_queues[i].empty())
				{
					fill_queue(block_queues[i], block_pointers[i]);
				}
				if(!block_queues[i].empty())
				{
					pq.push(make_pair(block_queues[i].front().first, i));
				}
			}
		}

		if(!cur_dict_entry.empty())
		{
			fout << cur.first;
			for(auto i : cur_dict_entry)
			{
				fout << " " << i.first;
				fout << " " << i.second.first;
				for(auto j : i.second.second)
				{
					fout << " " << j;
				}
			}
			fout << endl;
		}

	}
	fout.close();

	return 0;
}
