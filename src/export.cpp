/* wabbit.c -- Wabbitsign, the Free APP signer for TI-83 Plus
    by Spencer Putt and James Montelongo */

/* Modified for use in SPASM by Don Straney */

#include "stdafx.h"

#include "utils.h"

#undef  show_fatal_error_prefix
#define show_fatal_error_prefix(zcif, zln) printf ("signer: error: ")

#undef  show_warning_prefix
#define show_warning_prefix(zcif, zln) printf ("signer: warning: ")

#define name    (header8xk + 17)
#define hleng   sizeof(header8xk)
unsigned char header8xk[] = {
    '*','*','T','I','F','L','*','*',    /* required identifier */
    1, 1,                               /* version */
    1, 0x88,                            /* unsure, but always set like this */
    0x01, 0x01, 0x19, 0x97,             /* Always sets date to jan. 1, 1997 */
    8,                                  /* Length of name. */
    0,0,0,0,0,0,0,0};                   /* space for the name */


const unsigned char nbuf[]= {
    0xAD,0x24,0x31,0xDA,0x22,0x97,0xE4,0x17,
    0x5E,0xAC,0x61,0xA3,0x15,0x4F,0xA3,0xD8,
    0x47,0x11,0x57,0x94,0xDD,0x33,0x0A,0xB7,
    0xFF,0x36,0xBA,0x59,0xFE,0xDA,0x19,0x5F,
    0xEA,0x7C,0x16,0x74,0x3B,0xD7,0xBC,0xED,
    0x8A,0x0D,0xA8,0x85,0xE5,0xE5,0xC3,0x4D,
    0x5B,0xF2,0x0D,0x0A,0xB3,0xEF,0x91,0x81,
    0xED,0x39,0xBA,0x2C,0x4D,0x89,0x8E,0x87};
const unsigned char pbuf[]= {
    0x5B,0x2E,0x54,0xE9,0xB5,0xC1,0xFE,0x26,
    0xCE,0x93,0x26,0x14,0x78,0xD3,0x87,0x3F,
    0x3F,0xC4,0x1B,0xFF,0xF1,0xF5,0xF9,0x34,
    0xD7,0xA5,0x79,0x3A,0x43,0xC1,0xC2,0x1C};
const unsigned char qbuf[]= {
    0x97,0xF7,0x70,0x7B,0x94,0x07,0x9B,0x73,
    0x85,0x87,0x20,0xBF,0x6D,0x49,0x09,0xAB,
    0x3B,0xED,0xA1,0xBA,0x9B,0x93,0x11,0x2B,
    0x04,0x13,0x40,0xA1,0x6E,0xD5,0x97,0xB6,0x04};

// q ^ (p - 2))
const unsigned char qpowpbuf[] = {
    0xA3,0x82,0x96,0xAF,0x3D,0xDD,0x9B,0x94,
    0xAE,0xA0,0x2F,0x2C,0xE3,0x8B,0xCD,0xD9,
    0xC9,0x11,0x75,0x4F,0x00,0xE4,0xDF,0x47,
    0x38,0xCD,0x98,0x16,0x47,0xF5,0x2B,0x0F};
const unsigned char p14buf[] = {
    0x97,0x0B,0x55,0x7A,0x6D,0xB0,0xBF,0x89,
    0xF3,0xA4,0x09,0x05,0xDE,0xF4,0xE1,0xCF,
    0x0F,0xF1,0xC6,0x7F,0x7C,0x7D,0x3E,0xCD,
    0x75,0x69,0x9E,0xCE,0x50,0xB0,0x30,0x07};
const unsigned char q14buf[] = {
    0xE6,0x3D,0xDC,0x1E,0xE5,0xC1,0xE6,0x5C,
    0xE1,0x21,0xC8,0x6F,0x5B,0x52,0xC2,0xEA,
    0x4E,0x7B,0xA8,0xEE,0xE6,0x64,0xC4,0x0A,
    0xC1,0x04,0x50,0xA8,0x5B,0xF5,0xA5,0x2D,0x01};

const unsigned char fileheader[]= {
    '*','*','T','I','*',0x1A,0x0A,0x00};
const char comment[42]= "File generated by WabbitSign";

const char typearray[] = {
    '7','3','*',0x0B,
    '8','2','*',0x0B,
    '8','3','*',0x0B,
    '8','3','F',0x0D,
    '8','5','*',0x00,
    '8','6','*',0x0C,
    '8','5','*',0x00,
    '8','6','*',0x0C,
    };

const char extensions[][4] = {
    "73P","82P","83P","8XP","85P","86P","85S","86S","8XK","BIN"};

int findfield ( unsigned char byte, const unsigned char* buffer );
int siggen (const unsigned char* hashbuf, unsigned char* sigbuf, int* outf);
void intelhex (FILE * outfile , const unsigned char* buffer, int size);
void alphanumeric (char* namestring);
void makeapp (const unsigned char *output_contents, DWORD output_len, FILE *outfile, const char *prgmname);
void makeprgm (const unsigned char *output_contents, int size, FILE *outfile, const char *prgmname, int calc);

void write_file (const unsigned char *output_contents, int output_len, const char *output_filename) {
	FILE *outfile;
	int i, calc;

	//get the type from the extension of the output filename
	for (i = strlen (output_filename); output_filename[i] != '.' && i; i--);
	if (i != 0) {
		const char *ext = output_filename + i + 1;

		for (calc = 0; calc < 10; calc++) {
			if (!_stricmp (ext, extensions[calc]))
				break;
		}

		if (calc == 10) {
			show_warning ("Output extension not recognized, assuming .bin");
			calc = 9;
		}

	} else {
		show_warning ("No output extension given, assuming .bin");
		calc = 9;
	}

	//open the output file
	outfile = fopen (output_filename, "wb");
	if (!outfile) {
		show_fatal_error ("Couldn't open output file");
		return;
	}

	for (i = strlen (output_filename); output_filename[i] != '\\' && output_filename[i] != '/' && i; i--);
	if (i != 0)
		i++;
	
	char prgmname[MAX_PATH];
	strcpy(prgmname, output_filename);
	
	for (i = strlen (prgmname); prgmname[i] != '.' && i; i--);
	if (i != 0)
		prgmname[i] = '\0';

	//then decide how to write the contents
	if (calc == 8) //8XK
		makeapp (output_contents, (DWORD) output_len, outfile, prgmname);
	else if (calc == 9) { //BIN
		for (i = 0; i < output_len; i++)
			fputc(output_contents[i], outfile);
    } else
        makeprgm (output_contents, output_len, outfile, prgmname, calc);

	fclose (outfile);
}


void makeapp (const unsigned char *output_contents, DWORD size, FILE *outfile, const char* prgmname) {
    unsigned char *buffer;
    int i,pnt,siglength,tempnum,f,pages;
    unsigned int total_size;

    /* Copy file to memory */
    buffer = (unsigned char *) malloc_chk (size+256);
	memcpy (buffer, output_contents, sizeof (char) * size);

/* Check if size will fit in mem with signature */
    if ((tempnum = ((size+96)%16384))) {
        if (tempnum < 97) {
			show_fatal_error ("The last page must have room for the signature!\n Roughly 96 bytes.");
			return;
		}
        if (tempnum<1024 && (size+96)>>14) {
			show_warning ("Warning: There are only %d bytes on the last page.\n", tempnum);
		}
    }

/* Fix app header fields */
/* Length Field: set to size of app - 6 */
    if (!(buffer[0] == 0x80 && buffer[1] == 0x0F)) {
        show_fatal_error ("Length field not present.");
		return;
	}
    size -= 6;
    buffer[2] = size >> 24;         //Stored in Big Endian
    buffer[3] = (size>>16) & 0xFF;
    buffer[4] = (size>> 8) & 0xFF;
    buffer[5] = size & 0xFF;
    size += 6;
/* Program Type Field: Must be present and shareware (0104) */
    pnt = findfield(0x12, buffer);
    if (!pnt || ( buffer[pnt++]!=1) || (buffer[pnt]!=4) ) {
        show_fatal_error ("Program type field missing or incorrect.");
		return;
	}
/* Pages Field: Corrects page num*/
    pnt = findfield(0x81, buffer);
    if (!pnt) {
        show_fatal_error ("Pages field missing.");
		return;
	}
    
    pages = size>>14; /* this is safe because we know there's enough room for the sig */
    if (size & 0x3FFF) pages++;
    buffer[pnt] = pages;
/* Name Field: MUST BE 8 CHARACTERS, no checking if valid */
    pnt = findfield(0x48, buffer);
    if (!pnt) {
        show_fatal_error ("Name field missing.");
		return;
	}
    for (i=0; i < 8 ;i++) name[i]=buffer[i+pnt];

/* Calculate MD5 */
#ifdef WIN32
	unsigned char hashbuf[64];
	HCRYPTPROV hCryptProv; 
    HCRYPTHASH hCryptHash;
	DWORD sizebuf = ARRAYSIZE(hashbuf);
	CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET);
	CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hCryptHash);
	CryptHashData(hCryptHash, buffer, size, 0);
	CryptGetHashParam(hCryptHash, HP_HASHVAL, hashbuf, &sizebuf, 0);
#else
	unsigned char hashbuf[16];
    MD5 (buffer, size, hashbuf);  //This uses ssl but any good md5 should work fine.
#endif

/* Generate the signature to the buffer */
    siglength = siggen(hashbuf, buffer+size+3, &f );

/* append sig */
    buffer[size + 0] = 0x02;
    buffer[size + 1] = 0x2d;
    buffer[size + 2] = (unsigned char) siglength;
    total_size = size + siglength + 3;
    if (f) {
        buffer[total_size++] = 1;
        buffer[total_size++] = f;
    } else buffer[total_size++] = 0;
/* sig must be 96 bytes ( don't ask me why) */
    tempnum = 96 - (total_size - size);
    while (tempnum--) buffer[total_size++] = 0xFF;

/* Do 8xk header */
    for (i = 0; i < hleng; i++) fputc(header8xk[i], outfile);
    for (i = 0; i < 23; i++)    fputc(0, outfile);
    fputc(0x73, outfile);
    fputc(0x24, outfile);
    for (i = 0; i < 24; i++)    fputc(0, outfile);
    tempnum =  77 * (total_size>>5) + pages * 17 + 11;
    size = total_size & 0x1F;
    if (size) tempnum += (size<<1) + 13;
    fputc( tempnum & 0xFF, outfile); //little endian
    fputc((tempnum >> 8) & 0xFF, outfile);
    fputc((tempnum >> 16)& 0xFF, outfile);
    fputc( tempnum >> 24, outfile);
    
/* Convert to 8xk */
    intelhex(outfile, buffer, total_size);

#ifdef WIN32
	if (hCryptHash) {
		CryptDestroyHash(hCryptHash);
		hCryptHash = NULL;
	}
	if (hCryptProv) {
		CryptReleaseContext(hCryptProv,0);
		hCryptProv = NULL;
	}
#endif
	free(buffer);
//    if (pages==1) printf("%s (%d page",filename,pages);
//    else printf("%s (%d pages",filename,pages);
//	puts(") was successfully generated!");
}


/* Starting from 0006 searches for a field
 * in the in file buffer. */
int findfield( unsigned char byte, const unsigned char* buffer ) {
    int pnt=6;
    while (buffer[pnt++] == 0x80) {
        if (buffer[pnt] == byte) {
            pnt++;
            return pnt;
        } else
            pnt += (buffer[pnt]&0x0F);
        pnt++;
    }
    return 0;
}

/* Gmp was originally used by Ben Moody, but due to it's massive size
 * Spencer wrote his own big num routines,  cutting the size to a tenth 
 * of what it was. */
int siggen(const unsigned char* hashbuf, unsigned char* sigbuf, int* outf) {
#ifdef USE_GMP
	mpz_t
#else
	big 
#endif
    mhash, p, q, r, s, temp, result;
    
	unsigned int lp,lq;
    int siglength;
    
/* Intiate vars */
#ifdef USE_GMP
    mpz_init(mhash);
    mpz_init(p);
    mpz_init(q);
    mpz_init(r);
    mpz_init(s);
    mpz_init(temp);
    mpz_init(result);
#else
    mhash = big_create();
    p  = big_create();
    q = big_create();
    r = big_create();
    s = big_create();
    temp = big_create();
    result = big_create();
#endif
    
/* Import vars */
#ifdef USE_GMP
    mpz_import(mhash, 16, -1, 1, -1, 0, hashbuf);
    mpz_import(p, sizeof(pbuf), -1, 1, -1, 0, pbuf);
    mpz_import(q, sizeof(qbuf), -1, 1, -1, 0, qbuf);
#else
    big_read(mhash,hashbuf,16);
    big_read(p,pbuf,sizeof(pbuf));
    big_read(q,qbuf,sizeof(qbuf));
#endif
/*---------Find F----------*/
/*      M' = m*256+1      */
#ifdef USE_GMP
    mpz_mul_ui(mhash, mhash, 256);
    mpz_add_ui(mhash, mhash, 1);
#else
    for (i = 0; i < 8; i++)
    big_sll(mhash);
    big_add_ui(mhash,mhash,1);
#endif
    
/* calc f {2, 3,  0, 1 }  */
#ifdef USE_GMP
    lp = mpz_legendre(mhash, p) == 1 ? 0 : 1;
    lq = mpz_legendre(mhash, q) == 1 ? 1 : 0;
#else
    lp = ((big_legendre(mhash,p)==1)?0:1);
    lq = ((big_legendre(mhash,q)==1)?1:0);
#endif
    *outf = lp+lq+lq;

/*apply f */
#ifdef USE_GMP
    if (lp == lq)
    	mpz_mul_ui(mhash, mhash, 2);
    if (lq == 0) {
    	mpz_import(temp, sizeof(nbuf), -1, 1, -1, 0, nbuf);
    	mpz_sub(mhash, temp, mhash);
    }
#else
    if (lp==lq) big_sll(mhash);
    if (lq==0) {
        big_read(temp,nbuf,sizeof(nbuf));
        big_sub(mhash,temp,mhash);
    }
#endif

/* r = ( M' ^ ( ( p + 1) / 4 ) ) mod p */
#ifdef USE_GMP
    mpz_import(result, sizeof(p14buf), -1, 1, -1, 0, p14buf);
    mpz_powm(r, mhash, result, p);
#else
    big_read(result,p14buf,sizeof(p14buf)); //Several numbers are precalculated to save time
    big_powm(r,mhash,result,p);
#endif
    
/* s = ( M' ^ ( ( q + 1) / 4 ) ) mod q */
#ifdef USE_GMP
    mpz_import(result, sizeof(q14buf), -1, 1, -1, 0, q14buf);
    mpz_powm(s, mhash, result, q);
#else
    big_read(result,q14buf,sizeof(q14buf));
    big_powm(s,mhash,result,q);
#endif
    
/* r-s */
#ifdef USE_GMP
    mpz_set_ui(temp, 0);
    mpz_sub(temp, r, s);
#else
    big_set_ui(temp,0);
    big_sub(temp, r, s);
#endif
    
/* q ^ (p - 2)) */
#ifdef USE_GMP
    mpz_import(result, sizeof(qpowpbuf), -1, 1, -1, 0, qpowpbuf);
#else
    big_read(result,qpowpbuf,sizeof(qpowpbuf));
#endif
    
/* (r-s) * q^(p-2) mod p */
#ifdef USE_GMP
    mpz_mul(temp, temp, result);
    mpz_mod(temp, temp, p);
#else
    big_mul(temp, temp, result);
    big_mod(temp, temp, p);
#endif
	
/* ((r-s) * q^(p-2) mod p) * q + s */
#ifdef USE_GMP
    mpz_mul(result, temp, q);
    mpz_add(result, result, s);
#else
    big_mul(result, temp, q);
    big_add(result, result, s);
#endif
    
/* export sig */
#ifdef USE_GMP
    siglength = mpz_sizeinbase(result, 16);
    siglength = (siglength + 1) / 2;
    mpz_export(sigbuf, NULL, -1, 1, -1, 0, result);
#else
    for (i = 0; i < kMaxLength; i++) {
        if (result->number[i]) siglength = i;
    }
    
    siglength++;
//    for (siglength = 0; ( (siglength < kMaxLength) && (result->number[i]) ); siglength++); 
    for (i = 0; i < siglength; i++) sigbuf[i] = result->number[i];
#endif
    
/* Clean Up */
#ifdef USE_GMP
    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(temp);
    mpz_clear(result);
#else
    big_clear(p);
    big_clear(q);
    big_clear(r);
    big_clear(s);
    big_clear(temp);
    big_clear(result);
#endif
    return siglength;
}


void makeprgm (const unsigned char *output_contents, int size, FILE *outfile, const char *prgmname, int calc) {
    int i, temp, chksum;
    unsigned char* pnt;
    char *namestring;
    
	if (calc==1) {
		char name_buf[256];
		name_buf[0] = (char) 0xDC;
		name_buf[1] = '\0';
		strcat(name_buf, prgmname);
		namestring = strdup (name_buf);
	} else {
		char *p = (char *) prgmname, *lastSlash = (char *) prgmname;
		while ((p = strchr(p, '/')) != NULL) {
			printf("%p", p);
			lastSlash = ++p;
		}
		namestring = strdup (lastSlash);
	    /* The name must be Capital lettes and numbers */
	    if (calc < 4) {
	        alphanumeric (namestring);
	    }
	}
    /* get size */
	size += 2;
    /* 86ers don't need to put the asm token*/
    if (calc==5) {
		size += 2;
    }
    /* size must be smaller than z80 mem */
    if (size > 24000) {
        if (size > 65000) {
			show_warning ("File size is greater than 64k");
		} else {
        	show_warning ("File size is greater than 24k");
		}
    }
    
    /* Lots of pointless header crap */
    for (i = 0; i < 4; i++)
		fputc (fileheader[i], outfile);
    pnt = ((unsigned char*)typearray+(calc<<2));
    fputc (pnt[0],outfile);
    fputc (pnt[1],outfile);
    fputc (pnt[2],outfile);
    fputc (fileheader[i++],outfile);
    fputc (fileheader[i++],outfile);
    if (calc == 4) {
        fputc (0x0C,outfile);
    } else {
        fputc (fileheader[i++],outfile);
    }    
    fputc (fileheader[i++],outfile);
    
    // Copy in the comment
    for (i = 0; i < 42; i++)
		fputc(comment[i],outfile);

    if (calc == 1) size+=3;
    /* For some reason TI thinks it's important to put the file size */
    /* dozens of times, I suppose duplicates add security...*/
    /* yeah right. */
    if (calc == 4) {
        temp = size + 8 + strlen (namestring);
    } else {
        temp = size+15+((calc==3)?2:0)+((calc==5)?1:0);
    }
    fputc(temp & 0xFF,outfile);
    fputc(temp >> 8,outfile);
    if (calc==4) {
        chksum = fputc((6+strlen(namestring)),outfile);
    } else {
        chksum = fputc(pnt[3],outfile);
    }
    fputc(0,outfile);
    /* OMG the Size again! */
    chksum += fputc(size & 0xFF,outfile);
    chksum += fputc(size>>8,outfile);
    if ( calc>=4) {
        if (calc>=6) {
            chksum += fputc(0x0C,outfile);
        } else {
            chksum += fputc(0x12,outfile);
        }
        chksum += fputc(strlen(namestring),outfile);
    } else {
        chksum += fputc(6,outfile);
    }
    
    /* The actual name is placed with padded with zeros */
    if (calc<4 && calc != 1) { //i know...just leave it for now.
        if (!((temp=namestring[0])>='A' && temp<='Z')) show_warning ("First character in name must be a letter.");
    }
    for(i = 0; i < 8 && namestring[i]; i++) chksum += fputc(namestring[i], outfile);
    if (calc != 4 && calc!=6) {
        for(;i < 8; i++) fputc(0,outfile);
    }
    /* 83+ requires 2 extra bytes */
    if (calc==3) {
        fputc(0,outfile);
        fputc(0,outfile);
    }
    /*Yeah, lets put the size twice in a row  X( */
    chksum += fputc(size & 0xFF,outfile);
    chksum += fputc(size>>8,outfile);
    size-=2;
    chksum += fputc(size & 0xFF,outfile);
    chksum += fputc(size>>8,outfile);
    
    /* check for BB 6D on 83+ */
    if ((calc == 3) && !((((unsigned char) output_contents[0]) == 0xBB) && (((unsigned char) output_contents[1]) == 0x6D))) {
		show_warning ("83+ program does not begin with bytes BB 6D.");
    }
    if (calc == 5) {
	   chksum += fputc (0x8E, outfile);
	   chksum += fputc (0x28, outfile);
	   size -= 2;
    } else if (calc == 1) {
    	chksum += 
    		fputc (0xD5, outfile);
    	chksum += 
    		fputc (0x00, outfile);
    	chksum += 
    		fputc (0x11, outfile);
    }
    
    if (calc == 1) size -= 3;
    /* Actual program data! */
	for (i = 0; i < size; i++) {
	    chksum += fputc (output_contents[i], outfile);
	}
    /* short little endian Checksum */
    fputc (chksum & 0xFF,outfile);
    fputc ((chksum >> 8) & 0xFF,outfile);
//    printf("%s (%d bytes) was successfully generated!\n",filestring,size);

	free (namestring);
}


void alphanumeric (char* namestring) {
    char temp;

    while ((temp = *namestring)) {
        if (temp>='a' && temp<='z') *namestring = temp =(temp-('a'-'A'));
        if (!( ((temp>='A') && (temp<='Z')) || (temp>='0' && temp<='9') || (temp==0) ) ) {
            show_warning ("Invalid characters in name. Alphanumeric Only.");
        }
        namestring++;
    }
}


/* Convert binary buffer to intel hex in ti format
 * All pages addressed to $4000 and are only $4000
 * bytes long. */
void intelhex (FILE* outfile, const unsigned char* buffer, int size) {
    const char hexstr[] = "0123456789ABCDEF";
    int page = 0;
    int bpnt = 0;
    unsigned int address,ci,temp,i;
    unsigned char chksum;
    unsigned char outbuf[128];
    
    //We are in binary mode, we must handle carridge return ourselves.
   
    while (bpnt < size){
        fprintf(outfile,":02000002%04X%02X\r\n",page,(unsigned char) ( (~(0x04 + page)) +1));
        page++;
        address = 0x4000;   
        for (i = 0; bpnt < size && i < 512; i++) {
             chksum = (address>>8) + (address & 0xFF);
             for(ci = 0; ((ci < 64) && (bpnt < size)); ci++) {
                temp = buffer[bpnt++];
                outbuf[ci++] = hexstr[temp>>4];
                outbuf[ci] = hexstr[temp&0x0F];
                chksum += temp;
            }
            outbuf[ci] = 0;
            ci>>=1;
            fprintf(outfile,":%02X%04X00%s%02X\r\n",ci,address,outbuf,(unsigned char)( ~(chksum + ci)+1));
            address +=0x20;
        }         
    }
    fprintf(outfile,":00000001FF");
}
