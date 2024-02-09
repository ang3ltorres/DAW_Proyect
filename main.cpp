#include <projectFile/include/project_file.hpp>
#include <portaudio.h>
#include <iostream>

int main() {

	ProjectFileFormat::loadProject("../project.dawp");
	return 0;
}