#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {
const int _size=1000;
template<class T>
class deque {
public:
    struct block
    {
        friend class sjtu::deque<T>;
        friend class sjtu::deque<T>::iterator;
        friend class sjtu::deque<T>::const_iterator;
        int blocksize;
        T** data;
        block* next;
        block* prev;
        int num;
        block(int s=_size,block*n=NULL,block*p=NULL) {data=new T*[s];blocksize=s;next=n;prev=p;num=0;}
        block(const block &other):blocksize(other.blocksize),num(other.num),next(NULL),prev(NULL),data(NULL)
        {
            data=new T *[blocksize];
            for (int i=0;i<other.num;++i)
                data[i]=new T(*other.data[i]);
        }
        block &operator=(const block &rhs)
        {

            if (&rhs != this)
            {
                clear();
                blocksize = rhs.blocksize;
                num = rhs.num;
                data = new T *[blocksize];
                for (int i = 0; i < rhs.num; i++)
                {
                    data[i] = new T(*rhs.data[i]);
                }
            }
            return *this;
        }
        T &at(int n)
        {
            if (n<0||n>=num)
                throw index_out_of_bound();
            return *(data[n]);
        }
        T &operator[](int n)
        {
            if (n<0||n>=num)
                throw index_out_of_bound();
            return *(data[n]);
        }
        void erase(int n)
        {
            delete data[n];
            for (int i=n;i<num-1;++i)
                data[i]=data[i+1];
            --num;

        }
        void clear()
        {
            for (int i = 0; i < num; i++)
            {
                delete data[i];
            }
            delete[] data;
            num = 0;
        }
        void insert(int n,const T &value)
        {
            for (int i=num;i>n;--i)
                data[i]=data[i-1];
            data[n]=new T(value);
            ++num;

        }
        ~block(){clear();}

    };



public:
	class const_iterator;
	class iterator {
        friend class sjtu::deque<T>;

        friend class sjtu::deque<T>::const_iterator;

        friend class sjtu::deque<T>::block;
	private:
		block *_block;
		int situ;
		const deque<T> *deq;


	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
        iterator(const deque *d = NULL,block *b= NULL,int s=0):deq(d),_block(b),situ(s){}
        iterator(const iterator &rhs):deq(rhs.deq),_block(rhs._block),situ(rhs.situ){}
        iterator operator+(const int &n) const
        {
            iterator tmp(*this);
            tmp+=n;
            return tmp;
        }
		iterator operator-(const int &n) const {
			iterator tmp(*this);
			tmp-=n;
			return tmp;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const
        {
            if (deq!=rhs.deq) throw invalid_iterator();
			else if(_block==rhs._block)
                return situ-rhs.situ;
			else
			{
			    int dist= rhs._block->num - rhs.situ;
                block *b = rhs._block->next;
                while (b!=NULL&&b!=_block)
                {
                    dist += b->num;
                    b = b->next;
                }
                if (b != NULL)
                    return dist + situ;
                else
                {
                    b = _block->next;
                   dist = _block->num - situ;
                    while (b!=NULL&&b!=rhs._block)
                    {
                        dist+=b->num;
                        b=b->next;
                    }
                    if (b!=NULL)
                        return -(dist+rhs.situ);
                    else
                        throw runtime_error();
                }
			}
		}
		iterator operator+=(const int &n)
		{
           if (n < 0)
            {
                int dist=-n;
                while (_block->prev!=NULL&&situ<dist)
                {
                    dist -=situ + 1;
                    _block = _block->prev;
                    situ = _block->num - 1;
                }
                if (situ<dist&&_block->prev==NULL)
                    throw index_out_of_bound();
                situ -= dist;
            }
            else if (n > 0)
            {
                int dist = n;
                while (_block->next!=deq->rear&&(situ>=_block->num-dist))
                {
                    dist-=_block->num-situ;
                    situ = 0;
                    _block=_block->next;
                }
                if (_block->next==NULL)
                    throw index_out_of_bound();
                situ+=dist;
            }
           return *this;
		}
		iterator operator-=(const int &n)
		{
			return (*this+=(-n));
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int)
		{
            iterator tmp(*this);
            ++(*this);
            return tmp;
		}

		iterator& operator++()
		{
           if (situ == _block->num - 1)
            {
                situ=0;
                if (_block->next == NULL)
                {
                    throw index_out_of_bound();
                }
                else
                {

                    _block = _block->next;
                }
            }
            else
                ++situ;
            return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int)
		{
            iterator tmp(*this);
            --(*this);
            return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--()
		{

            if (situ==0)
            {
                if (_block->prev==NULL) throw index_out_of_bound();
                else
                {

                    _block=_block->prev;
                    situ=_block->num-1;
                }
            }
            else --situ;
            return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const
		{
            return _block->at(situ);
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept
		{
            return _block->data[situ];
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const
		{
            return (_block==rhs._block&&situ==rhs.situ)||(_block->next==NULL&&rhs._block->next==_block&&rhs.situ==rhs._block->num)||(rhs._block->next==NULL&&_block->next==rhs._block&&situ==_block->num);
		}
		bool operator==(const const_iterator &rhs) const
		{
            return (_block==rhs._block&&situ=rhs.situ)||(_block->next==NULL&&rhs._block->next==_block&&rhs.situ==rhs._block->num)||(rhs._block->next==NULL&&_block->next==rhs._block&&situ==_block->num);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const
		{
            return !(*this==rhs);
		}
		bool operator!=(const const_iterator &rhs) const
		{
            return !(*this==rhs);
		}

	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
        friend class sjtu::deque<T>;

        friend class sjtu::deque<T>::iterator;

        friend struct sjtu::deque<T>::block;
		private:
			const sjtu::deque<T> *deq;
			block *_block;
			int situ;
		public:
			const_iterator(const deque *d=NULL,block *b=NULL,int s=0):deq(d),_block(b),situ(s){}
			const_iterator(const const_iterator &other):deq(other.deq),_block(other._block),situ(other.situ){}
			const_iterator(const iterator &other):deq(other.deq),_block(other._block),situ(other.situ) {}
			const_iterator operator+(const int &n) const {
			const_iterator tmp=*this;
			return tmp+=n;
		}
		const_iterator operator-(const int &n) const {
			const_iterator tmp=*this;
			return tmp-=n;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const const_iterator &rhs) const
        {
			if (deq!=rhs.deq) throw invalid_iterator();
			else if(_block==rhs._block)
                return situ-rhs.situ;
			else
			{
			    int dist= rhs._block->num - rhs.situ;
                block *p = rhs._block->next;
                while (p != NULL && p != _block)
                {
                    dist += p->num;
                    p = p->next;
                }
                if (p != NULL)
                    return dist + situ;
                else
                {

                    p = _block->next;

                   dist = _block->num - situ;

                    while (p != NULL && p != rhs._block)
                    {
                        dist += p->num;
                        p = p->next;
                    }
                    if (p != NULL)
                        return -(dist + rhs.situ);
                    else
                        throw runtime_error();
                }
			}
		}
		const_iterator operator+=(const int &n)
		{
            if (n < 0)
            {
                int dist=-n;
                while (_block->prev!=NULL&&situ<dist)
                {
                    dist -=situ + 1;
                    _block = _block->prev;
                    situ = _block->num - 1;
                }
                if (situ<dist&&_block->prev==NULL)
                    throw index_out_of_bound();
                situ -= dist;
            }
            else if (n > 0)
            {
                int dist = n;
                while (_block->next!=deq->rear&&(situ>=_block->num-dist))
                {
                    dist-=_block->num-situ;
                    situ = 0;
                    _block=_block->next;
                }
                if (_block->next==NULL)
                    throw index_out_of_bound();
                situ+=dist;
            }
           return *this;
            return *this;
		}
		const_iterator operator-=(const int &n) {
			return *this+=(-n);
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int)
		{
            const_iterator tmp=*this;
            ++*this;
            return tmp;
		}

		const_iterator& operator++()
		{
            if (situ == _block->num - 1)
            {

                if (_block->next == NULL)
                {
                    throw index_out_of_bound();
                }
                else
                {
                    situ=0;
                    _block = _block->next;
                }
            }
            else
            {
                ++situ;
            }
            return *this;
		}
		/**
		 * TODO iter--
		 */
		const_iterator operator--(int)
		{
            const_iterator tmp=*this;
            --*this;
            return tmp;
		}
		/**
		 * TODO --iter
		 */
		const_iterator& operator--()
		{
            if (situ==0)
            {
                if (_block->prev==NULL) throw index_out_of_bound();
                else
                {

                    _block=_block->prev;
                    situ=_block->num-1;
                }
            }
            else --situ;

            return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const
		{
            return _block->at(situ);
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept
		{
            return _block->data[situ];
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const
		{
            return (_block==rhs._block&&situ==rhs.situ)||(_block->next==NULL&&rhs._block->next==_block&&rhs.situ==rhs._block->num)||(rhs._block->next==NULL&&_block->next==rhs._block&&situ==_block->num);
		}
		bool operator==(const const_iterator &rhs) const
		{
            return (_block==rhs._block&&situ==rhs.situ)||(_block->next==NULL&&rhs._block->next==_block&&rhs.situ==rhs._block->num)||(rhs._block->next==NULL&&_block->next==rhs._block&&situ==_block->num);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const
		{
            return !(*this==rhs);
		}
		bool operator!=(const const_iterator &rhs) const
		{
            return !(*this==rhs);
		}
	};
	/**
	 * TODO Constructors
	 */
public:
    int blocksize;
    block *head;
    block *rear;
    int num;

    void change(block *&_block)
    {
        if (_block==rear) return;
        if (_block->num<blocksize/2) merge(_block);
        if (_block->num>=blocksize) split(_block);
    }
    void split (block *&_block)
    {
        block *b=new block(_block->blocksize);
        b->prev=_block;
        b->next=_block->next;
        _block->next->prev=b;
        _block->next=b;
        int tmp=_block->num/2;
        for (int i=tmp;i<_block->num;++i)
            b->data[i-tmp]=_block->data[i];
        b->num=_block->num-tmp;
        _block->num=tmp;
    }
    void merge(block *&_block)
    {
        while (_block->next->next!=NULL&&_block->num+_block->next->num<=blocksize)
        {
            while (_block->next->next!=NULL&&_block->num==0)
            {
                block *b=_block;
                _block=_block->next;
                delete b;
            }
            block *b=_block->next;
            for (int i=_block->num;i<_block->num+b->num;++i)
                _block->data[i]=b->data[i-_block->num];
            _block->num+=b->num;
            b->num=0;
            _block->next=b->next;
            _block->next->prev=_block;
            delete b;

        }
        if (_block->next->next!=NULL&&_block->num<blocksize/2)
        {
            block *b=_block->next;
            int tmp=(blocksize/2-_block->num>b->num)?b->num:(blocksize/2-_block->num);
            for (int i=0;i<tmp;++i) _block->data[_block->num+i]=_block->next->data[i];
            _block->num+=tmp;
            int tmp1=b->num-tmp;
            if (tmp1==0)
            {
                _block->next=b->next;
                _block->next->prev=_block;
                delete b;
            }
            else
            {
                for (int i=0;i<tmp1;++i)
                    b->data[i]=b->data[i+tmp];
                b->num-=tmp;
            }
        }
    }
    deque():blocksize(_size),num(0),head(NULL),rear(NULL)
    {
        head=new block(blocksize);
        rear=new block(1);
        head->next=rear;
        rear->prev=head;
    }
	deque(const deque &other):blocksize(other.blocksize),num(other.num),head(NULL),rear(NULL)
	 {
        head=new block(*other.head);
        rear=new block(1);
        block *p=head,*q=other.head->next;
        for (;q!=other.rear;q=q->next)
        {
            p->next=new block(*q);
            p->next->prev=p;
            p=p->next;
        }

        p->next=rear;
        rear->prev=p;
	 }
	/**
	 * TODO Deconstructor
	 */
	~deque()
	{
        clear();
        delete head;
        delete rear;
	}
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other)
	 {
        if (&other!=this)
        {
            clear();
            num=other.num;
            delete head;
            head=new block(*other.head);
            block *p=head,*q=other.head->next;
            for (;q!=other.rear;q=q->next)
            {
                p->next=new block(*q);
                p->next->prev=p;
                p=p->next;
            }
            p->next=rear;
            rear->prev=p;
        }
        return *this;
	 }
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos)
	{
        block *p=head;
        int s=pos;
        while (p!=rear&&s>=p->num)
        {
            s-=p->num;
            p=p->next;
        }
        if (p==rear)
            throw index_out_of_bound();
        return p->at(s);
	}
	const T & at(const size_t &pos) const
	{
        block *p=head;
        int s=pos;
        while (p!=rear&&s>=p->num)
        {
            s-=p->num;
            p=p->next;
        }
        if (p==rear)
            throw index_out_of_bound();
        return p->at(s);
	}
	T & operator[](const size_t &pos)
	{
        return at(pos);
	}
	const T & operator[](const size_t &pos) const
	 {
        return at(pos);
	 }
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const
	{
        if (num==0) throw container_is_empty();
        else return head->at(0);
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const
	{
        if (num==0) throw container_is_empty();
        else return rear->prev->at(rear->prev->num-1);
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin()
    {
        if (num==0) return iterator(this,rear,0);
        else return iterator(this,head,0);

    }
	const_iterator cbegin() const
	{
        if (num==0) return const_iterator(this,rear,0);
        else return const_iterator(this,head,0);
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end()
	{
        return iterator(this,rear,0);
	}
	const_iterator cend() const
	{
        return const_iterator(this,rear,0);
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const
	{
        return num==0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const
	{
        return num;
	}
	/**
	 * clears the contents
	 */
	void clear()
	{
        block *p=head->next,*q;
        delete head;
        head=new block(_size);
        head->next=rear;
        rear->prev=head;
        while (p->next!=NULL)
        {
            q=p->next;
            delete p;
            p=q;
        }
        num=0;
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value)
	{
        if (this!=pos.deq)
            throw invalid_iterator();
        else if (pos.situ==0&&pos._block->prev!=NULL)
            pos=iterator(this,pos._block->prev,pos._block->prev->num);
        ++num;
        change(pos._block);
        while (pos._block->next!=NULL&&pos.situ>pos._block->num)
        {
            pos.situ-=pos._block->num;
            pos._block=pos._block->next;
        }
        if (pos._block->next==NULL)
            throw index_out_of_bound();
        pos._block->insert(pos.situ,value);
        return pos;

	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	     iterator erase(iterator pos) {

        if (this != pos.deq) throw invalid_iterator();
        else if (pos._block == rear) throw invalid_iterator();
        num--;
        while (pos.situ < 0)
        {
            pos._block = pos._block->prev;
            pos.situ += pos._block->num;
        }
        if (pos.situ == pos._block->num)
        {
            pos.situ = 0;
            pos._block = pos._block->next;
        }
        change(pos._block);
        while (pos._block->next != NULL && pos.situ >= pos._block->num)
        {
            pos.situ -= pos._block->num;
            pos._block = pos._block->next;
        }
        if (pos._block->next == NULL) throw index_out_of_bound();
         else
         {
            pos._block->erase(pos.situ);
            if (pos.situ == pos._block->num)
            {
                pos.situ= 0;
                pos._block = pos._block->next;
            }
            return pos;
        }
    }
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value)
    {
        insert(iterator (this,rear->prev,rear->prev->num),value);
    }
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back()
	{
        if (num==0)
            throw container_is_empty();
        else erase(iterator(this,rear->prev,rear->prev->num-1));
    }
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value)
    {
        insert(begin(),value);
    }
	/**

	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front()
	{
        if (num==0) throw container_is_empty();
        else erase(begin());
	}
};

}

#endif
