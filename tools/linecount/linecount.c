#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *countstream;
    int looper;
    unsigned long filecount = 0;
	unsigned long linesinfile;
	unsigned long totalcount = 0;

	if (argc == 1)
	{
		printf("USAGE: %s <filename> [...]\n"
			   "   This program will count the lines in all specified\n"
			   "   files, then report statistics. Wildcards allowed.\n",
				argv[0]);
		return(1);
	} /* if */

    #ifdef __EMX__
        _wildcard(&argc, &argv);
    #endif

	for (looper = 1; looper < argc; looper++)
	{
		printf("%s : ", argv[looper]);
		countstream = fopen(argv[looper], "rb");
		if (countstream == NULL)
			printf("Couldn't open.\n");
		else
		{
			filecount++;
			linesinfile = 0;
			do 
			{
				if (fgetc(countstream) == '\n')
					linesinfile++;
			} while (!feof(countstream));

			fclose(countstream);
            printf("%lu lines.\n", ++linesinfile);
			totalcount += linesinfile;
		} /* else */

	} /* for */

    printf("\n  Total lines in %lu files : %lu\n", filecount, totalcount);
	return(0);

} /* main */

/* end of linecount.c */

