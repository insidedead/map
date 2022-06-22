#include <cstddef>
#include <memory>

#define ALLOC(vec, size) \
	vec->first = vec->alloc.allocate(size); \
	vec->last = vec->first + size;
	

template <class T, class U>
union Pair {
	T first;
	U second;
};

template <class T, class U>
class Map {
	typedef Pair<T, U>* iterator;

	std::allocator<Pair<T, U>> alloc;
	iterator* first;
	iterator* last;

	public:
		Map(): first(0), last(0) {};
		Map( iterator, iterator );
		Map( const Map& );

		iterator begin() const { return first; };
		iterator end() const { return last; };
		std::ptrdiff_t size() const { return last - first; };

};

template <class T, class U>
Map<T, U>::Map( Map::iterator begin, Map::iterator end )
{
	ALLOC(this, end - first);
	std::uninitialized_copy(begin, end, first);
}

template <class T, class U>
Map<T, U>::Map( const Map &m )
{
	if( this != &m )
	{
		ALLOC(this, m.size());
		std::uninitialized_copy(m.begin(), m.end(), first);
	}
}
