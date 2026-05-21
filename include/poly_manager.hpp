#pragma once

#include <string>
#include <vector>
#include <utility>
#include "polynomus.hpp"
#include "wrappers.hpp"

enum class TableType
{
    AVL,
    RB,
    ChainHash,
    OpenAddressHash,
    Unordered,
    Ordered
};

class PolyManager
{
private:
    ITableWrapper<std::string, Polynomus>* m_currentTable;
    TableType m_currentType;

public:
    PolyManager() : m_currentTable(nullptr), m_currentType(TableType::AVL)
    {
        setTableType(TableType::AVL);
    }

    ~PolyManager()
    {
        delete m_currentTable;
    }

    void setTableType(TableType type)
    {
        std::vector<std::pair<std::string, Polynomus>> oldData;
        
        if (m_currentTable)
        {
            m_currentTable->collectAll(oldData);
            delete m_currentTable;
        }

        m_currentType = type;

        switch (type)
        {
            case TableType::AVL:
            {
                m_currentTable = new AVLTreeWrapper<std::string, Polynomus>();
                break;
            }
            case TableType::RB:
            {
                m_currentTable = new RBTreeWrapper<std::string, Polynomus>();
                break;
            }
            case TableType::ChainHash:
            {
                m_currentTable = new ChainHashTableWrapper<std::string, Polynomus>();
                break;
            }
            case TableType::OpenAddressHash:
            {
                m_currentTable = new OpenAddressHashTableWrapper<std::string, Polynomus>();
                break;
            }
            case TableType::Unordered:
            {
                m_currentTable = new UnorderedTableWrapper<std::string, Polynomus>();
                break;
            }
            case TableType::Ordered:
            {
                m_currentTable = new OrderedTableWrapper<std::string, Polynomus>();
                break;
            }
        }

        for (const auto& pair : oldData)
        {
            m_currentTable->insert(pair.first, pair.second);
        }
    }

    ITableWrapper<std::string, Polynomus>* getTable() const
    {
        return m_currentTable;
    }

    TableType getCurrentType() const
    {
        return m_currentType;
    }
};