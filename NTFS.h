#pragma once

#include <windows.h>
#include <vector>

void getVolumeHandle(BYTE id, HANDLE& handle);
void getVolumeHandle(PWCHAR formatName, HANDLE& handle);
void getUsnJournalInformation(HANDLE handle, USN& firstUsn, USN& nextUsn, DWORDLONG& usnJournalId);
void createUsnJournal(HANDLE handle);
void deleteUsnJournal(HANDLE handle);
UINT getFileReferenceNumber(PCHAR fullPath);
UINT getFileReferenceNumber(PWCHAR fullPath);
std::vector<BYTE> getVolumeId();
HICON getFileIconBySuffix(char* suff);
HICON getDirectoryIcon();
