#include <project_file.hpp>
#include <iostream>
#include <fstream>
#include <format>

static ProjectFileFormat::Project project;

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

std::fstream& ProjectFileFormat::operator>>(std::fstream &fs, Sample &sample) {

	// len aux
	uint16_t len;

	// Get Audio ID
	fs.read(reinterpret_cast<char*>(&len), 2);
	std::cout << std::format("ID: {:d}\n", len);
	sample.fileId = nullptr;

	for (uint16_t i = 0; i < len; i++) {

		if (project.audios[i].id == len) {

			sample.fileId = project.audios.data() + i;
			std::cout << std::format("Audio name: {:s}\n", sample.fileId->fileName);
		}
	}

	// Get Start Position
	fs.read(reinterpret_cast<char*>(&sample.startPosition), 8);
	std::cout << std::format("SP: {:f}\n", sample.startPosition);

	// Get End Position
	fs.read(reinterpret_cast<char*>(&sample.endPosition), 8);
	std::cout << std::format("SP: {:f}\n\n\n", sample.endPosition);

	return fs;
}

std::fstream& ProjectFileFormat::operator>>(std::fstream &fs, Track &track) {

	// len aux
	uint16_t len;

	// Get ID
	fs.read(reinterpret_cast<char*>(&track.id), 2);
	std::cout << std::format("ID: {:d}\n", track.id);

	// Get name
	fs.read(reinterpret_cast<char*>(&len), 2);
	track.name.resize(len);
	fs.read(track.name.data(), len);
	std::cout << std::format("TrackName: {}\n", track.name);

	// Get panning
	fs.read(reinterpret_cast<char*>(&track.panning), 1);
	std::cout << std::format("Panning: {:d}\n", track.panning);

	// Get volume
	fs.read(reinterpret_cast<char*>(&track.volume), 8);
	std::cout << std::format("Volume: {:f}\n\n", track.volume);

	// Get total samples
	fs.read(reinterpret_cast<char*>(&len), 2);
	std::cout << std::format("Total samples: {:d}\n\n", len);

	for (uint16_t i = 0; i < len; i++) {

		Sample sample;
		fs >> sample;
		track.samples.push_back(sample);
	}

	return fs;
}

ProjectFileFormat::Project ProjectFileFormat::loadProject(const std::string &fileName) {

	// len aux
	uint16_t len;

	// Open file
	std::fstream file{fileName, std::ios::binary | std::ios::in};

	// Compare header
	static const std::string header{"OPENDAWPROJECT"};
	std::string headerFile(header.length(), '\0');
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

	// Get isolated Track (uint16_t)
	project.soloTrack = nullptr;

	uint16_t isolatedTrackID;
	file.read(reinterpret_cast<char*>(&isolatedTrackID), 2);
	std::cout << std::format("Isolated Track ID: {:d}\n", isolatedTrackID);

	// Get total audios
	file.read(reinterpret_cast<char*>(&len), 2);
	std::cout << std::format("Total audios: {:d}\n", len);

	for (uint16_t i = 0; i < len; i++) {

		Audio audio;
		file >> audio;
		project.audios.push_back(audio);
	}

	// Get total tracks
	file.read(reinterpret_cast<char*>(&len), 2);
	std::cout << std::format("Total tracks: {:d}\n", len);

	// Get each track
	for (uint16_t i = 0; i < len; i++) {

		Track track;
		file >> track;
		project.tracks.push_back(track);

		// Set project isolated track ptr
		if (isolatedTrackID != 0 && track.id == isolatedTrackID) {

			project.soloTrack = project.tracks.data() + i;
			std::cout << std::format("Isolated Track name: {:s}\n\n", project.soloTrack->name);
		}
	}

	file.close();
	return project;
}