/*
 * gpib.h X68k XC Compiler v2.00 Copyright 1990 SHARP/Momodera's
 */
#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

int	gp_open(int,int,int,int *);
int	gp_close(void);
int	gp_ifc(int);
int	gp_connect(int,int *);
int	gp_eor(int,int,int);
int	gp_input(char *,int,int *,int);
int	gp_output(char *,int,int,int,int);
int	gp_atnon(void);
int	gp_atnoff(void);
int	gp_remote(void);
int	gp_local(void);
int	gp_dcl(void);
int	gp_llo(void);
int	gp_gtl(int *);
int	gp_sdc(int *);
int	gp_trg(int *);
int	gp_sla(int *);
int	gp_sta(int);
int	gp_unl(void);
int	gp_unt(void);
int	gp_comout(int);
int	gp_req(int);
int	gp_sp(int *,int *);
int	gp_st(void);

#undef	__PROTO_TYPE
#else

int	gp_open();
int	gp_close();
int	gp_ifc();
int	gp_connect();
int	gp_eor();
int	gp_input();
int	gp_output();
int	gp_atnon();
int	gp_atnoff();
int	gp_remote();
int	gp_local();
int	gp_dcl();
int	gp_llo();
int	gp_gtl();
int	gp_sdc();
int	gp_trg();
int	gp_sla();
int	gp_sta();
int	gp_unl();
int	gp_unt();
int	gp_comout();
int	gp_req();
int	gp_sp();
int	gp_st();

#endif
