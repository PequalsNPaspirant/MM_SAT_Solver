#include <iostream>
//using namespace std;

namespace mm {
	void sudoku_test();
	void google_or_tools_examples_main(int argc, char** argv);
}

int main(int argc, char** argv)
{
	//mm::sudoku_test();
	mm::google_or_tools_examples_main(argc, argv);

	std::cout << "\n\nReached end of program successfully! Press any key to exit...";
	std::cin.get();
    return 0;
}

