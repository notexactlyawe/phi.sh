from flask import Flask, render_template, request

app = Flask(__name__)

def uuid_in_db(uuid):
    if uuid == "96965dcd-1d6e-4758-8498-0a042c0134af":
        return True
    return False

@app.route("/")
def index():
    return render_template('index.html')

@app.route("/api/post", methods=['POST'])
def post_data():
    json = request.data
    print len(json)
    return "OK", 200

if __name__ == "__main__":
    app.run(host='0.0.0.0', debug=True)
