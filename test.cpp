#include<bits/stdc++.h>

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

int main()
{
	string word = "Psycho-analysis";
	cout << mystringtolower(word) << endl;
	return 0;
}