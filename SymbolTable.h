#ifndef HW3_SYMBOLTABLE_H
#define HW3_SYMBOLTABLE_H

#include <iostream>
#include <climits>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "hw3_output.hpp"
#include "GenericType.h"

extern int yylineno;

using namespace std;

class Symbol {
private:
    string name, type;
    int offset;
    bool isOverride;

public:
    Symbol(const string &name, const string &type, int offset, bool isOverride) : name(name), type(type),
                                                                                  offset(offset),
                                                                                  isOverride(isOverride) {
    }

    string getName() const {
        return name;
    }

    string getType() const {
        return type;
    }

    int getOffset() const {
        return offset;
    }

    bool getIsOverride() const {
        if (!isFunction()) {
            throw exception();
        }
        return isOverride;
    };

    bool isFunction() const {
        return type.find("->") != string::npos;
    }

    string getReturnType() const {
        if (!isFunction()) {
            throw exception();
        }
        return type.substr(type.find("->") + 2);
    }

    string getParametersStr() const {
        if (!isFunction()) {
            throw exception();
        }
        return type.substr(1, type.find("->") - 2);
    }

    vector<string> getParametersVec() const {
        string parametersType = getParametersStr();
        vector<string> result;
        istringstream iss(parametersType);
        std::string token;
        while (std::getline(iss, token, ',')) {
            result.push_back(token);
        }
        return result;
    }

    string getVarType() const {
        if (isFunction()) {
            throw exception();
        }
        return type;
    }

    bool canOverride(const Symbol &baseFunc) const {
        if (!isFunction() || !baseFunc.isFunction()) {
            throw exception();
        }
        if (isOverride && baseFunc.isOverride && name == baseFunc.name &&
            (getReturnType() != baseFunc.getReturnType() || getParametersStr() != baseFunc.getParametersStr())) {
            return true;
        }
        return false;
    }
};

class SymbolTable {
private:
    vector<vector<Symbol>> tablesStack;
    vector<int> offsetsStack;
    int activeLoops;
    bool mainEntry;

public:
    SymbolTable() {
        activeLoops = 0;
        mainEntry = false;
        vector<Symbol> baseTable;
        tablesStack.push_back(baseTable);
        offsetsStack.push_back(0);
        vector<FormalDecl> printArg = {FormalDecl("STRING", "innerString")};
        vector<FormalDecl> printiArg = {FormalDecl("INT", "innerInt")};
        insertFunc("print", "VOID", printArg, false, true);
        insertFunc("printi", "VOID", printiArg, false, true);
    };

    int getActiveLoops() const {
        return activeLoops;
    }

    bool getMainEntry() const {
        return mainEntry;
    }

    void insertTable(bool innerLoop = false) {
        tablesStack.push_back(vector<Symbol>{});
        offsetsStack.push_back(offsetsStack.back());
        if (innerLoop) {
            ++activeLoops;
        }
    }

    void removeTable(bool innerLoop = false) {
        tablesStack.pop_back();
        offsetsStack.pop_back();
        if (innerLoop) {
            --activeLoops;
        }
    }

    void insertVar(const string &varType, const string &varName) {
        Symbol varSymbol(varName, varType, offsetsStack.back(), false);
        tablesStack.back().push_back(varSymbol);
        ++offsetsStack.back();
    }

    void insertArg(const string &varName, const string &varType, int offset) {
        Symbol varSymbol(varName, varType, offset, false);
        tablesStack.back().push_back(varSymbol);
    }

    void insertFunc(const string &funcName, const string &returnType, vector<FormalDecl> &argC, bool isOverride,
                    bool isInternal = false) {
        if (funcName == "main" && returnType == "VOID" && argC.empty()) {
            if (!mainEntry) {
                mainEntry = true;
            }
        }
        vector<string> argTypes{};
        argTypes.reserve(argC.size());
        for (int i = 0; i < argC.size(); ++i) {
            argTypes.push_back(argC[i].Type);
        }
        string funcType = output::makeFunctionType(returnType, argTypes);
        Symbol funcSymbol(funcName, funcType, 0, isOverride);
        tablesStack.back().push_back(funcSymbol);
        if (isInternal) {
            return;
        }
        insertTable();
        for (int i = 0; i < argC.size(); ++i) {
            if (symbolExists(argC[i].Name)) {
                output::errorDef(yylineno, argC[i].Name);
                exit(1);
            }
            insertArg(argC[i].Name, argC[i].Type, (i + 1) * (-1));
        }
    }

    const Symbol *getVar(const string &name) {
        for (unsigned int i = 0; i < tablesStack.size(); ++i) {
            for (unsigned int j = 0; j < tablesStack[i].size(); ++j) {
                if (tablesStack[i][j].getName() == name && !tablesStack[i][j].isFunction()) {
                    return &tablesStack[i][j];
                }
            }
        }
        return nullptr;
    }

    vector<Symbol> getFuncs(const string &name) {
        vector<Symbol> matchingFuncs{};
        for (unsigned int i = 0; i < tablesStack.size(); ++i) {
            for (unsigned int j = 0; j < tablesStack[i].size(); ++j) {
                if (tablesStack[i][j].getName() == name && tablesStack[i][j].isFunction()) {
                    matchingFuncs.push_back(tablesStack[i][j]);
                }
            }
        }
        return matchingFuncs;
    }

    bool symbolExists(const string &name) {
        const Symbol *tVar = getVar(name);
        if (tVar != nullptr) {
            return true;
        } else {
            return !getFuncs(name).empty();
        }
    }

    string getRetType() {
        return tablesStack[0].back().getReturnType();
    }

    void checkOverride(const string &funcName, const string &returnType, vector<FormalDecl> &argC, bool isOverride) {
        vector<Symbol> funcEntries = getFuncs(funcName);
        vector<string> argTypes;
        argTypes.reserve(argC.size());
        for (int i = 0; i < argC.size(); ++i) {
            argTypes.push_back(argC[i].Type);
        }
        string funcType = output::makeFunctionType(returnType, argTypes);
        Symbol tmp(funcName, funcType, 0, isOverride);
        for (int i = 0; i < funcEntries.size(); ++i) {
            if (!funcEntries[i].getIsOverride() && isOverride) {
                output::errorFuncNoOverride(yylineno, funcName);
                exit(1);
            } else if (funcEntries[i].getIsOverride() && !isOverride) {
                output::errorOverrideWithoutDeclaration(yylineno, funcName);
                exit(1);
            } else if (!funcEntries[i].getIsOverride() && !isOverride) {
                output::errorDef(yylineno, funcName);
                exit(1);
            } else {
                if (!tmp.canOverride(funcEntries[i])) {
                    output::errorDef(yylineno, funcName);
                    exit(1);
                }
            }
        }
    }

    string validateExpList(const string &funcName, ExpList *argsList) {
        vector<Symbol> callableFuncs{};
        vector<Symbol> funcEntries = getFuncs(funcName);
        if (funcEntries.empty()) {
            throw exception();
        }
        for (int i = 0; i < funcEntries.size(); ++i) {
            vector<string> parametersType = funcEntries[i].getParametersVec();
            if (argsList == nullptr) {
                if (parametersType.empty()) {
                    callableFuncs.push_back(funcEntries[i]);
                }
            } else {
                if (parametersType.size() != argsList->Vec.size()) {
                    continue;
                }
                bool isLegal = true;
                for (int j = 0; j < argsList->Vec.size(); ++j) {
                    string argType = argsList->Vec[j].Type;
                    if (argType == parametersType[j] || (argType == "BYTE" && parametersType[j] == "INT")) {
                    } else {
                        isLegal = false;
                        break;
                    }
                }
                if (isLegal) {
                    callableFuncs.push_back(funcEntries[i]);
                }
            }
        }
        if (callableFuncs.size() > 1) {
            output::errorAmbiguousCall(yylineno, funcName);
            exit(1);
        } else if (callableFuncs.empty()) {
            output::errorPrototypeMismatch(yylineno, funcName);
            exit(1);
        }
        return callableFuncs[0].getReturnType();
    }

    void closeScope(bool innerLoop = false) {
        output::endScope();
        vector<Symbol> tmp = tablesStack.back();
        for (int i = 0; i < tmp.size(); ++i) {
            output::printID(tmp[i].getName(), tmp[i].getOffset(), tmp[i].getType());
        }
        removeTable(innerLoop);
    }
};

#endif //HW3_SYMBOLTABLE_H
