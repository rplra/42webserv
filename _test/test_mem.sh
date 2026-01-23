#!/bin/bash

# [General]
# run this during start process & end process to see if there is spike in CPU & MEM
#
# prerequisites:
# 1. run `./webserv ./conf/siege.conf`
# 2. in separate window, run `bash test_siege.sh`
# 3. in separate window, run `bash test_mem.sh`
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# METHOD 1:
# using ps aux to get processes memory
# -----------------------------------------------------------------------
# print headers
# ps aux | head -n 1

# i=0
# # repeat set to 1st param, else default 30x
# repeat=${1:-30}

# # loop
# while [ $i -lt $repeat ]; do
# 	ps aux | grep [w]ebserv
# 	sleep 1
# 	i=$((i+1))
# done

# -----------------------------------------------------------------------
# METHOD 2:
# use 'top -o MEM'
# -----------------------------------------------------------------------
pid=$(pgrep webserv)
top -o MEM | awk -v pid="$pid" '$1 == pid { print "PID", $1, "MEM", $8 }'

# get memory percentage:
# ps aux | awk 'NR==1 || /webserv/ {print $1,$2,$3,$4,$8,$9,$10,$11}'

# 1. get webserv pid: `pgrep webserv`
# 2. run `top -o MEM` to see all processes
# 3. to get only webserv & memory (8th column), do:
#    top -o MEM | awk '$1 == <PID_NUMBER> {print "PID", $1, "MEM", $8}'