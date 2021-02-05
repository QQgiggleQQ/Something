#include "NTFS.h"

void getVolumeHandle(BYTE id, HANDLE& handle)
{
	WCHAR formatName[8] = { L'\\', L'\\', L'.', L'\\', id, L':' };
	getVolumeHandle(formatName, handle);
}

void getVolumeHandle(PWCHAR formatName, HANDLE& handle)
{
	handle = CreateFileW(formatName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		exit(GetLastError());
	}
}

void getUsnJournalInformation(HANDLE handle, USN& firstUsn, USN& nextUsn, DWORDLONG& usnJournalId)
{
	DWORD retBytes = 0;
	USN_JOURNAL_DATA ujd = { 0 };
	if (!DeviceIoControl(handle, FSCTL_QUERY_USN_JOURNAL, NULL, 0, &ujd, sizeof(ujd), &retBytes, NULL))
	{
		createUsnJournal(handle);
		if (!DeviceIoControl(handle, FSCTL_QUERY_USN_JOURNAL, NULL, 0, &ujd, sizeof(ujd), &retBytes, NULL))
		{
			exit(GetLastError());
		}
	}
	firstUsn = ujd.FirstUsn;
	nextUsn = ujd.NextUsn;
	usnJournalId = ujd.UsnJournalID;
}

void createUsnJournal(HANDLE handle)
{
	DWORD retBytes = 0;
	CREATE_USN_JOURNAL_DATA cujd = { 0 };
	if (!DeviceIoControl(handle, FSCTL_CREATE_USN_JOURNAL, &cujd, sizeof(cujd), NULL, 0, &retBytes, NULL))
	{
		exit(GetLastError());
	}
}

void deleteUsnJournal(HANDLE handle)
{
	DWORD retBytes = 0;
	DELETE_USN_JOURNAL_DATA dujd = { 0 };
	if (!DeviceIoControl(handle, FSCTL_DELETE_USN_JOURNAL, &dujd, sizeof(dujd), NULL, 0, &retBytes, NULL))
	{
		exit(GetLastError());
	}
}

UINT getFileReferenceNumber(PCHAR fullPath)
{
	HANDLE handle = CreateFileA(fullPath, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	BY_HANDLE_FILE_INFORMATION bhfi;
	memset(&bhfi, 0, sizeof(BY_HANDLE_FILE_INFORMATION));
	if (!GetFileInformationByHandle(handle, &bhfi))
	{
		return 0;
	}
	CloseHandle(handle);
	return (UINT)bhfi.nFileIndexLow & 0x07FFFFFF;
}

UINT getFileReferenceNumber(PWCHAR fullPath)
{
	HANDLE handle = CreateFileW(fullPath, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	BY_HANDLE_FILE_INFORMATION bhfi;
	memset(&bhfi, 0, sizeof(BY_HANDLE_FILE_INFORMATION));
	if (!GetFileInformationByHandle(handle, &bhfi))
	{
		return 0;
	}
	CloseHandle(handle);
	return (UINT)bhfi.nFileIndexLow & 0x07FFFFFF;
}

std::vector<BYTE> getVolumeId()
{
	std::vector<BYTE> ret;
	WCHAR nameBuf[MAX_PATH] = { 0 };
	WCHAR typeBuf[MAX_PATH] = { 0 };
	PWCHAR wname = nameBuf;
	GetLogicalDriveStringsW(MAX_PATH, nameBuf);
	while (*wname != 0)
	{
		GetVolumeInformationW(wname, NULL, 0, NULL, NULL, NULL, typeBuf, MAX_PATH);
		if (typeBuf[0] == L'N' && typeBuf[1] == L'T' && typeBuf[2] == L'F' && typeBuf[3] == L'S')
		{
			ret.push_back((BYTE)*wname);
		}
		wname += 4;
	}
	return ret;
}

HICON getFileIconBySuffix(char* suff)
{
	SHFILEINFOA info = { 0 };
	if (SHGetFileInfoA(suff, FILE_ATTRIBUTE_NORMAL, &info, sizeof(info),
		SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES))
	{
		return info.hIcon;
	}
	return NULL;
}

HICON getDirectoryIcon()
{
	SHFILEINFOA info = { 0 };
	if (SHGetFileInfoA("folder", FILE_ATTRIBUTE_DIRECTORY, &info, sizeof(info),
		SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES))
	{
		return info.hIcon;
	}
	return NULL;
}
