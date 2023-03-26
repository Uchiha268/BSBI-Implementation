#include<bits/stdc++.h>
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

class myComparator
{
	public:
		int operator() (const pair<float, int> &p1, const pair<float, int> &p2)
		{
			return p1.first < p2.first;
		}
};

int main()
{
	ifstream fin;
	fin.open("tf_idf.txt");
	ofstream fout;
	fout.open("sorted_tf_idf.txt");
	string line, term;
	float idf;
	map<int, pair<float, pair<int, vector<long int> > > > doc_info;
	priority_queue< pair<float, int>, vector< pair<float, int> >, myComparator > pq;

	while(getline(fin, line))
	{
		vector<string> lineSplit = split(line, " ");
		term = lineSplit[0];
		idf = stof(lineSplit[1]);
		int i = 2;
		while(i < lineSplit.size())
		{
			vector<long int> *locs = new vector<long int>;
			int doc_id = stoi(lineSplit[i]);
			i++;
			float tf = stof(lineSplit[i]);
			i++;
			int term_freq = stol(lineSplit[i]);
			i++;
			for(int j = 0; j < term_freq; j++, i++)
			{
				(*locs).push_back(stoi(lineSplit[i]));
			}
			doc_info[doc_id] = make_pair(tf, make_pair(term_freq, (*locs)));
			pq.push(make_pair(tf, doc_id));
		}
		fout << term << " " << idf;
		while(!pq.empty())
		{
			pair<float, int> pq_entry = pq.top();
			pq.pop();
			float tf = pq_entry.first;
			int doc_id = pq_entry.second;
			pair<float, pair<int, vector<long int> > > doc_entry = doc_info[doc_id];
			fout << " " << doc_id;
			fout << " " << tf;
			fout << " " << doc_entry.second.first;
			for(auto j : doc_entry.second.second)
			{
				fout << " " << j;
			}
		}	
		fout << endl;
	}
	return 0;
}