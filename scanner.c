#line 1 "scanner.rl"
#include "globals.h"

int n;
uint32_t seed0 = 0;
#define EMIT(t) \
uint32_t token_id = murmur3_seeded_v2(seed0, &in[ts - in], te - ts); \
fprintf(stderr, "{<filepath:%s>,filepath_id:%lu,token_id:%lu,parent_id:%lu,offset:%d," \
	"length:%d,type:%s,<tok:%.*s>}\n", \
filename, (unsigned long)filepath_id, (unsigned long)token_id, \
(unsigned long)parent_id, prefix_len + (int)(ts - in), (int)(te - ts), \
t, (int)(te - ts), &in[ts - in]); \
printf("  %u  ", token_id);


#define EMIT_BLOCK(t) \
uint32_t token_id = murmur3_seeded_v2(seed0, &in[ts - in], te - ts); \
fprintf(stderr, "{<filepath:%s>,filepath_id:%lu,token_id:%lu,parent_id:%lu,offset:%d," \
	"length:%d,type:%s,<tok:%.*s>}\n", \
filename, (unsigned long)filepath_id, (unsigned long)token_id, \
(unsigned long)parent_id, prefix_len + (int)(ts - in), (int)(te - ts), \
t, (int)(te - ts), &in[ts - in]); \
printf("\n\n  %u  \n\n", token_id);



#line 66 "scanner.rl"



#line 29 "scanner.c"
static const int strings_start = 623;
static const int strings_first_final = 623;
static const int strings_error = -1;

static const int strings_en_main = 623;


#line 68 "scanner.rl"

int scanner(const char *in, int length, char filename[], uint32_t filepath_id,
uint32_t parent_id,int prefix_len,int suffix_len) {
	int in_size = length;
	int cs = 0, act = 0;
	const char *p = in;
	const char *pe = in + length;
	const char *ts = NULL, *te = NULL;
	const char *eof = pe;
	

#line 46 "scanner.c"
	{
		cs = (int)strings_start;
		ts = 0;
		te = 0;
	}
	
#line 78 "scanner.rl"


#line 53 "scanner.c"
{
		int _ck;
		switch ( cs ) {
			case 623:
			goto st_case_623;
			case 624:
			goto st_case_624;
			case 625:
			goto st_case_625;
			case 626:
			goto st_case_626;
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
			case 627:
			goto st_case_627;
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
			case 136:
			goto st_case_136;
			case 137:
			goto st_case_137;
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
			case 628:
			goto st_case_628;
			case 180:
			goto st_case_180;
			case 629:
			goto st_case_629;
			case 181:
			goto st_case_181;
			case 630:
			goto st_case_630;
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
			case 210:
			goto st_case_210;
			case 211:
			goto st_case_211;
			case 212:
			goto st_case_212;
			case 213:
			goto st_case_213;
			case 214:
			goto st_case_214;
			case 215:
			goto st_case_215;
			case 216:
			goto st_case_216;
			case 217:
			goto st_case_217;
			case 218:
			goto st_case_218;
			case 219:
			goto st_case_219;
			case 220:
			goto st_case_220;
			case 221:
			goto st_case_221;
			case 222:
			goto st_case_222;
			case 223:
			goto st_case_223;
			case 224:
			goto st_case_224;
			case 225:
			goto st_case_225;
			case 226:
			goto st_case_226;
			case 227:
			goto st_case_227;
			case 228:
			goto st_case_228;
			case 229:
			goto st_case_229;
			case 230:
			goto st_case_230;
			case 231:
			goto st_case_231;
			case 232:
			goto st_case_232;
			case 233:
			goto st_case_233;
			case 234:
			goto st_case_234;
			case 235:
			goto st_case_235;
			case 236:
			goto st_case_236;
			case 237:
			goto st_case_237;
			case 238:
			goto st_case_238;
			case 239:
			goto st_case_239;
			case 240:
			goto st_case_240;
			case 241:
			goto st_case_241;
			case 242:
			goto st_case_242;
			case 243:
			goto st_case_243;
			case 244:
			goto st_case_244;
			case 245:
			goto st_case_245;
			case 246:
			goto st_case_246;
			case 247:
			goto st_case_247;
			case 248:
			goto st_case_248;
			case 249:
			goto st_case_249;
			case 250:
			goto st_case_250;
			case 251:
			goto st_case_251;
			case 252:
			goto st_case_252;
			case 253:
			goto st_case_253;
			case 254:
			goto st_case_254;
			case 255:
			goto st_case_255;
			case 256:
			goto st_case_256;
			case 257:
			goto st_case_257;
			case 258:
			goto st_case_258;
			case 259:
			goto st_case_259;
			case 260:
			goto st_case_260;
			case 261:
			goto st_case_261;
			case 262:
			goto st_case_262;
			case 263:
			goto st_case_263;
			case 264:
			goto st_case_264;
			case 265:
			goto st_case_265;
			case 266:
			goto st_case_266;
			case 267:
			goto st_case_267;
			case 268:
			goto st_case_268;
			case 269:
			goto st_case_269;
			case 270:
			goto st_case_270;
			case 271:
			goto st_case_271;
			case 272:
			goto st_case_272;
			case 273:
			goto st_case_273;
			case 274:
			goto st_case_274;
			case 275:
			goto st_case_275;
			case 276:
			goto st_case_276;
			case 277:
			goto st_case_277;
			case 278:
			goto st_case_278;
			case 279:
			goto st_case_279;
			case 280:
			goto st_case_280;
			case 281:
			goto st_case_281;
			case 282:
			goto st_case_282;
			case 283:
			goto st_case_283;
			case 284:
			goto st_case_284;
			case 285:
			goto st_case_285;
			case 286:
			goto st_case_286;
			case 287:
			goto st_case_287;
			case 288:
			goto st_case_288;
			case 289:
			goto st_case_289;
			case 290:
			goto st_case_290;
			case 291:
			goto st_case_291;
			case 292:
			goto st_case_292;
			case 293:
			goto st_case_293;
			case 294:
			goto st_case_294;
			case 295:
			goto st_case_295;
			case 296:
			goto st_case_296;
			case 297:
			goto st_case_297;
			case 298:
			goto st_case_298;
			case 299:
			goto st_case_299;
			case 300:
			goto st_case_300;
			case 301:
			goto st_case_301;
			case 302:
			goto st_case_302;
			case 303:
			goto st_case_303;
			case 304:
			goto st_case_304;
			case 305:
			goto st_case_305;
			case 306:
			goto st_case_306;
			case 307:
			goto st_case_307;
			case 308:
			goto st_case_308;
			case 309:
			goto st_case_309;
			case 310:
			goto st_case_310;
			case 311:
			goto st_case_311;
			case 312:
			goto st_case_312;
			case 313:
			goto st_case_313;
			case 314:
			goto st_case_314;
			case 315:
			goto st_case_315;
			case 316:
			goto st_case_316;
			case 317:
			goto st_case_317;
			case 318:
			goto st_case_318;
			case 319:
			goto st_case_319;
			case 320:
			goto st_case_320;
			case 321:
			goto st_case_321;
			case 322:
			goto st_case_322;
			case 323:
			goto st_case_323;
			case 324:
			goto st_case_324;
			case 325:
			goto st_case_325;
			case 326:
			goto st_case_326;
			case 327:
			goto st_case_327;
			case 328:
			goto st_case_328;
			case 329:
			goto st_case_329;
			case 330:
			goto st_case_330;
			case 331:
			goto st_case_331;
			case 332:
			goto st_case_332;
			case 333:
			goto st_case_333;
			case 334:
			goto st_case_334;
			case 335:
			goto st_case_335;
			case 336:
			goto st_case_336;
			case 337:
			goto st_case_337;
			case 338:
			goto st_case_338;
			case 339:
			goto st_case_339;
			case 340:
			goto st_case_340;
			case 341:
			goto st_case_341;
			case 342:
			goto st_case_342;
			case 343:
			goto st_case_343;
			case 344:
			goto st_case_344;
			case 345:
			goto st_case_345;
			case 346:
			goto st_case_346;
			case 347:
			goto st_case_347;
			case 348:
			goto st_case_348;
			case 349:
			goto st_case_349;
			case 350:
			goto st_case_350;
			case 351:
			goto st_case_351;
			case 352:
			goto st_case_352;
			case 353:
			goto st_case_353;
			case 354:
			goto st_case_354;
			case 355:
			goto st_case_355;
			case 356:
			goto st_case_356;
			case 357:
			goto st_case_357;
			case 358:
			goto st_case_358;
			case 359:
			goto st_case_359;
			case 360:
			goto st_case_360;
			case 361:
			goto st_case_361;
			case 362:
			goto st_case_362;
			case 363:
			goto st_case_363;
			case 364:
			goto st_case_364;
			case 365:
			goto st_case_365;
			case 366:
			goto st_case_366;
			case 367:
			goto st_case_367;
			case 368:
			goto st_case_368;
			case 369:
			goto st_case_369;
			case 370:
			goto st_case_370;
			case 371:
			goto st_case_371;
			case 372:
			goto st_case_372;
			case 373:
			goto st_case_373;
			case 374:
			goto st_case_374;
			case 375:
			goto st_case_375;
			case 376:
			goto st_case_376;
			case 377:
			goto st_case_377;
			case 378:
			goto st_case_378;
			case 379:
			goto st_case_379;
			case 380:
			goto st_case_380;
			case 381:
			goto st_case_381;
			case 382:
			goto st_case_382;
			case 383:
			goto st_case_383;
			case 384:
			goto st_case_384;
			case 385:
			goto st_case_385;
			case 386:
			goto st_case_386;
			case 387:
			goto st_case_387;
			case 388:
			goto st_case_388;
			case 389:
			goto st_case_389;
			case 390:
			goto st_case_390;
			case 391:
			goto st_case_391;
			case 392:
			goto st_case_392;
			case 393:
			goto st_case_393;
			case 394:
			goto st_case_394;
			case 395:
			goto st_case_395;
			case 396:
			goto st_case_396;
			case 397:
			goto st_case_397;
			case 398:
			goto st_case_398;
			case 399:
			goto st_case_399;
			case 400:
			goto st_case_400;
			case 401:
			goto st_case_401;
			case 402:
			goto st_case_402;
			case 403:
			goto st_case_403;
			case 404:
			goto st_case_404;
			case 405:
			goto st_case_405;
			case 406:
			goto st_case_406;
			case 407:
			goto st_case_407;
			case 408:
			goto st_case_408;
			case 409:
			goto st_case_409;
			case 410:
			goto st_case_410;
			case 411:
			goto st_case_411;
			case 412:
			goto st_case_412;
			case 413:
			goto st_case_413;
			case 414:
			goto st_case_414;
			case 415:
			goto st_case_415;
			case 416:
			goto st_case_416;
			case 417:
			goto st_case_417;
			case 418:
			goto st_case_418;
			case 419:
			goto st_case_419;
			case 420:
			goto st_case_420;
			case 421:
			goto st_case_421;
			case 422:
			goto st_case_422;
			case 423:
			goto st_case_423;
			case 424:
			goto st_case_424;
			case 425:
			goto st_case_425;
			case 426:
			goto st_case_426;
			case 427:
			goto st_case_427;
			case 428:
			goto st_case_428;
			case 429:
			goto st_case_429;
			case 430:
			goto st_case_430;
			case 431:
			goto st_case_431;
			case 432:
			goto st_case_432;
			case 433:
			goto st_case_433;
			case 434:
			goto st_case_434;
			case 435:
			goto st_case_435;
			case 436:
			goto st_case_436;
			case 437:
			goto st_case_437;
			case 438:
			goto st_case_438;
			case 439:
			goto st_case_439;
			case 440:
			goto st_case_440;
			case 441:
			goto st_case_441;
			case 442:
			goto st_case_442;
			case 443:
			goto st_case_443;
			case 444:
			goto st_case_444;
			case 445:
			goto st_case_445;
			case 446:
			goto st_case_446;
			case 447:
			goto st_case_447;
			case 448:
			goto st_case_448;
			case 449:
			goto st_case_449;
			case 450:
			goto st_case_450;
			case 451:
			goto st_case_451;
			case 452:
			goto st_case_452;
			case 453:
			goto st_case_453;
			case 454:
			goto st_case_454;
			case 455:
			goto st_case_455;
			case 456:
			goto st_case_456;
			case 457:
			goto st_case_457;
			case 458:
			goto st_case_458;
			case 459:
			goto st_case_459;
			case 460:
			goto st_case_460;
			case 461:
			goto st_case_461;
			case 462:
			goto st_case_462;
			case 463:
			goto st_case_463;
			case 464:
			goto st_case_464;
			case 465:
			goto st_case_465;
			case 466:
			goto st_case_466;
			case 467:
			goto st_case_467;
			case 468:
			goto st_case_468;
			case 469:
			goto st_case_469;
			case 470:
			goto st_case_470;
			case 471:
			goto st_case_471;
			case 472:
			goto st_case_472;
			case 473:
			goto st_case_473;
			case 474:
			goto st_case_474;
			case 475:
			goto st_case_475;
			case 476:
			goto st_case_476;
			case 477:
			goto st_case_477;
			case 478:
			goto st_case_478;
			case 479:
			goto st_case_479;
			case 480:
			goto st_case_480;
			case 481:
			goto st_case_481;
			case 482:
			goto st_case_482;
			case 483:
			goto st_case_483;
			case 484:
			goto st_case_484;
			case 485:
			goto st_case_485;
			case 486:
			goto st_case_486;
			case 487:
			goto st_case_487;
			case 488:
			goto st_case_488;
			case 489:
			goto st_case_489;
			case 490:
			goto st_case_490;
			case 491:
			goto st_case_491;
			case 492:
			goto st_case_492;
			case 493:
			goto st_case_493;
			case 494:
			goto st_case_494;
			case 495:
			goto st_case_495;
			case 496:
			goto st_case_496;
			case 497:
			goto st_case_497;
			case 498:
			goto st_case_498;
			case 499:
			goto st_case_499;
			case 500:
			goto st_case_500;
			case 501:
			goto st_case_501;
			case 502:
			goto st_case_502;
			case 503:
			goto st_case_503;
			case 504:
			goto st_case_504;
			case 505:
			goto st_case_505;
			case 506:
			goto st_case_506;
			case 507:
			goto st_case_507;
			case 508:
			goto st_case_508;
			case 509:
			goto st_case_509;
			case 510:
			goto st_case_510;
			case 511:
			goto st_case_511;
			case 512:
			goto st_case_512;
			case 513:
			goto st_case_513;
			case 514:
			goto st_case_514;
			case 515:
			goto st_case_515;
			case 516:
			goto st_case_516;
			case 517:
			goto st_case_517;
			case 518:
			goto st_case_518;
			case 519:
			goto st_case_519;
			case 520:
			goto st_case_520;
			case 521:
			goto st_case_521;
			case 522:
			goto st_case_522;
			case 523:
			goto st_case_523;
			case 524:
			goto st_case_524;
			case 525:
			goto st_case_525;
			case 526:
			goto st_case_526;
			case 527:
			goto st_case_527;
			case 528:
			goto st_case_528;
			case 529:
			goto st_case_529;
			case 530:
			goto st_case_530;
			case 531:
			goto st_case_531;
			case 532:
			goto st_case_532;
			case 533:
			goto st_case_533;
			case 534:
			goto st_case_534;
			case 535:
			goto st_case_535;
			case 536:
			goto st_case_536;
			case 537:
			goto st_case_537;
			case 538:
			goto st_case_538;
			case 539:
			goto st_case_539;
			case 540:
			goto st_case_540;
			case 541:
			goto st_case_541;
			case 542:
			goto st_case_542;
			case 543:
			goto st_case_543;
			case 544:
			goto st_case_544;
			case 545:
			goto st_case_545;
			case 546:
			goto st_case_546;
			case 547:
			goto st_case_547;
			case 548:
			goto st_case_548;
			case 549:
			goto st_case_549;
			case 550:
			goto st_case_550;
			case 551:
			goto st_case_551;
			case 552:
			goto st_case_552;
			case 553:
			goto st_case_553;
			case 554:
			goto st_case_554;
			case 555:
			goto st_case_555;
			case 631:
			goto st_case_631;
			case 556:
			goto st_case_556;
			case 557:
			goto st_case_557;
			case 558:
			goto st_case_558;
			case 559:
			goto st_case_559;
			case 560:
			goto st_case_560;
			case 561:
			goto st_case_561;
			case 562:
			goto st_case_562;
			case 563:
			goto st_case_563;
			case 564:
			goto st_case_564;
			case 565:
			goto st_case_565;
			case 566:
			goto st_case_566;
			case 567:
			goto st_case_567;
			case 568:
			goto st_case_568;
			case 569:
			goto st_case_569;
			case 570:
			goto st_case_570;
			case 571:
			goto st_case_571;
			case 572:
			goto st_case_572;
			case 573:
			goto st_case_573;
			case 574:
			goto st_case_574;
			case 575:
			goto st_case_575;
			case 576:
			goto st_case_576;
			case 577:
			goto st_case_577;
			case 578:
			goto st_case_578;
			case 579:
			goto st_case_579;
			case 580:
			goto st_case_580;
			case 581:
			goto st_case_581;
			case 582:
			goto st_case_582;
			case 583:
			goto st_case_583;
			case 584:
			goto st_case_584;
			case 585:
			goto st_case_585;
			case 586:
			goto st_case_586;
			case 587:
			goto st_case_587;
			case 588:
			goto st_case_588;
			case 589:
			goto st_case_589;
			case 590:
			goto st_case_590;
			case 591:
			goto st_case_591;
			case 592:
			goto st_case_592;
			case 593:
			goto st_case_593;
			case 594:
			goto st_case_594;
			case 595:
			goto st_case_595;
			case 596:
			goto st_case_596;
			case 597:
			goto st_case_597;
			case 598:
			goto st_case_598;
			case 599:
			goto st_case_599;
			case 600:
			goto st_case_600;
			case 601:
			goto st_case_601;
			case 602:
			goto st_case_602;
			case 603:
			goto st_case_603;
			case 604:
			goto st_case_604;
			case 605:
			goto st_case_605;
			case 606:
			goto st_case_606;
			case 607:
			goto st_case_607;
			case 608:
			goto st_case_608;
			case 609:
			goto st_case_609;
			case 610:
			goto st_case_610;
			case 611:
			goto st_case_611;
			case 612:
			goto st_case_612;
			case 613:
			goto st_case_613;
			case 614:
			goto st_case_614;
			case 615:
			goto st_case_615;
			case 616:
			goto st_case_616;
			case 617:
			goto st_case_617;
			case 618:
			goto st_case_618;
			case 619:
			goto st_case_619;
			case 620:
			goto st_case_620;
			case 621:
			goto st_case_621;
			case 632:
			goto st_case_632;
			case 622:
			goto st_case_622;
		}
		_ctr184:
			{
#line 38 "scanner.rl"
			{te = p+1;{
#line 38 "scanner.rl"
					EMIT("parens"); }
			}}
		
#line 1331 "scanner.c"

		goto _st623;
		_ctr202:
			{
#line 41 "scanner.rl"
			{te = p+1;{
#line 41 "scanner.rl"
					EMIT("affiliation"); }
			}}
		
#line 1341 "scanner.c"

		goto _st623;
		_ctr211:
			{
#line 40 "scanner.rl"
			{te = p+1;{
#line 40 "scanner.rl"
					EMIT("author"); }
			}}
		
#line 1351 "scanner.c"

		goto _st623;
		_ctr264:
			{
#line 45 "scanner.rl"
			{te = p+1;{
#line 45 "scanner.rl"
					EMIT_BLOCK("abstract"); }
			}}
		
#line 1361 "scanner.c"

		goto _st623;
		_ctr295:
			{
#line 48 "scanner.rl"
			{te = p+1;{
#line 48 "scanner.rl"
					EMIT_BLOCK("align"); }
			}}
		
#line 1371 "scanner.c"

		goto _st623;
		_ctr345:
			{
#line 49 "scanner.rl"
			{te = p+1;{
#line 49 "scanner.rl"
					EMIT_BLOCK("eqnarray"); }
			}}
		
#line 1381 "scanner.c"

		goto _st623;
		_ctr388:
			{
#line 47 "scanner.rl"
			{te = p+1;{
#line 47 "scanner.rl"
					EMIT_BLOCK("equation"); }
			}}
		
#line 1391 "scanner.c"

		goto _st623;
		_ctr455:
			{
#line 50 "scanner.rl"
			{te = p+1;{
#line 50 "scanner.rl"
					EMIT_BLOCK("figure"); }
			}}
		
#line 1401 "scanner.c"

		goto _st623;
		_ctr488:
			{
#line 51 "scanner.rl"
			{te = p+1;{
#line 51 "scanner.rl"
					EMIT_BLOCK("table"); }
			}}
		
#line 1411 "scanner.c"

		goto _st623;
		_ctr549:
			{
#line 52 "scanner.rl"
			{te = p+1;{
#line 52 "scanner.rl"
					EMIT_BLOCK("bibliography"); }
			}}
		
#line 1421 "scanner.c"

		goto _st623;
		_ctr590:
			{
#line 54 "scanner.rl"
			{te = p+1;{
#line 54 "scanner.rl"
					EMIT_BLOCK("verbatim"); }
			}}
		
#line 1431 "scanner.c"

		goto _st623;
		_ctr618:
			{
#line 36 "scanner.rl"
			{te = p+1;{
#line 36 "scanner.rl"
					EMIT("cite"); }
			}}
		
#line 1441 "scanner.c"

		goto _st623;
		_ctr644:
			{
#line 35 "scanner.rl"
			{te = p+1;{
#line 35 "scanner.rl"
					EMIT("frac"); }
			}}
		
#line 1451 "scanner.c"

		goto _st623;
		_ctr673:
			{
#line 37 "scanner.rl"
			{te = p+1;{
#line 37 "scanner.rl"
					EMIT("ref"); }
			}}
		
#line 1461 "scanner.c"

		goto _st623;
		_ctr684:
			{
#line 46 "scanner.rl"
			{te = p+1;{
#line 46 "scanner.rl"
					EMIT_BLOCK("section"); }
			}}
		
#line 1471 "scanner.c"

		goto _st623;
		_ctr693:
			{
#line 43 "scanner.rl"
			{te = p+1;{
#line 43 "scanner.rl"
					EMIT("title"); }
			}}
		
#line 1481 "scanner.c"

		goto _st623;
		_ctr713:
			{
#line 42 "scanner.rl"
			{te = p+1;{
#line 42 "scanner.rl"
					EMIT("usepackage"); }
			}}
		
#line 1491 "scanner.c"

		goto _st623;
		_ctr721:
			{
#line 39 "scanner.rl"
			{te = p+1;{
#line 39 "scanner.rl"
					EMIT("braces"); }
			}}
		
#line 1501 "scanner.c"

		goto _st623;
		_ctr0:
			{
#line 64 "scanner.rl"
			{p = ((te))-1;
				{
#line 64 "scanner.rl"
					printf("%c", *ts); }
			}}
		
#line 1512 "scanner.c"

		goto _st623;
		_ctr2:
			{
#line 55 "scanner.rl"
			{te = p+1;{
#line 55 "scanner.rl"
					EMIT("math"); }
			}}
		
#line 1522 "scanner.c"

		goto _st623;
		_ctr82:
			{
#line 56 "scanner.rl"
			{te = p+1;{
#line 56 "scanner.rl"
					EMIT("comment");}
			}}
		
#line 1532 "scanner.c"

		goto _st623;
		_ctr185:
			{
#line 58 "scanner.rl"
			{p = ((te))-1;
				{
#line 58 "scanner.rl"
					printf("%.*s", (int)(te-ts), ts); }
			}}
		
#line 1543 "scanner.c"

		goto _st623;
		_ctr304:
			{
#line 33 "scanner.rl"
			{te = p+1;{
#line 33 "scanner.rl"
					EMIT("begin_doc"); }
			}}
		
#line 1553 "scanner.c"

		goto _st623;
		_ctr603:
			{
#line 53 "scanner.rl"
			{te = p+1;{
#line 53 "scanner.rl"
					EMIT_BLOCK("bibitem"); }
			}}
		
#line 1563 "scanner.c"

		goto _st623;
		_ctr627:
			{
#line 36 "scanner.rl"
			{p = ((te))-1;
				{
#line 36 "scanner.rl"
					EMIT("cite"); }
			}}
		
#line 1574 "scanner.c"

		goto _st623;
		_ctr639:
			{
#line 35 "scanner.rl"
			{te = p+1;{
#line 35 "scanner.rl"
					EMIT("frac"); }
			}}
		
#line 1584 "scanner.c"

		goto _st623;
		_ctr662:
			{
#line 34 "scanner.rl"
			{te = p+1;{
#line 34 "scanner.rl"
					EMIT("end_doc"); }
			}}
		
#line 1594 "scanner.c"

		goto _st623;
		_ctr723:
			{
#line 64 "scanner.rl"
			{te = p+1;{
#line 64 "scanner.rl"
					printf("%c", *ts); }
			}}
		
#line 1604 "scanner.c"

		goto _st623;
		_ctr725:
			{
#line 61 "scanner.rl"
			{te = p+1;{
#line 61 "scanner.rl"
					printf("\n"); }
			}}
		
#line 1614 "scanner.c"

		goto _st623;
		_ctr732:
			{
#line 60 "scanner.rl"
			{te = p;p = p - 1;{
#line 60 "scanner.rl"
					printf(" "); }
			}}
		
#line 1624 "scanner.c"

		goto _st623;
		_ctr733:
			{
#line 62 "scanner.rl"
			{te = p;p = p - 1;{
#line 62 "scanner.rl"
					printf("\n"); }
			}}
		
#line 1634 "scanner.c"

		goto _st623;
		_ctr734:
			{
#line 62 "scanner.rl"
			{te = p+1;{
#line 62 "scanner.rl"
					printf("\n"); }
			}}
		
#line 1644 "scanner.c"

		goto _st623;
		_ctr735:
			{
#line 64 "scanner.rl"
			{te = p;p = p - 1;{
#line 64 "scanner.rl"
					printf("%c", *ts); }
			}}
		
#line 1654 "scanner.c"

		goto _st623;
		_ctr738:
			{
#line 58 "scanner.rl"
			{te = p;p = p - 1;{
#line 58 "scanner.rl"
					printf("%.*s", (int)(te-ts), ts); }
			}}
		
#line 1664 "scanner.c"

		goto _st623;
		_ctr750:
			{
#line 36 "scanner.rl"
			{te = p;p = p - 1;{
#line 36 "scanner.rl"
					EMIT("cite"); }
			}}
		
#line 1674 "scanner.c"

		goto _st623;
		_st623:
		if ( p == eof )
			goto _out623;
			{
#line 1 "NONE"
			{ts = 0;}}
		
#line 1683 "scanner.c"

		p+= 1;
		st_case_623:
		if ( p == pe && p != eof )
			goto _out623;
			{
#line 1 "NONE"
			{ts = p;}}
		
#line 1692 "scanner.c"

		if ( p == eof ) {
			goto _st623;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st624;
				}
				case 10: {
					goto _ctr725;
				}
				case 13: {
					goto _st625;
				}
				case 32: {
					goto _st624;
				}
				case 36: {
					goto _ctr727;
				}
				case 37: {
					goto _ctr728;
				}
				case 40: {
					goto _ctr729;
				}
				case 92: {
					goto _ctr730;
				}
				case 123: {
					goto _ctr731;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr186;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr186;
			}
			goto _ctr723;
		}
		_st624:
		if ( p == eof )
			goto _out624;
		p+= 1;
		st_case_624:
		if ( p == pe && p != eof )
			goto _out624;
		if ( p == eof ) {
			goto _ctr732;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st624;
				}
				case 32: {
					goto _st624;
				}
			}
			goto _ctr732;
		}
		_st625:
		if ( p == eof )
			goto _out625;
		p+= 1;
		st_case_625:
		if ( p == pe && p != eof )
			goto _out625;
		if ( p == eof ) {
			goto _ctr733;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr734;
			}
			goto _ctr733;
		}
		_ctr727:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 1774 "scanner.c"

		goto _st626;
		_st626:
		if ( p == eof )
			goto _out626;
		p+= 1;
		st_case_626:
		if ( p == pe && p != eof )
			goto _out626;
		if ( p == eof ) {
			goto _ctr735;}
		else {
			if ( ( (*( p))) == 36 ) {
				goto _ctr735;
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
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
			if ( ( (*( p))) == 36 ) {
				goto _ctr2;
			}
			goto _ctr0;
		}
		_ctr728:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 2996 "scanner.c"

		goto _st627;
		_st627:
		if ( p == eof )
			goto _out627;
		p+= 1;
		st_case_627:
		if ( p == pe && p != eof )
			goto _out627;
		if ( p == eof ) {
			goto _ctr735;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
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
				goto _ctr82;
			}
			goto _st100;
		}
		_st100:
		if ( p == eof )
			goto _out100;
		p+= 1;
		st_case_100:
		if ( p == pe && p != eof )
			goto _out100;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st101;
		}
		_st101:
		if ( p == eof )
			goto _out101;
		p+= 1;
		st_case_101:
		if ( p == pe && p != eof )
			goto _out101;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st102;
		}
		_st102:
		if ( p == eof )
			goto _out102;
		p+= 1;
		st_case_102:
		if ( p == pe && p != eof )
			goto _out102;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st103;
		}
		_st103:
		if ( p == eof )
			goto _out103;
		p+= 1;
		st_case_103:
		if ( p == pe && p != eof )
			goto _out103;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st104;
		}
		_st104:
		if ( p == eof )
			goto _out104;
		p+= 1;
		st_case_104:
		if ( p == pe && p != eof )
			goto _out104;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st105;
		}
		_st105:
		if ( p == eof )
			goto _out105;
		p+= 1;
		st_case_105:
		if ( p == pe && p != eof )
			goto _out105;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st106;
		}
		_st106:
		if ( p == eof )
			goto _out106;
		p+= 1;
		st_case_106:
		if ( p == pe && p != eof )
			goto _out106;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st107;
		}
		_st107:
		if ( p == eof )
			goto _out107;
		p+= 1;
		st_case_107:
		if ( p == pe && p != eof )
			goto _out107;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st108;
		}
		_st108:
		if ( p == eof )
			goto _out108;
		p+= 1;
		st_case_108:
		if ( p == pe && p != eof )
			goto _out108;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st109;
		}
		_st109:
		if ( p == eof )
			goto _out109;
		p+= 1;
		st_case_109:
		if ( p == pe && p != eof )
			goto _out109;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st110;
		}
		_st110:
		if ( p == eof )
			goto _out110;
		p+= 1;
		st_case_110:
		if ( p == pe && p != eof )
			goto _out110;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st111;
		}
		_st111:
		if ( p == eof )
			goto _out111;
		p+= 1;
		st_case_111:
		if ( p == pe && p != eof )
			goto _out111;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st112;
		}
		_st112:
		if ( p == eof )
			goto _out112;
		p+= 1;
		st_case_112:
		if ( p == pe && p != eof )
			goto _out112;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st113;
		}
		_st113:
		if ( p == eof )
			goto _out113;
		p+= 1;
		st_case_113:
		if ( p == pe && p != eof )
			goto _out113;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st114;
		}
		_st114:
		if ( p == eof )
			goto _out114;
		p+= 1;
		st_case_114:
		if ( p == pe && p != eof )
			goto _out114;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st115;
		}
		_st115:
		if ( p == eof )
			goto _out115;
		p+= 1;
		st_case_115:
		if ( p == pe && p != eof )
			goto _out115;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st116;
		}
		_st116:
		if ( p == eof )
			goto _out116;
		p+= 1;
		st_case_116:
		if ( p == pe && p != eof )
			goto _out116;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st117;
		}
		_st117:
		if ( p == eof )
			goto _out117;
		p+= 1;
		st_case_117:
		if ( p == pe && p != eof )
			goto _out117;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st118;
		}
		_st118:
		if ( p == eof )
			goto _out118;
		p+= 1;
		st_case_118:
		if ( p == pe && p != eof )
			goto _out118;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st119;
		}
		_st119:
		if ( p == eof )
			goto _out119;
		p+= 1;
		st_case_119:
		if ( p == pe && p != eof )
			goto _out119;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st120;
		}
		_st120:
		if ( p == eof )
			goto _out120;
		p+= 1;
		st_case_120:
		if ( p == pe && p != eof )
			goto _out120;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st121;
		}
		_st121:
		if ( p == eof )
			goto _out121;
		p+= 1;
		st_case_121:
		if ( p == pe && p != eof )
			goto _out121;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st122;
		}
		_st122:
		if ( p == eof )
			goto _out122;
		p+= 1;
		st_case_122:
		if ( p == pe && p != eof )
			goto _out122;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st123;
		}
		_st123:
		if ( p == eof )
			goto _out123;
		p+= 1;
		st_case_123:
		if ( p == pe && p != eof )
			goto _out123;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st124;
		}
		_st124:
		if ( p == eof )
			goto _out124;
		p+= 1;
		st_case_124:
		if ( p == pe && p != eof )
			goto _out124;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st125;
		}
		_st125:
		if ( p == eof )
			goto _out125;
		p+= 1;
		st_case_125:
		if ( p == pe && p != eof )
			goto _out125;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st126;
		}
		_st126:
		if ( p == eof )
			goto _out126;
		p+= 1;
		st_case_126:
		if ( p == pe && p != eof )
			goto _out126;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st127;
		}
		_st127:
		if ( p == eof )
			goto _out127;
		p+= 1;
		st_case_127:
		if ( p == pe && p != eof )
			goto _out127;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st128;
		}
		_st128:
		if ( p == eof )
			goto _out128;
		p+= 1;
		st_case_128:
		if ( p == pe && p != eof )
			goto _out128;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st129;
		}
		_st129:
		if ( p == eof )
			goto _out129;
		p+= 1;
		st_case_129:
		if ( p == pe && p != eof )
			goto _out129;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st130;
		}
		_st130:
		if ( p == eof )
			goto _out130;
		p+= 1;
		st_case_130:
		if ( p == pe && p != eof )
			goto _out130;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st131;
		}
		_st131:
		if ( p == eof )
			goto _out131;
		p+= 1;
		st_case_131:
		if ( p == pe && p != eof )
			goto _out131;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st132;
		}
		_st132:
		if ( p == eof )
			goto _out132;
		p+= 1;
		st_case_132:
		if ( p == pe && p != eof )
			goto _out132;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st133;
		}
		_st133:
		if ( p == eof )
			goto _out133;
		p+= 1;
		st_case_133:
		if ( p == pe && p != eof )
			goto _out133;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st134;
		}
		_st134:
		if ( p == eof )
			goto _out134;
		p+= 1;
		st_case_134:
		if ( p == pe && p != eof )
			goto _out134;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st135;
		}
		_st135:
		if ( p == eof )
			goto _out135;
		p+= 1;
		st_case_135:
		if ( p == pe && p != eof )
			goto _out135;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st136;
		}
		_st136:
		if ( p == eof )
			goto _out136;
		p+= 1;
		st_case_136:
		if ( p == pe && p != eof )
			goto _out136;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st137;
		}
		_st137:
		if ( p == eof )
			goto _out137;
		p+= 1;
		st_case_137:
		if ( p == pe && p != eof )
			goto _out137;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st138;
		}
		_st138:
		if ( p == eof )
			goto _out138;
		p+= 1;
		st_case_138:
		if ( p == pe && p != eof )
			goto _out138;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st139;
		}
		_st139:
		if ( p == eof )
			goto _out139;
		p+= 1;
		st_case_139:
		if ( p == pe && p != eof )
			goto _out139;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st140;
		}
		_st140:
		if ( p == eof )
			goto _out140;
		p+= 1;
		st_case_140:
		if ( p == pe && p != eof )
			goto _out140;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st141;
		}
		_st141:
		if ( p == eof )
			goto _out141;
		p+= 1;
		st_case_141:
		if ( p == pe && p != eof )
			goto _out141;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st142;
		}
		_st142:
		if ( p == eof )
			goto _out142;
		p+= 1;
		st_case_142:
		if ( p == pe && p != eof )
			goto _out142;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st143;
		}
		_st143:
		if ( p == eof )
			goto _out143;
		p+= 1;
		st_case_143:
		if ( p == pe && p != eof )
			goto _out143;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st144;
		}
		_st144:
		if ( p == eof )
			goto _out144;
		p+= 1;
		st_case_144:
		if ( p == pe && p != eof )
			goto _out144;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st145;
		}
		_st145:
		if ( p == eof )
			goto _out145;
		p+= 1;
		st_case_145:
		if ( p == pe && p != eof )
			goto _out145;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st146;
		}
		_st146:
		if ( p == eof )
			goto _out146;
		p+= 1;
		st_case_146:
		if ( p == pe && p != eof )
			goto _out146;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st147;
		}
		_st147:
		if ( p == eof )
			goto _out147;
		p+= 1;
		st_case_147:
		if ( p == pe && p != eof )
			goto _out147;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st148;
		}
		_st148:
		if ( p == eof )
			goto _out148;
		p+= 1;
		st_case_148:
		if ( p == pe && p != eof )
			goto _out148;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st149;
		}
		_st149:
		if ( p == eof )
			goto _out149;
		p+= 1;
		st_case_149:
		if ( p == pe && p != eof )
			goto _out149;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st150;
		}
		_st150:
		if ( p == eof )
			goto _out150;
		p+= 1;
		st_case_150:
		if ( p == pe && p != eof )
			goto _out150;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st151;
		}
		_st151:
		if ( p == eof )
			goto _out151;
		p+= 1;
		st_case_151:
		if ( p == pe && p != eof )
			goto _out151;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st152;
		}
		_st152:
		if ( p == eof )
			goto _out152;
		p+= 1;
		st_case_152:
		if ( p == pe && p != eof )
			goto _out152;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st153;
		}
		_st153:
		if ( p == eof )
			goto _out153;
		p+= 1;
		st_case_153:
		if ( p == pe && p != eof )
			goto _out153;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st154;
		}
		_st154:
		if ( p == eof )
			goto _out154;
		p+= 1;
		st_case_154:
		if ( p == pe && p != eof )
			goto _out154;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st155;
		}
		_st155:
		if ( p == eof )
			goto _out155;
		p+= 1;
		st_case_155:
		if ( p == pe && p != eof )
			goto _out155;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st156;
		}
		_st156:
		if ( p == eof )
			goto _out156;
		p+= 1;
		st_case_156:
		if ( p == pe && p != eof )
			goto _out156;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st157;
		}
		_st157:
		if ( p == eof )
			goto _out157;
		p+= 1;
		st_case_157:
		if ( p == pe && p != eof )
			goto _out157;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st158;
		}
		_st158:
		if ( p == eof )
			goto _out158;
		p+= 1;
		st_case_158:
		if ( p == pe && p != eof )
			goto _out158;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st159;
		}
		_st159:
		if ( p == eof )
			goto _out159;
		p+= 1;
		st_case_159:
		if ( p == pe && p != eof )
			goto _out159;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st160;
		}
		_st160:
		if ( p == eof )
			goto _out160;
		p+= 1;
		st_case_160:
		if ( p == pe && p != eof )
			goto _out160;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st161;
		}
		_st161:
		if ( p == eof )
			goto _out161;
		p+= 1;
		st_case_161:
		if ( p == pe && p != eof )
			goto _out161;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st162;
		}
		_st162:
		if ( p == eof )
			goto _out162;
		p+= 1;
		st_case_162:
		if ( p == pe && p != eof )
			goto _out162;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st163;
		}
		_st163:
		if ( p == eof )
			goto _out163;
		p+= 1;
		st_case_163:
		if ( p == pe && p != eof )
			goto _out163;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st164;
		}
		_st164:
		if ( p == eof )
			goto _out164;
		p+= 1;
		st_case_164:
		if ( p == pe && p != eof )
			goto _out164;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st165;
		}
		_st165:
		if ( p == eof )
			goto _out165;
		p+= 1;
		st_case_165:
		if ( p == pe && p != eof )
			goto _out165;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st166;
		}
		_st166:
		if ( p == eof )
			goto _out166;
		p+= 1;
		st_case_166:
		if ( p == pe && p != eof )
			goto _out166;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st167;
		}
		_st167:
		if ( p == eof )
			goto _out167;
		p+= 1;
		st_case_167:
		if ( p == pe && p != eof )
			goto _out167;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st168;
		}
		_st168:
		if ( p == eof )
			goto _out168;
		p+= 1;
		st_case_168:
		if ( p == pe && p != eof )
			goto _out168;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st169;
		}
		_st169:
		if ( p == eof )
			goto _out169;
		p+= 1;
		st_case_169:
		if ( p == pe && p != eof )
			goto _out169;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st170;
		}
		_st170:
		if ( p == eof )
			goto _out170;
		p+= 1;
		st_case_170:
		if ( p == pe && p != eof )
			goto _out170;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st171;
		}
		_st171:
		if ( p == eof )
			goto _out171;
		p+= 1;
		st_case_171:
		if ( p == pe && p != eof )
			goto _out171;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st172;
		}
		_st172:
		if ( p == eof )
			goto _out172;
		p+= 1;
		st_case_172:
		if ( p == pe && p != eof )
			goto _out172;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st173;
		}
		_st173:
		if ( p == eof )
			goto _out173;
		p+= 1;
		st_case_173:
		if ( p == pe && p != eof )
			goto _out173;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st174;
		}
		_st174:
		if ( p == eof )
			goto _out174;
		p+= 1;
		st_case_174:
		if ( p == pe && p != eof )
			goto _out174;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st175;
		}
		_st175:
		if ( p == eof )
			goto _out175;
		p+= 1;
		st_case_175:
		if ( p == pe && p != eof )
			goto _out175;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st176;
		}
		_st176:
		if ( p == eof )
			goto _out176;
		p+= 1;
		st_case_176:
		if ( p == pe && p != eof )
			goto _out176;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st177;
		}
		_st177:
		if ( p == eof )
			goto _out177;
		p+= 1;
		st_case_177:
		if ( p == pe && p != eof )
			goto _out177;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st178;
		}
		_st178:
		if ( p == eof )
			goto _out178;
		p+= 1;
		st_case_178:
		if ( p == pe && p != eof )
			goto _out178;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _st179;
		}
		_st179:
		if ( p == eof )
			goto _out179;
		p+= 1;
		st_case_179:
		if ( p == pe && p != eof )
			goto _out179;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr82;
			}
			goto _ctr0;
		}
		_ctr729:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 4518 "scanner.c"

			{
#line 803 "latex.rl"
			n=0;}
		
#line 4523 "scanner.c"

		goto _st628;
		_st628:
		if ( p == eof )
			goto _out628;
		p+= 1;
		st_case_628:
		if ( p == pe && p != eof )
			goto _out628;
		if ( p == eof ) {
			goto _ctr735;}
		else {
			switch( ( (*( p))) ) {
				case 40: {
					goto _ctr182;
				}
				case 41: {
					_ck = 0;
					if ( (!n)
#line 4542 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr184;
					} else {
						goto _ctr183;
					}
					
				}
			}
			goto _st180;
		}
		_ctr183:
			{
#line 801 "latex.rl"
			n--; }
		
#line 4559 "scanner.c"

		goto _st180;
		_ctr182:
			{
#line 800 "latex.rl"
			n++;}
		
#line 4566 "scanner.c"

		goto _st180;
		_ctr666:
			{
#line 803 "latex.rl"
			n=0;}
		
#line 4573 "scanner.c"

		goto _st180;
		_st180:
		if ( p == eof )
			goto _out180;
		p+= 1;
		st_case_180:
		if ( p == pe && p != eof )
			goto _out180;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 40: {
					goto _ctr182;
				}
				case 41: {
					_ck = 0;
					if ( (!n)
#line 4592 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr184;
					} else {
						goto _ctr183;
					}
					
				}
			}
			goto _st180;
		}
		_ctr186:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 4609 "scanner.c"

		goto _st629;
		_st629:
		if ( p == eof )
			goto _out629;
		p+= 1;
		st_case_629:
		if ( p == pe && p != eof )
			goto _out629;
		if ( p == eof ) {
			goto _ctr738;}
		else {
			switch( ( (*( p))) ) {
				case 39: {
					goto _st181;
				}
				case 45: {
					goto _st181;
				}
			}
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr186;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr186;
			}
			goto _ctr738;
		}
		_st181:
		if ( p == eof )
			goto _out181;
		p+= 1;
		st_case_181:
		if ( p == pe && p != eof )
			goto _out181;
		if ( p == eof ) {
			goto _ctr185;}
		else {
			if ( ( (*( p))) > 90 ) {
				if ( 97 <= ( (*( p))) && ( (*( p))) <= 122 ) {
					goto _ctr186;
				}
			} else if ( ( (*( p))) >= 65 ) {
				goto _ctr186;
			}
			goto _ctr185;
		}
		_ctr730:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 4662 "scanner.c"

		goto _st630;
		_st630:
		if ( p == eof )
			goto _out630;
		p+= 1;
		st_case_630:
		if ( p == pe && p != eof )
			goto _out630;
		if ( p == eof ) {
			goto _ctr735;}
		else {
			switch( ( (*( p))) ) {
				case 97: {
					goto _st182;
				}
				case 98: {
					goto _st200;
				}
				case 99: {
					goto _st543;
				}
				case 100: {
					goto _st557;
				}
				case 101: {
					goto _st570;
				}
				case 102: {
					goto _st558;
				}
				case 108: {
					goto _st582;
				}
				case 114: {
					goto _st586;
				}
				case 115: {
					goto _st590;
				}
				case 116: {
					goto _st598;
				}
				case 117: {
					goto _st604;
				}
			}
			goto _ctr735;
		}
		_st182:
		if ( p == eof )
			goto _out182;
		p+= 1;
		st_case_182:
		if ( p == pe && p != eof )
			goto _out182;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 102: {
					goto _st183;
				}
				case 117: {
					goto _st194;
				}
			}
			goto _ctr0;
		}
		_st183:
		if ( p == eof )
			goto _out183;
		p+= 1;
		st_case_183:
		if ( p == pe && p != eof )
			goto _out183;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 102 ) {
				goto _st184;
			}
			goto _ctr0;
		}
		_st184:
		if ( p == eof )
			goto _out184;
		p+= 1;
		st_case_184:
		if ( p == pe && p != eof )
			goto _out184;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st185;
			}
			goto _ctr0;
		}
		_st185:
		if ( p == eof )
			goto _out185;
		p+= 1;
		st_case_185:
		if ( p == pe && p != eof )
			goto _out185;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 108 ) {
				goto _st186;
			}
			goto _ctr0;
		}
		_st186:
		if ( p == eof )
			goto _out186;
		p+= 1;
		st_case_186:
		if ( p == pe && p != eof )
			goto _out186;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st187;
			}
			goto _ctr0;
		}
		_st187:
		if ( p == eof )
			goto _out187;
		p+= 1;
		st_case_187:
		if ( p == pe && p != eof )
			goto _out187;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st188;
			}
			goto _ctr0;
		}
		_st188:
		if ( p == eof )
			goto _out188;
		p+= 1;
		st_case_188:
		if ( p == pe && p != eof )
			goto _out188;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st189;
			}
			goto _ctr0;
		}
		_st189:
		if ( p == eof )
			goto _out189;
		p+= 1;
		st_case_189:
		if ( p == pe && p != eof )
			goto _out189;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st190;
			}
			goto _ctr0;
		}
		_st190:
		if ( p == eof )
			goto _out190;
		p+= 1;
		st_case_190:
		if ( p == pe && p != eof )
			goto _out190;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 111 ) {
				goto _st191;
			}
			goto _ctr0;
		}
		_st191:
		if ( p == eof )
			goto _out191;
		p+= 1;
		st_case_191:
		if ( p == pe && p != eof )
			goto _out191;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 110 ) {
				goto _st192;
			}
			goto _ctr0;
		}
		_st192:
		if ( p == eof )
			goto _out192;
		p+= 1;
		st_case_192:
		if ( p == pe && p != eof )
			goto _out192;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _ctr198;
			}
			goto _ctr0;
		}
		_ctr201:
			{
#line 791 "latex.rl"
			n--; }
		
#line 4886 "scanner.c"

		goto _st193;
		_ctr198:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 4893 "scanner.c"

		goto _st193;
		_ctr200:
			{
#line 790 "latex.rl"
			n++; }
		
#line 4900 "scanner.c"

		goto _st193;
		_st193:
		if ( p == eof )
			goto _out193;
		p+= 1;
		st_case_193:
		if ( p == pe && p != eof )
			goto _out193;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr200;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 4919 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr202;
					} else {
						goto _ctr201;
					}
					
				}
			}
			goto _st193;
		}
		_st194:
		if ( p == eof )
			goto _out194;
		p+= 1;
		st_case_194:
		if ( p == pe && p != eof )
			goto _out194;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st195;
			}
			goto _ctr0;
		}
		_st195:
		if ( p == eof )
			goto _out195;
		p+= 1;
		st_case_195:
		if ( p == pe && p != eof )
			goto _out195;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 104 ) {
				goto _st196;
			}
			goto _ctr0;
		}
		_st196:
		if ( p == eof )
			goto _out196;
		p+= 1;
		st_case_196:
		if ( p == pe && p != eof )
			goto _out196;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 111 ) {
				goto _st197;
			}
			goto _ctr0;
		}
		_st197:
		if ( p == eof )
			goto _out197;
		p+= 1;
		st_case_197:
		if ( p == pe && p != eof )
			goto _out197;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 114 ) {
				goto _st198;
			}
			goto _ctr0;
		}
		_st198:
		if ( p == eof )
			goto _out198;
		p+= 1;
		st_case_198:
		if ( p == pe && p != eof )
			goto _out198;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _ctr207;
			}
			goto _ctr0;
		}
		_ctr210:
			{
#line 791 "latex.rl"
			n--; }
		
#line 5011 "scanner.c"

		goto _st199;
		_ctr207:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 5018 "scanner.c"

		goto _st199;
		_ctr209:
			{
#line 790 "latex.rl"
			n++; }
		
#line 5025 "scanner.c"

		goto _st199;
		_st199:
		if ( p == eof )
			goto _out199;
		p+= 1;
		st_case_199:
		if ( p == pe && p != eof )
			goto _out199;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr209;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 5044 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr211;
					} else {
						goto _ctr210;
					}
					
				}
			}
			goto _st199;
		}
		_st200:
		if ( p == eof )
			goto _out200;
		p+= 1;
		st_case_200:
		if ( p == pe && p != eof )
			goto _out200;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 101: {
					goto _st201;
				}
				case 105: {
					goto _st535;
				}
			}
			goto _ctr0;
		}
		_st201:
		if ( p == eof )
			goto _out201;
		p+= 1;
		st_case_201:
		if ( p == pe && p != eof )
			goto _out201;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 103 ) {
				goto _st202;
			}
			goto _ctr0;
		}
		_st202:
		if ( p == eof )
			goto _out202;
		p+= 1;
		st_case_202:
		if ( p == pe && p != eof )
			goto _out202;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st203;
			}
			goto _ctr0;
		}
		_st203:
		if ( p == eof )
			goto _out203;
		p+= 1;
		st_case_203:
		if ( p == pe && p != eof )
			goto _out203;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 110 ) {
				goto _st204;
			}
			goto _ctr0;
		}
		_st204:
		if ( p == eof )
			goto _out204;
		p+= 1;
		st_case_204:
		if ( p == pe && p != eof )
			goto _out204;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _st205;
			}
			goto _ctr0;
		}
		_st205:
		if ( p == eof )
			goto _out205;
		p+= 1;
		st_case_205:
		if ( p == pe && p != eof )
			goto _out205;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 97: {
					goto _st206;
				}
				case 100: {
					goto _st270;
				}
				case 101: {
					goto _st278;
				}
				case 102: {
					goto _st382;
				}
				case 116: {
					goto _st413;
				}
				case 118: {
					goto _st498;
				}
			}
			goto _ctr0;
		}
		_st206:
		if ( p == eof )
			goto _out206;
		p+= 1;
		st_case_206:
		if ( p == pe && p != eof )
			goto _out206;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 98: {
					goto _st207;
				}
				case 108: {
					goto _st243;
				}
			}
			goto _ctr0;
		}
		_st207:
		if ( p == eof )
			goto _out207;
		p+= 1;
		st_case_207:
		if ( p == pe && p != eof )
			goto _out207;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 115 ) {
				goto _st208;
			}
			goto _ctr0;
		}
		_st208:
		if ( p == eof )
			goto _out208;
		p+= 1;
		st_case_208:
		if ( p == pe && p != eof )
			goto _out208;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st209;
			}
			goto _ctr0;
		}
		_st209:
		if ( p == eof )
			goto _out209;
		p+= 1;
		st_case_209:
		if ( p == pe && p != eof )
			goto _out209;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 114 ) {
				goto _st210;
			}
			goto _ctr0;
		}
		_st210:
		if ( p == eof )
			goto _out210;
		p+= 1;
		st_case_210:
		if ( p == pe && p != eof )
			goto _out210;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st211;
			}
			goto _ctr0;
		}
		_st211:
		if ( p == eof )
			goto _out211;
		p+= 1;
		st_case_211:
		if ( p == pe && p != eof )
			goto _out211;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 99 ) {
				goto _st212;
			}
			goto _ctr0;
		}
		_st212:
		if ( p == eof )
			goto _out212;
		p+= 1;
		st_case_212:
		if ( p == pe && p != eof )
			goto _out212;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st213;
			}
			goto _ctr0;
		}
		_st213:
		if ( p == eof )
			goto _out213;
		p+= 1;
		st_case_213:
		if ( p == pe && p != eof )
			goto _out213;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 125 ) {
				goto _ctr232;
			}
			goto _ctr0;
		}
		_ctr232:
			{
#line 9 "latex.rl"
			n=1;}
		
#line 5298 "scanner.c"

		goto _st214;
		_ctr250:
			{
#line 6 "latex.rl"
			n++; }
		
#line 5305 "scanner.c"

		goto _st214;
		_st214:
		if ( p == eof )
			goto _out214;
		p+= 1;
		st_case_214:
		if ( p == pe && p != eof )
			goto _out214;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				goto _st215;
			}
			goto _st214;
		}
		_st215:
		if ( p == eof )
			goto _out215;
		p+= 1;
		st_case_215:
		if ( p == pe && p != eof )
			goto _out215;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 98: {
					goto _st216;
				}
				case 101: {
					goto _st230;
				}
			}
			goto _st214;
		}
		_st216:
		if ( p == eof )
			goto _out216;
		p+= 1;
		st_case_216:
		if ( p == pe && p != eof )
			goto _out216;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 101: {
					goto _st217;
				}
			}
			goto _st214;
		}
		_st217:
		if ( p == eof )
			goto _out217;
		p+= 1;
		st_case_217:
		if ( p == pe && p != eof )
			goto _out217;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 103: {
					goto _st218;
				}
			}
			goto _st214;
		}
		_st218:
		if ( p == eof )
			goto _out218;
		p+= 1;
		st_case_218:
		if ( p == pe && p != eof )
			goto _out218;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 105: {
					goto _st219;
				}
			}
			goto _st214;
		}
		_st219:
		if ( p == eof )
			goto _out219;
		p+= 1;
		st_case_219:
		if ( p == pe && p != eof )
			goto _out219;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 110: {
					goto _st220;
				}
			}
			goto _st214;
		}
		_st220:
		if ( p == eof )
			goto _out220;
		p+= 1;
		st_case_220:
		if ( p == pe && p != eof )
			goto _out220;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 123: {
					goto _st221;
				}
			}
			goto _st214;
		}
		_st221:
		if ( p == eof )
			goto _out221;
		p+= 1;
		st_case_221:
		if ( p == pe && p != eof )
			goto _out221;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 97: {
					goto _st222;
				}
			}
			goto _st214;
		}
		_st222:
		if ( p == eof )
			goto _out222;
		p+= 1;
		st_case_222:
		if ( p == pe && p != eof )
			goto _out222;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 98: {
					goto _st223;
				}
			}
			goto _st214;
		}
		_st223:
		if ( p == eof )
			goto _out223;
		p+= 1;
		st_case_223:
		if ( p == pe && p != eof )
			goto _out223;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 115: {
					goto _st224;
				}
			}
			goto _st214;
		}
		_st224:
		if ( p == eof )
			goto _out224;
		p+= 1;
		st_case_224:
		if ( p == pe && p != eof )
			goto _out224;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 116: {
					goto _st225;
				}
			}
			goto _st214;
		}
		_st225:
		if ( p == eof )
			goto _out225;
		p+= 1;
		st_case_225:
		if ( p == pe && p != eof )
			goto _out225;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 114: {
					goto _st226;
				}
			}
			goto _st214;
		}
		_st226:
		if ( p == eof )
			goto _out226;
		p+= 1;
		st_case_226:
		if ( p == pe && p != eof )
			goto _out226;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 97: {
					goto _st227;
				}
			}
			goto _st214;
		}
		_st227:
		if ( p == eof )
			goto _out227;
		p+= 1;
		st_case_227:
		if ( p == pe && p != eof )
			goto _out227;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 99: {
					goto _st228;
				}
			}
			goto _st214;
		}
		_st228:
		if ( p == eof )
			goto _out228;
		p+= 1;
		st_case_228:
		if ( p == pe && p != eof )
			goto _out228;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 116: {
					goto _st229;
				}
			}
			goto _st214;
		}
		_st229:
		if ( p == eof )
			goto _out229;
		p+= 1;
		st_case_229:
		if ( p == pe && p != eof )
			goto _out229;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 125: {
					goto _ctr250;
				}
			}
			goto _st214;
		}
		_st230:
		if ( p == eof )
			goto _out230;
		p+= 1;
		st_case_230:
		if ( p == pe && p != eof )
			goto _out230;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 110: {
					goto _st231;
				}
			}
			goto _st214;
		}
		_st231:
		if ( p == eof )
			goto _out231;
		p+= 1;
		st_case_231:
		if ( p == pe && p != eof )
			goto _out231;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 100: {
					goto _st232;
				}
			}
			goto _st214;
		}
		_st232:
		if ( p == eof )
			goto _out232;
		p+= 1;
		st_case_232:
		if ( p == pe && p != eof )
			goto _out232;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 123: {
					goto _st233;
				}
			}
			goto _st214;
		}
		_st233:
		if ( p == eof )
			goto _out233;
		p+= 1;
		st_case_233:
		if ( p == pe && p != eof )
			goto _out233;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 97: {
					goto _st234;
				}
			}
			goto _st214;
		}
		_st234:
		if ( p == eof )
			goto _out234;
		p+= 1;
		st_case_234:
		if ( p == pe && p != eof )
			goto _out234;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 98: {
					goto _st235;
				}
			}
			goto _st214;
		}
		_st235:
		if ( p == eof )
			goto _out235;
		p+= 1;
		st_case_235:
		if ( p == pe && p != eof )
			goto _out235;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 115: {
					goto _st236;
				}
			}
			goto _st214;
		}
		_st236:
		if ( p == eof )
			goto _out236;
		p+= 1;
		st_case_236:
		if ( p == pe && p != eof )
			goto _out236;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 116: {
					goto _st237;
				}
			}
			goto _st214;
		}
		_st237:
		if ( p == eof )
			goto _out237;
		p+= 1;
		st_case_237:
		if ( p == pe && p != eof )
			goto _out237;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 114: {
					goto _st238;
				}
			}
			goto _st214;
		}
		_st238:
		if ( p == eof )
			goto _out238;
		p+= 1;
		st_case_238:
		if ( p == pe && p != eof )
			goto _out238;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 97: {
					goto _st239;
				}
			}
			goto _st214;
		}
		_st239:
		if ( p == eof )
			goto _out239;
		p+= 1;
		st_case_239:
		if ( p == pe && p != eof )
			goto _out239;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 99: {
					goto _st240;
				}
			}
			goto _st214;
		}
		_st240:
		if ( p == eof )
			goto _out240;
		p+= 1;
		st_case_240:
		if ( p == pe && p != eof )
			goto _out240;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 116: {
					goto _st241;
				}
			}
			goto _st214;
		}
		_st241:
		if ( p == eof )
			goto _out241;
		p+= 1;
		st_case_241:
		if ( p == pe && p != eof )
			goto _out241;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st215;
				}
				case 125: {
					goto _ctr262;
				}
			}
			goto _st214;
		}
		_ctr262:
			{
#line 7 "latex.rl"
			n--; }
		
#line 5870 "scanner.c"

		goto _st242;
		_st242:
		if ( p == eof )
			goto _out242;
		p+= 1;
		st_case_242:
		if ( p == pe && p != eof )
			goto _out242;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				_ck = 0;
				if ( (!n)
#line 5885 "scanner.c"
 )
				_ck += 1;
				if ( _ck > 0 ) {
					goto _ctr264;
				} else {
					goto _st215;
				}
				
			}
			_ck = 0;
			if ( (!n)
#line 5896 "scanner.c"
 )
			_ck += 1;
			if ( _ck > 0 ) {
				goto _ctr264;
			} else {
				goto _st214;
			}
			
		}
		_st243:
		if ( p == eof )
			goto _out243;
		p+= 1;
		st_case_243:
		if ( p == pe && p != eof )
			goto _out243;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st244;
			}
			goto _ctr0;
		}
		_st244:
		if ( p == eof )
			goto _out244;
		p+= 1;
		st_case_244:
		if ( p == pe && p != eof )
			goto _out244;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 103 ) {
				goto _st245;
			}
			goto _ctr0;
		}
		_st245:
		if ( p == eof )
			goto _out245;
		p+= 1;
		st_case_245:
		if ( p == pe && p != eof )
			goto _out245;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 110 ) {
				goto _st246;
			}
			goto _ctr0;
		}
		_st246:
		if ( p == eof )
			goto _out246;
		p+= 1;
		st_case_246:
		if ( p == pe && p != eof )
			goto _out246;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 125 ) {
				goto _ctr269;
			}
			goto _ctr0;
		}
		_ctr269:
			{
#line 45 "latex.rl"
			n=1;}
		
#line 5970 "scanner.c"

		goto _st247;
		_ctr284:
			{
#line 42 "latex.rl"
			n++; }
		
#line 5977 "scanner.c"

		goto _st247;
		_st247:
		if ( p == eof )
			goto _out247;
		p+= 1;
		st_case_247:
		if ( p == pe && p != eof )
			goto _out247;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				goto _st248;
			}
			goto _st247;
		}
		_st248:
		if ( p == eof )
			goto _out248;
		p+= 1;
		st_case_248:
		if ( p == pe && p != eof )
			goto _out248;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 98: {
					goto _st249;
				}
				case 101: {
					goto _st260;
				}
			}
			goto _st247;
		}
		_st249:
		if ( p == eof )
			goto _out249;
		p+= 1;
		st_case_249:
		if ( p == pe && p != eof )
			goto _out249;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 101: {
					goto _st250;
				}
			}
			goto _st247;
		}
		_st250:
		if ( p == eof )
			goto _out250;
		p+= 1;
		st_case_250:
		if ( p == pe && p != eof )
			goto _out250;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 103: {
					goto _st251;
				}
			}
			goto _st247;
		}
		_st251:
		if ( p == eof )
			goto _out251;
		p+= 1;
		st_case_251:
		if ( p == pe && p != eof )
			goto _out251;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 105: {
					goto _st252;
				}
			}
			goto _st247;
		}
		_st252:
		if ( p == eof )
			goto _out252;
		p+= 1;
		st_case_252:
		if ( p == pe && p != eof )
			goto _out252;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 110: {
					goto _st253;
				}
			}
			goto _st247;
		}
		_st253:
		if ( p == eof )
			goto _out253;
		p+= 1;
		st_case_253:
		if ( p == pe && p != eof )
			goto _out253;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 123: {
					goto _st254;
				}
			}
			goto _st247;
		}
		_st254:
		if ( p == eof )
			goto _out254;
		p+= 1;
		st_case_254:
		if ( p == pe && p != eof )
			goto _out254;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 97: {
					goto _st255;
				}
			}
			goto _st247;
		}
		_st255:
		if ( p == eof )
			goto _out255;
		p+= 1;
		st_case_255:
		if ( p == pe && p != eof )
			goto _out255;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 108: {
					goto _st256;
				}
			}
			goto _st247;
		}
		_st256:
		if ( p == eof )
			goto _out256;
		p+= 1;
		st_case_256:
		if ( p == pe && p != eof )
			goto _out256;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 105: {
					goto _st257;
				}
			}
			goto _st247;
		}
		_st257:
		if ( p == eof )
			goto _out257;
		p+= 1;
		st_case_257:
		if ( p == pe && p != eof )
			goto _out257;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 103: {
					goto _st258;
				}
			}
			goto _st247;
		}
		_st258:
		if ( p == eof )
			goto _out258;
		p+= 1;
		st_case_258:
		if ( p == pe && p != eof )
			goto _out258;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 110: {
					goto _st259;
				}
			}
			goto _st247;
		}
		_st259:
		if ( p == eof )
			goto _out259;
		p+= 1;
		st_case_259:
		if ( p == pe && p != eof )
			goto _out259;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 125: {
					goto _ctr284;
				}
			}
			goto _st247;
		}
		_st260:
		if ( p == eof )
			goto _out260;
		p+= 1;
		st_case_260:
		if ( p == pe && p != eof )
			goto _out260;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 110: {
					goto _st261;
				}
			}
			goto _st247;
		}
		_st261:
		if ( p == eof )
			goto _out261;
		p+= 1;
		st_case_261:
		if ( p == pe && p != eof )
			goto _out261;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 100: {
					goto _st262;
				}
			}
			goto _st247;
		}
		_st262:
		if ( p == eof )
			goto _out262;
		p+= 1;
		st_case_262:
		if ( p == pe && p != eof )
			goto _out262;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 123: {
					goto _st263;
				}
			}
			goto _st247;
		}
		_st263:
		if ( p == eof )
			goto _out263;
		p+= 1;
		st_case_263:
		if ( p == pe && p != eof )
			goto _out263;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 97: {
					goto _st264;
				}
			}
			goto _st247;
		}
		_st264:
		if ( p == eof )
			goto _out264;
		p+= 1;
		st_case_264:
		if ( p == pe && p != eof )
			goto _out264;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 108: {
					goto _st265;
				}
			}
			goto _st247;
		}
		_st265:
		if ( p == eof )
			goto _out265;
		p+= 1;
		st_case_265:
		if ( p == pe && p != eof )
			goto _out265;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 105: {
					goto _st266;
				}
			}
			goto _st247;
		}
		_st266:
		if ( p == eof )
			goto _out266;
		p+= 1;
		st_case_266:
		if ( p == pe && p != eof )
			goto _out266;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 103: {
					goto _st267;
				}
			}
			goto _st247;
		}
		_st267:
		if ( p == eof )
			goto _out267;
		p+= 1;
		st_case_267:
		if ( p == pe && p != eof )
			goto _out267;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 110: {
					goto _st268;
				}
			}
			goto _st247;
		}
		_st268:
		if ( p == eof )
			goto _out268;
		p+= 1;
		st_case_268:
		if ( p == pe && p != eof )
			goto _out268;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st248;
				}
				case 125: {
					goto _ctr293;
				}
			}
			goto _st247;
		}
		_ctr293:
			{
#line 43 "latex.rl"
			n--; }
		
#line 6422 "scanner.c"

		goto _st269;
		_st269:
		if ( p == eof )
			goto _out269;
		p+= 1;
		st_case_269:
		if ( p == pe && p != eof )
			goto _out269;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				_ck = 0;
				if ( (!n)
#line 6437 "scanner.c"
 )
				_ck += 1;
				if ( _ck > 0 ) {
					goto _ctr295;
				} else {
					goto _st248;
				}
				
			}
			_ck = 0;
			if ( (!n)
#line 6448 "scanner.c"
 )
			_ck += 1;
			if ( _ck > 0 ) {
				goto _ctr295;
			} else {
				goto _st247;
			}
			
		}
		_st270:
		if ( p == eof )
			goto _out270;
		p+= 1;
		st_case_270:
		if ( p == pe && p != eof )
			goto _out270;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 111 ) {
				goto _st271;
			}
			goto _ctr0;
		}
		_st271:
		if ( p == eof )
			goto _out271;
		p+= 1;
		st_case_271:
		if ( p == pe && p != eof )
			goto _out271;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 99 ) {
				goto _st272;
			}
			goto _ctr0;
		}
		_st272:
		if ( p == eof )
			goto _out272;
		p+= 1;
		st_case_272:
		if ( p == pe && p != eof )
			goto _out272;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 117 ) {
				goto _st273;
			}
			goto _ctr0;
		}
		_st273:
		if ( p == eof )
			goto _out273;
		p+= 1;
		st_case_273:
		if ( p == pe && p != eof )
			goto _out273;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 109 ) {
				goto _st274;
			}
			goto _ctr0;
		}
		_st274:
		if ( p == eof )
			goto _out274;
		p+= 1;
		st_case_274:
		if ( p == pe && p != eof )
			goto _out274;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st275;
			}
			goto _ctr0;
		}
		_st275:
		if ( p == eof )
			goto _out275;
		p+= 1;
		st_case_275:
		if ( p == pe && p != eof )
			goto _out275;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 110 ) {
				goto _st276;
			}
			goto _ctr0;
		}
		_st276:
		if ( p == eof )
			goto _out276;
		p+= 1;
		st_case_276:
		if ( p == pe && p != eof )
			goto _out276;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st277;
			}
			goto _ctr0;
		}
		_st277:
		if ( p == eof )
			goto _out277;
		p+= 1;
		st_case_277:
		if ( p == pe && p != eof )
			goto _out277;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 125 ) {
				goto _ctr304;
			}
			goto _ctr0;
		}
		_st278:
		if ( p == eof )
			goto _out278;
		p+= 1;
		st_case_278:
		if ( p == pe && p != eof )
			goto _out278;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 113 ) {
				goto _st279;
			}
			goto _ctr0;
		}
		_st279:
		if ( p == eof )
			goto _out279;
		p+= 1;
		st_case_279:
		if ( p == pe && p != eof )
			goto _out279;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 110: {
					goto _st280;
				}
				case 117: {
					goto _st315;
				}
			}
			goto _ctr0;
		}
		_st280:
		if ( p == eof )
			goto _out280;
		p+= 1;
		st_case_280:
		if ( p == pe && p != eof )
			goto _out280;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st281;
			}
			goto _ctr0;
		}
		_st281:
		if ( p == eof )
			goto _out281;
		p+= 1;
		st_case_281:
		if ( p == pe && p != eof )
			goto _out281;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 114 ) {
				goto _st282;
			}
			goto _ctr0;
		}
		_st282:
		if ( p == eof )
			goto _out282;
		p+= 1;
		st_case_282:
		if ( p == pe && p != eof )
			goto _out282;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 114 ) {
				goto _st283;
			}
			goto _ctr0;
		}
		_st283:
		if ( p == eof )
			goto _out283;
		p+= 1;
		st_case_283:
		if ( p == pe && p != eof )
			goto _out283;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st284;
			}
			goto _ctr0;
		}
		_st284:
		if ( p == eof )
			goto _out284;
		p+= 1;
		st_case_284:
		if ( p == pe && p != eof )
			goto _out284;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 121 ) {
				goto _st285;
			}
			goto _ctr0;
		}
		_st285:
		if ( p == eof )
			goto _out285;
		p+= 1;
		st_case_285:
		if ( p == pe && p != eof )
			goto _out285;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 125 ) {
				goto _ctr313;
			}
			goto _ctr0;
		}
		_ctr313:
			{
#line 229 "latex.rl"
			n=1;}
		
#line 6707 "scanner.c"

		goto _st286;
		_ctr331:
			{
#line 226 "latex.rl"
			n++; }
		
#line 6714 "scanner.c"

		goto _st286;
		_st286:
		if ( p == eof )
			goto _out286;
		p+= 1;
		st_case_286:
		if ( p == pe && p != eof )
			goto _out286;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				goto _st287;
			}
			goto _st286;
		}
		_st287:
		if ( p == eof )
			goto _out287;
		p+= 1;
		st_case_287:
		if ( p == pe && p != eof )
			goto _out287;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 98: {
					goto _st288;
				}
				case 101: {
					goto _st302;
				}
			}
			goto _st286;
		}
		_st288:
		if ( p == eof )
			goto _out288;
		p+= 1;
		st_case_288:
		if ( p == pe && p != eof )
			goto _out288;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 101: {
					goto _st289;
				}
			}
			goto _st286;
		}
		_st289:
		if ( p == eof )
			goto _out289;
		p+= 1;
		st_case_289:
		if ( p == pe && p != eof )
			goto _out289;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 103: {
					goto _st290;
				}
			}
			goto _st286;
		}
		_st290:
		if ( p == eof )
			goto _out290;
		p+= 1;
		st_case_290:
		if ( p == pe && p != eof )
			goto _out290;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 105: {
					goto _st291;
				}
			}
			goto _st286;
		}
		_st291:
		if ( p == eof )
			goto _out291;
		p+= 1;
		st_case_291:
		if ( p == pe && p != eof )
			goto _out291;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 110: {
					goto _st292;
				}
			}
			goto _st286;
		}
		_st292:
		if ( p == eof )
			goto _out292;
		p+= 1;
		st_case_292:
		if ( p == pe && p != eof )
			goto _out292;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 123: {
					goto _st293;
				}
			}
			goto _st286;
		}
		_st293:
		if ( p == eof )
			goto _out293;
		p+= 1;
		st_case_293:
		if ( p == pe && p != eof )
			goto _out293;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 101: {
					goto _st294;
				}
			}
			goto _st286;
		}
		_st294:
		if ( p == eof )
			goto _out294;
		p+= 1;
		st_case_294:
		if ( p == pe && p != eof )
			goto _out294;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 113: {
					goto _st295;
				}
			}
			goto _st286;
		}
		_st295:
		if ( p == eof )
			goto _out295;
		p+= 1;
		st_case_295:
		if ( p == pe && p != eof )
			goto _out295;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 110: {
					goto _st296;
				}
			}
			goto _st286;
		}
		_st296:
		if ( p == eof )
			goto _out296;
		p+= 1;
		st_case_296:
		if ( p == pe && p != eof )
			goto _out296;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 97: {
					goto _st297;
				}
			}
			goto _st286;
		}
		_st297:
		if ( p == eof )
			goto _out297;
		p+= 1;
		st_case_297:
		if ( p == pe && p != eof )
			goto _out297;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 114: {
					goto _st298;
				}
			}
			goto _st286;
		}
		_st298:
		if ( p == eof )
			goto _out298;
		p+= 1;
		st_case_298:
		if ( p == pe && p != eof )
			goto _out298;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 114: {
					goto _st299;
				}
			}
			goto _st286;
		}
		_st299:
		if ( p == eof )
			goto _out299;
		p+= 1;
		st_case_299:
		if ( p == pe && p != eof )
			goto _out299;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 97: {
					goto _st300;
				}
			}
			goto _st286;
		}
		_st300:
		if ( p == eof )
			goto _out300;
		p+= 1;
		st_case_300:
		if ( p == pe && p != eof )
			goto _out300;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 121: {
					goto _st301;
				}
			}
			goto _st286;
		}
		_st301:
		if ( p == eof )
			goto _out301;
		p+= 1;
		st_case_301:
		if ( p == pe && p != eof )
			goto _out301;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 125: {
					goto _ctr331;
				}
			}
			goto _st286;
		}
		_st302:
		if ( p == eof )
			goto _out302;
		p+= 1;
		st_case_302:
		if ( p == pe && p != eof )
			goto _out302;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 110: {
					goto _st303;
				}
			}
			goto _st286;
		}
		_st303:
		if ( p == eof )
			goto _out303;
		p+= 1;
		st_case_303:
		if ( p == pe && p != eof )
			goto _out303;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 100: {
					goto _st304;
				}
			}
			goto _st286;
		}
		_st304:
		if ( p == eof )
			goto _out304;
		p+= 1;
		st_case_304:
		if ( p == pe && p != eof )
			goto _out304;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 123: {
					goto _st305;
				}
			}
			goto _st286;
		}
		_st305:
		if ( p == eof )
			goto _out305;
		p+= 1;
		st_case_305:
		if ( p == pe && p != eof )
			goto _out305;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 101: {
					goto _st306;
				}
			}
			goto _st286;
		}
		_st306:
		if ( p == eof )
			goto _out306;
		p+= 1;
		st_case_306:
		if ( p == pe && p != eof )
			goto _out306;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 113: {
					goto _st307;
				}
			}
			goto _st286;
		}
		_st307:
		if ( p == eof )
			goto _out307;
		p+= 1;
		st_case_307:
		if ( p == pe && p != eof )
			goto _out307;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 110: {
					goto _st308;
				}
			}
			goto _st286;
		}
		_st308:
		if ( p == eof )
			goto _out308;
		p+= 1;
		st_case_308:
		if ( p == pe && p != eof )
			goto _out308;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 97: {
					goto _st309;
				}
			}
			goto _st286;
		}
		_st309:
		if ( p == eof )
			goto _out309;
		p+= 1;
		st_case_309:
		if ( p == pe && p != eof )
			goto _out309;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 114: {
					goto _st310;
				}
			}
			goto _st286;
		}
		_st310:
		if ( p == eof )
			goto _out310;
		p+= 1;
		st_case_310:
		if ( p == pe && p != eof )
			goto _out310;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 114: {
					goto _st311;
				}
			}
			goto _st286;
		}
		_st311:
		if ( p == eof )
			goto _out311;
		p+= 1;
		st_case_311:
		if ( p == pe && p != eof )
			goto _out311;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 97: {
					goto _st312;
				}
			}
			goto _st286;
		}
		_st312:
		if ( p == eof )
			goto _out312;
		p+= 1;
		st_case_312:
		if ( p == pe && p != eof )
			goto _out312;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 121: {
					goto _st313;
				}
			}
			goto _st286;
		}
		_st313:
		if ( p == eof )
			goto _out313;
		p+= 1;
		st_case_313:
		if ( p == pe && p != eof )
			goto _out313;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st287;
				}
				case 125: {
					goto _ctr343;
				}
			}
			goto _st286;
		}
		_ctr343:
			{
#line 227 "latex.rl"
			n--; }
		
#line 7279 "scanner.c"

		goto _st314;
		_st314:
		if ( p == eof )
			goto _out314;
		p+= 1;
		st_case_314:
		if ( p == pe && p != eof )
			goto _out314;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				_ck = 0;
				if ( (!n)
#line 7294 "scanner.c"
 )
				_ck += 1;
				if ( _ck > 0 ) {
					goto _ctr345;
				} else {
					goto _st287;
				}
				
			}
			_ck = 0;
			if ( (!n)
#line 7305 "scanner.c"
 )
			_ck += 1;
			if ( _ck > 0 ) {
				goto _ctr345;
			} else {
				goto _st286;
			}
			
		}
		_st315:
		if ( p == eof )
			goto _out315;
		p+= 1;
		st_case_315:
		if ( p == pe && p != eof )
			goto _out315;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st316;
			}
			goto _ctr0;
		}
		_st316:
		if ( p == eof )
			goto _out316;
		p+= 1;
		st_case_316:
		if ( p == pe && p != eof )
			goto _out316;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st317;
			}
			goto _ctr0;
		}
		_st317:
		if ( p == eof )
			goto _out317;
		p+= 1;
		st_case_317:
		if ( p == pe && p != eof )
			goto _out317;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st318;
			}
			goto _ctr0;
		}
		_st318:
		if ( p == eof )
			goto _out318;
		p+= 1;
		st_case_318:
		if ( p == pe && p != eof )
			goto _out318;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 111 ) {
				goto _st319;
			}
			goto _ctr0;
		}
		_st319:
		if ( p == eof )
			goto _out319;
		p+= 1;
		st_case_319:
		if ( p == pe && p != eof )
			goto _out319;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 110 ) {
				goto _st320;
			}
			goto _ctr0;
		}
		_st320:
		if ( p == eof )
			goto _out320;
		p+= 1;
		st_case_320:
		if ( p == pe && p != eof )
			goto _out320;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 42: {
					goto _st321;
				}
				case 125: {
					goto _ctr353;
				}
			}
			goto _ctr0;
		}
		_st321:
		if ( p == eof )
			goto _out321;
		p+= 1;
		st_case_321:
		if ( p == pe && p != eof )
			goto _out321;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 125 ) {
				goto _ctr354;
			}
			goto _ctr0;
		}
		_ctr354:
			{
#line 244 "latex.rl"
			n=1; }
		
#line 7429 "scanner.c"

		goto _st322;
		_ctr373:
			{
#line 241 "latex.rl"
			n++; }
		
#line 7436 "scanner.c"

		goto _st322;
		_st322:
		if ( p == eof )
			goto _out322;
		p+= 1;
		st_case_322:
		if ( p == pe && p != eof )
			goto _out322;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				goto _st323;
			}
			goto _st322;
		}
		_st323:
		if ( p == eof )
			goto _out323;
		p+= 1;
		st_case_323:
		if ( p == pe && p != eof )
			goto _out323;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 98: {
					goto _st324;
				}
				case 101: {
					goto _st339;
				}
			}
			goto _st322;
		}
		_st324:
		if ( p == eof )
			goto _out324;
		p+= 1;
		st_case_324:
		if ( p == pe && p != eof )
			goto _out324;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 101: {
					goto _st325;
				}
			}
			goto _st322;
		}
		_st325:
		if ( p == eof )
			goto _out325;
		p+= 1;
		st_case_325:
		if ( p == pe && p != eof )
			goto _out325;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 103: {
					goto _st326;
				}
			}
			goto _st322;
		}
		_st326:
		if ( p == eof )
			goto _out326;
		p+= 1;
		st_case_326:
		if ( p == pe && p != eof )
			goto _out326;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 105: {
					goto _st327;
				}
			}
			goto _st322;
		}
		_st327:
		if ( p == eof )
			goto _out327;
		p+= 1;
		st_case_327:
		if ( p == pe && p != eof )
			goto _out327;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 110: {
					goto _st328;
				}
			}
			goto _st322;
		}
		_st328:
		if ( p == eof )
			goto _out328;
		p+= 1;
		st_case_328:
		if ( p == pe && p != eof )
			goto _out328;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 123: {
					goto _st329;
				}
			}
			goto _st322;
		}
		_st329:
		if ( p == eof )
			goto _out329;
		p+= 1;
		st_case_329:
		if ( p == pe && p != eof )
			goto _out329;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 101: {
					goto _st330;
				}
			}
			goto _st322;
		}
		_st330:
		if ( p == eof )
			goto _out330;
		p+= 1;
		st_case_330:
		if ( p == pe && p != eof )
			goto _out330;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 113: {
					goto _st331;
				}
			}
			goto _st322;
		}
		_st331:
		if ( p == eof )
			goto _out331;
		p+= 1;
		st_case_331:
		if ( p == pe && p != eof )
			goto _out331;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 117: {
					goto _st332;
				}
			}
			goto _st322;
		}
		_st332:
		if ( p == eof )
			goto _out332;
		p+= 1;
		st_case_332:
		if ( p == pe && p != eof )
			goto _out332;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 97: {
					goto _st333;
				}
			}
			goto _st322;
		}
		_st333:
		if ( p == eof )
			goto _out333;
		p+= 1;
		st_case_333:
		if ( p == pe && p != eof )
			goto _out333;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 116: {
					goto _st334;
				}
			}
			goto _st322;
		}
		_st334:
		if ( p == eof )
			goto _out334;
		p+= 1;
		st_case_334:
		if ( p == pe && p != eof )
			goto _out334;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 105: {
					goto _st335;
				}
			}
			goto _st322;
		}
		_st335:
		if ( p == eof )
			goto _out335;
		p+= 1;
		st_case_335:
		if ( p == pe && p != eof )
			goto _out335;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 111: {
					goto _st336;
				}
			}
			goto _st322;
		}
		_st336:
		if ( p == eof )
			goto _out336;
		p+= 1;
		st_case_336:
		if ( p == pe && p != eof )
			goto _out336;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 110: {
					goto _st337;
				}
			}
			goto _st322;
		}
		_st337:
		if ( p == eof )
			goto _out337;
		p+= 1;
		st_case_337:
		if ( p == pe && p != eof )
			goto _out337;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 42: {
					goto _st338;
				}
				case 92: {
					goto _st323;
				}
			}
			goto _st322;
		}
		_st338:
		if ( p == eof )
			goto _out338;
		p+= 1;
		st_case_338:
		if ( p == pe && p != eof )
			goto _out338;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 125: {
					goto _ctr373;
				}
			}
			goto _st322;
		}
		_st339:
		if ( p == eof )
			goto _out339;
		p+= 1;
		st_case_339:
		if ( p == pe && p != eof )
			goto _out339;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 110: {
					goto _st340;
				}
			}
			goto _st322;
		}
		_st340:
		if ( p == eof )
			goto _out340;
		p+= 1;
		st_case_340:
		if ( p == pe && p != eof )
			goto _out340;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 100: {
					goto _st341;
				}
			}
			goto _st322;
		}
		_st341:
		if ( p == eof )
			goto _out341;
		p+= 1;
		st_case_341:
		if ( p == pe && p != eof )
			goto _out341;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 123: {
					goto _st342;
				}
			}
			goto _st322;
		}
		_st342:
		if ( p == eof )
			goto _out342;
		p+= 1;
		st_case_342:
		if ( p == pe && p != eof )
			goto _out342;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 101: {
					goto _st343;
				}
			}
			goto _st322;
		}
		_st343:
		if ( p == eof )
			goto _out343;
		p+= 1;
		st_case_343:
		if ( p == pe && p != eof )
			goto _out343;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 113: {
					goto _st344;
				}
			}
			goto _st322;
		}
		_st344:
		if ( p == eof )
			goto _out344;
		p+= 1;
		st_case_344:
		if ( p == pe && p != eof )
			goto _out344;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 117: {
					goto _st345;
				}
			}
			goto _st322;
		}
		_st345:
		if ( p == eof )
			goto _out345;
		p+= 1;
		st_case_345:
		if ( p == pe && p != eof )
			goto _out345;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 97: {
					goto _st346;
				}
			}
			goto _st322;
		}
		_st346:
		if ( p == eof )
			goto _out346;
		p+= 1;
		st_case_346:
		if ( p == pe && p != eof )
			goto _out346;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 116: {
					goto _st347;
				}
			}
			goto _st322;
		}
		_st347:
		if ( p == eof )
			goto _out347;
		p+= 1;
		st_case_347:
		if ( p == pe && p != eof )
			goto _out347;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 105: {
					goto _st348;
				}
			}
			goto _st322;
		}
		_st348:
		if ( p == eof )
			goto _out348;
		p+= 1;
		st_case_348:
		if ( p == pe && p != eof )
			goto _out348;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 111: {
					goto _st349;
				}
			}
			goto _st322;
		}
		_st349:
		if ( p == eof )
			goto _out349;
		p+= 1;
		st_case_349:
		if ( p == pe && p != eof )
			goto _out349;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 110: {
					goto _st350;
				}
			}
			goto _st322;
		}
		_st350:
		if ( p == eof )
			goto _out350;
		p+= 1;
		st_case_350:
		if ( p == pe && p != eof )
			goto _out350;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 42: {
					goto _st351;
				}
				case 92: {
					goto _st323;
				}
			}
			goto _st322;
		}
		_st351:
		if ( p == eof )
			goto _out351;
		p+= 1;
		st_case_351:
		if ( p == pe && p != eof )
			goto _out351;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st323;
				}
				case 125: {
					goto _ctr386;
				}
			}
			goto _st322;
		}
		_ctr386:
			{
#line 242 "latex.rl"
			n--; }
		
#line 8041 "scanner.c"

		goto _st352;
		_st352:
		if ( p == eof )
			goto _out352;
		p+= 1;
		st_case_352:
		if ( p == pe && p != eof )
			goto _out352;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				_ck = 0;
				if ( (!n)
#line 8056 "scanner.c"
 )
				_ck += 1;
				if ( _ck > 0 ) {
					goto _ctr388;
				} else {
					goto _st323;
				}
				
			}
			_ck = 0;
			if ( (!n)
#line 8067 "scanner.c"
 )
			_ck += 1;
			if ( _ck > 0 ) {
				goto _ctr388;
			} else {
				goto _st322;
			}
			
		}
		_ctr353:
			{
#line 238 "latex.rl"
			n=1; }
		
#line 8081 "scanner.c"

		goto _st353;
		_ctr407:
			{
#line 235 "latex.rl"
			n++; }
		
#line 8088 "scanner.c"

		goto _st353;
		_st353:
		if ( p == eof )
			goto _out353;
		p+= 1;
		st_case_353:
		if ( p == pe && p != eof )
			goto _out353;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				goto _st354;
			}
			goto _st353;
		}
		_st354:
		if ( p == eof )
			goto _out354;
		p+= 1;
		st_case_354:
		if ( p == pe && p != eof )
			goto _out354;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 98: {
					goto _st355;
				}
				case 101: {
					goto _st369;
				}
			}
			goto _st353;
		}
		_st355:
		if ( p == eof )
			goto _out355;
		p+= 1;
		st_case_355:
		if ( p == pe && p != eof )
			goto _out355;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 101: {
					goto _st356;
				}
			}
			goto _st353;
		}
		_st356:
		if ( p == eof )
			goto _out356;
		p+= 1;
		st_case_356:
		if ( p == pe && p != eof )
			goto _out356;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 103: {
					goto _st357;
				}
			}
			goto _st353;
		}
		_st357:
		if ( p == eof )
			goto _out357;
		p+= 1;
		st_case_357:
		if ( p == pe && p != eof )
			goto _out357;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 105: {
					goto _st358;
				}
			}
			goto _st353;
		}
		_st358:
		if ( p == eof )
			goto _out358;
		p+= 1;
		st_case_358:
		if ( p == pe && p != eof )
			goto _out358;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 110: {
					goto _st359;
				}
			}
			goto _st353;
		}
		_st359:
		if ( p == eof )
			goto _out359;
		p+= 1;
		st_case_359:
		if ( p == pe && p != eof )
			goto _out359;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 123: {
					goto _st360;
				}
			}
			goto _st353;
		}
		_st360:
		if ( p == eof )
			goto _out360;
		p+= 1;
		st_case_360:
		if ( p == pe && p != eof )
			goto _out360;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 101: {
					goto _st361;
				}
			}
			goto _st353;
		}
		_st361:
		if ( p == eof )
			goto _out361;
		p+= 1;
		st_case_361:
		if ( p == pe && p != eof )
			goto _out361;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 113: {
					goto _st362;
				}
			}
			goto _st353;
		}
		_st362:
		if ( p == eof )
			goto _out362;
		p+= 1;
		st_case_362:
		if ( p == pe && p != eof )
			goto _out362;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 117: {
					goto _st363;
				}
			}
			goto _st353;
		}
		_st363:
		if ( p == eof )
			goto _out363;
		p+= 1;
		st_case_363:
		if ( p == pe && p != eof )
			goto _out363;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 97: {
					goto _st364;
				}
			}
			goto _st353;
		}
		_st364:
		if ( p == eof )
			goto _out364;
		p+= 1;
		st_case_364:
		if ( p == pe && p != eof )
			goto _out364;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 116: {
					goto _st365;
				}
			}
			goto _st353;
		}
		_st365:
		if ( p == eof )
			goto _out365;
		p+= 1;
		st_case_365:
		if ( p == pe && p != eof )
			goto _out365;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 105: {
					goto _st366;
				}
			}
			goto _st353;
		}
		_st366:
		if ( p == eof )
			goto _out366;
		p+= 1;
		st_case_366:
		if ( p == pe && p != eof )
			goto _out366;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 111: {
					goto _st367;
				}
			}
			goto _st353;
		}
		_st367:
		if ( p == eof )
			goto _out367;
		p+= 1;
		st_case_367:
		if ( p == pe && p != eof )
			goto _out367;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 110: {
					goto _st368;
				}
			}
			goto _st353;
		}
		_st368:
		if ( p == eof )
			goto _out368;
		p+= 1;
		st_case_368:
		if ( p == pe && p != eof )
			goto _out368;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 125: {
					goto _ctr407;
				}
			}
			goto _st353;
		}
		_st369:
		if ( p == eof )
			goto _out369;
		p+= 1;
		st_case_369:
		if ( p == pe && p != eof )
			goto _out369;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 110: {
					goto _st370;
				}
			}
			goto _st353;
		}
		_st370:
		if ( p == eof )
			goto _out370;
		p+= 1;
		st_case_370:
		if ( p == pe && p != eof )
			goto _out370;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 100: {
					goto _st371;
				}
			}
			goto _st353;
		}
		_st371:
		if ( p == eof )
			goto _out371;
		p+= 1;
		st_case_371:
		if ( p == pe && p != eof )
			goto _out371;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 123: {
					goto _st372;
				}
			}
			goto _st353;
		}
		_st372:
		if ( p == eof )
			goto _out372;
		p+= 1;
		st_case_372:
		if ( p == pe && p != eof )
			goto _out372;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 101: {
					goto _st373;
				}
			}
			goto _st353;
		}
		_st373:
		if ( p == eof )
			goto _out373;
		p+= 1;
		st_case_373:
		if ( p == pe && p != eof )
			goto _out373;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 113: {
					goto _st374;
				}
			}
			goto _st353;
		}
		_st374:
		if ( p == eof )
			goto _out374;
		p+= 1;
		st_case_374:
		if ( p == pe && p != eof )
			goto _out374;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 117: {
					goto _st375;
				}
			}
			goto _st353;
		}
		_st375:
		if ( p == eof )
			goto _out375;
		p+= 1;
		st_case_375:
		if ( p == pe && p != eof )
			goto _out375;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 97: {
					goto _st376;
				}
			}
			goto _st353;
		}
		_st376:
		if ( p == eof )
			goto _out376;
		p+= 1;
		st_case_376:
		if ( p == pe && p != eof )
			goto _out376;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 116: {
					goto _st377;
				}
			}
			goto _st353;
		}
		_st377:
		if ( p == eof )
			goto _out377;
		p+= 1;
		st_case_377:
		if ( p == pe && p != eof )
			goto _out377;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 105: {
					goto _st378;
				}
			}
			goto _st353;
		}
		_st378:
		if ( p == eof )
			goto _out378;
		p+= 1;
		st_case_378:
		if ( p == pe && p != eof )
			goto _out378;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 111: {
					goto _st379;
				}
			}
			goto _st353;
		}
		_st379:
		if ( p == eof )
			goto _out379;
		p+= 1;
		st_case_379:
		if ( p == pe && p != eof )
			goto _out379;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 110: {
					goto _st380;
				}
			}
			goto _st353;
		}
		_st380:
		if ( p == eof )
			goto _out380;
		p+= 1;
		st_case_380:
		if ( p == pe && p != eof )
			goto _out380;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st354;
				}
				case 125: {
					goto _ctr419;
				}
			}
			goto _st353;
		}
		_ctr419:
			{
#line 236 "latex.rl"
			n--; }
		
#line 8653 "scanner.c"

		goto _st381;
		_st381:
		if ( p == eof )
			goto _out381;
		p+= 1;
		st_case_381:
		if ( p == pe && p != eof )
			goto _out381;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				_ck = 0;
				if ( (!n)
#line 8668 "scanner.c"
 )
				_ck += 1;
				if ( _ck > 0 ) {
					goto _ctr388;
				} else {
					goto _st354;
				}
				
			}
			_ck = 0;
			if ( (!n)
#line 8679 "scanner.c"
 )
			_ck += 1;
			if ( _ck > 0 ) {
				goto _ctr388;
			} else {
				goto _st353;
			}
			
		}
		_st382:
		if ( p == eof )
			goto _out382;
		p+= 1;
		st_case_382:
		if ( p == pe && p != eof )
			goto _out382;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st383;
			}
			goto _ctr0;
		}
		_st383:
		if ( p == eof )
			goto _out383;
		p+= 1;
		st_case_383:
		if ( p == pe && p != eof )
			goto _out383;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 103 ) {
				goto _st384;
			}
			goto _ctr0;
		}
		_st384:
		if ( p == eof )
			goto _out384;
		p+= 1;
		st_case_384:
		if ( p == pe && p != eof )
			goto _out384;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 117 ) {
				goto _st385;
			}
			goto _ctr0;
		}
		_st385:
		if ( p == eof )
			goto _out385;
		p+= 1;
		st_case_385:
		if ( p == pe && p != eof )
			goto _out385;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 114 ) {
				goto _st386;
			}
			goto _ctr0;
		}
		_st386:
		if ( p == eof )
			goto _out386;
		p+= 1;
		st_case_386:
		if ( p == pe && p != eof )
			goto _out386;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st387;
			}
			goto _ctr0;
		}
		_st387:
		if ( p == eof )
			goto _out387;
		p+= 1;
		st_case_387:
		if ( p == pe && p != eof )
			goto _out387;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 125 ) {
				goto _ctr427;
			}
			goto _ctr0;
		}
		_ctr427:
			{
#line 285 "latex.rl"
			n=1;}
		
#line 8783 "scanner.c"

		goto _st388;
		_ctr443:
			{
#line 282 "latex.rl"
			n++; }
		
#line 8790 "scanner.c"

		goto _st388;
		_st388:
		if ( p == eof )
			goto _out388;
		p+= 1;
		st_case_388:
		if ( p == pe && p != eof )
			goto _out388;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				goto _st389;
			}
			goto _st388;
		}
		_st389:
		if ( p == eof )
			goto _out389;
		p+= 1;
		st_case_389:
		if ( p == pe && p != eof )
			goto _out389;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 98: {
					goto _st390;
				}
				case 101: {
					goto _st402;
				}
			}
			goto _st388;
		}
		_st390:
		if ( p == eof )
			goto _out390;
		p+= 1;
		st_case_390:
		if ( p == pe && p != eof )
			goto _out390;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 101: {
					goto _st391;
				}
			}
			goto _st388;
		}
		_st391:
		if ( p == eof )
			goto _out391;
		p+= 1;
		st_case_391:
		if ( p == pe && p != eof )
			goto _out391;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 103: {
					goto _st392;
				}
			}
			goto _st388;
		}
		_st392:
		if ( p == eof )
			goto _out392;
		p+= 1;
		st_case_392:
		if ( p == pe && p != eof )
			goto _out392;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 105: {
					goto _st393;
				}
			}
			goto _st388;
		}
		_st393:
		if ( p == eof )
			goto _out393;
		p+= 1;
		st_case_393:
		if ( p == pe && p != eof )
			goto _out393;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 110: {
					goto _st394;
				}
			}
			goto _st388;
		}
		_st394:
		if ( p == eof )
			goto _out394;
		p+= 1;
		st_case_394:
		if ( p == pe && p != eof )
			goto _out394;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 123: {
					goto _st395;
				}
			}
			goto _st388;
		}
		_st395:
		if ( p == eof )
			goto _out395;
		p+= 1;
		st_case_395:
		if ( p == pe && p != eof )
			goto _out395;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 102: {
					goto _st396;
				}
			}
			goto _st388;
		}
		_st396:
		if ( p == eof )
			goto _out396;
		p+= 1;
		st_case_396:
		if ( p == pe && p != eof )
			goto _out396;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 105: {
					goto _st397;
				}
			}
			goto _st388;
		}
		_st397:
		if ( p == eof )
			goto _out397;
		p+= 1;
		st_case_397:
		if ( p == pe && p != eof )
			goto _out397;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 103: {
					goto _st398;
				}
			}
			goto _st388;
		}
		_st398:
		if ( p == eof )
			goto _out398;
		p+= 1;
		st_case_398:
		if ( p == pe && p != eof )
			goto _out398;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 117: {
					goto _st399;
				}
			}
			goto _st388;
		}
		_st399:
		if ( p == eof )
			goto _out399;
		p+= 1;
		st_case_399:
		if ( p == pe && p != eof )
			goto _out399;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 114: {
					goto _st400;
				}
			}
			goto _st388;
		}
		_st400:
		if ( p == eof )
			goto _out400;
		p+= 1;
		st_case_400:
		if ( p == pe && p != eof )
			goto _out400;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 101: {
					goto _st401;
				}
			}
			goto _st388;
		}
		_st401:
		if ( p == eof )
			goto _out401;
		p+= 1;
		st_case_401:
		if ( p == pe && p != eof )
			goto _out401;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 125: {
					goto _ctr443;
				}
			}
			goto _st388;
		}
		_st402:
		if ( p == eof )
			goto _out402;
		p+= 1;
		st_case_402:
		if ( p == pe && p != eof )
			goto _out402;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 110: {
					goto _st403;
				}
			}
			goto _st388;
		}
		_st403:
		if ( p == eof )
			goto _out403;
		p+= 1;
		st_case_403:
		if ( p == pe && p != eof )
			goto _out403;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 100: {
					goto _st404;
				}
			}
			goto _st388;
		}
		_st404:
		if ( p == eof )
			goto _out404;
		p+= 1;
		st_case_404:
		if ( p == pe && p != eof )
			goto _out404;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 123: {
					goto _st405;
				}
			}
			goto _st388;
		}
		_st405:
		if ( p == eof )
			goto _out405;
		p+= 1;
		st_case_405:
		if ( p == pe && p != eof )
			goto _out405;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 102: {
					goto _st406;
				}
			}
			goto _st388;
		}
		_st406:
		if ( p == eof )
			goto _out406;
		p+= 1;
		st_case_406:
		if ( p == pe && p != eof )
			goto _out406;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 105: {
					goto _st407;
				}
			}
			goto _st388;
		}
		_st407:
		if ( p == eof )
			goto _out407;
		p+= 1;
		st_case_407:
		if ( p == pe && p != eof )
			goto _out407;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 103: {
					goto _st408;
				}
			}
			goto _st388;
		}
		_st408:
		if ( p == eof )
			goto _out408;
		p+= 1;
		st_case_408:
		if ( p == pe && p != eof )
			goto _out408;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 117: {
					goto _st409;
				}
			}
			goto _st388;
		}
		_st409:
		if ( p == eof )
			goto _out409;
		p+= 1;
		st_case_409:
		if ( p == pe && p != eof )
			goto _out409;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 114: {
					goto _st410;
				}
			}
			goto _st388;
		}
		_st410:
		if ( p == eof )
			goto _out410;
		p+= 1;
		st_case_410:
		if ( p == pe && p != eof )
			goto _out410;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 101: {
					goto _st411;
				}
			}
			goto _st388;
		}
		_st411:
		if ( p == eof )
			goto _out411;
		p+= 1;
		st_case_411:
		if ( p == pe && p != eof )
			goto _out411;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st389;
				}
				case 125: {
					goto _ctr453;
				}
			}
			goto _st388;
		}
		_ctr453:
			{
#line 283 "latex.rl"
			n--; }
		
#line 9275 "scanner.c"

		goto _st412;
		_st412:
		if ( p == eof )
			goto _out412;
		p+= 1;
		st_case_412:
		if ( p == pe && p != eof )
			goto _out412;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				_ck = 0;
				if ( (!n)
#line 9290 "scanner.c"
 )
				_ck += 1;
				if ( _ck > 0 ) {
					goto _ctr455;
				} else {
					goto _st389;
				}
				
			}
			_ck = 0;
			if ( (!n)
#line 9301 "scanner.c"
 )
			_ck += 1;
			if ( _ck > 0 ) {
				goto _ctr455;
			} else {
				goto _st388;
			}
			
		}
		_st413:
		if ( p == eof )
			goto _out413;
		p+= 1;
		st_case_413:
		if ( p == pe && p != eof )
			goto _out413;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 97: {
					goto _st414;
				}
				case 104: {
					goto _st441;
				}
			}
			goto _ctr0;
		}
		_st414:
		if ( p == eof )
			goto _out414;
		p+= 1;
		st_case_414:
		if ( p == pe && p != eof )
			goto _out414;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 98 ) {
				goto _st415;
			}
			goto _ctr0;
		}
		_st415:
		if ( p == eof )
			goto _out415;
		p+= 1;
		st_case_415:
		if ( p == pe && p != eof )
			goto _out415;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 108 ) {
				goto _st416;
			}
			goto _ctr0;
		}
		_st416:
		if ( p == eof )
			goto _out416;
		p+= 1;
		st_case_416:
		if ( p == pe && p != eof )
			goto _out416;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st417;
			}
			goto _ctr0;
		}
		_st417:
		if ( p == eof )
			goto _out417;
		p+= 1;
		st_case_417:
		if ( p == pe && p != eof )
			goto _out417;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 125 ) {
				goto _ctr462;
			}
			goto _ctr0;
		}
		_ctr462:
			{
#line 681 "latex.rl"
			n=1;}
		
#line 9395 "scanner.c"

		goto _st418;
		_ctr477:
			{
#line 678 "latex.rl"
			n++; }
		
#line 9402 "scanner.c"

		goto _st418;
		_st418:
		if ( p == eof )
			goto _out418;
		p+= 1;
		st_case_418:
		if ( p == pe && p != eof )
			goto _out418;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				goto _st419;
			}
			goto _st418;
		}
		_st419:
		if ( p == eof )
			goto _out419;
		p+= 1;
		st_case_419:
		if ( p == pe && p != eof )
			goto _out419;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 98: {
					goto _st420;
				}
				case 101: {
					goto _st431;
				}
			}
			goto _st418;
		}
		_st420:
		if ( p == eof )
			goto _out420;
		p+= 1;
		st_case_420:
		if ( p == pe && p != eof )
			goto _out420;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 101: {
					goto _st421;
				}
			}
			goto _st418;
		}
		_st421:
		if ( p == eof )
			goto _out421;
		p+= 1;
		st_case_421:
		if ( p == pe && p != eof )
			goto _out421;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 103: {
					goto _st422;
				}
			}
			goto _st418;
		}
		_st422:
		if ( p == eof )
			goto _out422;
		p+= 1;
		st_case_422:
		if ( p == pe && p != eof )
			goto _out422;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 105: {
					goto _st423;
				}
			}
			goto _st418;
		}
		_st423:
		if ( p == eof )
			goto _out423;
		p+= 1;
		st_case_423:
		if ( p == pe && p != eof )
			goto _out423;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 110: {
					goto _st424;
				}
			}
			goto _st418;
		}
		_st424:
		if ( p == eof )
			goto _out424;
		p+= 1;
		st_case_424:
		if ( p == pe && p != eof )
			goto _out424;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 123: {
					goto _st425;
				}
			}
			goto _st418;
		}
		_st425:
		if ( p == eof )
			goto _out425;
		p+= 1;
		st_case_425:
		if ( p == pe && p != eof )
			goto _out425;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 116: {
					goto _st426;
				}
			}
			goto _st418;
		}
		_st426:
		if ( p == eof )
			goto _out426;
		p+= 1;
		st_case_426:
		if ( p == pe && p != eof )
			goto _out426;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 97: {
					goto _st427;
				}
			}
			goto _st418;
		}
		_st427:
		if ( p == eof )
			goto _out427;
		p+= 1;
		st_case_427:
		if ( p == pe && p != eof )
			goto _out427;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 98: {
					goto _st428;
				}
			}
			goto _st418;
		}
		_st428:
		if ( p == eof )
			goto _out428;
		p+= 1;
		st_case_428:
		if ( p == pe && p != eof )
			goto _out428;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 108: {
					goto _st429;
				}
			}
			goto _st418;
		}
		_st429:
		if ( p == eof )
			goto _out429;
		p+= 1;
		st_case_429:
		if ( p == pe && p != eof )
			goto _out429;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 101: {
					goto _st430;
				}
			}
			goto _st418;
		}
		_st430:
		if ( p == eof )
			goto _out430;
		p+= 1;
		st_case_430:
		if ( p == pe && p != eof )
			goto _out430;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 125: {
					goto _ctr477;
				}
			}
			goto _st418;
		}
		_st431:
		if ( p == eof )
			goto _out431;
		p+= 1;
		st_case_431:
		if ( p == pe && p != eof )
			goto _out431;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 110: {
					goto _st432;
				}
			}
			goto _st418;
		}
		_st432:
		if ( p == eof )
			goto _out432;
		p+= 1;
		st_case_432:
		if ( p == pe && p != eof )
			goto _out432;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 100: {
					goto _st433;
				}
			}
			goto _st418;
		}
		_st433:
		if ( p == eof )
			goto _out433;
		p+= 1;
		st_case_433:
		if ( p == pe && p != eof )
			goto _out433;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 123: {
					goto _st434;
				}
			}
			goto _st418;
		}
		_st434:
		if ( p == eof )
			goto _out434;
		p+= 1;
		st_case_434:
		if ( p == pe && p != eof )
			goto _out434;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 116: {
					goto _st435;
				}
			}
			goto _st418;
		}
		_st435:
		if ( p == eof )
			goto _out435;
		p+= 1;
		st_case_435:
		if ( p == pe && p != eof )
			goto _out435;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 97: {
					goto _st436;
				}
			}
			goto _st418;
		}
		_st436:
		if ( p == eof )
			goto _out436;
		p+= 1;
		st_case_436:
		if ( p == pe && p != eof )
			goto _out436;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 98: {
					goto _st437;
				}
			}
			goto _st418;
		}
		_st437:
		if ( p == eof )
			goto _out437;
		p+= 1;
		st_case_437:
		if ( p == pe && p != eof )
			goto _out437;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 108: {
					goto _st438;
				}
			}
			goto _st418;
		}
		_st438:
		if ( p == eof )
			goto _out438;
		p+= 1;
		st_case_438:
		if ( p == pe && p != eof )
			goto _out438;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 101: {
					goto _st439;
				}
			}
			goto _st418;
		}
		_st439:
		if ( p == eof )
			goto _out439;
		p+= 1;
		st_case_439:
		if ( p == pe && p != eof )
			goto _out439;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st419;
				}
				case 125: {
					goto _ctr486;
				}
			}
			goto _st418;
		}
		_ctr486:
			{
#line 679 "latex.rl"
			n--; }
		
#line 9847 "scanner.c"

		goto _st440;
		_st440:
		if ( p == eof )
			goto _out440;
		p+= 1;
		st_case_440:
		if ( p == pe && p != eof )
			goto _out440;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				_ck = 0;
				if ( (!n)
#line 9862 "scanner.c"
 )
				_ck += 1;
				if ( _ck > 0 ) {
					goto _ctr488;
				} else {
					goto _st419;
				}
				
			}
			_ck = 0;
			if ( (!n)
#line 9873 "scanner.c"
 )
			_ck += 1;
			if ( _ck > 0 ) {
				goto _ctr488;
			} else {
				goto _st418;
			}
			
		}
		_st441:
		if ( p == eof )
			goto _out441;
		p+= 1;
		st_case_441:
		if ( p == pe && p != eof )
			goto _out441;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st442;
			}
			goto _ctr0;
		}
		_st442:
		if ( p == eof )
			goto _out442;
		p+= 1;
		st_case_442:
		if ( p == pe && p != eof )
			goto _out442;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 98 ) {
				goto _st443;
			}
			goto _ctr0;
		}
		_st443:
		if ( p == eof )
			goto _out443;
		p+= 1;
		st_case_443:
		if ( p == pe && p != eof )
			goto _out443;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st444;
			}
			goto _ctr0;
		}
		_st444:
		if ( p == eof )
			goto _out444;
		p+= 1;
		st_case_444:
		if ( p == pe && p != eof )
			goto _out444;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 98 ) {
				goto _st445;
			}
			goto _ctr0;
		}
		_st445:
		if ( p == eof )
			goto _out445;
		p+= 1;
		st_case_445:
		if ( p == pe && p != eof )
			goto _out445;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 108 ) {
				goto _st446;
			}
			goto _ctr0;
		}
		_st446:
		if ( p == eof )
			goto _out446;
		p+= 1;
		st_case_446:
		if ( p == pe && p != eof )
			goto _out446;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st447;
			}
			goto _ctr0;
		}
		_st447:
		if ( p == eof )
			goto _out447;
		p+= 1;
		st_case_447:
		if ( p == pe && p != eof )
			goto _out447;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 111 ) {
				goto _st448;
			}
			goto _ctr0;
		}
		_st448:
		if ( p == eof )
			goto _out448;
		p+= 1;
		st_case_448:
		if ( p == pe && p != eof )
			goto _out448;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 103 ) {
				goto _st449;
			}
			goto _ctr0;
		}
		_st449:
		if ( p == eof )
			goto _out449;
		p+= 1;
		st_case_449:
		if ( p == pe && p != eof )
			goto _out449;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 114 ) {
				goto _st450;
			}
			goto _ctr0;
		}
		_st450:
		if ( p == eof )
			goto _out450;
		p+= 1;
		st_case_450:
		if ( p == pe && p != eof )
			goto _out450;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st451;
			}
			goto _ctr0;
		}
		_st451:
		if ( p == eof )
			goto _out451;
		p+= 1;
		st_case_451:
		if ( p == pe && p != eof )
			goto _out451;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 112 ) {
				goto _st452;
			}
			goto _ctr0;
		}
		_st452:
		if ( p == eof )
			goto _out452;
		p+= 1;
		st_case_452:
		if ( p == pe && p != eof )
			goto _out452;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 104 ) {
				goto _st453;
			}
			goto _ctr0;
		}
		_st453:
		if ( p == eof )
			goto _out453;
		p+= 1;
		st_case_453:
		if ( p == pe && p != eof )
			goto _out453;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 121 ) {
				goto _st454;
			}
			goto _ctr0;
		}
		_st454:
		if ( p == eof )
			goto _out454;
		p+= 1;
		st_case_454:
		if ( p == pe && p != eof )
			goto _out454;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 125 ) {
				goto _ctr503;
			}
			goto _ctr0;
		}
		_ctr503:
			{
#line 709 "latex.rl"
			n=1;}
		
#line 10097 "scanner.c"

		goto _st455;
		_ctr528:
			{
#line 706 "latex.rl"
			n++; }
		
#line 10104 "scanner.c"

		goto _st455;
		_st455:
		if ( p == eof )
			goto _out455;
		p+= 1;
		st_case_455:
		if ( p == pe && p != eof )
			goto _out455;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				goto _st456;
			}
			goto _st455;
		}
		_st456:
		if ( p == eof )
			goto _out456;
		p+= 1;
		st_case_456:
		if ( p == pe && p != eof )
			goto _out456;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 98: {
					goto _st457;
				}
				case 101: {
					goto _st478;
				}
			}
			goto _st455;
		}
		_st457:
		if ( p == eof )
			goto _out457;
		p+= 1;
		st_case_457:
		if ( p == pe && p != eof )
			goto _out457;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 101: {
					goto _st458;
				}
			}
			goto _st455;
		}
		_st458:
		if ( p == eof )
			goto _out458;
		p+= 1;
		st_case_458:
		if ( p == pe && p != eof )
			goto _out458;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 103: {
					goto _st459;
				}
			}
			goto _st455;
		}
		_st459:
		if ( p == eof )
			goto _out459;
		p+= 1;
		st_case_459:
		if ( p == pe && p != eof )
			goto _out459;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 105: {
					goto _st460;
				}
			}
			goto _st455;
		}
		_st460:
		if ( p == eof )
			goto _out460;
		p+= 1;
		st_case_460:
		if ( p == pe && p != eof )
			goto _out460;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 110: {
					goto _st461;
				}
			}
			goto _st455;
		}
		_st461:
		if ( p == eof )
			goto _out461;
		p+= 1;
		st_case_461:
		if ( p == pe && p != eof )
			goto _out461;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 123: {
					goto _st462;
				}
			}
			goto _st455;
		}
		_st462:
		if ( p == eof )
			goto _out462;
		p+= 1;
		st_case_462:
		if ( p == pe && p != eof )
			goto _out462;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 116: {
					goto _st463;
				}
			}
			goto _st455;
		}
		_st463:
		if ( p == eof )
			goto _out463;
		p+= 1;
		st_case_463:
		if ( p == pe && p != eof )
			goto _out463;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 104: {
					goto _st464;
				}
			}
			goto _st455;
		}
		_st464:
		if ( p == eof )
			goto _out464;
		p+= 1;
		st_case_464:
		if ( p == pe && p != eof )
			goto _out464;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 101: {
					goto _st465;
				}
			}
			goto _st455;
		}
		_st465:
		if ( p == eof )
			goto _out465;
		p+= 1;
		st_case_465:
		if ( p == pe && p != eof )
			goto _out465;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 98: {
					goto _st466;
				}
			}
			goto _st455;
		}
		_st466:
		if ( p == eof )
			goto _out466;
		p+= 1;
		st_case_466:
		if ( p == pe && p != eof )
			goto _out466;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 105: {
					goto _st467;
				}
			}
			goto _st455;
		}
		_st467:
		if ( p == eof )
			goto _out467;
		p+= 1;
		st_case_467:
		if ( p == pe && p != eof )
			goto _out467;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 98: {
					goto _st468;
				}
			}
			goto _st455;
		}
		_st468:
		if ( p == eof )
			goto _out468;
		p+= 1;
		st_case_468:
		if ( p == pe && p != eof )
			goto _out468;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 108: {
					goto _st469;
				}
			}
			goto _st455;
		}
		_st469:
		if ( p == eof )
			goto _out469;
		p+= 1;
		st_case_469:
		if ( p == pe && p != eof )
			goto _out469;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 105: {
					goto _st470;
				}
			}
			goto _st455;
		}
		_st470:
		if ( p == eof )
			goto _out470;
		p+= 1;
		st_case_470:
		if ( p == pe && p != eof )
			goto _out470;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 111: {
					goto _st471;
				}
			}
			goto _st455;
		}
		_st471:
		if ( p == eof )
			goto _out471;
		p+= 1;
		st_case_471:
		if ( p == pe && p != eof )
			goto _out471;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 103: {
					goto _st472;
				}
			}
			goto _st455;
		}
		_st472:
		if ( p == eof )
			goto _out472;
		p+= 1;
		st_case_472:
		if ( p == pe && p != eof )
			goto _out472;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 114: {
					goto _st473;
				}
			}
			goto _st455;
		}
		_st473:
		if ( p == eof )
			goto _out473;
		p+= 1;
		st_case_473:
		if ( p == pe && p != eof )
			goto _out473;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 97: {
					goto _st474;
				}
			}
			goto _st455;
		}
		_st474:
		if ( p == eof )
			goto _out474;
		p+= 1;
		st_case_474:
		if ( p == pe && p != eof )
			goto _out474;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 112: {
					goto _st475;
				}
			}
			goto _st455;
		}
		_st475:
		if ( p == eof )
			goto _out475;
		p+= 1;
		st_case_475:
		if ( p == pe && p != eof )
			goto _out475;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 104: {
					goto _st476;
				}
			}
			goto _st455;
		}
		_st476:
		if ( p == eof )
			goto _out476;
		p+= 1;
		st_case_476:
		if ( p == pe && p != eof )
			goto _out476;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 121: {
					goto _st477;
				}
			}
			goto _st455;
		}
		_st477:
		if ( p == eof )
			goto _out477;
		p+= 1;
		st_case_477:
		if ( p == pe && p != eof )
			goto _out477;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 125: {
					goto _ctr528;
				}
			}
			goto _st455;
		}
		_st478:
		if ( p == eof )
			goto _out478;
		p+= 1;
		st_case_478:
		if ( p == pe && p != eof )
			goto _out478;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 110: {
					goto _st479;
				}
			}
			goto _st455;
		}
		_st479:
		if ( p == eof )
			goto _out479;
		p+= 1;
		st_case_479:
		if ( p == pe && p != eof )
			goto _out479;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 100: {
					goto _st480;
				}
			}
			goto _st455;
		}
		_st480:
		if ( p == eof )
			goto _out480;
		p+= 1;
		st_case_480:
		if ( p == pe && p != eof )
			goto _out480;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 123: {
					goto _st481;
				}
			}
			goto _st455;
		}
		_st481:
		if ( p == eof )
			goto _out481;
		p+= 1;
		st_case_481:
		if ( p == pe && p != eof )
			goto _out481;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 116: {
					goto _st482;
				}
			}
			goto _st455;
		}
		_st482:
		if ( p == eof )
			goto _out482;
		p+= 1;
		st_case_482:
		if ( p == pe && p != eof )
			goto _out482;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 104: {
					goto _st483;
				}
			}
			goto _st455;
		}
		_st483:
		if ( p == eof )
			goto _out483;
		p+= 1;
		st_case_483:
		if ( p == pe && p != eof )
			goto _out483;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 101: {
					goto _st484;
				}
			}
			goto _st455;
		}
		_st484:
		if ( p == eof )
			goto _out484;
		p+= 1;
		st_case_484:
		if ( p == pe && p != eof )
			goto _out484;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 98: {
					goto _st485;
				}
			}
			goto _st455;
		}
		_st485:
		if ( p == eof )
			goto _out485;
		p+= 1;
		st_case_485:
		if ( p == pe && p != eof )
			goto _out485;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 105: {
					goto _st486;
				}
			}
			goto _st455;
		}
		_st486:
		if ( p == eof )
			goto _out486;
		p+= 1;
		st_case_486:
		if ( p == pe && p != eof )
			goto _out486;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 98: {
					goto _st487;
				}
			}
			goto _st455;
		}
		_st487:
		if ( p == eof )
			goto _out487;
		p+= 1;
		st_case_487:
		if ( p == pe && p != eof )
			goto _out487;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 108: {
					goto _st488;
				}
			}
			goto _st455;
		}
		_st488:
		if ( p == eof )
			goto _out488;
		p+= 1;
		st_case_488:
		if ( p == pe && p != eof )
			goto _out488;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 105: {
					goto _st489;
				}
			}
			goto _st455;
		}
		_st489:
		if ( p == eof )
			goto _out489;
		p+= 1;
		st_case_489:
		if ( p == pe && p != eof )
			goto _out489;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 111: {
					goto _st490;
				}
			}
			goto _st455;
		}
		_st490:
		if ( p == eof )
			goto _out490;
		p+= 1;
		st_case_490:
		if ( p == pe && p != eof )
			goto _out490;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 103: {
					goto _st491;
				}
			}
			goto _st455;
		}
		_st491:
		if ( p == eof )
			goto _out491;
		p+= 1;
		st_case_491:
		if ( p == pe && p != eof )
			goto _out491;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 114: {
					goto _st492;
				}
			}
			goto _st455;
		}
		_st492:
		if ( p == eof )
			goto _out492;
		p+= 1;
		st_case_492:
		if ( p == pe && p != eof )
			goto _out492;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 97: {
					goto _st493;
				}
			}
			goto _st455;
		}
		_st493:
		if ( p == eof )
			goto _out493;
		p+= 1;
		st_case_493:
		if ( p == pe && p != eof )
			goto _out493;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 112: {
					goto _st494;
				}
			}
			goto _st455;
		}
		_st494:
		if ( p == eof )
			goto _out494;
		p+= 1;
		st_case_494:
		if ( p == pe && p != eof )
			goto _out494;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 104: {
					goto _st495;
				}
			}
			goto _st455;
		}
		_st495:
		if ( p == eof )
			goto _out495;
		p+= 1;
		st_case_495:
		if ( p == pe && p != eof )
			goto _out495;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 121: {
					goto _st496;
				}
			}
			goto _st455;
		}
		_st496:
		if ( p == eof )
			goto _out496;
		p+= 1;
		st_case_496:
		if ( p == pe && p != eof )
			goto _out496;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st456;
				}
				case 125: {
					goto _ctr547;
				}
			}
			goto _st455;
		}
		_ctr547:
			{
#line 707 "latex.rl"
			n--; }
		
#line 10949 "scanner.c"

		goto _st497;
		_st497:
		if ( p == eof )
			goto _out497;
		p+= 1;
		st_case_497:
		if ( p == pe && p != eof )
			goto _out497;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				_ck = 0;
				if ( (!n)
#line 10964 "scanner.c"
 )
				_ck += 1;
				if ( _ck > 0 ) {
					goto _ctr549;
				} else {
					goto _st456;
				}
				
			}
			_ck = 0;
			if ( (!n)
#line 10975 "scanner.c"
 )
			_ck += 1;
			if ( _ck > 0 ) {
				goto _ctr549;
			} else {
				goto _st455;
			}
			
		}
		_st498:
		if ( p == eof )
			goto _out498;
		p+= 1;
		st_case_498:
		if ( p == pe && p != eof )
			goto _out498;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st499;
			}
			goto _ctr0;
		}
		_st499:
		if ( p == eof )
			goto _out499;
		p+= 1;
		st_case_499:
		if ( p == pe && p != eof )
			goto _out499;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 114 ) {
				goto _st500;
			}
			goto _ctr0;
		}
		_st500:
		if ( p == eof )
			goto _out500;
		p+= 1;
		st_case_500:
		if ( p == pe && p != eof )
			goto _out500;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 98 ) {
				goto _st501;
			}
			goto _ctr0;
		}
		_st501:
		if ( p == eof )
			goto _out501;
		p+= 1;
		st_case_501:
		if ( p == pe && p != eof )
			goto _out501;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st502;
			}
			goto _ctr0;
		}
		_st502:
		if ( p == eof )
			goto _out502;
		p+= 1;
		st_case_502:
		if ( p == pe && p != eof )
			goto _out502;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st503;
			}
			goto _ctr0;
		}
		_st503:
		if ( p == eof )
			goto _out503;
		p+= 1;
		st_case_503:
		if ( p == pe && p != eof )
			goto _out503;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st504;
			}
			goto _ctr0;
		}
		_st504:
		if ( p == eof )
			goto _out504;
		p+= 1;
		st_case_504:
		if ( p == pe && p != eof )
			goto _out504;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 109 ) {
				goto _st505;
			}
			goto _ctr0;
		}
		_st505:
		if ( p == eof )
			goto _out505;
		p+= 1;
		st_case_505:
		if ( p == pe && p != eof )
			goto _out505;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 125 ) {
				goto _ctr558;
			}
			goto _ctr0;
		}
		_ctr558:
			{
#line 760 "latex.rl"
			n=1;}
		
#line 11109 "scanner.c"

		goto _st506;
		_ctr576:
			{
#line 757 "latex.rl"
			n++; }
		
#line 11116 "scanner.c"

		goto _st506;
		_st506:
		if ( p == eof )
			goto _out506;
		p+= 1;
		st_case_506:
		if ( p == pe && p != eof )
			goto _out506;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				goto _st507;
			}
			goto _st506;
		}
		_st507:
		if ( p == eof )
			goto _out507;
		p+= 1;
		st_case_507:
		if ( p == pe && p != eof )
			goto _out507;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 98: {
					goto _st508;
				}
				case 101: {
					goto _st522;
				}
			}
			goto _st506;
		}
		_st508:
		if ( p == eof )
			goto _out508;
		p+= 1;
		st_case_508:
		if ( p == pe && p != eof )
			goto _out508;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 101: {
					goto _st509;
				}
			}
			goto _st506;
		}
		_st509:
		if ( p == eof )
			goto _out509;
		p+= 1;
		st_case_509:
		if ( p == pe && p != eof )
			goto _out509;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 103: {
					goto _st510;
				}
			}
			goto _st506;
		}
		_st510:
		if ( p == eof )
			goto _out510;
		p+= 1;
		st_case_510:
		if ( p == pe && p != eof )
			goto _out510;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 105: {
					goto _st511;
				}
			}
			goto _st506;
		}
		_st511:
		if ( p == eof )
			goto _out511;
		p+= 1;
		st_case_511:
		if ( p == pe && p != eof )
			goto _out511;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 110: {
					goto _st512;
				}
			}
			goto _st506;
		}
		_st512:
		if ( p == eof )
			goto _out512;
		p+= 1;
		st_case_512:
		if ( p == pe && p != eof )
			goto _out512;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 123: {
					goto _st513;
				}
			}
			goto _st506;
		}
		_st513:
		if ( p == eof )
			goto _out513;
		p+= 1;
		st_case_513:
		if ( p == pe && p != eof )
			goto _out513;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 118: {
					goto _st514;
				}
			}
			goto _st506;
		}
		_st514:
		if ( p == eof )
			goto _out514;
		p+= 1;
		st_case_514:
		if ( p == pe && p != eof )
			goto _out514;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 101: {
					goto _st515;
				}
			}
			goto _st506;
		}
		_st515:
		if ( p == eof )
			goto _out515;
		p+= 1;
		st_case_515:
		if ( p == pe && p != eof )
			goto _out515;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 114: {
					goto _st516;
				}
			}
			goto _st506;
		}
		_st516:
		if ( p == eof )
			goto _out516;
		p+= 1;
		st_case_516:
		if ( p == pe && p != eof )
			goto _out516;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 98: {
					goto _st517;
				}
			}
			goto _st506;
		}
		_st517:
		if ( p == eof )
			goto _out517;
		p+= 1;
		st_case_517:
		if ( p == pe && p != eof )
			goto _out517;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 97: {
					goto _st518;
				}
			}
			goto _st506;
		}
		_st518:
		if ( p == eof )
			goto _out518;
		p+= 1;
		st_case_518:
		if ( p == pe && p != eof )
			goto _out518;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 116: {
					goto _st519;
				}
			}
			goto _st506;
		}
		_st519:
		if ( p == eof )
			goto _out519;
		p+= 1;
		st_case_519:
		if ( p == pe && p != eof )
			goto _out519;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 105: {
					goto _st520;
				}
			}
			goto _st506;
		}
		_st520:
		if ( p == eof )
			goto _out520;
		p+= 1;
		st_case_520:
		if ( p == pe && p != eof )
			goto _out520;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 109: {
					goto _st521;
				}
			}
			goto _st506;
		}
		_st521:
		if ( p == eof )
			goto _out521;
		p+= 1;
		st_case_521:
		if ( p == pe && p != eof )
			goto _out521;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 125: {
					goto _ctr576;
				}
			}
			goto _st506;
		}
		_st522:
		if ( p == eof )
			goto _out522;
		p+= 1;
		st_case_522:
		if ( p == pe && p != eof )
			goto _out522;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 110: {
					goto _st523;
				}
			}
			goto _st506;
		}
		_st523:
		if ( p == eof )
			goto _out523;
		p+= 1;
		st_case_523:
		if ( p == pe && p != eof )
			goto _out523;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 100: {
					goto _st524;
				}
			}
			goto _st506;
		}
		_st524:
		if ( p == eof )
			goto _out524;
		p+= 1;
		st_case_524:
		if ( p == pe && p != eof )
			goto _out524;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 123: {
					goto _st525;
				}
			}
			goto _st506;
		}
		_st525:
		if ( p == eof )
			goto _out525;
		p+= 1;
		st_case_525:
		if ( p == pe && p != eof )
			goto _out525;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 118: {
					goto _st526;
				}
			}
			goto _st506;
		}
		_st526:
		if ( p == eof )
			goto _out526;
		p+= 1;
		st_case_526:
		if ( p == pe && p != eof )
			goto _out526;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 101: {
					goto _st527;
				}
			}
			goto _st506;
		}
		_st527:
		if ( p == eof )
			goto _out527;
		p+= 1;
		st_case_527:
		if ( p == pe && p != eof )
			goto _out527;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 114: {
					goto _st528;
				}
			}
			goto _st506;
		}
		_st528:
		if ( p == eof )
			goto _out528;
		p+= 1;
		st_case_528:
		if ( p == pe && p != eof )
			goto _out528;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 98: {
					goto _st529;
				}
			}
			goto _st506;
		}
		_st529:
		if ( p == eof )
			goto _out529;
		p+= 1;
		st_case_529:
		if ( p == pe && p != eof )
			goto _out529;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 97: {
					goto _st530;
				}
			}
			goto _st506;
		}
		_st530:
		if ( p == eof )
			goto _out530;
		p+= 1;
		st_case_530:
		if ( p == pe && p != eof )
			goto _out530;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 116: {
					goto _st531;
				}
			}
			goto _st506;
		}
		_st531:
		if ( p == eof )
			goto _out531;
		p+= 1;
		st_case_531:
		if ( p == pe && p != eof )
			goto _out531;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 105: {
					goto _st532;
				}
			}
			goto _st506;
		}
		_st532:
		if ( p == eof )
			goto _out532;
		p+= 1;
		st_case_532:
		if ( p == pe && p != eof )
			goto _out532;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 109: {
					goto _st533;
				}
			}
			goto _st506;
		}
		_st533:
		if ( p == eof )
			goto _out533;
		p+= 1;
		st_case_533:
		if ( p == pe && p != eof )
			goto _out533;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 92: {
					goto _st507;
				}
				case 125: {
					goto _ctr588;
				}
			}
			goto _st506;
		}
		_ctr588:
			{
#line 758 "latex.rl"
			n--; }
		
#line 11681 "scanner.c"

		goto _st534;
		_st534:
		if ( p == eof )
			goto _out534;
		p+= 1;
		st_case_534:
		if ( p == pe && p != eof )
			goto _out534;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 92 ) {
				_ck = 0;
				if ( (!n)
#line 11696 "scanner.c"
 )
				_ck += 1;
				if ( _ck > 0 ) {
					goto _ctr590;
				} else {
					goto _st507;
				}
				
			}
			_ck = 0;
			if ( (!n)
#line 11707 "scanner.c"
 )
			_ck += 1;
			if ( _ck > 0 ) {
				goto _ctr590;
			} else {
				goto _st506;
			}
			
		}
		_st535:
		if ( p == eof )
			goto _out535;
		p+= 1;
		st_case_535:
		if ( p == pe && p != eof )
			goto _out535;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 98 ) {
				goto _st536;
			}
			goto _ctr0;
		}
		_st536:
		if ( p == eof )
			goto _out536;
		p+= 1;
		st_case_536:
		if ( p == pe && p != eof )
			goto _out536;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st537;
			}
			goto _ctr0;
		}
		_st537:
		if ( p == eof )
			goto _out537;
		p+= 1;
		st_case_537:
		if ( p == pe && p != eof )
			goto _out537;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st538;
			}
			goto _ctr0;
		}
		_st538:
		if ( p == eof )
			goto _out538;
		p+= 1;
		st_case_538:
		if ( p == pe && p != eof )
			goto _out538;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st539;
			}
			goto _ctr0;
		}
		_st539:
		if ( p == eof )
			goto _out539;
		p+= 1;
		st_case_539:
		if ( p == pe && p != eof )
			goto _out539;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 109 ) {
				goto _st540;
			}
			goto _ctr0;
		}
		_st540:
		if ( p == eof )
			goto _out540;
		p+= 1;
		st_case_540:
		if ( p == pe && p != eof )
			goto _out540;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _ctr597;
			}
			goto _ctr0;
		}
		_ctr600:
			{
#line 791 "latex.rl"
			n--; }
		
#line 11811 "scanner.c"

		goto _st541;
		_ctr597:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 11818 "scanner.c"

		goto _st541;
		_ctr599:
			{
#line 790 "latex.rl"
			n++; }
		
#line 11825 "scanner.c"

		goto _st541;
		_st541:
		if ( p == eof )
			goto _out541;
		p+= 1;
		st_case_541:
		if ( p == pe && p != eof )
			goto _out541;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr599;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 11844 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _st542;
					} else {
						goto _ctr600;
					}
					
				}
			}
			goto _st541;
		}
		_st542:
		if ( p == eof )
			goto _out542;
		p+= 1;
		st_case_542:
		if ( p == pe && p != eof )
			goto _out542;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 10 ) {
				goto _ctr603;
			}
			goto _st542;
		}
		_st543:
		if ( p == eof )
			goto _out543;
		p+= 1;
		st_case_543:
		if ( p == pe && p != eof )
			goto _out543;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st544;
			}
			goto _ctr0;
		}
		_st544:
		if ( p == eof )
			goto _out544;
		p+= 1;
		st_case_544:
		if ( p == pe && p != eof )
			goto _out544;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st545;
			}
			goto _ctr0;
		}
		_st545:
		if ( p == eof )
			goto _out545;
		p+= 1;
		st_case_545:
		if ( p == pe && p != eof )
			goto _out545;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st546;
			}
			goto _ctr0;
		}
		_st546:
		if ( p == eof )
			goto _out546;
		p+= 1;
		st_case_546:
		if ( p == pe && p != eof )
			goto _out546;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 91: {
					goto _ctr607;
				}
				case 92: {
					goto _st550;
				}
				case 123: {
					goto _ctr609;
				}
			}
			goto _ctr0;
		}
		_ctr612:
			{
#line 796 "latex.rl"
			n--; }
		
#line 11944 "scanner.c"

		goto _st547;
		_ctr607:
			{
#line 798 "latex.rl"
			n=0;}
		
#line 11951 "scanner.c"

		goto _st547;
		_ctr611:
			{
#line 795 "latex.rl"
			n++;}
		
#line 11958 "scanner.c"

		goto _st547;
		_st547:
		if ( p == eof )
			goto _out547;
		p+= 1;
		st_case_547:
		if ( p == pe && p != eof )
			goto _out547;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 91: {
					goto _ctr611;
				}
				case 93: {
					_ck = 0;
					if ( (!n)
#line 11977 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _st548;
					} else {
						goto _ctr612;
					}
					
				}
			}
			goto _st547;
		}
		_st548:
		if ( p == eof )
			goto _out548;
		p+= 1;
		st_case_548:
		if ( p == pe && p != eof )
			goto _out548;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _ctr614;
			}
			goto _ctr0;
		}
		_ctr617:
			{
#line 791 "latex.rl"
			n--; }
		
#line 12009 "scanner.c"

		goto _st549;
		_ctr614:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 12016 "scanner.c"

		goto _st549;
		_ctr616:
			{
#line 790 "latex.rl"
			n++; }
		
#line 12023 "scanner.c"

		goto _st549;
		_st549:
		if ( p == eof )
			goto _out549;
		p+= 1;
		st_case_549:
		if ( p == pe && p != eof )
			goto _out549;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr616;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 12042 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr618;
					} else {
						goto _ctr617;
					}
					
				}
			}
			goto _st549;
		}
		_st550:
		if ( p == eof )
			goto _out550;
		p+= 1;
		st_case_550:
		if ( p == pe && p != eof )
			goto _out550;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 108 ) {
				goto _st551;
			}
			goto _ctr0;
		}
		_st551:
		if ( p == eof )
			goto _out551;
		p+= 1;
		st_case_551:
		if ( p == pe && p != eof )
			goto _out551;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st552;
			}
			goto _ctr0;
		}
		_st552:
		if ( p == eof )
			goto _out552;
		p+= 1;
		st_case_552:
		if ( p == pe && p != eof )
			goto _out552;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 102 ) {
				goto _st553;
			}
			goto _ctr0;
		}
		_st553:
		if ( p == eof )
			goto _out553;
		p+= 1;
		st_case_553:
		if ( p == pe && p != eof )
			goto _out553;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st554;
			}
			goto _ctr0;
		}
		_st554:
		if ( p == eof )
			goto _out554;
		p+= 1;
		st_case_554:
		if ( p == pe && p != eof )
			goto _out554;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 91 ) {
				goto _ctr607;
			}
			goto _ctr0;
		}
		_ctr625:
			{
#line 791 "latex.rl"
			n--; }
		
#line 12134 "scanner.c"

		goto _st555;
		_ctr609:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 12141 "scanner.c"

		goto _st555;
		_ctr624:
			{
#line 790 "latex.rl"
			n++; }
		
#line 12148 "scanner.c"

		goto _st555;
		_st555:
		if ( p == eof )
			goto _out555;
		p+= 1;
		st_case_555:
		if ( p == pe && p != eof )
			goto _out555;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr624;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 12167 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr626;
					} else {
						goto _ctr625;
					}
					
				}
			}
			goto _st555;
		}
		_ctr626:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 12184 "scanner.c"

		goto _st631;
		_st631:
		if ( p == eof )
			goto _out631;
		p+= 1;
		st_case_631:
		if ( p == pe && p != eof )
			goto _out631;
		if ( p == eof ) {
			goto _ctr750;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _ctr751;
			}
			goto _ctr750;
		}
		_ctr630:
			{
#line 791 "latex.rl"
			n--; }
		
#line 12206 "scanner.c"

		goto _st556;
		_ctr751:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 12213 "scanner.c"

		goto _st556;
		_ctr629:
			{
#line 790 "latex.rl"
			n++; }
		
#line 12220 "scanner.c"

		goto _st556;
		_st556:
		if ( p == eof )
			goto _out556;
		p+= 1;
		st_case_556:
		if ( p == pe && p != eof )
			goto _out556;
		if ( p == eof ) {
			goto _ctr627;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr629;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 12239 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr618;
					} else {
						goto _ctr630;
					}
					
				}
			}
			goto _st556;
		}
		_st557:
		if ( p == eof )
			goto _out557;
		p+= 1;
		st_case_557:
		if ( p == pe && p != eof )
			goto _out557;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 102 ) {
				goto _st558;
			}
			goto _ctr0;
		}
		_st558:
		if ( p == eof )
			goto _out558;
		p+= 1;
		st_case_558:
		if ( p == pe && p != eof )
			goto _out558;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 114 ) {
				goto _st559;
			}
			goto _ctr0;
		}
		_st559:
		if ( p == eof )
			goto _out559;
		p+= 1;
		st_case_559:
		if ( p == pe && p != eof )
			goto _out559;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st560;
			}
			goto _ctr0;
		}
		_st560:
		if ( p == eof )
			goto _out560;
		p+= 1;
		st_case_560:
		if ( p == pe && p != eof )
			goto _out560;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 99 ) {
				goto _st561;
			}
			goto _ctr0;
		}
		_st561:
		if ( p == eof )
			goto _out561;
		p+= 1;
		st_case_561:
		if ( p == pe && p != eof )
			goto _out561;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 32: {
					goto _st562;
				}
				case 123: {
					goto _ctr637;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st564;
			}
			goto _ctr0;
		}
		_st562:
		if ( p == eof )
			goto _out562;
		p+= 1;
		st_case_562:
		if ( p == pe && p != eof )
			goto _out562;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st563;
			}
			goto _ctr0;
		}
		_st563:
		if ( p == eof )
			goto _out563;
		p+= 1;
		st_case_563:
		if ( p == pe && p != eof )
			goto _out563;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr639;
			}
			goto _ctr0;
		}
		_st564:
		if ( p == eof )
			goto _out564;
		p+= 1;
		st_case_564:
		if ( p == pe && p != eof )
			goto _out564;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _ctr640;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr639;
			}
			goto _ctr0;
		}
		_ctr643:
			{
#line 791 "latex.rl"
			n--; }
		
#line 12387 "scanner.c"

		goto _st565;
		_ctr640:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 12394 "scanner.c"

		goto _st565;
		_ctr642:
			{
#line 790 "latex.rl"
			n++; }
		
#line 12401 "scanner.c"

		goto _st565;
		_st565:
		if ( p == eof )
			goto _out565;
		p+= 1;
		st_case_565:
		if ( p == pe && p != eof )
			goto _out565;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr642;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 12420 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr644;
					} else {
						goto _ctr643;
					}
					
				}
			}
			goto _st565;
		}
		_ctr647:
			{
#line 791 "latex.rl"
			n--; }
		
#line 12437 "scanner.c"

		goto _st566;
		_ctr637:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 12444 "scanner.c"

		goto _st566;
		_ctr646:
			{
#line 790 "latex.rl"
			n++; }
		
#line 12451 "scanner.c"

		goto _st566;
		_st566:
		if ( p == eof )
			goto _out566;
		p+= 1;
		st_case_566:
		if ( p == pe && p != eof )
			goto _out566;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr646;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 12470 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _st567;
					} else {
						goto _ctr647;
					}
					
				}
			}
			goto _st566;
		}
		_st567:
		if ( p == eof )
			goto _out567;
		p+= 1;
		st_case_567:
		if ( p == pe && p != eof )
			goto _out567;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 10: {
					goto _st568;
				}
				case 123: {
					goto _ctr640;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr639;
			}
			goto _ctr0;
		}
		_st568:
		if ( p == eof )
			goto _out568;
		p+= 1;
		st_case_568:
		if ( p == pe && p != eof )
			goto _out568;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 32 ) {
				goto _st569;
			}
			goto _ctr0;
		}
		_st569:
		if ( p == eof )
			goto _out569;
		p+= 1;
		st_case_569:
		if ( p == pe && p != eof )
			goto _out569;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 32: {
					goto _st569;
				}
				case 123: {
					goto _ctr640;
				}
			}
			goto _ctr0;
		}
		_st570:
		if ( p == eof )
			goto _out570;
		p+= 1;
		st_case_570:
		if ( p == pe && p != eof )
			goto _out570;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 110 ) {
				goto _st571;
			}
			goto _ctr0;
		}
		_st571:
		if ( p == eof )
			goto _out571;
		p+= 1;
		st_case_571:
		if ( p == pe && p != eof )
			goto _out571;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 100 ) {
				goto _st572;
			}
			goto _ctr0;
		}
		_st572:
		if ( p == eof )
			goto _out572;
		p+= 1;
		st_case_572:
		if ( p == pe && p != eof )
			goto _out572;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _st573;
			}
			goto _ctr0;
		}
		_st573:
		if ( p == eof )
			goto _out573;
		p+= 1;
		st_case_573:
		if ( p == pe && p != eof )
			goto _out573;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 100 ) {
				goto _st574;
			}
			goto _ctr0;
		}
		_st574:
		if ( p == eof )
			goto _out574;
		p+= 1;
		st_case_574:
		if ( p == pe && p != eof )
			goto _out574;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 111 ) {
				goto _st575;
			}
			goto _ctr0;
		}
		_st575:
		if ( p == eof )
			goto _out575;
		p+= 1;
		st_case_575:
		if ( p == pe && p != eof )
			goto _out575;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 99 ) {
				goto _st576;
			}
			goto _ctr0;
		}
		_st576:
		if ( p == eof )
			goto _out576;
		p+= 1;
		st_case_576:
		if ( p == pe && p != eof )
			goto _out576;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 117 ) {
				goto _st577;
			}
			goto _ctr0;
		}
		_st577:
		if ( p == eof )
			goto _out577;
		p+= 1;
		st_case_577:
		if ( p == pe && p != eof )
			goto _out577;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 109 ) {
				goto _st578;
			}
			goto _ctr0;
		}
		_st578:
		if ( p == eof )
			goto _out578;
		p+= 1;
		st_case_578:
		if ( p == pe && p != eof )
			goto _out578;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st579;
			}
			goto _ctr0;
		}
		_st579:
		if ( p == eof )
			goto _out579;
		p+= 1;
		st_case_579:
		if ( p == pe && p != eof )
			goto _out579;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 110 ) {
				goto _st580;
			}
			goto _ctr0;
		}
		_st580:
		if ( p == eof )
			goto _out580;
		p+= 1;
		st_case_580:
		if ( p == pe && p != eof )
			goto _out580;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st581;
			}
			goto _ctr0;
		}
		_st581:
		if ( p == eof )
			goto _out581;
		p+= 1;
		st_case_581:
		if ( p == pe && p != eof )
			goto _out581;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 125 ) {
				goto _ctr662;
			}
			goto _ctr0;
		}
		_st582:
		if ( p == eof )
			goto _out582;
		p+= 1;
		st_case_582:
		if ( p == pe && p != eof )
			goto _out582;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st583;
			}
			goto _ctr0;
		}
		_st583:
		if ( p == eof )
			goto _out583;
		p+= 1;
		st_case_583:
		if ( p == pe && p != eof )
			goto _out583;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 102 ) {
				goto _st584;
			}
			goto _ctr0;
		}
		_st584:
		if ( p == eof )
			goto _out584;
		p+= 1;
		st_case_584:
		if ( p == pe && p != eof )
			goto _out584;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st585;
			}
			goto _ctr0;
		}
		_st585:
		if ( p == eof )
			goto _out585;
		p+= 1;
		st_case_585:
		if ( p == pe && p != eof )
			goto _out585;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 40 ) {
				goto _ctr666;
			}
			goto _ctr0;
		}
		_st586:
		if ( p == eof )
			goto _out586;
		p+= 1;
		st_case_586:
		if ( p == pe && p != eof )
			goto _out586;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st587;
			}
			goto _ctr0;
		}
		_st587:
		if ( p == eof )
			goto _out587;
		p+= 1;
		st_case_587:
		if ( p == pe && p != eof )
			goto _out587;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 102 ) {
				goto _st588;
			}
			goto _ctr0;
		}
		_st588:
		if ( p == eof )
			goto _out588;
		p+= 1;
		st_case_588:
		if ( p == pe && p != eof )
			goto _out588;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _ctr669;
			}
			goto _ctr0;
		}
		_ctr672:
			{
#line 791 "latex.rl"
			n--; }
		
#line 12830 "scanner.c"

		goto _st589;
		_ctr669:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 12837 "scanner.c"

		goto _st589;
		_ctr671:
			{
#line 790 "latex.rl"
			n++; }
		
#line 12844 "scanner.c"

		goto _st589;
		_st589:
		if ( p == eof )
			goto _out589;
		p+= 1;
		st_case_589:
		if ( p == pe && p != eof )
			goto _out589;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr671;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 12863 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr673;
					} else {
						goto _ctr672;
					}
					
				}
			}
			goto _st589;
		}
		_st590:
		if ( p == eof )
			goto _out590;
		p+= 1;
		st_case_590:
		if ( p == pe && p != eof )
			goto _out590;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st591;
			}
			goto _ctr0;
		}
		_st591:
		if ( p == eof )
			goto _out591;
		p+= 1;
		st_case_591:
		if ( p == pe && p != eof )
			goto _out591;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 99 ) {
				goto _st592;
			}
			goto _ctr0;
		}
		_st592:
		if ( p == eof )
			goto _out592;
		p+= 1;
		st_case_592:
		if ( p == pe && p != eof )
			goto _out592;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st593;
			}
			goto _ctr0;
		}
		_st593:
		if ( p == eof )
			goto _out593;
		p+= 1;
		st_case_593:
		if ( p == pe && p != eof )
			goto _out593;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 105 ) {
				goto _st594;
			}
			goto _ctr0;
		}
		_st594:
		if ( p == eof )
			goto _out594;
		p+= 1;
		st_case_594:
		if ( p == pe && p != eof )
			goto _out594;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 111 ) {
				goto _st595;
			}
			goto _ctr0;
		}
		_st595:
		if ( p == eof )
			goto _out595;
		p+= 1;
		st_case_595:
		if ( p == pe && p != eof )
			goto _out595;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 110 ) {
				goto _st596;
			}
			goto _ctr0;
		}
		_st596:
		if ( p == eof )
			goto _out596;
		p+= 1;
		st_case_596:
		if ( p == pe && p != eof )
			goto _out596;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _ctr680;
			}
			goto _ctr0;
		}
		_ctr683:
			{
#line 791 "latex.rl"
			n--; }
		
#line 12985 "scanner.c"

		goto _st597;
		_ctr680:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 12992 "scanner.c"

		goto _st597;
		_ctr682:
			{
#line 790 "latex.rl"
			n++; }
		
#line 12999 "scanner.c"

		goto _st597;
		_st597:
		if ( p == eof )
			goto _out597;
		p+= 1;
		st_case_597:
		if ( p == pe && p != eof )
			goto _out597;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr682;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 13018 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr684;
					} else {
						goto _ctr683;
					}
					
				}
			}
			goto _st597;
		}
		_st598:
		if ( p == eof )
			goto _out598;
		p+= 1;
		st_case_598:
		if ( p == pe && p != eof )
			goto _out598;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 102: {
					goto _st558;
				}
				case 105: {
					goto _st599;
				}
			}
			goto _ctr0;
		}
		_st599:
		if ( p == eof )
			goto _out599;
		p+= 1;
		st_case_599:
		if ( p == pe && p != eof )
			goto _out599;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st600;
			}
			goto _ctr0;
		}
		_st600:
		if ( p == eof )
			goto _out600;
		p+= 1;
		st_case_600:
		if ( p == pe && p != eof )
			goto _out600;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 108 ) {
				goto _st601;
			}
			goto _ctr0;
		}
		_st601:
		if ( p == eof )
			goto _out601;
		p+= 1;
		st_case_601:
		if ( p == pe && p != eof )
			goto _out601;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st602;
			}
			goto _ctr0;
		}
		_st602:
		if ( p == eof )
			goto _out602;
		p+= 1;
		st_case_602:
		if ( p == pe && p != eof )
			goto _out602;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _ctr689;
			}
			goto _ctr0;
		}
		_ctr692:
			{
#line 791 "latex.rl"
			n--; }
		
#line 13115 "scanner.c"

		goto _st603;
		_ctr689:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 13122 "scanner.c"

		goto _st603;
		_ctr691:
			{
#line 790 "latex.rl"
			n++; }
		
#line 13129 "scanner.c"

		goto _st603;
		_st603:
		if ( p == eof )
			goto _out603;
		p+= 1;
		st_case_603:
		if ( p == pe && p != eof )
			goto _out603;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr691;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 13148 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr693;
					} else {
						goto _ctr692;
					}
					
				}
			}
			goto _st603;
		}
		_st604:
		if ( p == eof )
			goto _out604;
		p+= 1;
		st_case_604:
		if ( p == pe && p != eof )
			goto _out604;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 115 ) {
				goto _st605;
			}
			goto _ctr0;
		}
		_st605:
		if ( p == eof )
			goto _out605;
		p+= 1;
		st_case_605:
		if ( p == pe && p != eof )
			goto _out605;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st606;
			}
			goto _ctr0;
		}
		_st606:
		if ( p == eof )
			goto _out606;
		p+= 1;
		st_case_606:
		if ( p == pe && p != eof )
			goto _out606;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 112 ) {
				goto _st607;
			}
			goto _ctr0;
		}
		_st607:
		if ( p == eof )
			goto _out607;
		p+= 1;
		st_case_607:
		if ( p == pe && p != eof )
			goto _out607;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st608;
			}
			goto _ctr0;
		}
		_st608:
		if ( p == eof )
			goto _out608;
		p+= 1;
		st_case_608:
		if ( p == pe && p != eof )
			goto _out608;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 99 ) {
				goto _st609;
			}
			goto _ctr0;
		}
		_st609:
		if ( p == eof )
			goto _out609;
		p+= 1;
		st_case_609:
		if ( p == pe && p != eof )
			goto _out609;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 107 ) {
				goto _st610;
			}
			goto _ctr0;
		}
		_st610:
		if ( p == eof )
			goto _out610;
		p+= 1;
		st_case_610:
		if ( p == pe && p != eof )
			goto _out610;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 97 ) {
				goto _st611;
			}
			goto _ctr0;
		}
		_st611:
		if ( p == eof )
			goto _out611;
		p+= 1;
		st_case_611:
		if ( p == pe && p != eof )
			goto _out611;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 103 ) {
				goto _st612;
			}
			goto _ctr0;
		}
		_st612:
		if ( p == eof )
			goto _out612;
		p+= 1;
		st_case_612:
		if ( p == pe && p != eof )
			goto _out612;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st613;
			}
			goto _ctr0;
		}
		_st613:
		if ( p == eof )
			goto _out613;
		p+= 1;
		st_case_613:
		if ( p == pe && p != eof )
			goto _out613;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 91: {
					goto _ctr703;
				}
				case 92: {
					goto _st617;
				}
				case 123: {
					goto _ctr705;
				}
			}
			goto _ctr0;
		}
		_ctr708:
			{
#line 796 "latex.rl"
			n--; }
		
#line 13323 "scanner.c"

		goto _st614;
		_ctr703:
			{
#line 798 "latex.rl"
			n=0;}
		
#line 13330 "scanner.c"

		goto _st614;
		_ctr707:
			{
#line 795 "latex.rl"
			n++;}
		
#line 13337 "scanner.c"

		goto _st614;
		_st614:
		if ( p == eof )
			goto _out614;
		p+= 1;
		st_case_614:
		if ( p == pe && p != eof )
			goto _out614;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 91: {
					goto _ctr707;
				}
				case 93: {
					_ck = 0;
					if ( (!n)
#line 13356 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _st615;
					} else {
						goto _ctr708;
					}
					
				}
			}
			goto _st614;
		}
		_st615:
		if ( p == eof )
			goto _out615;
		p+= 1;
		st_case_615:
		if ( p == pe && p != eof )
			goto _out615;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 123 ) {
				goto _ctr705;
			}
			goto _ctr0;
		}
		_ctr712:
			{
#line 791 "latex.rl"
			n--; }
		
#line 13388 "scanner.c"

		goto _st616;
		_ctr705:
			{
#line 793 "latex.rl"
			n=0;}
		
#line 13395 "scanner.c"

		goto _st616;
		_ctr711:
			{
#line 790 "latex.rl"
			n++; }
		
#line 13402 "scanner.c"

		goto _st616;
		_st616:
		if ( p == eof )
			goto _out616;
		p+= 1;
		st_case_616:
		if ( p == pe && p != eof )
			goto _out616;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr711;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 13421 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr713;
					} else {
						goto _ctr712;
					}
					
				}
			}
			goto _st616;
		}
		_st617:
		if ( p == eof )
			goto _out617;
		p+= 1;
		st_case_617:
		if ( p == pe && p != eof )
			goto _out617;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 108 ) {
				goto _st618;
			}
			goto _ctr0;
		}
		_st618:
		if ( p == eof )
			goto _out618;
		p+= 1;
		st_case_618:
		if ( p == pe && p != eof )
			goto _out618;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 101 ) {
				goto _st619;
			}
			goto _ctr0;
		}
		_st619:
		if ( p == eof )
			goto _out619;
		p+= 1;
		st_case_619:
		if ( p == pe && p != eof )
			goto _out619;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 102 ) {
				goto _st620;
			}
			goto _ctr0;
		}
		_st620:
		if ( p == eof )
			goto _out620;
		p+= 1;
		st_case_620:
		if ( p == pe && p != eof )
			goto _out620;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 116 ) {
				goto _st621;
			}
			goto _ctr0;
		}
		_st621:
		if ( p == eof )
			goto _out621;
		p+= 1;
		st_case_621:
		if ( p == pe && p != eof )
			goto _out621;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			if ( ( (*( p))) == 91 ) {
				goto _ctr703;
			}
			goto _ctr0;
		}
		_ctr731:
			{
#line 1 "NONE"
			{te = p+1;}}
		
#line 13513 "scanner.c"

			{
#line 793 "latex.rl"
			n=0;}
		
#line 13518 "scanner.c"

		goto _st632;
		_st632:
		if ( p == eof )
			goto _out632;
		p+= 1;
		st_case_632:
		if ( p == pe && p != eof )
			goto _out632;
		if ( p == eof ) {
			goto _ctr735;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr719;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 13537 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr721;
					} else {
						goto _ctr720;
					}
					
				}
			}
			goto _st622;
		}
		_ctr720:
			{
#line 791 "latex.rl"
			n--; }
		
#line 13554 "scanner.c"

		goto _st622;
		_ctr719:
			{
#line 790 "latex.rl"
			n++; }
		
#line 13561 "scanner.c"

		goto _st622;
		_st622:
		if ( p == eof )
			goto _out622;
		p+= 1;
		st_case_622:
		if ( p == pe && p != eof )
			goto _out622;
		if ( p == eof ) {
			goto _ctr0;}
		else {
			switch( ( (*( p))) ) {
				case 123: {
					goto _ctr719;
				}
				case 125: {
					_ck = 0;
					if ( (!n)
#line 13580 "scanner.c"
 )
					_ck += 1;
					if ( _ck > 0 ) {
						goto _ctr721;
					} else {
						goto _ctr720;
					}
					
				}
			}
			goto _st622;
		}
		_out623: cs = 623; goto _out; 
		_out624: cs = 624; goto _out; 
		_out625: cs = 625; goto _out; 
		_out626: cs = 626; goto _out; 
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
		_out627: cs = 627; goto _out; 
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
		_out100: cs = 100; goto _out; 
		_out101: cs = 101; goto _out; 
		_out102: cs = 102; goto _out; 
		_out103: cs = 103; goto _out; 
		_out104: cs = 104; goto _out; 
		_out105: cs = 105; goto _out; 
		_out106: cs = 106; goto _out; 
		_out107: cs = 107; goto _out; 
		_out108: cs = 108; goto _out; 
		_out109: cs = 109; goto _out; 
		_out110: cs = 110; goto _out; 
		_out111: cs = 111; goto _out; 
		_out112: cs = 112; goto _out; 
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
		_out136: cs = 136; goto _out; 
		_out137: cs = 137; goto _out; 
		_out138: cs = 138; goto _out; 
		_out139: cs = 139; goto _out; 
		_out140: cs = 140; goto _out; 
		_out141: cs = 141; goto _out; 
		_out142: cs = 142; goto _out; 
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
		_out153: cs = 153; goto _out; 
		_out154: cs = 154; goto _out; 
		_out155: cs = 155; goto _out; 
		_out156: cs = 156; goto _out; 
		_out157: cs = 157; goto _out; 
		_out158: cs = 158; goto _out; 
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
		_out628: cs = 628; goto _out; 
		_out180: cs = 180; goto _out; 
		_out629: cs = 629; goto _out; 
		_out181: cs = 181; goto _out; 
		_out630: cs = 630; goto _out; 
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
		_out210: cs = 210; goto _out; 
		_out211: cs = 211; goto _out; 
		_out212: cs = 212; goto _out; 
		_out213: cs = 213; goto _out; 
		_out214: cs = 214; goto _out; 
		_out215: cs = 215; goto _out; 
		_out216: cs = 216; goto _out; 
		_out217: cs = 217; goto _out; 
		_out218: cs = 218; goto _out; 
		_out219: cs = 219; goto _out; 
		_out220: cs = 220; goto _out; 
		_out221: cs = 221; goto _out; 
		_out222: cs = 222; goto _out; 
		_out223: cs = 223; goto _out; 
		_out224: cs = 224; goto _out; 
		_out225: cs = 225; goto _out; 
		_out226: cs = 226; goto _out; 
		_out227: cs = 227; goto _out; 
		_out228: cs = 228; goto _out; 
		_out229: cs = 229; goto _out; 
		_out230: cs = 230; goto _out; 
		_out231: cs = 231; goto _out; 
		_out232: cs = 232; goto _out; 
		_out233: cs = 233; goto _out; 
		_out234: cs = 234; goto _out; 
		_out235: cs = 235; goto _out; 
		_out236: cs = 236; goto _out; 
		_out237: cs = 237; goto _out; 
		_out238: cs = 238; goto _out; 
		_out239: cs = 239; goto _out; 
		_out240: cs = 240; goto _out; 
		_out241: cs = 241; goto _out; 
		_out242: cs = 242; goto _out; 
		_out243: cs = 243; goto _out; 
		_out244: cs = 244; goto _out; 
		_out245: cs = 245; goto _out; 
		_out246: cs = 246; goto _out; 
		_out247: cs = 247; goto _out; 
		_out248: cs = 248; goto _out; 
		_out249: cs = 249; goto _out; 
		_out250: cs = 250; goto _out; 
		_out251: cs = 251; goto _out; 
		_out252: cs = 252; goto _out; 
		_out253: cs = 253; goto _out; 
		_out254: cs = 254; goto _out; 
		_out255: cs = 255; goto _out; 
		_out256: cs = 256; goto _out; 
		_out257: cs = 257; goto _out; 
		_out258: cs = 258; goto _out; 
		_out259: cs = 259; goto _out; 
		_out260: cs = 260; goto _out; 
		_out261: cs = 261; goto _out; 
		_out262: cs = 262; goto _out; 
		_out263: cs = 263; goto _out; 
		_out264: cs = 264; goto _out; 
		_out265: cs = 265; goto _out; 
		_out266: cs = 266; goto _out; 
		_out267: cs = 267; goto _out; 
		_out268: cs = 268; goto _out; 
		_out269: cs = 269; goto _out; 
		_out270: cs = 270; goto _out; 
		_out271: cs = 271; goto _out; 
		_out272: cs = 272; goto _out; 
		_out273: cs = 273; goto _out; 
		_out274: cs = 274; goto _out; 
		_out275: cs = 275; goto _out; 
		_out276: cs = 276; goto _out; 
		_out277: cs = 277; goto _out; 
		_out278: cs = 278; goto _out; 
		_out279: cs = 279; goto _out; 
		_out280: cs = 280; goto _out; 
		_out281: cs = 281; goto _out; 
		_out282: cs = 282; goto _out; 
		_out283: cs = 283; goto _out; 
		_out284: cs = 284; goto _out; 
		_out285: cs = 285; goto _out; 
		_out286: cs = 286; goto _out; 
		_out287: cs = 287; goto _out; 
		_out288: cs = 288; goto _out; 
		_out289: cs = 289; goto _out; 
		_out290: cs = 290; goto _out; 
		_out291: cs = 291; goto _out; 
		_out292: cs = 292; goto _out; 
		_out293: cs = 293; goto _out; 
		_out294: cs = 294; goto _out; 
		_out295: cs = 295; goto _out; 
		_out296: cs = 296; goto _out; 
		_out297: cs = 297; goto _out; 
		_out298: cs = 298; goto _out; 
		_out299: cs = 299; goto _out; 
		_out300: cs = 300; goto _out; 
		_out301: cs = 301; goto _out; 
		_out302: cs = 302; goto _out; 
		_out303: cs = 303; goto _out; 
		_out304: cs = 304; goto _out; 
		_out305: cs = 305; goto _out; 
		_out306: cs = 306; goto _out; 
		_out307: cs = 307; goto _out; 
		_out308: cs = 308; goto _out; 
		_out309: cs = 309; goto _out; 
		_out310: cs = 310; goto _out; 
		_out311: cs = 311; goto _out; 
		_out312: cs = 312; goto _out; 
		_out313: cs = 313; goto _out; 
		_out314: cs = 314; goto _out; 
		_out315: cs = 315; goto _out; 
		_out316: cs = 316; goto _out; 
		_out317: cs = 317; goto _out; 
		_out318: cs = 318; goto _out; 
		_out319: cs = 319; goto _out; 
		_out320: cs = 320; goto _out; 
		_out321: cs = 321; goto _out; 
		_out322: cs = 322; goto _out; 
		_out323: cs = 323; goto _out; 
		_out324: cs = 324; goto _out; 
		_out325: cs = 325; goto _out; 
		_out326: cs = 326; goto _out; 
		_out327: cs = 327; goto _out; 
		_out328: cs = 328; goto _out; 
		_out329: cs = 329; goto _out; 
		_out330: cs = 330; goto _out; 
		_out331: cs = 331; goto _out; 
		_out332: cs = 332; goto _out; 
		_out333: cs = 333; goto _out; 
		_out334: cs = 334; goto _out; 
		_out335: cs = 335; goto _out; 
		_out336: cs = 336; goto _out; 
		_out337: cs = 337; goto _out; 
		_out338: cs = 338; goto _out; 
		_out339: cs = 339; goto _out; 
		_out340: cs = 340; goto _out; 
		_out341: cs = 341; goto _out; 
		_out342: cs = 342; goto _out; 
		_out343: cs = 343; goto _out; 
		_out344: cs = 344; goto _out; 
		_out345: cs = 345; goto _out; 
		_out346: cs = 346; goto _out; 
		_out347: cs = 347; goto _out; 
		_out348: cs = 348; goto _out; 
		_out349: cs = 349; goto _out; 
		_out350: cs = 350; goto _out; 
		_out351: cs = 351; goto _out; 
		_out352: cs = 352; goto _out; 
		_out353: cs = 353; goto _out; 
		_out354: cs = 354; goto _out; 
		_out355: cs = 355; goto _out; 
		_out356: cs = 356; goto _out; 
		_out357: cs = 357; goto _out; 
		_out358: cs = 358; goto _out; 
		_out359: cs = 359; goto _out; 
		_out360: cs = 360; goto _out; 
		_out361: cs = 361; goto _out; 
		_out362: cs = 362; goto _out; 
		_out363: cs = 363; goto _out; 
		_out364: cs = 364; goto _out; 
		_out365: cs = 365; goto _out; 
		_out366: cs = 366; goto _out; 
		_out367: cs = 367; goto _out; 
		_out368: cs = 368; goto _out; 
		_out369: cs = 369; goto _out; 
		_out370: cs = 370; goto _out; 
		_out371: cs = 371; goto _out; 
		_out372: cs = 372; goto _out; 
		_out373: cs = 373; goto _out; 
		_out374: cs = 374; goto _out; 
		_out375: cs = 375; goto _out; 
		_out376: cs = 376; goto _out; 
		_out377: cs = 377; goto _out; 
		_out378: cs = 378; goto _out; 
		_out379: cs = 379; goto _out; 
		_out380: cs = 380; goto _out; 
		_out381: cs = 381; goto _out; 
		_out382: cs = 382; goto _out; 
		_out383: cs = 383; goto _out; 
		_out384: cs = 384; goto _out; 
		_out385: cs = 385; goto _out; 
		_out386: cs = 386; goto _out; 
		_out387: cs = 387; goto _out; 
		_out388: cs = 388; goto _out; 
		_out389: cs = 389; goto _out; 
		_out390: cs = 390; goto _out; 
		_out391: cs = 391; goto _out; 
		_out392: cs = 392; goto _out; 
		_out393: cs = 393; goto _out; 
		_out394: cs = 394; goto _out; 
		_out395: cs = 395; goto _out; 
		_out396: cs = 396; goto _out; 
		_out397: cs = 397; goto _out; 
		_out398: cs = 398; goto _out; 
		_out399: cs = 399; goto _out; 
		_out400: cs = 400; goto _out; 
		_out401: cs = 401; goto _out; 
		_out402: cs = 402; goto _out; 
		_out403: cs = 403; goto _out; 
		_out404: cs = 404; goto _out; 
		_out405: cs = 405; goto _out; 
		_out406: cs = 406; goto _out; 
		_out407: cs = 407; goto _out; 
		_out408: cs = 408; goto _out; 
		_out409: cs = 409; goto _out; 
		_out410: cs = 410; goto _out; 
		_out411: cs = 411; goto _out; 
		_out412: cs = 412; goto _out; 
		_out413: cs = 413; goto _out; 
		_out414: cs = 414; goto _out; 
		_out415: cs = 415; goto _out; 
		_out416: cs = 416; goto _out; 
		_out417: cs = 417; goto _out; 
		_out418: cs = 418; goto _out; 
		_out419: cs = 419; goto _out; 
		_out420: cs = 420; goto _out; 
		_out421: cs = 421; goto _out; 
		_out422: cs = 422; goto _out; 
		_out423: cs = 423; goto _out; 
		_out424: cs = 424; goto _out; 
		_out425: cs = 425; goto _out; 
		_out426: cs = 426; goto _out; 
		_out427: cs = 427; goto _out; 
		_out428: cs = 428; goto _out; 
		_out429: cs = 429; goto _out; 
		_out430: cs = 430; goto _out; 
		_out431: cs = 431; goto _out; 
		_out432: cs = 432; goto _out; 
		_out433: cs = 433; goto _out; 
		_out434: cs = 434; goto _out; 
		_out435: cs = 435; goto _out; 
		_out436: cs = 436; goto _out; 
		_out437: cs = 437; goto _out; 
		_out438: cs = 438; goto _out; 
		_out439: cs = 439; goto _out; 
		_out440: cs = 440; goto _out; 
		_out441: cs = 441; goto _out; 
		_out442: cs = 442; goto _out; 
		_out443: cs = 443; goto _out; 
		_out444: cs = 444; goto _out; 
		_out445: cs = 445; goto _out; 
		_out446: cs = 446; goto _out; 
		_out447: cs = 447; goto _out; 
		_out448: cs = 448; goto _out; 
		_out449: cs = 449; goto _out; 
		_out450: cs = 450; goto _out; 
		_out451: cs = 451; goto _out; 
		_out452: cs = 452; goto _out; 
		_out453: cs = 453; goto _out; 
		_out454: cs = 454; goto _out; 
		_out455: cs = 455; goto _out; 
		_out456: cs = 456; goto _out; 
		_out457: cs = 457; goto _out; 
		_out458: cs = 458; goto _out; 
		_out459: cs = 459; goto _out; 
		_out460: cs = 460; goto _out; 
		_out461: cs = 461; goto _out; 
		_out462: cs = 462; goto _out; 
		_out463: cs = 463; goto _out; 
		_out464: cs = 464; goto _out; 
		_out465: cs = 465; goto _out; 
		_out466: cs = 466; goto _out; 
		_out467: cs = 467; goto _out; 
		_out468: cs = 468; goto _out; 
		_out469: cs = 469; goto _out; 
		_out470: cs = 470; goto _out; 
		_out471: cs = 471; goto _out; 
		_out472: cs = 472; goto _out; 
		_out473: cs = 473; goto _out; 
		_out474: cs = 474; goto _out; 
		_out475: cs = 475; goto _out; 
		_out476: cs = 476; goto _out; 
		_out477: cs = 477; goto _out; 
		_out478: cs = 478; goto _out; 
		_out479: cs = 479; goto _out; 
		_out480: cs = 480; goto _out; 
		_out481: cs = 481; goto _out; 
		_out482: cs = 482; goto _out; 
		_out483: cs = 483; goto _out; 
		_out484: cs = 484; goto _out; 
		_out485: cs = 485; goto _out; 
		_out486: cs = 486; goto _out; 
		_out487: cs = 487; goto _out; 
		_out488: cs = 488; goto _out; 
		_out489: cs = 489; goto _out; 
		_out490: cs = 490; goto _out; 
		_out491: cs = 491; goto _out; 
		_out492: cs = 492; goto _out; 
		_out493: cs = 493; goto _out; 
		_out494: cs = 494; goto _out; 
		_out495: cs = 495; goto _out; 
		_out496: cs = 496; goto _out; 
		_out497: cs = 497; goto _out; 
		_out498: cs = 498; goto _out; 
		_out499: cs = 499; goto _out; 
		_out500: cs = 500; goto _out; 
		_out501: cs = 501; goto _out; 
		_out502: cs = 502; goto _out; 
		_out503: cs = 503; goto _out; 
		_out504: cs = 504; goto _out; 
		_out505: cs = 505; goto _out; 
		_out506: cs = 506; goto _out; 
		_out507: cs = 507; goto _out; 
		_out508: cs = 508; goto _out; 
		_out509: cs = 509; goto _out; 
		_out510: cs = 510; goto _out; 
		_out511: cs = 511; goto _out; 
		_out512: cs = 512; goto _out; 
		_out513: cs = 513; goto _out; 
		_out514: cs = 514; goto _out; 
		_out515: cs = 515; goto _out; 
		_out516: cs = 516; goto _out; 
		_out517: cs = 517; goto _out; 
		_out518: cs = 518; goto _out; 
		_out519: cs = 519; goto _out; 
		_out520: cs = 520; goto _out; 
		_out521: cs = 521; goto _out; 
		_out522: cs = 522; goto _out; 
		_out523: cs = 523; goto _out; 
		_out524: cs = 524; goto _out; 
		_out525: cs = 525; goto _out; 
		_out526: cs = 526; goto _out; 
		_out527: cs = 527; goto _out; 
		_out528: cs = 528; goto _out; 
		_out529: cs = 529; goto _out; 
		_out530: cs = 530; goto _out; 
		_out531: cs = 531; goto _out; 
		_out532: cs = 532; goto _out; 
		_out533: cs = 533; goto _out; 
		_out534: cs = 534; goto _out; 
		_out535: cs = 535; goto _out; 
		_out536: cs = 536; goto _out; 
		_out537: cs = 537; goto _out; 
		_out538: cs = 538; goto _out; 
		_out539: cs = 539; goto _out; 
		_out540: cs = 540; goto _out; 
		_out541: cs = 541; goto _out; 
		_out542: cs = 542; goto _out; 
		_out543: cs = 543; goto _out; 
		_out544: cs = 544; goto _out; 
		_out545: cs = 545; goto _out; 
		_out546: cs = 546; goto _out; 
		_out547: cs = 547; goto _out; 
		_out548: cs = 548; goto _out; 
		_out549: cs = 549; goto _out; 
		_out550: cs = 550; goto _out; 
		_out551: cs = 551; goto _out; 
		_out552: cs = 552; goto _out; 
		_out553: cs = 553; goto _out; 
		_out554: cs = 554; goto _out; 
		_out555: cs = 555; goto _out; 
		_out631: cs = 631; goto _out; 
		_out556: cs = 556; goto _out; 
		_out557: cs = 557; goto _out; 
		_out558: cs = 558; goto _out; 
		_out559: cs = 559; goto _out; 
		_out560: cs = 560; goto _out; 
		_out561: cs = 561; goto _out; 
		_out562: cs = 562; goto _out; 
		_out563: cs = 563; goto _out; 
		_out564: cs = 564; goto _out; 
		_out565: cs = 565; goto _out; 
		_out566: cs = 566; goto _out; 
		_out567: cs = 567; goto _out; 
		_out568: cs = 568; goto _out; 
		_out569: cs = 569; goto _out; 
		_out570: cs = 570; goto _out; 
		_out571: cs = 571; goto _out; 
		_out572: cs = 572; goto _out; 
		_out573: cs = 573; goto _out; 
		_out574: cs = 574; goto _out; 
		_out575: cs = 575; goto _out; 
		_out576: cs = 576; goto _out; 
		_out577: cs = 577; goto _out; 
		_out578: cs = 578; goto _out; 
		_out579: cs = 579; goto _out; 
		_out580: cs = 580; goto _out; 
		_out581: cs = 581; goto _out; 
		_out582: cs = 582; goto _out; 
		_out583: cs = 583; goto _out; 
		_out584: cs = 584; goto _out; 
		_out585: cs = 585; goto _out; 
		_out586: cs = 586; goto _out; 
		_out587: cs = 587; goto _out; 
		_out588: cs = 588; goto _out; 
		_out589: cs = 589; goto _out; 
		_out590: cs = 590; goto _out; 
		_out591: cs = 591; goto _out; 
		_out592: cs = 592; goto _out; 
		_out593: cs = 593; goto _out; 
		_out594: cs = 594; goto _out; 
		_out595: cs = 595; goto _out; 
		_out596: cs = 596; goto _out; 
		_out597: cs = 597; goto _out; 
		_out598: cs = 598; goto _out; 
		_out599: cs = 599; goto _out; 
		_out600: cs = 600; goto _out; 
		_out601: cs = 601; goto _out; 
		_out602: cs = 602; goto _out; 
		_out603: cs = 603; goto _out; 
		_out604: cs = 604; goto _out; 
		_out605: cs = 605; goto _out; 
		_out606: cs = 606; goto _out; 
		_out607: cs = 607; goto _out; 
		_out608: cs = 608; goto _out; 
		_out609: cs = 609; goto _out; 
		_out610: cs = 610; goto _out; 
		_out611: cs = 611; goto _out; 
		_out612: cs = 612; goto _out; 
		_out613: cs = 613; goto _out; 
		_out614: cs = 614; goto _out; 
		_out615: cs = 615; goto _out; 
		_out616: cs = 616; goto _out; 
		_out617: cs = 617; goto _out; 
		_out618: cs = 618; goto _out; 
		_out619: cs = 619; goto _out; 
		_out620: cs = 620; goto _out; 
		_out621: cs = 621; goto _out; 
		_out632: cs = 632; goto _out; 
		_out622: cs = 622; goto _out; 
		_out: {}
	}
	
#line 79 "scanner.rl"

	
	if (cs == strings_error)
		printf("Error near %zd\n", p - in);
}

