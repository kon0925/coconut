make coconut
date

#COCONUT+
# example coconut_plus
#/coconut_plus datasize(recordsnum) memsize(recordsnum) exact(0/1) number_of_queries indexing(1 this is the only parameter that workks currently) leaf_size(recordsnum) dataset_file queries_file ts_size frequency

# example coconut
#/coconut datasize(recordsnum) memsize(recordsnum) exact(0/1) number_of_queries indexing(0=only_querying/1=only_indexing/2=both_indexing_and_querying) leaf_size(recordsnum) dataset_file queries_file ts_size frequency


#free && sync && echo 3 > /proc/sys/vm/drop_caches && free
./coconut 400000 100000 1 100 2 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1 


