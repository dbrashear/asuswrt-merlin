#include "base.h"
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netinet/in.h>
#include "array.h"
#include "buffer.h"
#include <string.h>
#include <stdio.h>
#include "log.h"

#include <stdlib.h>
#include <limits.h>

#include <errno.h>
#include <assert.h>

#define HAVE_LIBSMBCLIENT_H

#include "sys-socket.h"
#include <libsmbclient.h>
#include <dlinklist.h>

//#define _USEMYTIMER
#define _USEARP
#define MAC_BCAST_ADDR  "\xff\xff\xff\xff\xff\xff"
#define TT_SIGUSR2 (SIGUSR2)
#define ARP_TIME_INTERVAL 2
#define RE_SCAN_TIME_INTERVAL 120

#define THREAD_BEGIN_INDEX 1

#define DBE 1

#define NUM_THREADS 15
#define MAX_THREADS 254
int g_threadIndex = THREAD_BEGIN_INDEX;
int g_current_time = 0;
int g_rescancount=0;
int g_useSystemTimer=0;
int g_bInitialize=0;
time_t g_begin_time=0;

struct arpMsg {
    struct ethhdr ethhdr;                   /* Ethernet header */
    u_short htype;                          /* hardware type (must be ARPHRD_ETHER) */
    u_short ptype;                          /* protocol type (must be ETH_P_IP) */
    u_char  hlen;                           /* hardware address length (must be 6) */
    u_char  plen;                           /* protocol address length (must be 4) */
    u_short operation;                      /* ARP opcode */
    u_char  sHaddr[6];                      /* sender's hardware address */
    u_char  sInaddr[4];                     /* sender's IP address */
    u_char  tHaddr[6];                      /* target's hardware address */
    u_char  tInaddr[4];                     /* target's IP address */
    u_char  pad[18];                        /* pad for min. Ethernet payload (60 bytes) */
};

struct ifinfo {
    char ifname[IFNAMSIZ];
    u_long addr;                /* network byte order */
    u_long mask;                /* network byte order */
    u_long bcast;               /* network byte order */
    u_char haddr[6];
    short flags;
};
#if 0
typedef struct smb_srv_info_s {
	int id;
	buffer *ip;
	buffer *mac;
	buffer *name;
	struct smb_srv_info_s *prev, *next;
}smb_srv_info_t;

smb_srv_info_t *smb_srv_info_list;
#endif

typedef struct _SrvInfo{
	int id;
	pthread_t t;
	char iface[10];
	struct in_addr ipSrv;
	u_int8_t macSrv[6];	
	in_addr_t arp_ip;
	buffer* arp_cip;
}SrvInfo;
SrvInfo a_srvInfo[MAX_THREADS];


int openRawSocket (int *s, u_short type) {
	int optval = 1;

	if((*s = socket (AF_INET, SOCK_PACKET, htons (type))) == -1) {
		Cdbg(DBE, "Could not open raw socket\n");
		return -1;
	}

	if(setsockopt (*s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof (optval)) == -1) {
		Cdbg(DBE, "Could not setsocketopt on raw socket\n");
		return -1;
	}
	return 0;
}

void mkArpMsg(int opcode, u_long tInaddr, u_char *tHaddr,
		 u_long sInaddr, u_char *sHaddr, struct arpMsg *msg) {
	bzero(msg, sizeof(*msg));
	bcopy(MAC_BCAST_ADDR, msg->ethhdr.h_dest, 6); /* MAC DA */
	bcopy(sHaddr, msg->ethhdr.h_source, 6); /* MAC SA */
	msg->ethhdr.h_proto = htons(ETH_P_ARP); /* protocol type (Ethernet) */
	msg->htype = htons(ARPHRD_ETHER);	       /* hardware type */
	msg->ptype = htons(ETH_P_IP);		   /* protocol type (ARP message) */
	msg->hlen = 6;						  /* hardware address length */
	msg->plen = 4;						  /* protocol address length */
	msg->operation = htons(opcode);		 /* ARP op code */
	*((u_int *)msg->sInaddr) = sInaddr;	     /* source IP address */
	bcopy(sHaddr, msg->sHaddr, 6);		  /* source hardware address */
	*((u_int *)msg->tInaddr) = tInaddr;	     /* target IP address */
	if ( opcode == ARPOP_REPLY )
		bcopy(tHaddr, msg->tHaddr, 6);	  /* target hardware address */
}

int arpCheck(int thread_id,u_long inaddr, struct ifinfo *ifbuf, long timeout, buffer* arp_ip)  {
	int			     s;		      /* socket */
	int			     rv;		     /* return value */
	struct sockaddr addr;	   /* for interface name */
	struct arpMsg   arp;
	fd_set		  fdset;
	struct timeval  tm;
	time_t		  prevTime;
	char checkip[100];
	char checkmac[100];

	strcpy(checkip, arp_ip->ptr);

	rv = 1;
	if( openRawSocket(&s, ETH_P_ARP) == -1 ){
		return -1;
	}
		
	/* send arp request */
	mkArpMsg(ARPOP_REQUEST, inaddr, NULL, ifbuf->addr, ifbuf->haddr, &arp);
	bzero(&addr, sizeof(addr));
	strcpy(addr.sa_data, ifbuf->ifname);
	if ( sendto(s, &arp, sizeof(arp), 0, &addr, sizeof(addr)) < 0 ) rv = 0;
	

	/* wait arp reply, and check it */
	tm.tv_usec = 0;
	time(&prevTime);
	while ( timeout > 0 ) {
		FD_ZERO(&fdset);
		FD_SET(s, &fdset);
		tm.tv_sec  = timeout;
		if ( select(s+1, &fdset, (fd_set *)NULL, (fd_set *)NULL, &tm) < 0 ) {
			Cdbg(DBE, "Error on ARPING request: %s\n", strerror(errno));
			if (errno != EINTR) rv = 0;
		} else if ( FD_ISSET(s, &fdset) ) {
			if (recv(s, &arp, sizeof(arp), 0) < 0) rv = 0;
			if( arp.operation == htons(ARPOP_REPLY) && 
			     bcmp(arp.tHaddr, ifbuf->haddr, 6) == 0 && 
			     *((u_int *)arp.sInaddr) == inaddr ) {
				
				//inet_ntop(AF_INET, arp.sInaddr, checkip, sizeof(checkip)) ;

				//fprintf(stderr,"enter %d\n", thread_id);
				//char* hostname = smbc_nmblookup(checkip);				
				//fprintf(stderr,"leave %d, %s\n", thread_id, hostname);
				//hostname = "TSET";

				#if 1
				//if(hostname!=NULL)
				{
					
					sprintf(checkmac, "%02X:%02X:%02X:%02X:%02X:%02X", 
						    arp.sHaddr[0]&0xff, arp.sHaddr[1]&0xff, arp.sHaddr[2]&0xff,
						    arp.sHaddr[3]&0xff, arp.sHaddr[4]&0xff, arp.sHaddr[5]&0xff);

					#if 0
					fprintf(stderr, "Valid arp reply receved for this address, thread_id=[%d]\n", thread_id);
					fprintf(stderr, "IP is %s, MAC is %s\n\n", 
						    checkip,
						    checkmac);
					#endif

					int bFound = 0;
					smb_srv_info_t *p;
					for (p = smb_srv_info_list; p; p = p->next) {
						if(strcmp(p->ip->ptr, checkip)==0){
							//fprintf(stderr,"\t\tFound ip=[%s] in list!\n", checkip);
							bFound = 1;
							break;
						}
					}

					if(bFound==0){
						
						smb_srv_info_t *smb_srv_info;
						smb_srv_info = (smb_srv_info_t *)calloc(1, sizeof(smb_srv_info_t));

						smb_srv_info->id = thread_id;
						
						smb_srv_info->ip = buffer_init();
						buffer_copy_string(smb_srv_info->ip, checkip);
						
						smb_srv_info->mac = buffer_init();
						buffer_copy_string(smb_srv_info->mac, checkmac);
						
						smb_srv_info->name = buffer_init();
						buffer_copy_string(smb_srv_info->name, "");
						//buffer_copy_string(smb_srv_info->name, hostname);
			
						DLIST_ADD(smb_srv_info_list, smb_srv_info);

						//fprintf(stderr,"add ip=[%s] to list!\n", checkip);

						/*
						int count = 0;
						smb_srv_info_t *c;
						for (c = smb_srv_info_list; c; c = c->next) {
							count++;
						}
						fprintf(stderr,"List count [%d]!\n", count);
						*/
					}
				}

				rv = 0;
				#else
				rv = 1;
				#endif
				
				break;
			}
		}
		timeout -= time(NULL) - prevTime;
		time(&prevTime);
	}


	close(s);
	
	if(rv==1){
		smb_srv_info_t *p;
		for (p = smb_srv_info_list; p; p = p->next) {
			//fprintf(stderr,"check [%s]->[%s]\n", p->ip->ptr, checkip);
			if(strcmp(p->ip->ptr, checkip)==0){
				Cdbg(DBE,"ip=[%s] name=[%s] is not exist!\n", checkip, p->name->ptr);
				buffer_free(p->ip);
				buffer_free(p->mac);
				buffer_free(p->name);
				DLIST_REMOVE(smb_srv_info_list, p);
				free(p);
				break;
			}
		}
	}
	//fprintf(stderr, "ip=[%s], %salid arp replies for this address\n", checkip, rv ? "No v" : "V");       

	return rv;
}


int arpping(SrvInfo *psrvInfo) {
	struct ifinfo ifbuf;

	strcpy(ifbuf.ifname, psrvInfo->iface);
	ifbuf.addr = psrvInfo->ipSrv.s_addr;
	ifbuf.mask = 0x0;
	ifbuf.bcast = 0x0;

	memcpy(ifbuf.haddr, psrvInfo->macSrv, 6);
	ifbuf.flags = 0;
	
	return arpCheck(psrvInfo->id, psrvInfo->arp_ip, &ifbuf, 2, psrvInfo->arp_cip);
}

static void *thread_do_arp_check_function(void *srvInfo)
{	
	SrvInfo *psrvInfo = srvInfo;
	
	if(psrvInfo){
		arpping(psrvInfo);
	}

	pthread_exit(NULL);
}

static int thread_arpping(char* iface)
{
	int res;
	void *thread_result;
	int lots_of_threads;

	struct sockaddr_in *sin;
	struct ifreq ifr;
	int fd = -1;
	struct in_addr ipSrv;
	u_int8_t macSrv[6];

	if(g_threadIndex>=MAX_THREADS){
		return 0;	
	}
		
	if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) {
		ifr.ifr_addr.sa_family = AF_INET;
		strcpy(ifr.ifr_name, iface);
		
		if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
			sin = (struct sockaddr_in *) &ifr.ifr_addr;
			ipSrv = sin->sin_addr;
		} else {
			Cdbg(DBE, "%s SIOCGIFADDR failed!\n", iface);
			close(fd);
			return 0;	
		}
		if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
			memcpy(macSrv, ifr.ifr_hwaddr.sa_data, 6);
		} else {
			Cdbg(DBE, "%s SIOCGIFHWADDR failed!\n", iface);
			close(fd);
			return 0;	
		}

		close(fd);
	} else {
		Cdbg(DBE, "cannot init socket.\n");
		return 0;	
	}

	
	
	int create_num_threads = 0;
	for( lots_of_threads=g_threadIndex; 
	     lots_of_threads<g_threadIndex+NUM_THREADS; 
	     lots_of_threads++){

		//- Scan IP
		register char *p = (char *)&ipSrv;		
		#define	UC(b)	(((int)b)&0xff)		

		if(lots_of_threads>=MAX_THREADS){
			g_threadIndex = MAX_THREADS;
			return 0;	
		}

		SrvInfo *pSrvInfo = &a_srvInfo[lots_of_threads];

		pSrvInfo->id = lots_of_threads;
		pSrvInfo->ipSrv = ipSrv;
		memcpy(pSrvInfo->macSrv, macSrv, 6);
				
		int scanIndex =  lots_of_threads+1;
		char ipAddr[18] = "";
		sprintf(ipAddr,"%u.%u.%u.%u", UC(p[0]), UC(p[1]), UC(p[2]), scanIndex);
		//Cdbg(DBE, "Send arp to %s\n", ipAddr);
		pSrvInfo->arp_ip = inet_addr(ipAddr);
		
		buffer_copy_string(pSrvInfo->arp_cip, ipAddr);		
		strcpy( pSrvInfo->iface, iface );		
		
		pthread_attr_t thread_attr;
		res = pthread_attr_init(&thread_attr);
		
		if(res!=0){
			Cdbg(DBE, "Attribute creation failed!\n");
			continue;
		}

		res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
		if(res!=0){
			Cdbg(DBE, "Setting detached attribute failed!\n");
			continue;
		}

		res = pthread_create( &pSrvInfo->t, 
						      &thread_attr,
						      thread_do_arp_check_function, 
						      (void*)pSrvInfo );
		
		if(res!=0){
			Cdbg(DBE, "Thread create fail! %s, lots_of_threads= %d, errno=[%d]\n", ipAddr, lots_of_threads, res);
			continue;
		}
		
		create_num_threads++;
	}
	
#if 0
	//fprintf(stderr, "Waiting for threads to finish...\n");

	int i;
	
	for(i=g_threadIndex+NUM_THREADS-1;i>=g_threadIndex;i--){
		//fprintf(stderr, "Waiting %d\n", i );
		res = pthread_join(a_srvInfo[i].t, &thread_result);
		if(res==0){
			//fprintf(stderr, "Picked up a thread [%d]\n", i);
			//a_srvInfo[i].result = 1;
		}
		else{
			//fprintf(stderr, "pthread_join failed [%d]\n", i);
		}
	}
#endif

	g_threadIndex=lots_of_threads;

	if(g_threadIndex>=MAX_THREADS)
		return 0;	

	return 1;
}

void query_one_hostname(){
	smb_srv_info_t *p;
	for (p = smb_srv_info_list; p; p = p->next) {

		if(strcmp(p->name->ptr, "")!=0){
			continue;
		}

		if(p->id==0){
			Cdbg(DBE, "query_one_hostname [%s]\n", p->ip->ptr);
			//continue;
		}

		char* hostname = smbc_nmblookup(p->ip->ptr);
		
		if(hostname==NULL){
			//Cdbg(DBE, "\t\tCan't query samba server name[%s]\n", p->ip->ptr);

			buffer_free(p->ip);
			buffer_free(p->mac);
			buffer_free(p->name);
			
			DLIST_REMOVE(smb_srv_info_list, p);
			free(p);
		}
		else{
			buffer_copy_string(p->name, hostname);
		}
	}
}

void on_arpping_timer_handler(server *srv) {

	if(g_bInitialize==0)
		return;

	time_t cur_time = srv->cur_ts;
	thread_arpping(srv->srvconf.arpping_interface->ptr);
		
	query_one_hostname();
	
	//- Rescan samba server
	if( g_current_time > RE_SCAN_TIME_INTERVAL &&g_threadIndex == MAX_THREADS){

		g_rescancount++;
		g_threadIndex = THREAD_BEGIN_INDEX;;

		char strTime[9] = {0};
		strftime(strTime, sizeof(strTime), "%T", localtime(&cur_time));	
		Cdbg(DBE, "start to rescan samba server count=[%d], time_offset=[%d], time=[%s]\n", g_rescancount, g_current_time, strTime);
		
		g_current_time = 0;
		g_begin_time = 0;
		init_a_srvInfo();
	}

	if(g_useSystemTimer==1){
		if(g_begin_time==0)
			g_begin_time = cur_time;
		else
			g_current_time = cur_time - g_begin_time;
	}
#ifdef _USEMYTIMER
	else
		g_current_time += ARP_TIME_INTERVAL;
#endif
	
}

#ifdef _USEMYTIMER
timer_t create_timer(int signo) {
	timer_t timerid;
    struct sigevent se;
    se.sigev_notify = SIGEV_SIGNAL;
    se.sigev_signo = signo;

    if (timer_create(CLOCK_REALTIME, &se, &timerid) == -1) {
		return -1;
   	}
    return timerid;
}

void install_sighandler(int signo, void(*handler)(int)) {
    sigset_t set;
    struct sigaction act;

    /* Setup the handler */
    act.sa_handler = handler;
    act.sa_flags = SA_RESTART;
    sigaction(signo, &act, 0);

	/* Unblock the signal */
	sigemptyset(&set);
	sigaddset(&set, signo);
	sigprocmask(SIG_UNBLOCK, &set, NULL);
}

void set_timer(timer_t timerid, int seconds) {
	struct itimerspec timervals;
	timervals.it_value.tv_sec = seconds;
	timervals.it_value.tv_nsec = 0;
	timervals.it_interval.tv_sec = seconds;
	timervals.it_interval.tv_nsec = 0;

	if (timer_settime(timerid, 0, &timervals, NULL) == -1) {
	    
	}
}
#endif

void init_a_srvInfo()	
{
	int i;
	for(i=0; i<MAX_THREADS; i++){
		SrvInfo *pSrvInfo = &a_srvInfo[i];

		pSrvInfo->id = -1;
		pSrvInfo->t = NULL;
		pSrvInfo->arp_cip = buffer_init();
	}
}

void start_scan_sambaserver(int use_sys_timer)
{
	//- Remove all
	smb_srv_info_t *c;
	for (c = smb_srv_info_list; c; c = c->next) {
		Cdbg(DBE, "remove , ip=[%s]\n", c->ip->ptr);
		DLIST_REMOVE(smb_srv_info_list,c);
		free(c);
		c = smb_srv_info_list;
	}
	free(smb_srv_info_list);
	
	init_a_srvInfo();

	g_useSystemTimer = use_sys_timer;
	g_bInitialize = 1;
	
#ifdef _USEMYTIMER
	if(g_useSystemTimer==0){
		Cdbg(DBE, "create timer\n");
		timer_t arp_timer = create_timer(TT_SIGUSR2);
    	install_sighandler(TT_SIGUSR2, on_arpping_timer_handler);
    	set_timer(arp_timer, ARP_TIME_INTERVAL);
	}
#endif
}

