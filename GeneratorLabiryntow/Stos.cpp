//
// Created by piotrek on 1/3/23.
//

#include "Stos.h"

Stos::Stos() {
    this->Top = -1;
}

void Stos::Push(elementtype x) {

    if(this->Top == (maxlegth-1))
        return;

    S[++this->Top] = x;
}

void Stos::Pop() {

    if(this->Top == -1)
        return;

    this->Top--;
}

bool Stos::Empty() const{
    return this->Top == -1;
}

elementtype Stos::TopElem() {
    //Jeśli nie ma elementu to zwróć element niemożliwy
    if(this->Top == -1)
        return INT_MIN;

    return S[this->Top];
}

void Stos::Makenul() {
    this->Top = -1;
}
