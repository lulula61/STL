#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cmath>

namespace sjtu {

    template<class T>
    class deque {
    public:
        inline int max(int a, int b){
            return (a > b)? a : b;
        }

    private:

        class node{
            friend class iterator;
            friend class const_iterator;
        public:
            T * data;
            size_t size;
            node * next, * prev;
            int NodeSize;
            node(){
                NodeSize = 512;
                data = (T*)operator new(sizeof(T) * NodeSize);
                size = 0;
            }
            node(int a){
                NodeSize = a;
                data = (T*)operator new(sizeof(T) *  NodeSize);
                size = 0;
            }

            node(const node * other){
                 NodeSize = other-> NodeSize;
                size = other->size;
                data = (T*)operator new(sizeof(T) *  NodeSize);
                for (int i = 0; i < size; i++)
                {
                    new(&data[i])T(other->data[i]);
                }
            }
            void DoubleSpace(){
                T * newdata = (T*)operator new(sizeof(T) * 2 * NodeSize);
                for (int i = 0; i < size; i++)
                {
                    new(newdata+i)T(data[i]);
                }
                NodeSize *= 2;
                for (int i = 0; i < size; i++) data[i].~T();
                delete data; data = newdata;
            }

            ~node(){
                for (int i = 0; i < size; i++) {
                    data[i].~T();
                }
                operator delete(data);
            }

        };
        node * Head;
        node * Tail;
        int CurrentLen;
    public:
        class const_iterator;
        class iterator {
            friend deque;
            friend class const_iterator;
            friend iterator deque::insert(iterator pos, const T &value);
            friend iterator deque::erase(iterator pos);
        private:
            node * nd;
            deque * deq;
            int pos;

            iterator More(const int &n){                                            //向后移动
                node* p = nd; int cnt = 0; int delta;
                if (n > deq->CurrentLen) throw(runtime_error());
                if (n == 0) return *this;
                while(cnt < n){
                    if (p == nd){ delta = (p->size - pos);}
                    else delta = p->size;

                    if (cnt + delta <= n){
                        if (p == deq->Tail) {
                            if (cnt + delta < n) throw(runtime_error());
                            else return iterator(p,deq, p->size);
                        }
                        p = p->next; cnt += delta;
                        if (cnt == n)
                        {
                            cnt = 0; break;
                        }
                    }
                    else
                    {
                        if (p == nd) cnt = pos+(n-cnt);
                        else cnt = n - cnt;
                        break;
                    }
                }
                if (cnt >= p->size) throw(runtime_error());
                return iterator(p,deq, cnt);
            }
            iterator Less(const int &n){                               //向前移动
                node * p = nd; int cnt = n;
                if (n == 0) return *this;
                while(cnt > 0){
                    int delta;
                    if (p == nd) {delta = pos;}
                    else delta = p->size;
                    if (cnt - delta > 0){
                        p = p->prev; cnt -= delta;
                    }
                    else {
                        cnt = delta - cnt; break;
                    }
                }
                return iterator(p, deq, cnt);
            }


        public:
            iterator(){}
            iterator(node * ori, deque * d, int po): nd(ori),deq(d), pos(po) {}
            iterator(const iterator &other){nd = other.nd; pos = other.pos; deq = other.deq;}
            /**
             * return a new iterator which pointer n-next elements
             *   even if there are not enough elements, the behaviour is **undefined**.
             * as well as operator-
             */
            int FindIndex(){
                node * p = deq->Head; int index = 0;
                while(p != nd){
                    index += p->size;
                    p = p->next;
                }
                index += pos;
                return index;
            }
            iterator operator+(const int &n)  {
                if (n >= 0) return More(n);
                else {return Less(-n); }
            }
            iterator operator-(const int &n) {
                if (n <= 0) return More(-n);
                else {return Less(n);}
            }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(iterator rhs){
                if (deq != rhs.deq) throw(invalid_iterator());
                int a = FindIndex();
                int b = rhs.FindIndex();
                return a - b;
            }
            iterator operator+=(const int &n) {
                *this = *this + n;
                return *this;
            }
            iterator operator-=(const int &n) {
                *this = *this - n;
                return *this;
            }
            /**
             * TODO iter++
             */
            iterator operator++(int) {
                if (nd == deq->Tail && size_t(pos) == deq->Tail->size-1){
                    pos++;
                    return iterator(deq->Tail, deq, deq->Tail->size-1);
                }
                iterator ans(*this);
                if (pos <= nd->size-2) pos++;
                else {nd = nd->next; pos = 0;}
                return ans;
            }
            /**
             * TODO ++iter
             */
            iterator& operator++() {
                if (nd == deq->Tail && pos == deq->Tail->size) throw(runtime_error());
                if (nd == deq->Tail && pos == deq->Tail->size-1){
                    pos++; return *this;
                }
                *this = More(1);
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int) {
                if (nd == deq->Head && pos == 0) throw(runtime_error());
                iterator ans(*this);
                if (pos > 0) pos--;
                else {nd = nd->prev; pos = nd->size-1;}
                return ans;
            }
            /**
             * TODO --iter
             */
            iterator& operator--() {
                if (nd == deq->Head && pos == 0) throw(runtime_error());
                if (pos > 0) pos--;
                else {nd = nd->prev; pos = nd->size-1;}
                return *this;
            }
            /**
             * TODO *it
             */
            T& operator*() const {
                if (nd == deq->Tail && pos == deq->Tail->size) throw(runtime_error());
                return nd->data[pos];
            }
            /**
             * TODO it->field
             */
            T* operator->() const noexcept {
                T * ans = &nd->data[pos];
                return ans;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                return ((nd == rhs.nd) && (pos == rhs.pos) );
            }
            bool operator==(const const_iterator &rhs) const {
                return ((nd == rhs.nd) && (pos == rhs.pos) );
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !((nd == rhs.nd) && (pos == rhs.pos) );
            }
            bool operator!=(const const_iterator &rhs) const {
                return !((nd == rhs.nd) && (pos == rhs.pos) );
            }
        };
        class const_iterator {
        private:
            node * nd;
            int pos;
            deque * deq;
            const_iterator More(const int &n){
                node * p = nd; int count = 0; int delta;
                if (n > deq->CurrentLen) throw(runtime_error());
                if (n == 0) return *this;
                while(count < n){
                    if (p == nd){ delta = (p->size - pos);}
                    else delta = p->size;

                    if (count + delta <= n){
                        if (p == deq->Tail) {
                            if (count + delta < n) throw(runtime_error());
                            else return iterator(p,deq, p->size);
                        }
                        p = p->next; count += delta;
                        if (count == n){
                            count = 0; break;
                        }
                    }
                    else {
                        if (p == nd) count = pos+(n-count);
                        else count = n - count;
                        break;
                    }
                }
                if (count >= p->size) throw(runtime_error());
                return const_iterator(p,deq, count);
            }
            const_iterator Less(const int &n){
                node * p = nd; int count = n;
                if (n == 0) return *this;
                while(count > 0){
                    int delta;
                    if (p == nd) {delta = pos;}
                    else delta = p->size;
                    if (count - delta > 0){
                        p = p->prev; count -= delta;
                    }
                    else {
                        count = delta - count; break;
                    }
                }
                return const_iterator(p, deq, count);
            }
            int FindIndex(){
                node * p = deq->Head; int index = 0;
                while(p != nd){
                    index += p->size;
                    p = p->next;
                }
                index += pos;
                return index;
            }
        public:
            const_iterator() {
                // TODO
            }
            const_iterator(node * ori, deque * d, int p):nd(ori), deq(d), pos(p){}
            const_iterator(const const_iterator &other) {
                nd = other.nd; deq = other.deq; pos = other.pos;
            }
            const_iterator(const iterator &other) {
                nd = other.nd; deq = other.deq; pos = other.pos;
            }
            const_iterator operator+(const int &n) const {
                if (n >= 0) return const_cast<const_iterator *>(this)->More(n);
                else {return const_cast<const_iterator *>(this)->Less(-n);}
            }
            const_iterator operator-(const int &n) const {
                if (n <= 0) return const_cast<const_iterator *>(this)->More(-n);
                else {return const_cast<const_iterator *>(this)->Less(n);}
            }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const{
                if (deq != rhs.deq) throw(invalid_iterator());
                int a = const_cast<const_iterator *>(this)->FindIndex();
                int b = const_cast<const_iterator *>(&rhs)->FindIndex();
                return (a -  b);
            }
            const_iterator operator+=(const int &n) {
                *this = *this + n;
                return *this;
            }
            const_iterator operator-=(const int &n) {
                *this = *this - n;
                return *this;
            }
            const_iterator operator++(int) {
                if (nd == deq->Tail && pos == deq->Tail->size){
                    pos++;
                    return const_iterator(deq->Tail, deq, deq->Tail->size-1);
                }
                const_iterator ans(*this); *this = More(1);
                return ans;
            }
            const_iterator& operator++() {
                if (nd == deq->Tail && pos == deq->Tail->size){
                    throw(runtime_error());
                }
                if (nd == deq->Tail && pos == deq->Tail->size-1) {
                    *this = const_iterator(deq->Tail, deq->Tail, deq->Tail->size);
                    return *this;
                }
                *this = More(1);
                return *this;
            }
            const_iterator operator--(int) {
                const_iterator ans(*this); *this = Less(1);
                return ans;
            }
            const_iterator& operator--() {
                if (nd == deq->Head && pos == 0){throw(runtime_error());}
                *this = Less(1);
                return *this;
            }
            T& operator*() const {
                if (nd == deq->Tail && pos == deq->Tail->size) throw (runtime_error());
                return nd->data[pos];
            }
            T* operator->() const noexcept {
                T * ans = &nd->data[pos];
                return ans;
            }
            bool operator==(const iterator &rhs) const {
                return ((nd == rhs.nd) && (pos == rhs.pos) );
            }
            bool operator==(const const_iterator &rhs) const {
                return ((nd == rhs.nd) && (pos == rhs.pos) );
            }
            bool operator!=(const iterator &rhs) const {
                return !((nd == rhs.nd) && (pos == rhs.pos) );
            }
            bool operator!=(const const_iterator &rhs) const {
                return !((nd == rhs.nd) && (pos == rhs.pos) );
            }
        };
        /**
         * TODO Constructors
         */
        deque() {
            CurrentLen = 0;
            Head = Tail = new node();
            Head->next = Tail; Tail->prev = Head;
        }
        deque(const deque &other) {
            CurrentLen = other.CurrentLen;
            Head = new node(other.Head);
            if (other.Head == other.Tail) {Tail = Head; Tail->prev = Head; Head->next = Tail;}
            else {
                node * op = other.Head; node * nextp;
                node * p = Head;
                while (op != other.Tail){
                    nextp = new node(op->next);
                    nextp->prev = p; p->next = nextp;
                    op = op->next; p = p->next;
                }
                Tail = p;
            }
        }
        ~deque() {
            clear();
            if (Head != nullptr) delete Head;
        }
        /**
         * TODO assignment operator
         */
        deque &operator=(const deque &other) {

            if (&other == this) return *this;
            clear(); if (Head != nullptr) delete Head;
            Head = new node(other.Head); CurrentLen = other.CurrentLen;
            if (other.Head == other.Tail) {Tail = Head; Tail->prev = Head; Head->next = Tail;}
            else {
                node * op = other.Head;
                node * p = Head;
                while (op != other.Tail){
                    node * nextp = new node(op->next);
                    nextp->prev = p; p->next = nextp;
                    op = op->next; p = p->next;
                }
                Tail = p;
            }
            return *this;
        }
        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
        T & at(const size_t &pos) {
            if (pos >= CurrentLen || pos < 0) throw(index_out_of_bound());
            iterator cur = Findpos(pos);
            return *cur;
        }
        const T & at(const size_t &pos) const {
            if (pos >= CurrentLen || pos < 0) throw(index_out_of_bound());
            iterator cur = const_cast<deque *>(this)->Findpos(pos);
            return *cur;
        }
        T & operator[](const size_t &pos) {
            return at(pos);
        }
        const T & operator[](const size_t &pos) const {
            return at(pos);
        }
        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T & front() const {
            if (CurrentLen == 0) throw(container_is_empty());
            return Head->data[0];
        }
        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T & back() const {
            if (CurrentLen == 0) throw(container_is_empty());
            if (Tail->size == 0) return Tail->prev->data[Head->size - 1];
            else return Tail->data[Tail->size - 1];
        }
        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            return iterator(Head,this, 0);
        }
        const_iterator cbegin() const {
            return const_iterator(Head,const_cast<deque *>(this), 0);
        }
        /**
         * returns an iterator to the end.
         */
        iterator end() {
            return iterator(Tail, this,Tail->size);
        }
        const_iterator cend() const {
            return const_iterator(Tail, const_cast<deque *>(this), Tail->size);
        }
        /**
         * checks whether the container is empty.
         */
        bool empty() const {
            return (CurrentLen == 0);
        }
        /**
         * returns the number of elements
         */
        size_t size() const {
            return size_t(CurrentLen);
        }
        /**
         * clears the contents
         */
        void clear() {
            node * p = Head;
            node *pNode;
            if (Head == Tail) {
                delete Head; Head = Tail = new node();
                Head->next = Tail; Tail->prev = Head;
                CurrentLen = 0; return;
            }
            while(p != Tail){
                pNode = p->next; delete p;
                p = pNode;
            }
            delete Tail;
            Head = Tail = new node();
            Head->next = Tail; Tail->prev = Head;
            CurrentLen = 0;
        }
        /**
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {
            if (pos.deq != this) throw(runtime_error());
            node * p = pos.nd; int index = pos.pos;
            if (pos == end()){
                push_back(value); return iterator(Tail, this, Tail->size-1);
            }
            if (index > p->size || index < 0) throw (invalid_iterator());

            if (p->size < p->NodeSize) {
                for (int i = p->size; i > index; i--) {
                    new(p->data + i)T(p->data[i - 1]);
                    p->data[i - 1].~T();
                }
                new(p->data + index)T(value); CurrentLen++; p->size++;
                //return iterator(p, this, p->size-1);
                return iterator(p, this, index);
            }
            else{
                if (p->NodeSize * 2 <= floor(sqrt(CurrentLen)) ){
                    p->DoubleSpace();
                    for (int i = p->size; i > index; i--) {
                        new(p->data + i)T(p->data[i - 1]);
                        p->data[i - 1].~T();
                    }
                    new(p->data + index)T(value); CurrentLen++; p->size++;
                    return iterator(p, this, index);
                }
                split(pos); new(p->data+index)T(value);
                p->size++; CurrentLen++;
                return iterator(p, this, index);
            }

        }
        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {
            //TODO ~T() the value
            if (CurrentLen == 0) throw(container_is_empty());
            if (pos == end()-1) {
                pop_back(); return end();
            }

            node * p = pos.nd; int index = pos.pos;
            p->data[index].~T();

            for (int i = index+1; i < p->size; i++)
            {
                new(p->data+i-1)T(p->data[i]);
                p->data[i].~T();
            }
            p->size--; CurrentLen--;
            if (index == p->size) return iterator(p->next, this, 0);
            if (p != Tail) merge(p);
            return iterator(p, this, index);
        }
        /**
         * adds an element to the end
         */
        void push_back(const T &value) {
            if (Tail->size < Tail->NodeSize)
            {
                new(&Tail->data[Tail->size])T(value);
                Tail->size++; CurrentLen++;
            }
            else {
                if (Tail->size * 2 < floor(sqrt(CurrentLen)))
                {
                    Tail->DoubleSpace();
                    new(Tail->data+Tail->size)T(value);
                    Tail->size++; CurrentLen++; return;
                }
                node * p = new node();
                p->size = 1;
                new(&p->data[0])T(value);
                Tail->next = p; p->prev = Tail;
                Tail = p; CurrentLen++;
            }
        }
        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back() {
            if (Tail->size != 1) {
                Tail->data[Tail->size-1].~T();
                Tail->size--;
                if (Tail != Head) merge(Tail->prev);
            }
            else {
                if (CurrentLen == 1){ clear(); return;}
                node * p = Tail->prev;
                delete Tail;
                Tail = p;
                if (Tail != Head) merge(Tail->prev);
            }
            CurrentLen--;
        }
        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value) {
            if (Head->size < Head->NodeSize){
                for (int i = Head->size; i >= 1; i--){
                    new(Head->data + i)T(Head->data[i-1]);
                    Head->data[i-1].~T();
                }
                Head->size++; CurrentLen++;
                new(Head->data)T(value); return;
            }
            else {
                if (Head->NodeSize * 2 < floor(sqrt(CurrentLen))){
                    Head->DoubleSpace();
                    for (int i = Head->size; i >= 1; i--){
                        new(Head->data + i)T(Head->data[i-1]);
                        Head->data[i-1].~T();
                    }
                    new(Head->data)T(value);
                    Head->size++; CurrentLen++;
                    return;
                }
                node *p = new node();
                p->size = 1;
                new(p->data)T(value);
                p->next = Head;
                Head->prev = p;
                Head = p;
                CurrentLen++;
            }
        }
        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front() {
            if (CurrentLen == 0) throw(container_is_empty());
            if (Head->size == 0 && Head != Tail) {
                node * p = Head; Head = p->next; delete p;
            }
            Head->data[0].~T();
            for (int i = 1; i < Head->size; i++){
                new(Head->data+i-1)T(Head->data[i]);
                Head->data[i].~T();
            }
            Head->size--; CurrentLen--;
            merge(Head);
        }

    private:
        iterator Findpos(int index){
            node * p = Head; int pos = 0, count = 0;
            if (index == 0) return iterator(p, this, pos);
            while(count < index){
                if (count + p->size - 1 < index){
                    count += p->size; p = p->next;
                }
                else{
                    pos = index - count;
                    break;
                }
            }
            return iterator(p,this, pos);
        }
        void merge(node * p){
            if (Head == Tail) return;
            node * nextp = p->next; node * nnp = nextp->next;
            if (p == Tail) return;

            if(nextp == Tail){
                if (p->size + nextp->size <= p->NodeSize){
                    for (int i = 0; i < nextp->size; i++){
                        new(p->data+i+(p->size) )T(nextp->data[i]);
                    }
                    p->size += nextp->size;
                    Tail = p;
                    delete nextp;
                }
                return;
            }
            if (p->size + nextp->size <= p->NodeSize){
                for (int i = 0; i < nextp->size; i++){
                    new(p->data+i+(p->size) )T(nextp->data[i]);
                }
                p->size += nextp->size;
                p->next = nnp; nnp->prev = p;
                delete nextp;
            }
        }
        void split(iterator t){
            node * p = t.nd;
            int pos = t.pos;
            if (p == Tail){
                node * NewNode = new node( max(floor(sqrt(CurrentLen)), p->NodeSize ) );
                NewNode->size = (p->size - pos);
                for (int i = 0; i < p->size-pos; i++){
                    new(NewNode->data+i)T(p->data[i+pos]);
                    p->data[i+pos].~T();
                }
                p->size = pos;
                p->next = NewNode; NewNode->prev = p;
                Tail = NewNode;
                return;
            }

            node * nextp = p->next;
            node * NewBlock = new node( p->NodeSize );
            for (int i = 0; i < p->size-pos; i++){
                new(NewBlock->data+i)T(p->data[i+pos]);
                p->data[i+pos].~T();
            }
            NewBlock->size = p->size - pos;
            p->size = pos;
            nextp->prev = NewBlock; NewBlock->next = nextp;
            p->next = NewBlock; NewBlock->prev = p;
        }

    };

}

#endif
