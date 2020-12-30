#ifndef MODE_H
#define MODE_H
/**
 * @author Mateusz Kordowski
 * 
 */

#include <iostream>

struct Mode{

    enum class Operation{
        Read,
        Write,
        ReadWrite        
    };

    enum class Type{
        File,
        Catalog
    };

    Operation operation;
    Type type;

    Mode(Operation oper, Type type)
    : operation(oper), type(type)
    {}

    Mode(const Mode& other) = default;
    Mode(Mode&& other) = default;
    Mode& operator=(const Mode& other) = default;
    Mode& operator=(Mode&& other) = default;

    friend std::ostream& operator<<(std::ostream& os, const Mode& mode){
        return os << static_cast<std::string>(mode);
    }

    operator std::string() const{
        const char* read = "Read";
        const char* write = "Write";
        const char* readwrite = "ReadWrite";
        const char* file = "File";
        const char* catalog = "Catalog";
        const char * one, *two;
        switch(operation){
            case Mode::Operation::Read:
                one = read;
                break;
            case Mode::Operation::Write:
                one = write;
                break;
            case Mode::Operation::ReadWrite:
                one = readwrite;
                break;
        }

        switch(type){
            case Mode::Type::File:
                two = file;
                break;
            case Mode::Type::Catalog:
                two = catalog;
                break;
        }

        return "Operation: " + std::string(one) + "\tType: " + std::string(two);
    }

};


#endif