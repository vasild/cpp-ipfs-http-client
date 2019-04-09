#!/bin/bash
# Written by Wang Yi (<wangyi@storswift.com>), Wan Lei (<wanlei@storswift.com>), and Leng Bo (<lengbo@storswift.com>).

# Uncomment the two lines to use the fixed UID
# HIVE_UID=uid-ac27da17-5649-497e-a8a8-5f022b7f8f68
# HIVE_PEERID=QmNTWTjUfe1AXkMpeTf47taXCWrfC6GhThmMCXmTgJybg5


if [ $# -lt 2 ]; then
    echo "Please set the IP addresses of two servers!"
    echo " $0 SERVER1 SERVER2"
    echo "Example:"
    echo " $0 192.168.0.2 192.168.0.3"
    echo "    # It will test send key-value to 192.168.0.2, and read this key-value from 192.168.0.3."
    exit 1
fi

HIVE_HOST1=$1
HIVE_HOST2=$2

if [ "${HIVE_UID}" = "" ] && [ "${HIVE_PEERID}" = "" ]; then
    temp=`curl http://${HIVE_HOST1}:9095/api/v0/uid/new 2>/dev/null` || show_error "Can not new a UID on ${HIVE_HOST1}"
    HIVE_UID=`echo ${temp} | jq .UID | sed s/\"//g`
    HIVE_PEERID=`echo ${temp} | jq .PeerID | sed s/\"//g`
fi

logfile=SERVER-$1-`date "+%Y-%m-%d_%H:%M"`.log
statisticsfile=SERVER-$1-`date "+%Y-%m-%d_%H:%M"`.sta

function show_error()
{
    logs "[Error] $1"
    exit 2
}

function show_steps()
{
    echo
    echo "************************************************************"
    echo $1
    echo "************************************************************"
}

function logs_time()
{
    start=$2
    end=$3
    start_s=$(echo ${start} | cut -d '.' -f 1)
    start_ns=$(echo ${start} | cut -d '.' -f 2)
    end_s=$(echo ${end} | cut -d '.' -f 1)
    end_ns=$(echo ${end} | cut -d '.' -f 2)
    time_ns=$(( ( 10#${end_s} - 10#${start_s} ) * 1000 + ( 10#${end_ns} / 1000000 - 10#${start_ns} / 1000000 ) ))
    echo "[$1] Begin:`date -d @${start_s} +%H:%M:%S`, End:`date -d @${end_s} +%H:%M:%S`. Last ${time_ns} ms" | tee -a $logfile $statisticsfile
}

function logs()
{
    echo [ `date "+%Y-%m-%d %H:%M:%S"`] $1 >>$logfile
}


# send message / add k-v
# ussage: ./cpp-test/dstore_test send <host> <uid> <key> <value>

function msg_send {
    host=$1
    uid=$2
    key=$3
    value=$4
    # echo dstore_test send ${host} ${uid} ${key} ${value}
    dstore_test send ${host} ${uid} ${key} ${value}
}

# read message / read value by key
# usage: ./cpp-test/dstore_test read <host> <peerId> <key>

function msg_read {
    host=$1
    peerid=$2
    key=$3
    # echo dstore_test read ${host} ${peerid} ${key}
    dstore_test read ${host} ${peerid} ${key}
}

# remove messsage / remove k and its values
# usage: ./cpp-test/dstore_test remove <host> <uid> <key>
function msg_remove {
    host=$1
    uid=$2
    key=$3
    # echo dstore_test remove ${host} ${uid} ${key}
    dstore_test remove ${host} ${uid} ${key}
}


echo "===============================================================================================" | tee -a $logfile $statisticsfile
echo "******************** Send key-value to ${HIVE_HOST1}. Read from ${HIVE_HOST2}*****************"  | tee -a $logfile $statisticsfile
echo "===============================================================================================" | tee -a $logfile $statisticsfile
echo 
echo HIVE_UID: $HIVE_UID
echo HIVE_PEERID: $HIVE_PEERID
logs "[Info] Begin to  send key-value to ${HIVE_HOST1}"
time_start_send=`date +%s.%N`
msg_send ${HIVE_HOST1} ${HIVE_UID} "test-key-${HIVE_HOST2}" "test-key-${HIVE_HOST2}-item001"  || show_error "Can not send key-value to ${HIVE_HOST1}"
# sleep 3
time_end_send=`date +%s.%N`
logs "[Info] Now send key-value to ${HIVE_HOST1} successfully"
i=1
res=""
while :
do
    res=`msg_read ${HIVE_HOST2} ${HIVE_PEERID} "test-key-${HIVE_HOST2}"`
    res3=`echo $res | awk '{print $7}'`
    if [ x"$res3" == x"test-key-${HIVE_HOST2}-item001" ];then
        time_end_read=`date +%s.%N`
        logs "[Info] Now read key-value from ${HIVE_HOST2} successfully" 
    break;
    fi
    echo $i,
    ((i++))
done
msg_remove ${HIVE_HOST1} ${HIVE_UID} "test-key-${HIVE_HOST2}"
logs_time Send_K-V_To_${HIVE_HOST1} ${time_start_send} ${time_end_send}
logs_time Read_K-V_From_${HIVE_HOST2} ${time_end_send} ${time_end_read}
echo "" >>$statisticsfile 


echo "Test done!"
