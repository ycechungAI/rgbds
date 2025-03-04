// SPDX-License-Identifier: MIT

#ifndef RGBDS_FILE_HPP
#define RGBDS_FILE_HPP

#include <fcntl.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <streambuf>
#include <string.h>
#include <string>

#include "either.hpp"
#include "helpers.hpp" // assume
#include "platform.hpp"

#include "gfx/main.hpp"

class File {
	Either<std::streambuf *, std::filebuf> _file;

public:
	File() : _file(nullptr) {}

	// This should only be called once, and before doing any `->` operations.
	// Returns `nullptr` on error, and a non-null pointer otherwise.
	File *open(std::string const &path, std::ios_base::openmode mode) {
		if (path != "-") {
			_file.emplace<std::filebuf>();
			return _file.get<std::filebuf>().open(path, mode) ? this : nullptr;
		} else if (mode & std::ios_base::in) {
			assume(!(mode & std::ios_base::out));
			_file.emplace<std::streambuf *>(std::cin.rdbuf());
			if (setmode(STDIN_FILENO, (mode & std::ios_base::binary) ? O_BINARY : O_TEXT) == -1) {
				fatal(
				    "Failed to set stdin to %s mode: %s",
				    mode & std::ios_base::binary ? "binary" : "text",
				    strerror(errno)
				);
			}
		} else {
			assume(mode & std::ios_base::out);
			_file.emplace<std::streambuf *>(std::cout.rdbuf());
		}
		return this;
	}
	std::streambuf &operator*() {
		return _file.holds<std::filebuf>() ? _file.get<std::filebuf>()
		                                   : *_file.get<std::streambuf *>();
	}
	std::streambuf const &operator*() const {
		// The non-`const` version does not perform any modifications, so it's okay.
		return **const_cast<File *>(this);
	}
	std::streambuf *operator->() { return &**this; }
	std::streambuf const *operator->() const {
		// See the `operator*` equivalent.
		return const_cast<File *>(this)->operator->();
	}

	char const *c_str(std::string const &path) const {
		return _file.holds<std::filebuf>()                         ? path.c_str()
		       : _file.get<std::streambuf *>() == std::cin.rdbuf() ? "<stdin>"
		                                                           : "<stdout>";
	}
};

#endif // RGBDS_FILE_HPP
