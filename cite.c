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
char pd_name[URLLEN];
FILE *fidx;
/* 
 * Forward declarations
 */
FILE *inject_head(FILE * page);
FILE *inject_foot(FILE * page);
FILE *inject_contents(FILE * body, FILE * in);
int make_page(char *fname, char *path);
int make_dir(char *dname, char *path);
int get_relpath(char *relpath, char *path);
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
	fclose(inject_foot(inject_contents(inject_head(fout), fin)));
	fclose(fin);
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
 * Returns 0 if the path is to be included in indexes
 */
int is_html_dir(char *d) {
    if (scmp(d, ".") && scmp(d, "..") && scmp(d, "css")) {
        return 0;
    } else {
        return -1;
    }
}

void add_to_index(FILE *fidx, struct dirent *dir, char *path, int depth)
{
    char url[URLLEN];
    char name[URLLEN];
    char relpath[URLLEN];

    get_relpath(relpath, path);

    scp(name, dir->d_name, URLLEN);
    sr(name, '_', ' ');
    slcut(name, '.');
   
    scp(url, URL, URLLEN);
    sct(url, relpath, URLLEN);
    if (scmp(relpath, "")) {
    sct(url, "/", URLLEN);

    }
    sct(url, dir->d_name, URLLEN);


    if (dir->d_type == DT_DIR ) {
        if (is_html_dir(dir->d_name) == 0) {
            sct(url, "/index.html", URLLEN);
            fprintf(fidx, "<h%d><a class='dir-link' href='%s'>%s</a></h%d>\n", depth, url, name, depth);
        }
    } else if (dir->d_type == DT_REG && scmp(dir->d_name, "index.html")) {
        fprintf(fidx, "<div><a href='%s'>%s</a></div>\n", url, name);
    }
}

int get_index_links(char *path, FILE *fidx, int depth)
{
    int m, n;
    struct dirent **dirlist;
    struct dirent *dir;
    char ppd_name[URLLEN];

    depth++;
    n = scandir(path, &dirlist, NULL, alphasort);
    m = 0;

    if (n == -1) {
        return -1;
    }

    while (m < n) {
        if (ALPHASORTFILES == 0) {
	    dir = dirlist[n-1];
	} else {
	    dir = dirlist[m];
	}
        if (dir->d_type == DT_REG) {
            add_to_index(fidx, dir, path, depth);
        }
	if (ALPHASORTFILES == 0) {
	    n--;
	} else {
	    m++;
	}
    }

    n = scandir(path, &dirlist, NULL, alphasort);
    m = 0;

    while (m < n) {
	if (ALPHASORTDIRS == 0) {
	    dir = dirlist[n-1];
	} else {
	    dir = dirlist[m];
	}

        if (dir->d_type == DT_DIR && is_html_dir(dir->d_name) == 0) {
            add_to_index(fidx, dir, path, depth);

            scp(ppd_name, pd_name, URLLEN);
            set_pd_name(dir->d_name, path);
            get_index_links(pd_name, fidx, depth);
            scp(pd_name, ppd_name, URLLEN);
        }

	if (ALPHASORTDIRS == 0) {
	    n--;
	} else {
	    m++;
	}
    }

    depth--;

    return 0;
}

int generate_index_file(char *path, char *idx)
{
    FILE *fidx;
    int err;
    int depth = 1;
    fidx = inject_head(fopen(idx, "w"));

    err = get_index_links(path, fidx, depth);
    fclose(inject_foot(fidx));
    return err;
}

int get_relpath(char *relpath, char *path) { 
    int i = slen(SRCDIR) + 1;
    while (i < slen(path)) {
        relpath[i - slen(SRCDIR) - 1] = path[i];
        i++;
    }
    relpath[slen(path) - slen(SRCDIR) - 1] = '\0';
    return 0;
}

/*
 * Make a page from source directory to destination
 */
int make_page(char *fname, char *path)
{
	char srcurl[URLLEN];
	char desturl[URLLEN];
	char ppd_name[URLLEN];
	int err;

	scp(ppd_name, pd_name, URLLEN);
	set_pd_name(fname, path);

	scp(srcurl, SRCDIR, URLLEN);
	sct(srcurl, pd_name, URLLEN);

	scp(desturl, DESTDIR, URLLEN);
	sct(desturl, pd_name, URLLEN);

	err = inject_page(srcurl, desturl);
	scp(pd_name, ppd_name, URLLEN);
	
    return err;
}

/*
 * Make all pages in directory and add a header to index
 */
int make_dir(char *d_name, char *path)
{
	char ppd_name[URLLEN];
    char index[URLLEN];
    char fp[URLLEN];

	scp(ppd_name, pd_name, URLLEN);
	set_pd_name(d_name, path);
    scp(fp, DESTDIR, URLLEN);
    sct(fp, pd_name, URLLEN);

    /* Take reference of current path */

    scp(index, fp, URLLEN);
    sct(index, "/index.html", URLLEN);
    sf_mkdir(pd_name);
	build_pages(pd_name);
    
    generate_index_file(fp, index);
    
    /* Go back to current path */
	scp(pd_name, ppd_name, URLLEN);
	return 0;
}

/*
 * Set path + file name to pass to build functions
 */
void set_pd_name(char *d_name, char *path)
{
	scp(pd_name, path, URLLEN);
	if (scmp(path, SRCDIR) && scmp(path, DESTDIR)) {
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
	int n;

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
		} else if (dir->d_type == DT_DIR) {
			if (is_html_dir(dir->d_name) == 0) {
                make_dir(dir->d_name, path);
			}
		}
	}

	free(dirlist);
	free(dir);
}

int main(void)
{
	char idx[URLLEN];
    scp(idx, DESTDIR, URLLEN);
    sct(idx, "index.html", URLLEN); 
	build_pages("");

    generate_index_file(DESTDIR, idx);
	return 0;
}

