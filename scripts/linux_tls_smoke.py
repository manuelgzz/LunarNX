#!/usr/bin/env python3
"""Exercise the application's actual HTTP client against a local TLS server."""
import http.server
import os
from pathlib import Path
import ssl
import subprocess
import tempfile
import threading

ROOT = Path(__file__).resolve().parent.parent


class Handler(http.server.BaseHTTPRequestHandler):
    def do_POST(self):
        body = self.rfile.read(int(self.headers.get("Content-Length", "0")))
        self.send_response(200 if body == b"offline-probe" else 400)
        self.end_headers()
        self.wfile.write(b"ok")

    def log_message(self, *_args):
        pass


def certificate(folder, name):
    cert, key = folder / (name + ".pem"), folder / (name + ".key")
    subprocess.run(["openssl", "req", "-x509", "-newkey", "rsa:2048", "-nodes",
        "-sha256", "-days", "1", "-subj", "/CN=localhost",
        "-addext", "subjectAltName=IP:127.0.0.1",
        "-keyout", str(key), "-out", str(cert)], check=True,
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return cert, key


def main():
    with tempfile.TemporaryDirectory(prefix="lunarnx-tls-") as folder:
        folder = Path(folder)
        trusted, key = certificate(folder, "trusted")
        unrelated, _ = certificate(folder, "unrelated")
        server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Handler)
        context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        context.load_cert_chain(trusted, key)
        server.socket = context.wrap_socket(server.socket, server_side=True)
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        url = f"https://127.0.0.1:{server.server_port}/?private-query-marker"
        try:
            for bundle, expected in ((trusted, "success"), (unrelated, "failure")):
                environment = dict(os.environ, SSL_CERT_FILE=str(bundle),
                    XDG_CACHE_HOME=str(folder / "cache"), XDG_CONFIG_HOME=str(folder / "config"))
                subprocess.run([str(ROOT / "build/linux-ui/native_http_probe"),
                    url, expected], env=environment, check=True, timeout=15)
            log = (folder / "cache/lunarnx/lunarnx.log").read_text()
            assert "host=127.0.0.1" in log and "curl=60" in log
            assert "private-query-marker" not in log
        finally:
            server.shutdown()
            server.server_close()
            thread.join()
    print("PASS native HTTPS: trusted CA succeeds, untrusted CA is rejected, failure metadata excludes query")


if __name__ == "__main__":
    main()
