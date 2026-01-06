#!/bin/bash

# [General]
# run this during start process & end process to see if there is spike in CPU & MEM
#
# prerequisites:
# 1. run `./webserv ./conf/siege.conf`
# 2. in separate window, run `bash test_siege.sh`
# 3. in separate window, run `bash test_mem.sh`

# print headers
ps aux | head -n 1

i=0
# repeat set to 1st param, else default 30x
repeat=${1:-30}

# loop
while [ $i -lt $repeat ]; do
	ps aux | grep [w]ebserv
	sleep 1
	i=$((i+1))
done

# selected params only
# ps aux | awk 'NR==1 || /webserv/ {print $1,$2,$3,$4,$8,$9,$10,$11}'

# alternative use watch
# watch -n 1 "ps -o pid,rss,command | grep webserv"