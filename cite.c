#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "config.h"

int slen(char a[])
{
	int i = 0;
	for (i = 0; i < URLLEN - 1; i++) {
		if (a[i] == '\0') {
			return i;
		}
	}
	return i;
}

void sct(char a[], char b[], int l)
{
	int s = slen(a);
	int i = s;
	for (i = s; i < l - 1; i++) {
		if (b[i - s] != '\0') {
			a[i] = b[i - s];
		} else {
			a[i] = '\0';
			break;
		}
	}
	a[i] = '\0';
}

void scp(char a[], char b[], int l)
{
	int i = 0;
	for (i = 0; i < l - 1; i++) {
		if (b[i] != '\0') {
			a[i] = b[i];
		} else {
			a[i] = '\0';
      break;
		}
	}
	a[i] = '\0';
}

int scmp(char a[], char b[])
{
	int i = 0;
	int l = slen(a);
	for (i = 0; i < l - 1; i++) {
		if (a[i] != b[i]) {
			return 1;
		}
	}
	return 0;
}

/*
 * Create and open index file
 */
FILE *open_index(char *fname)
{
	FILE *index;
	char idx[URLLEN];
	scp(idx, DESTDIR, URLLEN);
	sct(idx, "/", URLLEN);
	sct(idx, fname, URLLEN);

	index = fopen(idx, "w");

	if (index == NULL) {
		printf("Failed to open %s\n", fname);
		exit(EXIT_FAILURE);
	}

	return index;
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

	/*
	 * body
	 */
	fprintf(page, "<!-- Generated static page, don't edit this -->\n");
	fprintf(page, "<body>\n");
	fprintf(page, "<header><a href='%s/index.html'>Home</a>\n", URL);

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
FILE *inject_contents(FILE * body, FILE *in)
{
	char s[URLLEN];

	while (fgets(s, URLLEN, in) != NULL) {
		fprintf(body, "%s", s);
	}

	return body;
}

/*
 * Add a link to index file
 */
int add_to_index(FILE * index, char *link, char *name)
{

	char a[URLLEN];

	scp(a, "<div><a href='", URLLEN);
	sct(a, URL, URLLEN);
	sct(a, link, URLLEN);
	sct(a, "'>", URLLEN);
	sct(a, name, URLLEN);
	sct(a, "</a></div>\n", URLLEN);
	fprintf(index, "%s", a);
	return 0;
}

/*
 * Build page from source
 */
int build_page(char *in, char *out)
{
  FILE *cont = fopen(in, "r");
	FILE *page = fopen(out, "w");

  if (cont == NULL) {
	printf("Couldn't open %s\n", in);
    return 1;
  }
  
  if (page == NULL) {
	printf("Couldn't open %s\n", out);
    return 1;
	}
  
  page = inject_foot(inject_contents(inject_head(page), cont));

  fclose(cont);
  fclose(page);
  
	return 0;
}

/*
 * Build all pages in directory
 */
int build_pages(char *dirname)
{
	int builderr;
	char idx[URLLEN];
	char outidx[URLLEN];
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
		if (scmp(dir->d_name, ".") && scmp(dir->d_name, "..")) {
			printf("Building %s - ", dir->d_name);

			scp(idx, dirname, URLLEN);
			sct(idx, "/", URLLEN);
			sct(idx, dir->d_name, URLLEN);

			scp(outidx, DESTDIR, URLLEN);
			sct(outidx, dir->d_name, URLLEN);

			builderr = build_page(idx, outidx);

			if (builderr == 0) {
				add_to_index(index, dir->d_name, dir->d_name);
			  printf("success\n");
      } else {
        printf("fail\n");
      }
		}

	}
	free(dir);
	free(dirlist);

	fclose(inject_foot(index));

	return 0;
}

int main(int argc, char *argv[])
{
	char fullsrc[URLLEN];
	int err;

	scp(fullsrc, SRCDIR, URLLEN);
	
  	err = build_pages(fullsrc);

	return err;

}
