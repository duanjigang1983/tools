#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int init_tcp_sock(int	aMode, const char *aAddr, unsigned short  aPort)
/********************************************************
*				Initial  TCP  socket					*
*	IN:	aAddr:	local bind address						*
*		aPort:	listen port number						*
*		aMode:	s:	open as server						*
*				c:	open as client						*
*	Return:		<0:	open  error							*
*				>0:	success								*
*********************************************************/
{
    	struct sockaddr_in addr;
    	int 				on;
    	int 				sock_fd;
	int					status;
	int					err_n,bsize;
	socklen_t n;
	char szaddr[20] = {0};
	struct hostent * host = gethostbyname(aAddr);
	if (host)
	{
		sprintf(szaddr, "%s", inet_ntoa(*((struct in_addr *)host->h_addr)));
        }else
	{
		printf ("get host ip of '%s' failed\n", aAddr);
		return -1;
	}

    	sock_fd = socket(AF_INET,SOCK_STREAM,0);
    	if  (sock_fd < 0)
    	{   
		perror("socket_create::");
		sock_fd = -1;
        	goto    ret;
    	}

	on = 1;
	setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(int) );

	if	(aMode == 's')
	{
		bzero((char *)&addr,sizeof(struct sockaddr_in));
		addr.sin_family 	  = AF_INET;
		addr.sin_addr.s_addr  = inet_addr(szaddr);
		addr.sin_port 		  = htons(aPort);

		status = bind(sock_fd,(struct sockaddr *)&addr,sizeof(addr));
		if	(status < 0)
		{
			printf("COMM_ERR::addr = [%s]\n",aAddr);
			perror("bind error::");
			close(sock_fd);
			sock_fd = -1;
			goto ret;
		}

	//  ******  ижии???o3???      ******************
    	err_n = getsockopt(sock_fd,SOL_SOCKET,SO_SNDBUF,(char*)&bsize,&n);
    	bsize = 10000000;
    	err_n = setsockopt(sock_fd,SOL_SOCKET,SO_SNDBUF,
    	                    (char*)&bsize,sizeof(bsize));
    	if  (err_n < 0)
    	{
    	    printf("rsock error4(WSAIoctl)\n");
    	    perror("SET_BUF::");
    	}
    	err_n = getsockopt(sock_fd,SOL_SOCKET,SO_SNDBUF,
                        (char*)&bsize,&n);

		status = listen(sock_fd,5 );
		if	(status < 0)
		{
			perror("listen error::");
			close(sock_fd);
			sock_fd = -1;
			goto ret;
		}
	} 

    ret:
    return sock_fd;
}

int	tcp_accept(int	fd,int *aIp,int *aPort)
/********************************************************
*				Accept a TCP connect					*
*	IN:	fd:		TCP socket								*
*	OUT:		aIP	 :ip address						*
*				aPort:port number						*
*	Return:		<0:	error								*
*				>0:	accept a connect					*
*********************************************************/
{
	int					status;
	struct sockaddr_in 	addr;
	socklen_t					len;
	int					val;
	int					err_n,bsize;
	socklen_t n;	

	len = sizeof(addr);
	status = accept(fd,(struct sockaddr *)&addr,&len);
	if	(status < 0)
	{
		status = -1;
		perror("accept::");
	}
	else
	{
		*aIp 	= addr.sin_addr.s_addr;
		*aPort	= addr.sin_port;

    	val  = fcntl(status,F_GETFL,0);
    	val |= O_NONBLOCK;
    	val  = fcntl(status, F_SETFL, val);
    	if  (val < 0)
			perror("fcntl F_SETFL, O_NONBLOCK");

		//  ******  ижии???o3???      ******************
    	err_n = getsockopt(fd,SOL_SOCKET,SO_SNDBUF,(char*)&bsize,&n);
    	bsize = 10000000;
    	err_n = setsockopt(fd,SOL_SOCKET,SO_SNDBUF,
    	                    (char*)&bsize,sizeof(bsize));
    	if  (err_n < 0)
    	{
    	    printf("rsock error4(WSAIoctl)\n");
    	    perror("SET_BUF::");
    	}
    	err_n = getsockopt(fd,SOL_SOCKET,SO_SNDBUF,
                        (char*)&bsize,&n);
	}
 
	return	status;
}

int tcp_connect(int fd, const char *aAddr, unsigned short aPort)
/********************************************************
*				Initial  TCP  socket					*
*	IN:	aAddr:	local bind address						*
*		aPort:	listen port number						*
*		fd:		socket									*
*	Return:		<0:	open  error							*
*				>0:	success								*
*********************************************************/
{
    	struct sockaddr_in 	addr;
    	int 	status;
	int	val;
	char szaddr[20] = {0};
	struct hostent * host = gethostbyname(aAddr);
	if (host)
	{
		sprintf(szaddr, "%s", inet_ntoa(*((struct in_addr *)host->h_addr)));
        }else
	{
		printf ("get host ip of '%s' failed\n", aAddr);
		return -1;
	}


	bzero((char *)&addr,sizeof(struct sockaddr_in));
	addr.sin_family 	  = AF_INET;
	addr.sin_addr.s_addr = inet_addr(szaddr);
	addr.sin_port 		  = htons(aPort);

	status = connect(fd,(struct sockaddr *)&addr,sizeof(addr));
	if	(status < 0)
	{
		perror("Connect::");
	}
	else
	{
    	val  = fcntl(fd,F_GETFL,0);
    	val |= O_NONBLOCK;
    	val  = fcntl(fd, F_SETFL, val);
    	if  (val < 0)
    		perror("fcntl F_SETFL, O_NONBLOCK");
	}

	return status;
}

int tcp_read(int aFd,char *aBuf)
/********************************************************
*				read data from tcp						*
*	IN:	aFd:	socket									*
*	out:aBuf:	data buffer pointer						*
*	return:		<0:	read nothing						*
*				>0:	number  of data to read				*
*********************************************************/
{
	int	status;

    status = recv(aFd,aBuf,1500,0);
	if	(status < 0)
	{
		if	(errno == EAGAIN)
			status = 0;
		else
		{
			printf("TCP_READ_ERROR::[%d--[%d]--[%d]]\n",aFd,status,errno);
			perror("TCP recv::");
			status = -1;
		}
	}

    return status;
}

int tcp_send(int aFd,char *aBuf,int aLen)
/********************************************************
*				aend data from tcp						*
*	IN:	aFd:	socket									*
*	  	aBuf:	data buffer pointer						*
*		alen:	data length								*
*	return:		<0:	errorg								*
*				=0:	itry again							*
*				=0: send OK								*
*********************************************************/
{
	int	status;

	//status = send(aFd, aBuf, aLen, MSG_NOSIGNAL|MSG_DONTWAIT);
	status = send(aFd, aBuf, aLen, MSG_NOSIGNAL);
	if	(status <= 0)
	{
		if	(errno == EAGAIN)
			status = 0;
		else
		{
			printf("TCP_SEND_ERROR::[%d--[%d]--[%d]---%d]\n",aFd,status,errno,aLen);
			fflush(stdout);
			perror("TCP send::");
		}
	}
//	else
//		printf("send=[%d]\n",status);

    return status;
}

int tcp_close(int aFd)
{
    if  (aFd != -1)
		close(aFd);
    return 1;
}
 
