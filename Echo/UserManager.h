#pragma once
#include "User.h"
#include "MusicLibrary.h"
#include "FileManager.h"

class UserManager {
public:
	static bool userExists(const string&username) {
		return FileManager::findAccount(username);
	}

	// solo crea el registro, la instancia se crea desde AppController
	static bool registerUser(const string& username, const string& hashPassword, bool isPremium) {
		if (userExists(username)) return false;
		
		FileManager::addAccount(username, hashPassword, isPremium);
		return true;
	}

	static User* loginFakeAccount(const string& username, const string& hashPassword, vector<User>& fakeAccounts) {
		for (User& user : fakeAccounts) {
			if (user.getUsername() == username && user.getHashPassword() == hashPassword)
				return &user;
		}
		return nullptr;
	}
	static bool loginUser(const string& username, const string& hashPassword, User& outUser, MusicLibrary& musicLib) {
		ifstream file(appFiles::USERS_LIST_FILE);
		if (!file.is_open()) return false;

		string line;
		while (getline(file, line)) {
			if (!line.empty() && line.back() == '\r') line.pop_back();

			stringstream ss(line);
			string storedUser, storedPass, premiumStr;
			getline(ss, storedUser, '|');
			getline(ss, storedPass, '|');
			getline(ss, premiumStr, '|');

			if (storedUser == username && storedPass == hashPassword) {
				outUser = User(username, hashPassword, premiumStr == "1");
				FileManager::loadAccountConfig(username, outUser, musicLib);
				file.close();
				return true;
			}
		}
		file.close();
		return false;
	}

};