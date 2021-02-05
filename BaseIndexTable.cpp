#include "BaseIndexTable.h"

CBaseIndexTable::CBaseIndexTable()
{
    m_tableLength = 0;
    m_table = NULL;
}

CBaseIndexTable::~CBaseIndexTable()
{
    if (m_table)
    {
        std::free(m_table);
    }
}

bool CBaseIndexTable::exist(UINT index)
{
    if (index >= m_tableLength || !m_table[index].info)
    {
        return false;
    }
    return true;
}

void CBaseIndexTable::insert(PINDEX_NODE pNode, UINT index)
{
    if (index >= m_tableLength)
    {
        UINT temp = m_tableLength;
        while (index >= m_tableLength)
        {
            m_tableLength += 0X20000;
        }
        PINDEX_NODE _table = m_table;
        m_table = (PINDEX_NODE)std::realloc(m_table, m_tableLength * sizeof(INDEX_NODE));
        if (!m_table)
        {
            std::free(_table);
            throw std::bad_alloc();
        }
        memset(m_table + temp, 0, (m_tableLength - temp) * sizeof(INDEX_NODE));
    }
    memcpy(m_table + index, pNode, sizeof(INDEX_NODE));
}

void CBaseIndexTable::erase(UINT index)
{
    if (index >= m_tableLength)
    {
        return;
    }
    if (m_table[index].name)
    {
        GlobalDataV0::g_nodePool.erase(m_table[index].name);
    }
    if (m_table[index].suff)
    {
        GlobalDataV0::g_nodePool.erase(m_table[index].suff);
    }
    memset(m_table + index, 0, sizeof(INDEX_NODE));
}

UINT CBaseIndexTable::length()
{
    return m_tableLength;
}

INDEX_NODE& CBaseIndexTable::at(UINT index)
{
    return m_table[index];
}

INDEX_NODE& CBaseIndexTable::operator[](UINT index)
{
    return m_table[index];
}

void CBaseIndexTable::clear()
{
    if (m_table)
    {
        std::free(m_table);
        m_table = NULL;
    }
    m_tableLength = 0;
}

void CBaseIndexTable::clean()
{
    if (m_tableLength < 0X20000)
    {
        return;
    }
    for (UINT i = m_tableLength - 1; i >= 0X20000; --i)
    {
        if (m_table[i].info != 0)
        {
            m_tableLength = i + 1;
            break;
        }
    }
    PINDEX_NODE _table = m_table;
    m_table = (PINDEX_NODE)std::realloc(m_table, m_tableLength * sizeof(INDEX_NODE));
    if (!m_table)
    {
        std::free(_table);
        throw std::bad_alloc();
    }
}
