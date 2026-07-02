#pragma once
#include <iostream>
#include "User.h"
#include "MusicLibrary.h"
#include "FileManager.h"

class EchoService {
private:
	MusicLibrary musicLibrary;
	User currentUser;
public:
	EchoService() : currentUser("null", "null", false) {
		// Initialize music library and user
		musicLibrary = MusicLibrary();
	}

};