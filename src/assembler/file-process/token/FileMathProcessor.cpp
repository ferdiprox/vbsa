//
// Created by ferdinand on 5/24/25.
//

#include "FileMathProcessor.h"

#include "assembler/file-process/inter-directive.h"
#include "common/parse-data.h"

void FileMathProcessor::execute() {
     for (iter = tokensList.begin(); iter != tokensList.end(); ++iter) {
          if (*iter != "{") {
               continue;
          }

          expressionBegin = iter;

          if (!isActuallyExpression()) {
               continue;
          }

          iter = processExpression();
     }
}

bool FileMathProcessor::isActuallyExpression() {
     ++iter;
     // may be expressions like ( ( ( 1 + 2 ) * 3 ) / 4 )
     // and we need to skip these '(' to reach math tokens
     while (*iter == "(") {
          ++iter;
     }
     retry_number:
     if (!isNumberString(*iter)) {
          if (iter->at(0) != INTERDIRECTIVE_SIGN) {
               iter = expressionBegin;
               return false;
          }

          ++iter;
          goto retry_number;
     }
     retry_operator:
     ++iter;
     if (!isOperatorChar(iter->at(0))) {
          if (iter->at(0) != INTERDIRECTIVE_SIGN) {
               iter = expressionBegin;
               return false;
          }
          goto retry_operator;
     }
     return true;
}

token_iter_t FileMathProcessor::processExpression() {
     while (++iter != tokensList.end()) {
          if (*iter == "}") {
               expressionEnd = iter;
               break;
          }
     }

     if (iter == tokensList.end()) {
          tokenError("couldn't find a '}' symbol as the end of math expression", expressionBegin);
     }

     ++expressionBegin;

     expr_t expression = cutExpression();

     int result =  solveExpression(expression);

     --expressionBegin;
     ++expressionEnd;

     return tokensList.insert(tokensList.erase(expressionBegin, expressionEnd), Token(std::to_string(result)));
}

FileMathProcessor::expr_t FileMathProcessor::cutExpression() {
     expr_t expression;

     for (iter = expressionBegin; iter != expressionEnd; ++iter) {
          if (isNumberString(*iter)) {
               expression.emplace_back().type = ExpressionNode::NodeType::Value;
               expression.back().data = std::stoi(*iter);
               continue;
          }

         std::optional<int> tokenOperator = getOperatorIndex(iter->at(0));

          if (!tokenOperator.has_value()) {
               if (iter->size() == 3) {
                    if (iter->at(0) == INTERDIRECTIVE_SIGN &&
                         iter->at(1) == InterdirectiveType::ProcessorBreakpoint &&
                         iter->at(2) == BreakpointTargetType::MathProcessor) {
                         call_breakpoint();
                         continue;
                    }
               }

               tokenError("unexpected token in math expression: " + *iter, iter);
          }

          expression.emplace_back().type = ExpressionNode::NodeType::Operator;
          expression.back().data = tokenOperator.value();
     }

     return expression;
}

#define FORCE_NODE_TYPE(type) if (expriter->type != ExpressionNode::NodeType::type) \
         { tokenError("invalid value-operator-value sequence in math expression", expressionBegin); }

int FileMathProcessor::solveExpression(expr_t& expression) {
     solveSubexpressions(expression);

     seekAndSolve(expression, OperatorType::Divide);
     seekAndSolve(expression, OperatorType::Multiply);
     seekAndSolve(expression, OperatorType::Minus);
     seekAndSolve(expression, OperatorType::Plus);

     return expression.front().data;
}

void FileMathProcessor::solveSubexpressions(expr_t& expression) {
     for (auto expriter = expression.begin(); expriter != expression.end(); ++expriter) {
          if (expriter->type != ExpressionNode::NodeType::Operator) {
               continue;
          }

          if (expriter->data != OperatorType::LeftBracket) {
               continue;
          }

          expr_t subexpression = cutSubexpression(expriter, expression);
          expression.insert(expriter, ExpressionNode(solveExpression(subexpression), ExpressionNode::NodeType::Value));
          --expriter;
     }
}

FileMathProcessor::expr_t FileMathProcessor::cutSubexpression(expr_iter_t &subiter, expr_t& sourceExpression) {
     expr_iter_t subexpressionBegin = subiter;
     expr_iter_t subexpressionEnd;
     expr_t subexpression;

     for (++subiter; subiter != sourceExpression.end(); ++subiter) {
          subexpression.push_back(*subiter);

          if (subiter->type != ExpressionNode::NodeType::Operator) {
               continue;
          }

          if (subiter->data == OperatorType::RightBracket) {
               subexpressionEnd = ++subiter;
               break;
          }
     }

     subiter = sourceExpression.erase(subexpressionBegin, subexpressionEnd);

     return subexpression;
}

void FileMathProcessor::seekAndSolve(expr_t& expression, OperatorType operatorType) {
     expr_iter_t erasingEnd;
     int result;
     for (auto expriter = expression.begin(); expriter != expression.end(); ++expriter) {
          if (expriter->type != ExpressionNode::NodeType::Operator){
               continue;
          }

          if (expriter->data != operatorType) {
              continue;
          }

          erasingEnd = expriter;
          ++erasingEnd;
          ++erasingEnd;
          result = subSolve(--expriter);

          expriter = expression.erase(expriter, erasingEnd);
          expriter = expression.insert(expriter, ExpressionNode(result, ExpressionNode::NodeType::Value));
     }
}

int FileMathProcessor::subSolve(expr_iter_t expriter) {
     int result;

     FORCE_NODE_TYPE(Value);
     int valueA = expriter->data;

     // dogshit
     ++expriter;
     ++expriter;
     FORCE_NODE_TYPE(Value);
     int valueB = expriter->data;

     --expriter;
     switch (expriter->data) {
          default:
               throw std::runtime_error("Unexpected operator type: " + std::to_string(expriter->data));
          case OperatorType::Divide:
               if (!valueB) {
                    tokenError("detected division by zero", expressionBegin);
               }

               result = valueA / valueB;
               break;
          case OperatorType::Multiply:
               result = valueA * valueB;
               break;
          case OperatorType::Minus:
               result = valueA - valueB;
               break;
          case OperatorType::Plus:
               result = valueA + valueB;
               break;
     }

     return result;
}
