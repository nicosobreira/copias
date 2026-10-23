#!/usr/bin/env bash

set -eou pipefail

SCRIPT_NAME="$0"

# ── Configuração ────────────────────────────────────────────────
PROJECT_NAME="project-name"
PROJECT_DIR="project"
BUILD_DIR_DEBUG="build/debug"

BINARY_DEBUG="${BUILD_DIR_DEBUG}/${PROJECT_DIR}/${PROJECT_NAME}"

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
    local preset="$1"

    require cmake

    log "Configuring preset '${preset}'..."
    cmake --workflow "$preset"
}

run_debug() {
    local -a args
    args=("$@")

    require gdb

    run_workflow "dev"

    [[ -x "$BINARY_DEBUG" ]] || die "Binary not found: $BINARY_DEBUG"

    local port
    port=$(shuf -i 2000-65000 -n 1)

    if [[ -n "${TMUX-}" ]]; then
        require gdbserver

        log "Init gdbserver in port ${port}..."
        tmux split-window -h -p 50 \
            "exec gdbserver :${port} '${BINARY_DEBUG}' ${args[*]}"

        sleep 0.2
        tmux select-pane -t left

        gdb --tui -q \
            -ex "file ${BINARY_DEBUG}" \
            -ex "target remote :${port}" \

        exit 0
    else
        log "TMUX not detected - simple gui mode..."
        gdb --tui -q --args "$BINARY_DEBUG" "${args[@]}"
    fi
}

run() {
    local -a args
    args=("$@")
    run_workflow "dev"

    [[ -x "$BINARY_DEBUG" ]] || die "Binary not found: $BINARY_DEBUG"

    log "Executing ${BINARY_DEBUG}..."

    exec "$BINARY_DEBUG" "${args[@]}"
}

usage() {
    cat <<EOF
Usage: ${SCRIPT_NAME} [mode]

Modes:
  run     | r    Build and execute  (padrão)
  build   | b    Just build debug
  debug   | d    Build debug and run gdb

EOF
}

# ── Main ─────────────────────────────────────────────────────────
MODE="${1-run}"

shift
case "$MODE" in
    debug   | d) log "Debug";    run_debug "$@"   ;;
    build   | b) log "Build";    run_workflow "dev" ;;
    run     | r) log "Run";      run "$@"        ;;
    help    | h) usage ;;
    *) err "Unknown mode: '${MODE}'"; usage; exit 1 ;;
esac
