This Repo is a simulation of a Simple C++ Order Management System that needs to store the state as part of its memory. This repo is currently a work in progress. Currently this can only compare `boost::multi_index` containers vs `sqlite` in-memory tables.   

Sample output
```
build$ ./src/app/omscompare -h
Usage: omscompare [--help] [--version] [--boost_ordered] [--boost_hashed] [--sqlite] --count VAR [--inner_count VAR] [--order_route] [--order_route_fill] [--order_multi_route_fill] [--order_multi_route_multi_fill]

Optional arguments:
  -h, --help                          shows help message and exits
  -v, --version                       prints version information and exits
  -b, --boost_ordered                 Use boost_ordered for storage
  -d, --boost_hashed                  Use boost_hashed for storage
  -s, --sqlite                        Use sqlite for storage
  -c, --count                         Number of runs per workflow [required]
  -i, --inner_count                   Number of routes or fills per each iteration per workflow [nargs=0..1] [default: 256]
  -o, --order_route                   Creates order_route per workflow
  -r, --order_route_fill              Creates order_route_fill per workflow
  -m, --order_multi_route_fill        Creates order_multi_route_fill per workflow
  -f, --order_multi_route_multi_fill  Creates order_multi_route_multi_fill per workflow

build$ ./src/app/omscompare -b -d -s -f -c 1000 -i 50
Running workflow 'order_multi_route_multi_fill' for 1000 times
        Start run for 'boost_ordered' container storage type
2602000 operations took 31.5091 secs.
99.8670% operations took an avg of 1.1789 msecs
0.0973% operations took an avg of 12.7030 msecs
0.0185% operations took an avg of 26.2599 msecs
0.0143% operations took an avg of 93.3306 msecs
0.0029% operations took an avg of 105.0789 msecs
126 was the max time for any event in this run
        Start run for 'boost_hashed' container storage type
2602000 operations took 28.4543 secs.
99.9359% operations took an avg of 0.9086 msecs
0.0491% operations took an avg of 12.5560 msecs
0.0124% operations took an avg of 25.5015 msecs
0.0012% operations took an avg of 47.7500 msecs
0.0008% operations took an avg of 112.5000 msecs
0.0007% operations took an avg of 26742.0000 msecs
230001 was the max time for any event in this run
        Start run for 'sqlite' container storage type
2602000 operations took 239.7030 secs.
79.7245% operations took an avg of 7.1795 msecs
19.1056% operations took an avg of 12.3736 msecs
0.8712% operations took an avg of 23.7983 msecs
0.1040% operations took an avg of 55.3690 msecs
0.0551% operations took an avg of 141.8703 msecs
0.1395% operations took an avg of 559.8769 msecs
6633 was the max time for any event in this run

```
