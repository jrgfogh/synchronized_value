#include "synchronized_value.h"
#include <map>
#include <mutex>

using namespace std;

// first slide
class legacy_class
{
	std::map<int, shared_data> map_;
public:
	auto function_a() {
		// ... 100 lines
	}
	auto function_b() {
		// ... 50 lines
	}
	auto function_c();
	auto function_d();
	auto function_e();
};

// second slide
class legacy_class1
{
	std::mutex mutex_;
	std::map<int, shared_data> map_;
public:
	auto function_a() {
		std::unique_lock lock{mutex_};
		// ... 100 lines
	}
	auto function_b() {
		std::unique_lock lock{mutex_};
		// ... 50 lines
	}
	auto function_c();
	auto function_d();
	auto function_e();
};

// Problem: Too much locking!

template <typename Key, typename Value>
class synchronized_map
{
	std::mutex mutex_;
	std::map<Key, Value> map_;
public:
	auto& operator[](Key k) {
		std::unique_lock lock{mutex_};
		return map_[k];
	}
	// Delegate all methods:
	// at(), begin(), end(), cbegin(), cend(), ...
};

// General solution:
class legacy_class2
{
	synchronized_value<std::map<int, shared_data>> map_;
public:
	auto function_a() {
		// ... 1000 lines
	}
	auto function_b() {
		// ... 1000 lines
	}
	auto function_c();
	auto function_d();
	auto function_e();
};

int main()
{
	cout << "Hello CMake." << endl;
	return 0;
}
