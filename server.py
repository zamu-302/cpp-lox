import os
import subprocess
import tempfile
from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

LOX_BINARY = "./build/cpp-lox"
PORT = 3000
TIMEOUT_S = 5


# Serve frontend
@app.route("/")
def index():
    return send_from_directory(".", "index.html")


@app.post("/run")
def run():
    data = request.get_json(silent=True) or {}
    code = data.get("code", "")

    if not isinstance(code, str):
        return jsonify({"error": "No code provided."}), 400

    # Write to temp file
    with tempfile.NamedTemporaryFile(mode="w", suffix=".lox", delete=False) as f:
        f.write(code)
        tmp = f.name

    try:
        result = subprocess.run(
            [LOX_BINARY, tmp],
            capture_output=True,
            text=True,
            timeout=TIMEOUT_S,
            check=False,
        )
        return jsonify(
            {
                "stdout": result.stdout,
                "stderr": result.stderr,
                "exitCode": result.returncode,
            }
        )
    except subprocess.TimeoutExpired:
        return jsonify(
            {
                "stdout": "",
                "stderr": f"[timeout] Execution exceeded {TIMEOUT_S}s limit.",
                "exitCode": 1,
            }
        )
    except FileNotFoundError:
        return jsonify(
            {
                "stdout": "",
                "stderr": f"Binary not found: {LOX_BINARY}",
                "exitCode": 1,
            }
        )
    finally:
        os.unlink(tmp)


if __name__ == "__main__":
    print("\n  Lox Playground server running")
    print(f"  http://localhost:{PORT}\n")
    print(f"  Binary: {LOX_BINARY}")
    if not os.path.exists(LOX_BINARY):
        print(f"\n  ⚠  Binary not found at {LOX_BINARY}")
        print("  Compile your interpreter and place lox here, or update LOX_BINARY.\n")
    app.run(port=PORT, debug=False)
