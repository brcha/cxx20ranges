#include <unordered_map>
#include <algorithm>

int main() {
	std::unordered_map<int, float> m = {
		{1, 2.3f}, {2, 3.4f}, {3, 4.5f}
	};

	std::ranges::sort(m);
}
