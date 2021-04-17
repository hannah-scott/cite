void make_dirs(char *dname, char *pd_name)
{
	char d_header[URLLEN];
	if (scmp(dname, ".") && scmp(dname, "..")) {
		sf_mkdir(pd_name);

		scp(d_header, dname, URLLEN);
		sr(d_header, '_', ' ');
		slcut(d_header, '.');

		header_depth++;

		fprintf(fidx, "<h%d>%s</h%d>\n",
				header_depth,
				d_header,
				header_depth);

		build_pages(pd_name);

		header_depth--;
	}
}
