/*-
 * Copyright (c) 2017, Haridimos Kondylakis, kondylak@ics.forth.gr
 
TODO: load for exact sax buffer
TODO: add external sort_merge
*/



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <db.h>
#include <math.h>
#include <values.h>
#include <pthread.h>
#include <stdlib_mt.h>
#include "./sax_breakpoints.h"

#define CLEANUP_CMD "rm -rf " BULKDIR

int LEAF_SIZE=10;
#define INTERNAL_NODE_SIZE 50000

//#define PASEGMENTS 8
int PASEGMENTS=8;
#define BULKDIR                 "BULK"   /* Environment directory name */
#define DATABASE_FILE           "coconut.db"    /* Database file name */
#define PRIMARY_NAME            "primary"       /* Primary sub-database */
#define DATALEN         256*sizeof(float)                      /* The primary value's length */
#define NTHREADS        16		 
#define	DATABASE	"coconut.db"
#define	WORDLIST	"../../../data_1T_new.bin"
//##define	WORDLIST	"../../../data_1T_new.bin.100000000.ordered"
//#define	WORDLIST	"../../../data_1T_new.bin.100000000.ordered"
//#define	WORDLIST	"../../../DATA/astronomy.bin"
//#define	WORDLIST2	"../../../DATA/seismic.data.bin"


#define	QUERIES		"../../../Queries_new.bin"
//#define	QUERIES		"../../../astronomy.queries.bin"
//#define	QUERIESNOISE	"../../../DATA/seismic.queries.bin"

#define DB_MAXIMUM_PAGESIZE     (64 * 1024)     /* Maximum database page size */
//#define DB_MAXIMUM_PAGESIZE     (100 * (1024+256))     /* Maximum database page size */


#define CREATE_MASK(mask, index, sax_array)\
        int mask__i; \
        for (mask__i=0; mask__i < index->settings->paa_segments; mask__i++) \
                if(index->settings->bit_masks[index->settings->sax_bit_cardinality - 1] & sax_array[mask__i]) \
                        mask |= index->settings->bit_masks[index->settings->paa_segments - mask__i - 1];

float my_ts_euclidean_distance(float * t, float * s, int size, float bound);


typedef struct {
        //unsigned long long *fileposition;
        //char *value;
	//TODO REMOVE //
        float *ts;
        unsigned char *sax;
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
    //my_node_sax *lis;
    my_node *lis;
};




typedef struct {
	int size;
	struct btree_node *parent;
	int is_over_leaf;
	int **to_leaf;
	struct btree_node **child_nodes;
	unsigned char **keys;

} btree_node;


typedef struct {
	FILE *index_file;
	unsigned long long total_records;
	struct btree_node *root_node;
} btree_index;


my_node_sax *keys;
int keyscnt=0;

btree_node *root_node=NULL;
btree_node *current_node=NULL;
float searchTree(btree_node *current_node, unsigned char *key, float * ts,  int paa_segments, int sax_bit_cardinality, char *basefilename, int ts_num, int timeseries_size);

//0 equal, 1 first is larger, -1 second is larger
int invSaxCompare(unsigned char *keya, unsigned char *keyb, int paa_segments)
{
	int i;
	for(i=0;i<paa_segments;i++)
	{
		 if (keya[i] > keyb[i])
                        return 1;
                else if (keya[i] < keyb[i])
                        return -1;
	}	
	return 0;
}

float searchAllLeafs(unsigned char *key, float *qts, int paa_segments, int sax_bit_cardinality, char * basefilename, int ts_num, int timeseries_size)
{

	FILE * ifile;
        unsigned char * invsax=malloc(sizeof(unsigned char)*paa_segments);
        unsigned char * tempinvsax=malloc(sizeof(unsigned char)*paa_segments);
        unsigned char * sax=malloc(sizeof(unsigned char)*paa_segments);
        float *ts = malloc(sizeof(float) * timeseries_size);
        float *tempts = malloc(sizeof(float) * timeseries_size);
        float mindist=MAXFLOAT;
        float dist;
        char *filename = (char *)malloc(255);

        int leafno=0;
	int z=0;
	printf("\n");
        while(leafno<100)
        {

		snprintf(filename, 255, "/tmp/%s.%d.ordered.%d",basename(basefilename),ts_num, leafno);
 		ifile = fopen (filename,"r");

	        int flag=0;
        	while(fread(invsax, sizeof(char), paa_segments, ifile)>=paa_segments)
        	{
                	fread(ts, sizeof(float), timeseries_size, ifile);
	
	        	//printf("%d\t", z++);
	
                        //if(sax_compare(invsax2,currqueue[mo].invsax, paa_segments)>0)
			//	memcpy(tempts, ts, sizeof(float) * timeseries_size);
                	
			

			//printf("%d\t", sax_compare(invsax,tempinvsax, paa_segments));
        		
			//invPrint(invsax, paa_segments ,sax_bit_cardinality);
        
			searchTree(root_node, invsax, ts,  paa_segments, sax_bit_cardinality, basefilename, ts_num, timeseries_size);


			
			memcpy(tempinvsax, invsax, sizeof(char) * paa_segments);

        	        dist = my_ts_euclidean_distance((float *) qts,(float *)ts , timeseries_size, mindist);
	
	
	
        	        //if(dist<mindist)
                	//        mindist=dist;
	                //if(mindist==0)
        	        //        break;
        	
			//printf("\n\n ========================================================== \n", mindist);
		}


	 	leafno++;
        	//mindist = my_ts_euclidean_distance((float *) qts,(float *)tempts , timeseries_size, MAXFLOAT);
	        //printf("\n ================================ result ========================== \n", mindist);
        	//printf("\n Found Key:%f\n", mindist);
        	//invPrint(invsax, paa_segments ,sax_bit_cardinality);
        	//printf("\n Found Value:%f\n", mindist);
        	//ts_print(ts, timeseries_size);
        	//printf("\n ================================ Next  ========================== \n", mindist);


        	fclose(ifile);
        }

        free(invsax);
        free(filename);
        free(tempinvsax);
        free(sax);
        free(ts);
        free(tempts);

        return mindist;

}

float searchLeaf(int leafno,  unsigned char *key, float *qts, int paa_segments, int sax_bit_cardinality, char * basefilename, int ts_num, int timeseries_size)
{
	
        FILE * ifile;
        unsigned char * invsax=malloc(sizeof(unsigned char)*paa_segments);
        unsigned char * tempinvsax=malloc(sizeof(unsigned char)*paa_segments);
        unsigned char * sax=malloc(sizeof(unsigned char)*paa_segments);
        float *ts = malloc(sizeof(float) * timeseries_size);
        float *tempts = malloc(sizeof(float) * timeseries_size);
	float mindist=MAXFLOAT;
	float dist;
	char *filename = (char *)malloc(255);

	//leafno=0;
	//while(leafno<=99)
	//{
	
        snprintf(filename, 255, "/tmp/%s.%d.ordered.%d",basename(basefilename),ts_num, leafno);
	//printf("\n Searching Leafno: %d filename:%s\n", leafno, filename);
	ifile = fopen (filename,"r");

	int flag=0;
        while(fread(invsax, sizeof(char), paa_segments, ifile)>=paa_segments)
	{
        	fread(ts, sizeof(float), timeseries_size, ifile);


		dist = my_ts_euclidean_distance((float *) qts,(float *)ts , timeseries_size, mindist);

		

		if(dist<mindist)
			mindist=dist;
		if(mindist==0)
			break;

		//if(flag==0)
		//{
		//	flag=1;
		//	memcpy(tempts, ts, sizeof(float) * timeseries_size);
                //	memcpy(tempinvsax, invsax, sizeof(char) * paa_segments);
		//}

		//printf("\n");
		//invPrint(key, paa_segments ,sax_bit_cardinality);
		//invPrint(invsax, paa_segments ,sax_bit_cardinality);
		//int result=invSaxCompare(key, invsax, paa_segments);
		//if(result==0)
		//{
		//	memcpy(tempts, ts, sizeof(float) * timeseries_size);
                //	memcpy(tempinvsax, invsax, sizeof(char) * paa_segments);
		//	//printf("\n ===================Found equality============\n");
		//
		//	break;
		//}
		//else if(result==-1)
		//{
		//	//printf("\n ===================Found iinequality============\n");
		//	break;
		//}		
		//memcpy(tempts, ts, sizeof(float) * timeseries_size);
                //memcpy(tempinvsax, invsax, sizeof(char) * paa_segments);
        	//fwrite(mybuffer[xi].invsax, sizeof(unsigned char), paa_segments, outfile);
                //fwrite(mybuffer[xi].ts, sizeof(float), timeseries_size, outfile);
                //sax_from_ts2(ts, sax, ts_values_per_segment,
                //               paa_segments, sax_alphabet_cardinality,
                //               sax_bit_cardinality);
	}	

	//leafno++;
	//mindist = my_ts_euclidean_distance((float *) qts,(float *)tempts , timeseries_size, MAXFLOAT);
	//if(mindist!=0)
	//printf("\n Found Key:%f\n", mindist);
	//invPrint(invsax, paa_segments ,sax_bit_cardinality);
	//printf("\n Found Value:%f\n", mindist);
	//ts_print(ts, timeseries_size);

 	
	fclose(ifile);	

	free(invsax);
	free(filename);
	free(tempinvsax);
	free(sax);
	free(ts);
	free(tempts);

	return mindist;
}

float searchTree(btree_node *current_node, unsigned char *key, float * ts,  int paa_segments, int sax_bit_cardinality, char *basefilename, int ts_num, int timeseries_size)
{

	int size=current_node->size;

        int i;
	//printf("\n Looking in node with size %d\n", size);


	/*if(current_node->is_over_leaf==1)
	{
		for(i=0;i<size;i++)
		{
			printf(" %d ",current_node->to_leaf[i] );
		}
			printf(" %d ",current_node->to_leaf[i] );
	}*/


        for(i=0;i<size;i++)
	{
		if(current_node->keys!=NULL &&  current_node->keys[i]!=NULL)
		{
			//printf("\n %d==> ",i );fflush(stdout);
                	//invPrint(current_node->keys[i], paa_segments ,sax_bit_cardinality);

			int result=invSaxCompare(key, current_node->keys[i], paa_segments);

			//while key is larger continue
			if(result==0)
			{
				//printf("\n key is equal than current point");fflush(stdout);
				if(current_node->is_over_leaf==1)
					return searchLeaf(current_node->to_leaf[i+1], key, ts, paa_segments, sax_bit_cardinality, basefilename, ts_num, timeseries_size);
				else
					return searchTree(current_node->child_nodes[i+1], key, ts, paa_segments, sax_bit_cardinality, basefilename, ts_num, timeseries_size);
				//return;
			}
			else if(result==1)
			{
				//printf("\n key is larger than current point");fflush(stdout);
				;
			
			}
			else
			{
				//printf("\n key is smaller than current point");fflush(stdout);
				if(current_node->is_over_leaf==1)
					return searchLeaf(current_node->to_leaf[i], key, ts, paa_segments, sax_bit_cardinality, basefilename, ts_num, timeseries_size);
				else
					return searchTree(current_node->child_nodes[i], key, ts, paa_segments, sax_bit_cardinality,basefilename, ts_num, timeseries_size);
				//return;
			}
			
			//printf("\n");
		}

			
	}
	
	if(current_node->is_over_leaf==1)
		return searchLeaf(current_node->to_leaf[i], key, ts, paa_segments, sax_bit_cardinality, basefilename, ts_num, timeseries_size);
	else
		return searchTree(current_node->child_nodes[i], key, ts, paa_segments, sax_bit_cardinality, basefilename, ts_num, timeseries_size);

}




void index_insert_to_parent(unsigned char *key, btree_node *left, btree_node *right, int paa_segments)
{

	//if parent does not exist create this
	if(left->parent==NULL)
	{
		//printf("\n PARENT: parent not exists \n ");

		btree_node *new_node=(btree_node *)malloc(sizeof(btree_node));
                new_node->size=0;
                new_node->is_over_leaf=0;
                new_node->child_nodes=(btree_node **)malloc(sizeof(btree_node*)*(LEAF_SIZE+1));
                new_node->keys=(unsigned char **)malloc(sizeof(unsigned char*)*LEAF_SIZE);
        


		new_node->keys[new_node->size]=(unsigned char *) malloc(sizeof(unsigned char)*paa_segments);
		memcpy(new_node->keys[new_node->size], key, sizeof(unsigned char) * paa_segments);
		new_node->child_nodes[0]=left;
		new_node->child_nodes[1]=right;
		new_node->size++;

		left->parent=new_node;
		right->parent=new_node;
		root_node=new_node;
	}
	else 
	{
		btree_node *curr_node=left->parent;
		int cur_size=curr_node->size;
		if(cur_size<LEAF_SIZE)
		{
			//printf("\n PARENT: --------parent exists add to current node\n ");fflush(stdout);
			curr_node->keys[curr_node->size]=(unsigned char *) malloc(sizeof(unsigned char)*paa_segments);
			memcpy(curr_node->keys[curr_node->size], key, sizeof(unsigned char) * paa_segments);
			curr_node->child_nodes[(curr_node->size)+1]=right;
			curr_node->size++;

			right->parent=curr_node;
		}
		else
		{
			//printf("\n PARENT: --------parent exists but does not fit and should spit\n ");fflush(stdout);
			
                	//a. create a new node
                	btree_node *new_node=(btree_node *)malloc(sizeof(btree_node));
                	new_node->size=0;
                	new_node->is_over_leaf=0;

                	new_node->child_nodes=(btree_node **)malloc(sizeof(btree_node*)*(LEAF_SIZE+1));
                	new_node->keys=(unsigned char **)malloc(sizeof(unsigned char*)*LEAF_SIZE);


                	//b. insert middle element above
                                        //current key                  //left node  //right node
                	index_insert_to_parent(curr_node->keys[LEAF_SIZE/2],curr_node, new_node, paa_segments);


                	//printf("\n PARENT SPLIT: transfer element:%d", (LEAF_SIZE/2)+1);fflush(stdout);
                	//c.transfer index
                	//new_node->to_leaf[0]=(int *)malloc(sizeof(int));
                	new_node->child_nodes[0]=curr_node->child_nodes[(LEAF_SIZE/2)+1];


                	//d. copy half the elements in the new node
                	int temp;
                	for(temp=(LEAF_SIZE/2)+1;temp<LEAF_SIZE;temp++)
                	{
                	        //printf("\n copying leaf_no->%d current_no:%d", new_node->size, temp);fflush(stdout);

                        	//copy keys
                        	new_node->keys[new_node->size]=(unsigned char *) malloc(sizeof(unsigned char)*paa_segments);
                        	memcpy(new_node->keys[new_node->size], curr_node->keys[temp], sizeof(unsigned char) * paa_segments);
                        	//copy indexes
                        	//new_node->to_leaf[(new_node->size)+1]=(int *)malloc(sizeof(int));
                        	//memcpy(new_node->to_leaf[(new_node->size)+1], current_node->to_leaf[temp+1], sizeof(int));
                        	new_node->child_nodes[(new_node->size)+1]=curr_node->child_nodes[temp+1];

                        	new_node->size++;
                	}

                	//free past entries
                	//printf("\n PARENT SPLIT: free past entries, current_size:%d", current_node->size);fflush(stdout);
                	for(temp=(LEAF_SIZE/2)+1;temp<LEAF_SIZE;temp++)
                	{
                	        free(curr_node->keys[temp]);
                	        //free(current_node->to_leaf[temp+1]);
                	}
                	//printf("\n PARENT SPLIT: end free past entries");fflush(stdout);
	
	                //e. update current node size
	                curr_node->size=(curr_node->size)/2;
	
	
	                //g. enter new data at the end of the new node
                        new_node->keys[new_node->size]=(unsigned char *) malloc(sizeof(unsigned char)*paa_segments);
                        memcpy(new_node->keys[new_node->size], key, sizeof(unsigned char) * paa_segments);
			new_node->child_nodes[(new_node->size)+1]=right;
			new_node->size++;
			right->parent=new_node;

			//index_insert_to_parent(key, curr_node,new_node, paa_segments);
			




		}
	}

}//end void index_insert_to_parent

int tobeginwith=0;
void index_insert(unsigned char *key, int leaf_no, int paa_segments)
{
      
	//current node will be always the right most one over the leafs
	

	//as it is not full add this
	if(current_node->size<LEAF_SIZE)	
	{

		if(tobeginwith==0)
		{
			tobeginwith=1;
		
		}
		else
		{
			current_node->keys[current_node->size]=(unsigned char *) malloc(sizeof(unsigned char)*paa_segments);
        		memcpy(current_node->keys[current_node->size], key, sizeof(unsigned char) * paa_segments);
	
			current_node->to_leaf[current_node->size+1]=(int *)malloc(sizeof(int));
			current_node->to_leaf[current_node->size+1]=leaf_no;

			//printf("\n leaf_no->%d current_size:%d", leaf_no, current_node->size);fflush(stdout);
			current_node->size++;
		}
	}
	else
	{
			
//		printf("\n SPLIT ");
		////split existing node
		
		//a. create a new node

		btree_node *new_node=(btree_node *)malloc(sizeof(btree_node));
		new_node->size=0;
		new_node->is_over_leaf=1;
		
		new_node->to_leaf=(int **)malloc(sizeof(int*)*(LEAF_SIZE+1));
		new_node->keys=(unsigned char **)malloc(sizeof(unsigned char*)*LEAF_SIZE);


		//b. insert middle element above

					//current key		       //left node  //right node
		index_insert_to_parent(current_node->keys[LEAF_SIZE/2],current_node, new_node, paa_segments);
		
		
		//c.transfer index
		new_node->to_leaf[0]=(int *)malloc(sizeof(int));
		new_node->to_leaf[0]=current_node->to_leaf[(LEAF_SIZE/2)+1];

	//	printf("\n SPLIT: transfer element:%d", (LEAF_SIZE/2)+1);fflush(stdout);
		
		//free(current_node->keys[LEAF_SIZE/2]);
		//free(current_node->to_leaf[(LEAF_SIZE/2)+1]);

		//d. copy half the elements in the new node

		int temp;
		for(temp=(LEAF_SIZE/2)+1;temp<LEAF_SIZE;temp++)
		{
			//printf("\n copying leaf_no->%d current_no:%d", new_node->size, temp);fflush(stdout);
	
			//copy keys	
			new_node->keys[new_node->size]=(unsigned char *) malloc(sizeof(unsigned char)*paa_segments);
        		memcpy(new_node->keys[new_node->size], current_node->keys[temp], sizeof(unsigned char) * paa_segments);
			//copy indexes
			new_node->to_leaf[(new_node->size)+1]=(int *)malloc(sizeof(int));
        		//memcpy(new_node->to_leaf[(new_node->size)+1], current_node->to_leaf[temp+1], sizeof(int));
			new_node->to_leaf[(new_node->size)+1]=current_node->to_leaf[temp+1];
			
			new_node->size++;
		}

		//free past entries
	//	printf("\n SPLIT: free past entries, current_size:%d", current_node->size);fflush(stdout);
		for(temp=(LEAF_SIZE/2)+1;temp<LEAF_SIZE;temp++)
		{
			free(current_node->keys[temp]);
			//free(current_node->to_leaf[temp+1]);
		}
	//	printf("\n SPLIT: end free past entries");fflush(stdout);

		//e. update current node size
		current_node->size=(current_node->size)/2;

		//f. make current node the new node
		current_node=new_node;

		//g. enter new data at the end of the new node
		index_insert(key, leaf_no, paa_segments);		
		

	}

}


float *MINDISTS;
unsigned char * max_sax_cardinalities;
float mindist_sqrt;

int myz_compare(const void * a1, const void *b1);



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



int fflaag=0;
void printTr(btree_node *node, int paa_segments, int sax_bit_cardinality)
{

	if(fflaag==0)
		printf("\n\n\n -------- TREE -----------\n \n ");
	
	fflaag=1;


	if(node==NULL)
		printf("\n\n\n -------- NULL ROOT -----------\n \n ");
	else
	{
		int sizem=node->size;

		printf("\n root size:%d\n", sizem);
		fflush(stdout);

		int i=0;
		for(i=0;i<sizem;i++)
		{
                	
			invPrint(node->keys[i], paa_segments ,sax_bit_cardinality);
			fflush(stdout);
			printf("\n\n");

			//if(node->keys[i]==NULL)
			//{
			//	printf("\n NULL KEY ");
			//	fflush(stdout);
			//}
			//else
			//{
			//	printf("\n NOT NULL KEY");
			//	fflush(stdout);
			//}
		}

	}
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
        //unsigned char *sax = (unsigned char *)(arguments->lis)[i].sax;
        unsigned char *sax = (unsigned char *)keys[i].sax;


        //msax_print(sax,16 ,8);  //print
        //ts_print(arguments->paa,16);

        MINDISTS[i] = minidist_paa_to_isax_raw(arguments->paa,
                                                sax,
                                                max_sax_cardinalities,
                                                8,//sax_bit_cardinality,
                                                256,//sax_alphabet_cardinality,
                                                PASEGMENTS,//paa_segments,
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
    
    printf("[");
    for (i=0; i < size; i++) {

	if(i==0)
        printf("%lf", ts[i]);
        else
	printf(",%lf", ts[i]);
    }
    printf("]");
    //printf("\n");
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


//float * tstemp0 = malloc(sizeof(float) * timeseries_size);

int leafno=0;

void mergeandbuildindex(int startrun, int endrun, int currpass, int currrun, int totalpasses,  const char *ifilename, int ts_num, int timeseries_size, int paa_segments, int ts_values_per_segment, int sax_alphabet_cardinality, int sax_bit_cardinality, int max_total_full_buffer_size)
{
   int globalcount=0;
   int runs=endrun-startrun+1;
   int buffer_size=max_total_full_buffer_size;
   
   char * filename=ifilename;
   char* basefilename = basename(ifilename);
   unsigned long long * posinfile = malloc(sizeof(unsigned long long));
   unsigned char * sax = malloc(sizeof(unsigned char) * paa_segments);
   float * ts = malloc(sizeof(float) * timeseries_size);
   int i,j;
   if(runs==1)
   {
        char* oldfilename = malloc(255);
        snprintf(oldfilename, 255, "%s.%d.%d",filename,currpass,startrun);

        char* newfilename = malloc(255);
        snprintf(newfilename, 255, "/tmp/%s.%d.%d",basefilename,currpass+1,currrun);
	printf("\n1================================%s",newfilename);


        rename(oldfilename, newfilename);

        free(oldfilename);
        free(newfilename);
   }
   else if(runs<=buffer_size)
   {
        FILE * ifile;

        my_node * currqueue  = malloc(sizeof(my_node) * runs);
        int pos[runs];

        int currcount=0;
        int mo=0;
        FILE ** files=malloc(sizeof(FILE)*runs);
        char * filenames[runs];

        int fileno=0;
        int limits[runs];

        int totalrecords=0;

        //opening files
        for(i=startrun;i<=endrun;i++)
        {
                //printf("\n    ==> bconsuming%s.%d.%d",filename,currpass,i);fflush(stdout);

                filenames[fileno] = malloc(255);
                snprintf(filenames[fileno], 255, "/tmp/%s.%d.%d",basefilename,currpass,i);


                files[fileno] = fopen(filenames[fileno], "r");


                //filenames[fileno]=strdup(pfilename);


                if (files[fileno]== NULL) {
                        fprintf(stderr, "B:File %s not found!\n",filenames[fileno]);
                        exit(-1);
                }

                fseek(files[fileno], 0L, SEEK_END);

                unsigned long long sz = (unsigned long long) ftell(files[fileno]);

                unsigned long long total_records_in_file;
                total_records_in_file = (sz/(sizeof(float)* timeseries_size ));

                fseek(files[fileno], 0L, SEEK_SET);

                limits[fileno]=total_records_in_file-1;
                totalrecords+=total_records_in_file;

                currqueue[fileno].invsax=malloc(sizeof(unsigned char)*paa_segments);
                currqueue[fileno].sax=malloc(sizeof(unsigned char)*paa_segments);
                currqueue[fileno].ts = malloc(sizeof(float) * timeseries_size);

                fileno++;
        }

        //read one element from each file
        for(i=0;i<fileno;i++)
        {

                FILE * ifile2=files[i];

                fread(ts, sizeof(float), timeseries_size, ifile2);
                sax_from_ts2(ts, sax, ts_values_per_segment,
                               paa_segments, sax_alphabet_cardinality,
                               sax_bit_cardinality);


                if(currcount==0)
                {
                        invSax(currqueue[currcount].invsax, sax,paa_segments ,sax_bit_cardinality);
                        memcpy(currqueue[currcount].sax, sax, sizeof(unsigned char) * paa_segments);
                        memcpy(currqueue[currcount].ts, ts, sizeof(float) * timeseries_size);
                        pos[currcount]=i;
                }
                else
                {
                        int mo;

                        unsigned char * invsax2=malloc(sizeof(unsigned char)*paa_segments);
                        invSax(invsax2, sax,paa_segments ,sax_bit_cardinality);

                        unsigned char * sax2=malloc(sizeof(unsigned char)*paa_segments);
                        memcpy(sax2, sax,sizeof(unsigned char) * paa_segments);


                        for(mo=currcount-1;mo>=0;mo--)
                        {
                                if(sax_compare(invsax2,currqueue[mo].invsax, paa_segments)>0)
                                {
                                        memcpy(currqueue[mo+1].invsax, currqueue[mo].invsax, sizeof(unsigned char)*paa_segments );
                                        memcpy(currqueue[mo+1].sax, currqueue[mo].sax, sizeof(unsigned char)*paa_segments );
                                        memcpy(currqueue[mo+1].ts,  currqueue[mo].ts, sizeof(float) * timeseries_size);
                                        pos[mo+1]=pos[mo];
                                }
                                else
                                        break;
                        }


                        memcpy(currqueue[mo+1].invsax, invsax2, sizeof(unsigned char)*paa_segments );
                        memcpy(currqueue[mo+1].sax, sax2, sizeof(unsigned char)*paa_segments );
                        memcpy(currqueue[mo+1].ts, ts, sizeof(float) * timeseries_size);
                        pos[mo+1]=i;
                        free(invsax2);
                }

                currcount++;

        }

        //char* newfilename = malloc(255);
        //snprintf(newfilename, 255, "/tmp/%s.%d.%d",basefilename,currpass+1, currrun);
	//printf("\n3================================%s",newfilename);

        //char *zivalue= malloc(sizeof(char)*(((index->settings->paa_segments)*(index->settings->sax_bit_cardinality))+1));
        unsigned char *ziinvsax= malloc(sizeof(unsigned char)*paa_segments);
        unsigned char *zisax= malloc(sizeof(unsigned char)*paa_segments);

	unsigned char *index_invsax= malloc(sizeof(unsigned char)*paa_segments);
	int new_leaf=1;

        my_node * mybuffer;
        int sizecounter=0;

        int flag=0;
        int maxpos=0;
        int new_buffer_size=buffer_size;
        
	char* outfilename = malloc(255);
        FILE *outfile;

        for(j=0;j<totalrecords;j++)
        {
                maxpos=pos[currcount-1];

                if(flag==0)
                {
                        //mybuffer = malloc(sizeof(my_node) * buffer_size*runs);
                        mybuffer = malloc(sizeof(my_node) * buffer_size);
                        flag=1;
                        sizecounter=0;
                }

		if(j%20000000==0)
		{
			printf("\n\n Stepped %d:", totalrecords);
		}

                if( buffer_size<LEAF_SIZE)
		{
		       
			if(j%buffer_size==0)
			{
				 int xi=0;
	                        for(xi=0;xi<sizecounter;xi++)
        	                {
	
        	                        fwrite(mybuffer[xi].invsax, sizeof(unsigned char), paa_segments, outfile);
                	                fwrite(mybuffer[xi].ts, sizeof(float), timeseries_size, outfile);
                        
					keys[keyscnt].sax = (unsigned char *)  malloc(sizeof(unsigned char) * paa_segments);
                			memcpy(keys[keyscnt].sax, mybuffer[xi].sax, sizeof(unsigned char) * paa_segments);
					keyscnt++;
                                        //sax_print(mybuffer[xi].invsax, paa_segments ,sax_bit_cardinality);

                			//memcpy(index_invsax, mybuffer[xi].invsax, sizeof(unsigned char) * paa_segments);
					if(new_leaf==1)
					{
						index_insert(mybuffer[xi].invsax, j/LEAF_SIZE, paa_segments);
						new_leaf=0;
					}

                        	        globalcount++;

                        	}

                        	for(xi=0;xi<sizecounter;xi++)
                        	{
                        	        free(mybuffer[xi].ts);
                        	        free(mybuffer[xi].invsax);
                                        free(mybuffer[xi].sax);
                        	}

                        	free(mybuffer);
                        	mybuffer = malloc(sizeof(my_node) * buffer_size);
                        	sizecounter=0;
                        	new_buffer_size=buffer_size;
			}	
			
			if(j%LEAF_SIZE==0)
			{
			if(j>1)
       				fclose(outfile);
       			snprintf(outfilename, 255, "/tmp/%s.%d.ordered.%d", basefilename, ts_num, (j/LEAF_SIZE));
			new_leaf=1;
       			outfile = fopen(outfilename, "w");
			//printf("\n2===============================%s",outfilename);
			}

		}
		else if(buffer_size>LEAF_SIZE)
		{
		

			if(j%buffer_size==0)
                        {
                                int xi=0;
                                for(xi=0;xi<sizecounter;xi++)
                                {

					if(globalcount%LEAF_SIZE==0)
                        		{
                        			if(globalcount>1)
                                		fclose(outfile);
                        			snprintf(outfilename, 255, "/tmp/%s.%d.ordered.%d", basefilename, ts_num, (globalcount/LEAF_SIZE));
						new_leaf=1;
                        			outfile = fopen(outfilename, "w");
                        			//printf("\n2===============================%s",outfilename);
                        		}	


                                        fwrite(mybuffer[xi].invsax, sizeof(unsigned char), paa_segments, outfile);
                                        fwrite(mybuffer[xi].ts, sizeof(float), timeseries_size, outfile);
					
					keys[keyscnt].sax = (unsigned char *)  malloc(sizeof(unsigned char) * paa_segments);
                			memcpy(keys[keyscnt].sax, mybuffer[xi].sax, sizeof(unsigned char) * paa_segments);
					keyscnt++;
					
					if(new_leaf==1)
					{
						index_insert(mybuffer[xi].invsax, globalcount/LEAF_SIZE, paa_segments);
						new_leaf=0;
					}
					//index_insert(mybuffer[xi].invsax, globalcount/LEAF_SIZE, paa_segments);
                                        //sax_print(mybuffer[xi].invsax, paa_segments ,sax_bit_cardinality);

                                        globalcount++;

                                }

                                for(xi=0;xi<sizecounter;xi++)
                                {
                                     //   free(mybuffer[xi].invsax);
                                        free(mybuffer[xi].sax);
                                        free(mybuffer[xi].ts);
                                }

                                free(mybuffer);
                                mybuffer = malloc(sizeof(my_node) * buffer_size);
                                sizecounter=0;
                                new_buffer_size=buffer_size;
                        }



		}
		else if(buffer_size==LEAF_SIZE && j%buffer_size==0 && j>1)
		{
		
			if(j>1)
       				fclose(outfile);
       			snprintf(outfilename, 255, "/tmp/%s.%d.ordered.%d", basefilename, ts_num, (j/LEAF_SIZE)-1);
			new_leaf=1;
       			outfile = fopen(outfilename, "w");
			//printf("\n2===============================%s, %d",outfilename, sizecounter);

               	        int xi=0;
               	        for(xi=0;xi<sizecounter;xi++)
               	        {

                       	        fwrite(mybuffer[xi].invsax, sizeof(unsigned char), paa_segments, outfile);
                       	        fwrite(mybuffer[xi].ts, sizeof(float), timeseries_size, outfile);
				
				keys[keyscnt].sax = (unsigned char *)  malloc(sizeof(unsigned char) * paa_segments);
                		memcpy(keys[keyscnt].sax, mybuffer[xi].sax, sizeof(unsigned char) * paa_segments);
				keyscnt++;
				if(new_leaf==1)
				{
					index_insert(mybuffer[xi].invsax, (j/LEAF_SIZE)-1, paa_segments);
					new_leaf=0;
				}
				//index_insert(mybuffer[xi].invsax, j/LEAF_SIZE, paa_segments);
          				//sax_print(mybuffer[xi].invsax, paa_segments ,sax_bit_cardinality);

                                globalcount++;

       	                }

			for(xi=0;xi<sizecounter;xi++)
                       	{
                       	        free(mybuffer[xi].ts);
                                free(mybuffer[xi].sax);
                       	}

       	                free(mybuffer);
               	        mybuffer = malloc(sizeof(my_node) * buffer_size);
                       	sizecounter=0;
                       	new_buffer_size=buffer_size;

		}


		mybuffer[sizecounter].ts=malloc(sizeof(float)* timeseries_size);
                memcpy(mybuffer[sizecounter].ts, currqueue[currcount-1].ts, sizeof(float) * timeseries_size);
		
		mybuffer[sizecounter].invsax=malloc(sizeof(unsigned char)* paa_segments);
                memcpy(mybuffer[sizecounter].invsax, currqueue[currcount-1].invsax, sizeof(unsigned char) * paa_segments);

		mybuffer[sizecounter].sax=malloc(sizeof(unsigned char)* paa_segments);
                memcpy(mybuffer[sizecounter].sax, currqueue[currcount-1].sax, sizeof(unsigned char) * paa_segments);
                //invPrint(currqueue[currcount-1].invsax, paa_segments ,sax_bit_cardinality);
                //ts_print(currqueue[currcount-1].ts,  index->settings->sax_bit_cardinality);


                sizecounter++;
                currcount--;



                //open file that record belonged and insert it as next element in queue
                if(limits[maxpos]>0)
                {
                        FILE * ifile2=files[maxpos];

                        fread(ts, sizeof(float), timeseries_size, ifile2);

                        sax_from_ts2(ts, sax, ts_values_per_segment,
                                        paa_segments, sax_alphabet_cardinality,
                                        sax_bit_cardinality);

                        if(currcount==0)
                        {

                                invSax(currqueue[currcount].invsax, sax, paa_segments ,sax_bit_cardinality);
                                memcpy(currqueue[currcount].ts, ts, sizeof(float) * timeseries_size);
                                memcpy(currqueue[currcount].sax, sax, sizeof(unsigned char) * paa_segments);
                                pos[currcount]=maxpos;
                        }
                        else
                        {
                                invSax(ziinvsax, sax, paa_segments ,sax_bit_cardinality);
                                memcpy(zisax, sax,  sizeof(unsigned char)*paa_segments);

                                int mo;
                                for(mo=currcount-1;mo>=0;mo--)
                                {
                                        if(sax_compare(ziinvsax,currqueue[mo].invsax, paa_segments)>0)
                                        {
                                                memcpy(currqueue[mo+1].invsax, currqueue[mo].invsax,  sizeof(unsigned char)*paa_segments);
                                                memcpy(currqueue[mo+1].sax, currqueue[mo].sax,  sizeof(unsigned char)*paa_segments);
                                                memcpy(currqueue[mo+1].ts,  currqueue[mo].ts, sizeof(float) * timeseries_size);
                                                pos[mo+1]=pos[mo];
                                        }
                                        else
                                                break;
                                }


                                memcpy(currqueue[mo+1].ts, ts, sizeof(float) * timeseries_size);
                                memcpy(currqueue[mo+1].invsax, ziinvsax,  sizeof(unsigned char)*paa_segments);
                                memcpy(currqueue[mo+1].sax, zisax,  sizeof(unsigned char)*paa_segments);
                                pos[mo+1]=maxpos;
                        }
                        currcount++;
                        limits[maxpos]--;
                }



        }


        int xi=0;
        for(xi=0;xi<sizecounter;xi++)
        {
		 if(LEAF_SIZE<buffer_size && globalcount%LEAF_SIZE==0)
                 {
        	         fclose(outfile);
                         snprintf(outfilename, 255, "/tmp/%s.%d.ordered.%d", basefilename, ts_num, (globalcount/LEAF_SIZE));
			 new_leaf=1;
                         outfile = fopen(outfilename, "w");
                         //printf("\n2===============================%s",outfilename);
                 }
		 else if(LEAF_SIZE==buffer_size && globalcount%LEAF_SIZE==0)
		 {
        	         fclose(outfile);
                         snprintf(outfilename, 255, "/tmp/%s.%d.ordered.%d", basefilename, ts_num, (globalcount/LEAF_SIZE));
			 new_leaf=1;
                         outfile = fopen(outfilename, "w");
                         //printf("\n2===============================%s",outfilename);
		 }


                fwrite(mybuffer[xi].invsax, sizeof(unsigned char), paa_segments, outfile);
        	fwrite(mybuffer[xi].ts, sizeof(float), timeseries_size, outfile);
		
		keys[keyscnt].sax = (unsigned char *)  malloc(sizeof(unsigned char) * paa_segments);
                
		memcpy(keys[keyscnt].sax, mybuffer[xi].sax, sizeof(unsigned char) * paa_segments);
		keyscnt++;
		
		if(new_leaf==1)
		{
			index_insert(mybuffer[xi].invsax, globalcount/LEAF_SIZE, paa_segments);
			new_leaf=0;
		}
		//index_insert(mybuffer[xi].invsax, globalcount/LEAF_SIZE, paa_segments);
                //ts_print(mybuffer[xi].ts,  index->settings->sax_bit_cardinality);
                globalcount++;
        }
        for(xi=0;xi<sizecounter;xi++)
        {
        	free(mybuffer[xi].invsax);
        	free(mybuffer[xi].ts);
        }


        for(i=0;i<fileno;i++)
        {
                free(currqueue[i].invsax);
                free(currqueue[i].sax);
                free(currqueue[i].ts);
                fclose(files[i]);
                unlink(filenames[i]);
                free(filenames[i]);
        }

        free(files);
        free(currqueue);
        free(mybuffer);
        //free(newfilename);
        free(outfilename);
        free(ziinvsax);
        free(zisax);

        fclose(outfile);
   }
   else
   {
        printf("\n mergind startrun:%d endrun:%d in  pass %d",startrun,endrun,currpass);fflush(stdout);

        int newruns=0;
        int paststep=0;
        for(i=startrun;i<endrun;i=i+buffer_size)
        {
                newruns++;
                paststep=i+buffer_size-1;
                if(endrun<i+buffer_size-1)
                        mergeandbuildindex(i, endrun,currpass,newruns, totalpasses, ifilename, ts_num, timeseries_size, paa_segments, ts_values_per_segment, sax_alphabet_cardinality, sax_bit_cardinality, max_total_full_buffer_size);
                else
                        mergeandbuildindex(i, i+buffer_size-1,currpass,newruns, totalpasses, ifilename, ts_num, timeseries_size, paa_segments, ts_values_per_segment, sax_alphabet_cardinality, sax_bit_cardinality, max_total_full_buffer_size);
        }

        if(endrun>paststep)
        {
                //printf("\n zzzi:%d runs:%d ",endrun,paststep);
                newruns++;
                mergeandbuildindex(paststep+1, endrun, currpass, newruns, totalpasses, ifilename, ts_num, timeseries_size, paa_segments, ts_values_per_segment, sax_alphabet_cardinality, sax_bit_cardinality, max_total_full_buffer_size);
        }

        currpass++;
        mergeandbuildindex(1, newruns, currpass, 1, totalpasses, ifilename, ts_num, timeseries_size, paa_segments, ts_values_per_segment, sax_alphabet_cardinality, sax_bit_cardinality, max_total_full_buffer_size);


   }
   free(posinfile);
   free(sax);
   free(ts);
}




double external_sort_merge_index(const char *ifilename, int ts_num, int max_total_full_buffer_size, int timeseries_size, int paa_segments, int ts_values_per_segment, int sax_alphabet_cardinality, int sax_bit_cardinality)
{
	FILE * ifile;
	double time_to_substr=0;
	time_t current_time;
	struct timeval start;

	ifile = fopen (ifilename,"r");
	if (ifile == NULL) {
        	fprintf(stderr, "A:File %s not found!\n", ifilename);
        	exit(-1);
    	}
  	


	unsigned char * sax = (unsigned char *)malloc(sizeof(unsigned char) * paa_segments);
	float * ts = (float *)malloc(sizeof(float) * timeseries_size);

	my_node * currqueue;
	int currcount=0;
    	int runs=0;
	int ts_loaded=0;
    	while (ts_loaded<ts_num)
    	{
            currcount=0;

            int curqueusize=max_total_full_buffer_size;
            if(ts_num<max_total_full_buffer_size)
                curqueusize=ts_num;
            my_node * currqueue  = malloc(sizeof(my_node) * curqueusize);

            while (ts_loaded<ts_num && currcount<max_total_full_buffer_size)
            {


		fread(ts, sizeof(float), timeseries_size, ifile);
                sax_from_ts2(ts, sax, ts_values_per_segment,
                               paa_segments, sax_alphabet_cardinality,
                               sax_bit_cardinality);

 
                currqueue[currcount].invsax = malloc(sizeof(unsigned char)*paa_segments);
                invSax(currqueue[currcount].invsax, sax,paa_segments ,sax_bit_cardinality);
                currqueue[currcount].ts = malloc(sizeof(float) * timeseries_size);
                memcpy(currqueue[currcount].ts, ts, sizeof(float) * timeseries_size);
                       
                currcount++;
                ts_loaded++;
             }

             runs++;

 	     mergesort_mts(currqueue, currcount, sizeof(my_node),  myz_compare, 16);
		
	     char* basefilename = basename(ifilename);

	     if(currcount>0)
             {
                char* pfilename = malloc(255);
                snprintf(pfilename, 255, "/tmp/%s.0.%d",basefilename,runs);
                
		FILE *pfile = fopen(pfilename, "w");
                int j;
                for (j = 0; j < currcount; j++) {
                	fwrite(currqueue[j].ts,
                        sizeof(float),timeseries_size, pfile);
                }
                fclose(pfile);
                free(pfilename);

                for (j = 0; j < currcount; j++) {
                        free(currqueue[j].ts);
                        free(currqueue[j].invsax);
                }
             }
             free(currqueue);
      }//end while
      
      //printf("\n4================================%d",ts_loaded);

      int passes=(float)ceil(log((float)runs)/log((float)max_total_full_buffer_size));

      free(ts);
      free(sax);
      fclose(ifile);

      mergeandbuildindex(1,runs, 0,0,passes, ifilename, ts_num, timeseries_size, paa_segments, ts_values_per_segment, sax_alphabet_cardinality, sax_bit_cardinality, max_total_full_buffer_size);

//void mergeandbuildindex(int startrun, int endrun, int currpass, int currrun, int totalpasses,  const char *ifilename, int ts_num, int timeseries_size, int paa_segments, int ts_values_per_segment, int sax_alphabet_cardinality, int sax_bit_cardinality, int max_total_full_buffer_size)

	return time_to_substr;
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
        for(i=0;i<PASEGMENTS;i++)
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
int	coconut __P((int, int,int,int, int, int, char *, char *, int));
void	show __P((const char *, DBT *, DBT *));






int
main(int argc, int **argv)
{
	int size=atoi(argv[1]);
	int memory=atoi(argv[2]);
	int exact=atoi(argv[3]);
	int queriesnumber=atoi(argv[4]);
	int indexing=atoi(argv[5]);
	int leafsize=atoi(argv[6]);
	LEAF_SIZE=leafsize;
	char *dataset=argv[7];
	char *queries=argv[8];
	int lenz=atoi(argv[9]);
	PASEGMENTS=atoi(argv[10]);	
	
	printf("\n -THIS ALGORITHM ASSUMES THAT DATA ARE LARGER THAN MEMORY");fflush(stdout);
	printf("\n -BUFFER_SIZE SHOULD BE A MULTIPLIER of LEAF_SIZE");fflush(stdout);

	//printf("\n{\n	{\"algorithm\": \"CTreeFull\"},");fflush(stdout);

	return (coconut(size, memory, exact, queriesnumber, indexing, leafsize, dataset, queries, lenz) == 1 ? EXIT_FAILURE : EXIT_SUCCESS);
	
}

int
coconut(int ts_num, int memory, int EXACT, int queriesnumber, int indexing, int leafsize, char * dataset, char * queries, int lenz)
{
	int range=leafsize/2;
	int debug=0;
	int queriesno=queriesnumber;

	

	FILE *fp;		/* File pointer that points to the wordlist. */
	FILE *qfp;		/* File pointer that points to the queries. */
	FILE *indexfp;		/* File pointer that points to the queries. */
	size_t len;		/* The size of buffer. */
	int cnt;		/* The count variable to read records from wordlist. */
	int ret;		/* Return code from call into Berkeley DB. */
	char *p;		/* Pointer to store buffer. */
	char *t;		/* Pointer to store reverse buffer. */
	const char *progname = "coconut";		/* Program name. */
	struct timeval start;   /* Stores the time when an interval started */
	struct timeval sortstart;   /* Stores the time when an interval started */
	struct timeval iostart;   /* Stores the time when an interval started */
	
 	void *poskey=NULL;
        void *posdata=NULL;
	

	int timeseries_size=lenz;
        int paa_segments=PASEGMENTS;
        int ts_values_per_segment=timeseries_size/paa_segments;//16;
        int sax_alphabet_cardinality=256;
        int sax_bit_cardinality=8;


	
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

    	
	//my_node * currqueue=(my_node *)malloc(sizeof(my_node)*ts_num);

	//initiate_index;
	//myindex.total_records=0;

	current_node=(btree_node *)malloc(sizeof(btree_node));
	current_node->size=0;
	current_node->is_over_leaf=1;
	current_node->to_leaf=(int **)malloc(sizeof(int*)*(LEAF_SIZE+1));
	current_node->to_leaf[0]=(int *)malloc(sizeof(int));
	current_node->to_leaf[0]=0;
	current_node->keys=(unsigned char **)malloc(sizeof(unsigned char*)*LEAF_SIZE);
	root_node=current_node;



        keys=(my_node_sax *)malloc(sizeof(my_node_sax)*ts_num);


	
	current_time = time(NULL);
    	c_time_string = ctime(&current_time);
    	//printf("\nIndexing starting at %s for %d data series  with %d memory and leafsize:%d", c_time_string, ts_num, memory, leafsize);
		
	double subtract=0;
	
	if(debug==1)
	{
		printf("\nSorting started");
		fflush(stdout);
	}
	timer_start(&sortstart);	
	external_sort_merge_index(dataset, ts_num, memory, timeseries_size, paa_segments, ts_values_per_segment, sax_alphabet_cardinality, sax_bit_cardinality);
	durationsort = timer_end(&sortstart);
	if(debug==1)
	{
    		printf("\nSorting ended");
		fflush(stdout);
	}


	/*

	
	timer_start(&iostart);	
	if(memory<ts_num)
		(void)fclose(fp);
	ioduration += timer_end(&iostart);

	
*/
    	current_time = time(NULL);
    	c_time_string = ctime(&current_time);
    	//printf("\nQuerying starting at %s", c_time_string);


	if ((qfp = fopen(queries, "r")) == NULL) {
	fprintf(stderr, "%s: open %s: %s\n",
	    progname, queries, db_strerror(errno));
	return (1);
	}

	float mindist=MAXFLOAT;
	float dist;
/*
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));


	int myheat[10];
	int heatno=0;
	for(heatno=0;heatno<10;heatno++)
		myheat[heatno]=0;


	int myvisitedleafs[ts_num/2000];
	for(heatno=0;heatno<ts_num/2000;heatno++)
		myvisitedleafs[heatno]=0;


	if(EXACT==1)
	{
	//printf(" exact ");
	--*
		if ((fp = fopen(WORDLIST, "r")) == NULL) {
                        fprintf(stderr, "%s: open %s: %s\n",
                            progname, WORDLIST, db_strerror(errno));
                        return (1);
                }

                int i;
		for(i=0;i<ts_num;i++)
                {
                        keys[i].sax = (unsigned char *)  malloc(sizeof(unsigned char) * paa_segments);
                        fread(ts, sizeof(float), timeseries_size, fp);

                        sax_from_ts2( ((float *)ts), ((unsigned char *) keys[i].sax), ts_values_per_paa_segment,
                                       paa_segments, sax_alphabet_cardinality,
                                       sax_bit_cardinality);

                }
	--*
 
	}
	//else
		//printf(" approximate ");

*/

float avdist=0;
float avdistapprox=0;
int nodesvisited=0;
duration=0;
/*
int exactleafs=0;
int maxleafs=0;
int tempexactleaf=0;

data.data=(float *)malloc(sizeof(float) * timeseries_size);
printf("\n	{\"results\": \"[");fflush(stdout);

//for (cnt = 0; cnt < 0; ++cnt) {
*/
for (cnt = 0; cnt < queriesno; ++cnt) {
//for (cnt = 0; cnt < 1; ++cnt) {

	qts = (float *)malloc(sizeof(float) * timeseries_size);
	float * mqts = (float *)malloc(sizeof(float) * timeseries_size);
	fread(qts, sizeof(float), timeseries_size, qfp);
	

        if(sax_from_ts2( ((float *)qts), ((unsigned char *) sax), ts_values_per_segment,
                               paa_segments, sax_alphabet_cardinality,
                               sax_bit_cardinality) == 1)
       {
                invSax(invsax,  sax, paa_segments ,sax_bit_cardinality);	
       }
       else 
       {
             printf("error: cannot insert record in index, since sax representation\
                    failed to be created");
       } //end sax!=success


        //printf("\nSearch forkey \n");
	//invPrint(invsax, paa_segments ,sax_bit_cardinality);
        //printf("\nSearch for value\n");
	//ts_print(qts, 256);

	//TODO remove these two lines
	//searchAllLeafs(invsax, qts, paa_segments, sax_bit_cardinality, dataset, ts_num, timeseries_size);
	//return;


	timer_start(&start);	
        mindist=searchTree(root_node, invsax, qts,  paa_segments, sax_bit_cardinality, dataset, ts_num, timeseries_size);

	
	avdistapprox+=mindist;

        //printf("\nResult:%f\n", searchTree(root_node, invsax, qts,  paa_segments, sax_bit_cardinality, dataset, ts_num, timeseries_size));
	//fflush(stdout);


	if(EXACT==1 && mindist!=0)
	{

		MINDISTS=(float *) malloc(sizeof(float)*ts_num);

       		max_sax_cardinalities = (unsigned char *) malloc(sizeof(unsigned char) * paa_segments);
		int i;
       		for(i=0; i<paa_segments;i++)
        	       	max_sax_cardinalities[i] = sax_bit_cardinality;
       		mindist_sqrt = ((float) timeseries_size / (float) paa_segments);
		float * paa = (float *) malloc(sizeof(float) * paa_segments);
        	my_paa_from_ts(qts, paa, paa_segments, ts_values_per_segment);
        
		//printf("\nzzzzzzzzzzzzzzzzzzzzzzzz@ %d", keyscnt);
		fflush(stdout);
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



		int currleaf=-1;
		char *indexfilename;

		int counti=0;
                for(counti=0;counti<ts_num;counti++)
		{
                	
			
			if(MINDISTS[counti]<=mindist)
			{
				if(currleaf==-1)
				{
					currleaf=counti/LEAF_SIZE;
	        			indexfilename = (char *)malloc(255);
					snprintf(indexfilename, 255, "/tmp/%s.%d.ordered.%d",basename(dataset),ts_num, currleaf);
					
					//printf("\n Searching Leafno: %d filename:%s\n", currleaf, indexfilename);
					indexfp = fopen (indexfilename,"r");
				}
				else 
				{
					if (currleaf != (counti/LEAF_SIZE))
					{
						fclose(indexfp);
						currleaf=counti/LEAF_SIZE;
	                                        indexfilename = (char *)malloc(255);
        	                                snprintf(indexfilename, 255, "/tmp/%s.%d.ordered.%d",basename(dataset),ts_num, currleaf);
						//printf("\n Searching Leafno: %d filename:%s\n", currleaf, indexfilename);
                	                        indexfp = fopen (indexfilename,"r");
					}
				}
                        
				
				nodesvisited++;
				fseek(indexfp, (counti%LEAF_SIZE)*((sizeof(float)*timeseries_size)+(sizeof(unsigned char)*paa_segments)), SEEK_SET);
			
				fread(tmpsax, sizeof(char), paa_segments, indexfp);
				fread(mqts, sizeof(float), timeseries_size, indexfp);
			



				//printf("\n%d\n", counti);
				
				//invSax(invsax,keys[counti].sax, paa_segments ,sax_bit_cardinality);
						
				//invPrint(invsax, paa_segments ,sax_bit_cardinality);
				//invPrint(tmpsax, paa_segments ,sax_bit_cardinality);
                               

				dist = my_ts_euclidean_distance((float *) qts,(float *) mqts , lenz, mindist);
                                if(dist<mindist)
					mindist=dist;
			}
		}

		if(currleaf!=-1)
			fclose(indexfp);

		/*
		int mleafno=0;
		while(mleafno<(ts_num/LEAF_SIZE))
		{
        		
			snprintf(indexfilename, 255, "/tmp/%s.%d.ordered.%d",basename(dataset),ts_num, mleafno);
			//printf("\n Searching Leafno: %d filename:%s\n", mleafno, indexfilename);
			indexfp = fopen (indexfilename,"r");
        		while(fread(invsax, sizeof(char), paa_segments, indexfp)>=paa_segments)
			{
        			fread(ts, sizeof(float), timeseries_size, indexfp);
				counti++;
			}
			mleafno++;
		}*/

		
		free(MINDISTS);
		free(paa);
		free(max_sax_cardinalities);

		avdist+=mindist;
	}//end if exact==1
        //printf("\nResult:%f\n", mindist);
	duration += timer_end(&start);

	free(qts);
	free(mqts);

}//end query

	free(keys);
        
	printf("\n Indexing 	%f seconds: ", durationsort);
	printf("\n Querying 	%f seconds: ", duration);
	printf("\n Avg approx 	%f ", avdistapprox);
	printf("\n Avg exact 	%f ", avdist);
	printf("\n Records visited  %d \n", nodesvisited);

/*		

		int counti;
		int zfl=0;
                for(counti=0;counti<ts_num;counti++)
                {

                                        if(MINDISTS[counti]<=mindist)
                                        {
                                                //fseek(fp, counti*(sizeof(float)*timeseries_size), SEEK_SET);
                                                //fread(ts, sizeof(float), timeseries_size, fp);
						
                       				
						myheat[counti%10]++; 
						
						myvisitedleafs[counti/2000]++;

                        
						nodesvisited++;
						//dist = my_ts_euclidean_distance((float *) qts,(float *) ts , 256, mindist);
	   					key.data=currqueue[counti].invsax;
	   					key.size=sizeof(unsigned char)*paa_segments;
						dbcp->get(dbcp, &key, &data, DB_SET);
                                                dist = my_ts_euclidean_distance((float *) qts,(float *) data.data , 256, mindist);
                                                if(dist<mindist)
                                                {
							mindist=dist;
							memcpy(mqts,data.data, sizeof(float) * timeseries_size);

							tempexactleaf=counti/2000;
							//memcpy(mqts,ts, sizeof(float) * timeseries_size);
                                                        //printf("\nDistance:%f",dist);
                                                        //ts_print((float *)data.data, 256);
                                                }

                                        }
        	}//end for
	
                        //printf("\nDistance:%f\n",mindist); 
               	
		if(cnt!=0)
			printf(",");
		//ts_print((float *)mqts, 256);
		fflush(stdout);


                 free(MINDISTS);




	}//end else db->get
		
		int zleafs=0;
		for(heatno=0;heatno<ts_num/2000;heatno++)
			if(myvisitedleafs[heatno]>0)
			{
				zleafs++;
				if(tempexactleaf==heatno)
					exactleafs+=zleafs;
			}

		
		maxleafs=maxleafs+zleafs;

                //ts_print((float *)qts, 256);
		//printf("\nDist:%f\n", mindist);
                //ts_print((float *)mqts, 256);
		//printf("\nDist:%f\n", mindist);
		avdist+=mindist;
}//end if exact==1
	

		//ts_print(ts, 8);

	free(qts);
	free(mqts);
}

	printf("]},");fflush(stdout);

	printf("\n	{\"records\": \"[");fflush(stdout);
	for(heatno=0; heatno<10;heatno++)
		if(heatno==0)
		{
		printf("%d",myheat[heatno]);fflush(stdout);;
		}
		else
		{
		printf(",%d",myheat[heatno]);fflush(stdout);
		}

        //printf("\n[STAT] Querying %lld records",   ts_num);fflush(stdout);
        //printf(" in %f seconds: ", duration);
		printf("]},");fflush(stdout);
		printf("\n	{\"querying_time\": \"%f\"},",duration );fflush(stdout);

	if(queriesno>0)
	{
		printf("\n	{\"average_records_visited\": \"%d\"},",nodesvisited );fflush(stdout);
		printf("\n	{\"average_distance_approximate\": \"%f\"},",(avdistapprox/queriesno) );fflush(stdout);
		printf("\n	{\"average_distance_exact\": \"%f\"}",(avdist/queriesno) );fflush(stdout);
		printf("\n	{\"exact_leafs\": \"%d\"}",exactleafs);fflush(stdout);
		printf("\n	{\"max_leafs\": \"%d\"}",maxleafs );fflush(stdout);




		//printf("\nNodesVisited:%d", (nodesvisited/queriesno));
		//printf("\nAvdistapprox:%f", (avdistapprox/queriesno));
		//printf("\nAvdistexact :%f\n", (avdist/queriesno));
	}

        int ts_loaded=0;
        while (ts_loaded<ts_num)
        {
	 	free(currqueue[ts_loaded].invsax);
 		if(EXACT==1)
	    		free(currqueue[ts_loaded].sax);
                ts_loaded++;
        }


*/

	(void)fclose(qfp);
	//printf("\n}\n\n");fflush(stdout);
	free(ts);
	free(sax);
	free(tmpsax);
	free(invsax);

//	free(currqueue);


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
