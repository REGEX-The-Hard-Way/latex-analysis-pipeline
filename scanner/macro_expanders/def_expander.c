#line 1 "macro_expanders/def_expander.rl"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../cypher/vendor/klib/khashl.h"

#define PARAM_BASE 0x01
static int n;

typedef struct {
	char *body;
	int body_len;
	int argc;
} macro_val_t;

static kh_inline khint_t str_hash(const char *s) {
	khint_t h = 0;
	while (*s) h = (h << 5) - h + (unsigned char)*s++;
	return h;
}
static kh_inline int str_eq(const char *a, const char *b) {
	return strcmp(a, b) == 0;
}

KHASHL_MAP_INIT(static, macro_map_t, macro_map, const char*, macro_val_t, str_hash, str_eq);

static macro_map_t *macros = NULL;
static char *input = NULL;
static long input_len = 0;

static int macro_exists(macro_map_t *h, const char *key) {
	if (!h || !h->keys) return 0;
		khint_t k = macro_map_get(h, key);
	khint_t nb = (khint_t)1U << h->bits;
	return k < nb;
}

static void store_macro(const char *name, int name_len, const char *body_start, const char *body_end, int argc) {
	if (!macros) macros = macro_map_init();
		
	char *name_dup = (char*)malloc(name_len + 1);
	memcpy(name_dup, name, name_len);
	name_dup[name_len] = '\0';
	
	int body_raw_len = (int)(body_end - body_start);
	char *body_buf = (char*)malloc(body_raw_len + 1);
	int j = 0;
	for (int i = 0; i < body_raw_len; i++) {
		char c = body_start[i];
		if (c == '#' && i + 1 < body_raw_len) {
			if (body_start[i+1] == '#') {
				body_buf[j++] = '#';
				i++;
			} else if (body_start[i+1] >= '1' && body_start[i+1] <= '9') {
				body_buf[j++] = PARAM_BASE + (body_start[i+1] - '1');
				i++;
			} else {
				body_buf[j++] = c;
			}
		} else {
			body_buf[j++] = c;
		}
	}
	body_buf[j] = '\0';
	
	macro_val_t val = { body_buf, j, argc };
	int absent;
	khint_t k = macro_map_put(macros, name_dup, &absent);
	macros->keys[k].val = val;
}

static int body_has_at(const char *s, int len) {
	for (int i = 0; i < len - 1; i++) {
		if (s[i] == '\\' && s[i+1] == '@') return 1;
		}
	return 0;
}



#line 199 "macro_expanders/def_expander.rl"



#line 84 "macro_expanders/def_expander.c"
static const int def_expander_start = 136;
static const int def_expander_first_final = 136;
static const int def_expander_error = -1;

static const int def_expander_en_main = 136;


#line 201 "macro_expanders/def_expander.rl"

int main(int argc, char **argv) {
	int ch;
	long cap = 65536;
	input = (char*)malloc(cap);
	input_len = 0;
	while ((ch = fgetc(stdin)) != EOF) {
		if (input_len >= cap - 1) {
			cap *= 2;
			input = (char*)realloc(input, cap);
		}
		input[input_len++] = (char)ch;
	}
	input[input_len] = '\0';
	
	int cs = 0;
	const char *p = input;
	const char *pe = input + input_len;
	const char *eof = pe;
	const char *ts = NULL, *te = NULL;
	

#line 112 "macro_expanders/def_expander.c"
	{
		cs = (int)def_expander_start;
		ts = 0;
		te = 0;
	}
	
#line 222 "macro_expanders/def_expander.rl"


#line 119 "macro_expanders/def_expander.c"
{
		int _ck;
		switch ( cs ) {
			case 136:
			goto st_case_136;
			case 137:
			goto st_case_137;
			case 0:
			goto st_case_0;
			case 1:
			goto st_case_1;
			case 2:
			goto st_case_2;
			case 3:
			goto st_case_3;
			case 4:
			goto st_case_4;
			case 5:
			goto st_case_5;
			case 6:
			goto st_case_6;
			case 7:
			goto st_case_7;
			case 8:
			goto st_case_8;
			case 9:
			goto st_case_9;
			case 10:
			goto st_case_10;
			case 11:
			goto st_case_11;
			case 12:
			goto st_case_12;
			case 13:
			goto st_case_13;
			case 14:
			goto st_case_14;
			case 15:
			goto st_case_15;
			case 16:
			goto st_case_16;
			case 17:
			goto st_case_17;
			case 18:
			goto st_case_18;
			case 19:
			goto st_case_19;
			case 20:
			goto st_case_20;
			case 21:
			goto st_case_21;
			case 22:
			goto st_case_22;
			case 23:
			goto st_case_23;
			case 24:
			goto st_case_24;
			case 25:
			goto st_case_25;
			case 26:
			goto st_case_26;
			case 27:
			goto st_case_27;
			case 28:
			goto st_case_28;
			case 29:
			goto st_case_29;
			case 30:
			goto st_case_30;
			case 31:
			goto st_case_31;
			case 32:
			goto st_case_32;
			case 33:
			goto st_case_33;
			case 34:
			goto st_case_34;
			case 35:
			goto st_case_35;
			case 36:
			goto st_case_36;
			case 37:
			goto st_case_37;
			case 38:
			goto st_case_38;
			case 39:
			goto st_case_39;
			case 40:
			goto st_case_40;
			case 41:
			goto st_case_41;
			case 42:
			goto st_case_42;
			case 43:
			goto st_case_43;
			case 44:
			goto st_case_44;
			case 45:
			goto st_case_45;
			case 46:
			goto st_case_46;
			case 47:
			goto st_case_47;
			case 48:
			goto st_case_48;
			case 49:
			goto st_case_49;
			case 50:
			goto st_case_50;
			case 51:
			goto st_case_51;
			case 52:
			goto st_case_52;
			case 53:
			goto st_case_53;
			case 54:
			goto st_case_54;
			case 55:
			goto st_case_55;
			case 56:
			goto st_case_56;
			case 57:
			goto st_case_57;
			case 58:
			goto st_case_58;
			case 59:
			goto st_case_59;
			case 60:
			goto st_case_60;
			case 61:
			goto st_case_61;
			case 62:
			goto st_case_62;
			case 63:
			goto st_case_63;
			case 64:
			goto st_case_64;
			case 65:
			goto st_case_65;
			case 66:
			goto st_case_66;
			case 67:
			goto st_case_67;
			case 68:
			goto st_case_68;
			case 69:
			goto st_case_69;
			case 70:
			goto st_case_70;
			case 71:
			goto st_case_71;
			case 72:
			goto st_case_72;
			case 73:
			goto st_case_73;
			case 74:
			goto st_case_74;
			case 75:
			goto st_case_75;
			case 76:
			goto st_case_76;
			case 77:
			goto st_case_77;
			case 78:
			goto st_case_78;
			case 79:
			goto st_case_79;
			case 80:
			goto st_case_80;
			case 81:
			goto st_case_81;
			case 82:
			goto st_case_82;
			case 83:
			goto st_case_83;
			case 84:
			goto st_case_84;
			case 85:
			goto st_case_85;
			case 86:
			goto st_case_86;
			case 87:
			goto st_case_87;
			case 88:
			goto st_case_88;
			case 89:
			goto st_case_89;
			case 90:
			goto st_case_90;
			case 91:
			goto st_case_91;
			case 92:
			goto st_case_92;
			case 93:
			goto st_case_93;
			case 94:
			goto st_case_94;
			case 95:
			goto st_case_95;
			case 96:
			goto st_case_96;
			case 97:
			goto st_case_97;
			case 98:
			goto st_case_98;
			case 99:
			goto st_case_99;
			case 138:
			goto st_case_138;
			case 139:
			goto st_case_139;
			case 140:
			goto st_case_140;
			case 141:
			goto st_case_141;
			case 142:
			goto st_case_142;
			case 100:
			goto st_case_100;
			case 101:
			goto st_case_101;
			case 102:
			goto st_case_102;
			case 103:
			goto st_case_103;
			case 104:
			goto st_case_104;
			case 105:
			goto st_case_105;
			case 106:
			goto st_case_106;
			case 107:
			goto st_case_107;
			case 143:
			goto st_case_143;
			case 144:
			goto st_case_144;
			case 145:
			goto st_case_145;
			case 146:
			goto st_case_146;
			case 147:
			goto st_case_147;
			case 148:
			goto st_case_148;
			case 149:
			goto st_case_149;
			case 150:
			goto st_case_150;
			case 151:
			goto st_case_151;
			case 152:
			goto st_case_152;
			case 108:
			goto st_case_108;
			case 109:
			goto st_case_109;
			case 110:
			goto st_case_110;
			case 111:
			goto st_case_111;
			case 112:
			goto st_case_112;
			case 153:
			goto st_case_153;
			case 154:
			goto st_case_154;
			case 155:
			goto st_case_155;
			case 156:
			goto st_case_156;
			case 157:
			goto st_case_157;
			case 158:
			goto st_case_158;
			case 113:
			goto st_case_113;
			case 114:
			goto st_case_114;
			case 115:
			goto st_case_115;
			case 116:
			goto st_case_116;
			case 117:
			goto st_case_117;
			case 118:
			goto st_case_118;
			case 119:
			goto st_case_119;
			case 120:
			goto st_case_120;
			case 121:
			goto st_case_121;
			case 122:
			goto st_case_122;
			case 123:
			goto st_case_123;
			case 124:
			goto st_case_124;
			case 125:
			goto st_case_125;
			case 126:
			goto st_case_126;
			case 127:
			goto st_case_127;
			case 128:
			goto st_case_128;
			case 129:
			goto st_case_129;
			case 130:
			goto st_case_130;
			case 131:
			goto st_case_131;
			case 132:
			goto st_case_132;
			case 133:
			goto st_case_133;
			case 134:
			goto st_case_134;
			case 135:
			goto st_case_135;
			case 159:
			goto st_case_159;
			case 160:
			goto st_case_160;
			case 161:
			goto st_case_161;
			case 162:
			goto st_case_162;
			case 163:
			goto st_case_163;
			case 164:
			goto st_case_164;
			case 165:
			goto st_case_165;
			case 166:
			goto st_case_166;
			case 167:
			goto st_case_167;
			case 168:
			goto st_case_168;
			case 169:
			goto st_case_169;
			case 170:
			goto st_case_170;
			case 171:
			goto st_case_171;
			case 172:
			goto st_case_172;
			case 173:
			goto st_case_173;
			case 174:
			goto st_case_174;
			case 175:
			goto st_case_175;
			case 176:
			goto st_case_176;
			case 177:
			goto st_case_177;
			case 178:
			goto st_case_178;
			case 179:
			goto st_case_179;
			case 180:
			goto st_case_180;
			case 181:
			goto st_case_181;
			case 182:
			goto st_case_182;
			case 183:
			goto st_case_183;
			case 184:
			goto st_case_184;
			case 185:
			goto st_case_185;
			case 186:
			goto st_case_186;
			case 187:
			goto st_case_187;
			case 188:
			goto st_case_188;
			case 189:
			goto st_case_189;
			case 190:
			goto st_case_190;
			case 191:
			goto st_case_191;
			case 192:
			goto st_case_192;
			case 193:
			goto st_case_193;
			case 194:
			goto st_case_194;
			case 195:
			goto st_case_195;
			case 196:
			goto st_case_196;
			case 197:
			goto st_case_197;
			case 198:
			goto st_case_198;
			case 199:
			goto st_case_199;
			case 200:
			goto st_case_200;
			case 201:
			goto st_case_201;
			case 202:
			goto st_case_202;
			case 203:
			goto st_case_203;
			case 204:
			goto st_case_204;
			case 205:
			goto st_case_205;
			case 206:
			goto st_case_206;
			case 207:
			goto st_case_207;
			case 208:
			goto st_case_208;
			case 209:
			goto st_case_209;
		}
		_ctr113:
			{
#line 99 "macro_expanders/def_expander.rl"
			{te = p+1;{
#line 99 "macro_expanders/def_expander.rl"
					
					fwrite(ts, 1, te - ts, stdout);
					const char *p = ts;
					const char *end = te;
					
					while (p < end) {
						if (*p == '\\' && end - p >= 4) {
							if ((p[1]=='d'||p[1]=='D') && (p[2]=='e'||p[2]=='E') && (p[3]=='f'||p[3]=='F')) { p += 4; break; }
							if ((p[1]=='g'||p[1]=='G') && (p[2]=='d'||p[2]=='D') && (p[3]=='e'||p[3]=='E') && (p[4]=='f'||p[4]=='F')) { p += 5; break; }
						}
						p++;
					}
					
					while (p < end && (*p == ' ' || *p == '\t')) p++;
					if (p < end && *p == '\\') p++;
					const char *ns = p;
					while (p < end && (isalpha(*p) || *p == '@')) p++;
					const char *ne = p;
					
					while (p < end && (*p == ' ' || *p == '\t')) p++;
					int argc = 0;
					while (p + 1 < end && *p == '#' && isdigit(p[1])) { argc++; p += 2; }
					
					while (p < end && (*p == ' ' || *p == '\t')) p++;
					if (p < end && *p == '{') {
							const char *bs = p + 1;
							const char *be = end - 1;
							if (!body_has_at(bs, (int)(be - bs))) store_macro(ns, (int)(ne - ns), bs, be, argc);
						}
					}
				}}
			
#line 580 "macro_expanders/def_expander.c"

			goto _st136;
			_ctr0:
				{
#line 196 "macro_expanders/def_expander.rl"
				{p = ((te))-1;
					{
#line 196 "macro_expanders/def_expander.rl"
						fputc(*ts, stdout); }
				}}
			
#line 591 "macro_expanders/def_expander.c"

			goto _st136;
			_ctr2:
				{
#line 194 "macro_expanders/def_expander.rl"
				{te = p+1;{
#line 194 "macro_expanders/def_expander.rl"
						fwrite(ts, 1, te - ts, stdout); }
				}}
			
#line 601 "macro_expanders/def_expander.c"

			goto _st136;
			_ctr101:
				{
#line 137 "macro_expanders/def_expander.rl"
				{p = ((te))-1;
					{
#line 137 "macro_expanders/def_expander.rl"
						
						int nl = (int)(te - ts - 1);
						int did_expand = 0;
						if (nl > 0 && nl < 256 && macros && macros->keys) {
							char nb[256];
							memcpy(nb, ts + 1, nl);
							nb[nl] = '\0';
							khint_t k = macro_map_get(macros, nb);
							khint_t nbk = (khint_t)1U << macros->bits;
							if (k < nbk) {
								macro_val_t mv = macros->keys[k].val;
								const char *sc = te;
								char *args[9] = {0};
								int ga = 0;
								for (int i = 0; i < mv.argc; i++) {
									while (*sc == ' ' || *sc == '\t' || *sc == '\n') sc++;
									if (*sc == '{') {
											sc++;
											const char *as = sc;
											int d = 1;
											while (*sc && d > 0) {
												if (*sc == '{') d++;
													else if (*sc == '}') { d--; if (d == 0) break; }
												sc++;
											}
											int al = (int)(sc - as);
											args[i] = (char*)malloc(al + 1);
											memcpy(args[i], as, al);
											args[i][al] = '\0';
											if (*sc) sc++;
										} else {
											args[i] = (char*)malloc(2);
											args[i][0] = *sc;
											args[i][1] = '\0';
											if (*sc) sc++;
										}
										ga = i + 1;
									}
									for (int i = 0; i < mv.body_len; i++) {
										unsigned char c = (unsigned char)mv.body[i];
										if (c >= PARAM_BASE && c < PARAM_BASE + 9) {
											int pi = c - PARAM_BASE;
											if (pi < ga && args[pi]) fputs(args[pi], stdout);
										} else {
											fputc(c, stdout);
										}
									}
									for (int i = 0; i < ga; i++) free(args[i]);
									did_expand = 1;
									{p = (( sc))-1;}
									
								}
							}
							if (!did_expand) {
								fwrite(ts, 1, te - ts, stdout);
							}
						}
					}}
				
#line 668 "macro_expanders/def_expander.c"

				goto _st136;
				_ctr143:
					{
#line 196 "macro_expanders/def_expander.rl"
					{te = p+1;{
#line 196 "macro_expanders/def_expander.rl"
							fputc(*ts, stdout); }
					}}
				
#line 678 "macro_expanders/def_expander.c"

				goto _st136;
				_ctr146:
					{
#line 196 "macro_expanders/def_expander.rl"
					{te = p;p = p - 1;{
#line 196 "macro_expanders/def_expander.rl"
							fputc(*ts, stdout); }
					}}
				
#line 688 "macro_expanders/def_expander.c"

				goto _st136;
				_ctr158:
					{
#line 137 "macro_expanders/def_expander.rl"
					{te = p;p = p - 1;{
#line 137 "macro_expanders/def_expander.rl"
							
							int nl = (int)(te - ts - 1);
							int did_expand = 0;
							if (nl > 0 && nl < 256 && macros && macros->keys) {
								char nb[256];
								memcpy(nb, ts + 1, nl);
								nb[nl] = '\0';
								khint_t k = macro_map_get(macros, nb);
								khint_t nbk = (khint_t)1U << macros->bits;
								if (k < nbk) {
									macro_val_t mv = macros->keys[k].val;
									const char *sc = te;
									char *args[9] = {0};
									int ga = 0;
									for (int i = 0; i < mv.argc; i++) {
										while (*sc == ' ' || *sc == '\t' || *sc == '\n') sc++;
										if (*sc == '{') {
												sc++;
												const char *as = sc;
												int d = 1;
												while (*sc && d > 0) {
													if (*sc == '{') d++;
														else if (*sc == '}') { d--; if (d == 0) break; }
													sc++;
												}
												int al = (int)(sc - as);
												args[i] = (char*)malloc(al + 1);
												memcpy(args[i], as, al);
												args[i][al] = '\0';
												if (*sc) sc++;
											} else {
												args[i] = (char*)malloc(2);
												args[i][0] = *sc;
												args[i][1] = '\0';
												if (*sc) sc++;
											}
											ga = i + 1;
										}
										for (int i = 0; i < mv.body_len; i++) {
											unsigned char c = (unsigned char)mv.body[i];
											if (c >= PARAM_BASE && c < PARAM_BASE + 9) {
												int pi = c - PARAM_BASE;
												if (pi < ga && args[pi]) fputs(args[pi], stdout);
											} else {
												fputc(c, stdout);
											}
										}
										for (int i = 0; i < ga; i++) free(args[i]);
										did_expand = 1;
										{p = (( sc))-1;}
										
									}
								}
								if (!did_expand) {
									fwrite(ts, 1, te - ts, stdout);
								}
							}
						}}
					
#line 754 "macro_expanders/def_expander.c"

					goto _st136;
					_ctr164:
						{
#line 133 "macro_expanders/def_expander.rl"
						{te = p;p = p - 1;{
#line 133 "macro_expanders/def_expander.rl"
								fwrite(ts, 1, te - ts, stdout); }
						}}
					
#line 764 "macro_expanders/def_expander.c"

					goto _st136;
					_ctr173:
						{
#line 135 "macro_expanders/def_expander.rl"
						{te = p;p = p - 1;{
#line 135 "macro_expanders/def_expander.rl"
								fwrite(ts, 1, te - ts, stdout); }
						}}
					
#line 774 "macro_expanders/def_expander.c"

					goto _st136;
					_ctr185:
						{
#line 130 "macro_expanders/def_expander.rl"
						{te = p;p = p - 1;{
#line 130 "macro_expanders/def_expander.rl"
								fwrite(ts, 1, te - ts, stdout); }
						}}
					
#line 784 "macro_expanders/def_expander.c"

					goto _st136;
					_ctr209:
						{
#line 132 "macro_expanders/def_expander.rl"
						{te = p;p = p - 1;{
#line 132 "macro_expanders/def_expander.rl"
								fwrite(ts, 1, te - ts, stdout); }
						}}
					
#line 794 "macro_expanders/def_expander.c"

					goto _st136;
					_ctr221:
						{
#line 131 "macro_expanders/def_expander.rl"
						{te = p;p = p - 1;{
#line 131 "macro_expanders/def_expander.rl"
								fwrite(ts, 1, te - ts, stdout); }
						}}
					
#line 804 "macro_expanders/def_expander.c"

					goto _st136;
					_ctr225:
						{
#line 134 "macro_expanders/def_expander.rl"
						{te = p;p = p - 1;{
#line 134 "macro_expanders/def_expander.rl"
								fwrite(ts, 1, te - ts, stdout); }
						}}
					
#line 814 "macro_expanders/def_expander.c"

					goto _st136;
					_st136:
					if ( p == eof )
						goto _out136;
						{
#line 1 "NONE"
						{ts = 0;}}
					
#line 823 "macro_expanders/def_expander.c"

					p+= 1;
					st_case_136:
					if ( p == pe && p != eof )
						goto _out136;
						{
#line 1 "NONE"
						{ts = p;}}
					
#line 832 "macro_expanders/def_expander.c"

					if ( p == eof ) {
						goto _st136;}
					else {
						switch( ( (*( p))) ) {
							case 37: {
								goto _ctr144;
							}
							case 92: {
								goto _st138;
							}
						}
						goto _ctr143;
					}
					_ctr144:
						{
#line 1 "NONE"
						{te = p+1;}}
					
#line 851 "macro_expanders/def_expander.c"

					goto _st137;
					_st137:
					if ( p == eof )
						goto _out137;
					p+= 1;
					st_case_137:
					if ( p == pe && p != eof )
						goto _out137;
					if ( p == eof ) {
						goto _ctr146;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st0;
					}
					_st0:
					if ( p == eof )
						goto _out0;
					p+= 1;
					st_case_0:
					if ( p == pe && p != eof )
						goto _out0;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st1;
					}
					_st1:
					if ( p == eof )
						goto _out1;
					p+= 1;
					st_case_1:
					if ( p == pe && p != eof )
						goto _out1;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st2;
					}
					_st2:
					if ( p == eof )
						goto _out2;
					p+= 1;
					st_case_2:
					if ( p == pe && p != eof )
						goto _out2;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st3;
					}
					_st3:
					if ( p == eof )
						goto _out3;
					p+= 1;
					st_case_3:
					if ( p == pe && p != eof )
						goto _out3;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st4;
					}
					_st4:
					if ( p == eof )
						goto _out4;
					p+= 1;
					st_case_4:
					if ( p == pe && p != eof )
						goto _out4;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st5;
					}
					_st5:
					if ( p == eof )
						goto _out5;
					p+= 1;
					st_case_5:
					if ( p == pe && p != eof )
						goto _out5;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st6;
					}
					_st6:
					if ( p == eof )
						goto _out6;
					p+= 1;
					st_case_6:
					if ( p == pe && p != eof )
						goto _out6;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st7;
					}
					_st7:
					if ( p == eof )
						goto _out7;
					p+= 1;
					st_case_7:
					if ( p == pe && p != eof )
						goto _out7;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st8;
					}
					_st8:
					if ( p == eof )
						goto _out8;
					p+= 1;
					st_case_8:
					if ( p == pe && p != eof )
						goto _out8;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st9;
					}
					_st9:
					if ( p == eof )
						goto _out9;
					p+= 1;
					st_case_9:
					if ( p == pe && p != eof )
						goto _out9;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st10;
					}
					_st10:
					if ( p == eof )
						goto _out10;
					p+= 1;
					st_case_10:
					if ( p == pe && p != eof )
						goto _out10;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st11;
					}
					_st11:
					if ( p == eof )
						goto _out11;
					p+= 1;
					st_case_11:
					if ( p == pe && p != eof )
						goto _out11;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st12;
					}
					_st12:
					if ( p == eof )
						goto _out12;
					p+= 1;
					st_case_12:
					if ( p == pe && p != eof )
						goto _out12;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st13;
					}
					_st13:
					if ( p == eof )
						goto _out13;
					p+= 1;
					st_case_13:
					if ( p == pe && p != eof )
						goto _out13;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st14;
					}
					_st14:
					if ( p == eof )
						goto _out14;
					p+= 1;
					st_case_14:
					if ( p == pe && p != eof )
						goto _out14;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st15;
					}
					_st15:
					if ( p == eof )
						goto _out15;
					p+= 1;
					st_case_15:
					if ( p == pe && p != eof )
						goto _out15;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st16;
					}
					_st16:
					if ( p == eof )
						goto _out16;
					p+= 1;
					st_case_16:
					if ( p == pe && p != eof )
						goto _out16;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st17;
					}
					_st17:
					if ( p == eof )
						goto _out17;
					p+= 1;
					st_case_17:
					if ( p == pe && p != eof )
						goto _out17;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st18;
					}
					_st18:
					if ( p == eof )
						goto _out18;
					p+= 1;
					st_case_18:
					if ( p == pe && p != eof )
						goto _out18;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st19;
					}
					_st19:
					if ( p == eof )
						goto _out19;
					p+= 1;
					st_case_19:
					if ( p == pe && p != eof )
						goto _out19;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st20;
					}
					_st20:
					if ( p == eof )
						goto _out20;
					p+= 1;
					st_case_20:
					if ( p == pe && p != eof )
						goto _out20;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st21;
					}
					_st21:
					if ( p == eof )
						goto _out21;
					p+= 1;
					st_case_21:
					if ( p == pe && p != eof )
						goto _out21;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st22;
					}
					_st22:
					if ( p == eof )
						goto _out22;
					p+= 1;
					st_case_22:
					if ( p == pe && p != eof )
						goto _out22;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st23;
					}
					_st23:
					if ( p == eof )
						goto _out23;
					p+= 1;
					st_case_23:
					if ( p == pe && p != eof )
						goto _out23;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st24;
					}
					_st24:
					if ( p == eof )
						goto _out24;
					p+= 1;
					st_case_24:
					if ( p == pe && p != eof )
						goto _out24;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st25;
					}
					_st25:
					if ( p == eof )
						goto _out25;
					p+= 1;
					st_case_25:
					if ( p == pe && p != eof )
						goto _out25;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st26;
					}
					_st26:
					if ( p == eof )
						goto _out26;
					p+= 1;
					st_case_26:
					if ( p == pe && p != eof )
						goto _out26;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st27;
					}
					_st27:
					if ( p == eof )
						goto _out27;
					p+= 1;
					st_case_27:
					if ( p == pe && p != eof )
						goto _out27;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st28;
					}
					_st28:
					if ( p == eof )
						goto _out28;
					p+= 1;
					st_case_28:
					if ( p == pe && p != eof )
						goto _out28;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st29;
					}
					_st29:
					if ( p == eof )
						goto _out29;
					p+= 1;
					st_case_29:
					if ( p == pe && p != eof )
						goto _out29;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st30;
					}
					_st30:
					if ( p == eof )
						goto _out30;
					p+= 1;
					st_case_30:
					if ( p == pe && p != eof )
						goto _out30;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st31;
					}
					_st31:
					if ( p == eof )
						goto _out31;
					p+= 1;
					st_case_31:
					if ( p == pe && p != eof )
						goto _out31;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st32;
					}
					_st32:
					if ( p == eof )
						goto _out32;
					p+= 1;
					st_case_32:
					if ( p == pe && p != eof )
						goto _out32;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st33;
					}
					_st33:
					if ( p == eof )
						goto _out33;
					p+= 1;
					st_case_33:
					if ( p == pe && p != eof )
						goto _out33;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st34;
					}
					_st34:
					if ( p == eof )
						goto _out34;
					p+= 1;
					st_case_34:
					if ( p == pe && p != eof )
						goto _out34;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st35;
					}
					_st35:
					if ( p == eof )
						goto _out35;
					p+= 1;
					st_case_35:
					if ( p == pe && p != eof )
						goto _out35;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st36;
					}
					_st36:
					if ( p == eof )
						goto _out36;
					p+= 1;
					st_case_36:
					if ( p == pe && p != eof )
						goto _out36;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st37;
					}
					_st37:
					if ( p == eof )
						goto _out37;
					p+= 1;
					st_case_37:
					if ( p == pe && p != eof )
						goto _out37;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st38;
					}
					_st38:
					if ( p == eof )
						goto _out38;
					p+= 1;
					st_case_38:
					if ( p == pe && p != eof )
						goto _out38;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st39;
					}
					_st39:
					if ( p == eof )
						goto _out39;
					p+= 1;
					st_case_39:
					if ( p == pe && p != eof )
						goto _out39;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st40;
					}
					_st40:
					if ( p == eof )
						goto _out40;
					p+= 1;
					st_case_40:
					if ( p == pe && p != eof )
						goto _out40;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st41;
					}
					_st41:
					if ( p == eof )
						goto _out41;
					p+= 1;
					st_case_41:
					if ( p == pe && p != eof )
						goto _out41;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st42;
					}
					_st42:
					if ( p == eof )
						goto _out42;
					p+= 1;
					st_case_42:
					if ( p == pe && p != eof )
						goto _out42;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st43;
					}
					_st43:
					if ( p == eof )
						goto _out43;
					p+= 1;
					st_case_43:
					if ( p == pe && p != eof )
						goto _out43;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st44;
					}
					_st44:
					if ( p == eof )
						goto _out44;
					p+= 1;
					st_case_44:
					if ( p == pe && p != eof )
						goto _out44;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st45;
					}
					_st45:
					if ( p == eof )
						goto _out45;
					p+= 1;
					st_case_45:
					if ( p == pe && p != eof )
						goto _out45;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st46;
					}
					_st46:
					if ( p == eof )
						goto _out46;
					p+= 1;
					st_case_46:
					if ( p == pe && p != eof )
						goto _out46;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st47;
					}
					_st47:
					if ( p == eof )
						goto _out47;
					p+= 1;
					st_case_47:
					if ( p == pe && p != eof )
						goto _out47;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st48;
					}
					_st48:
					if ( p == eof )
						goto _out48;
					p+= 1;
					st_case_48:
					if ( p == pe && p != eof )
						goto _out48;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st49;
					}
					_st49:
					if ( p == eof )
						goto _out49;
					p+= 1;
					st_case_49:
					if ( p == pe && p != eof )
						goto _out49;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st50;
					}
					_st50:
					if ( p == eof )
						goto _out50;
					p+= 1;
					st_case_50:
					if ( p == pe && p != eof )
						goto _out50;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st51;
					}
					_st51:
					if ( p == eof )
						goto _out51;
					p+= 1;
					st_case_51:
					if ( p == pe && p != eof )
						goto _out51;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st52;
					}
					_st52:
					if ( p == eof )
						goto _out52;
					p+= 1;
					st_case_52:
					if ( p == pe && p != eof )
						goto _out52;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st53;
					}
					_st53:
					if ( p == eof )
						goto _out53;
					p+= 1;
					st_case_53:
					if ( p == pe && p != eof )
						goto _out53;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st54;
					}
					_st54:
					if ( p == eof )
						goto _out54;
					p+= 1;
					st_case_54:
					if ( p == pe && p != eof )
						goto _out54;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st55;
					}
					_st55:
					if ( p == eof )
						goto _out55;
					p+= 1;
					st_case_55:
					if ( p == pe && p != eof )
						goto _out55;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st56;
					}
					_st56:
					if ( p == eof )
						goto _out56;
					p+= 1;
					st_case_56:
					if ( p == pe && p != eof )
						goto _out56;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st57;
					}
					_st57:
					if ( p == eof )
						goto _out57;
					p+= 1;
					st_case_57:
					if ( p == pe && p != eof )
						goto _out57;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st58;
					}
					_st58:
					if ( p == eof )
						goto _out58;
					p+= 1;
					st_case_58:
					if ( p == pe && p != eof )
						goto _out58;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st59;
					}
					_st59:
					if ( p == eof )
						goto _out59;
					p+= 1;
					st_case_59:
					if ( p == pe && p != eof )
						goto _out59;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st60;
					}
					_st60:
					if ( p == eof )
						goto _out60;
					p+= 1;
					st_case_60:
					if ( p == pe && p != eof )
						goto _out60;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st61;
					}
					_st61:
					if ( p == eof )
						goto _out61;
					p+= 1;
					st_case_61:
					if ( p == pe && p != eof )
						goto _out61;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st62;
					}
					_st62:
					if ( p == eof )
						goto _out62;
					p+= 1;
					st_case_62:
					if ( p == pe && p != eof )
						goto _out62;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st63;
					}
					_st63:
					if ( p == eof )
						goto _out63;
					p+= 1;
					st_case_63:
					if ( p == pe && p != eof )
						goto _out63;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st64;
					}
					_st64:
					if ( p == eof )
						goto _out64;
					p+= 1;
					st_case_64:
					if ( p == pe && p != eof )
						goto _out64;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st65;
					}
					_st65:
					if ( p == eof )
						goto _out65;
					p+= 1;
					st_case_65:
					if ( p == pe && p != eof )
						goto _out65;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st66;
					}
					_st66:
					if ( p == eof )
						goto _out66;
					p+= 1;
					st_case_66:
					if ( p == pe && p != eof )
						goto _out66;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st67;
					}
					_st67:
					if ( p == eof )
						goto _out67;
					p+= 1;
					st_case_67:
					if ( p == pe && p != eof )
						goto _out67;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st68;
					}
					_st68:
					if ( p == eof )
						goto _out68;
					p+= 1;
					st_case_68:
					if ( p == pe && p != eof )
						goto _out68;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st69;
					}
					_st69:
					if ( p == eof )
						goto _out69;
					p+= 1;
					st_case_69:
					if ( p == pe && p != eof )
						goto _out69;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st70;
					}
					_st70:
					if ( p == eof )
						goto _out70;
					p+= 1;
					st_case_70:
					if ( p == pe && p != eof )
						goto _out70;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st71;
					}
					_st71:
					if ( p == eof )
						goto _out71;
					p+= 1;
					st_case_71:
					if ( p == pe && p != eof )
						goto _out71;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st72;
					}
					_st72:
					if ( p == eof )
						goto _out72;
					p+= 1;
					st_case_72:
					if ( p == pe && p != eof )
						goto _out72;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st73;
					}
					_st73:
					if ( p == eof )
						goto _out73;
					p+= 1;
					st_case_73:
					if ( p == pe && p != eof )
						goto _out73;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st74;
					}
					_st74:
					if ( p == eof )
						goto _out74;
					p+= 1;
					st_case_74:
					if ( p == pe && p != eof )
						goto _out74;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st75;
					}
					_st75:
					if ( p == eof )
						goto _out75;
					p+= 1;
					st_case_75:
					if ( p == pe && p != eof )
						goto _out75;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st76;
					}
					_st76:
					if ( p == eof )
						goto _out76;
					p+= 1;
					st_case_76:
					if ( p == pe && p != eof )
						goto _out76;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st77;
					}
					_st77:
					if ( p == eof )
						goto _out77;
					p+= 1;
					st_case_77:
					if ( p == pe && p != eof )
						goto _out77;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st78;
					}
					_st78:
					if ( p == eof )
						goto _out78;
					p+= 1;
					st_case_78:
					if ( p == pe && p != eof )
						goto _out78;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st79;
					}
					_st79:
					if ( p == eof )
						goto _out79;
					p+= 1;
					st_case_79:
					if ( p == pe && p != eof )
						goto _out79;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st80;
					}
					_st80:
					if ( p == eof )
						goto _out80;
					p+= 1;
					st_case_80:
					if ( p == pe && p != eof )
						goto _out80;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st81;
					}
					_st81:
					if ( p == eof )
						goto _out81;
					p+= 1;
					st_case_81:
					if ( p == pe && p != eof )
						goto _out81;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st82;
					}
					_st82:
					if ( p == eof )
						goto _out82;
					p+= 1;
					st_case_82:
					if ( p == pe && p != eof )
						goto _out82;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st83;
					}
					_st83:
					if ( p == eof )
						goto _out83;
					p+= 1;
					st_case_83:
					if ( p == pe && p != eof )
						goto _out83;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st84;
					}
					_st84:
					if ( p == eof )
						goto _out84;
					p+= 1;
					st_case_84:
					if ( p == pe && p != eof )
						goto _out84;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st85;
					}
					_st85:
					if ( p == eof )
						goto _out85;
					p+= 1;
					st_case_85:
					if ( p == pe && p != eof )
						goto _out85;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st86;
					}
					_st86:
					if ( p == eof )
						goto _out86;
					p+= 1;
					st_case_86:
					if ( p == pe && p != eof )
						goto _out86;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st87;
					}
					_st87:
					if ( p == eof )
						goto _out87;
					p+= 1;
					st_case_87:
					if ( p == pe && p != eof )
						goto _out87;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st88;
					}
					_st88:
					if ( p == eof )
						goto _out88;
					p+= 1;
					st_case_88:
					if ( p == pe && p != eof )
						goto _out88;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st89;
					}
					_st89:
					if ( p == eof )
						goto _out89;
					p+= 1;
					st_case_89:
					if ( p == pe && p != eof )
						goto _out89;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st90;
					}
					_st90:
					if ( p == eof )
						goto _out90;
					p+= 1;
					st_case_90:
					if ( p == pe && p != eof )
						goto _out90;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st91;
					}
					_st91:
					if ( p == eof )
						goto _out91;
					p+= 1;
					st_case_91:
					if ( p == pe && p != eof )
						goto _out91;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st92;
					}
					_st92:
					if ( p == eof )
						goto _out92;
					p+= 1;
					st_case_92:
					if ( p == pe && p != eof )
						goto _out92;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st93;
					}
					_st93:
					if ( p == eof )
						goto _out93;
					p+= 1;
					st_case_93:
					if ( p == pe && p != eof )
						goto _out93;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st94;
					}
					_st94:
					if ( p == eof )
						goto _out94;
					p+= 1;
					st_case_94:
					if ( p == pe && p != eof )
						goto _out94;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st95;
					}
					_st95:
					if ( p == eof )
						goto _out95;
					p+= 1;
					st_case_95:
					if ( p == pe && p != eof )
						goto _out95;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st96;
					}
					_st96:
					if ( p == eof )
						goto _out96;
					p+= 1;
					st_case_96:
					if ( p == pe && p != eof )
						goto _out96;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st97;
					}
					_st97:
					if ( p == eof )
						goto _out97;
					p+= 1;
					st_case_97:
					if ( p == pe && p != eof )
						goto _out97;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st98;
					}
					_st98:
					if ( p == eof )
						goto _out98;
					p+= 1;
					st_case_98:
					if ( p == pe && p != eof )
						goto _out98;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _st99;
					}
					_st99:
					if ( p == eof )
						goto _out99;
					p+= 1;
					st_case_99:
					if ( p == pe && p != eof )
						goto _out99;
					if ( p == eof ) {
						goto _ctr0;}
					else {
						if ( ( (*( p))) == 10 ) {
							goto _ctr2;
						}
						goto _ctr0;
					}
					_st138:
					if ( p == eof )
						goto _out138;
					p+= 1;
					st_case_138:
					if ( p == pe && p != eof )
						goto _out138;
					if ( p == eof ) {
						goto _ctr146;}
					else {
						switch( ( (*( p))) ) {
							case 100: {
								goto _st140;
							}
							case 101: {
								goto _st143;
							}
							case 103: {
								goto _st147;
							}
							case 108: {
								goto _st153;
							}
							case 110: {
								goto _st159;
							}
							case 111: {
								goto _st169;
							}
							case 112: {
								goto _st174;
							}
							case 114: {
								goto _st194;
							}
							case 120: {
								goto _st206;
							}
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 65 ) {
							goto _st139;
						}
						goto _ctr146;
					}
					_st139:
					if ( p == eof )
						goto _out139;
					p+= 1;
					st_case_139:
					if ( p == pe && p != eof )
						goto _out139;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st140:
					if ( p == eof )
						goto _out140;
					p+= 1;
					st_case_140:
					if ( p == pe && p != eof )
						goto _out140;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st141;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st141:
					if ( p == eof )
						goto _out141;
					p+= 1;
					st_case_141:
					if ( p == pe && p != eof )
						goto _out141;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 102 ) {
							goto _ctr160;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_ctr160:
						{
#line 1 "NONE"
						{te = p+1;}}
					
#line 2484 "macro_expanders/def_expander.c"

					goto _st142;
					_st142:
					if ( p == eof )
						goto _out142;
					p+= 1;
					st_case_142:
					if ( p == pe && p != eof )
						goto _out142;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st100;
							}
							case 32: {
								goto _st100;
							}
							case 92: {
								goto _st101;
							}
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st100:
					if ( p == eof )
						goto _out100;
					p+= 1;
					st_case_100:
					if ( p == pe && p != eof )
						goto _out100;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st100;
							}
							case 32: {
								goto _st100;
							}
							case 92: {
								goto _st101;
							}
						}
						goto _ctr101;
					}
					_st101:
					if ( p == eof )
						goto _out101;
					p+= 1;
					st_case_101:
					if ( p == pe && p != eof )
						goto _out101;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st102;
							}
						} else if ( ( (*( p))) >= 65 ) {
							goto _st102;
						}
						goto _ctr101;
					}
					_st102:
					if ( p == eof )
						goto _out102;
					p+= 1;
					st_case_102:
					if ( p == pe && p != eof )
						goto _out102;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st103;
							}
							case 32: {
								goto _st103;
							}
							case 35: {
								goto _st104;
							}
							case 123: {
								goto _ctr107;
							}
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st102;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st102;
						}
						goto _ctr101;
					}
					_st103:
					if ( p == eof )
						goto _out103;
					p+= 1;
					st_case_103:
					if ( p == pe && p != eof )
						goto _out103;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st103;
							}
							case 32: {
								goto _st103;
							}
							case 35: {
								goto _st104;
							}
							case 123: {
								goto _ctr107;
							}
						}
						goto _ctr101;
					}
					_st104:
					if ( p == eof )
						goto _out104;
					p+= 1;
					st_case_104:
					if ( p == pe && p != eof )
						goto _out104;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
							goto _st105;
						}
						goto _ctr101;
					}
					_st105:
					if ( p == eof )
						goto _out105;
					p+= 1;
					st_case_105:
					if ( p == pe && p != eof )
						goto _out105;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st106;
							}
							case 32: {
								goto _st106;
							}
							case 35: {
								goto _st104;
							}
							case 123: {
								goto _ctr107;
							}
						}
						goto _ctr101;
					}
					_st106:
					if ( p == eof )
						goto _out106;
					p+= 1;
					st_case_106:
					if ( p == pe && p != eof )
						goto _out106;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st106;
							}
							case 32: {
								goto _st106;
							}
							case 123: {
								goto _ctr107;
							}
						}
						goto _ctr101;
					}
					_ctr112:
						{
#line 800 "macro_expanders/../latex.rl"
						n--; }
					
#line 2686 "macro_expanders/def_expander.c"

					goto _st107;
					_ctr107:
						{
#line 802 "macro_expanders/../latex.rl"
						n=0;}
					
#line 2693 "macro_expanders/def_expander.c"

					goto _st107;
					_ctr111:
						{
#line 799 "macro_expanders/../latex.rl"
						n++; }
					
#line 2700 "macro_expanders/def_expander.c"

					goto _st107;
					_st107:
					if ( p == eof )
						goto _out107;
					p+= 1;
					st_case_107:
					if ( p == pe && p != eof )
						goto _out107;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 123: {
								goto _ctr111;
							}
							case 125: {
								_ck = 0;
								if ( (!n)
#line 2719 "macro_expanders/def_expander.c"
 )
								_ck += 1;
								if ( _ck > 0 ) {
									goto _ctr113;
								} else {
									goto _ctr112;
								}
								
							}
						}
						goto _st107;
					}
					_st143:
					if ( p == eof )
						goto _out143;
					p+= 1;
					st_case_143:
					if ( p == pe && p != eof )
						goto _out143;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 100 ) {
							goto _st144;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st144:
					if ( p == eof )
						goto _out144;
					p+= 1;
					st_case_144:
					if ( p == pe && p != eof )
						goto _out144;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st145;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st145:
					if ( p == eof )
						goto _out145;
					p+= 1;
					st_case_145:
					if ( p == pe && p != eof )
						goto _out145;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 102 ) {
							goto _st146;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st146:
					if ( p == eof )
						goto _out146;
					p+= 1;
					st_case_146:
					if ( p == pe && p != eof )
						goto _out146;
					if ( p == eof ) {
						goto _ctr164;}
					else {
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr164;
					}
					_st147:
					if ( p == eof )
						goto _out147;
					p+= 1;
					st_case_147:
					if ( p == pe && p != eof )
						goto _out147;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						switch( ( (*( p))) ) {
							case 100: {
								goto _st140;
							}
							case 108: {
								goto _st148;
							}
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st148:
					if ( p == eof )
						goto _out148;
					p+= 1;
					st_case_148:
					if ( p == pe && p != eof )
						goto _out148;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 111 ) {
							goto _st149;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st149:
					if ( p == eof )
						goto _out149;
					p+= 1;
					st_case_149:
					if ( p == pe && p != eof )
						goto _out149;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 98 ) {
							goto _st150;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st150:
					if ( p == eof )
						goto _out150;
					p+= 1;
					st_case_150:
					if ( p == pe && p != eof )
						goto _out150;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 97 ) {
							goto _st151;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 98 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st151:
					if ( p == eof )
						goto _out151;
					p+= 1;
					st_case_151:
					if ( p == pe && p != eof )
						goto _out151;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 108 ) {
							goto _ctr169;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_ctr169:
						{
#line 1 "NONE"
						{te = p+1;}}
					
#line 2936 "macro_expanders/def_expander.c"

					goto _st152;
					_st152:
					if ( p == eof )
						goto _out152;
					p+= 1;
					st_case_152:
					if ( p == pe && p != eof )
						goto _out152;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st108;
							}
							case 32: {
								goto _st108;
							}
							case 92: {
								goto _st109;
							}
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st108:
					if ( p == eof )
						goto _out108;
					p+= 1;
					st_case_108:
					if ( p == pe && p != eof )
						goto _out108;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st108;
							}
							case 32: {
								goto _st108;
							}
							case 92: {
								goto _st109;
							}
						}
						goto _ctr101;
					}
					_st109:
					if ( p == eof )
						goto _out109;
					p+= 1;
					st_case_109:
					if ( p == pe && p != eof )
						goto _out109;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 100: {
								goto _st110;
							}
							case 103: {
								goto _st112;
							}
						}
						goto _ctr101;
					}
					_st110:
					if ( p == eof )
						goto _out110;
					p+= 1;
					st_case_110:
					if ( p == pe && p != eof )
						goto _out110;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st111;
						}
						goto _ctr101;
					}
					_st111:
					if ( p == eof )
						goto _out111;
					p+= 1;
					st_case_111:
					if ( p == pe && p != eof )
						goto _out111;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 102 ) {
							goto _st100;
						}
						goto _ctr101;
					}
					_st112:
					if ( p == eof )
						goto _out112;
					p+= 1;
					st_case_112:
					if ( p == pe && p != eof )
						goto _out112;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 100 ) {
							goto _st110;
						}
						goto _ctr101;
					}
					_st153:
					if ( p == eof )
						goto _out153;
					p+= 1;
					st_case_153:
					if ( p == pe && p != eof )
						goto _out153;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						switch( ( (*( p))) ) {
							case 101: {
								goto _st154;
							}
							case 111: {
								goto _st156;
							}
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st154:
					if ( p == eof )
						goto _out154;
					p+= 1;
					st_case_154:
					if ( p == pe && p != eof )
						goto _out154;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 116 ) {
							goto _st155;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st155:
					if ( p == eof )
						goto _out155;
					p+= 1;
					st_case_155:
					if ( p == pe && p != eof )
						goto _out155;
					if ( p == eof ) {
						goto _ctr173;}
					else {
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr173;
					}
					_st156:
					if ( p == eof )
						goto _out156;
					p+= 1;
					st_case_156:
					if ( p == pe && p != eof )
						goto _out156;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 110 ) {
							goto _st157;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st157:
					if ( p == eof )
						goto _out157;
					p+= 1;
					st_case_157:
					if ( p == pe && p != eof )
						goto _out157;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 103 ) {
							goto _ctr175;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_ctr175:
						{
#line 1 "NONE"
						{te = p+1;}}
					
#line 3173 "macro_expanders/def_expander.c"

					goto _st158;
					_st158:
					if ( p == eof )
						goto _out158;
					p+= 1;
					st_case_158:
					if ( p == pe && p != eof )
						goto _out158;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st113;
							}
							case 32: {
								goto _st113;
							}
							case 92: {
								goto _st114;
							}
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st113:
					if ( p == eof )
						goto _out113;
					p+= 1;
					st_case_113:
					if ( p == pe && p != eof )
						goto _out113;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st113;
							}
							case 32: {
								goto _st113;
							}
							case 92: {
								goto _st114;
							}
						}
						goto _ctr101;
					}
					_st114:
					if ( p == eof )
						goto _out114;
					p+= 1;
					st_case_114:
					if ( p == pe && p != eof )
						goto _out114;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 100: {
								goto _st110;
							}
							case 103: {
								goto _st115;
							}
							case 111: {
								goto _st120;
							}
							case 112: {
								goto _st126;
							}
						}
						goto _ctr101;
					}
					_st115:
					if ( p == eof )
						goto _out115;
					p+= 1;
					st_case_115:
					if ( p == pe && p != eof )
						goto _out115;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 100: {
								goto _st110;
							}
							case 108: {
								goto _st116;
							}
						}
						goto _ctr101;
					}
					_st116:
					if ( p == eof )
						goto _out116;
					p+= 1;
					st_case_116:
					if ( p == pe && p != eof )
						goto _out116;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 111 ) {
							goto _st117;
						}
						goto _ctr101;
					}
					_st117:
					if ( p == eof )
						goto _out117;
					p+= 1;
					st_case_117:
					if ( p == pe && p != eof )
						goto _out117;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 98 ) {
							goto _st118;
						}
						goto _ctr101;
					}
					_st118:
					if ( p == eof )
						goto _out118;
					p+= 1;
					st_case_118:
					if ( p == pe && p != eof )
						goto _out118;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 97 ) {
							goto _st119;
						}
						goto _ctr101;
					}
					_st119:
					if ( p == eof )
						goto _out119;
					p+= 1;
					st_case_119:
					if ( p == pe && p != eof )
						goto _out119;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 108 ) {
							goto _st108;
						}
						goto _ctr101;
					}
					_st120:
					if ( p == eof )
						goto _out120;
					p+= 1;
					st_case_120:
					if ( p == pe && p != eof )
						goto _out120;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 117 ) {
							goto _st121;
						}
						goto _ctr101;
					}
					_st121:
					if ( p == eof )
						goto _out121;
					p+= 1;
					st_case_121:
					if ( p == pe && p != eof )
						goto _out121;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 116 ) {
							goto _st122;
						}
						goto _ctr101;
					}
					_st122:
					if ( p == eof )
						goto _out122;
					p+= 1;
					st_case_122:
					if ( p == pe && p != eof )
						goto _out122;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st123;
						}
						goto _ctr101;
					}
					_st123:
					if ( p == eof )
						goto _out123;
					p+= 1;
					st_case_123:
					if ( p == pe && p != eof )
						goto _out123;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 114 ) {
							goto _st124;
						}
						goto _ctr101;
					}
					_st124:
					if ( p == eof )
						goto _out124;
					p+= 1;
					st_case_124:
					if ( p == pe && p != eof )
						goto _out124;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st124;
							}
							case 32: {
								goto _st124;
							}
							case 92: {
								goto _st125;
							}
						}
						goto _ctr101;
					}
					_st125:
					if ( p == eof )
						goto _out125;
					p+= 1;
					st_case_125:
					if ( p == pe && p != eof )
						goto _out125;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 100: {
								goto _st110;
							}
							case 103: {
								goto _st115;
							}
							case 112: {
								goto _st126;
							}
						}
						goto _ctr101;
					}
					_st126:
					if ( p == eof )
						goto _out126;
					p+= 1;
					st_case_126:
					if ( p == pe && p != eof )
						goto _out126;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 114 ) {
							goto _st127;
						}
						goto _ctr101;
					}
					_st127:
					if ( p == eof )
						goto _out127;
					p+= 1;
					st_case_127:
					if ( p == pe && p != eof )
						goto _out127;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 111 ) {
							goto _st128;
						}
						goto _ctr101;
					}
					_st128:
					if ( p == eof )
						goto _out128;
					p+= 1;
					st_case_128:
					if ( p == pe && p != eof )
						goto _out128;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 116 ) {
							goto _st129;
						}
						goto _ctr101;
					}
					_st129:
					if ( p == eof )
						goto _out129;
					p+= 1;
					st_case_129:
					if ( p == pe && p != eof )
						goto _out129;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st130;
						}
						goto _ctr101;
					}
					_st130:
					if ( p == eof )
						goto _out130;
					p+= 1;
					st_case_130:
					if ( p == pe && p != eof )
						goto _out130;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 99 ) {
							goto _st131;
						}
						goto _ctr101;
					}
					_st131:
					if ( p == eof )
						goto _out131;
					p+= 1;
					st_case_131:
					if ( p == pe && p != eof )
						goto _out131;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 116 ) {
							goto _st132;
						}
						goto _ctr101;
					}
					_st132:
					if ( p == eof )
						goto _out132;
					p+= 1;
					st_case_132:
					if ( p == pe && p != eof )
						goto _out132;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st133;
						}
						goto _ctr101;
					}
					_st133:
					if ( p == eof )
						goto _out133;
					p+= 1;
					st_case_133:
					if ( p == pe && p != eof )
						goto _out133;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						if ( ( (*( p))) == 100 ) {
							goto _st134;
						}
						goto _ctr101;
					}
					_st134:
					if ( p == eof )
						goto _out134;
					p+= 1;
					st_case_134:
					if ( p == pe && p != eof )
						goto _out134;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st134;
							}
							case 32: {
								goto _st134;
							}
							case 92: {
								goto _st135;
							}
						}
						goto _ctr101;
					}
					_st135:
					if ( p == eof )
						goto _out135;
					p+= 1;
					st_case_135:
					if ( p == pe && p != eof )
						goto _out135;
					if ( p == eof ) {
						goto _ctr101;}
					else {
						switch( ( (*( p))) ) {
							case 100: {
								goto _st110;
							}
							case 103: {
								goto _st115;
							}
						}
						goto _ctr101;
					}
					_st159:
					if ( p == eof )
						goto _out159;
					p+= 1;
					st_case_159:
					if ( p == pe && p != eof )
						goto _out159;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st160;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st160:
					if ( p == eof )
						goto _out160;
					p+= 1;
					st_case_160:
					if ( p == pe && p != eof )
						goto _out160;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 119 ) {
							goto _st161;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st161:
					if ( p == eof )
						goto _out161;
					p+= 1;
					st_case_161:
					if ( p == pe && p != eof )
						goto _out161;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 99 ) {
							goto _st162;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st162:
					if ( p == eof )
						goto _out162;
					p+= 1;
					st_case_162:
					if ( p == pe && p != eof )
						goto _out162;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 111 ) {
							goto _st163;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st163:
					if ( p == eof )
						goto _out163;
					p+= 1;
					st_case_163:
					if ( p == pe && p != eof )
						goto _out163;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 109 ) {
							goto _st164;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st164:
					if ( p == eof )
						goto _out164;
					p+= 1;
					st_case_164:
					if ( p == pe && p != eof )
						goto _out164;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 109 ) {
							goto _st165;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st165:
					if ( p == eof )
						goto _out165;
					p+= 1;
					st_case_165:
					if ( p == pe && p != eof )
						goto _out165;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 97 ) {
							goto _st166;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 98 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st166:
					if ( p == eof )
						goto _out166;
					p+= 1;
					st_case_166:
					if ( p == pe && p != eof )
						goto _out166;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 110 ) {
							goto _st167;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st167:
					if ( p == eof )
						goto _out167;
					p+= 1;
					st_case_167:
					if ( p == pe && p != eof )
						goto _out167;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 100 ) {
							goto _st168;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st168:
					if ( p == eof )
						goto _out168;
					p+= 1;
					st_case_168:
					if ( p == pe && p != eof )
						goto _out168;
					if ( p == eof ) {
						goto _ctr185;}
					else {
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr185;
					}
					_st169:
					if ( p == eof )
						goto _out169;
					p+= 1;
					st_case_169:
					if ( p == pe && p != eof )
						goto _out169;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 117 ) {
							goto _st170;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st170:
					if ( p == eof )
						goto _out170;
					p+= 1;
					st_case_170:
					if ( p == pe && p != eof )
						goto _out170;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 116 ) {
							goto _st171;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st171:
					if ( p == eof )
						goto _out171;
					p+= 1;
					st_case_171:
					if ( p == pe && p != eof )
						goto _out171;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st172;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st172:
					if ( p == eof )
						goto _out172;
					p+= 1;
					st_case_172:
					if ( p == pe && p != eof )
						goto _out172;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 114 ) {
							goto _ctr189;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_ctr189:
						{
#line 1 "NONE"
						{te = p+1;}}
					
#line 3913 "macro_expanders/def_expander.c"

					goto _st173;
					_st173:
					if ( p == eof )
						goto _out173;
					p+= 1;
					st_case_173:
					if ( p == pe && p != eof )
						goto _out173;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st124;
							}
							case 32: {
								goto _st124;
							}
							case 92: {
								goto _st125;
							}
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st174:
					if ( p == eof )
						goto _out174;
					p+= 1;
					st_case_174:
					if ( p == pe && p != eof )
						goto _out174;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 114 ) {
							goto _st175;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st175:
					if ( p == eof )
						goto _out175;
					p+= 1;
					st_case_175:
					if ( p == pe && p != eof )
						goto _out175;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 111 ) {
							goto _st176;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st176:
					if ( p == eof )
						goto _out176;
					p+= 1;
					st_case_176:
					if ( p == pe && p != eof )
						goto _out176;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						switch( ( (*( p))) ) {
							case 116: {
								goto _st177;
							}
							case 118: {
								goto _st183;
							}
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st177:
					if ( p == eof )
						goto _out177;
					p+= 1;
					st_case_177:
					if ( p == pe && p != eof )
						goto _out177;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st178;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st178:
					if ( p == eof )
						goto _out178;
					p+= 1;
					st_case_178:
					if ( p == pe && p != eof )
						goto _out178;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 99 ) {
							goto _st179;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st179:
					if ( p == eof )
						goto _out179;
					p+= 1;
					st_case_179:
					if ( p == pe && p != eof )
						goto _out179;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 116 ) {
							goto _st180;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st180:
					if ( p == eof )
						goto _out180;
					p+= 1;
					st_case_180:
					if ( p == pe && p != eof )
						goto _out180;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st181;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st181:
					if ( p == eof )
						goto _out181;
					p+= 1;
					st_case_181:
					if ( p == pe && p != eof )
						goto _out181;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 100 ) {
							goto _ctr198;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_ctr198:
						{
#line 1 "NONE"
						{te = p+1;}}
					
#line 4131 "macro_expanders/def_expander.c"

					goto _st182;
					_st182:
					if ( p == eof )
						goto _out182;
					p+= 1;
					st_case_182:
					if ( p == pe && p != eof )
						goto _out182;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						switch( ( (*( p))) ) {
							case 9: {
								goto _st134;
							}
							case 32: {
								goto _st134;
							}
							case 92: {
								goto _st135;
							}
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st183:
					if ( p == eof )
						goto _out183;
					p+= 1;
					st_case_183:
					if ( p == pe && p != eof )
						goto _out183;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 105 ) {
							goto _st184;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st184:
					if ( p == eof )
						goto _out184;
					p+= 1;
					st_case_184:
					if ( p == pe && p != eof )
						goto _out184;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 100 ) {
							goto _st185;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st185:
					if ( p == eof )
						goto _out185;
					p+= 1;
					st_case_185:
					if ( p == pe && p != eof )
						goto _out185;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st186;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st186:
					if ( p == eof )
						goto _out186;
					p+= 1;
					st_case_186:
					if ( p == pe && p != eof )
						goto _out186;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 99 ) {
							goto _st187;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st187:
					if ( p == eof )
						goto _out187;
					p+= 1;
					st_case_187:
					if ( p == pe && p != eof )
						goto _out187;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 111 ) {
							goto _st188;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st188:
					if ( p == eof )
						goto _out188;
					p+= 1;
					st_case_188:
					if ( p == pe && p != eof )
						goto _out188;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 109 ) {
							goto _st189;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st189:
					if ( p == eof )
						goto _out189;
					p+= 1;
					st_case_189:
					if ( p == pe && p != eof )
						goto _out189;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 109 ) {
							goto _st190;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st190:
					if ( p == eof )
						goto _out190;
					p+= 1;
					st_case_190:
					if ( p == pe && p != eof )
						goto _out190;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 97 ) {
							goto _st191;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 98 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st191:
					if ( p == eof )
						goto _out191;
					p+= 1;
					st_case_191:
					if ( p == pe && p != eof )
						goto _out191;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 110 ) {
							goto _st192;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st192:
					if ( p == eof )
						goto _out192;
					p+= 1;
					st_case_192:
					if ( p == pe && p != eof )
						goto _out192;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 100 ) {
							goto _st193;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st193:
					if ( p == eof )
						goto _out193;
					p+= 1;
					st_case_193:
					if ( p == pe && p != eof )
						goto _out193;
					if ( p == eof ) {
						goto _ctr209;}
					else {
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr209;
					}
					_st194:
					if ( p == eof )
						goto _out194;
					p+= 1;
					st_case_194:
					if ( p == pe && p != eof )
						goto _out194;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st195;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st195:
					if ( p == eof )
						goto _out195;
					p+= 1;
					st_case_195:
					if ( p == pe && p != eof )
						goto _out195;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 110 ) {
							goto _st196;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st196:
					if ( p == eof )
						goto _out196;
					p+= 1;
					st_case_196:
					if ( p == pe && p != eof )
						goto _out196;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st197;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st197:
					if ( p == eof )
						goto _out197;
					p+= 1;
					st_case_197:
					if ( p == pe && p != eof )
						goto _out197;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 119 ) {
							goto _st198;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st198:
					if ( p == eof )
						goto _out198;
					p+= 1;
					st_case_198:
					if ( p == pe && p != eof )
						goto _out198;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 99 ) {
							goto _st199;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st199:
					if ( p == eof )
						goto _out199;
					p+= 1;
					st_case_199:
					if ( p == pe && p != eof )
						goto _out199;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 111 ) {
							goto _st200;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st200:
					if ( p == eof )
						goto _out200;
					p+= 1;
					st_case_200:
					if ( p == pe && p != eof )
						goto _out200;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 109 ) {
							goto _st201;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st201:
					if ( p == eof )
						goto _out201;
					p+= 1;
					st_case_201:
					if ( p == pe && p != eof )
						goto _out201;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 109 ) {
							goto _st202;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st202:
					if ( p == eof )
						goto _out202;
					p+= 1;
					st_case_202:
					if ( p == pe && p != eof )
						goto _out202;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 97 ) {
							goto _st203;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 98 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st203:
					if ( p == eof )
						goto _out203;
					p+= 1;
					st_case_203:
					if ( p == pe && p != eof )
						goto _out203;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 110 ) {
							goto _st204;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st204:
					if ( p == eof )
						goto _out204;
					p+= 1;
					st_case_204:
					if ( p == pe && p != eof )
						goto _out204;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 100 ) {
							goto _st205;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st205:
					if ( p == eof )
						goto _out205;
					p+= 1;
					st_case_205:
					if ( p == pe && p != eof )
						goto _out205;
					if ( p == eof ) {
						goto _ctr221;}
					else {
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr221;
					}
					_st206:
					if ( p == eof )
						goto _out206;
					p+= 1;
					st_case_206:
					if ( p == pe && p != eof )
						goto _out206;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 100 ) {
							goto _st207;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st207:
					if ( p == eof )
						goto _out207;
					p+= 1;
					st_case_207:
					if ( p == pe && p != eof )
						goto _out207;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 101 ) {
							goto _st208;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st208:
					if ( p == eof )
						goto _out208;
					p+= 1;
					st_case_208:
					if ( p == pe && p != eof )
						goto _out208;
					if ( p == eof ) {
						goto _ctr158;}
					else {
						if ( ( (*( p))) == 102 ) {
							goto _st209;
						}
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr158;
					}
					_st209:
					if ( p == eof )
						goto _out209;
					p+= 1;
					st_case_209:
					if ( p == pe && p != eof )
						goto _out209;
					if ( p == eof ) {
						goto _ctr225;}
					else {
						if ( ( (*( p))) > 90 ) {
							if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
								goto _st139;
							}
						} else if ( ( (*( p))) >= 64 ) {
							goto _st139;
						}
						goto _ctr225;
					}
					_out136: cs = 136; goto _out; 
					_out137: cs = 137; goto _out; 
					_out0: cs = 0; goto _out; 
					_out1: cs = 1; goto _out; 
					_out2: cs = 2; goto _out; 
					_out3: cs = 3; goto _out; 
					_out4: cs = 4; goto _out; 
					_out5: cs = 5; goto _out; 
					_out6: cs = 6; goto _out; 
					_out7: cs = 7; goto _out; 
					_out8: cs = 8; goto _out; 
					_out9: cs = 9; goto _out; 
					_out10: cs = 10; goto _out; 
					_out11: cs = 11; goto _out; 
					_out12: cs = 12; goto _out; 
					_out13: cs = 13; goto _out; 
					_out14: cs = 14; goto _out; 
					_out15: cs = 15; goto _out; 
					_out16: cs = 16; goto _out; 
					_out17: cs = 17; goto _out; 
					_out18: cs = 18; goto _out; 
					_out19: cs = 19; goto _out; 
					_out20: cs = 20; goto _out; 
					_out21: cs = 21; goto _out; 
					_out22: cs = 22; goto _out; 
					_out23: cs = 23; goto _out; 
					_out24: cs = 24; goto _out; 
					_out25: cs = 25; goto _out; 
					_out26: cs = 26; goto _out; 
					_out27: cs = 27; goto _out; 
					_out28: cs = 28; goto _out; 
					_out29: cs = 29; goto _out; 
					_out30: cs = 30; goto _out; 
					_out31: cs = 31; goto _out; 
					_out32: cs = 32; goto _out; 
					_out33: cs = 33; goto _out; 
					_out34: cs = 34; goto _out; 
					_out35: cs = 35; goto _out; 
					_out36: cs = 36; goto _out; 
					_out37: cs = 37; goto _out; 
					_out38: cs = 38; goto _out; 
					_out39: cs = 39; goto _out; 
					_out40: cs = 40; goto _out; 
					_out41: cs = 41; goto _out; 
					_out42: cs = 42; goto _out; 
					_out43: cs = 43; goto _out; 
					_out44: cs = 44; goto _out; 
					_out45: cs = 45; goto _out; 
					_out46: cs = 46; goto _out; 
					_out47: cs = 47; goto _out; 
					_out48: cs = 48; goto _out; 
					_out49: cs = 49; goto _out; 
					_out50: cs = 50; goto _out; 
					_out51: cs = 51; goto _out; 
					_out52: cs = 52; goto _out; 
					_out53: cs = 53; goto _out; 
					_out54: cs = 54; goto _out; 
					_out55: cs = 55; goto _out; 
					_out56: cs = 56; goto _out; 
					_out57: cs = 57; goto _out; 
					_out58: cs = 58; goto _out; 
					_out59: cs = 59; goto _out; 
					_out60: cs = 60; goto _out; 
					_out61: cs = 61; goto _out; 
					_out62: cs = 62; goto _out; 
					_out63: cs = 63; goto _out; 
					_out64: cs = 64; goto _out; 
					_out65: cs = 65; goto _out; 
					_out66: cs = 66; goto _out; 
					_out67: cs = 67; goto _out; 
					_out68: cs = 68; goto _out; 
					_out69: cs = 69; goto _out; 
					_out70: cs = 70; goto _out; 
					_out71: cs = 71; goto _out; 
					_out72: cs = 72; goto _out; 
					_out73: cs = 73; goto _out; 
					_out74: cs = 74; goto _out; 
					_out75: cs = 75; goto _out; 
					_out76: cs = 76; goto _out; 
					_out77: cs = 77; goto _out; 
					_out78: cs = 78; goto _out; 
					_out79: cs = 79; goto _out; 
					_out80: cs = 80; goto _out; 
					_out81: cs = 81; goto _out; 
					_out82: cs = 82; goto _out; 
					_out83: cs = 83; goto _out; 
					_out84: cs = 84; goto _out; 
					_out85: cs = 85; goto _out; 
					_out86: cs = 86; goto _out; 
					_out87: cs = 87; goto _out; 
					_out88: cs = 88; goto _out; 
					_out89: cs = 89; goto _out; 
					_out90: cs = 90; goto _out; 
					_out91: cs = 91; goto _out; 
					_out92: cs = 92; goto _out; 
					_out93: cs = 93; goto _out; 
					_out94: cs = 94; goto _out; 
					_out95: cs = 95; goto _out; 
					_out96: cs = 96; goto _out; 
					_out97: cs = 97; goto _out; 
					_out98: cs = 98; goto _out; 
					_out99: cs = 99; goto _out; 
					_out138: cs = 138; goto _out; 
					_out139: cs = 139; goto _out; 
					_out140: cs = 140; goto _out; 
					_out141: cs = 141; goto _out; 
					_out142: cs = 142; goto _out; 
					_out100: cs = 100; goto _out; 
					_out101: cs = 101; goto _out; 
					_out102: cs = 102; goto _out; 
					_out103: cs = 103; goto _out; 
					_out104: cs = 104; goto _out; 
					_out105: cs = 105; goto _out; 
					_out106: cs = 106; goto _out; 
					_out107: cs = 107; goto _out; 
					_out143: cs = 143; goto _out; 
					_out144: cs = 144; goto _out; 
					_out145: cs = 145; goto _out; 
					_out146: cs = 146; goto _out; 
					_out147: cs = 147; goto _out; 
					_out148: cs = 148; goto _out; 
					_out149: cs = 149; goto _out; 
					_out150: cs = 150; goto _out; 
					_out151: cs = 151; goto _out; 
					_out152: cs = 152; goto _out; 
					_out108: cs = 108; goto _out; 
					_out109: cs = 109; goto _out; 
					_out110: cs = 110; goto _out; 
					_out111: cs = 111; goto _out; 
					_out112: cs = 112; goto _out; 
					_out153: cs = 153; goto _out; 
					_out154: cs = 154; goto _out; 
					_out155: cs = 155; goto _out; 
					_out156: cs = 156; goto _out; 
					_out157: cs = 157; goto _out; 
					_out158: cs = 158; goto _out; 
					_out113: cs = 113; goto _out; 
					_out114: cs = 114; goto _out; 
					_out115: cs = 115; goto _out; 
					_out116: cs = 116; goto _out; 
					_out117: cs = 117; goto _out; 
					_out118: cs = 118; goto _out; 
					_out119: cs = 119; goto _out; 
					_out120: cs = 120; goto _out; 
					_out121: cs = 121; goto _out; 
					_out122: cs = 122; goto _out; 
					_out123: cs = 123; goto _out; 
					_out124: cs = 124; goto _out; 
					_out125: cs = 125; goto _out; 
					_out126: cs = 126; goto _out; 
					_out127: cs = 127; goto _out; 
					_out128: cs = 128; goto _out; 
					_out129: cs = 129; goto _out; 
					_out130: cs = 130; goto _out; 
					_out131: cs = 131; goto _out; 
					_out132: cs = 132; goto _out; 
					_out133: cs = 133; goto _out; 
					_out134: cs = 134; goto _out; 
					_out135: cs = 135; goto _out; 
					_out159: cs = 159; goto _out; 
					_out160: cs = 160; goto _out; 
					_out161: cs = 161; goto _out; 
					_out162: cs = 162; goto _out; 
					_out163: cs = 163; goto _out; 
					_out164: cs = 164; goto _out; 
					_out165: cs = 165; goto _out; 
					_out166: cs = 166; goto _out; 
					_out167: cs = 167; goto _out; 
					_out168: cs = 168; goto _out; 
					_out169: cs = 169; goto _out; 
					_out170: cs = 170; goto _out; 
					_out171: cs = 171; goto _out; 
					_out172: cs = 172; goto _out; 
					_out173: cs = 173; goto _out; 
					_out174: cs = 174; goto _out; 
					_out175: cs = 175; goto _out; 
					_out176: cs = 176; goto _out; 
					_out177: cs = 177; goto _out; 
					_out178: cs = 178; goto _out; 
					_out179: cs = 179; goto _out; 
					_out180: cs = 180; goto _out; 
					_out181: cs = 181; goto _out; 
					_out182: cs = 182; goto _out; 
					_out183: cs = 183; goto _out; 
					_out184: cs = 184; goto _out; 
					_out185: cs = 185; goto _out; 
					_out186: cs = 186; goto _out; 
					_out187: cs = 187; goto _out; 
					_out188: cs = 188; goto _out; 
					_out189: cs = 189; goto _out; 
					_out190: cs = 190; goto _out; 
					_out191: cs = 191; goto _out; 
					_out192: cs = 192; goto _out; 
					_out193: cs = 193; goto _out; 
					_out194: cs = 194; goto _out; 
					_out195: cs = 195; goto _out; 
					_out196: cs = 196; goto _out; 
					_out197: cs = 197; goto _out; 
					_out198: cs = 198; goto _out; 
					_out199: cs = 199; goto _out; 
					_out200: cs = 200; goto _out; 
					_out201: cs = 201; goto _out; 
					_out202: cs = 202; goto _out; 
					_out203: cs = 203; goto _out; 
					_out204: cs = 204; goto _out; 
					_out205: cs = 205; goto _out; 
					_out206: cs = 206; goto _out; 
					_out207: cs = 207; goto _out; 
					_out208: cs = 208; goto _out; 
					_out209: cs = 209; goto _out; 
					_out: {}
				}
				
#line 223 "macro_expanders/def_expander.rl"

				
				if (cs == def_expander_error) {
					fprintf(stderr, "Scan error at position %ld\n", p - input);
					return 1;
				}
				
				if (macros) macro_map_destroy(macros);
					free(input);
				return 0;
			}
