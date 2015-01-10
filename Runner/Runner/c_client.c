#include <WinSock2.h>
#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>


//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <strings.h>
//#include <unistd.h>
//#include <stdbool.h>


#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

typedef struct {
    int socket;
    SSL *sslHandle;
    SSL_CTX *sslContext;
} connection;

#define SERVER "localhost"
#define PORT 8080

int tcpConnect() {
    int error, handle;
    struct hostent *host;
    struct sockaddr_in server;

    host = gethostbyname(SERVER);
    handle = socket(AF_INET, SOCK_STREAM, 0);
    struct timeval timeout;      
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    if (setsockopt (handle, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
	sizeof(timeout)) < 0) { perror("setsockopt failed\n"); }
    if (handle == -1) {
        perror("Socket");
	handle = 0;
    } else {
        server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr = *((struct in_addr *) host->h_addr);
	memset(&(server.sin_zero) , 0, 8);
	//bzero(&(server.sin_zero), 8);

	error = connect(handle, (struct sockaddr *) &server,
			sizeof(struct sockaddr));

	if (error == -1) {
            perror("Connect");
	    handle = 0;
	}
    }
    return handle;
}

connection * sslConnect(void) {
    connection *c;

    c = (connection *) malloc(sizeof(connection));
    c->sslHandle = NULL;
    c->sslContext = NULL;

    c->socket = tcpConnect();
    if (c->socket) {
        SSL_load_error_strings();
	SSL_library_init();

	c->sslContext = SSL_CTX_new(TLSv1_method());
	if (c->sslContext == NULL) {
            ERR_print_errors_fp (stderr);
	}


	c->sslHandle = SSL_new(c->sslContext);
	if (c->sslHandle == NULL) {
            ERR_print_errors_fp(stderr);
	}
	if (!SSL_set_fd(c->sslHandle, c->socket)) {
            ERR_print_errors_fp (stderr);
	}
	if (SSL_connect (c->sslHandle) != 1) {
            ERR_print_errors_fp (stderr);
	}
    } else {
        perror("Connect failed");
    }
    return c;
}

void sslDisconnect (connection * c)
{
    if (c->socket) {
        close(c->socket);
    }
    if (c->sslHandle) {
        SSL_shutdown (c->sslHandle);
	SSL_free (c->sslHandle);
    }
    if (c->sslContext) {
        SSL_CTX_free (c->sslContext);
    }
    free(c);
}

char * sslRead (connection * c)
{
    const int readSize = 1024;
    char *rc = NULL;
    int received, count = 0;
    char buffer[1024];

    rc = (char *) malloc(readSize * sizeof(char) + 1);
	memset(rc,0, readSize * sizeof(char) + 1);
    // bzero(rc, readSize * sizeof(char) + 1);
    received = SSL_read(c->sslHandle, buffer, readSize);
    if ((size_t)received != sizeof(char)) {
	printf("failed to receive\n");
        return NULL;
    }
    buffer[received] = '\0';

    return buffer;
}

void sslWrite(connection * c, char * text)
{
    if (c) {
        SSL_write (c->sslHandle, text, strlen(text));
    }
}

connection * init()
{
    connection * c;
    char * cert;
    char * response = NULL;
    int fd;

    c = sslConnect();
    response = sslRead(c);
    printf("response is: %s\n", response);
    cert = (char *) malloc(sizeof(char) * 4097);
    if (strcmp(response, "1") != 0) {
        printf("check 1 failed\n");
	goto error;
    }
	memset(cert,0,4097);
//    bzero(cert, 4097);
//    assert(cert[4096] == '\0');
    fd = open("game_cert", O_RDONLY);
    read(fd, cert, 4096);
    printf("%s\n", cert);
    sslWrite(c, cert);
    response = sslRead(c);
    if (strcmp(response, "1") != 0) {
        printf("check 2 failed\n");
	goto error;
    }
    sslWrite(c, "99999|today\n");
    response = sslRead(c);
    if (strcmp(response, "1") != 0) {
        printf("check 3 failed\n");
	goto error;
    }
    free(cert);
    return c;
error:
    free(cert);
    sslDisconnect(c);
    return NULL;
}

int main(int argc, char ** argv)
{
    connection * c;
    char * response = NULL;
    c = init();
    int num_attempts;
    if (c == NULL) {
        printf("could not connect\n");
	return -1;
    }
    printf("Success!\n");
    while (1) {
        char line[4096];
        fgets(line, 4096, stdin);
	if (strcmp(line, "END\n") == 0) break;
        sslWrite(c, line);

        response = sslRead(c);
	num_attempts = 0;
        while (response == NULL && num_attempts < 10) {
	    num_attempts++;
            printf("failed to receive OK from server...disconnecting\n");
	    sslDisconnect(c);
	    c = init(); // reconnect to server
	    sslWrite(c, line);
	    response = sslRead(c);
	}
	if (response == NULL) {
            printf("too many failed attempts, check your connection\n");
	    return -1;
	}
	printf("%s\n", response);
    }    
    return 0;
}
