#include<bits/stdc++.h>

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

int main()
{
	string word = "psycho-analysis";
	cout << word_pre_processor(word) << endl;
	return 0;
}