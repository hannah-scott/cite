#define _GNU_SOURCE
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "config.h"
#include "sops.h"

/*
 * Define globals
 */
FILE *fidx;
int header_depth = 1;
char pd_name[URLLEN];

/* 
 * Forward declarations
 */
FILE *inject_head(FILE * page);
FILE *inject_foot(FILE * page);
FILE *inject_contents(FILE * body, FILE * in);
int make_page(char *fname, char *path);
int make_dir(char *dname, char *path);
void sf_mkdir(char *dirname);
void set_pd_name(char *d_name, char *path);
void build_pages(char *path);

/*
 * Inject head and header text into file
 */
FILE *inject_head(FILE * page)
{
	fprintf(page, "<!DOCTYPE html>\n");
	fprintf(page, "<html>\n");
	fprintf(page, "<head>\n");
	fprintf(page, "<title>%s</title>\n", TITLE);
	fprintf(page, "<link rel='stylesheet' href='%s%s'>\n", URL, CSS);
	fprintf(page,
		"<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
	fprintf(page, "</head>\n");
	fprintf(page, "<!-- Generated static page, don't edit this -->\n");
	fprintf(page, "<body>\n");
	fprintf(page, "<header><a href='%sindex.html'><h1>%s</h1></a></header>\n", URL, TITLE);

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
void add_to_index(char *name, char *link)
{
	char n[URLLEN];
	scp(n, name, URLLEN);
	sr(n, '_', ' ');
	slcut(n, '.');
	fprintf(fidx, "<div>\n<a href='%s%s'>%s</a>\n</div>\n", URL, link, n);
}

/*
 * Build page from source
 */
int inject_page(char *in, char *out)
{
	FILE *fin, *fout;

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

/*
 * Build a directory if it doesn't exist
 */
void sf_mkdir(char *dirname)
{
	char dd[URLLEN];
	struct stat s;

	scp(dd, DESTDIR, URLLEN);
	sct(dd, dirname, URLLEN);

	if (stat(dd, &s) == -1) {
		mkdir(dd, 0700);
	}
}

/*
 * Make a page from source directory to destination
 */
int make_page(char *fname, char *path)
{
	char srcurl[URLLEN];
	char desturl[URLLEN];
	char ppd_name[URLLEN];
	int err = 0;

	scp(ppd_name, pd_name, URLLEN);
	set_pd_name(fname, path);

	scp(srcurl, SRCDIR, URLLEN);
	sct(srcurl, pd_name, URLLEN);

	scp(desturl, DESTDIR, URLLEN);
	sct(desturl, pd_name, URLLEN);

	err = inject_page(srcurl, desturl);

	if (err == 0) {
		add_to_index(fname, pd_name);
	}

	scp(pd_name, ppd_name, URLLEN);
	return err;
}

/*
 * Make all pages in directory and add a header to index
 */
int make_dir(char *d_name, char *path)
{
	char d_header[URLLEN];
	char ppd_name[URLLEN];

	header_depth++;

	scp(ppd_name, pd_name, URLLEN);
	set_pd_name(d_name, path);
	sf_mkdir(pd_name);

	scp(d_header, d_name, URLLEN);
	sr(d_header, '_', ' ');
	slcut(d_header, '.');

	fprintf(fidx, "<h%d>%s</h%d>\n", header_depth, d_header, header_depth);
	build_pages(pd_name);

	header_depth--;
	scp(pd_name, ppd_name, URLLEN);
	return 0;
}

/*
 * Set path + file name to pass to build functions
 */
void set_pd_name(char *d_name, char *path)
{
	scp(pd_name, path, URLLEN);
	if (scmp(path, SRCDIR)) {
		sct(pd_name, "/", URLLEN);
	}
	sct(pd_name, d_name, URLLEN);
}

/*
 * Build all pages and subdirectories at path
 */
void build_pages(char *path)
{
	struct dirent **dirlist;
	struct dirent *dir;
	char fullpath[URLLEN];
	int m, n;

	scp(fullpath, SRCDIR, URLLEN);
	sct(fullpath, path, URLLEN);

	n = scandir(fullpath, &dirlist, NULL, alphasort);
	if (n == -1) {
		printf("ERROR: %s\n", fullpath);
		exit(EXIT_FAILURE);
	}

	while (n--) {
		dir = dirlist[n];

		if (dir->d_type == DT_REG) {
			make_page(dir->d_name, path);
		}
	}

	n = scandir(fullpath, &dirlist, NULL, alphasort);
	m = 0;

	while (m < n) {
		dir = dirlist[m];

		if (dir->d_type == DT_DIR) {
			if (scmp(dir->d_name, ".") && scmp(dir->d_name, "..")) {
				make_dir(dir->d_name, path);
			}
		}
		m++;
	}

	free(dirlist);
	free(dir);
}

int main(void)
{
	char idx[URLLEN];
	scp(idx, DESTDIR, URLLEN);
	sct(idx, "index.html", URLLEN);

	fidx = fopen(idx, "w");
	if (fidx == NULL) {
		printf("Couldn't create index.html\n");
		return 1;
	}

	fidx = inject_head(fidx);
	build_pages("");
	fclose(inject_foot(fidx));

	return 0;
}
