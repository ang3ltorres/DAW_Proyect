#include <iostream>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <portaudio.h>
#include <pa_linux_alsa.h>
#include <UT_audio.hpp>
#include <UT_format.hpp>
#include <audioDecoder/include/wav_decoder.hpp>

struct sampleInfo
{
	unsigned short pcm_value;
	unsigned short channel_count;
	unsigned short bits_per_sample;
	unsigned short block_align;
	unsigned int sample_rate;
	unsigned int byte_rate;
	unsigned int raw_data_size;

	public:
	sampleInfo (const unsigned short &pcm_value,
				const unsigned short &channel_count,
				const unsigned short &bits_per_sample,
				const unsigned short &block_align,
				const unsigned int &sample_rate,
				const unsigned int &byte_rate,
				const unsigned int &raw_data_size) :
				pcm_value(pcm_value),
				channel_count(channel_count),
				bits_per_sample(bits_per_sample),
				block_align(block_align),
				sample_rate(sample_rate),
				byte_rate(byte_rate),
				raw_data_size(raw_data_size) {}
};

std::vector<sampleInfo> info_list = {
	sampleInfo(1, 2, 16, 4, 44100, 176400, 13641600),
	sampleInfo(1, 2, 24, 6, 44100, 264600, 8599500)
};

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
	"./unit_tests/test_files/audioFile/ex_sample.wav",
	"./unit_tests/test_files/audioFile/ex_sample2.wav"
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
		void assertInfo (const sampleInfo &sample_info);
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

void DecoderUnitTest::assertInfo (const sampleInfo &sample_info)
{
	printResult(assertion(testDecoder.PCM_value(), sample_info.pcm_value),
				"Getting PCM value");

	printResult(assertion(testDecoder.getChannelCount(), sample_info.channel_count),
				"Getting channel count");

	printResult(assertion(testDecoder.getSampleRate(), sample_info.sample_rate),
		        "Getting sample rate");

	printResult(assertion(testDecoder.getByteRate(), sample_info.byte_rate),
		        "Getting byte rate");

	printResult(assertion(testDecoder.getBlockAlign(), sample_info.block_align),
		        "Getting block align");

	printResult(assertion(testDecoder.getBitsPerSample(), sample_info.bits_per_sample),
			    "Getting get bytes per sample");

	printResult(assertion(testDecoder.getRawDataSize(), sample_info.raw_data_size),
			    "Getting get raw data byte size");
}

void DecoderUnitTest::run()
{
	bool result = false;

	std::cout << "---------\n" << color::output("Testing loadFile error codes", color::BOLD) << "\n---------\n";

	// std::vector<std::vector<std::string>::iterator> it = {errors.begin(), files.begin(), messages.begin()};

	int vector_size = files.size();

	for (size_t i = 0; i < vector_size - 1; i ++) {
		printResult(assertion(testDecoder.loadFile(files[i]), errors[i]),
					messages[i]);
	}

	std::cout << "\n---------\n" << color::output("Testing file header info", color::BOLD) << "\n---------\n";

	assertInfo(info_list[0]);

	std::cout << "\n---------\n" << color::output("Testing file audio playback", color::BOLD) << "\n---------\n";

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

	test_data.pcmData = (unsigned short *)testDecoder.getRawData();
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
	printResult(true, "portaudio start stream");

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

	playbackFinished = 0;

	testDecoder.loadFile(files[files.size() - 1]);

	std::cout << "\n---------\n" << color::output("Testing 24 bit file info", color::BOLD) << "\n---------\n";

	assertInfo(info_list[1]);

	paTestData24 data_24bit;
	data_24bit.rawDataSize = testDecoder.getRawDataSize() / testDecoder.getBlockAlign();
	data_24bit.pcmData = (unsigned int *)testDecoder.getRawData();

	std::cout << "\n---------\n" << color::output("Testing 24 bit audio playback", color::BOLD) << "\n---------\n";


	if (err != paNoError) {
		printResult(false, "portaudio initialization");
		std::cout << color::output("Error: ", color::BOLD) << Pa_GetErrorText(err) << "\n";
		exit (err);
	}
	printResult(true, "portaudio initialization");

	PaStream *test_stream_24bit;

	double frameRate24bit = static_cast<double>(testDecoder.getByteRate()) /
                       (testDecoder.getChannelCount() * testDecoder.getBitsPerSample());

	err = Pa_OpenDefaultStream(&test_stream_24bit,
						 0,
						 testDecoder.getChannelCount(),
						 paInt32,
						 testDecoder.getSampleRate(),
						 frameRate24bit,
						 wav_play_callback_24,
						 &data_24bit);
	if (err != paNoError) {
		printResult(false, "portaudio open stream");
		std::cout << color::output("Error: ", color::BOLD) << Pa_GetErrorText(err) << "\n";
		exit (err);
	}
	printResult(true, "portaudio open stream");

	err = Pa_SetStreamFinishedCallback(test_stream_24bit, streamFinishedCallback);
	if (err != paNoError) {
		printResult(false, "portaudio set finish callback");
		std::cout << color::output("Error: ", color::BOLD) << Pa_GetErrorText(err) << "\n";
		exit (err);
	}

	err = Pa_StartStream(test_stream);
	if (err != paNoError) {
		printResult(false, "portaudio start stream");
		std::cout << color::output("Error: ", color::BOLD) << Pa_GetErrorText(err) << "\n";
		exit (err);
	}
	printResult(true, "portaudio start stream");

	while (!playbackFinished)
	{
	    Pa_Sleep(100);
	}


	err = Pa_StopStream(test_stream_24bit);
	if (err != paNoError) {
		printResult(false, "portaudio stop stream");
		std::cout << color::output("Error: ", color::BOLD) << Pa_GetErrorText(err) << "\n";
		exit (err);
	}
	printResult(true, "portaudio stop stream");

	err = Pa_CloseStream(test_stream_24bit);
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