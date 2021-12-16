import os
from urllib.parse import urlparse, urljoin

import pexpect as pexpect
from flask import request
from flask_login import UserMixin
import pwd

WRONG_ATTEMPT_TIMEOUT = 0.5


def get_login_message(message):
    if message == "LOGGED_OUT":
        return "You are now logged out"
    if message == "WRONG":
        return "Username or Password are incorrect"


class User(UserMixin):
    def __init__(self, username, _id, fullname):
        self.username = username
        self.id = _id
        self.fullname = fullname

    id = 0
    username = ""
    fullname = ""


def is_safe_url(target):
    ref_url = urlparse(request.host_url)
    test_url = urlparse(urljoin(request.host_url, target))
    return test_url.scheme in ('http', 'https') and \
           ref_url.netloc == test_url.netloc


def assert_user(username, password):
    print(f"{username} tried to log-in with password: {password}")

    child = pexpect.spawn(f'su - {username}')
    if child.expect(['Password: ', pexpect.EOF]) != 0:
        return None

    child.sendline(password)
    ret = child.expect(
        ["logged_in", "su: Authentication failure", pexpect.EOF, pexpect.TIMEOUT],
        timeout=WRONG_ATTEMPT_TIMEOUT
    )
    if ret == 0:
        return get_user(pwd.getpwnam(username).pw_uid)
    else:
        return None


def get_user(_id):
    unix_user = pwd.getpwuid(int(_id))
    name = unix_user.pw_name
    fullname = unix_user.pw_gecos
    return User(name, _id, fullname)


def find_owner(filename):
    return pwd.getpwuid(os.stat(filename).st_uid).pw_name


def ls(directory):
    path = os.path.abspath(directory)
    return [entry.path for entry in os.scandir(path) if entry.is_file()]


def get_user_files(user, where):
    files = []
    for x in ls(where):
        if find_owner(x) == user:
            files.append(x[len(where) + 1:].replace(".txt", ""))
    return files


def get_user_links(user):
    credits = get_user_files(user, "/credits")
    deposits = get_user_files(user, "/deposits")
    return credits, deposits


def get_content(type, num, user):
    back_button = '<a href="home" class="button"><button>←Back</button></a><br>'
    if type not in ["credit", "deposit"]:
        return back_button + "<h1>Something Went Wrong"

    filename = f"/{type}s/{num}.txt"
    if not os.path.exists(filename):
        return back_button + f"<h1>No such {type}"
    if find_owner(filename) != user:
        return back_button + f"<h1>You don't have access to this {type}"

    f = open(filename, "r")
    content = f.read()
    f.close()

    return back_button + content.replace("\n", "<br>")
