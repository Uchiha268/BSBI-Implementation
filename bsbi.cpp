#include<bits/stdc++.h>
#define DOC_COUNT 30
#define WORDS_PER_BLOCK 100000
#define ITEMS_PER_BLOCK_IN_MEM 10000

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



void fill_queue(queue< pair<string, map<int, int> > > &q, ifstream *fp)
{
	int lc = 0;
	string line;
	while(lc++ < ITEMS_PER_BLOCK_IN_MEM && getline(*fp, line))
	{
		stringstream ss(line);
		string word;
		ss >> word;
		string term = word;
		map<int, int> *temp = new map<int, int>;
		while(ss >> word)
		{
			int doc_id = stoi(word);
			ss >> word;
			int term_freq = stoi(word);
			(*temp)[doc_id] = term_freq;
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
	map<string, map<int, int> > term_doc_pairs;
	int flag = 0, block_count = 0;
	int wc = 0;
	for(int i = 0; i < DOC_COUNT; i++)
	{
		ifstream f;
		string word;
		f.open("document" + to_string(i + 1) + ".txt");
		while(f >> word)
		{
			if(!flag)
			{
				if(word.find(",") != string::npos || word.find(";") != string::npos || word.find(":") != string::npos || word.find(".") != string::npos)
				{
					word = word.substr(0, word.size() - 1);
				}
				if(word != "" && stop_words.find(word) == stop_words.end())
				{
					if(term_doc_pairs.find(word) == term_doc_pairs.end())
					{
						term_doc_pairs[word].insert(make_pair(i + 1, 0));
					}
					else if(term_doc_pairs[word].find(i + 1) == term_doc_pairs[word].end())
					{
						term_doc_pairs[word][i + 1] = 0;
					}
					term_doc_pairs[word][i + 1]++;
					if(++wc >= WORDS_PER_BLOCK)
					{
						flag = 1;
					}
				}
			}
			else
			{
				ofstream f1;
				f1.open("block" + to_string(++block_count) + ".txt");
				for(auto i : term_doc_pairs)
				{
					f1 << i.first;
					for(auto j : term_doc_pairs[i.first])
					{
						f1 << " " << j.first << " " << j.second;
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
		f1.open("block" + to_string(++block_count) + ".txt");
		for(auto i : term_doc_pairs)
		{
			f1 << i.first;
			for(auto j : term_doc_pairs[i.first])
			{
				f1 << " " << j.first << " " << j.second;
			}
			f1 << endl;
		}
		f1.close();
		term_doc_pairs.clear();
	}
	vector < queue< pair<string, map<int, int > > > > block_queues;
	vector < ifstream * > block_pointers;
	priority_queue< pair<string, int> , vector < pair<string, int> >, myComparator> pq;
	for(int i = 0; i < block_count; i++)
	{
		queue< pair<string, map<int, int> > > temp;
		block_queues.push_back(temp) ;
		ifstream *fp = new ifstream;
		(*fp).open("block" + to_string(i + 1) + ".txt");
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
		map< int, int > cur_dict_entry;

		for(int i = 0; i < block_count; i++)
		{
			if(!block_queues[i].empty() && block_queues[i].front().first == cur.first)
			{
				pair< string, map<int, int> > block_entry = block_queues[i].front();
				block_queues[i].pop();
				for(auto j : block_entry.second)
				{
					if(cur_dict_entry.find(j.first) == cur_dict_entry.end())
					{
						cur_dict_entry[j.first] = j.second;
					}
					else
					{
						cur_dict_entry[j.first] += j.second;
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
				fout << " " << i.first << " " << i.second;
			}
			fout << endl;
		}

	}
	fout.close();

	return 0;
}
