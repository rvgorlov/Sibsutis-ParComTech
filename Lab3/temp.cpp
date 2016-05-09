#include <vector>
#include <iostream>
#include <iomanip>
#include <future>

using namespace std;

int func() {

	return 10; 
}

int main(int argc, char const *argv[])
{	
	int sz = 11;
	std::vector<future<int>> v;
	for (int i = 0; i < 10; ++i)
	{	
		v.emplace_back( async([&sz](int size) { return 10; }) ); 
	}

	for (int i = 0; i < 10; ++i)
	{
		cout << v[i].get() << endl;  
	}

	return 0;
}