#include <iostream>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <portaudio.h>
#include <pa_linux_alsa.h>
#include <UT_audio.hpp>
#include <UT_format.hpp>
#include <audioDecoder/include/wav_decoder.hpp>




std::vector<int> errors =
{
	(int)wavDecoder::Error::NOT_WAV_FILE,
	(int)wavDecoder::Error::FAIL_OPEN,
	(int)wavDecoder::Error::NOT_RIFF,
	(int)wavDecoder::Error::NOT_WAVE,
	(int)wavDecoder::Error::NOT_FMT,
	(int)wavDecoder::Error::NOT_DATA,
	(int)wavDecoder::Error::NO_ERROR,
};

std::vector files {
	"foo.av",
	"foo.wav",
	"./unit_tests/test_files/audioFile/bad_ex_sample.wav",
	"./unit_tests/test_files/audioFile/bad_ex_sample1.wav",
	"./unit_tests/test_files/audioFile/bad_ex_sample2.wav",
	"./unit_tests/test_files/audioFile/bad_ex_sample3.wav",
	"./unit_tests/test_files/audioFile/ex_sample.wav"
};

std::vector<std::string> messages = {
	"Wrong suffix",
	"Non existent file",
	"No RIFF ID",
	"NO WAVE ID",
	"NO fmt  ID",
	"NO DATA ID",
	"NO ERRROR"
};

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

	std::cout << "---------\n" << color::output("Testing loadFile error codes", color::BOLD) << "\n---------\n";

	// std::vector<std::vector<std::string>::iterator> it = {errors.begin(), files.begin(), messages.begin()};

	int vector_size = files.size();

	for (size_t i = 0; i < vector_size; i ++) {
		printResult(assertion(testDecoder.loadFile(files[i]), errors[i]),
					messages[i]);
	}

	std::cout << "\n---------\n" << color::output("Testing file header info", color::BOLD) << "\n---------\n";

	printResult(assertion(testDecoder.PCM_value(), (decltype(testDecoder.PCM_value()))1),
				"Getting PCM value");

	printResult(assertion(testDecoder.getChannelCount(), (decltype(testDecoder.getChannelCount()))2),
				"Getting channel count");

	printResult(assertion(testDecoder.getSampleRate(), (decltype(testDecoder.getSampleRate()))44100),
		        "Getting sample rate");

	printResult(assertion(testDecoder.getByteRate(), (decltype(testDecoder.getByteRate()))176400),
		        "Getting byte rate");

	printResult(assertion(testDecoder.getBlockAlign(), (decltype(testDecoder.getBlockAlign()))4),
		        "Getting block align");

	printResult(assertion(testDecoder.getBitsPerSample(), (decltype(testDecoder.getBitsPerSample()))16),
			    "Getting get bytes per sample");

	printResult(assertion(testDecoder.getRawDataSize(), (decltype(testDecoder.getRawDataSize()))13641600),
			    "Getting get raw data byte size");
	PaError err;
	err = Pa_Initialize();
	if (err != paNoError) {
		printResult(false, "portaudio initialization");
		std::cout << color::output("Error: ", color::BOLD) << Pa_GetErrorText(err) << "\n";
		exit (err);
	}
	printResult(true, "portaudio initialization");

	PaStream *test_stream;
	paTestData test_data;

	test_data.pcmData = testDecoder.getRawData();
	test_data.rawDataSize = testDecoder.getRawDataSize() / testDecoder.getBlockAlign();


	double frameRate = static_cast<double>(testDecoder.getByteRate()) /
                       (testDecoder.getChannelCount() * testDecoder.getBitsPerSample());

	err = Pa_OpenDefaultStream(&test_stream,
						 0,
						 testDecoder.getChannelCount(),
						 paInt16,
						 testDecoder.getSampleRate(),
						 frameRate,
						 wav_play_callback,
						 &test_data);

	if (err != paNoError) {
		printResult(false, "portaudio open stream");
		std::cout << color::output("Error: ", color::BOLD) << Pa_GetErrorText(err) << "\n";
		exit (err);
	}
	printResult(true, "portaudio open stream");

	err = Pa_SetStreamFinishedCallback(test_stream, streamFinishedCallback);
	if (err != paNoError) {
		printResult(false, "portaudio set finish callback");
		std::cout << color::output("Error: ", color::BOLD) << Pa_GetErrorText(err) << "\n";
		exit (err);
	}
	printResult(true, "portaudio set finish callback");

	err = Pa_StartStream(test_stream);
	if (err != paNoError) {
		printResult(false, "portaudio start stream");
		std::cout << color::output("Error: ", color::BOLD) << Pa_GetErrorText(err) << "\n";
		exit (err);
	}
	printResult(true, "portaudio set start stream");

	while (!playbackFinished)
	{
	    Pa_Sleep(100);
	}


	err = Pa_StopStream(test_stream);
	if (err != paNoError) {
		printResult(false, "portaudio stop stream");
		std::cout << color::output("Error: ", color::BOLD) << Pa_GetErrorText(err) << "\n";
		exit (err);
	}
	printResult(true, "portaudio stop stream");

	err = Pa_CloseStream(test_stream);
	if (err != paNoError) {
		printResult(false, "portaudio close stream");
		std::cout << color::output("Error: ", color::BOLD) << Pa_GetErrorText(err) << "\n";
		exit (err);
	}
	printResult(true, "portaudio close stream");

	Pa_Terminate();
}

int main()
{
	DecoderUnitTest myTester;
	myTester.run();
	return 0;
}