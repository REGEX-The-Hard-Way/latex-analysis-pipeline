/*
 * sidecar_mod.c — mmap-based sidecar.json rule engine.
 *
 * Uses json_scan Ragel scanner for proper JSON field extraction (handles
 * escape sequences, nested braces, and embedded quotes correctly).
 *
 * Usage: ./sidecar_mod input.json rules.txt output.json [undo.log]
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "json_scan.h"

#define MAX_LINE  16384
#define MAX_RULES 64

typedef struct { const char *name,*match_type,*prop,*transform,*pattern,*replace; regex_t c; int ok; } rule_t;
static rule_t rules[MAX_RULES]; static int n_rules;

static int json_get_str2(const char *l,const char*k,char*o,int m){
    char p[128];snprintf(p,sizeof(p),"\"%s\":\"",k);const char*s=strstr(l,p);if(!s)return 0;s+=strlen(p);
    int i=0;while(*s&&*s!='"'&&i<m-1){if(*s=='\\'&&s[1]){s++;o[i++]=*s++;}else o[i++]=*s++;}o[i]=0;return 1;
}
static int json_get_int2(const char *l,const char*k,int*o){
    char p[128];snprintf(p,sizeof(p),"\"%s\":",k);const char*s=strstr(l,p);if(!s)return 0;*o=atoi(s+strlen(p));return 1;
}
static int apply_transform(const char*v,int vl,const rule_t*r,char*o,int mo){
    if(!strcmp(r->transform,"regex")&&r->ok){regmatch_t g[16];if(regexec(&r->c,v,16,g,0))return -1;
        int oi=0;const char*rp=r->replace;while(*rp&&oi<mo-1){if(*rp=='$'&&rp[1]>='1'&&rp[1]<='9'){
            int gn=rp[1]-'0';rp+=2;if(gn<16&&g[gn].rm_so>=0){int gl=g[gn].rm_eo-g[gn].rm_so;
            if(oi+gl>=mo)gl=mo-oi-1;memcpy(o+oi,v+g[gn].rm_so,gl);oi+=gl;}}else o[oi++]=*rp++;}o[oi]=0;
        return(oi==vl&&!memcmp(o,v,oi))?-1:oi;}
    if(!strcmp(r->transform,"strip_prefix")){int pl=strlen(r->pattern);if(pl>vl||strncmp(v,r->pattern,pl))return-1;
        int nl=vl-pl;if(nl>=mo)nl=mo-1;memcpy(o,v+pl,nl);o[nl]=0;return nl;}
    if(!strcmp(r->transform,"strip_suffix")){int sl=strlen(r->pattern);if(sl>vl||strcmp(v+vl-sl,r->pattern))return-1;
        int nl=vl-sl;if(nl>=mo)nl=mo-1;memcpy(o,v,nl);o[nl]=0;return nl;}
    if(!strcmp(r->transform,"trim")){const char*s=v,*e=v+vl;
        while(s<e&&(*s==' '||*s=='\t'||*s=='\n'))s++;while(e>s&&(*(e-1)==' '||*(e-1)=='\t'||*(e-1)=='\n'))e--;
        int nl=e-s;if(nl>=mo)nl=mo-1;memcpy(o,s,nl);o[nl]=0;return nl;}return-1;
}

/* ---- record accumulator ---- */
#define MAX_FIELDS 16
typedef struct { char key[64],val[4096]; int vlen,vtype; } field_t;
typedef struct { field_t f[MAX_FIELDS]; int nf; } record_t;

static record_t cur_rec;
static FILE *g_out, *g_undo;
static int g_changed, g_records, g_undo_seq;

static int esc_append(char *buf, int off, int max, const char *s, int len) {
    for (int i = 0; i < len && off < max - 2; i++) {
        if (s[i] == '\\') { buf[off++] = '\\'; buf[off++] = '\\'; }
        else if (s[i] == '"') { buf[off++] = '\\'; buf[off++] = '"'; }
        else if (s[i] == '\n') { buf[off++] = '\\'; buf[off++] = 'n'; }
        else if (s[i] == '\t') { buf[off++] = '\\'; buf[off++] = 't'; }
        else buf[off++] = s[i];
    }
    return off;
}

static void rec_collect(const char *key,int klen,const char *val,int vlen,int vtype,void*u){
    (void)u;
    if (!key) {
        /* record complete — apply rules and write */
        g_records++;
        char line[MAX_LINE];
        int off=0;
        off+=snprintf(line+off,sizeof(line)-off,"{");
        for(int i=0;i<cur_rec.nf;i++){
            if(i>0)off+=snprintf(line+off,sizeof(line)-off,",");
            off+=snprintf(line+off,sizeof(line)-off,"\"%s\":",cur_rec.f[i].key);
            if(cur_rec.f[i].vtype==1)
                off+=snprintf(line+off,sizeof(line)-off,"%s",cur_rec.f[i].val);
            else {
                off+=snprintf(line+off,sizeof(line)-off,"\"");
                off=esc_append(line,off,sizeof(line),cur_rec.f[i].val,cur_rec.f[i].vlen);
                off+=snprintf(line+off,sizeof(line)-off,"\"");
            }
        }
        off+=snprintf(line+off,sizeof(line)-off,"}");

        /* apply rules */
        char type[128]="";
        json_get_str2(line,"type",type,sizeof(type));
        int modified=0;
        for(int ri=0;ri<n_rules;ri++){
            if(rules[ri].match_type&&rules[ri].match_type[0]&&strcmp(type,rules[ri].match_type))continue;
            char pval[4096]="";
            if(!json_get_str2(line,rules[ri].prop,pval,sizeof(pval)))continue;
            int pvl=strlen(pval);
            char result[4096];int nl=apply_transform(pval,pvl,&rules[ri],result,sizeof(result));
            if(nl<0)continue;
            if(g_undo){
                fprintf(g_undo,"{\"step\":%d,\"name\":\"%s\",\"record_line\":%d,",g_undo_seq++,rules[ri].name,g_records);
                fprintf(g_undo,"\"prop\":\"%s\",\"original\":\"",rules[ri].prop);
                for(int i=0;pval[i];i++){if(pval[i]=='\\')fprintf(g_undo,"\\\\");else if(pval[i]=='"')fprintf(g_undo,"\\\"");else fputc(pval[i],g_undo);}
                fprintf(g_undo,"\",\"transformed\":\"");
                for(int i=0;i<nl;i++){if(result[i]=='\\')fprintf(g_undo,"\\\\");else if(result[i]=='"')fprintf(g_undo,"\\\"");else fputc(result[i],g_undo);}
                fprintf(g_undo,"\"}\n");
            }
            /* rebuild line with replaced value */
            char pat[128];snprintf(pat,sizeof(pat),"\"%s\":\"",rules[ri].prop);
            char*ps=strstr(line,pat);if(!ps)continue;ps+=strlen(pat);
            char*pe=ps;while(*pe&&*pe!='"'){if(*pe=='\\'&&pe[1])pe++;pe++;}
            char*tail=pe;
            char tmp[MAX_LINE];int pre_len=(int)(ps-line);
            memcpy(tmp,line,pre_len);if(pre_len+nl>=MAX_LINE-1)nl=MAX_LINE-pre_len-1;
            memcpy(tmp+pre_len,result,nl);int post_off=pre_len+nl;
            int tail_len=off-(int)(tail-line);
            if(tail_len>0&&post_off+tail_len<MAX_LINE-1)memcpy(tmp+post_off,tail,tail_len);
            tmp[post_off+tail_len]=0;off=post_off+tail_len;
            memcpy(line,tmp,off+1);modified=1;
        }
        fwrite(line,1,off,g_out);fputc('\n',g_out);
        if(modified)g_changed++;
        cur_rec.nf=0;
        return;
    }
    /* accumulate field */
    if(cur_rec.nf<MAX_FIELDS){
        int nl=klen<63?klen:63;memcpy(cur_rec.f[cur_rec.nf].key,key,nl);cur_rec.f[cur_rec.nf].key[nl]=0;
        int vl=vlen<4095?vlen:4095;memcpy(cur_rec.f[cur_rec.nf].val,val,vl);cur_rec.f[cur_rec.nf].val[vl]=0;
        cur_rec.f[cur_rec.nf].vlen=vl;cur_rec.f[cur_rec.nf].vtype=vtype;cur_rec.nf++;
    }
}

int main(int argc,char*argv[]){
    if(argc<4){fprintf(stderr,"Usage: %s in.json rules.txt out.json [undo.log]\n",argv[0]);return 1;}
    FILE*rf=fopen(argv[2],"r");if(!rf)return 1;
    char rl[4096];
    while(fgets(rl,sizeof(rl),rf)&&n_rules<MAX_RULES){if(rl[0]!='{')continue;char b[256];
        if(json_get_str2(rl,"name",b,sizeof(b)))rules[n_rules].name=strdup(b);
        if(json_get_str2(rl,"match_type",b,sizeof(b)))rules[n_rules].match_type=strdup(b);
        if(json_get_str2(rl,"prop",b,sizeof(b)))rules[n_rules].prop=strdup(b);
        if(json_get_str2(rl,"transform",b,sizeof(b)))rules[n_rules].transform=strdup(b);
        if(json_get_str2(rl,"pattern",b,sizeof(b)))rules[n_rules].pattern=strdup(b);
        if(json_get_str2(rl,"replace",b,sizeof(b)))rules[n_rules].replace=strdup(b);
        if(rules[n_rules].pattern&&rules[n_rules].transform&&!strcmp(rules[n_rules].transform,"regex"))
            rules[n_rules].ok=!regcomp(&rules[n_rules].c,rules[n_rules].pattern,REG_EXTENDED);n_rules++;}
    fclose(rf);
    g_out=fopen(argv[3],"w");g_undo=argc>=5?fopen(argv[4],"w"):NULL;
    int n=json_scan_file(argv[1],rec_collect,NULL);
    fclose(g_out);if(g_undo)fclose(g_undo);
    printf("Loaded %d rules, processed %d records, %d changed\n",n_rules,g_records,g_changed);
    return 0;
}
