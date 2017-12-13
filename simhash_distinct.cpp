// gcc __builtin_popcount
// TBB

#include <chrono>
#include <cstdio>
#include <bitset>
#include <array>

#include "tbb/tbb.h"

using std::bitset;
using namespace tbb;

const size_t BITSET_SIZE = 300;
 
class SimilarIn {
	const unsigned long long _x;
	const std::vector<unsigned long long> _ys;
	const int _n;
	const std::vector<bool> _buffer;
public:
    void operator()( const blocked_range<size_t>& r ) const {
    	unsigned long long x = _x;
    	int n = _n;
        for( size_t i=r.begin(); i!=r.end(); ++i ) {
        	if(is_similar(x, _ys[i] ,n)){
				break;	
			}
        }
    }

    SimilarIn(const unsigned long long x, const std::vector<unsigned long long> ys,
     		const int n, std::vector<bool> result_buffer) :
        _x(x), _ys(ys), _n(n), _buffer(result_buffer)
    {}

private:  	
	inline bool is_similar(const unsigned long long x, const unsigned long long y, const int n) const {
    	return __builtin_popcountll(x ^ y) < n;
	}
};

class SimhashDistinct{

public:
	std::bitset<BITSET_SIZE> b;

	void operator()( const blocked_range<size_t>& r ) {
		const auto *xs = _xs;
		const auto *ys = _ys;
		const auto ys_size = _ys_size;
		const auto n = _n;

        for( size_t i=r.begin(); i!=r.end(); ++i ) {
        	if(similar_in(xs[i], ys, ys_size, n))
        	{
        		b.set(i);
			}
        }
    }

	SimhashDistinct( SimhashDistinct& sd, split ) : _xs(sd._xs), _ys(sd._ys), _ys_size(sd._ys_size), _n(sd._n), b(0){}
 
    void join( const SimhashDistinct& sd ) {b|=sd.b;}

	SimhashDistinct(const unsigned long long xs[], 
					const unsigned long long ys[], const std::size_t ys_size,
     				const int n) :
        			_xs(xs), _ys(ys), _ys_size(ys_size),_n(n), b(0){};
private:
	const unsigned long long *_xs;
	const unsigned long long *_ys;
	const size_t _ys_size;
	const int _n;

	inline bool is_similar(const unsigned long long x, 
					   const unsigned long long y, 
					   const int n) {
    	return __builtin_popcountll(x ^ y) < n;
	}


	inline bool similar_in(const unsigned long long x,
						   const unsigned long long ys[],
						   const size_t ys_size,
						   const int n)
	{      
		for (int i = 0; i < ys_size; ++i)
		{
			if(is_similar(x, ys[i], n)){
				return true;
			}
		}
		return false;
	}	
};

inline bool is_similar(const unsigned long long x, 
					   const unsigned long long y, 
					   const int n) {
    	return __builtin_popcountll(x ^ y) < n;
}


inline bool similar_in(const unsigned long long x,
					   const unsigned long long ys[],
					   const size_t ys_size,
					   const int n)
{      
	for (int i = 0; i < ys_size; ++i)
	{
		if(is_similar(x, ys[i], n)){
			return true;
		}
	}
	return false;
}


std::bitset<BITSET_SIZE> simhash_distinct(const unsigned long long xs[], const size_t xs_size,
											  const unsigned long long ys[], const size_t ys_size,
     										  const int n)
{
	std::bitset<BITSET_SIZE> b;
    for( size_t i=0; i < xs_size; ++i) {
    	if(similar_in(xs[i], ys, ys_size, n))
    	{
    		b.set(i);
		}
    }
    return b;
}


std::bitset<BITSET_SIZE> parallel_simhash_distinct(const unsigned long long xs[], const size_t xs_size,
											  const unsigned long long ys[], const size_t ys_size,
     										  const int n)
{
	SimhashDistinct sd(xs, ys, ys_size, n);
    parallel_reduce(blocked_range<size_t>(0, xs_size), sd);
    return sd.b;
}


int main(){
	
	constexpr size_t xs_size = 100;
	unsigned long long xs[xs_size];
	for (int i = 0; i < xs_size; ++i)
	{
		xs[i] = ~0x0ULL;
	}
	xs[xs_size-1] = 0x0ULL;
	xs[xs_size-2] = 0x0ULL;

	constexpr std::size_t ys_size = 5000;
	unsigned long long ys[ys_size];
	for (int i = 0; i < ys_size; ++i)
	{
		ys[i] = 0x0ULL;
	}
	
	printf("x:%lxx y:%lxx is_similar %d\n",xs[xs_size-1], ys[0], is_similar(xs[xs_size-1], ys[0], 3));
	auto start = std::chrono::system_clock::now();

	std::bitset<BITSET_SIZE> result;
	const auto count = 10000;
	for (int i = 0; i < count; ++i)
	{
		result = simhash_distinct(xs, xs_size, ys, ys_size, 3);
	}

	auto end = std::chrono::system_clock::now();
	auto averge_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / count;	

    std::printf("loop count=%d\nxs.size=%ld SimhashDistinct %ld in ys.size=%ld\nfinish in averge_time %ld us\n", 
    	count, xs_size, result.count(), ys_size,averge_time);
	
	return 0;
}