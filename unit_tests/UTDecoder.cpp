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
		template <typename t>
		int playAudio (const wavDecoder &decodedData, int err);
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

template <typename t>
int DecoderUnitTest::playAudio (const wavDecoder &decodedData, int err)
{
	PaStream *stream;
	PaData<t> test_data;
	test_data.pcmData = (t *)decodedData.getRawData();
	test_data.dataSize = decodedData.getRawDataSize() / decodedData.getBlockAlign();

	double frames_per_buffer = decodedData.getByteRate() / (decodedData.getChannelCount() * decodedData.getBitsPerSample());

	PaSampleFormat format;

	switch (decodedData.getBitsPerSample()) {
		case 16:
			format = paInt16;
		break;
		case 24:
			format = paInt32;
		break;
	}

	err = Pa_OpenDefaultStream (&stream,
								0,
								decodedData.getChannelCount(),
								format,
								decodedData.getSampleRate(),
								frames_per_buffer,
								wPlayCallback<t>,
								&test_data);

	if (paNoError == err) {
		err = Pa_SetStreamFinishedCallback(stream, streamFinishedCallback);
	}

	if (paNoError == err) {
		err = Pa_StartStream(stream);
	}

	while (!playbackFinished) {
		Pa_Sleep(100);
	}

	if (paNoError == err) {
		err = Pa_StopStream(stream);
	}

	if (paNoError == err) {
		err = Pa_CloseStream(stream);
	}

	playbackFinished = 0;

	return err;
}

void DecoderUnitTest::run()
{
	bool result = false;

	PaError err;
	err = Pa_Initialize();

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

	if (paNoError == err) {
		err = playAudio<unsigned short>(testDecoder, err);
	}

	std::cout << "\n---------\n" << color::output("Testing 24 bit depth file", color::BOLD) << "\n---------\n";

	testDecoder.loadFile(files[files.size() - 1]);

	assertInfo(info_list[1]);

	std::cout << "\n---------\n" << color::output("Testing file audio playback for 24bit", color::BOLD) << "\n---------\n";

	if (paNoError == err) {
		err = playAudio<unsigned int>(testDecoder, err);
	}

	Pa_Terminate();
}

int main()
{
	DecoderUnitTest myTester;
	myTester.run();
	return 0;
}