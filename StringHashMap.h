#pragma once

#include <stdexcept>

template<typename ValueType>
class TStringHashMap {
public:
	TStringHashMap(unsigned int _tableLength, ValueType _badValue);
	virtual ~TStringHashMap();

	int insert(const char* str, ValueType value);
	int insert(const char* str, const char* end, ValueType value);
	ValueType operator[](const char* str);
	ValueType value(const char* str);
	ValueType value(const char* str, const char* end);

	unsigned int _tableLength();
	unsigned int _usingLength();
	float loadFactor();
	float ASL();

protected:
	const unsigned int MaxTableLength = 2181271;
	const unsigned int TableLengthArray[24] = { 7, 17, 31, 79, 163, 317, 673, 1361, 2729, 5471, 10949, 21911, 38833, 69061, 122777, 218357, 388211, 690163, 1226959, 2181271 };

	typedef struct _Node {
		unsigned int hash1;
		unsigned int hash2;
		ValueType value;

		_Node() : hash1(0), hash2(0), value() {}
	}Node;

	Node* m_hashTable;
	ValueType m_badValue;
	unsigned int m_tableLength;
	unsigned int m_usingLength;

protected:
	unsigned int nextLength(unsigned int nowLength);
	void hash(const char* str, unsigned int& hash1, unsigned int& hash2);
	void hash(const char* str, const char* end, unsigned int& hash1, unsigned int& hash2);
	void expandTable();
};

template<typename ValueType>
inline TStringHashMap<ValueType>::TStringHashMap(unsigned int _tableLength, ValueType _badValue)
{
	m_usingLength = 0;
	m_badValue = _badValue;
	m_tableLength = nextLength(_tableLength);
	m_hashTable = new Node[m_tableLength];
	for (unsigned int i = 0; i < m_tableLength; ++i)
	{
		m_hashTable[i].value = m_badValue;
	}
}

template<typename ValueType>
inline TStringHashMap<ValueType>::~TStringHashMap()
{
	if (m_hashTable)
	{
		delete[] m_hashTable;
	}
}

template<typename ValueType>
inline int TStringHashMap<ValueType>::insert(const char* str, ValueType value)
{
	if (!str)
	{
		return 0; // 插入失败返回零
	}
	if ((m_tableLength >> 1) <= m_usingLength) // loadFactor始终 < 0.5
	{
		expandTable();
	}
	unsigned int hash1 = 0;
	unsigned int hash2 = 0;
	hash(str, hash1, hash2);
	unsigned int addr = hash1 % m_tableLength;
	while (m_hashTable[addr].value != m_badValue)
	{
		if (m_hashTable[addr].hash1 == hash1 && m_hashTable[addr].hash2 == hash2)
		{
			return 0; // 插入失败返回零
		}
		addr = (addr + 1) % m_tableLength;
	}
	m_hashTable[addr].hash1 = hash1;
	m_hashTable[addr].hash2 = hash2;
	m_hashTable[addr].value = value;
	m_usingLength++;
	return (addr + m_tableLength - (hash1 % m_tableLength)) % m_tableLength + 1; // 插入成功返回当前查找长度
}

template<typename ValueType>
inline int TStringHashMap<ValueType>::insert(const char* str, const char* end, ValueType value)
{
	if (!str || !end)
	{
		return 0; // 插入失败返回零
	}
	if ((m_tableLength >> 1) <= m_usingLength) // loadFactor始终 < 0.5
	{
		expandTable();
	}
	unsigned int hash1 = 0;
	unsigned int hash2 = 0;
	hash(str, end, hash1, hash2);
	unsigned int addr = hash1 % m_tableLength;
	while (m_hashTable[addr].value != m_badValue)
	{
		if (m_hashTable[addr].hash1 == hash1 && m_hashTable[addr].hash2 == hash2)
		{
			return 0; // 插入失败返回零
		}
		addr = (addr + 1) % m_tableLength;
	}
	m_hashTable[addr].hash1 = hash1;
	m_hashTable[addr].hash2 = hash2;
	m_hashTable[addr].value = value;
	m_usingLength++;
	return (addr + m_tableLength - (hash1 % m_tableLength)) % m_tableLength + 1; // 插入成功返回当前查找长度
}

template<typename ValueType>
inline ValueType TStringHashMap<ValueType>::operator[](const char* str)
{
	if (!str)
	{
		return m_badValue;
	}
	unsigned int hash1 = 0;
	unsigned int hash2 = 0;
	hash(str, hash1, hash2);
	unsigned int addr = hash1 % m_tableLength;
	while (m_hashTable[addr].value != m_badValue)
	{
		if (m_hashTable[addr].hash1 == hash1 && m_hashTable[addr].hash2 == hash2)
		{
			break;
		}
		addr = (addr + 1) % m_tableLength;
	}
	return m_hashTable[addr].value; // 查找失败返回badValue
}

template<typename ValueType>
inline ValueType TStringHashMap<ValueType>::value(const char* str)
{
	if (!str)
	{
		return m_badValue;
	}
	unsigned int hash1 = 0;
	unsigned int hash2 = 0;
	hash(str, hash1, hash2);
	unsigned int addr = hash1 % m_tableLength;
	while (m_hashTable[addr].value != m_badValue)
	{
		if (m_hashTable[addr].hash1 == hash1 && m_hashTable[addr].hash2 == hash2)
		{
			break;
		}
		addr = (addr + 1) % m_tableLength;
	}
	return m_hashTable[addr].value; // 查找失败返回badValue
}

template<typename ValueType>
inline ValueType TStringHashMap<ValueType>::value(const char* str, const char* end)
{
	if (!str || !end)
	{
		return m_badValue;
	}
	unsigned int hash1 = 0;
	unsigned int hash2 = 0;
	hash(str, end, hash1, hash2);
	unsigned int addr = hash1 % m_tableLength;
	while (m_hashTable[addr].value != m_badValue)
	{
		if (m_hashTable[addr].hash1 == hash1 && m_hashTable[addr].hash2 == hash2)
		{
			break;
		}
		addr = (addr + 1) % m_tableLength;
	}
	return m_hashTable[addr].value; // 查找失败返回badValue
}

template<typename ValueType>
inline unsigned int TStringHashMap<ValueType>::_tableLength()
{
	return m_tableLength;
}

template<typename ValueType>
inline unsigned int TStringHashMap<ValueType>::_usingLength()
{
	return m_usingLength;
}

template<typename ValueType>
inline float TStringHashMap<ValueType>::loadFactor()
{
	if (!m_tableLength)
	{
		return 0.0f;
	}
	return ((float)m_usingLength) / ((float)m_tableLength);
}

template<typename ValueType>
inline float TStringHashMap<ValueType>::ASL()
{
	if (!m_usingLength)
	{
		return 0.0f;
	}
	float sum = 0.0f;
	for (unsigned int i = 0; i < m_tableLength; ++i)
	{
		if (m_hashTable[i].value != m_badValue)
		{
			sum += (i + m_tableLength - m_hashTable[i].hash1 % m_tableLength) % m_tableLength + 1;
		}
	}
	return sum / (float)m_usingLength;
}

template<typename ValueType>
inline unsigned int TStringHashMap<ValueType>::nextLength(unsigned int nowLength)
{
	if (nowLength >= MaxTableLength)
	{
		throw std::length_error("Table length out of range");
	}
	else
	{
		for (int i = 0; ; ++i)
		{
			if (TableLengthArray[i] > nowLength)
			{
				return TableLengthArray[i];
			}
		}
	}
}

template<typename ValueType>
inline void TStringHashMap<ValueType>::hash(const char* str, unsigned int& hash1, unsigned int& hash2)
{
	hash1 = 0;
	hash2 = 0;
	while (*str != 0)
	{
		hash1 = hash1 * 131 + *str; // BKDRHash
		hash2 = *str++ + (hash2 << 6) + (hash2 << 16) - hash2; // SDBMHash
	}
}

template<typename ValueType>
inline void TStringHashMap<ValueType>::hash(const char* str, const char* end, unsigned int& hash1, unsigned int& hash2)
{
	hash1 = 0;
	hash2 = 0;
	while (str != end)
	{
		hash1 = hash1 * 131 + *str; // BKDRHash
		hash2 = *str++ + (hash2 << 6) + (hash2 << 16) - hash2; // SDBMHash
	}
}

template<typename ValueType>
inline void TStringHashMap<ValueType>::expandTable()
{
	Node* tempTable = m_hashTable;
	unsigned int tempLength = m_tableLength;
	m_tableLength = nextLength(m_tableLength);
	m_hashTable = new Node[m_tableLength];
	for (unsigned int i = 0; i < m_tableLength; ++i)
	{
		m_hashTable[i].value = m_badValue;
	}
	for (unsigned int i = 0; i < tempLength; ++i)
	{
		if (tempTable[i].value != m_badValue)
		{
			unsigned int addr = tempTable[i].hash1 % m_tableLength;
			while (m_hashTable[addr].value != m_badValue)
			{
				addr = (addr + 1) % m_tableLength;
			}
			m_hashTable[addr] = tempTable[i];
		}
	}
	if (tempTable)
	{
		delete[] tempTable;
	}
}
