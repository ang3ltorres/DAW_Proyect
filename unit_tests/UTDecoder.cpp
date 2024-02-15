#include <iostream>
#include <typeinfo>

#include <portaudio.h>
#include <UT_format.hpp>
#include <audioDecoder/include/wav_decoder.hpp>

#define TEST_FILEPATH "./unit_tests/test_files/audioFile/ex_sample.wav"

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
	std::string result_str = result ? color::output("OK!", color::GREEN):  color::output("FAILED!", color::RED);
	std::cout << "=> Test [" << color::BOLD << message << color::ENDC << "] " << result_str << " \n";
}

void DecoderUnitTest::run()
{
	bool result = false;
	testDecoder.loadFile (TEST_FILEPATH);
	std::cout << color::output("Testing file header info", color::BOLD) << "---------\n";

	printResult(assertion(testDecoder.PCM_value(), (decltype(testDecoder.PCM_value()))1),
				"Getting PCM value");

	printResult(assertion(testDecoder.getChannelCount(), (decltype(testDecoder.getChannelCount()))2),
				"Getting channel count");

	printResult(assertion(testDecoder.getSampleRate(), (decltype(testDecoder.getSampleRate()))44100),
		        "Getting sample rate");


	std::cout << color::output("Testing file raw data", color::BOLD) << "---------\n";
}

int main()
{
	DecoderUnitTest myTester;
	myTester.run();
	return 0;
}