#include <project_file.hpp>
#include <iostream>
#include <fstream>
#include <format>

std::fstream& ProjectFileFormat::operator>>(std::fstream &fs, Date &date) {

	fs.read(reinterpret_cast<char*>(&date.day), 1);
	fs.read(reinterpret_cast<char*>(&date.month), 1);
	fs.read(reinterpret_cast<char*>(&date.year), 2);
	return fs;
}

std::fstream& ProjectFileFormat::operator>>(std::fstream &fs, Audio &audio) {

	// len aux
	uint16_t len;

	// Get ID
	fs.read(reinterpret_cast<char*>(&audio.id), 2);
	std::cout << std::format("ID: {:d}\n", audio.id);

	// Get filename
	fs.read(reinterpret_cast<char*>(&len), 2);
	audio.fileName.resize(len);
	fs.read(audio.fileName.data(), len);
	std::cout << std::format("FileName: {}\n", audio.fileName);

	return fs;
}

ProjectFileFormat::Project ProjectFileFormat::loadProject(const std::string &fileName) {

	// Project
	Project project;

	// len aux
	uint16_t len;

	// Open file
	std::fstream file{fileName, std::ios::binary | std::ios::in};

	// Compare header
	static const std::string header{"OPENDAWPROJECT"};
	std::string headerFile(header.length() , '\0');
	file.read(headerFile.data(), header.length());

	if (header != headerFile)
		std::cout << "Error!!\n";

	// Get version
	file.read(reinterpret_cast<char*>(&project.major), 1);
	file.read(reinterpret_cast<char*>(&project.minor), 1);
	file.read(reinterpret_cast<char*>(&project.patch), 1);
	std::cout << std::format("{:d}.{:d}.{:d}\n", project.major, project.minor, project.patch);

	// Get creation date
	file >> project.creationDate;
	std::cout << std::format("Day: {:d}\nMonth: {:d}\nYear: {:d}\n\n", project.creationDate.day, project.creationDate.month, project.creationDate.year);

	// Get last saved date
	file >> project.lastSavedDate;
	std::cout << std::format("Day: {:d}\nMonth: {:d}\nYear: {:d}\n\n", project.lastSavedDate.day, project.lastSavedDate.month, project.lastSavedDate.year);

	// Get project name
	file.read(reinterpret_cast<char*>(&len), 2);
	project.name.resize(len);
	file.read(project.name.data(), len);
	std::cout << project.name << "\n";

	// Get total audios
	file.read(reinterpret_cast<char*>(&len), 2);
	std::cout << std::format("Total audios: {:d}\n", len);

	for (uint16_t i = 0; i < len; i++) {

		Audio audio;
		file >> audio;
		project.audios.push_back(audio);
	}

	// TODO


	file.close();
	return project;
}