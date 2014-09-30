/*
 * test.c -- test code for FEC library
 *
 * (C) 1997-98 Luigi Rizzo (luigi@iet.unipi.it)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fecpp.h"

/*
 * compatibility stuff
 */
#ifdef MSDOS	/* but also for others, e.g. sun... */
#define NEED_BCOPY
#define bcmp(a,b,n) memcmp(a,b,n)
#endif

#ifdef NEED_BCOPY
#define bcopy(s, d, siz)        memcpy((d), (s), (siz))
#define bzero(d, siz)   memset((d), '\0', (siz))
#endif

/*
 * stuff used for testing purposes only
 */

#define DEB(x)
#define DDB(x) x
#define	DEBUG	0	/* minimal debugging */
#ifdef	MSDOS
#include <time.h>
struct timeval {
    unsigned long ticks;
};
#define gettimeofday(x, dummy) { (x)->ticks = clock() ; }
#define DIFF_T(a,b) (1+ 1000000*(a.ticks - b.ticks) / CLOCKS_PER_SEC )
typedef unsigned long u_long ;
typedef unsigned short u_short ;
#else /* typically, unix systems */
#include <sys/time.h>
#define DIFF_T(a,b) \
	(1+ 1000000*(a.tv_sec - b.tv_sec) + (a.tv_usec - b.tv_usec) )
#endif

#define TICK(t) \
	{struct timeval x ; \
	gettimeofday(&x, NULL) ; \
	t = x.tv_usec + 1000000* (x.tv_sec & 0xff ) ; \
	}
#define TOCK(t) \
	{ fecpp::u_long t1 ; TICK(t1) ; \
	  if (t1 < t) t = 256000000 + t1 - t ; \
	  else t = t1 - t ; \
	  if (t == 0) t = 1 ;}

fecpp::u_long ticks[10];	/* vars for timekeeping */

void *
my_malloc(int sz, const char *s)
{
    void *p = malloc(sz) ;
    if (p != NULL)
        return p ;
    fprintf(stderr, "test: malloc failure for %d bytes in <%s>\n",
            sz, s);
    exit(1);
}

/*
 * the following is only test code and can be safely omitted
 * in applications.
 * Creates k packets of size sz of random data, encodes them,
 * and tries to decode.
 * Index contains the permutation entry.
 */

int
test_decode(fecpp::fec_code& code, size_t k, size_t index[], size_t sz,
        const char *s)
{
    int errors;
    int reconstruct = 0 ;
    int item, i ;

    static size_t prev_k = 0, prev_sz = 0;
    static fecpp::byte **d_original = NULL, **d_src = NULL ;

    if (sz < 1 || sz > 8192) {
        fprintf(stderr, "test_decode: size %d invalid, must be 1..8K\n",
                sz);
        return 1 ;
    }
    if (k < 1 || k > 255 + 1) {
        fprintf(stderr, "test_decode: k %d invalid, must be 1..%d\n",
                k, 255 + 1 );
        return 2 ;
    }
    if (prev_k != k || prev_sz != sz) {
        if (d_original != NULL) {
            for (i = 0 ; i < prev_k ; i++ ) {
                free(d_original[i]);
                free(d_src[i]);
            }
            free(d_original);
            free(d_src);
            d_original = NULL ;
            d_src = NULL ;
        }
    }
    prev_k = k ;
    prev_sz = sz ;
    if (d_original == NULL) {
        d_original = (fecpp::byte**)my_malloc(k * sizeof(fecpp::byte *), "d_original ptr");
        d_src = (fecpp::byte**)my_malloc(k * sizeof(void *), "d_src ptr");

        for (i = 0 ; i < k ; i++ ) {
            d_original[i] = (fecpp::byte*)my_malloc(sz, "d_original data");
            d_src[i] = (fecpp::byte*)my_malloc(sz, "d_src data");
        }
        /*
         * build sample data
         */
        for (i = 0 ; i < k ; i++ ) {
            for (item=0; item < sz; item++)
                d_original[i][item] = ((item ^ i) + 3) & 255;
        }
    }

    errors = 0 ;

    for( i = 0 ; i < k ; i++ )
        if (index[i] >= k ) reconstruct ++ ;

    TICK(ticks[2]);
    for( i = 0 ; i < k ; i++ )
        code.encode(d_original, d_src[i], index[i], sz);
    TOCK(ticks[2]);

    TICK(ticks[1]);
    code.decode(d_src, index, sz);
    TOCK(ticks[1]);

    for (i=0; i<k; i++)
        if (bcmp(d_original[i], d_src[i], sz )) {
            errors++;
            fprintf(stderr, "error reconstructing block %d\n", i);
        }
    if (errors)
        fprintf(stderr, "Errors reconstructing %d blocks out of %d\n",
                errors, k);

    fprintf(stderr,
            "  k %3d, l %3d  c_enc %10.6f MB/s c_dec %10.6f MB/s     \r",
            k, reconstruct,
            (double)(k * sz * reconstruct)/(double)ticks[2],
            (double)(k * sz * reconstruct)/(double)ticks[1]);

    return errors ;
}

#if 0
void
test_gf()
{
    int i ;
    /*
     * test gf tables. Sufficiently tested...
     */
    for (i=0; i<= 255; i++) {
        if (gf_exp[gf_log[i]] != i)
	    fprintf(stderr, "bad exp/log i %d log %d exp(log) %d\n",
		i, gf_log[i], gf_exp[gf_log[i]]);

        if (i != 0 && gf_mul(i, inverse[i]) != 1)
	    fprintf(stderr, "bad mul/inv i %d inv %d i*inv(i) %d\n",
		i, inverse[i], gf_mul(i, inverse[i]) );
	if (gf_mul(0,i) != 0)
	    fprintf(stderr, "bad mul table 0,%d\n",i);
	if (gf_mul(i,0) != 0)
	    fprintf(stderr, "bad mul table %d,0\n",i);
    }
}
#endif

#define KK 64 /* 255 */
#define SZ 1024
int
main(int argc, char *argv[])
{
    char buf[256];

    int kk ;
    int i ;

    size_t *ixs ;

    int lim = 255 + 1 ;

    if (lim > 1024) lim = 1024 ;

#if 0
    test_gf();
#endif

    for ( kk = KK ; kk > 2 ; kk-- )
    {
        fecpp::fec_code code(kk, lim);
        ixs = (size_t*)my_malloc(kk * sizeof(size_t), "ixs" );

        for (i=0; i<kk; i++) ixs[i] = kk - i ;
        sprintf(buf, "kk=%d, kk - i", kk);
        test_decode(code, kk, ixs, SZ, buf);

        for (i=0; i<kk; i++) ixs[i] = i ;
        test_decode(code, kk, ixs, SZ, "i");

        if (0)
        {
            for (i=0; i<kk; i++) ixs[i] = i ;
            ixs[0] = ixs[kk/2] ;
            test_decode(code, kk, ixs, SZ, "0 = 1 (error expected)");
        }

        if (0)
            for (i= lim-1 ; i >= kk ; i--)
            {
                int j ;
                for (j=0; j<KK; j++) ixs[j] = kk - j ;
                ixs[0] = i ;
                test_decode(code, kk, ixs, SZ, "0 = big");
            }

        if (1)
            for (i= lim - kk ; i >= 0 && i>= lim - kk - 4 ; i--)
            {
                int j ;
                for (j=0; j<kk; j++)
                    ixs[j] = kk -1 - j + i ;
                test_decode(code, kk, ixs, SZ, "shifted j");
            }

        if (1)
        {
            int j, max_i0 = KK/2 ;
            if (max_i0 + KK > lim)
                max_i0 = lim - KK ;
            for (i= 0 ; i <= max_i0 ; i++)
            {
                for (j=0; j<kk; j++)
                    ixs[j] = j + i ;
                test_decode(code, kk, ixs, SZ, "shifted j");
            }
        }

        fprintf(stderr, "\n");
        free(ixs);
    }
    return 0;
}
