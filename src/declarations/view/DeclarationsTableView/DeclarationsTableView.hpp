#pragma once

#include <Declaration.hpp>
#include <TableView.hpp>
#include <string>

#include "DeclarationsTable.hpp"

class DeclarationsTableView {
private:
    TableView<Declaration> _table;

public:
    DeclarationsTableView(std::string title, const DeclarationsTable& data);

    void print() const;
};
