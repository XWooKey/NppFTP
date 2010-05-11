/*
    NppFTP: FTP/SFTP functionality for Notepad++
    Copyright (C) 2010  Harry (harrybharry@users.sourceforge.net)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "StdInc.h"
#include "FTPProfile.h"

const char * FTPProfile::ProfilesElement = "Profiles";

FTPProfile::FTPProfile() :
	m_name(NULL),
	m_cache(NULL),
	m_hostname(NULL),
	m_port(0),
	m_username(NULL),
	m_password(NULL),
	m_askPassword(false),
	m_timeout(30),
	m_keepAlive(30),
	m_keepAliveTransfer(90),
	m_securityMode(Mode_FTP),
	m_transferMode(Mode_Binary),
	m_connectionMode(Mode_Passive),
	m_initialDir(NULL)
{
}

FTPProfile::FTPProfile(const TCHAR * name) :
	m_port(21),
	m_askPassword(false),
	m_timeout(30),
	m_keepAlive(30),
	m_keepAliveTransfer(90),
	m_securityMode(Mode_FTP),
	m_transferMode(Mode_Binary),
	m_connectionMode(Mode_Passive)
{
	m_cache = new FTPCache();

	m_name = SU::DupString(name);
	m_hostname = SU::strdup("");
	m_username = SU::strdup("");
	m_password = SU::strdup("");
	m_initialDir = SU::strdup("");

	m_cache->SetEnvironment(m_hostname, m_username);
}

FTPProfile::~FTPProfile() {
	if (m_name) {
		SU::FreeTChar(m_name);
		m_name = NULL;
	}
	if (m_hostname) {
		free(m_hostname);
		m_hostname = NULL;
	}
	if (m_username) {
		free(m_username);
		m_username = NULL;
	}
	if (m_password) {
		free(m_password);
		m_password = NULL;
	}

	if (m_cache) {
		delete m_cache;
		m_cache = NULL;
	}

	if (m_initialDir) {
		free(m_initialDir);
		m_initialDir = NULL;
	}

	for(size_t i = 0; i < m_asciiTypes.size(); i++) {
		free(m_asciiTypes[i]);
	}
	m_asciiTypes.clear();

	for(size_t i = 0; i < m_binTypes.size(); i++) {
		free(m_binTypes[i]);
	}
	m_binTypes.clear();
}

FTPClientWrapper* FTPProfile::CreateWrapper() {
	FTPClientWrapper * wrapper = NULL;
	switch(m_securityMode) {
		case Mode_SFTP:
			wrapper = new FTPClientWrapperSSH(m_hostname, m_port, m_username, m_password);
			break;
		case Mode_FTP:
			wrapper = new FTPClientWrapperSSL(m_hostname, m_port, m_username, m_password);
			((FTPClientWrapperSSL*)wrapper)->SetMode(CUT_FTPClient::FTP);
			((FTPClientWrapperSSL*)wrapper)->SetConnectionMode(m_connectionMode);
			break;
		case Mode_FTPS:
			wrapper = new FTPClientWrapperSSL(m_hostname, m_port, m_username, m_password);
			((FTPClientWrapperSSL*)wrapper)->SetMode(CUT_FTPClient::FTPS);
			((FTPClientWrapperSSL*)wrapper)->SetConnectionMode(m_connectionMode);
			break;
		case Mode_FTPES:
			wrapper = new FTPClientWrapperSSL(m_hostname, m_port, m_username, m_password);
			((FTPClientWrapperSSL*)wrapper)->SetMode(CUT_FTPClient::FTPES);
			((FTPClientWrapperSSL*)wrapper)->SetConnectionMode(m_connectionMode);
			break;
		case Mode_SecurityMax:
		default:
			break;

	}

	wrapper->SetTimeout(m_timeout);

	return wrapper;
}

const TCHAR* FTPProfile::GetName() const {
	return m_name;
}

int FTPProfile::SetName(const TCHAR * name) {
	SU::FreeTChar(m_name);
	m_name = SU::DupString(name);
	return 0;
}


const char* FTPProfile::GetHostname() const {
	return m_hostname;
}

int FTPProfile::SetHostname(const char * hostname) {
	free(m_hostname);
	m_hostname = SU::strdup(hostname);
	m_cache->SetEnvironment(m_hostname, m_username);
	return 0;
}

int FTPProfile::GetPort() const {
	return m_port;
}

int FTPProfile::SetPort(int port) {
	if (port <= 0 || port >= 65536)
		return -1;
	m_port = port;
	return 0;
}

const char* FTPProfile::GetUsername() const {
	return m_username;
}

int FTPProfile::SetUsername(const char * username) {
	free(m_username);
	m_username = SU::strdup(username);
	m_cache->SetEnvironment(m_hostname, m_username);
	return 0;
}

const char* FTPProfile::GetPassword() const {
	return m_password;
}

int FTPProfile::SetPassword(const char * password) {
	free(m_password);
	m_password = SU::strdup(password);
	return 0;
}

bool FTPProfile::GetAskPassword() const {
	return m_askPassword;
}

int FTPProfile::SetAskPassword(bool ask) {
	if (ask)		//asking for password atm not supported
		return -1;

	m_askPassword = ask;
	return 0;
}


int FTPProfile::GetTimeout() const {
	return m_timeout;
}

int FTPProfile::SetTimeout(int timeout) {
	if (timeout < 0)
		return -1;

	m_timeout = timeout;
	return 0;
}

int FTPProfile::GetKeepAlive() const {
	return m_keepAlive;
}

int FTPProfile::SetKeepAlive(int keepalive) {
	if (keepalive < 0)
		return -1;

	m_keepAlive = keepalive;
	return 0;
}

int FTPProfile::GetKeepAliveTransfer() const {
	return m_keepAliveTransfer;
}

int FTPProfile::SetKeepAliveTransfer(int keepalivetransfer) {
	if (keepalivetransfer < 0)
		return -1;

	m_keepAliveTransfer = keepalivetransfer;
	return 0;
}

Security_Mode FTPProfile::GetSecurityMode() const {
	return m_securityMode;
}

int FTPProfile::SetSecurityMode(Security_Mode mode) {
	if (mode < 0 || mode > Mode_SecurityMax)
		return -1;

	m_securityMode = mode;
	return 0;
}

Transfer_Mode FTPProfile::GetTransferMode() const {
	return m_transferMode;
}

int FTPProfile::SetTransferMode(Transfer_Mode mode) {
	if (mode < 0 || mode > Mode_TransferMax)
		return -1;

	m_transferMode = mode;
	return 0;
}

Connection_Mode FTPProfile::GetConnectionMode() const {
	return m_connectionMode;
}

int FTPProfile::SetConnectionMode(Connection_Mode mode) {
	if (mode < 0 || mode > Mode_ConnectionMax)
		return -1;

	m_connectionMode = mode;
	return 0;
}

const char* FTPProfile::GetInitialDir() const {
	return m_initialDir;
}

int FTPProfile::SetInitialDir(const char * dir) {
	free(m_initialDir);
	m_initialDir = SU::strdup(dir);
	return 0;
}

int FTPProfile::SetCacheParent(FTPCache * parentCache) {
	if (!m_cache)
		return -1;

	m_cache->SetCacheParent(parentCache);
	return 0;
}


//other functions
int FTPProfile::AddAsciiType(const TCHAR * type) {
	if (!ValidType(type))
		return -1;

	for(size_t i = 0; i < m_asciiTypes.size(); i++) {
		if (!lstrcmp(type, m_asciiTypes[i])) {
			return 0;
		}
	}

	m_asciiTypes.push_back(SU::DupString(type));
	return 0;
}

int FTPProfile::AddBinaryType(const TCHAR * type) {
	if (!ValidType(type))
		return -1;

	for(size_t i = 0; i < m_binTypes.size(); i++) {
		if (!lstrcmp(type, m_binTypes[i])) {
			return 0;
		}
	}

	m_binTypes.push_back(SU::DupString(type));
	return 0;
}

int FTPProfile::RemoveAsciiType(const TCHAR * type) {
	for(size_t i = 0; i < m_asciiTypes.size(); i++) {
		if (!lstrcmp(type, m_asciiTypes[i])) {
			SU::FreeTChar(m_asciiTypes[i]);
			m_asciiTypes.erase(m_asciiTypes.begin()+i);
			return 0;
		}
	}
	return -1;
}

int FTPProfile::RemoveBinaryType(const TCHAR * type) {
	for(size_t i = 0; i < m_binTypes.size(); i++) {
		if (!lstrcmp(type, m_binTypes[i])) {
			SU::FreeTChar(m_binTypes[i]);
			m_binTypes.erase(m_binTypes.begin()+i);
			return 0;
		}
	}
	return -1;
}

int FTPProfile::GetAsciiCount() {
	return (int)m_asciiTypes.size();
}

int FTPProfile::GetBinaryCount() {
	return (int)m_binTypes.size();
}

const TCHAR* FTPProfile::GetAsciiType(int i) {
	return m_asciiTypes.at(i);
}

const TCHAR* FTPProfile::GetBinaryType(int i) {
	return m_binTypes.at(i);
}

Transfer_Mode FTPProfile::GetFileTransferMode(const TCHAR* file) const {
	LPCTSTR suffix = NULL;
	suffix = PathFindSuffixArray(file, (const TCHAR**)(&m_asciiTypes[0]), m_asciiTypes.size());
	if (suffix) {
		return Mode_ASCII;
	}

	suffix = PathFindSuffixArray(file, (const TCHAR**)(&m_binTypes[0]), m_binTypes.size());
	if (suffix) {
		return Mode_Binary;
	}

	return m_transferMode;
}

int FTPProfile::GetCacheExternal(const TCHAR* localfile, char* extbuffer, int extbuffersize) const {
	if (!extbuffer || extbuffersize == 0)
		return -1;

	return m_cache->GetExternalPathFromLocal(localfile, extbuffer, extbuffersize);
}

int FTPProfile::GetCacheLocal(const char * externalfile, TCHAR* localbuffer, int localbuffersize) const {
	if (!localbuffer || localbuffersize == 0)
		return -1;

	return m_cache->GetLocalPathFromExternal(externalfile, localbuffer, localbuffersize);
}

FTPCache* FTPProfile::GetCache() const {
	return m_cache;
}

vProfile FTPProfile::LoadProfiles(const TiXmlElement * profilesElem) {
	vProfile profiles;

	if (!profilesElem)
		return profiles;

	if ( strcmp(FTPProfile::ProfilesElement, profilesElem->Value()) )
		return profiles;

	const TiXmlElement* child = profilesElem->FirstChildElement("Profile");

	for( ; child; child = child->NextSiblingElement("Profile") )
	{
		FTPProfile * profile = FTPProfile::LoadProfile(child);
		if (profile) {
			profiles.push_back(profile);
		}
	}

	return profiles;
}

TiXmlElement* FTPProfile::SaveProfiles(const vProfile profiles) {
	TiXmlElement * profilesElem = new TiXmlElement(FTPProfile::ProfilesElement);
	size_t count = profiles.size();
	for(size_t i = 0; i < count; i++) {
		TiXmlElement* profile = profiles[i]->SaveProfile();
		if (profile) {
			profilesElem->LinkEndChild(profile);
		}
	}

	return profilesElem;
}

FTPProfile* FTPProfile::LoadProfile(const TiXmlElement * profileElem) {
	if (!profileElem)
		return NULL;

	FTPProfile * profile = new FTPProfile();
	bool success = false;
	const char * attrstr = NULL;

	do {
		attrstr = profileElem->Attribute("name");
		if (!attrstr)
			break;
		profile->m_name = SU::Utf8ToTChar(attrstr);

		attrstr = profileElem->Attribute("hostname");
		if (!attrstr)
			break;
		profile->m_hostname = SU::strdup(attrstr);

		profileElem->Attribute("port", &profile->m_port);

		attrstr = profileElem->Attribute("username");
		if (!attrstr)
			break;
		profile->m_username = SU::strdup(attrstr);

		attrstr = profileElem->Attribute("password");	//password may be empty, but not missing
		if (!attrstr)
			break;
		profile->m_password = SU::strdup(attrstr);

		profileElem->Attribute("askPassword", (int*)(&profile->m_askPassword));
		profileElem->Attribute("timeout", &profile->m_timeout);
		profileElem->Attribute("keepAlive", &profile->m_keepAlive);
		profileElem->Attribute("keepAliveTransfer", &profile->m_keepAliveTransfer);

	//TODO: this is rather risky casting, check if the compiler accepts it
		profileElem->Attribute("securityMode", (int*)(&profile->m_securityMode));
		profileElem->Attribute("transferMode", (int*)(&profile->m_transferMode));
		profileElem->Attribute("connectionMode", (int*)(&profile->m_connectionMode));

		const TiXmlElement * cacheElem = profileElem->FirstChildElement(FTPCache::CacheElem);
		if (!cacheElem)
			break;

		profile->m_cache = FTPCache::LoadCache(cacheElem);
		if (!profile->m_cache)
			break;

		//TODO: enforce root

		attrstr = profileElem->Attribute("initialDir");
		if (!attrstr)
			break;
		profile->m_initialDir = SU::strdup(attrstr);

		const TiXmlElement * typesElem = profileElem->FirstChildElement("FileTypes");
		if (!typesElem)
			break;

		const char * asciistr = typesElem->Attribute("asciiTypes");
		const char * binstr = typesElem->Attribute("binaryTypes");
		if (!asciistr || !binstr)
			break;

		TCHAR * asciistrW = SU::Utf8ToTChar(asciistr);
		TCHAR * binstrW = SU::Utf8ToTChar(binstr);

		profile->ExpandTypeVector(asciistrW, true);
		profile->ExpandTypeVector(binstrW, false);

		SU::FreeTChar(asciistrW);
		SU::FreeTChar(binstrW);

		profile->Sanitize();

		success = true;
	} while(false);

	if (!success) {
		delete profile;
		return NULL;
	}

	return profile;
}

TiXmlElement* FTPProfile::SaveProfile() const {
	TiXmlElement * profileElem = new TiXmlElement("Profile");

	char * utf8name = SU::TCharToUtf8(m_name);
	profileElem->SetAttribute("name", utf8name);
	SU::FreeUtf8(utf8name);
	profileElem->SetAttribute("hostname", m_hostname);
	profileElem->SetAttribute("port", m_port);
	profileElem->SetAttribute("username", m_username);
	profileElem->SetAttribute("password", m_password);
	profileElem->SetAttribute("askPassword", m_askPassword?1:0);
	profileElem->SetAttribute("timeout", m_timeout);
	profileElem->SetAttribute("keepAlive", m_keepAlive);
	profileElem->SetAttribute("keepAliveTransfer", m_keepAliveTransfer);

	profileElem->SetAttribute("securityMode", m_securityMode);
	profileElem->SetAttribute("transferMode", m_transferMode);
	profileElem->SetAttribute("connectionMode", m_connectionMode);

	profileElem->SetAttribute("initialDir", m_initialDir);

	TiXmlElement * cacheElem = FTPCache::SaveCache(m_cache);
	if (!cacheElem) {
		delete profileElem;
		return NULL;
	}
	profileElem->LinkEndChild(cacheElem);

	tstring asciiString = CompactTypeVector(m_asciiTypes);
	tstring binaryString = CompactTypeVector(m_binTypes);

	char * asciiUtf8 = SU::TCharToUtf8(asciiString.c_str());
	char * binUtf8 = SU::TCharToUtf8(binaryString.c_str());

	TiXmlElement * typesElem = new TiXmlElement("FileTypes");
	typesElem->SetAttribute("asciiTypes", asciiUtf8);
	typesElem->SetAttribute("binaryTypes", binUtf8);
	profileElem->LinkEndChild(typesElem);

	SU::FreeUtf8(asciiUtf8);
	SU::FreeUtf8(binUtf8);

	return profileElem;
}

int FTPProfile::Sanitize() {
	if (m_port <= 0 || m_port >= 65536)
		m_port = 21;

	if (m_timeout < 0)
		m_timeout = 0;

	if (m_keepAlive < 0)
		m_keepAlive = 0;

	if (m_keepAliveTransfer < 0)
		m_keepAliveTransfer = 0;

	m_askPassword = (m_askPassword == true);

	if (m_securityMode < 0 || m_securityMode >= Mode_SecurityMax)
		m_securityMode = Mode_FTP;

	if (m_transferMode < 0 || m_transferMode >= Mode_TransferMax)
		m_transferMode = Mode_Binary;

	if (m_connectionMode < 0 || m_connectionMode >= Mode_ConnectionMax)
		m_connectionMode = Mode_Passive;

	m_askPassword = false;

	return 0;
}

bool FTPProfile::ValidType(const TCHAR * type) const {
	if (!type)
		return false;

	if (lstrlen(type) < 2)
		return false;

	if (type[0] != TEXT('.'))
		return false;

	return true;
}

int FTPProfile::ExpandTypeVector(tstring types, bool isAscii) {
	const tstring delimiter = TEXT("|");
    tstring::size_type lastPos = types.find_first_not_of(delimiter, 0);
    tstring::size_type pos     = types.find_first_of(delimiter, lastPos);

    while (tstring::npos != pos || tstring::npos != lastPos)
    {
        tstring type = types.substr(lastPos, pos - lastPos);
        if (isAscii)
			AddAsciiType(type.c_str());
		else
			AddBinaryType(type.c_str());

        lastPos = types.find_first_not_of(delimiter, pos);
        pos = types.find_first_of(delimiter, lastPos);
    }

	return 0;
}

tstring FTPProfile::CompactTypeVector(vString vect) const {
	tstring typeString;

	if (vect.size() > 0) {
		typeString = vect[0];
		for(size_t i = 1; i < vect.size(); i++) {
			typeString += TEXT("|");
			typeString += vect[i];
		}
	}

	return typeString;
}
