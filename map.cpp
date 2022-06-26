#include <cstddef>
#include <memory>
#include <algorithm>
#include <iostream>

#define ALLOC(vec, size) \
	vec->first = vec->track = vec->alloc.allocate(size); \
	vec->last = vec->first + size;
	

template <class T, class U>
struct Pair {
	~Pair();

	T first;
	U second;
};

template <class T, class U>
Pair<T, U>::~Pair<T, U>()
{
	
}

template <class T, class U>
class Map {
	typedef Pair<T, U>* iterator;

	std::allocator<Pair<T, U>> alloc;
	iterator first;
	iterator last;
	iterator track;

	public:
		Map(): first(0), last(0) {};
		Map( iterator, iterator );
		Map( const Map& );
		explicit Map(std::size_t s) { ALLOC(this, s); };

		iterator begin() const { return first; };
		iterator end() const { return last; };
		std::ptrdiff_t size() const { return last - first; };

		U& operator[]( const T );

		void insert( Pair<T, U>& );
		void erase( T );
		std::ptrdiff_t erase( iterator, iterator );
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
void Map<T, U>::erase(T key)
{
	Pair<T, U>* it = std::find_if(first, last, [=] ( Pair<T, U> p ) { return p.first == key; });

	if(it)
	{
		alloc.destroy(it);
		--track;
	}
}

int main()
{
	Pair<char, int> t;
	t.first = 'a';
	t.second = 69;

	Map<char, int> p(5);

	p.insert(t);

	std::cout << p['a'];

	p.erase('a');

	std::cout << p['a'];
}
