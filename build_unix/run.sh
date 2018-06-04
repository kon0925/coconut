make coconut



#COCONUT+
# example ./coconut_plus size memsize exact(0/1) number_of_queries indexing(0/1) leaf_size dataset_file queries_file ts_size frequency



date
#./coconut_plus 100000 10000 1 10 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 0
#./coconut_plus 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 0
#./coconut_plus 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 0
#./coconut_plus 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1

#./coconut_plus 100000000 40000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 0


#free && sync && echo 3 > /proc/sys/vm/drop_caches && free
#./coconut_plus 100000000 30000000 1 100 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1 
#free && sync && echo 3 > /proc/sys/vm/drop_caches && free
#./coconut_plus 100000000 30000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1 
#free && sync && echo 3 > /proc/sys/vm/drop_caches && free
#./coconut_plus 100000000 30000000 1 100 1 20000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1 

free && sync && echo 3 > /proc/sys/vm/drop_caches && free
./coconut_plus 40000000 10000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1 

#./coconut_plus 40000000 10000000 1 100 1 20000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1 
#./coconut_plus 40000000 10000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1 
#./coconut_plus 40000000 10000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1 
#./coconut_plus 40000000 10000000 1 100 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1 
#./coconut_plus 40000000 10000000 1 100 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1 


#./coconut 40000000 10000000 1 100 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  
#./coconut 40000000 10000000 1 100 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  
#./coconut 40000000 10000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  
#./coconut 40000000 10000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  
#./coconut 40000000 10000000 1 100 1 20000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  
#./coconut 40000000 10000000 1 100 1 20000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  
#./coconut 40000000 10000000 1 100 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  
#./coconut 40000000 10000000 1 100 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  
#./coconut 40000000 10000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  
#./coconut 40000000 10000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  
#./coconut 40000000 10000000 1 100 1 20000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  
#./coconut 40000000 10000000 1 100 1 20000 ../../../data_1T_new.bin ../../../Queries_new.bin 256  



#./coconut_plus 60000000 40000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1
#./coconut_plus 40000000 40000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1
#./coconut_plus 20000000 40000000 1 100 1 2000 ../../../data_1T_new.bin ../../../Queries_new.bin 256 1

#./coconut_plus 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 6000000 70000000

#./coconut_plus 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 10000000 50000000

#./coconut_plus 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 14000000 30000000

#./coconut_plus 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 18000000 10000000



#./coconut_plus 100000000 30000000 1 0 1 2000 ../../../seismic.data.bin ../../../xab 256 1 100000000
#mv coconut_plus.db seismic_plus.db
#date
#./coconut 100000000 30000000 1 0 1 2000 ../../../seismic.data.bin ../../../xab 256 1 100000000
#mv coconut.db seismic.db

#date
#./coconut_plus 277000000 30000000 1 0 1 2000 ../../../astronomy.bin ../../../xab 256 1 277000000
#mv coconut_plus.db astro_plus.db
#date
#./coconut 277000000 30000000 1 0 1 2000 ../../../astronomy.bin ../../../xab 256 1 277000000
#mv coconut.db astro.db

#./coconut_plus 100000000 100000 1 500 1 2000 ../../../data_1T_new.bin ../../../xab 256 1000000 50000000
#./coconut_plus 100000000 100000 1 500 1 2000 ../../../data_1T_new.bin ../../../xab 256 600000 70000000
#./coconut_plus 100000000 100000 1 500 1 2000 ../../../data_1T_new.bin ../../../xab 256 200000 90000000

#./coconut_plus 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 1800000 10000000



#./coconut 100000 10000 1 0 1 2000 ../../../data_1T_new.bin ../../../xab 256 2000 90000

#./coconut 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 2000000 90000000

#./coconut 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 6000000 70000000

#./coconut 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 10000000 50000000

#./coconut 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 14000000 30000000

#./coconut 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 18000000 10000000

#./coconut 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 200000 90000000

#./coconut 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 600000 70000000

#./coconut 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 1000000 50000000

#./coconut 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 1400000 30000000

#./coconut 100000000 100000 1 100 1 2000 ../../../data_1T_new.bin ../../../xab 256 1800000 10000000










#./coconut_plus 100000000 100000 1 1000 1 2000 ../../../data_1T_new.bin ../../../xab 256 10000000

#./coconut_plus 100000000 100000 1 1000 1 2000 ../../../data_1T_new.bin ../../../xab 256 1000000
#./coconut_plus 100000000 100000 1 1000 1 2000 ../../../data_1T_new.bin ../../../xab 256 1000000
#./coconut_plus 100000000 100000 1 1000 1 2000 ../../../data_1T_new.bin ../../../xab 256 100000
#./coconut_plus 100000000 100000 1 1000 1 2000 ../../../data_1T_new.bin ../../../xab 256 100000

#./coconut 100000000 100000 1 1000 1 2000 ../../../data_1T_new.bin ../../../xab 256 10000000
#./coconut 100000000 100000 1 1000 1 2000 ../../../data_1T_new.bin ../../../xab 256 1000000
#./coconut 100000000 100000 1 1000 1 2000 ../../../data_1T_new.bin ../../../xab 256 100000



#./coconut_plus 10000 10000 1 1000 1 2000 ../../../data_1T_new.bin ../../../xab 256 100
#./coconut 100000000 100000 1 100000 1 2000 ../../../data_1T_new.bin ../../../xab 256



#./coconut 100 10 0 0 1 2000 ../../../data_128_100M.txt.bin ../../../data_128_100M_queries.txt.bin 128
#rm *.db
#./coconut 100000000 100000 0 0 1 2000 ../../../data_128_100M.txt.bin ../../../data_128_100M_queries.txt.bin 128
#rm *.db
#./coconut 100000000 100000 0 0 1 2000 ../../../data_128_100M.txt.bin ../../../data_128_100M_queries.txt.bin 128
#rm *.db
#./coconut 100000000 100000 0 0 1 2000 ../../../data_256_100M.txt.bin ../../../data_256_100M_queries.txt.bin 256 
#rm *.db
#./coconut 100000000 100000 0 0 1 2000 ../../../data_256_100M.txt.bin ../../../data_256_100M_queries.txt.bin 256 
#./coconut 100000000 100000 0 0 1 2000 ../../../data_512_100M.txt.bin ../../../data_512_100M_queries.txt.bin 512 
#rm *.db
#./coconut 100000000 100000 0 0 1 2000 ../../../data_512_100M.txt.bin ../../../data_512_100M_queries.txt.bin 512 
#rm *.db
#./coconut 100000000 100000 0 0 1 2000 ../../../data_1024_100M.txt.bin ../../../data_1024_100M_queries.txt.bin 1024
#rm *.db
#./coconut 100000000 100000 0 0 1 2000 ../../../data_1024_100M.txt.bin ../../../data_1024_100M_queries.txt.bin 1024 



#COCONUT
#./ex_bulk 100000000 100000 0 0 1 1000 ../../../data_1024_100M.txt.bin ../../../data_1024_100M_queries.txt.bin 1024
#./ex_bulk 100000000 100000 0 0 1 1000 ../../../data_512_100M.txt.bin ../../../data_512_100M_queries.txt.bin 512
#./ex_bulk 100000000 100000 0 0 1 1000 ../../../data_256_100M.txt.bin ../../../data_256_100M_queries.txt.bin 256
#./ex_bulk 100000000 100000 0 0 1 1000 ../../../data_128_100M.txt.bin ../../../data_128_100M_queries.txt.bin 128



#COCONUT+
#./ex_btrec 270000000 200000 0 0 1 1000 ../../../DATA/astronomy.bin ../../../astronomy.queries.bin
#./ex_btrec 270000000 20000 1 0 1 1000 ../../../DATA/astronomy.bin ../../../astronomy.queries.bin

#./ex_bulk 10000000 10000000 0 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin 
#./ex_btrec 1000000000 8000000 0 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin 
#./ex_btrec 100000000 10000 0 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin 

#./ex_btrec 100000000 40000000 0 0 1 1000 ../../../DATA/seismic.data.bin ../../../DATA/seismic.queries.bin
#./ex_btrec 100000000 100000 1 0 1 1000 ../../../DATA/seismic.data.bin ../../../DATA/seismic.queries.bin
#./ex_btrec 100000000 10000 1 0 1 1000 ../../../DATA/seismic.data.bin ../../../DATA/seismic.queries.bin


#COCONUT
#./ex_bulk 1000000000 8000000 0 0 1 1000 ../../../data_1T_new.1000000000.ordere ../../../Queries_new.bin 
#./ex_bulk 100000000 10000 0 0 1 1000 ../../../data_1T_new.100000000.ordere ../../../Queries_new.bin 

#./ex_bulk 270000000 40000000 0 0 1 1000 ../../../astronomy.270000000.ordered ../../../astronomy.queries.bin
#./ex_bulk 270000000 200000 0 0 1 1000 ../../../astronomy.270000000.ordered ../../../astronomy.queries.bin
#./ex_bulk 270000000 20000 1 0 1 1000 ../../../astronomy.270000000.ordered ../../../astronomy.queries.bin

#./ex_bulk 100000000 40000000 0 0 0 1000 ../../../seismic.data.100000000.ordered ../../../DATA/seismic.queries.bin
#./ex_bulk 100000000 100000 1 0 1 1000 ../../../seismic.data.100000000.ordered ../../../DATA/seismic.queries.bin
#./ex_bulk 100000000 10000 1 0 1 1000 ../../../seismic.data.100000000.ordered ../../../DATA/seismic.queries.bin




#./ex_bulk 10000000 8000000 0 0 1 1000 ../../../data_1T_new.10000000.ordered ../../../Queries_new.bin 
#./ex_bulk 50000000 8000000 0 0 1 1000 ../../../data_1T_new.50000000.ordered ../../../Queries_new.bin 
#./ex_bulk 100000000 8000000 0 0 1 1000 ../../../data_1T_new.100000000.ordered ../../../Queries_new.bin 
#./ex_bulk 200000000 8000000 0 0 1 1000 ../../../data_1T_new.200000000.ordered ../../../Queries_new.bin 
#./ex_bulk 400000000 8000000 0 0 1 1000 ../../../data_1T_new.400000000.ordered ../../../Queries_new.bin 
#./ex_bulk 600000000 8000000 0 0 1 1000 ../../../data_1T_new.600000000.ordered ../../../Queries_new.bin 

#./ex_bulk 60000000 40000000 1 0 1 1000 ../../../data_1T_new.60000000.ordered ../../../Queries_new.bin 
#./ex_bulk 60000000 40000000 0 0 0 1000 ../../../data_1T_new.60000000.ordered ../../../Queries_new.bin 


#./ex_bulk 100000000 40000000 1 0 1 1000 ../../../data_1T_new.100000000.ordered ../../../Queries_new.bin 
#./ex_bulk 100000000 40000000 0 0 0 1000 ../../../data_1T_new.100000000.ordered ../../../Queries_new.bin 


#./ex_bulk 100000000 10000000 0 0 1 1000 ../../../data_1T_new.100000000.ordered ../../../Queries_new.bin 
#./ex_bulk 100000000 1000000 0 0 1 1000 ../../../data_1T_new.100000000.ordered ../../../Queries_new.bin 
#./ex_bulk 100000000 100000 0 0 1 1000 ../../../data_1T_new.100000000.ordered ../../../Queries_new.bin 
#./ex_bulk 100000000 10000 0 0 1 1000 ../../../data_1T_new.100000000.ordered ../../../Queries_new.bin 




########### COCONUT+ 
#./ex_btrec 40000000 40000000 0 0 0 500 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 40000000 40000000 1 0 0 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 40000000 40000000 0 0 0 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 40000000 40000000 1 0 0 10000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 40000000 40000000 0 0 0 10000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 40000000 40000000 1 0 0 2000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 40000000 40000000 0 0 0 2000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 40000000 40000000 1 0 0 4000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 40000000 40000000 0 0 0 4000 ../../../data_1T_new.bin ../../../Queries_new.bin

#./ex_btrec 60000000 60000000 1 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 60000000 60000000 0 0 0 1000 ../../../data_1T_new.bin ../../../Queries_new.bin

#./ex_btrec 10000000 10000000 1 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 10000000 10000000 0 0 0 1000 ../../../data_1T_new.bin ../../../Queries_new.bin

#./ex_btrec 100000000 100000000 1 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 100000000 100000000 0 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 100000000 50000000 0 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 100000000 500000 1 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 100000000 500000 0 0 0 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 200000000 100000000 1 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 200000000 100000000 0 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 400000000 100000000 1 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 400000000 100000000 0 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 600000000 100000000 1 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin
#./ex_btrec 600000000 100000000 0 0 1 1000 ../../../data_1T_new.bin ../../../Queries_new.bin

#./ex_btrec 270000000 100000000 0 0 1 1000 ../../../DATA/astronomy.bin ../../../astronomy.queries.bin
#./ex_btrec 270000000 10000000 1 0 1 1000 ../../../DATA/astronomy.bin ../../../astronomy.queries.bin
#./ex_btrec 100000000 100000000 0 0 1 1000 ../../../DATA/seismic.data.bin ../../../DATA/seismic.queries.bin
#./ex_btrec 100000000 50000000 1 0 1 1000 ../../../DATA/seismic.data.bin ../../../DATA/seismic.queries.bin

#valgrind --leak-check=yes -v 
#./ex_btrec 10000000 10000000 0 0 1 1000 ../../../data_1T_new.bin 
#./ex_btrec 50000000 50000000 0 0 1 1000 ../../../data_1T_new.bin 
#./ex_btrec 100000000 100000000 0 0 1 1000 ../../../data_1T_new.bin 
#./ex_btrec 200000000 200000000 0 0 1 1000 ../../../data_1T_new.bin 
#./ex_btrec 400000000 400000000 0 0 1 1000 ../../../data_1T_new.bin 
#./ex_btrec 600000000 600000000 0 0 1 1000 ../../../data_1T_new.bin 

########### COCONUT FULL
#./ex_bulk 60000000 40000000 0 0 1 1000 ../../../data_1T_new.bin.60000000.ordered 
#./ex_bulk 10000000 8000000 0 0 1 1000 ../../../data_1T_new.bin.10000000.ordered 
#./ex_bulk 50000000 8000000 0 0 1 1000 ../../../data_1T_new.bin.50000000.ordered
#./ex_bulk 100000000 8000000 0 0 1 1000 ../../../data_1T_new.bin.100000000.ordered 
#./ex_bulk 200000000 8000000 0 0 1 1000 ../../../data_1T_new.bin.200000000.ordered
#./ex_bulk 400000000 8000000 0 0 1 1000 ../../../data_1T_new.bin.400000000.ordered 
#./ex_bulk 600000000 8000000 0 0 1 1000 ../../../data_1T_new.bin.600000000.ordered 

