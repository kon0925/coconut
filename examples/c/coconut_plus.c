/*-
 * Copyright (c) 2017, Haridimos Kondylakis, kondylak@ics.forth.gr
 */

#include <sys/types.h>

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
//#include <bsd/stdlib.h>i
#include <stdlib_mt.h>
#include <assert.h>
#include <string.h>
#include <db.h>
#include <math.h>
#include <values.h>
#include <pthread.h>
#include "./sax_breakpoints.h"

#define CLEANUP_CMD "rm -rf " BULKDIR


#define BULKDIR                 "BULK"   /* Environment directory name */
#define DATABASE_FILE           "coconut_plus.db"    /* Database file name */
#define PRIMARY_NAME            "primary"       /* Primary sub-database */
#define DATALEN         256*sizeof(float)                      /* The primary value's length */
#define NTHREADS        16		 
#define	DATABASE	"coconut_plus.db"

#define DB_MAXIMUM_PAGESIZE     (64 * 1024)     /* Maximum database page size */
//#define DB_MAXIMUM_PAGESIZE     (100 * (1024+256))     /* Maximum database page size */

typedef struct {
        unsigned long long *fileposition;
        //char *value;
	//TODO REMOVE //
        //float *ts;
        //unsigned char *sax;
        unsigned char *invsax;
} my_node;


typedef struct {
        float *ts;
} my_node_ts;

typedef struct {
        unsigned char *sax;
} my_node_sax;

struct args {
    unsigned int i;
    unsigned long from;
    unsigned long to;
    float *paa;
    my_node_sax *lis;
    //my_node *lis;
};

float *MINDISTS;
unsigned char * max_sax_cardinalities;
float mindist_sqrt;




int sax_compare(unsigned char *a, unsigned char *b, int sax_segments)
{

        int i;
        for(i=0;i<sax_segments;i++)
        {
                if ( a[i] > b[i])
                        return 1;
                else if (a[i] < b[i])
                        return -1;
        }
        return 0;


}

void invPrint(unsigned char *sax, int segments, int cardinality)
{
    int i;
    for (i=0; i < segments; i++) {
        printbin2(sax[i], cardinality);
    }
    printf("\n");
}

void printbin2(unsigned long long n, int size) {
    char *b = malloc(sizeof(char) * (size+1));
    int i;

    for (i=0; i<size; i++) {
        b[i] = '0';
    }

    for (i=0; i<size; i++, n=n/2)
        if (n%2) b[size-1-i] = '1';

    b[size] = '\0';
    printf("%s", b);
    free(b);
}






float minidist_paa_to_isax_raw(float *paa, unsigned char *sax,
                           unsigned char *sax_cardinalities,
                           unsigned char max_bit_cardinality,
                           int max_cardinality,
                           int number_of_segments,
                           int min_val,
                           int max_val,
                           float ratio_sqrt)
{

    float distance = 0;

    int offset = ((max_cardinality - 1) * (max_cardinality - 2)) / 2;

    // For each sax record find the break point
    int i;
    for (i=0; i<number_of_segments; i++) {

        unsigned char c_c = sax_cardinalities[i];
        unsigned char c_m = max_bit_cardinality;
        unsigned char v = sax[i];
        //sax_print(&v, 1, c_m);

        unsigned char region_lower = (v >> (c_m - c_c)) <<  (c_m - c_c);
        unsigned char region_upper = (~((int)MAXFLOAT << (c_m - c_c)) | region_lower);
                //printf("[%d, %d] %d -- %d\n", sax[i], c_c, region_lower, region_upper);

        float breakpoint_lower = 0; // <-- TODO: calculate breakpoints.
        float breakpoint_upper = 0; // <-- - || -


        if (region_lower == 0) {
            breakpoint_lower = min_val;
        }
        else
        {
            breakpoint_lower = sax_breakpoints[offset + region_lower - 1];
        }
        if (region_upper == max_cardinality - 1) {
            breakpoint_upper = max_val;
        }
        else
        {
            breakpoint_upper = sax_breakpoints[offset + region_upper];
        }

        //printf("\n%d.%d is from %d to %d, %lf - %lf\n", v, c_c, region_lower, region_upper,
        //       breakpoint_lower, breakpoint_upper);

        //printf("FROM: \n");
        //sax_print(&region_lower, 1, c_m);
        //printf("TO: \n");
        //sax_print(&region_upper, 1, c_m);

                //printf ("\n---------\n");

        if (breakpoint_lower > paa[i]) {
            distance += pow(breakpoint_lower - paa[i], 2);
        }
        else if(breakpoint_upper < paa[i]) {
            distance += pow(breakpoint_upper - paa[i], 2);
        }
//        else {
//            printf("%lf is between: %lf and %lf\n", paa[i], breakpoint_lower, breakpoint_upper);
//        }
    }

    //distance = ratio_sqrt * sqrtf(distance);
    distance = ratio_sqrt * distance;
    return distance;
}

void *compute_mindists(void *ptr) {
    struct args *arguments = (struct args*) ptr;

    //printf("\n[%u] Computing mindists from: %lu to: %lu\n", arguments->i, arguments->from, arguments->to);

    unsigned long i;
    for(i=arguments->from; i<arguments->to; i++) {
        unsigned char *sax = (unsigned char *)(arguments->lis)[i].sax;


        //msax_print(sax,16 ,8);  //print
        //ts_print(arguments->paa,16);

        MINDISTS[i] = minidist_paa_to_isax_raw(arguments->paa,
                                                sax,
                                                max_sax_cardinalities,
                                                8,//sax_bit_cardinality,
                                                256,//sax_alphabet_cardinality,
                                                16,//paa_segments,
                                                -2000000, //MINVAL,
                                                2000000, //MAXVAL,
                                                mindist_sqrt);
        //printf("\n%f",MINDISTS[i]);

    }

    //printf("\n[%u] Computing mindists from: %lu to: %lu\n", arguments->i, arguments->from, arguments->to);
    return NULL;
}



pthread_t workers[NTHREADS];
struct args arguments[NTHREADS];

int my_paa_from_ts (float *ts_in, float *paa_out, int segments, int ts_values_per_segment) {
    int s, i;
    for (s=0; s<segments; s++) {
        paa_out[s] = 0;
        for (i=0; i<ts_values_per_segment; i++) {
            paa_out[s] += ts_in[(s * ts_values_per_segment)+i];
        }
        paa_out[s] /= ts_values_per_segment;
    }
    return 1;
}


float my_ts_euclidean_distance(float * t, float * s, int size, float bound) {
                          //printf("\na11111111111111111bbba");fflush(stdout);
    float distance = 0;
    while (size > 0 && distance < bound) {
        size--;
        distance += (t[size] - s[size]) * (t[size] - s[size]);
        //printf("\n%f %f Distance:%f",t[size], s[size], distance);fflush(stdout);
    }
//    distance = sqrtf(distance);
                          //printf("\n22222222222222222222221111bbba");fflush(stdout);


    return distance;
}


/*
 * timer_start --
 *      Remember the start of an interval, e.g., the beginning of a major
 *      loop of bulk operations. At the end of the interval, use timer_end().
 */
void
timer_start(struct timeval *timer)
{
        (void)gettimeofday(timer, NULL);
}




/*
 * timer_end --
 *      Return the length of an interval whose start was remembered by
 *      calling timer_start().
 *
 *      Returns:
 *              The elapsed time, in floating point seconds.
 *              It never returns 0, even when the interval was smaller
 *              than the precision of the timer, which is easily possible
 *              on systems with millisecond resolution. This means that the
 *              caller does not need to guard against divide by zero errors,
 *              as when computing the average number of records per second.
 */
double
timer_end(const struct timeval *start)
{
        struct timeval now;
        double elapsed;

        (void)gettimeofday(&now, NULL);
        elapsed = (now.tv_sec - start->tv_sec) +
            ((double)now.tv_usec - start->tv_usec) / 1000000;
        /* Return a minimum duration of 1 microsecond. */
        if (elapsed <= 0.0)
                elapsed = 0.000001;
        return (elapsed);
}







void ts_print(float *ts, int size)
{
    int i;
    for (i=0; i < size; i++) {
        printf("%lf", ts[i]);
    }
    printf("\n");
}



/**
 This function prints a sax record.
 */
void sax_print(unsigned char *sax, int segments, int cardinality)
{
    int i;
    for (i=0; i < segments; i++) {
        printf("%d:\t", i);
        printbin(sax[i], cardinality);
    }
    printf("\n");
}

void printbin(unsigned long long n, int size) {
    char *b = malloc(sizeof(char) * (size+1));
    int i;

    for (i=0; i<size; i++) {
        b[i] = '0';
    }

    for (i=0; i<size; i++, n=n/2)
        if (n%2) b[size-1-i] = '1';

    b[size] = '\0';
    printf("%s\n", b);
    free(b);
}

//print sax as a single sring using MSB bits first etc.
void invSax(unsigned char *s, unsigned char  *sax, int segments, int cardinality)
{
    int j,i,k, segi, invj;
    unsigned long long n;
    //char sum[segments][cardinality];//=malloc(sizeof(char *) * (segments));

    for (j=0; j < segments; j++)
                s[j] = 0;

    segi=0;
    invj=cardinality-1;

    //printf("\n-->\n");

    for (i=cardinality-1; i>=0; i--)
    {

        for (j=0; j < segments; j++)
        {
                n=sax[j];
                n=n>>i;

                s[segi]|=(n%2)<<invj;

                invj--;
                if(invj==-1)
                {
                        segi++;
                        invj=cardinality-1;
                }

        }

    }
    //printf("\n-->\n");

}







void ordering_value(char *s, char *sax, int segments, int cardinality)
{
    int j,i,k;
    unsigned long long n;
    char sum[segments][cardinality];//=malloc(sizeof(char *) * (segments));

    for (j=0; j < segments; j++) {


        for (i=0; i<cardinality; i++) {
                sum[j][i] = '0';
        }

        n=sax[j];
        for (i=0; i<cardinality; i++, n=n/2)
                if (n%2) sum[j][cardinality-1-i] = '1';

    }


    k=0;
    for (i=0; i<cardinality; i++)
        for (j=0; j < segments; j++)
        {
         s[k]=sum[j][i];
         k++;
        }


    s[segments*cardinality] = '\0';
}

int myz_compare(const void * a1, const void *b1)
{
      	my_node *a = a1;
        my_node *b = b1;

        int i;
        for(i=0;i<16;i++)
        {
                if ((*a).invsax[i] > (*b).invsax[i])
                        return 1;
                else if ((*a).invsax[i] < (*b).invsax[i])
                        return -1;
        }
        return 0;

/*
          
	my_node *a = (my_node *)a1;
        my_node *b = (my_node *)b1;

        //my_uint128_t *a = (my_uint128_t*) &((sax_vector*)a_or)->sax;

        //my_node *b = &((my_node *) b1);

        if(a==NULL || ((*a).value)==NULL)
                printf( "\nmyz_compare==>>>>Error sorting NULL values for a value\n\n");
        if(b==NULL || ((*b).value)==NULL)
                printf( "\nmyz_compare==>>>>Error sorting NULL values for b value\n\n");

        int x=strcmp( (*a).value, (*b).value);

        if(x>0)//b less than a
                return 1;
        else if(x<0)//a less than b
                return -1;
        else //equals
                return 0;
*/	
}

int compare(const void *a, const void *b)
{
    float * c = (float *) b - 1;
    if (*(float*)a>*(float*)c && *(float*)a<=*(float*)b) {
        //printf("Found %lf between %lf and %lf\n",*(float*)a,*(float*)c,*(float*)b);
        return 0;
    }
    else if (*(float*)a<=*(float*)c) {
        return -1;
    }
    else
    {
        return 1;
    }
}



int sax_from_ts2( float *ts_in, unsigned char *sax_out, int ts_values_per_segment,
                 int segments, int cardinality, int bit_cardinality)
{
    // Create PAA representation
    float * paa = (float *)malloc(sizeof(float) * segments);
    if(paa == NULL) {
        fprintf(stderr,"error: could not allocate memory for PAA representation.\n");
        return 0;
    }

    int s, i;
    for (s=0; s<segments; s++) {
        paa[s] = 0;
        for (i=0; i<ts_values_per_segment; i++) {
            paa[s] += ts_in[(s * ts_values_per_segment)+i];
            //printf("%f", ts_in[(s * ts_values_per_segment)+i]);
        }
        paa[s] /= ts_values_per_segment;
//#ifdef DEBUG
        //printf("%d: %lf\n", s, paa[s]);
//#endif
    }

    // Convert PAA to SAX
    // Note: Each cardinality has cardinality - 1 break points if c is cardinality
    //       the breakpoints can be found in the following array positions:
    //       FROM (c - 1) * (c - 2) / 2
    //       TO   (c - 1) * (c - 2) / 2 + c - 1
    int offset = ((cardinality - 1) * (cardinality - 2)) / 2;
    //printf("FROM %lf TO %lf\n", sax_breakpoints[offset], sax_breakpoints[offset + cardinality - 2]);

    int si;
    for (si=0; si<segments; si++) {
        sax_out[si] = 0;

        // First object = sax_breakpoints[offset]
        // Last object = sax_breakpoints[offset + cardinality - 2]
        // Size of sub-array = cardinality - 1

        float *res = (float *) bsearch(&paa[si], &sax_breakpoints[offset], cardinality - 1,
                                       sizeof(float), compare);
        if(res != NULL)
            sax_out[si] = (int) (res -  &sax_breakpoints[offset]);
        else if (paa[si] > 0)
            sax_out[si] = cardinality-1;
    }

    //sax_print(sax_out, segments, cardinality);
    free(paa);
    return 1;
}


main __P((int, int **));
int	coconut_plus __P((int, int,int,int, int, int, char *, char *, int, int));
void	show __P((const char *, DBT *, DBT *));


int
bulk_dbt_init(bulk, itemcount)
        DBT *bulk;
        int itemcount;
{
        memset(bulk, 0, sizeof(DBT));
        /*
         * Allow each key/value pair to use twice the size of the data to be
         * inserted. This gives space for the bookkeeping fields required by
         * DB_MULTIPLE_WRITE() and DB_MULTIPLE_WRITE(). A simple one megabyte
         * buffer is suitable for most applications.
         */
        bulk->ulen = (u_int32_t)itemcount * 2 * (sizeof(u_int32_t) + DATALEN);
        /* Round the size up to be a multiple of 1024. */
        bulk->ulen += 1024 - (bulk->ulen % 1024);
        /*
         * In order to use a bulk DBT for get() calls, it must be at least as
         * large as the database's pages. Make sure that it as least 64KB, the
         * maximum database page size.
         */
        if (bulk->ulen < DB_MAXIMUM_PAGESIZE)
                bulk->ulen = DB_MAXIMUM_PAGESIZE;
        bulk->flags = DB_DBT_USERMEM | DB_DBT_BULK;
        if ((bulk->data = malloc(bulk->ulen)) == NULL) {
                printf("bulk_dbt_init: malloc(%u) failed: %s",
                        (unsigned)bulk->ulen, strerror(errno));
                return (errno);
        }
        memset(bulk->data, 0, bulk->ulen);
        return (0);
}


/*
 * env_init --
 *      Create the environment handle, then configure and open it for the
 *      Transactional Data Store (TDS).
 *
 */
DB_ENV *
env_init(home, progname, cachesize)
        const char *home;
        const char *progname;
        u_int cachesize;
{
        DB_ENV *dbenv;
        int ret;

        /* Allocate and initialize an empty environment handle. */
        if ((ret = db_env_create(&dbenv, 0)) != 0) {
                dbenv->err(dbenv, ret, "db_env_create");
                return (NULL);
        }

        /*
         * Send error messages to the standard error stream. You could also
         * open an application-specific log file to use here.
         */
        dbenv->set_errfile(dbenv, stderr);

        /* Include the name of the program before each error message. */
        dbenv->set_errpfx(dbenv, progname);

        /* Set the size of the cache which holds database pages. */
        if ((ret = dbenv->set_cachesize(dbenv, 0, cachesize, 0)) != 0) {
                dbenv->err(dbenv, ret, "DB_ENV->set_cachesize(%u)", cachesize);
                return (NULL);
        }
        
	if ((ret = dbenv->set_memory_max(dbenv, 2, 0)) != 0) {
                dbenv->err(dbenv, ret, "DB_ENV->set_memory_max(%u)", cachesize);
                return (NULL);
	}

        /*
         * Open the now-configured environment handle, creating the support
         * files required by the Berkeley DB Transactional Data Store and
         * setting up the in-memory data structures of the dbenv for DB access.
         */
        if ((ret = dbenv->open(dbenv, home, DB_CREATE | DB_INIT_MPOOL |
            DB_INIT_TXN | DB_INIT_LOCK, 0)) != 0) {
                dbenv->err(dbenv, ret, "DB_ENV->open(%s, TDS)", home);
                (void)dbenv->close(dbenv, 0);
                return (NULL);
        }
	

        return (dbenv);
}


int
db_init(dbenv, dbpp,  dups, secondaryflag, pagesize)
        DB_ENV *dbenv;          /* The already-opened database environemnt */
        DB **dbpp;              /* 'out': return the primary db handle. */
        int dups;               /* #duplicate per key in primary database. */
        int secondaryflag;      /* If true, create the secondary database. */
        int pagesize;           /* Create the database with this page size. */
{
        DB *dbp;                /* Local handle for the primary database. */
        DB_TXN *txnp;           /* Open/create the databases with this txn. */
        int ret;                /* Berkeley DB API return code. */
        char *dbname;           /* Database name: "primary" or "secondary". */

        /*
         * Clear these Berkeley DB handles so we know that they do not be
         * closed or aborted, otherwise cleaned up if an error occurs.
         */
        dbp = NULL;
        txnp = NULL;

        /* Allocate and initialze the handle for the primary database. */
        if ((ret = db_create(&dbp, dbenv, 0)) != 0) {
                dbenv->err(dbenv, ret, "db_create main database handle");
                return (ret);
        }

        /*
         * Configure the error handling for the database, similarly to what was
         * done for the environment handle. Here we continue to set the error
         * file to stderr; however, the prefix is set to the the database
         * filename rather than the program name as was done for the DB_ENV.
         */
        dbp->set_errfile(dbp, stderr);
        dbname = PRIMARY_NAME;
        dbp->set_errpfx(dbp, dbname);

        /*
         * By setting the btree comparison function, the records in the primary
         * database will be stored and retrieved in the numerical order. Without
         * this the keys will be sorted as byte streams. On little-endian CPUs,
         * the values returned would not be in numeric order.
         */
        //if ((ret = dbp->set_bt_compare(dbp, compare_int)) != 0) {
        //        dbp->err(dbp, ret, "set_bt_compare");
        //        goto err_cleanup;
       // }
        /*
         * Set the size of the Berkeley DB pages to use. This is a tuning
         * paramter; it does not limit the length of keys or values.
         */
        if ((ret = dbp->set_pagesize(dbp, pagesize)) != 0) {
                dbp->err(dbp, ret, "set_pagesize(%d)", pagesize);
                goto err_cleanup;
        }
        /*
         * Permits duplicates if duplicates were requested. Without this it is
         * not permitted to have two key-value pairs with the same key.
         */
        if (dups && (ret = dbp->set_flags(dbp, DB_DUP)) != 0) {
                dbp->err(dbp, ret, "set_flags(DB_DUP)");
                goto err_cleanup;
        }

        if (dups && (ret = dbp->set_flags(dbp, DB_TXN_NOT_DURABLE)) != 0) {
                dbp->err(dbp, ret, "set_flags(DB_DUP)");
                goto err_cleanup;
        }


/*	DB_TXN_STAT *statp;	
	
	if ((ret = dbenv->txn_stat(dbenv, &statp, 0)) != 0) {
		printf("\nsata stat error");
	}
	printf("\n -->  %lu  -->%lu\n",
	     (u_long)statp->st_inittxns, (u_long)statp->st_maxtxns);fflush(stdout);
	free(statp);
  */      
	/* Begin the transaction to use for creating the database(s). */
        if ((ret = dbenv->txn_begin(dbenv, NULL, &txnp, 0)) != 0)
                goto err_cleanup;

        /*
         * This DB->open() call creates the database file in the file system,
         * creates a sub-database dbname ("primary") inside that file, and
         * opens that sub-database into the handle 'dbp'.
         */
        if ((ret = dbp->open(dbp, txnp,
            DATABASE_FILE, dbname, DB_BTREE, DB_CREATE , 0664)) != 0) {
                dbp->err(dbp, ret, "DB->open(%s)", dbname);
                goto err_cleanup;
        }
        *dbpp = dbp;


        /*
         * Commit the transaction which opens and possible creates the on-disk
         * database file.
         */
        ret = txnp->commit(txnp, 0);
        txnp = NULL;
        if (ret != 0)
                goto err_cleanup;

        return (0);

        /* This label is used by any error in this function which requires
         * releasing any locally acquired resources, such as allocated
         * database handles or active transactions.
         */
err_cleanup:
        if (txnp != NULL)
                (void)txnp->abort(txnp);
        if (dbp != NULL)
                (void)dbp->close(dbp, 0);
	


        return (ret);
}







int
main(int argc, int **argv)
{
	int size=atoi(argv[1]);
	int memory=atoi(argv[2]);
	int exact=atoi(argv[3]);
	int queriesnumber=atoi(argv[4]);
	int indexing=atoi(argv[5]);
	int leafsize=atoi(argv[6]);
	char *dataset=argv[7];
	char *queries=argv[8];
	int lenz=atoi(argv[9]);
	int msort=atoi(argv[10]);

	printf("\nCOCONUT+>>Indexing %s\n", dataset);
	return (coconut_plus(size, memory, exact, queriesnumber, indexing, leafsize, dataset, queries, lenz, msort) == 1 ? EXIT_FAILURE : EXIT_SUCCESS);
}

int
coconut_plus(int ts_num, int memory, int EXACT, int queriesnumber, int indexing, int leafsize, char * dataset, char * queries, int lenz, int msort)
{
	int range=leafsize/2;
	int debug=0;
	int queriesno=queriesnumber;

	

	DB *dbp;		/* Handle of the main database to store the content of wordlist. */
	//DB *dbp_bulk;		/* Handle of the main database to store the content of wordlist. */
	DBC *dbcp;		/* Handle of database cursor used for putting or getting the word data. */
	//DB_TXN *txnp;
	DBT data;		/* The data to dbcp->put()/from dbcp->get(). */
	DBT key;		/* The key to dbcp->put()/from dbcp->get(). */
	DBT keybulk;		/* The key to dbcp->put()/from dbcp->get(). */
	DBT databulk;		/* The data to dbcp->put()/from dbcp->get(). */
	DB_BTREE_STAT *statp;	/* The statistic pointer to record the total amount of record number. */
	FILE *fp;		/* File pointer that points to the wordlist. */
	FILE *qfp;		/* File pointer that points to the queries. */
	db_recno_t recno;	/* Record number to retrieve a record in access.db database. */
	size_t len;		/* The size of buffer. */
	int cnt;		/* The count variable to read records from wordlist. */
	int ret;		/* Return code from call into Berkeley DB. */
	char *p;		/* Pointer to store buffer. */
	char *t;		/* Pointer to store reverse buffer. */
	const char *progname = "coconut_plus";		/* Program name. */
	struct timeval start;   /* Stores the time when an interval started */
	struct timeval start2;   /* Stores the time when an interval started */
	struct timeval sortstart;   /* Stores the time when an interval started */
	struct timeval iostart;   /* Stores the time when an interval started */
	
 	void *poskey=NULL;
        void *posdata=NULL;
	
	int timeseries_size=lenz;
        int paa_segments=16;
        int ts_values_per_paa_segment=timeseries_size/paa_segments;//16;
        int sax_alphabet_cardinality=256;
        int sax_bit_cardinality=8;


	if(indexing==1)
	{
		(void)remove(DATABASE); 
	
		/* Create the database handle. */
		if ((ret = db_create(&dbp, NULL, 0)) != 0) {
			fprintf(stderr,
			    "%s: db_create: %s\n", progname, db_strerror(ret));
			return (1);
		}
		/* Configure the database to use 1KB page sizes */
		if ((ret = dbp->set_pagesize(dbp, DB_MAXIMUM_PAGESIZE)) != 0) {
			dbp->err(dbp, ret, "set_pagesize");
			return (1);
		}
		


		/*
		 * Prefix any error messages with the name of this program and a ':'.
		 * Setting the errfile to stderr is not necessary, since that is the
		 * default; it is provided here as a placeholder showing where one
		 * could direct error messages to an application-specific log file.
		 */
		//dbp->set_errfile(dbp, stderr);
		//dbp->set_errpfx(dbp, progname);

		/* Open it with DB_CREATE, making it a DB_BTREE. */
		if ((ret = dbp->set_flags(dbp, DB_DUP)) != 0) {
			dbp->err(dbp, ret, "set_flags: DB_DUO");
			return (1);
		}
		if ((ret = dbp->open(dbp,
		    NULL, DATABASE, NULL, DB_BTREE, DB_CREATE, 0664)) != 0) {
			dbp->err(dbp, ret, "open: %s", DATABASE);
			return (1);
		}
			
		//memset(&key, 0, sizeof(DBT));
		//memset(&data, 0, sizeof(DBT));
		

		memset(&keybulk, 0, sizeof(DBT));
		memset(&databulk, 0, sizeof(DBT));
   	    		

		
		//keybulk.ulen= (100000000)*2*((sizeof(unsigned char)*paa_segments));
		keybulk.ulen= (memory)*2*((sizeof(unsigned char)*paa_segments));
        	keybulk.ulen += 1024 - (keybulk.ulen % 1024);
		keybulk.data = malloc(keybulk.ulen);
		keybulk.flags= DB_DBT_USERMEM | DB_DBT_BULK;
		//DB_DBT_USERMEM | DB_DBT_BULK
		memset(keybulk.data, 0, keybulk.ulen);	

	
		//databulk.ulen= (100000000)*2*(sizeof(unsigned long long));
		databulk.ulen= (memory)*2*(sizeof(unsigned long long));
        	databulk.ulen += 1024 - (databulk.ulen % 1024);
		databulk.data = malloc(databulk.ulen);
		databulk.flags= DB_DBT_USERMEM | DB_DBT_BULK;
		memset(databulk.data, 0, databulk.ulen);	
		
	 	DB_MULTIPLE_WRITE_INIT(poskey, &keybulk);
		DB_MULTIPLE_WRITE_INIT(posdata, &databulk);

	}//end if indexing

//////////////////////////////// NEW CODE
	time_t current_time;
	double duration;
	double ioduration=0;
	double durationsort=0;
	float *qts;	
    	char* c_time_string;
	float * ts = (float *)malloc(sizeof(float) * timeseries_size);
        unsigned char * sax = (unsigned char *)malloc(sizeof(unsigned char) * paa_segments);
        unsigned char * tmpsax = (unsigned char *)malloc(sizeof(unsigned char) * paa_segments);
        unsigned char * invsax = (unsigned char *)malloc(sizeof(unsigned char) * paa_segments);

        //unsigned char * value = (unsigned char *)  malloc(sizeof(char) * ((paa_segments*sax_bit_cardinality)+1));
    	
	my_node * currqueue=(my_node *)malloc(sizeof(my_node)*ts_num);

	
	current_time = time(NULL);
    	c_time_string = ctime(&current_time);
    	printf("\nIndexing starting for %d data series with %d memory and leafsize %d time series size:%d", ts_num, memory, leafsize, timeseries_size);

	timer_start(&start);	
		
	timer_start(&iostart);	

	if ((fp = fopen(dataset, "r")) == NULL) {
		fprintf(stderr, "%s: open %s: %s\n",
		    progname, dataset, db_strerror(errno));
		return (1);
	}

	ioduration = timer_end(&iostart);

	for (cnt = 0; cnt < ts_num; cnt++) 
	{
	//printf("\nzzzzzzzzzzzzzzzzzzzzzz");fflush(stdout);
		timer_start(&iostart);	
		if(indexing==1)
		{
			currqueue[cnt].fileposition= (unsigned long long *)malloc(sizeof(unsigned long long));
               		*(currqueue[cnt].fileposition)= ftell(fp);
		}
		//if(cnt==0)
		fread(ts, sizeof(float), timeseries_size, fp);
		ioduration += timer_end(&iostart);

		if(sax_from_ts2( ((float *)ts), ((unsigned char *)  sax), ts_values_per_paa_segment,
                              paa_segments, sax_alphabet_cardinality,
                              sax_bit_cardinality) == 1)
               	{

			//if(EXACT==1)
			//{
			//	currqueue[cnt].sax = (unsigned char *)malloc(sizeof(unsigned char) * paa_segments );
			//	memcpy(currqueue[cnt].sax, sax, sizeof(unsigned char) * paa_segments);
			//}
			
			currqueue[cnt].invsax = malloc(sizeof(unsigned char)*paa_segments);
                       	
			if(msort==1)
				invSax(currqueue[cnt].invsax,  sax, paa_segments ,sax_bit_cardinality);	
			else
				memcpy(currqueue[cnt].invsax, sax, sizeof(unsigned char) * paa_segments);

			if(debug==1 && cnt%((int)ts_num/10)==0)
			{
				printf("\n %d", cnt);
				fflush(stdout);
			}
				
               	}
               	else //if sax != success
               	{
               	    fprintf(stderr, "error: cannot insert record in index, since sax representation\
                       	    failed to be created");
               	} //end sax!=success


	}//end for
	if(debug==1)
	{
		printf("\nSorting started");
		fflush(stdout);
	}

	timer_start(&sortstart);	
	
	if(memory*sizeof(float)*lenz<= ts_num*(sizeof(unsigned char)*paa_segments)){
		printf("\n Summarizations do not fit in main memory and external merge sort should be used instead");
		//return;
	}
	//qsort(((void *)currqueue), ((size_t)ts_num), ((size_t)sizeof(my_node)),  myz_compare);
	//qsort_mt(((void *)currqueue), ((size_t)ts_num), ((size_t)sizeof(my_node)),  myz_compare);
	//if(msort==1)
	mergesort_mt(((void *)currqueue), ((size_t)ts_num), ((size_t)sizeof(my_node)),  myz_compare);

	durationsort = timer_end(&sortstart);

//return;
		
	if(debug==1)
	{
    		printf("\nSorting ended %f", durationsort);
		fflush(stdout);
	}

		//printf("\n\n");
	if(indexing==1)
	{
		cnt=0;
		while(cnt<ts_num)
		{

   	    		//key.data = currqueue[cnt].invsax;
      	    		//key.size = sizeof(unsigned char)*paa_segments;

	                //data.data = currqueue[cnt].fileposition;
              		//data.size = sizeof(unsigned long long);
    		
			if(cnt%memory==0)
			{
				if (cnt>1 && (ret = dbp->put(dbp, NULL, &keybulk, &databulk, DB_MULTIPLE)) != 0) {
              				dbp->err(dbp, ret, "DB->put");
     	    			}
				//printf("\n flushing  %d \n", cnt);

	 			DB_MULTIPLE_WRITE_INIT(poskey, &keybulk);
				DB_MULTIPLE_WRITE_INIT(posdata, &databulk);
			}	
				
			DB_MULTIPLE_WRITE_NEXT(poskey, &keybulk,
                                   currqueue[cnt].invsax,  sizeof(unsigned char)*paa_segments);
			
				if(poskey==NULL)
				{
					printf("\n 1:error  %d \n", cnt);
					fflush(stdout);
				}
                                assert(poskey != NULL);
                        DB_MULTIPLE_WRITE_NEXT(posdata, &databulk,
                                    currqueue[cnt].fileposition, sizeof(unsigned long long));
	
				if(posdata==NULL)
				{
				printf("\n 2:error  %d \n", cnt);
				fflush(stdout);
				}
                                assert(posdata != NULL);
			
		        /*if ((ret = dbp->put(dbp, NULL, &key, &data, DB_NOOVERWRITE)) != 0) {
              				//dbp->err(dbp, ret, "DB->put");
	       				printf("\n key exists");
					//ts_print(tsqueue[read].ts, 8);
  					//if (ret != DB_KEYEXIST)
					//{
					//	goto err1;
					//}
     	    		}*/
	 		

		    	//free(currqueue[cnt].invsax);
			//free(currqueue[cnt].fileposition);
			cnt++;	
		}
		if ((ret = dbp->put(dbp, NULL, &keybulk, &databulk, DB_MULTIPLE)) != 0) {
              			dbp->err(dbp, ret, "DB->put");
     	    	}
		
		for(cnt=0;cnt<ts_num;cnt++)
		{
		    	free(currqueue[cnt].invsax);
			free(currqueue[cnt].fileposition);
		}
		free(keybulk.data);
		free(databulk.data);
		free(currqueue);



	}//end if
        fclose(fp);	
	duration = timer_end(&start);
       	printf("\n[STAT] Insert %lld records",   ts_num);
       	printf(" in %f seconds (durationsort:%f) ", duration, durationsort);
		

	if(indexing==1)
	{
		dbp->compact(dbp, NULL, NULL, NULL, NULL,DB_FREE_SPACE , NULL);
		if ((ret = dbp->close(dbp, 0)) != 0) {
			fprintf(stderr,
	    		"%s: DB->close: %s\n", progname, db_strerror(ret));
			return (1);
		}
	}

	
       	printf("\n IO read:%f ", ioduration);
	fflush(stdout);

int ti=0;
while(ti<6)
{

	if(ti==0 || ti==3) range=500;
	else if(ti==1 || ti==4) range=1000;
	else if(ti==2 || ti==5) range=10000;

//////////////////////////////// NEW CODE
	
	/* Create the database handle.*/ 
	if ((ret = db_create(&dbp, NULL, 0)) != 0) {
		fprintf(stderr,
		    "%s: db_create: %s\n", progname, db_strerror(ret));
		return (1);
	}
	
	if ((ret = dbp->open(dbp,
	    NULL, DATABASE, NULL, DB_BTREE, DB_RDONLY, 0664)) != 0) {
		dbp->err(dbp, ret, "open: %s", DATABASE);
		return (1);
	}

	/* Get the database statistics and print the total number of records. */
	if ((ret = dbp->stat(dbp, NULL, &statp, 0)) != 0) {
		dbp->err(dbp, ret, "DB->stat");
		goto err1;
	}
	printf("\n%s: database contains %lu records in %lu leafs with minkeysperlead:%lu\n",
	    progname, (u_long)statp->bt_ndata, (u_long)statp->bt_leaf_pg, (u_long)statp->bt_minkey);
	free(statp);

    	current_time = time(NULL);
    	c_time_string = ctime(&current_time);
    	printf("\nQuerying starting at %s", c_time_string);

/////////////////////////////////////SEARCH

if ((qfp = fopen(queries, "r")) == NULL) {
	fprintf(stderr, "%s: open %s: %s\n",
	    progname, queries, db_strerror(errno));
	return (1);
}

/* Acquire a cursor for sequential access to the database. */
if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
	dbp->err(dbp, ret, "DB->cursor");
	goto err1;
}

float mindist=MAXFLOAT;
float dist;

memset(&key, 0, sizeof(key));
memset(&data, 0, sizeof(data));



my_node_sax * keys;
if(EXACT==1)
{
	printf(" exact ");fflush(stdout);

		if ((fp = fopen(dataset, "r")) == NULL) {
                        fprintf(stderr, "%s: open %s: %s\n",
                            progname, dataset, db_strerror(errno));
                        return (1);
                }

                keys=(my_node_sax *)malloc(sizeof(my_node_sax)*ts_num);
                int i;
		for(i=0;i<ts_num;i++)
                {
                        keys[i].sax = (unsigned char *)  malloc(sizeof(unsigned char) * paa_segments);
                        fread(ts, sizeof(float), timeseries_size, fp);

                        sax_from_ts2( ((float *)ts), ((unsigned char *) keys[i].sax), ts_values_per_paa_segment,
                                       paa_segments, sax_alphabet_cardinality,
                                       sax_bit_cardinality);

                }

		//fclose(fp); 
}
else
	printf(" approximate ");


unsigned long long *xi;

float avdist=0;
float avdistapprox=0;
int avgvisitednodes=0;
duration=0;
float approximateonlytime=0;

for (cnt = 0; cnt < queriesno; ++cnt) {

	qts = (float *)malloc(sizeof(float) * timeseries_size);
	fread(qts, sizeof(float), timeseries_size, qfp);
	
	float * paa = (float *) malloc(sizeof(float) * paa_segments);
        my_paa_from_ts(qts, paa, paa_segments, ts_values_per_paa_segment);

	float * mqts = (float *)malloc(sizeof(float) * timeseries_size);


        if(sax_from_ts2( ((float *)qts), ((unsigned char *) sax), ts_values_per_paa_segment,
                               paa_segments, sax_alphabet_cardinality,
                               sax_bit_cardinality) == 1)
       {
                invSax(invsax,  sax, paa_segments ,sax_bit_cardinality);	

		if(msort==1)
		      	key.data=invsax;
		else
		      	key.data=sax;
	      	key.size=sizeof(unsigned char)*paa_segments;

			
		//printf("\nSearch:%s\nSearch:", value);
		//ts_print(qts, 8);

        }
        else //if sax != success
        {
              fprintf(stderr, "error: cannot insert record in index, since sax representation\
                     failed to be created");
        } //end sax!=success




	timer_start(&start);	
	timer_start(&start2);	

		
		//printf("\naaaaaaaaaaaaaaaaaaaa");fflush(stdout);


		dbcp->get(dbcp, &key, &data, DB_SET_RANGE);
		xi = data.data;

		//printf("\nzzzzzzz exact i%d", *xi);fflush(stdout);


		fseek(fp, *xi, SEEK_SET);
		//printf("\nzzzzzzz exact i%d", *xi);fflush(stdout);
		
		fread(mqts, sizeof(float), timeseries_size, fp);
		//printf("\nFetch :%s\nFetch :", key.data);
		//ts_print(data.data, 256);


		mindist = my_ts_euclidean_distance((float *) qts,(float *)mqts, lenz, MAXFLOAT);

		//printf("\nMINDIST %f",mindist); 
	
		if(mindist!=0)
		{
			
			//printf("\naaaaaaaaaaaaaaaaaaaa %d %f\n", cnt, mindist);fflush(stdout);
			//ts_print(qts, 256);
			//printf("\n");
			//sax_print(sax, paa_segments,sax_bit_cardinality);
			//sax_print(invsax, paa_segments, sax_bit_cardinality);
			//printf("\nzzzaaaaaaaaaaaaaaaaaaaa %d %f\n", cnt, mindist);fflush(stdout);
			//ts_print(data.data, 256);


			int prevposition=0;
			while ((ret = dbcp->get(dbcp, &key, &data, DB_PREV)) == 0 && prevposition<range ){
				prevposition++;
     			}
			prevposition=0;
			while ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) == 0 && prevposition<(range*2) ){

			
				xi = data.data;
				fseek(fp, *xi, SEEK_SET);
				fread(mqts, sizeof(float), timeseries_size, fp);

				dist = my_ts_euclidean_distance((float *)qts,(float *)mqts , lenz, mindist);
                		if(dist<mindist)
                		{
                		        mindist=dist;
					//memcpy(ts,data.data, sizeof(float) * timeseries_size);
					//printf("\n");
                                        //ts_print(data.data, 8);
        	        	}
	
				prevposition++;
     			}
			//printf("\naaaaaaaaaaaaaaaaaaaa %d %f\n", cnt, mindist);fflush(stdout);
			//break;
			//printf("\nDist:%f\n", mindist);
			
			avdistapprox+=mindist;
		}
		approximateonlytime += timer_end(&start2);
int myrecs=0;
if(EXACT==1)
{
	if(mindist==0)
	{
		mindist=0;
		//memcpy(ts,data.data, sizeof(float) * timeseries_size);
	}
	else
	{


		MINDISTS=(float *) malloc(sizeof(float)*ts_num);


        	max_sax_cardinalities = (unsigned char *) malloc(sizeof(unsigned char) * paa_segments);
		int i;
        	for(i=0; i<paa_segments;i++)
                	max_sax_cardinalities[i] = sax_bit_cardinality;


        	mindist_sqrt = ((float) timeseries_size / (float) paa_segments);


// printf("\nzzzzzzzzzzzzz %s", c_time_string);fflush(stdout);


               	int ti;
                for(ti=0; ti<NTHREADS; ti++) {
	                                        arguments[ti].i = ti;
                                        arguments[ti].from = ti*(ts_num / NTHREADS);
                                        if(ti < (NTHREADS-1)) {
                                                arguments[ti].to = (ti+1)*(ts_num / NTHREADS);
                                        }
                                        else {
                                                arguments[ti].to = ts_num;
                                        }
                                        arguments[ti].paa = paa;
                                        arguments[ti].lis = keys;//currqueue;
                                        int ret = pthread_create(&workers[ti], NULL, compute_mindists, &arguments[ti]);
                 }

                 for(ti=0; ti<NTHREADS;ti++) {
                   pthread_join(workers[ti], NULL);
                 }


 //printf("\nzzzzzzzzzzzzz %s", c_time_string);fflush(stdout);


		int counti;
                for(counti=0;counti<ts_num;counti++)
                {

                                        if(MINDISTS[counti]<=mindist)
                                        {
                                                fseek(fp, counti*(sizeof(float)*timeseries_size), SEEK_SET);
                                                //fread(ts, sizeof(float), timeseries_size, fp);
                                                //dist = my_ts_euclidean_distance((float *) qts,(float *) ts , 256, mindist);
                				//invSax(invsax,  keys[counti].sax, paa_segments ,sax_bit_cardinality);	
	   					//key.data=invsax;
	   					//key.size=sizeof(unsigned char)*paa_segments;
						//dbcp->get(dbcp, &key, &data, DB_SET);
						
						//xi = data.data;
						//fseek(fp, *xi, SEEK_SET);
						fread(mqts, sizeof(float), timeseries_size, fp);

                                                dist = my_ts_euclidean_distance((float *) qts,(float *) mqts , lenz, mindist);
						avgvisitednodes++;
						myrecs++;
                                                if(dist<mindist)
                                                {
							mindist=dist;
							//memcpy(mqts,data.data, sizeof(float) * timeseries_size);
							//memcpy(mqts,ts, sizeof(float) * timeseries_size);
                                                        //printf("\nDistance:%f",dist);
                                                        //ts_print((float *)data.data, 256);
                                                }


                                        }
        	}//end for
	
                        //printf("\nDistance:%f\n",mindist); 
                        //ts_print((ts_type *)value, sax_bit_cardinality);


                 free(MINDISTS);




	}//end else db->get

                //ts_print((float *)qts, 256);
		//printf("\nDist:%f\n", mindist);
                //ts_print((float *)mqts, 256);
		//printf("\nDist:%f\n", mindist);
		avdist+=mindist;
}//end if exact==1
	

		//ts_print(ts, 8);

	duration += timer_end(&start);
	free(qts);
	free(mqts);
}
        

	if(queriesno>0)
	{
		printf("\n[STAT] Querying %lld records",   ts_num);
        	printf(" in %f seconds: ", duration);
        	printf("\nApproximate %f seconds: ", approximateonlytime);
		printf("\nAvvisitednodes:%d", avgvisitednodes/queriesno);
		printf("\nAvdistapprox:%f", (avdistapprox/queriesno));
		printf("\nAvdistexact :%f\n", (avdist/queriesno));fflush(stdout);
	}


	(void)fclose(qfp);

ti++;

                int zi;
		for(zi=0;zi<ts_num;zi++)
			free(keys[zi].sax);
		free(keys);

}//end while ti

	free(ts);
	free(sax);
	//free(value);
	free(invsax);


	/*
	 * Repeatly prompt the user for a record number, then retrieve and
	 * display that record as well as the one after it. Quit on EOF or
	 * when a zero record number is entered.
	 */
//	for (;;) {
///		/* Get a record number. */
//		printf("recno #> ");
//		fflush(stdout);
//		if (fgets(buf, sizeof(buf), stdin) == NULL)
//			break;
//		recno = atoi(buf);
//		/*
//		 * Zero is an invalid record number: exit when that (or a
//		 * non-numeric string) is entered.
//		 */
//		if (recno == 0)
//			break;
//
//		/*
//		 * Reset the key each time, the dbp->get() routine returns
//		 * the key and data pair, not just the key!
//		 */
//		key.data = &recno;
//		key.size = sizeof(recno);
//		if ((ret = dbcp->get(dbcp, &key, &data, DB_SET_RECNO)) != 0)
//			goto get_err;
//
//		/* Display the key and data. */
//		show("k/d\t", &key, &data);
//
//		/* DB_NEXT moves the cursor to the next record. */
//		if ((ret = dbcp->get(dbcp, &key, &data, DB_NEXT)) != 0)
//			goto get_err;
//		
//		/* Display the successor. */
//		show("next\t", &key, &data);
//
//		/*
//		 * Retrieve the record number for the successor into "recno"
//		 * and print it. Set data flags to DB_DBT_USERMEM so that the
//		 * record number will be retrieved into a proper, aligned
//		 * integer, which is needed by some hardware platforms. It
//		 * also makes it easier to print the value: no u_int32_t
//		 * casting is needed.
//		 */
//		data.data = &recno;
//		data.size = sizeof(recno);
//		data.ulen = sizeof(recno);
//		data.flags |= DB_DBT_USERMEM;
//		if ((ret = dbcp->get(dbcp, &key, &data, DB_GET_RECNO)) != 0) {
//get_err:		dbp->err(dbp, ret, "DBcursor->get");
//			if (ret != DB_NOTFOUND && ret != DB_KEYEMPTY)
//				goto err2;
//		} else
//			printf("retrieved recno: %lu\n", (u_long)recno);
//
//		/* Reset the data DBT. */
//		memset(&data, 0, sizeof(data));
//	}

	/* Close the cursor, then its database. */
	if ((ret = dbcp->close(dbcp)) != 0) {
		dbp->err(dbp, ret, "DBcursor->close");
		goto err1;
	}

	if ((ret = dbp->close(dbp, 0)) != 0) {
		/*
		 * This uses fprintf rather than dbp->err because the dbp has
		 * been deallocated by dbp->close() and may no longer be used.
		 */
		fprintf(stderr,
		    "%s: DB->close: %s\n", progname, db_strerror(ret));
		return (1);
	}

	return (0);

err2:	(void)dbcp->close(dbcp);
err1:	(void)dbp->close(dbp, 0);
	return (ret);

}

/*
 * show --
 *	Display a key/data pair.
 *
 * Parameters:
 *  msg		print message
 *  key		the target key to print
 *  data	the target data to print
 */
void
show(msg, key, data)
	const char *msg;
	DBT *key, *data;
{
	printf("%s%.*s :", msg,
	    (int)key->size, (char *)key->data);
	ts_print(data->data, 8);
}
