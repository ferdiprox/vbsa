//
// Created by ferdinand on 5/24/25.
//

#pragma once

#include <string>
#include <list>

#include "common/parse-data.h"

#include "assembler/file-process/token/token-trace.h"

class FileMathProcessor {
public:
    FileMathProcessor(std::string& fileNameRef, std::list<Token>& tokensListRef)
        : fileName(fileNameRef), tokensList(tokensListRef) {};

    void execute();
private:

    struct ExpressionNode {
        enum NodeType {
            Value,
            Operator,
        };
        int data;
        NodeType type;

        ExpressionNode() = default;
        ExpressionNode(const int myData, const NodeType& myType) : data(myData), type(myType) {};
    };

    using expr_t = std::list<ExpressionNode>;
    using expr_iter_t = expr_t::iterator;

    std::string& fileName;
    std::list<Token>& tokensList;

    token_iter_t iter;

    token_iter_t expressionBegin, expressionEnd;

    bool isActuallyExpression();

    token_iter_t processExpression();

    expr_t cutExpression();
    int solveExpression(expr_t& expression);
    void solveSubexpressions(expr_t& expression);

    expr_t cutSubexpression(expr_iter_t &subiter, expr_t& sourceExpression);

    void seekAndSolve(expr_t& expression, OperatorType operatorType);
    int subSolve(expr_iter_t expriter);
};
