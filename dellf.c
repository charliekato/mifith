#include <stdio.h>

void usage(const char *cmdname);

void usage(const char *cmdname) {
    printf("%s : strip lf character...\n",cmdname);
    printf("usage; \n");
    printf("   %s infile outfile\n",cmdname);
}

char	*
remove_lf(char	*buf) {
    char	*ptr=buf;
    while(*ptr) {
	if ((*ptr==0x0d)&&(*(ptr+1)==0x0a)) {
	    *ptr++=0x0a;
	    *ptr=0;
	    break;
	}
	ptr++;
    }
    return buf;
}
		
		

int main(int argc, char **argv) {
    FILE	*infile;
    FILE	*outfile;
    char	buf[BUFSIZ];

    if (argc !=3) {
	usage(argv[0]);
	return 0;
    }
    infile = fopen(argv[1],"r");
    if (!infile) {
	fprintf(stderr,"cannot open infile %s\n", argv[1]);
	return 0;
    }
    outfile = fopen(argv[2],"w");
    if (!outfile) {
	fprintf(stderr,"cannot open outfile %s\n", argv[2]);
	return 0;
    }
    while (fgets(buf, BUFSIZ,infile)) {
	fprintf(outfile,"%s", remove_lf(buf) );
    }
    fclose(infile);
    fclose(outfile);
    return 1;
}


