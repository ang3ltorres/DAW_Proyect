#include <audioDecoder/include/decoder.hpp>
#include <projectFile/include/project_file.hpp>
#include <portaudio.h>
#include <iostream>

int main() {

	ProjectFileFormat::loadProject("../project.dawp");
	PaStreamParameters outputParameters;

	Decoder myDecoder = Decoder();

	myDecoder.changeStr("Hola Mundo");

	std::cout << myDecoder.getStr() << &outputParameters;

	return 0;
}