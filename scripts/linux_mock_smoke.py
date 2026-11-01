#!/usr/bin/env python3
"""Run the native stream stack against the local fixture server, without accounts."""
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import time
import urllib.request

ROOT = Path(__file__).resolve().parent.parent


def stop(process):
    if process.poll() is None:
        process.terminate()
        try:
            process.wait(timeout=10)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()


def main():
    with tempfile.TemporaryDirectory(prefix="lunarnx-mock-") as folder:
        directory = Path(folder)
        fixture = directory / "fixture.mp4"
        subprocess.run([
            "ffmpeg", "-hide_banner", "-loglevel", "error",
            "-f", "lavfi", "-i", "testsrc2=size=1280x720:rate=30",
            "-f", "lavfi", "-i", "sine=frequency=440:sample_rate=48000",
            "-t", "12", "-c:v", "libx264", "-preset", "ultrafast",
            "-tune", "zerolatency", "-profile:v", "baseline",
            "-pix_fmt", "yuv420p", "-g", "30", "-c:a", "aac",
            "-ar", "48000", "-ac", "2", str(fixture)
        ], check=True)
        # Use the host's LAN address for ICE while HTTP stays on loopback.
        addresses = subprocess.check_output(["hostname", "-I"], text=True).split()
        address = next(value for value in addresses if "." in value and not value.startswith("127."))
        environment = {**os.environ, "SDL_AUDIODRIVER": "dummy",
                       "LIBGL_ALWAYS_SOFTWARE": "1"}
        server_log = directory / "server.log"
        client_log = directory / "client.log"
        with server_log.open("w") as output:
            server = subprocess.Popen([
                sys.executable, str(ROOT / "tools/mock_xbox/mock_xbox_server.py"),
                "--video", str(fixture), "--host", "127.0.0.1",
                "--http-port", "18080", "--public-ip", address,
                "--cloud-titles", "0"
            ], cwd=directory, stdout=output, stderr=subprocess.STDOUT, env=environment)
            try:
                for attempt in range(100):
                    if server.poll() is not None:
                        raise RuntimeError("Mock server exited before readiness")
                    try:
                        with urllib.request.urlopen("http://127.0.0.1:18080/health", timeout=1):
                            break
                    except OSError:
                        time.sleep(0.1)
                else:
                    raise RuntimeError("Mock server readiness timed out")
                with client_log.open("w") as output:
                    client = subprocess.Popen([
                        "xvfb-run", "-a", str(ROOT / "build/linux/linux_mock_stream"),
                        "http://127.0.0.1:18080"
                    ], cwd=directory, stdout=output, stderr=subprocess.STDOUT,
                       env=environment, start_new_session=True)
                    try:
                        result = client.wait(timeout=90)
                    except subprocess.TimeoutExpired:
                        os.killpg(client.pid, signal.SIGKILL)
                        client.wait()
                        raise RuntimeError("Mock streaming timed out")
                if result:
                    raise RuntimeError(f"Mock streaming exited with {result}")
                print(client_log.read_text())
            except Exception:
                # Logs contain fixture traffic and dummy tokens only.
                print(server_log.read_text(), file=sys.stderr)
                diagnostic = directory / "lunarnx.log"
                if diagnostic.exists():
                    print(diagnostic.read_text(), file=sys.stderr)
                if client_log.exists():
                    print(client_log.read_text(), file=sys.stderr)
                raise
            finally:
                stop(server)


if __name__ == "__main__":
    main()
