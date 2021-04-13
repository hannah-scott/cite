#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "config.h"
#include "sops.h"

/*
 * Inject head and header text into file
 */
FILE *inject_head(FILE * page)
{
	fprintf(page, "<html>\n");
	fprintf(page, "<head>\n");
	fprintf(page, "<title>%s</title>\n", TITLE);
	fprintf(page, "<link rel='stylesheet' href='%s%s'>\n", URL, CSS);
	fprintf(page, "</head>\n");
	fprintf(page, "<!-- Generated static page, don't edit this -->\n");

	fprintf(page, "<body>\n");
	fprintf(page, "<header><a href='%sindex.html'>Home</a>\n", URL);
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
FILE *inject_contents(FILE * body, FILE * in)
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
void add_to_index(FILE * index, char *name, char *link)
{
	char n[URLLEN];
	scp(n, name, URLLEN);
	sr(n, '_', ' ');
	slcut(n, '.');
	fprintf(index, "<div>\n<a href='%s%s'>%s</a>\n</div>\n", URL, link, n);
}

/*
 * Build page from source
 */
int make_page(char *in, char *out)
{
	FILE *fin;
	FILE *fout;

	if ((fin = fopen(in, "r")) == NULL) {
		printf("Couldn't open %s\n", in);
		return 1;
	}

	if ((fout = fopen(out, "w")) == NULL) {
		printf("Couldn't create %s\n", out);
		return 1;
	}

	fout = inject_foot(inject_contents(inject_head(fout), fin));

	fclose(fin);
	fclose(fout);

	return 0;
}

void build_pages(char *path, FILE * fidx)
{
	struct dirent **dirlist;
	struct dirent *dir;
	char fullpath[URLLEN];
	char srcurl[URLLEN];
	char desturl[URLLEN];
	char pd_name[URLLEN];
	int n, berr;

	scp(fullpath, SRCDIR, URLLEN);
	sct(fullpath, path, URLLEN);

	n = scandir(fullpath, &dirlist, NULL, alphasort);

	if (n == -1) {
		perror("scandir");
		exit(EXIT_FAILURE);
	}

	while (n--) {
		dir = dirlist[n];

		scp(pd_name, path, URLLEN);
		if (scmp(path, SRCDIR)) {
			sct(pd_name, "/", URLLEN);
		}
		sct(pd_name, dir->d_name, URLLEN);

		if (dir->d_type == DT_DIR) {
			if (scmp(dir->d_name, ".") && scmp(dir->d_name, "..")) {
				/* build pages in subdirectory */
				build_pages(pd_name, fidx);
			}
		} else if (dir->d_type == DT_REG) {
			/* cat together link to file */
			scp(srcurl, SRCDIR, URLLEN);
			sct(srcurl, pd_name, URLLEN);

			/* cat together destination */
			scp(desturl, DESTDIR, URLLEN);
			sct(desturl, pd_name, URLLEN);

			berr = make_page(srcurl, desturl);

			/* cat together link for index.html */
			if (berr == 0) {
				add_to_index(fidx, dir->d_name, pd_name);
			}
		}
	}

	free(dirlist);
	free(dir);
}

int main(void)
{
	char idx[URLLEN];
	FILE *fidx;

	scp(idx, DESTDIR, URLLEN);
	sct(idx, "index.html", URLLEN);

	fidx = fopen(idx, "w");

	if (fidx == NULL) {
		printf("Couldn't create index.html\n");
		return 1;
	}

	fidx = inject_head(fidx);

	build_pages("", fidx);

	fclose(inject_foot(fidx));

	return 0;

}
