//
// Created by piotrek on 1/3/23.
//

#ifndef UNTITLED_STOS_H
#define UNTITLED_STOS_H

#include <climits>

const int maxlegth = 2048;
typedef int elementtype;
typedef int pos;

class Stos {
    int S[maxlegth];
    pos Top;//szczyt stosu
public:
    Stos();

    void Push(elementtype x);

    void Pop();

    bool Empty() const;

    elementtype TopElem();

    void Makenul();
};


#endif //UNTITLED_STOS_H
