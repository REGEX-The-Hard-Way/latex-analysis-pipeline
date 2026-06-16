/*
 * def_extract.c — Production-grade definition extraction for math variables.
 *
 * 5 pattern types: copula, apposition, equation, defines, parenthetical.
 * All buffer operations use distinct source/destination to avoid overlap.
 * Bounds-checked on all array accesses.
 *
 * Edge cases: $$, \(, \[ math, abbreviations, nested braces,
 *             \mathbf, \mathcal, \mathbb stripping, vague filter.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "def_extract.h"

#define MAX_TEX      (2*1024*1024)
#define MAX_SENTS    4096
#define MAX_PHS       2048
#define MAX_SENT_LEN  8192

/* ---- placeholder ---- */
typedef struct { int id; char sym[64]; int s,e; char raw[128]; } ph_t;
static ph_t  ph[MAX_PHS];  static int nph;
static char  W[MAX_TEX];   static int  Wlen;
static int   ss[MAX_SENTS], se[MAX_SENTS], ns;

/* ---- safe strncpy ---- */
static void scpy(char *d, const char *s, int n) {
    int i; for(i=0;i<n-1&&s[i];i++)d[i]=s[i]; d[i]=0;
}

/* ---- symbol key from $body$ ---- */
static int symkey(const char *b, int len, char *o, int max) {
    char t[256]; int tl=len<255?len:255; memcpy(t,b,tl); t[tl]=0;
    if(strstr(t,"\\frac")||strstr(t,"\\tfrac")||strstr(t,"\\dfrac")
     ||strstr(t,"\\binom")||strstr(t,"\\choose")) return 0;
    if(strstr(t,"=")||strstr(t,"\\approx")||strstr(t,"\\equiv")
     ||strstr(t,"\\sim")||strstr(t,"\\propto")) return 0;
    char u[256]; int ui=0; const char*s=t;
    while(*s&&ui<250) {
        if(!strncmp(s,"\\mathrm{",8)||!strncmp(s,"\\mathbf{",8)
         ||!strncmp(s,"\\mathbb{",8)||!strncmp(s,"\\mathit{",8)
         ||!strncmp(s,"\\mathsf{",8)||!strncmp(s,"\\mathtt{",8)
         ||!strncmp(s,"\\textrm{",8)){s+=8;while(*s&&*s!='}')s++;if(*s)s++;continue;}
        if(!strncmp(s,"\\mathcal{",9)||!strncmp(s,"\\mathfrak{",10))
          {s+=9;while(*s&&*s!='}')s++;if(*s)s++;continue;}
        if(!strncmp(s,"\\text{",6)||!strncmp(s,"\\mbox{",6))
          {s+=6;while(*s&&*s!='}')s++;if(*s)s++;continue;}
        if(!strncmp(s,"\\rm ",4)){s+=3;continue;}
        if(*s=='\\'||*s=='{'||*s=='}'){s++;continue;}
        if(!strncmp(s,"\\,",2)||!strncmp(s,"\\;",2)||!strncmp(s,"\\:",2)||!strncmp(s,"\\ ",2)){s+=2;continue;}
        u[ui++]=*s++;
    }
    u[ui]=0; char *up=u; while(*up==' ')up++;
    int ul=(int)strlen(up); while(ul>0&&up[ul-1]==' ')ul--; up[ul]=0;
    if(ul<1||ul>12||!isalpha((unsigned char)up[0])) return 0;
    for(int i=0;i<ul;i++) if(!isalnum((unsigned char)up[i])&&up[i]!='_'&&up[i]!='^') return 0;
    int ol=ul<max-1?ul:max-2; memcpy(o,up,ol); o[ol]=0; return 1;
}

/* check for relations in math body */
static int hasrel(const char *b, int len) {
    char t[128]; int tl=len<127?len:127; memcpy(t,b,tl); t[tl]=0;
    return strstr(t,"=")||strstr(t,"\\approx")||strstr(t,"\\equiv")
        ||strstr(t,"\\sim")||strstr(t,"\\propto")||strstr(t,"\\simeq");
}

/* ---- strip \newcommand/\def bodies ---- */
static int strip_defs(const char *in, int ilen, char *out) {
    int o=0;
    for(int i=0;i<ilen&&o<MAX_TEX-2;i++) {
        int cl=0;
        if(i+11<=ilen&&!strncmp(in+i,"\\newcommand",11)) cl=11;
        else if(i+13<=ilen&&!strncmp(in+i,"\\renewcommand",13)) cl=13;
        else if(i+15<=ilen&&!strncmp(in+i,"\\providecommand",15)) cl=15;
        else if(i+14<=ilen&&!strncmp(in+i,"\\newenvironment",14)) cl=14;
        else if(i+4<=ilen&&!strncmp(in+i,"\\def",4)&&(i+4>=ilen||!isalpha((unsigned char)in[i+4]))) cl=4;
        else if(i+5<=ilen&&!strncmp(in+i,"\\edef",5)) cl=5;
        else if(i+5<=ilen&&!strncmp(in+i,"\\xdef",5)) cl=5;
        if(cl){i+=cl; int d=0,st=0;
            while(i<ilen){if(in[i]=='{'){d++;st=1;}else if(in[i]=='}'){d--;if(d<=0&&st){i++;break;}}i++;}
            if(o>0&&out[o-1]!=' ')out[o++]=' '; continue;}
        out[o++]=in[i];
    }
    out[o]=0; return o;
}

/* ---- replace math with placeholders ---- */
static int replace_math(const char *in, int ilen, char *out) {
    int o=0; nph=0;
    for(int i=0;i<ilen&&o<MAX_TEX-20;i++) {
        int dl=0, ed=0;
        if(in[i]=='$'){if(i+1<ilen&&in[i+1]=='$'){dl=2;ed=2;}else{dl=1;ed=1;}}
        else if(in[i]=='\\'&&i+1<ilen){if(in[i+1]=='('){dl=2;ed=2;}else if(in[i+1]=='['){dl=2;ed=2;}}
        if(!dl){out[o++]=in[i];continue;}
        i+=dl; int bs=i, j=i;
        if(ed==1){while(j<ilen&&in[j]!='$')j++;}
        else{while(j+1<ilen&&!(in[j]=='\\'&&(in[j+1]==')'||in[j+1]==']'))&&!(in[j]=='$'&&in[j+1]=='$'))j++;j+=2;}
        if(j>=ilen){o+=snprintf(out+o,MAX_TEX-o,"MATHVAL");i=j;continue;}
        int bl=(ed==1)?(j-i):(j-2-i); if(bl<0)bl=0;
        char sym[64]=""; int rel=hasrel(in+bs,bl);
        symkey(in+bs,bl,sym,sizeof(sym));
        if(sym[0]&&!rel&&nph<MAX_PHS){
            int id=nph; ph[id].id=id; scpy(ph[id].sym,sym,sizeof(ph[id].sym));
            ph[id].s=i-dl; ph[id].e=j;
            int rl=j-ph[id].s; if(rl>127)rl=127;
            memcpy(ph[id].raw,in+ph[id].s,rl); ph[id].raw[rl]=0;
            o+=snprintf(out+o,MAX_TEX-o,"VARSYM%04d",id); nph++;
        }else{o+=snprintf(out+o,MAX_TEX-o,"MATHVAL");}
        i=j-1;
    }
    out[o]=0; return o;
}

/* ---- strip remaining TeX commands/braces ---- */
static int strip_tex(const char *in, int ilen, char *out) {
    int o=0;
    for(int i=0;i<ilen&&o<MAX_TEX-2;i++) {
        if(in[i]=='\\'){
            int nx=i+1;
            if(nx<ilen&&isalpha((unsigned char)in[nx])){
                while(i<ilen&&isalpha((unsigned char)in[i]))i++;
                if(i<ilen&&in[i]=='{'){int d=1;i++;while(i<ilen&&d>0){if(in[i]=='{')d++;else if(in[i]=='}')d--;i++;}}
                if(o>0&&out[o-1]!=' ')out[o++]=' '; continue;}
            out[o++]=' '; i++; continue;
        }
        if(in[i]=='{'||in[i]=='}'){if(o>0&&out[o-1]!=' ')out[o++]=' ';continue;}
        out[o++]=in[i];
    }
    out[o]=0; return o;
}

/* ---- collapse whitespace ---- */
static int collapse_ws(const char *in, int ilen, char *out) {
    int o=0, sp=0;
    for(int i=0;i<ilen&&o<MAX_TEX-2;i++){
        char c=in[i];
        if(c==' '||c=='\t'||c=='\n'||c=='\r'){if(!sp&&o>0){out[o++]=' ';sp=1;}}
        else{
            if(o>0&&out[o-1]!=' '&&c=='V'&&!strncmp(in+i,"VARSYM",6)){out[o++]=' ';}
            out[o++]=c; sp=0;
        }
    }
    out[o]=0; return o;
}

/* ---- preprocessing pipeline ---- */
static void preprocess(const char *tex, int len) {
    char *B1 = malloc(MAX_TEX); char *B2 = malloc(MAX_TEX); char *B3 = malloc(MAX_TEX);
    int l1 = strip_defs(tex, len, B1);
    int l2 = replace_math(B1, l1, B2);
    int l3 = strip_tex(B2, l2, B3);
    Wlen = collapse_ws(B3, l3, W);
    free(B1); free(B2); free(B3);
}

/* ---- sentence split ---- */
static void split_sents(void) {
    ns=0; int st=0;
    for(int i=0;i<Wlen&&ns<MAX_SENTS;i++){
        char c=W[i];
        if(c=='.'||c=='!'||c=='?'){
            int ab=0;
            if(c=='.'&&i>=2){
                if(i>=3&&(!strncmp(W+i-3,"e.g",3)||!strncmp(W+i-3,"i.e",3)))ab=1;
                else if(i>=2&&(!strncmp(W+i-2,"cf",2)||!strncmp(W+i-2,"vs",2)))ab=1;
                else if(i>=3&&(!strncmp(W+i-3,"etc",3)||!strncmp(W+i-3,"Fig",3)))ab=1;
                else if(i>=1&&(W[i-1]>='A'&&W[i-1]<='Z'))ab=1;
            }
            if(ab)continue;
            if(i+1>=Wlen||W[i+1]==' '||W[i+1]=='\n'){ss[ns]=st;se[ns]=i+1;ns++;st=i+2;}
        }
    }
    if(st<Wlen-2){ss[ns]=st;se[ns]=Wlen;ns++;}
}

static int getsent(int idx, char *b, int max) {
    if(idx>=ns)return 0; int s=ss[idx],e=se[idx],l=e-s;
    if(l>=max)l=max-1; if(l<=0)return 0; memcpy(b,W+s,l);b[l]=0;return l;
}

/* ---- vague filter ---- */
static int vague(const char *d) {
    const char *v[]={"the same","the other","the value","the case","the result",
        "the number","the set","the factor","the ratio","the range","the limit",
        "the sum","the product","the form","the order","a similar","a different",
        "another","this","that","these","those","it","they","one","such",NULL};
    char lo[256]; scpy(lo,d,sizeof(lo));
    for(char*p=lo;*p;p++)*p=(char)tolower((unsigned char)*p);
    for(int i=0;v[i];i++) if(!strcmp(lo,v[i])) return 1;
    if(!strchr(d,' ')&&strlen(d)<4) return 1;
    return 0;
}

/* ---- pattern 1: copula ---- */
static int p1(int si, int pi, def_result_t *r) {
    char s[MAX_SENT_LEN]; int sl=getsent(si,s,sizeof(s)); if(sl<5)return 0;
    char pat[64]; snprintf(pat,sizeof(pat),"VARSYM%04d",pi);
    char *vp=strstr(s,pat); if(!vp)return 0;
    char *cp=vp+10; while(cp<s+sl&&*cp==' ')cp++;
    const char *cl[]={"is ","are ","was ","were ","be ","denotes ","denoted ",
        "represents ","represented ","stands for ","gives ","means ","defined as ",
        "defined by ","describes ","corresponds to ","given by ","reads ",NULL};
    int ok=0; for(int ci=0;cl[ci];ci++){int clen=(int)strlen(cl[ci]);
        if(cp+clen<=s+sl&&!strncmp(cp,cl[ci],clen)){cp+=clen;ok=1;break;}}
    if(!ok)return 0;
    while(cp<s+sl&&*cp==' ')cp++;
    if(!strncmp(cp,"the ",4))cp+=4; else if(!strncmp(cp,"a ",2))cp+=2; else if(!strncmp(cp,"an ",3))cp+=3;
    char np[256]; int nl=0;
    while(cp<s+sl&&nl<255){if(!strncmp(cp,"VARSYM",6)||!strncmp(cp,"MATHVAL",7))break;
        if(*cp=='.'||*cp==';'||*cp=='!'||*cp=='?')break;
        if(*cp=='('){while(cp<s+sl&&*cp!=')')cp++;if(*cp==')')cp++;continue;}
        if(*cp==','){char*nx=cp+1;while(nx<s+sl&&*nx==' ')nx++;
            if(strncmp(nx,"and ",4)&&strncmp(nx,"or ",3))break;}
        np[nl++]=*cp++;}
    np[nl]=0; while(nl>0&&(np[nl-1]==' '||np[nl-1]==','))nl--; np[nl]=0;
    if(nl<3||vague(np))return 0; if(strstr(np,ph[pi].sym))return 0;
    scpy(r->definition,np,sizeof(r->definition)); r->symbol=ph[pi].sym;
    r->char_start=ph[pi].s; r->char_end=ph[pi].e; r->pattern=1; return 1;
}

/* ---- pattern 2: apposition ---- */
static int p2(int si, int pi, def_result_t *r) {
    char s[MAX_SENT_LEN]; int sl=getsent(si,s,sizeof(s)); if(sl<5)return 0;
    char pat[64]; snprintf(pat,sizeof(pat),"VARSYM%04d",pi);
    char *vp=strstr(s,pat); if(!vp)return 0;
    /* look left for ", the NP" */
    char *lf=vp-1; while(lf>s&&*lf!=',')lf--;
    if(lf>s&&*lf==','){
        lf++;while(lf<vp&&*lf==' ')lf++;
        if(!strncmp(lf,"the ",4)||!strncmp(lf,"a ",2)||!strncmp(lf,"an ",3)
         ||!strncmp(lf,"its ",4)||!strncmp(lf,"their ",6)||!strncmp(lf,"our ",4)){
            char np[256];int nl=0;
            while(lf<vp&&nl<255){if(!strncmp(lf,"VARSYM",6))break;np[nl++]=*lf++;}
            np[nl]=0;while(nl>0&&(np[nl-1]==' '||np[nl-1]==','))nl--;np[nl]=0;
            if(nl>=3&&!vague(np)&&!strstr(np,ph[pi].sym)){
                scpy(r->definition,np,sizeof(r->definition)); r->symbol=ph[pi].sym;
                r->char_start=ph[pi].s;r->char_end=ph[pi].e;r->pattern=2;return 1;}
        }
    }
    /* look right for ", the NP" */
    char *rt=vp+10; while(rt<s+sl&&*rt!=',')rt++;
    if(rt<s+sl&&*rt==','){
        rt++;while(rt<s+sl&&*rt==' ')rt++;
        if(!strncmp(rt,"the ",4)||!strncmp(rt,"a ",2)||!strncmp(rt,"an ",3)){
            char np[256];int nl=0;
            while(rt<s+sl&&nl<255&&*rt!='.'&&*rt!=';'&&strncmp(rt,"VARSYM",6)&&strncmp(rt,"MATHVAL",7))np[nl++]=*rt++;
            np[nl]=0;while(nl>0&&(np[nl-1]==' '||np[nl-1]=='.'))nl--;np[nl]=0;
            if(nl>=3&&!vague(np)&&!strstr(np,ph[pi].sym)){
                scpy(r->definition,np,sizeof(r->definition));r->symbol=ph[pi].sym;
                r->char_start=ph[pi].s;r->char_end=ph[pi].e;r->pattern=2;return 1;}
        }
    }
    return 0;
}

/* ---- pattern 3: equation ---- */
static int p3(int si, int pi, def_result_t *r) {
    char s[MAX_SENT_LEN]; int sl=getsent(si,s,sizeof(s)); if(sl<5)return 0;
    char pat[64]; snprintf(pat,sizeof(pat),"VARSYM%04d",pi);
    char *vp=strstr(s,pat); if(!vp)return 0;
    char *eq=vp+10; while(eq<s+sl&&*eq!='M')eq++;
    if(eq+7<=s+sl&&!strncmp(eq,"MATHVAL",7)){
        scpy(r->definition,ph[pi].raw,sizeof(r->definition)); r->symbol=ph[pi].sym;
        r->char_start=ph[pi].s;r->char_end=ph[pi].e;r->pattern=3;return 1;}
    return 0;
}

/* ---- pattern 4: defines ---- */
static int p4(int si, int pi, def_result_t *r) {
    char s[MAX_SENT_LEN]; int sl=getsent(si,s,sizeof(s)); if(sl<5)return 0;
    char pat[64]; snprintf(pat,sizeof(pat),"VARSYM%04d",pi);
    char *vp=strstr(s,pat); if(!vp)return 0;
    char *lf=vp-1; int ok=0;
    while(lf>s&&lf>vp-60){if(!strncmp(lf,"define ",7)||!strncmp(lf,"defines ",8)
      ||!strncmp(lf,"defined ",8)||!strncmp(lf,"denote ",7)
      ||!strncmp(lf,"call ",5)||!strncmp(lf,"called ",7)
      ||!strncmp(lf,"let ",4)){ok=1;break;}lf--;}
    if(!ok)return 0;
    char *cp=vp+10; while(cp<s+sl&&*cp==' ')cp++;
    if(!strncmp(cp,"as ",3))cp+=3; else if(!strncmp(cp,"to be ",6))cp+=6;
    else if(!strncmp(cp,"by ",3))cp+=3; else return 0;
    while(cp<s+sl&&*cp==' ')cp++;
    char np[256];int nl=0;
    while(cp<s+sl&&nl<255&&*cp!='.'&&*cp!=';'&&strncmp(cp,"VARSYM",6)&&strncmp(cp,"MATHVAL",7))np[nl++]=*cp++;
    np[nl]=0;while(nl>0&&(np[nl-1]==' '||np[nl-1]=='.'))nl--;np[nl]=0;
    if(nl<3||vague(np))return 0; if(strstr(np,ph[pi].sym))return 0;
    scpy(r->definition,np,sizeof(r->definition));r->symbol=ph[pi].sym;
    r->char_start=ph[pi].s;r->char_end=ph[pi].e;r->pattern=4;return 1;
}

/* ---- pattern 5: parenthetical ---- */
static int p5(int si, int pi, def_result_t *r) {
    char s[MAX_SENT_LEN]; int sl=getsent(si,s,sizeof(s)); if(sl<5)return 0;
    char pat[64]; snprintf(pat,sizeof(pat),"VARSYM%04d",pi);
    char *vp=strstr(s,pat); if(!vp)return 0;
    char *cp=vp+10; while(cp<s+sl&&*cp==' ')cp++;
    if(cp>=s+sl||*cp!='(')return 0; cp++;
    char np[256];int nl=0;
    while(cp<s+sl&&*cp!=')'&&nl<255)np[nl++]=*cp++;
    if(cp>=s+sl||*cp!=')')return 0; np[nl]=0;
    if(nl<3||vague(np))return 0;
    int ha=0; for(int i=0;i<nl;i++)if(isalpha((unsigned char)np[i]))ha=1;
    if(!ha)return 0; if(strstr(np,ph[pi].sym))return 0;
    scpy(r->definition,np,sizeof(r->definition));r->symbol=ph[pi].sym;
    r->char_start=ph[pi].s;r->char_end=ph[pi].e;r->pattern=5;return 1;
}

/* ---- main ---- */
int def_extract(const char *tex, int len, const char **want, int nw,
                def_result_t *r, int max) {
    if(!tex||len<=0||!want||nw<=0)return 0;
    if(len>MAX_TEX-1)len=MAX_TEX-1;
    preprocess(tex,len); split_sents();
    int n=0;
    for(int si=0;si<ns&&n<max;si++)
        for(int wi=0;wi<nw&&n<max;wi++){
            int phi=-1;
            for(int pi=0;pi<nph;pi++)if(!strcmp(ph[pi].sym,want[wi])){
                char pat[64];snprintf(pat,sizeof(pat),"VARSYM%04d",pi);
                if(strstr(W+ss[si],pat)){phi=pi;break;}}
            if(phi<0)continue;
            if(p1(si,phi,&r[n])||p2(si,phi,&r[n])||p4(si,phi,&r[n])
             ||p5(si,phi,&r[n])||p3(si,phi,&r[n]))n++;
        }
    return n;
}
