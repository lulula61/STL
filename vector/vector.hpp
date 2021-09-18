//vector

#ifndef VECTOR_HPP_INCLUDED
#define VECTOR_HPP_INCLUDED

#include <climits>
#include <cstddef>
#include "exceptions.hpp"

namespace sjtu {

template<typename T>
class vector {
public:
     T *data;    
     size_t sizeofdata; //存储数据大小
     size_t max_size;  //总共空间大小

     void doublespace()
     {
         T *tmp = data;
         max_size *= 2;
         data = new T[max_size];
         for(size_t i=0; i<sizeofdata; i++)
            data[i] = tmp[i];
        delete [] tmp;
     }



    class const_iterator;
    class iterator {
        friend class vector<T>;
    private:
          T *pos;
    public:
          iterator(T *obj = NULL){  pos = obj; }

        iterator operator+(const int &n)  const{
           return iterator(pos+n);
        }
        iterator operator-(const int &n)  const{
            return iterator(pos-n);
        }

        int operator-(const iterator &rhs) const {                //return distance
            if(this != rhs ) throw invalid_iterator();
            int dis = pos - rhs.pos;
            return dis;

        }
        iterator& operator+=(const int &n) {
            pos = pos + n;
            return *this;

        }
        iterator& operator-=(const int &n) {
            pos = pos - n;
            return *this;
        }
        iterator operator++(int)
        {
            iterator tmp;
            tmp = *this;
            pos += 1;
            return tmp;
        }

        iterator& operator++()
        {
            pos++; return *this;
        }

        iterator operator--(int)
        {
            iterator tmp = *this;
            pos--; return tmp;
        }

        iterator& operator--()
        {
            pos--;return *this;
        }

        T& operator*() const
        {
            return *pos;
        }

        bool operator==(const iterator &rhs) const {return pos == rhs.pos;}
        bool operator==(const const_iterator &rhs) const { return pos == rhs.pos;}
        bool operator!=(const iterator &rhs) const { return pos != rhs.pos;}
        bool operator!=(const const_iterator &rhs) const {return pos != rhs.pos;}
    };

class const_iterator {

        friend class vector<T>;
        private:
        T *pos;
        public:

        const_iterator(T *obj = NULL) {  pos = obj;}


        const_iterator operator+(const int &n) const {
            return iterator(pos+n);
        }
        const_iterator operator-(const int &n) const {
            return const_iterator(pos-n);
        }

        int operator-(const const_iterator &rhs) const {
            if(*this != rhs ) throw invalid_iterator();
            int dis = pos - rhs.pos;
            return dis;

        }


        const_iterator& operator+=(const int &n) const{
            return const_iterator(pos+n);

        }
        const_iterator& operator-=(const int &n) const {
            return const_iterator(pos-n);
        }
        const_iterator operator++(int) const
        {
            const_iterator tmp;
            tmp = *this;
            pos += 1;
            return tmp;
        }

        const_iterator& operator++() const
        {
            pos++; return *this;
        }

       const_iterator operator--(int)  const
        {
           const_iterator tmp = *this;
            pos--; return tmp;
        }

       const_iterator& operator--() const
        {
            pos--;return *this;
        }

        T& operator*() const
        {
            return *pos;
        }

        bool operator==(const iterator &rhs) const {return pos == rhs.pos;}
        bool operator==(const const_iterator &rhs) const { return pos == rhs.pos;}
        bool operator!=(const iterator &rhs) const { return pos != rhs.pos;}
        bool operator!=(const const_iterator &rhs) const {return pos != rhs.pos;}

    };

    vector( )         // default constructor
    {
        data = new T[10];
        sizeofdata = 0;
        max_size = 10;
    }


    vector(const vector &other)               //copy constructor
    {
        sizeofdata = other.size();
        max_size = other.max_size;
        data = new T[sizeofdata];
        for(size_t i=0; i<sizeofdata; i++)
            data[i] = other.data[i];
    }

    ~vector() {  delete [] data;}                              //destructor

     vector &operator=(const vector &other) {            //assignment operator

        if(this == other) return *this;

        max_size = other.max_size;
        sizeofdata = other.size();

        for(size_t i=0;i<sizeofdata; i++)
            data[i] = other.data[i];

        return *this;

    }

    T & at(const size_t &pos)
    {
        if(pos > sizeofdata) throw index_out_of_bound();
        return data[pos];
    }

    const T & at(const size_t &pos) const
    {
        if(pos > sizeofdata) throw index_out_of_bound();
        return data[pos];
    }

    T & operator[](const size_t &ind)
    {
            if( ind<0 || ind >= sizeofdata) throw index_out_of_bound() ;
            else return data[ind];
    }

    const T & operator[](const size_t &ind) const
     {
         if( ind < 0 || ind >= sizeofdata) throw index_out_of_bound() ;
            else return data[ind];    }

    const T & front() const
    {
        if(empty()) throw container_is_empty();
        else return data[0];
    }

    const T & back() const
    {
        if(empty()) throw container_is_empty();
        else return data[sizeofdata-1];
    }

    iterator begin() { return iterator(data);}                                  
    const_iterator cbegin() const {return const_iterator(data);}                     

    iterator end() {return iterator(data+sizeofdata);}
    const_iterator cend() const {return const_iterator(data+sizeofdata);}

    bool empty() const
    {
        return max_size == 0;
    }
    /**
     * returns the number of elements
     */
    size_t size() const
   {
       return sizeofdata;
   }
    /**
     * clears the contents
     */
    void clear()
    {
         if(max_size == 0) {}
        else  delete [] data;
    }
    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value)
    {
        T *q;

        if(sizeofdata == max_size)
        {
            int dis = pos.pos - data;
            doublespace();
            pos.pos = data+dis;
        }

        q = data + sizeofdata;

        while(q > pos.pos) {*q = *(q-1); --q;}

        *pos.pos = value;
        ++sizeofdata;
        return pos;
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value)
     {
         if(ind > max_size)  throw index_out_of_bound();
         if(ind == max_size) doublespace();
         for(int i=sizeofdata; i>ind; i--)
            data[i] = data[i-1];
         data[ind] = value;
         ++sizeofdata;
         return iterator(data+ind);
     }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator p)
    {
        T *q = p.pos;
        --sizeofdata;
        while(q != data+sizeofdata ) { *q = *(q+1);++q;}
        if( q == data+sizeofdata-1) return end();
        else return iterator(p.pos+1);
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind)
    {
        if(ind >= sizeofdata) throw index_out_of_bound();
        for(int i=ind;i<sizeofdata-1;i++)
            data[i] = data[i+1];
        sizeofdata--;
        return iterator(data+ind);
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T &value)
    {
        if(sizeofdata == max_size) doublespace();
        data[sizeofdata] = value;
        sizeofdata++;

    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back()
    {
        if(empty()) throw container_is_empty();
        sizeofdata--;

    }
};


}

#endif // VECTOR_HPP_INCLUDED
