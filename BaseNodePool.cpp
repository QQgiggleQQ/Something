#include "BaseNodePool.h"

CBaseNodePool::CBaseNodePool()
{
    initialize();
}

CBaseNodePool::~CBaseNodePool()
{
    destroy();
}

void CBaseNodePool::initialize()
{
    m_chunk = NULL;
    m_virtCount = 0;
    m_tailOffset = 0xFFFF;
    m_tailChunk = 0xFFFFFFFF;
    memset(m_recyBin, 0, sizeof(m_recyBin));

    m_table = (UINT*)std::malloc(TableLength * sizeof(UINT));
    if (!m_table)
    {
        throw std::bad_alloc();
    }
    memset(m_table, 0, TableLength * sizeof(UINT));
}

void CBaseNodePool::destroy()
{
    if (m_chunk)
    {
        for (UINT i = 0; i < m_virtCount; ++i)
        {
            if (m_chunk[i])
            {
                std::free(m_chunk[i]);
                m_chunk[i] = NULL;
            }
        }
        std::free(m_chunk);
        m_chunk = NULL;
    }

    if (m_table)
    {
        std::free(m_table);
        m_table = NULL;
    }
}

void CBaseNodePool::clear()
{
    if (m_chunk)
    {
        for (UINT i = 0; i < m_virtCount; ++i)
        {
            if (m_chunk[i])
            {
                std::free(m_chunk[i]);
                m_chunk[i] = NULL;
            }
        }
        std::free(m_chunk);
        m_chunk = NULL;
    }

    m_chunk = NULL;
    m_virtCount = 0;
    m_tailOffset = 0xFFFF;
    m_tailChunk = 0xFFFFFFFF;
    memset(m_recyBin, 0, sizeof(m_recyBin));
    memset(m_table, 0, TableLength * sizeof(UINT));
}

void* CBaseNodePool::allocate(USHORT size)
{
    if (size < 64 && m_recyBin[size])
    {
        UINT address = m_recyBin[size];
        UINT* ret = (UINT*)(m_chunk[address >> 16] + (address & 0XFFFF));
        m_recyBin[size] = *ret;
        *ret = address;
        return ret;
    }
    if (ChunkSize - m_tailOffset < size)
    {
        if (ChunkSize - m_tailOffset >= NODE_MINSIZE)
        {
            recycle((((UINT)m_tailChunk) << 16) | m_tailOffset, ChunkSize - m_tailOffset);
        }
        if (++m_tailChunk >= m_virtCount)
        {
            BYTE** _chunk = m_chunk;
            m_chunk = (BYTE**)std::realloc(m_chunk, (m_virtCount + 64) * sizeof(BYTE*));
            if (!m_chunk)
            {
                throw std::bad_alloc();
            }
            memset(m_chunk + m_virtCount, 0, 64 * sizeof(BYTE*));
            m_virtCount += 64;
        }
        if (!m_chunk[m_tailChunk])
        {
            m_chunk[m_tailChunk] = (BYTE*)std::malloc(ChunkSize);
            if (!m_chunk[m_tailChunk])
            {
                throw std::bad_alloc();
            }
        }
        m_tailOffset = m_tailChunk ? 0 : 1;
    }
    UINT* ret = (UINT*)(m_chunk[m_tailChunk] + m_tailOffset);
    *ret = (((UINT)m_tailChunk) << 16) | m_tailOffset;
    m_tailOffset += size;
    return ret;
}

#define SIZE_SPLIT(a, b, c) { b = a; a = a >> 1; b = b - a; c = b / 3; b = b - c; }

void CBaseNodePool::recycle(UINT address, USHORT size)
{
    if (size >= 64)
    {
        UINT size2, size3;
        SIZE_SPLIT(size, size2, size3);
        recycle(address, size);
        recycle(address + size, size2);
        recycle(address + size + size2, size3);
    }
    else
    {
        UINT* rec = (UINT*)(m_chunk[address >> 16] + (address & 0XFFFF));
        *rec = m_recyBin[size];
        m_recyBin[size] = address;
    }
}

void CBaseNodePool::erase(UINT address)
{
    PNODE pNode = (PNODE)(m_chunk[address >> 16] + (address & 0XFFFF));
    if (pNode->cite == 0XFF || --pNode->cite)
    {
        return;
    }

    UINT index = hash(pNode->str, pNode->length) % TableLength;

    if (m_table[index] == address)
    {
        m_table[index] = pNode->next;
    }
    else
    {
        PNODE pPrev = (PNODE)(m_chunk[m_table[index] >> 16] + (m_table[index] & 0XFFFF));
        while (pPrev && pPrev->next != address)
        {
            pPrev = (PNODE)(m_chunk[pPrev->next >> 16] + (pPrev->next & 0XFFFF));
        }
        if (pPrev)
        {
            pPrev->next = pNode->next;
        }
    }
    recycle(address, pNode->length + NODE_MINSIZE);
}

PNODE CBaseNodePool::translate(UINT address)
{
    return (PNODE)(m_chunk[address >> 16] + (address & 0xFFFF));
}

UINT CBaseNodePool::_nodeCount()
{
    UINT nodeCount = 0;
    for (UINT i = 0; i < TableLength; ++i)
    {
        UINT address = m_table[i];
        while (address)
        {
            PNODE pNode = (PNODE)(m_chunk[address >> 16] + (address & 0XFFFF));
            nodeCount++;
            address = pNode->next;
        }
    }
    return nodeCount;
}

UINT CBaseNodePool::_nameCount()
{
    UINT nameCount = 0;
    for (UINT i = 0; i < TableLength; ++i)
    {
        UINT address = m_table[i];
        while (address)
        {
            PNODE pNode = (PNODE)(m_chunk[address >> 16] + (address & 0XFFFF));
            if (pNode->mark & NAME_SPECIFY)
            {
                nameCount++;
            }
            address = pNode->next;
        }
    }
    return nameCount;
}

UINT CBaseNodePool::_suffCount()
{
    UINT suffCount = 0;
    for (UINT i = 0; i < TableLength; ++i)
    {
        UINT address = m_table[i];
        while (address)
        {
            PNODE pNode = (PNODE)(m_chunk[address >> 16] + (address & 0XFFFF));
            if (!(pNode->mark & NAME_SPECIFY))
            {
                suffCount++;
            }
            address = pNode->next;
        }
    }
    return suffCount;
}

UINT CBaseNodePool::_recyCount()
{
    UINT recyCount = 0;
    for (UINT i = 0; i < 64; ++i)
    {
        UINT address = m_recyBin[i];
        while (address)
        {
            PNODE pNode = (PNODE)(m_chunk[address >> 16] + (address & 0XFFFF));
            recyCount++;
            address = pNode->next;
        }
    }
    return recyCount;
}

UINT CBaseNodePool::hash(char* str)
{
    register UINT key = 0;

    while (*str != 0)
    {
        key = 131 * key + *str++;
    }

    return key;
}

UINT CBaseNodePool::hash(char* str, USHORT length)
{
    register UINT key = 0;

    char* end = str + length;
    while (str != end)
    {
        key = 131 * key + *str++;
    }

    return key;
}

PNODE CBaseNodePool::newNode(char* str, USHORT length, BYTE mark)
{
    PNODE pNode = (PNODE)allocate(NODE_MINSIZE + length);
    pNode->cite = 1;
    pNode->mark = mark;
    pNode->length = length;
    memcpy(pNode->str, str, length);
    return pNode;
}

UINT CBaseNodePool::insert(char* str, USHORT length, BYTE mark)
{
    UINT index = hash(str) % TableLength;
    UINT address = m_table[index];
    PNODE pNode = NULL;
    while (address)
    {
        pNode = (PNODE)(m_chunk[address >> 16] + (address & 0XFFFF));
        if (pNode->equal(str, length, mark))
        {
            break;
        }
        address = pNode->next;
    }
    if (!address)
    {
        pNode = newNode(str, length, mark);
        address = *(UINT*)pNode;
        pNode->next = m_table[index];
        m_table[index] = address;
    }
    else
    {
        if (pNode->cite != 0XFF)
        {
            pNode->cite++;
        }
    }
    return address;
}