#ifndef HW3_GENERICTYPE_H
#define HW3_GENERICTYPE_H

#include <iostream>
#include <string>
#include "hw3_output.hpp"

using namespace std;

class GenericType {
public:
    string Type;

    GenericType() {
        Type = "N/A";
    }

    explicit GenericType(const string &Type) : Type(Type) {
    }
};

class VoidType : public GenericType {
public:
    VoidType() : GenericType("VOID") {
    }
};

class IntType : public GenericType {
public:
    IntType() : GenericType("INT") {
    }
};

class BoolType : public GenericType {
public:
    BoolType() : GenericType("BOOL") {
    }
};

class OverrideType : public GenericType {
public:
    OverrideType() : GenericType("OVERRIDE") {
    }
};

class ByteType : public GenericType {
public:
    ByteType() : GenericType("BYTE") {
    }
};

class IDType : public GenericType {
public:
    string Name;

    explicit IDType(const string &name) : GenericType("ID") {
        this->Name = name;
    }
};

class NumType : public GenericType {
public:
    int Value;

    explicit NumType(const string &value) : GenericType("NUM") {
        this->Value = stoi(value);
    }
};

class StringType : public GenericType {
public:
    string Value;

    explicit StringType(const string &value) : GenericType("STRING") {
        this->Value = value;
    }
};

/** **/

class Program : public GenericType {
};

class Funcs : public GenericType {
};

class FuncDecl : public GenericType {
};

class OverRide : public GenericType {
public:
    bool isOverride;

    explicit OverRide(bool isOverride) : isOverride(isOverride) {};
};

class RetType : public GenericType {
public:
    explicit RetType(const string &type) : GenericType(type) {
    }
};

class FormalDecl : public GenericType {
public:
    string Name;

    FormalDecl(const string &type, const string &name) : GenericType(type) {
        this->Name = name;
    }
};

class FormalsList : public GenericType {
public:
    vector<FormalDecl> Vec;

    void addFormalDecl(FormalDecl *fd) {
        if (fd != nullptr) {
            Vec.insert(Vec.begin(), *fd);
        }
    }
};

class Formals : public FormalsList {
};

class Statements : public GenericType {
};

class Statement : public GenericType {
};

class Call : public GenericType {
public:
    explicit Call(const string &type) : GenericType(type) {
    }
};

class TypeClass : public GenericType {
public:
    explicit TypeClass(const string &type) : GenericType(type) {
    }
};

class Exp : public GenericType {
public:
    explicit Exp(const string &type) : GenericType(type) {
    }
};

class ExpList : public GenericType {
public:
    vector<Exp> Vec;

    void addExp(Exp *exp) {
        if (exp != nullptr) {
            Vec.insert(Vec.begin(), *exp);
        }
    }
};

#endif //HW3_GENERICTYPE_H