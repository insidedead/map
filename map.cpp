#include <cstddef>
#include <memory>
#include <algorithm>

#ifdef DB
#include <iostream>
#endif

#define ALLOC(vec, size) \
	vec->first = vec->track = vec->alloc.allocate(size); \
	vec->last = vec->first + size;
	

template <class T, class U>
struct Pair {
	~Pair();

	T first;
	U second;
};

#ifdef DB
template <class T, class U>
Pair<T, U>::~Pair<T, U>()
{
	std::cout << "destructor called" << std::endl;
}
#endif

template <class T, class U>
class Map {
	typedef Pair<T, U>* iterator;

	std::allocator<Pair<T, U>> alloc;
	iterator first;
	iterator last;
	iterator track;

	protected:
		void grow(std::size_t size);

	public:
		Map(): first(0), last(0) {};
		Map( iterator, iterator );
		Map( const Map& );
		explicit Map(std::size_t s) { ALLOC(this, s); };
		~Map();

		iterator begin() const { return first; };
		iterator end() const { return last; };
		std::ptrdiff_t size() const { return last - first; };

		U& operator[]( const T );

		void insert( Pair<T, U>& );
		void erase( T );
		std::ptrdiff_t erase( iterator, iterator );
		void swap( Map& );
};

template <class T, class U>
Map<T, U>::Map( Map::iterator begin, Map::iterator end )
{
	ALLOC(this, end - first);
	track = std::uninitialized_copy(begin, end, first);
}

template <class T, class U>
Map<T, U>::Map( const Map &m )
{
	if( this != &m )
	{
		ALLOC(this, m.size());
		track = std::uninitialized_copy(m.begin(), m.end(), first);
	}
}

template <class T, class U>
Map<T, U>::~Map()
{
	if(first)
	{
		while(last-- != first)
		{
			alloc.destroy(last);
		}

		alloc.deallocate(first, last - first);
	}
}

template <class T, class U>
U& Map<T, U>::operator[]( const T k )
{
	Pair<T, U>* l = std::find_if(first, last, [=] ( Pair<T, U>& p) {
			return p.first == k;
	});

	return l->second;
}

template <class T, class U>
void Map<T,U>::insert( Pair<T, U> &p )
{
	if(track != last)
	{
		alloc.construct(track++, p);	
	}
}

template <class T, class U>
void Map<T, U>::erase( T key )
{
	Pair<T, U>* it = std::find_if(first, last, [=] ( Pair<T, U> p ) { return p.first == key; });

	if(it)
	{
		alloc.destroy(it);
		delete it;
		
		--track;
	}
}

template <class T, class U>
std::ptrdiff_t Map<T, U>::erase( Pair<T, U> *begin, Pair<T, U> *end )
{
	while(end-- != begin)
	{
		alloc.destroy(end);
		delete end;
	}

	track = begin;

	return end - begin;
}

template <class T, class U>
void Map<T, U>::swap( Map<T, U> &m )
{
	std::pair<Pair<T,U>*, int> temp = std::get_temporary_buffer<Pair<T, U>>(this->size());
	
	std::uninitialized_copy(this->first, this->last, temp.first);

	if(this != &m)
	{
		if(m.size() > size()) grow(m.size());
		if(size() > m.size()) m.grow(size());

		std::uninitialized_copy(m.begin(), m.end(), first);
		std::uninitialized_copy(temp.first, temp.first + temp.second, m.first);
	}
}

template <class T, class U>
void Map<T, U>::grow(std::size_t size)
{
	delete this;
	ALLOC(this, size);
}

int main()
{
#ifdef DB
	Pair<char, int> t;
	t.first = 'a';
	t.second = 69;

	Pair<char, int> oo;
	oo.first = 'v';
	oo.second = 11;

	Map<char, int> p(5);
	Map<char, int> pp(5);
	
	p.insert(t);
	pp.insert(oo);

	std::cout << p['a'];

	p.swap(pp);

	std::cout << p['v'];
#endif
	return 0;
}
