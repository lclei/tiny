#include "mynet.h"

void doit(int fd);
void read_requesthdrs(int fd);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char*cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, char** argv)
{
	int listenfd,connfd;
	char hostname[MAXLINE], port[MAXLINE];
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;

	if(argc!=2){
		fprintf(stderr, "usage: %s <port>\n",argv[0]);
		exit(1);
	}

	listenfd = open_listenfd(argv[1]);
	while(1){
		clientlen = sizeof(clientaddr);
		connfd = accept(listenfd,(SA*)&clientaddr,&clientlen);
		getnameinfo((SA*)&clientaddr,clientlen,hostname,MAXLINE,port,MAXLINE,0);
		printf("Accepted connection from (%s, %s)\n",hostname,port);
		doit(connfd);
		close(connfd);
	}
}


void doit(int fd){
	int is_static;
	struct stat sbuf;
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	char filename[MAXLINE], cgiargs[MAXLINE];

	read(fd, buf, MAXLINE);
	printf("Requset hears:\n");
	printf("%s",buf);
	sscanf(buf, "%s %s %s", method, uri, version);
	if(strcasecmp(method, "GET")){
		clienterror(fd, method, "501", "Not implementedd","Tiny does not implement this method");
		return;
	}
	read_requesthdrs(fd);
	is_static = parse_uri(uri,filename,cgiargs);
	if(stat(filename,&sbuf)<0){
		clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
		return;
	}

	if(is_static){
		if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)){
			clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
			return;
		}
		serve_static(fd,filename,sbuf.st_size);
	}
	else {
		if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)){
			clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
			return;
		}
		serve_dynamic(fd,filename,cgiargs);
	}
}

void read_requesthdrs(int fd){
	char buf[MAXLINE];

	read(fd,buf,MAXLINE);
	while(strcmp(buf, "\r\n")){
		read(fd,buf,MAXLINE);
		printf("%s",buf);
	}
	return;
}
int parse_uri(char *uri, char *filename, char *cgiargs){
	char *ptr;

	if(!strstr(uri, "cgi-bin")) {
		strcpy(cgiargs,"");
		strcpy(filename,".");
		strcat(filename,uri);
		if(uri[strlen(uri)-1] == '/')
		  strcat(filename,"home.html");
		return 1;
	}
	else{	//dynammic content
		ptr = index(uri,'?');
		if (ptr) {
			strcpy(cgiargs, ptr+1);
			*ptr = '\0';
		}
		else
		  strcpy(cgiargs, ptr+1);
		strcpy(filename,".");
		strcat(filename,uri);
		return 0;
	}
}


void serve_static(int fd, char *filename, int filesize){}
void get_filetype(char *filename, char *filetype){}
void serve_dynamic(int fd, char *filename, char *cgiargs){}
void clienterror(int fd, char*cause, char *errnum, char *shortmsg, char *longmsg){
	char buf[MAXLINE], body[100*MAXLINE];
	sprintf(body,"<html><title>Tiny Error</title>");
	sprintf(body,"%s<body bgcolor=""ffffff"">\r\n",body);
	sprintf(body,"%s%s: %s\r\n",body,errnum,shortmsg);
	sprintf(body,"%s<p>%s: %s\r\n",body,longmsg,cause);
	sprintf(body,"%s<hr><em>The Tiny Web servver</em>\r\n",body);

	sprintf(buf, "HTTP/1.0 %s %s\r\n",errnum,shortmsg);
	write(fd,buf,strlen(buf));
	sprintf(buf, "Content-type: text/html\r\n");
	write(fd,buf,strlen(buf));
	sprintf(buf, "Content-length: %d\r\n",(int)strlen(body));
	write(fd,buf,strlen(buf));
	write(fd,body,strlen(body));
}

