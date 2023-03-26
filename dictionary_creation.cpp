//Creates the dictionary for sorted_tf_idf.txt
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

int main()
{
	ifstream fin;
	fin.open("sorted_tf_idf.txt");
	ofstream fout;
	fout.open("dictionary.txt");
	long int cur_pos = fin.tellg();
	string line;
	while(getline(fin, line))
	{
		vector<string> lineSplit = split(line, " ");
		string term = lineSplit[0];
		int doc_freq = 0;
		int i = 2;
		while(i < lineSplit.size())
		{
			doc_freq++;
			i += 2;
			long int term_freq = stol(lineSplit[i]);
			i++;
			for(int j = 0; j < term_freq; j++, i++){}
		}
		fout << term << " " << doc_freq << " " << cur_pos << endl;
		cur_pos = fin.tellg();
	}
	return 0;
}