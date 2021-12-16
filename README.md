# Duże Zadanie 1
- Wszystkie hasła są domyślne `1234`
## Docker
Urochomienie
```shell
docker build .
docker run -it -p 5000:8080 -p 2200:22 $(docker build -q .)
```
- W katalogu `sh` znajdują się:
  - `admin_shell.sh` domyślny shell pracownika (uruchamia PAM, po którym aplikację)
  - `empty_shell.sh` domyślny shell klienta (nie daję klientom dostępu do shell)
  - `init.sh` uruchamia SSH Server oraz Aplikację WWW
  - `parse.sh` parsuje zawartość pliku /c/uzytkownicy.txt

## Aplikacja WWW
- Kod źródłowy w kataloku `www`. 
- Domyślne dostęptna na `localhost:5000` ale można urochomić `/sh/make_host.sh` aby dodać host 


## Aplikacja Pracownika
- `ssh -p 2200 <officer>@localhost` żeby uruchomić
- Kod źródłowy w kataloku `c`
- `make valgrind` do badania bezbłędności oprogramowania


## Ściana ogniowa
- zasady ścainy ogniowej są w pliku `sh/firewall.sh`