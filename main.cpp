#include <audioDecoder/include/decoder.hpp>
#include <projectFile/include/project_file.hpp>
#include <iostream>

int main() {

	ProjectFileFormat::loadProject("../project.dawp");

	Decoder myDecoder = Decoder();

	myDecoder.changeStr("Hola Mundo");

	std::cout << myDecoder.getStr();

	return 0;
}