from flask import Flask, render_template, request, session, redirect, url_for, send_from_directory
from flask_login import LoginManager, login_user, login_required, logout_user, current_user
from utils import assert_user, get_user, User, get_login_message, get_user_links, is_safe_url, get_content
import os

app = Flask(__name__)
app.config['SECRET_KEY'] = 'thisissecret'

login_manager = LoginManager()
login_manager.init_app(app)
login_manager.login_view = 'login'
login_manager.login_message = "Please Log in first"


@login_manager.user_loader
def load_user(user_id):
    return get_user(user_id)


@app.route('/')
def index():
    return redirect("/home")


@app.route('/login')
def login():
    if current_user.is_authenticated:
        return redirect("/home")
    session["next"] = request.args.get("next")
    login_message = get_login_message(request.args.get("login_message"))
    return render_template("login.html", login_message=login_message)


@app.route('/logmein', methods=["POST"])
def logmein():
    username = request.form["username"]
    password = request.form["password"]
    user = assert_user(username, password)
    if not user:
        return redirect(url_for("login", login_message="WRONG"))
    login_user(user)

    if "next" in session and session["next"] and is_safe_url(session["next"]):
        return redirect(session["next"])

    return redirect("/home")


@app.route('/logout')
@login_required
def logout():
    logout_user()
    return redirect(url_for("login", login_message="LOGGED_OUT"))


@app.route('/home')
@login_required
def home():
    credits, deposits = get_user_links(current_user.username)
    return render_template("home.html", deposits=deposits, credits=credits)


@app.route('/view')
@login_required
def view():
    type = request.args.get("type")
    num = request.args.get("num")

    return get_content(type, num, current_user.username)


@app.route('/favicon.ico')
def favicon():
    return send_from_directory(os.path.join(app.root_path, 'static'),
                               'favicon.ico', mimetype='image/vnd.microsoft.icon')


if __name__ == '__main__':
    # app.run(host='0.0.0.0', port=80, debug=False, ssl_context=('cert.pem', 'key.pem'))
    app.run(host='0.0.0.0', port=80, debug=False)
