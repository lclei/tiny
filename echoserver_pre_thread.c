#include "mynet.h"
#include "sbuf.h"
#include "echo_cnt.h"
#define NTHREADS 4
#define SBUFSIZE 16

int open_listenfd(char *port);
void *thread(void *vargp);

sbuf_t sbuf;

int main(int argc, char **argv)
{
	int listenfd, connfd, i;
	socklen_t clientlen;
	pthread_t tid;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];

	if(argc!=2){
	  fprintf(stderr,"usage: %s <port>\n",argv[0]);
	  exit(0);
	}
	listenfd = open_listenfd(argv[1]);
	sbuf_init(&sbuf,SBUFSIZE);
	for(i = 0;i<NTHREADS;i++){
		pthread_create(&tid,NULL,thread,NULL);
	}
	while(1){
		clientlen = sizeof(struct sockaddr_storage);
		connfd = accept(listenfd, (SA*)&clientaddr,&clientlen);
        getnameinfo((SA*)&clientaddr,clientlen,client_hostname,MAXLINE,client_port,MAXLINE,0);
		sbuf_insert(&sbuf,connfd);
		printf("client[%s:%s] connected! connfd %d!\n",
					client_hostname,client_port,(int)connfd);
	}
	exit(0);
}

void *thread(void* vargp){
	pthread_detach(pthread_self());
	while(1){
		int connfd = sbuf_remove(&sbuf);
		echo_cnt(connfd);
	    close(connfd);
	}
	return NULL;
}


