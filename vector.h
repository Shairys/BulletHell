#pragma once

const int STARTING_SIZE_OF_VECTOR = 16;

template <typename T>
class vector { //Jeœli T nie ma zdefiniowanego copy constructora lub operatora = to vector mo¿e nie dzia³aæ prawid³owo

protected:
    int actualSize;
    int arrSize;
    T* data;
    bool initialized;

    void resize(const int newSize) {
        T* tmp = new T[newSize];
        for (int i = 0; i < min(actualSize, newSize); i++)
            tmp[i] = data[i];
        if(initialized)
            delete[] data;
        if (actualSize > newSize)
            actualSize = newSize; 
        arrSize = newSize;
        data = tmp;
        initialized = true;
    }

    void init() {
        actualSize = 0;
        data = new T[0];
        arrSize = 0;
        initialized = true;
    }

public:
    void push_back(const T& element) {
        if (arrSize == 0 || !initialized)
            resize(STARTING_SIZE_OF_VECTOR);
        else if (arrSize == actualSize)
            resize(actualSize * 2);
        data[actualSize] = element;
        actualSize++;
    }

    void swap(const int idx1, const int idx2) {
        T tmp = data[idx1];
        data[idx1] = data[idx2];
        data[idx2] = tmp;
    }

    void pop_back() {
        actualSize = actualSize - 1;
    }


    T& getElement(const int idx) const {
        return data[idx];
    }

    T& operator [] (const int idx) const {
        return getElement(idx);
    }

    void clear() {
        actualSize = 0;
    }

    void setSize(const int newSize) {
        this->clear();
        while (actualSize < newSize)
            this->push_back(T());
    }

    vector() {
        init();
    }

    int size() const {
        return actualSize;
    }

    ~vector() {
        if (!initialized)
            return;
        delete[] data;
        initialized = false;
    }

    vector& operator = (const vector& assignedValue) {
        this->clear();
        for (int i = 0; i < assignedValue.size(); i++) {
            this->push_back(assignedValue[i]);
        }
        return (*this);
    }

};