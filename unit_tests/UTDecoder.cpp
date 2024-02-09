#include <portaudio.h>
#include <iostream>
#include <type_traits>

#include <format.hpp>
#include <audioDecoder/include/wav_decoder.hpp>

class DecoderUnitTest
{
	public:
		void run();
	private:
		wavDecoder testDecoder;
		template <class t>
		bool assertion(const t &result, const t &expected_result);
		void printResult(bool, const std::string &message);
};

template <class t>
bool DecoderUnitTest::assertion(const t &result, const t &expected_result)
{
	return result == expected_result;
}

void DecoderUnitTest::printResult (bool result, const std::string &message)
{
	std::string result_str = result ? color::output("OK!", color::RED):  color::output("FAILED", color::RED);
	std::cout << "=> Test [" << color::BOLD << message << color::ENDC << "] " << result_str << " \n";
}

void DecoderUnitTest::run()
{
	// testDecoder.loadFile ("./test_files/sample.wav");
}

int main()
{
	DecoderUnitTest myTester;
	myTester.run();
	return 0;
}