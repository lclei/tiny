#include "mynet.h"
typedef struct{
	int maxfd;
	fd_set read_set;
	fd_set ready_set;
	int nready;
	int maxi;
	int clientfd[FD_SETSIZE];
}pool;
void add_client(int connfd, pool *p);
void check_clients(pool *p);
void init_pool(int listenfd, pool *p);
int open_listenfd(char *port);

int byte_cnt = 0;

int main(int argc, char **argv)
{
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];
	static pool pool;

	if(argc!=2){
	  fprintf(stderr,"usage: %s <port>\n",argv[0]);
	  exit(0);
	}
	listenfd = open_listenfd(argv[1]);
	init_pool(listenfd,&pool);

	while(1){
		pool.ready_set = pool.read_set;
		pool.nready = select(pool.maxfd+1,&pool.ready_set,NULL,NULL,NULL);
		if(FD_ISSET(listenfd,&pool.ready_set))
		{
			clientlen = sizeof(struct sockaddr_storage);
			connfd = accept(listenfd, (SA*)&clientaddr,&clientlen);
			getnameinfo((SA*)&clientaddr,clientlen,client_hostname,MAXLINE,client_port,MAXLINE,0);
			printf("%s:%s connected!\n",client_hostname,client_port);
			add_client(connfd, &pool);
		}
		check_clients(&pool);	
	}
	exit(0);
}

void init_pool(int listenfd, pool *p){
	int i;
	p->maxi = -1;
	for(i=0;i<FD_SETSIZE;i++)
	  p->clientfd[i] = -1;

	p->maxfd = listenfd;
	FD_ZERO(&p -> read_set);
	FD_SET(listenfd, &p->read_set);
}

void add_client(int connfd, pool *p){
	int i;
	p->nready--;
	for(i=0;i<FD_SETSIZE;i++){
		if(p->clientfd[i]<0){
			p->clientfd[i] = connfd;
			FD_SET(connfd,&p->read_set);
			if(connfd,&p->maxfd)
			  p->maxfd = connfd;
			if(i>p->maxi)
			  p->maxi = i;
			break;
		}
		if(i == FD_SETSIZE)
		  printf("Add client error! Beyond max client numbers.\n");
	}
}

void check_clients(pool *p)
{
	int i,connfd,n;
	char buf[MAXLINE];
	char echostr[MAXLINE+10];

	for(i=0;(i<=p->maxi)&&(p->nready>0);i++){
		connfd = p->clientfd[i];
		sprintf(buf,"\0");
	
		if((connfd>0)&&(FD_ISSET(connfd,&p->ready_set))){
			p->nready--;
			if((n=read(connfd,buf,MAXLINE))!=0){
				byte_cnt+=n;
				printf("Server recived %d (%d total) bytes on fd %d\n",n,byte_cnt,connfd);
				write(connfd,buf,n);
			}
			else{
				printf("Close connfd %d\n",connfd);
				close(connfd);
				FD_CLR(connfd,&p->read_set);
				p->clientfd[i] = -1;
			}
		}
	}
}

