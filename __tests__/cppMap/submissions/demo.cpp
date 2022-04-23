#include <iostream>
#include<map>
#include <cstdio>
using namespace std;

int main()
{
    int n;
    scanf("%d", &n);
    map<string, int>mp;
    string s;
    char c;
    getchar();
    getline(cin, s);
    for(int i = 0; i < s.length() - n + 1; i++) mp[s.substr(i, n)]++;
    map<string,int>::iterator pointer;
    string resolution;
    int max = 0;
    for(pointer = mp.begin(); pointer != mp.end(); pointer++)
    {
        if(pointer -> second > max)
        {
            max = pointer -> second;
            resolution = pointer -> first;
        }
    }
    printf("%s %d", resolution.c_str(), max);
    return 0;
}