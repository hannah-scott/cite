#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "config.h"

/*
 * Create and open index file
 */
FILE *open_index(char *fname)
{
	char idx[MAXLINELENGTH];
	strcpy(idx, DESTIDX);
	strcat(idx, "/");
	strcat(idx, fname);

	FILE *index = fopen(idx, "w");

	if (index == NULL) {
		printf("Failed to open index");
		exit(EXIT_FAILURE);
	}
}

/*
 * Inject head and header text into file
 */
FILE *inject_head(FILE * page)
{
	fprintf(page, "<html>\n");
	fprintf(page, "<head>\n");
	fprintf(page, "<title>%s</title>\n", TITLE);
	fprintf(page, "<link rel='stylesheet' href='%s'", CSS);
	fprintf(page, "</head>\n");

	// body
	fprintf(page, "<!-- Generated static page, don't edit this -->\n");
	fprintf(page, "<body>\n");
	fprintf(page, "<header><a href='%sindex.html'>Home</a>\n", BASEURL);

	fprintf(page, "<h1>%s</h1>\n", TITLE);

	return page;
}

/*
 * Inject footer text into file 
 */
FILE *inject_foot(FILE * page)
{
	fprintf(page, "</body>\n</html>\n");

	return page;
}

/*
 * Inject contents on file into another
 */
FILE *inject_contents(FILE * body, char *in)
{
	char s[MAXLINELENGTH];

	FILE *inf = fopen(in, "r");

	if (inf == NULL) {
		printf("File not found: %s\n", in);
		return body;
	}

	while (fgets(s, MAXLINELENGTH, inf) != NULL) {
		fprintf(body, "%s", s);
	}

	fclose(inf);

	return body;
}

/*
 * Add a link to index file
 */
int add_to_index(FILE * index, char *link, char *name)
{

	char a[MAXLINELENGTH];

	strcpy(a, "<div><a href='");
	strcat(a, BASEURL);
	strcat(a, link);
	strcat(a, "'>");
	strcat(a, name);
	strcat(a, "</a></div>\n");
	fprintf(index, "%s", a);
	return 0;
}

/*
 * Build page from source
 */
int build_page(char *in, char *out)
{
	FILE *page = fopen(out, "w");

	if (page == NULL) {
		printf("\nFile couldn't be created: %s\n", out);
		return 1;
	}

	page = inject_foot(inject_contents(inject_head(page), in));

	fclose(page);
	return 0;
}

/*
 * Build all pages in directory
 */
int build_pages(char *dirname)
{
	int builderr;
	char idx[MAXLINELENGTH];
	char outidx[MAXLINELENGTH];
	struct dirent **dirlist;
	struct dirent *dir;

	FILE *index = inject_head(open_index("index.html"));

	int n = scandir(dirname, &dirlist, NULL, alphasort);

	if (n == -1) {
		perror("scandir");
		exit(EXIT_FAILURE);
	}

	while (n--) {
		dir = dirlist[n];
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
			printf("building %s ... ", dir->d_name);

			strcpy(idx, dirname);
			strcat(idx, "/");
			strcat(idx, dir->d_name);

			strcpy(outidx, DESTDIR);
			strcat(outidx, "/");
			strcat(outidx, dir->d_name);

			builderr = build_page(idx, outidx);

			if (builderr == 0) {
				add_to_index(index, idx, dir->d_name);
			}
			printf("done\n");
		} 

	}
	free(dir);
	free(dirlist);

	fclose(inject_foot(index));

	return 0;
}

int main(int argc, char *argv[])
{
	int err = build_pages(SRCDIR);

	return 0;
}
