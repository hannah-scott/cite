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

void slcut(char a[], char d)
{
	int i = 0;
	int l = slen(a);

	for (i = 0; i < l - 1; i++) {
		if (a[i] != '\0') {
			if (a[i] == d) {
				a[i] = '\0';
				break;
			}
		}
	}
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

void sr(char s[], char a, char b)
{
	int i = 0;
	int l = slen(s);
	
	for (i = 0; i < l - 1; i++) {
		if (s[i] == a) {
			s[i] = b;
		}
	}
}
