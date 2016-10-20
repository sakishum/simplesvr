#!/usr/bin/env bash
#shell starter - developed by simplejia [2014/11/28]

this="$0"
while [ -h "$this" ]; do
    ls=`ls -ld "$this"`
    link=`expr "$ls" : '.*-> \(.*\)$'`
    if expr "$link" : '.*/.*' > /dev/null; then
        this="$link"
    else
        this=`dirname "$this"`/"$link"
    fi
done

# configuration variables
basedir=`dirname $this`
cd $basedir
basedir=`pwd`
basename=`basename $this`
procname=${basename%.*}

cmd="$basedir/$procname"

retval=0

# setting environment variables
PATH=".:$PATH"
export PATH

pid_cmd="ps -e opid oppid ocmd|grep -v grep|grep -v "$procname.sh"|grep $cmd|awk '{if (\$2 == 1 || \$2 == $$ || \$2 == $PPID) print \$1}'"

# start the server
start(){
    retval=0
    printf "Starting the server of $procname\n"

    local pid=`eval $pid_cmd`
    if test $pid; then
        printf 'Existing process: %d, Stop it first.\n' "$pid"
        retval=1
        return
    fi

    nohup $cmd >>$procname.log 2>&1 &
    if [ ! "$?" -eq 0 ] ; then
        printf 'The server could not started\n'
        retval=1
        return
    fi

	sleep 2; status
	if [ "$retval" -eq 0 ] ; then
        printf 'Done\n'
    else
        printf 'The server could not started\n'
        retval=1
    fi
}

# stop the server
stop(){
    retval=0
    printf "Stopping the server of $procname\n"

    local pid=`eval $pid_cmd`
    if test $pid; then
        printf "Sending the terminal signal to the process: %s\n" "$pid"
        kill -TERM "$pid"
        local c=0
        while true ; do
            sleep 0.1
            if kill -0 $pid > /dev/null 2>&1; then
                c=`expr $c + 1`
                if [ "$c" -ge 100 ] ; then
                    printf 'Hanging process: %d\n' "$pid"
                    retval=1
                    break
                fi
            else
                printf 'Done\n'
                break
            fi
        done
    else
        printf 'No process found\n'
        retval=1
    fi
}


# get status of the server
status(){
    retval=0
    printf "Get status of the server of $procname\n"

    local pid=`eval $pid_cmd`
    if test $pid; then
        printf 'Process running: %d\n' "$pid"
    else
        printf 'No process found\n'
        retval=1
    fi
}

# check if alive
check(){
    retval=0
    status
    if [ $retval -eq 1 ];then
        stop
        start
    fi
}

# dispatch the command
case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    restart)
        stop
        start
        ;;
    status)
        status
        ;;
    check)
        check
        ;;        
    *)
        printf 'Usage: %s {start|stop|restart|status|check}\n' "$0"
        exit 1
        ;;
esac


# exit
exit "$retval"



# END OF FILE
