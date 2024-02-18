#pragma once

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>

namespace ProjectFileFormat {

	struct Date {

		uint8_t day;
		uint8_t month;
		uint16_t year;
	};
	std::basic_istream<char>& operator>>(std::basic_istream<char> &fs, Date &date);

	struct Audio {

		uint16_t id;
		std::string fileName;
	};
	std::basic_istream<char>& operator>>(std::basic_istream<char> &fs, Audio &audio);

	// Sample
	struct Sample {

		Audio *fileId; // Reference to the "AudioReference"
		double startPosition;
		double endPosition;
	};
	std::basic_istream<char>& operator>>(std::basic_istream<char> &fs, Sample &sample);

	// Sample-Track Type
	struct Track {

		uint16_t id;
		std::string name;
		int8_t panning;
		double volume;

		std::vector<Sample> samples;
	};
	std::basic_istream<char>& operator>>(std::basic_istream<char> &fs, Track &track);

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

		// Tracks vector
		std::vector<Track> tracks;

		// If a track is isolated != nullptr
		Track *soloTrack; // Track Reference
	};

	Project loadProject(const std::string &fileName);
}
