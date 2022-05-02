#ifndef __COMMON_H
#define __COMMON_H
#include<iostream>
#include <string>
#include "EnumReflect.h"
/**
 * @brief 所有终结符
 * =: EQUAL
 * <: SMALLER
 * <=: SMALLER_EQUAL
 * >: GREATER
 * >=: GREATER_EQUAL
 * NULL: _NULL
 * <=>: SMALLER_EQUAL_GREATER
 * !=: NOT_EQUAL
 * (: LEFT_BRAKET
 * ,: COMMA
 * .: DOT
 * $: VAREPSILON
 * ||: OR_OR
 * &&: AND_AND
 * GROUP BY: GROUP_BY
 * ORDER BY: ORDER_BY
 * !: SIGH
 * *: MULT
 * -: SUB
 */
MAKE_ENUM(terminal_symbol_t, SELECT,FROM,WHERE,AS,INSERT,INTO,VALUES,UPDATE,DELETE,JOIN,LEFT,RIGHT,MIN,MAX,AVG,SUM,UNION,STRING,ON,VALUE,DEFAULT,SET,SIGH,MULT,SUB,
ALL,GROUP_BY,HAVING,DISTINCT,ORDER_BY,TRUE,FALSE,IS,NOT,_NULL,EQUAL,GREATER,SMALLER,GREATER_EQUAL,SMALLER_EQUAL,
NOT_EQUAL,SMALLER_EQUAL_GREATER,AND,AND_AND,OR_OR,OR,XOR,DOT,LEFT_BRAKET,COMMA,RIGHT_BRAKET,IDN,INT,FLOAT,UNKNOWN,
VAREPSILON);

MAKE_ENUM(noterminal_symbol_t,root,dmlStatement,selectStatement,insertStatement,updateStatement,deleteStatement,querySpecification,unionStatements,unionStatement,unionStatementKey,unionStatementQuery,unionType,
selectElements,selectClause,fromClause,groupByClause,havingClause,orderByClause,tableSources,whereExpression,
expressions,expression,selectElementHead,selectElementListRec,selectElement,fullColumnName,elementNameAlias,
functionCall,tableSourceListRec,tableSource,tableSourceItem,joinParts,joinPart,tableName,uid,uidList,
uidListRec,dottedId,expressionRec,opposite,predicate,expressionRight,logicalOperator,oppositeOrNot,trueValue,
expressionAtom,predicateRight,comparisonOperator,constant,stringLiteral,decimalLiteral,booleanLiteral,
aggregateWindowedFunction,joinRightPart,joinDirection,insertKeyword,insertStatementRight,insertStatementValue,into,
insertFormat,expressionsWithDefaults,expressionsWithDefaultsListRec,expressionOrDefault,expressionOrDefaultListRec,
updatedElement,updatedElementListRec,deleteStatementRight, function
);
//char *terminal_symbol[] = {"SELECT","FROM","WHERE","AS","INSERT","INTO","VALUES","UPDATE","DELETE","JOIN","LEFT","RIGHT","MIN","MAX","AVG","SUM","UNION","STRING","ON","VALUE","DEFAULT","SET","SIGH","MULT","SUB",
//                           "ALL","GROUP_BY","HAVING","DISTINCT","ORDER_BY","TRUE","FALSE","IS","NOT","_NULL","EQUAL","GREATER","SMALLER","GREATER_EQUAL","SMALLER_EQUAL",
//                           "NOT_EQUAL","SMALLER_EQUAL_GREATER","AND","AND_AND","OR_OR","OR","XOR","DOT","LEFT_BRAKET","COMMA","RIGHT_BRAKET","IDN","INT","FLOAT","UNKNOWN",
//                           "VAREPSILON"};

enum operation_t{ERROR, SHIFT, REDUCTION, GO, ACCEPT};

#endif