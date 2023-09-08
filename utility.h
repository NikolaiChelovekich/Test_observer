#ifndef UTILITY_H
#define UTILITY_H

#include<QMessageBox>

#define ERROR_CODE 1

inline void panicIf(bool condition, QString message)
{
    if(condition)
    {
        QMessageBox::critical(nullptr,"Error",message);
        exit(ERROR_CODE);
    }

}

inline bool warnIf(bool condition, QString message)
{
    if(condition)
    {
        QMessageBox::critical(nullptr,"Error",message);
    }
   return condition;
}

#endif // UTILITY_H
