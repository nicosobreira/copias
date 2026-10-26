#!/usr/bin/env bash

set -euo pipefail

SCRIPT_NAME="$(basename "$0")"

# Roda a partir da raiz do projeto (pai de tools/), de qualquer diretório
cd "$(dirname "${BASH_SOURCE[0]}")/.."

# ── Configuração ────────────────────────────────────────────────
PRESET="dev"
BINARY_DIR="build/debug/bin"

# ── Utilitários ─────────────────────────────────────────────────
log()  { echo -e "\e[1;34m[${SCRIPT_NAME}]\e[m $*"; }
err()  { echo -e "\e[1;31m[error]\e[m $*" >&2; }
die()  { err "$*"; exit 1; }

require() {
    for cmd in "$@"; do
        command -v "$cmd" >/dev/null || die "Dependency not found: '$cmd'"
    done
}

# ── Funções ──────────────────────────────────────────────────────
run_workflow() {
    require cmake
    log "Running workflow preset '${PRESET}'..."
    cmake --workflow --preset "$PRESET"
}

# Recebe o nome do binário e imprime o caminho completo
resolve_binary() {
    local name="${1-}"
    [[ -n "$name" ]] || { usage >&2; die "Missing binary name"; }

    local path="${BINARY_DIR}/${name}"
    [[ -x "$path" ]] || die "Binary not found: ${path}"
    echo "$path"
}

run_debug() {
    require gdb
    run_workflow

    local binary
    binary="$(resolve_binary "${1-}")"
    shift

    if [[ -n "${TMUX-}" ]]; then
        require gdbserver tmux

        local port cmd
        port=$(shuf -i 2000-65000 -n 1)
        # %q escapa cada argumento corretamente para o shell do tmux
        cmd=$(printf '%q ' gdbserver ":${port}" "$binary" "$@")

        log "Starting gdbserver on port ${port}..."
        tmux split-window -h -l 50% "exec ${cmd}"

        sleep 0.2
        tmux select-pane -t '{left}'

        gdb --tui -q "$binary" -ex "target remote :${port}"
    else
        log "TMUX not detected - running gdb directly..."
        gdb --tui -q --args "$binary" "$@"
    fi
}

run() {
    run_workflow

    local binary
    binary="$(resolve_binary "${1-}")"
    shift

    log "Executing ${binary}..."
    exec "$binary" "$@"
}

usage() {
    cat <<EOF
Usage: ${SCRIPT_NAME} <mode> [binary] [args...]

Modes:
  run   | r  <binary> [args...]   Build and execute
  debug | d  <binary> [args...]   Build and run under gdb
  build | b                       Just build

Binaries are looked up in ${BINARY_DIR}/.

EOF
}

# ── Main ─────────────────────────────────────────────────────────
if [[ $# -eq 0 ]]; then
    usage
    exit 1
fi

MODE="$1"
shift

case "$MODE" in
    run   | r) run "$@" ;;
    debug | d) run_debug "$@" ;;
    build | b) run_workflow ;;
    help  | h) usage ;;
    *) err "Unknown mode: '${MODE}'"; usage; exit 1 ;;
esac
