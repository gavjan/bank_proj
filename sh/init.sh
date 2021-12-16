service ssh start
service --status-all
./parse.sh

# Run the server without root privileges
useradd --no-create-home www
usermod -a -G officers www

cd /app/www 
openssl req -x509 -newkey rsa:4096 -nodes -out cert.pem -keyout key.pem -days 365 -subj "/C=PL/ST=Mazowieckie/L=Warsaw/O=BSK-CA-2021/OU=alo/CN=172.17.0.2"
chown www:www /app/www/key.pem /app/www/cert.pem


su -c "python3 /app/www/app.py" - www
