sudo tee -a /etc/nginx/conf.d/gc401929.zadanie1.bsk.conf > /dev/null <<EOT
server {
    listen gc401929.zadanie1.bsk:80;
    server_name gc401929.zadanie1.bsk;

    location / {
        proxy_pass http://127.0.0.1:5000/;
    }
    location = /favicon.ico {
        log_not_found off;
    }
}

EOT


sudo semanage port -m --type http_port_t --proto tcp 5000
sudo setsebool -P httpd_can_network_connect true
sudo systemctl restart nginx