#include <portaudio.h>
#include <iostream>

#include <UT_format.hpp>
#include <audioDecoder/include/wav_decoder.hpp>

#define TEST_FILEPATH "./test_files/sample2.wav"

class DecoderUnitTest
{
	public:
		void run();
	private:
		wavDecoder testDecoder;
		template <typename t>
		bool assertion(const t &result, const t &expected_result);
		void printResult(bool, const std::string &message);
};

template <typename t>
bool DecoderUnitTest::assertion(const t &result, const t &expected_result)
{
	return result == expected_result;
}

void DecoderUnitTest::printResult (bool result, const std::string &message)
{
	std::string result_str = result ? color::output("OK!", color::RED):  color::output("FAILED!", color::RED);
	std::cout << "=> Test [" << color::BOLD << message << color::ENDC << "] " << result_str << " \n";
}

void DecoderUnitTest::run()
{
	bool result = false;
	std::cout << color::output("Testing file header info", color::BOLD) << "---------\n";
	testDecoder.loadFile (TEST_FILEPATH);

	printResult(assertion(testDecoder.PCM_value(), (unsigned int)1),
				"Getting PCM value");

	printResult(assertion(testDecoder.getSampleRate(), (size_t) 44100),
		        "Getting sample rate");

	printResult(assertion(testDecoder.getChannelCount(), (unsigned short)2),
				"Getting channel count");

	std::cout << color::output("Testing file raw data", color::BOLD) << "---------\n";
	//TODO: create bin file with test raw data from ex_sample file
}

int main()
{
	DecoderUnitTest myTester;
	myTester.run();
	return 0;
}