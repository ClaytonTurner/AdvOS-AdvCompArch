#include <math.h>
#include <emmintrin.h>

__m128d c_swap(__m128d x, __m128d cond)
{
	__m128d t = _mm_xor_pd((__m128d)_mm_shuffle_epi32((__m128i) x, 0x4e), x);
	cond = _mm_and_pd(cond, t);
	return _mm_xor_pd(cond, x);
}

__m128d in2_mul8(__m128d x, __m128d y)
{
	__m128d t1 = (__m128d)_mm_shuffle_epi32((__m128i) x, 0xee);
	__m128d t2 = (__m128d)_mm_shuffle_epi32((__m128i) y, 0xee);
			
	__m128d t3 = _mm_xor_pd(x, t1);
	__m128d t4 = _mm_xor_pd(y, t2);

	if (_mm_movemask_pd(_mm_and_pd(t3, t4)))
	{
		__m128d c = {0.0, 0.0};
		__m128d c1 = _mm_cmple_pd(t2, c);
		__m128d c2 = _mm_cmple_pd(t1, c);
		__m128d c3 = (__m128d) {-0.0, 0.0};
		
		x = c_swap(_mm_xor_pd(x, c3), c1);
		y = c_swap(_mm_xor_pd(y, c3), c2);
	
		return x * _mm_xor_pd(y, c3);
	}

	/* There is a zero overlap */
	t1 = (__m128d)_mm_shuffle_epi32((__m128i) x, 0x4e) * _mm_unpacklo_pd(y, y);
	t2 *= x;

	return _mm_max_pd(t1, t2);
}

int main(){
	__m128d n;
	__m128d x = _mm_set_pd(2.0,1.0);
	__m128d y = _mm_set_pd(4.0,7.0);
	n = in2_mul8(x,y);
	//printf("%d\n",n);
	return 0;
}
