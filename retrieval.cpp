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

int main()
{
	map< string, pair< int, long int> > dictionary = load_dictionary("dictionary.txt");
	return 0;
}