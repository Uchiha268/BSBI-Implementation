#include<bits/stdc++.h>

int main()
{
	ifstream f;
	f.open("document1.txt");
	string word;
	string line = "";
	f.seekg(92354);
	f >> word;
	cout << "The word is - " << word;
	return 0;
}