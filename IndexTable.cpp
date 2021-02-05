#include "IndexTable.h"

CIndexTable::CIndexTable() : CBaseIndexTable()
{
	m_id = 0;
	m_nextUsn = 0;
	m_firstUsn = 0;
	m_fileCount = 0;
	m_usnJournalId = 0;
	m_handle = INVALID_HANDLE_VALUE;
}

CIndexTable::~CIndexTable()
{
	if (m_handle && m_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_handle);
	}
}

void CIndexTable::initialize(BYTE id)
{
	m_id = id;
	m_fileCount = 1;
	getVolumeHandle(id, m_handle);
	getUsnJournalInformation(m_handle, m_firstUsn, m_nextUsn, m_usnJournalId);
}

void CIndexTable::create()
{
	DWORD retBytes = 0;
	MFT_ENUM_DATA_V0 med;
	med.LowUsn = 0;
	med.HighUsn = m_nextUsn;
	med.StartFileReferenceNumber = 0;

	static char s_buffer[0X20000] = { 0 };

	while (DeviceIoControl(m_handle, FSCTL_ENUM_USN_DATA, &med, sizeof(med), s_buffer, sizeof(s_buffer), &retBytes, NULL))
	{
		retBytes -= sizeof(DWORDLONG);
		if (retBytes == 0)
		{
			break;
		}
		PUSN_RECORD pUsnRecord = (PUSN_RECORD)(s_buffer + sizeof(DWORDLONG));
		while (retBytes > 0)
		{
			m_fileCount++;
			insert(&buildIndexNode(pUsnRecord), (UINT)(pUsnRecord->FileReferenceNumber));
			retBytes -= pUsnRecord->RecordLength;
			pUsnRecord = (PUSN_RECORD)((PBYTE)pUsnRecord + pUsnRecord->RecordLength);
		}
		med.StartFileReferenceNumber = *(DWORDLONG*)s_buffer;
	}
	clean();
}

void CIndexTable::getName(UINT frn, char* name)
{
	if (frn < m_tableLength && m_table[frn].info)
	{
		PNODE pNameNode = GlobalDataV0::g_nodePool.translate(m_table[frn].name);
		memcpy(name, pNameNode->str, pNameNode->length);
		name += pNameNode->length;
		if (m_table[frn].suff)
		{
			*name++ = '.';
			PNODE pSuffNode = GlobalDataV0::g_nodePool.translate(m_table[frn].suff);
			memcpy(name, pSuffNode->str, pSuffNode->length);
			name += pSuffNode->length;
		}
	}
	*name = 0;
}

void CIndexTable::getPath(UINT frn, char* path)
{
	UINT pfrn = m_table[frn].info & 0X07FFFFFF;
	appendPath(pfrn, path);
	*path = 0;
}

void CIndexTable::getSuff(UINT frn, char* suff)
{
	if (frn < m_tableLength && m_table[frn].info)
	{
		if (m_table[frn].suff)
		{
			PNODE pSuffNode = GlobalDataV0::g_nodePool.translate(m_table[frn].suff);
			memcpy(suff, pSuffNode->str, pSuffNode->length);
			suff += pSuffNode->length;
		}
	}
	*suff = 0;
}

void CIndexTable::getFullPath(UINT frn, char* path)
{
	appendPath(frn, path);
	--path;
	if (frn < m_tableLength && m_table[frn].suff)
	{
		*path++ = '.';
		PNODE pSuffNode = GlobalDataV0::g_nodePool.translate(m_table[frn].suff);
		memcpy(path, pSuffNode->str, pSuffNode->length);
		path += pSuffNode->length;
	}
	*path = 0;
}

BYTE CIndexTable::id()
{
	return m_id;
}

bool CIndexTable::hasParent(UINT frn, UINT pfrn)
{
	while (m_table[frn].info)
	{
		UINT _pfrn = m_table[frn].info & 0X07FFFFFF;
		if (_pfrn == pfrn)
		{
			return true;
		}
		frn = _pfrn;
	}
	return false;
}

void CIndexTable::updata()
{
	DWORD retBytes = 0;
	READ_USN_JOURNAL_DATA_V0 rujd;
	memset(&rujd, 0, sizeof(READ_USN_JOURNAL_DATA_V0));
	rujd.ReasonMask = -1;
	rujd.StartUsn = m_nextUsn;
	rujd.UsnJournalID = m_usnJournalId;

	static char s_buffer[0X20000] = { 0 };

	while (DeviceIoControl(m_handle, FSCTL_READ_USN_JOURNAL, &rujd, sizeof(rujd), s_buffer, sizeof(s_buffer), &retBytes, NULL))
	{
		retBytes -= sizeof(USN);
		if (retBytes == 0) break;
		PUSN_RECORD pUsnRecord = (PUSN_RECORD)(s_buffer + sizeof(USN));
		while (retBytes > 0)
		{
			DWORD reason = pUsnRecord->Reason;
			if ((reason & USN_REASON_CLOSE) && (reason & (USN_REASON_FILE_CREATE | USN_REASON_RENAME_NEW_NAME)))
			{
				addFile(pUsnRecord);
			}
			if ((reason & USN_REASON_CLOSE) && (reason & USN_REASON_FILE_DELETE))
			{
				delFile(pUsnRecord);
			}
			else if (reason & USN_REASON_RENAME_OLD_NAME)
			{
				delFile(pUsnRecord);
			}

			if (reason & USN_REASON_BASIC_INFO_CHANGE)
			{
				chaAttr(pUsnRecord);
			}

			retBytes -= pUsnRecord->RecordLength;
			pUsnRecord = (PUSN_RECORD)(((PBYTE)pUsnRecord) + pUsnRecord->RecordLength);
		}
		rujd.StartUsn = *(USN*)s_buffer;
		m_nextUsn = *(USN*)s_buffer;
	}
}

void CIndexTable::addFile(PUSN_RECORD pUsnRecord)
{
	UINT frn = (UINT)pUsnRecord->FileReferenceNumber;
	UINT pfrn = (UINT)pUsnRecord->ParentFileReferenceNumber;
	if (!exist(frn))
	{
		insert(&buildIndexNode(pUsnRecord), frn);
		m_fileCount++;
	}
}

void CIndexTable::delFile(PUSN_RECORD pUsnRecord)
{
	UINT frn = (UINT)pUsnRecord->FileReferenceNumber;
	if (exist(frn))
	{
		erase(frn);
		m_fileCount--;
	}
}

void CIndexTable::chaAttr(PUSN_RECORD pUsnRecord)
{
	UINT frn = (UINT)pUsnRecord->FileReferenceNumber;
	if (exist(frn))
	{
		at(frn).changeAttributes(selectAttributes(pUsnRecord->FileAttributes));
	}
}

void CIndexTable::appendPath(UINT frn, char*& path)
{
	if (frn < m_tableLength && m_table[frn].info)
	{
		appendPath(m_table[frn].info & 0X07FFFFFF, path);
		PNODE pNameNode = GlobalDataV0::g_nodePool.translate(m_table[frn].name);
		memcpy(path, pNameNode->str, pNameNode->length);
		path += pNameNode->length;
		*path++ = '\\';
	}
	else
	{
		*path++ = m_id;
		*path++ = ':';
		*path++ = '\\';
	}
}

BYTE CIndexTable::selectAttributes(DWORD dwSysAttr)
{
	UINT ret = 0;
	if (dwSysAttr & FILE_ATTRIBUTE_SYSTEM)
	{
		ret |= ATTR_SYS;
	}
	if (dwSysAttr & FILE_ATTRIBUTE_READONLY)
	{
		ret |= ATTR_RON;
	}
	if (dwSysAttr & FILE_ATTRIBUTE_HIDDEN)
	{
		ret |= ATTR_HID;
	}
	if (!ret)
	{
		ret = ATTR_NOR;
	}
	return ret;
}

INDEX_NODE& CIndexTable::buildIndexNode(PUSN_RECORD pUsnRecord)
{
	static INDEX_NODE s_iNode;
	PWCHAR name = (PWCHAR)((PBYTE)pUsnRecord + pUsnRecord->FileNameOffset), suff = NULL;
	USHORT nameLength = pUsnRecord->FileNameLength >> 1, suffLength = 0;
	if (!(pUsnRecord->FileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		strpro::splitSuffix(name, nameLength, suff, suffLength);
	}
	s_iNode.name = GlobalDataV0::g_nodePool.nameInsert(name, nameLength);
	s_iNode.suff = suff ? GlobalDataV0::g_nodePool.suffInsert(suff, suffLength) : NULL;
	s_iNode.info = (UINT)(pUsnRecord->ParentFileReferenceNumber);
	s_iNode.info |= (selectAttributes(pUsnRecord->FileAttributes) << 28);
	if (pUsnRecord->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		s_iNode.info |= INDEX_TYPEDIR;
	}
	return s_iNode;
}

void CIndexTable::save(FILE* output)
{
	SAVE_NODE sNode;
	sNode.id = m_id;
	sNode.nextUsn = m_nextUsn;
	sNode.fileCount = m_fileCount;
	sNode.tableLength = m_tableLength;
	sNode.usnJournalId = m_usnJournalId;

	fwrite(&sNode, sizeof(sNode), 1, output);
	fwrite(m_table, m_tableLength * sizeof(INDEX_NODE), 1, output);
}

bool CIndexTable::read(FILE* input)
{
	clear();

	SAVE_NODE sNode;
	if (1 != fread(&sNode, sizeof(sNode), 1, input))
	{
		return false;
	}
	if (sNode.id != m_id || sNode.usnJournalId != m_usnJournalId || sNode.nextUsn < m_firstUsn)
	{
		return false;
	}

	m_nextUsn = sNode.nextUsn;
	m_fileCount = sNode.fileCount;
	m_tableLength = sNode.tableLength;

	m_table = (PINDEX_NODE)std::malloc(m_tableLength * sizeof(INDEX_NODE));
	if (!m_table)
	{
		throw std::bad_alloc();
	}
	if (1 != fread(m_table, m_tableLength * sizeof(INDEX_NODE), 1, input))
	{
		return false;
	}
	return true;
}
