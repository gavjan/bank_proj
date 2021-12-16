#!/bin/bash

mkdir /credits /deposits

# Create Admin User
useradd --no-create-home admin

# Set Admin passwords to 1234
echo "root:1234" | chpasswd
echo "admin:1234" | chpasswd

# Make groups
groupadd officers
groupadd clients


# Set folder owner and group
chown -R admin:officers /credits /deposits

# Set permissions for folders
chmod -R 777 /credits /deposits # rwxrwxrwx

# Set Default Permissions for new files
chmod g+s /credits /deposits
setfacl -d -m u::rwx /credits /deposits
setfacl -d -m g::rwx /credits /deposits
setfacl -d -m o::x /credits /deposits

# Add admin user to sudoers. (-sudo +wheel for CentOS based Linux distros)
usermod -aG sudo admin

# Allow officer application to chown
setcap 'cap_chown=eip' /usr/bin/admin_app

exec_user() {
	user="$1"; role="$2"; name="$3"

	if [ "$role" = "officer" ]; then

		mkdir /home/"$user"

		# Create User
		useradd -d /home/"$user" "$user"

		chown -R "$user:$user" /home/"$user"

		# Set default shell
		chsh -s /usr/bin/admin_shell "$user"

		# Add to Group
		usermod -a -G officers "$user"

		echo -e "\n$user ALL = (ALL) ALL\n" >> /etc/sudoers
		echo -e "\n$user ALL = (root) NOPASSWD: /usr/bin/admin_app, /usr/bin/admin_auth\n" >> /etc/sudoers
	else
		# Create User
		useradd --no-create-home "$user"

		# Set default shell
		chsh -s /usr/bin/empty_shell "$user"


		# Add to Group
		usermod -a -G clients "$user"
	fi

	# Change Name and Surname
	chfn -f "$name" "$user" 2>/dev/null

	# Set passwords to 1234
	echo "$user:1234" | chpasswd
}

# Parse user
regex='^(\w+)\s+(\w+)\s+(.+)$'
while read p; do
	if [[ $p =~ $regex ]]; then
        user="${BASH_REMATCH[1]}"
        role="${BASH_REMATCH[2]}"
        name="${BASH_REMATCH[3]}"
    else
        echo "'$p' has a wrong format"
    fi

	

	exec_user "$user" "$role" "$name"
done <uzytkownicy.txt
echo "---Default password is 1234---"



