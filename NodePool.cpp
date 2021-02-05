#include "NodePool.h"

CNodePool::CNodePool() : CBaseNodePool()
{
    memset(m_buffer, 0, sizeof(m_buffer));
}

UINT CNodePool::nameInsert(wchar_t* wname, USHORT length)
{
    BYTE mark = strpro::wideCharToMultiByte(wname, length, m_buffer) ? (NAME_SPECIFY | NAME_NOCHINA) : NAME_SPECIFY;
    return insert(m_buffer, length, mark);
}

UINT CNodePool::suffInsert(wchar_t* wsuff, USHORT length)
{
    strpro::wideCharToMultiByte(wsuff, length, m_buffer);
    strpro::tolower(m_buffer, length);
    return insert(m_buffer, length, m_suffixTypeMap[m_buffer]);
}

void CNodePool::save(FILE* output)
{
    SAVE_NODE sNode;
    sNode.virtCount = m_virtCount;
    sNode.tailChunk = m_tailChunk;
    sNode.tailOffset = m_tailOffset;
    fwrite(&sNode, sizeof(sNode), 1, output);
    fwrite(m_table, sizeof(UINT) * TableLength, 1, output);
    fwrite(m_recyBin, sizeof(m_recyBin), 1, output);
    for (UINT i = 0; i <= m_tailChunk; ++i)
    {
        fwrite(m_chunk[i], ChunkSize, 1, output);
    }
}

bool CNodePool::read(FILE* input)
{
    clear();

    SAVE_NODE sNode;
    if (1 != fread(&sNode, sizeof(sNode), 1, input))
    {
        return false;
    }
    if (1 != fread(m_table, sizeof(UINT) * TableLength, 1, input))
    {
        return false;
    }
    if (1 != fread(m_recyBin, sizeof(m_recyBin), 1, input))
    {
        return false;
    }

    m_virtCount = sNode.virtCount;
    m_tailChunk = sNode.tailChunk;
    m_tailOffset = sNode.tailOffset;

    m_chunk = (BYTE**)std::malloc(m_virtCount * sizeof(BYTE*));
    if (!m_chunk)
    {
        throw std::bad_alloc();
    }
    memset(m_chunk, 0, m_virtCount * sizeof(BYTE*));

    for (UINT i = 0; i <= m_tailChunk; ++i)
    {
        *(m_chunk + i) = (BYTE*)std::malloc(ChunkSize);
        if (!(*(m_chunk + i)))
        {
            throw std::bad_alloc();
        }
        if (1 != fread((*(m_chunk + i)), ChunkSize, 1, input))
        {
            return false;
        }
    }
    return true;
}

CNodePool GlobalDataV0::g_nodePool;
CResultTable GlobalDataV0::g_resultTable;
