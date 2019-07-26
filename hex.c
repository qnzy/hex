/*
 * hex: simple hex dump / reverse hex dump utility
 * yek, 2017
 * public domain / CC0
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFERSIZE 16
unsigned char buffer[BUFFERSIZE];

void bin2text(FILE*fin, FILE*fout) {
    size_t bytes_read=0;
    unsigned int i;
    while((bytes_read = fread(buffer, sizeof(unsigned char), BUFFERSIZE, fin))>0) {
        for (i=0; i<bytes_read; i++)
            fprintf(fout, "%02x ", buffer[i]);
        for (; i<BUFFERSIZE;i++)
            fprintf(fout, "   ");
        fprintf(fout, " ; ");
        for (i=0; i<bytes_read; i++) {
            if (isprint(buffer[i]))
                fprintf(fout, "%c", buffer[i]);
            else
                fprintf(fout, ".");
        }
        fprintf(fout, "\n");
        if (feof(fin)) break;
    }
}

int decode_nibble(char c) {
    if (c>='0' && c<='9')
        return c-'0';
    else if (c>='a' && c <= 'f')
        return c-'a'+10;
    else
        return c-'A'+10;
}

void parse_error(int line) {
    fprintf(stderr,"error on line %i\n", line);
    exit(EXIT_FAILURE);
}

void text2bin(FILE*fin, FILE*fout) {
    size_t bytes_read=0;
    int i;
    int accu=0;
    int line=1;
#define CLEAN_ST   0
#define NUM_ST     1
#define COMMENT_ST 2
    int state=CLEAN_ST;
    while((bytes_read = fread(buffer, sizeof(unsigned char), BUFFERSIZE, fin))>0) {
        for (i=0; i<bytes_read; i++) {
            switch(state) {
                case CLEAN_ST: 
                    accu=0;
                    if (isxdigit(buffer[i])) {
                        accu=decode_nibble(buffer[i]);
                        state=NUM_ST;
                    } else if (isspace(buffer[i])) {
                        state=CLEAN_ST;
                    } else if (buffer[i]==';') {
                        state=COMMENT_ST;
                    } else {
                        parse_error(line);
                    }
                    break;
                case COMMENT_ST:
                    if (buffer[i]=='\n') {
                        state=CLEAN_ST;
                    } else {
                        state=COMMENT_ST;
                    }
                    break;
                case NUM_ST:
                    if (isxdigit(buffer[i])) {
                        accu=accu*16+decode_nibble(buffer[i]);
                        if (accu>255) {
                            parse_error(line);
                        }
                        else state=NUM_ST;
                    } else {
                        fputc(accu, fout);
                        accu=0;
                        if (buffer[i]==';')  {
                            state=COMMENT_ST;
                        } else if (isspace(buffer[i])) {
                            state=CLEAN_ST;
                        } else {
                            parse_error(line);
                        }
                        accu=0;
                    }
                    break;
                default: break;
            }
            if (buffer[i]=='\n') {
                line++;
            }
        }
        if (feof(fin)) break;
    }
    if (state==NUM_ST) {
        fputc(accu, fout);
    }
}

void usage_error(void) {
    fprintf(stderr,"hex [-r] [-h] [inputFile] [-o outputFile]\n");
    fprintf(stderr,"    -h         : this help\n");
    fprintf(stderr,"    -r         : reverse hexdump (read hex and write binary)\n");
    fprintf(stderr,"    inputFile  : file to read,  default is stdin\n");
    fprintf(stderr,"    outputFile : file to write, default is stdout\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char ** argv) {
    FILE * fin;
    FILE * fout;
    int reverse=0;
    int infileidx=-1;
    int outfileidx=-1;
    int i;
    for (i=1; i<argc; i++)  {
        if (!strcmp(argv[i], "-r")) reverse=1;
        else if (!strcmp(argv[i], "-h")) usage_error();
        else if (!strcmp(argv[i], "-o")) {
                if (i<argc-1) outfileidx=++i;
                else          usage_error();
        }
        else {
            if (infileidx==-1) infileidx=i;
            else               usage_error();
        }
    }
    if (infileidx==-1) 
        fin=stdin;
    else {
    fin = fopen(argv[infileidx],"rb");
        if (fin == NULL) {
            fprintf(stderr,"hex: could not open file %s\n", argv[infileidx]);
            return EXIT_FAILURE;
        }
    }
    if (outfileidx==-1)
        fout=stdout;
    else {
        fout = fopen(argv[outfileidx],"wb");
        if (fout == NULL) {
            fprintf(stderr,"hex: could not open file %s\n", argv[outfileidx]);
            return EXIT_FAILURE;
        }
    }
    if (reverse) text2bin(fin, fout);
    else         bin2text(fin, fout);
    fclose(fin);
    fclose(fout);
    return EXIT_SUCCESS;
}

