This Repo is a simulation of a Simple C++ Order Management System that needs to store the state as part of its memory. This repo is currently a work in progress. Currently this can only compare `boost::multi_index` containers vs `sqlite` in-memory tables.   

Sample output
```
build$ ./src/app/omscompare -s -b -f -c 1000 -i 50
Running workflow 'order_multi_route_multi_fill' for 1000 times
        Start run for 'boost' container storage type
2602000 operations took 33.4477 secs.
2600067 operations took an avg of 1.0508 msecs
968 operations took an avg of 11.9659 msecs
335 operations took an avg of 26.6239 msecs
36 operations took an avg of 48.6944 msecs
572 operations took an avg of 117.087 msecs
22 operations took an avg of 23790 msecs
257508 was the max time for any event in this run
        Start run for 'sqlite' container storage type
2602000 operations took 254.775 secs.
2004908 operations took an avg of 7.35651 msecs
549689 operations took an avg of 12.689 msecs
38794 operations took an avg of 23.9157 msecs
3511 operations took an avg of 55.5067 msecs
1354 operations took an avg of 147.666 msecs
3744 operations took an avg of 649.226 msecs
4671 was the max time for any event in this run
```
