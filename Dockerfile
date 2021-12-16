FROM debian:latest


WORKDIR /app

EXPOSE 8080 22

RUN apt-get update 
RUN apt-get install python3 openssh-server pip acl sudo libpam0g-dev libcap2-bin -y

COPY www/requirements.txt .
RUN pip install -r requirements.txt

RUN printf "\nPermitRootLogin Yes\n" >> /etc/ssh/sshd_config


COPY sh sh
RUN chmod +x /app/sh/*.sh
RUN mv /app/sh/empty_shell.sh /usr/bin/empty_shell
RUN mv /app/sh/admin_shell.sh /usr/bin/admin_shell

COPY c c
RUN make -C c clean
RUN make -C c && \
mv /app/c/app /usr/bin/admin_app && \
mv /app/c/auth /usr/bin/admin_auth

COPY . .
CMD cd sh && chmod +x init.sh && ./init.sh
