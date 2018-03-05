from flask import Flask, render_template

app = Flask(__name__)


@app.route('/')
def hello_world():
    return render_template('index.html', title='Nixie Control')


@app.route('/api')
def api():
    return 'this is nixie control api'


if __name__ == '__main__':
    app.run()
