#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace ProjectFileFormat {

	struct Date {

		uint8_t day;
		uint8_t month;
		uint16_t year;
	};
	std::fstream& operator>>(std::fstream &fs, Date &date);

	struct Audio {

		uint16_t id;
		std::string fileName;
	};
	std::fstream& operator>>(std::fstream &fs, Audio &audio);

	// Sample
	struct Sample {

		Audio &fileId; // Reference to the "AudioReference"
		double startPosition;
		double endPosition;
	};

	// Sample-Track Type
	struct Track {

		uint16_t id;
		std::string name;
		uint8_t panning;
		float volume;

		std::vector<Audio> audios;
	};

	struct Project {

		uint8_t major;
		uint8_t minor;
		uint8_t patch;

		std::string name;
		std::string author;
		Date creationDate;
		Date lastSavedDate;

		// Table with all the audio references
		std::vector<Audio> audios;

		// If a track is isolated >= 0
		Track *soloTrack; // Track Reference
	};

	Project loadProject(const std::string &fileName);
}
