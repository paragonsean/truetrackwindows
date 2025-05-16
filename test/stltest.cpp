#pragma warning(disable:4786)
#include <string>
#include <iostream>
#include <vector>
#include <iterator>

using namespace std ;

void main()
{
vector<string> v;
int    i;
string s1;

v.push_back("Hi you guys");
v.push_back("WHat's up" );

for ( i=0; i<v.size(); i++ )
    {
    cout << v[i];
    }

cout << "Enter a sentence:";
getline( cin, s1 );
cout << "You entered: " << s1;
}

