#include <project_file.hpp>
#include <sstream>
#include <fstream>
#include <format>

static ProjectFile::Project project;

std::basic_istream<char>& ProjectFile::operator>>(std::basic_istream<char> &s, Date &date) {

	s.read(reinterpret_cast<char*>(&date.day), sizeof(uint8_t));
	s.read(reinterpret_cast<char*>(&date.month), sizeof(uint8_t));
	s.read(reinterpret_cast<char*>(&date.year), sizeof(uint16_t));
	return s;
}

std::basic_istream<char>& ProjectFile::operator>>(std::basic_istream<char> &s, Audio &audio) {

	// len aux
	uint16_t len;

	// Get ID
	s.read(reinterpret_cast<char*>(&audio.id), sizeof(uint16_t));
	std::cout << std::format("ID: {:d}\n", audio.id);

	// Get filename
	s.read(reinterpret_cast<char*>(&len), sizeof(uint16_t));
	audio.fileName.resize(len);
	s.read(audio.fileName.data(), len);
	std::cout << std::format("FileName: {}\n", audio.fileName);

	return s;
}

std::basic_istream<char>& ProjectFile::operator>>(std::basic_istream<char> &s, Sample &sample) {

	// len aux
	uint16_t len;

	// Get Audio ID
	s.read(reinterpret_cast<char*>(&len), sizeof(uint16_t));
	std::cout << std::format("ID: {:d}\n", len);
	sample.fileId = nullptr;

	for (uint16_t i = 0; i < len; i++) {

		if (project.audios[i].id == len) {

			sample.fileId = project.audios.data() + i;
			std::cout << std::format("Audio name: {:s}\n", sample.fileId->fileName);
		}
	}

	// Get Start Position
	s.read(reinterpret_cast<char*>(&sample.startPosition), sizeof(double));
	std::cout << std::format("SP: {:f}\n", sample.startPosition);

	// Get End Position
	s.read(reinterpret_cast<char*>(&sample.endPosition), sizeof(double));
	std::cout << std::format("SP: {:f}\n\n\n", sample.endPosition);

	return s;
}

std::basic_istream<char>& ProjectFile::operator>>(std::basic_istream<char> &s, Track &track) {

	// len aux
	uint16_t len;

	// Get ID
	s.read(reinterpret_cast<char*>(&track.id), sizeof(uint16_t));
	std::cout << std::format("ID: {:d}\n", track.id);

	// Get name
	s.read(reinterpret_cast<char*>(&len), sizeof(uint16_t));
	track.name.resize(len);
	s.read(track.name.data(), len);
	std::cout << std::format("TrackName: {}\n", track.name);

	// Get panning
	s.read(reinterpret_cast<char*>(&track.panning), sizeof(int8_t));
	std::cout << std::format("Panning: {:d}\n", track.panning);

	// Get volume
	s.read(reinterpret_cast<char*>(&track.volume), sizeof(double));
	std::cout << std::format("Volume: {:f}\n\n", track.volume);

	// Get total samples
	s.read(reinterpret_cast<char*>(&len), sizeof(uint16_t));
	std::cout << std::format("Total samples: {:d}\n\n", len);

	for (uint16_t i = 0; i < len; i++) {

		Sample sample;
		s >> sample;
		track.samples.push_back(sample);
	}

	return s;
}

ProjectFile::Project ProjectFile::loadProject(const std::string &fileName) {

	// len aux
	uint16_t len;

	// Open file
	std::fstream file{fileName, std::ios::binary | std::ios::in};
	std::stringstream fileBuffer;
	fileBuffer << file.rdbuf();
	file.close();

	// Compare header
	static const std::string header{"OPENDAWPROJECT"};
	std::string headerFile(header.length(), '\0');
	fileBuffer.read(headerFile.data(), header.length());

	if (header != headerFile)
		std::cout << "Error!!\n";

	// Get version
	fileBuffer.read(reinterpret_cast<char*>(&project.major), sizeof(uint8_t));
	fileBuffer.read(reinterpret_cast<char*>(&project.minor), sizeof(uint8_t));
	fileBuffer.read(reinterpret_cast<char*>(&project.patch), sizeof(uint8_t));
	std::cout << std::format("{:d}.{:d}.{:d}\n", project.major, project.minor, project.patch);

	// Get creation date
	fileBuffer >> project.creationDate;
	std::cout << std::format("Day: {:d}\nMonth: {:d}\nYear: {:d}\n\n", project.creationDate.day, project.creationDate.month, project.creationDate.year);

	// Get last saved date
	fileBuffer >> project.lastSavedDate;
	std::cout << std::format("Day: {:d}\nMonth: {:d}\nYear: {:d}\n\n", project.lastSavedDate.day, project.lastSavedDate.month, project.lastSavedDate.year);

	// Get project name
	fileBuffer.read(reinterpret_cast<char*>(&len), sizeof(uint16_t));
	project.name.resize(len);
	fileBuffer.read(project.name.data(), len);
	std::cout << project.name << "\n";

	// Get isolated Track (uint16_t)
	project.soloTrack = nullptr;

	uint16_t isolatedTrackID;
	fileBuffer.read(reinterpret_cast<char*>(&isolatedTrackID), sizeof(uint16_t));
	std::cout << std::format("Isolated Track ID: {:d}\n", isolatedTrackID);

	// Get total audios
	fileBuffer.read(reinterpret_cast<char*>(&len), sizeof(uint16_t));
	std::cout << std::format("Total audios: {:d}\n", len);

	for (uint16_t i = 0; i < len; i++) {

		Audio audio;
		fileBuffer >> audio;
		project.audios.push_back(audio);
	}

	// Get total tracks
	fileBuffer.read(reinterpret_cast<char*>(&len), sizeof(uint16_t));
	std::cout << std::format("Total tracks: {:d}\n", len);

	// Get each track
	for (uint16_t i = 0; i < len; i++) {

		Track track;
		fileBuffer >> track;
		project.tracks.push_back(track);

		// Set project isolated track ptr
		if (isolatedTrackID != 0 && track.id == isolatedTrackID) {

			project.soloTrack = project.tracks.data() + i;
			std::cout << std::format("Isolated Track name: {:s}\n\n", project.soloTrack->name);
		}
	}

	return project;
}