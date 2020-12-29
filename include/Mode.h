#ifndef MODE_H
#define MODE_H
/**
 * @author Mateusz Kordowski
 * 
 */

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

};


#endif