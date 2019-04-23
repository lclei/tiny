echoserver_pre_thread: echoserver_pre_thread.c echo_cnt.c mylib.c 
	gcc echoserver_pre_thread.c echo_cnt.c mylib.c sbuf.c -g -lpthread -o echoserver_pre_thread
echoserverthread: echoserverthread.c mylib.c 
	gcc echoserverthread.c mylib.c -lpthread -g -o echoserverthread
