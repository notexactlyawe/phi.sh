from flask import Flask, render_template, request

app = Flask(__name__)

def uuid_in_db(uuid):
    if uuid == "96965dcd-1d6e-4758-8498-0a042c0134af":
        return True
    return False

@app.route("/")
def index():
    return render_template('index.html')

@app.route("/api/post/<uuid>", methods=['POST'])
def post_data(uuid):
    if not uuid_in_db(uuid):
        return "Not allowed", 403
    json = request.get_json(force=True)
    print json
    return "OK", 200

if __name__ == "__main__":
    app.run(debug=True)
