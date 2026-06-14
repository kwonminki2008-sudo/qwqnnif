#!/usr/bin/env bash
# Same-WiFi mobile test server (binds all interfaces, not just localhost)
PORT="${1:-8000}"
ROOT="$(cd "$(dirname "$0")" && pwd)"

IP=""
if command -v ip >/dev/null 2>&1; then
  IP=$(ip route get 1.1.1.1 2>/dev/null | awk '{for (i=1;i<=NF;i++) if ($i=="src") print $(i+1); exit}')
fi
if [ -z "$IP" ] && command -v hostname >/dev/null 2>&1; then
  IP=$(hostname -I 2>/dev/null | awk '{print $1}')
fi
if [ -z "$IP" ]; then
  IP="<your-computer-ip>"
fi

echo "Serving: $ROOT"
echo "Port:    $PORT (0.0.0.0 — reachable on LAN)"
echo ""
echo "Mobile:  http://${IP}:${PORT}/index.html"
echo "PC:      http://127.0.0.1:${PORT}/index.html"
echo ""
echo "Ctrl+C to stop"
cd "$ROOT"
exec python3 -m http.server "$PORT" --bind 0.0.0.0
