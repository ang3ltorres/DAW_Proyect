#include <audioDecoder/include/decoder.hpp>
#include <portaudio.h>
#include <iostream>

#define ANSI_RED    "\x1b[31m"
#define ANSI_GREEN  "\x1b[32m"
#define ENDC		"\033[0m"
#define BOLD		"\033[1m"

class DecoderUnitTest {
	public:
		void run ();
	private:
		Decoder testDecoder;
		template <class t>
		bool assertion (const t &result, const t &expected_result);
		void printResult (bool, const std::string &message);
};

template <class t>
bool DecoderUnitTest::assertion (const t &result, const t &expected_result) {
	return result == expected_result;
}

void DecoderUnitTest::printResult (bool result, const std::string &message) {
	std::cout << "=> Test ";
}

void DecoderUnitTest::run () {

}

int main() {
	DecoderUnitTest myTester;
	myTester.run();
	return 0;
}