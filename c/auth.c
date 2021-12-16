#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdbool.h>
#define curr_time() time(0)

time_t diff(time_t a, time_t b) {
	time_t max = a > b ? a : b;
	time_t min = a < b ? a : b;
	return max - min;
}
int pam_login() {
	static struct pam_conv login_conv = {
			misc_conv,               /* przykładowa funkcja konwersacji z libpam_misc */
			NULL                        /* ewentualne dane aplikacji (,,domknięcie'') */
	};

	pam_handle_t *pamh = NULL;
	int retval;
	char *username = NULL;

	retval = pam_start("login", username, &login_conv, &pamh);
	if (pamh == NULL || retval != PAM_SUCCESS) {
		fprintf(stderr, "Error when starting: %d\n", retval);
		return 1;
	}

	retval = pam_authenticate(pamh, 0);
	if (retval != PAM_SUCCESS) {
		fprintf(stderr, "Login Failed!\n");
		return 1;
	}
	time_t entered_time;
	printf("Please enter the current UNIX Time in seconds (`date +%%s`): ");
	scanf("%ld", &entered_time);
	pam_end(pamh, PAM_SUCCESS);

	return diff(entered_time, curr_time()) <= 15 ? 0 : 2;
}
int main() {
	return pam_login();
}