/*
 * ipcrawl - crawl through ip address space getting DNS names
 * by dmuz @ AP
 */
/* $Id: ipcrawl.c,v 1.7 2004/02/05 18:58:04 dmuz Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_FAST_CHECKS 50

extern char *optarg;
extern int optind;

int main(int argc, char **argv) {

 int total;
 int ch;
 char *ipstring;
 int unsigned long start, end;
 int verbose = 0;
 int quiet = 0;
 char *outfile = NULL;
 FILE *output = NULL;
 char *appname = NULL;
 int count = 0;
 int pause = 0;

 struct in_addr saddr, eaddr;
 struct hostent *hinfo=NULL, *haddr=NULL;

 appname = argv[0];

 /* check args */
 if(argc < 2) {
  printf("\nipcrawl by dmuz @ AngryPacket\n");
  printf("%s -q -v -s <seconds> -f <outfile> <startip> <endip>\n", appname);
  printf("\t<startip>\tfirst address in range to scan\n");
  printf("\t<endip>\t\tlast address of range to scan\n");
  printf("\t-f <outfile>\tlog output to file\n");
  printf("\t-s <seconds>\tseconds to wait between every 50 lookups\n");
  printf("\t-q\t\tquiet (no console output)\n");
  printf("\t-v\t\tverbose mode\n");
  exit(0);
 }


 /* parse arguments */
 while((ch = getopt(argc, argv, "vqs:f:")) != -1) {
  switch(ch) {
  case 'v':
   verbose = 1;
   fprintf(stderr,"verbose mode enabled.\n");
   break;
  case 'q':
   quiet = 1;
   break;
  case 's':
   pause = atoi(optarg);
   break;
  case 'f':
   outfile = optarg;
   fprintf(stderr,"logging to %s.\n", outfile);
   output = fopen(outfile,"w+");
   break;
  }
 }

 if(verbose) {argc--; argv++;}
 if(quiet) {argc--; argv++;}
 if(outfile) {argc -= 2; argv += 2;}
 if(pause) {argc -= 2; argv += 2;}

 if(!argv[1] || !argv[2]) {
  printf("\nerror: missing address range\n");
  printf("%s -q -v -s <seconds> -f <outfile> <startip> <endip>\n", appname);
  exit(0);
}

 inet_aton(argv[1], &saddr);
 start = saddr.s_addr;

 inet_aton(argv[2], &eaddr);
 end = eaddr.s_addr;

 total = end - start;

 if(verbose) printf("lookup %lu to %lu total: %i\n", start, end, total);

 while(saddr.s_addr < end+1) {

  haddr = NULL; hinfo = NULL;

  /* pause to avoid hammering dns servers */
  if(pause) {
   count++;
   if(count == MAX_FAST_CHECKS) {
    fprintf(stderr,"Sleeping %i seconds to avoid DNS server fatiuge...\n", pause);
    sleep(pause);
    count = 0;
   }
  }

  ipstring = strdup(inet_ntoa(saddr));
  if(verbose) fprintf(stderr,"checking %s...", ipstring);

  haddr = gethostbyname(ipstring);
  if(haddr == NULL) {
    if(verbose) fprintf(stderr," failed.\n");
  }

  hinfo = gethostbyaddr(haddr->h_addr, 4, AF_INET);
  if(hinfo == NULL) {
   if(verbose) fprintf(stderr," failed.\n");
   saddr.s_addr = htonl( ntohl( saddr.s_addr) + 1);
   continue;
  }

  /* log to file */
  if(outfile) {
   fprintf(output, "%s:%s\n", hinfo->h_name, ipstring);
   //fflush(output);
  }

  /* print output */
  if(quiet) {
   printf("%s\n", hinfo->h_name);
  } else {
   printf("%s:%s\n", hinfo->h_name, ipstring);
  }

  /* increment to next address */
  saddr.s_addr = htonl( ntohl( saddr.s_addr) + 1);
 }

 if(output) fclose(output);

}
