#!/bin/bash

# ----------------------------
# Container-specific settings
# ----------------------------

MODE="undetected"
if [[ -z "$CPP_DOCKER" ]]; then

    THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]:-${(%):-%x}}" )" >/dev/null 2>&1 && pwd )"
    PROJ_ROOT="${THIS_DIR}/.."
    VENV_PATH="${PROJ_ROOT}/venv-bm"

    # Normal terminal
    MODE="terminal"
else
    # Running inside the container, we know the project root
    PROJ_ROOT="/code/cpp"
    VENV_PATH="/code/cpp/venv"

    # Use containerised redis
    alias redis-cli="redis-cli -h redis"

    MODE="container"
fi

pushd ${PROJ_ROOT}>>/dev/null

# ----------------------------
# Virtualenv
# ----------------------------

if [ ! -d ${VENV_PATH} ]; then
    ./bin/create_venv.sh
fi

export VIRTUAL_ENV_DISABLE_PROMPT=1
source ${VENV_PATH}/bin/activate

# ----------------------------
# Invoke tab-completion
# (http://docs.pyinvoke.org/en/stable/invoke.html#shell-tab-completion)
# ----------------------------

_complete_invoke() {
    local candidates
    candidates=`invoke --complete -- ${COMP_WORDS[*]}`
    COMPREPLY=( $(compgen -W "${candidates}" -- $2) )
}

# If running from zsh, run autoload for tab completion
if [ "$(ps -o comm= -p $$)" = "zsh" ]; then
    autoload bashcompinit
    bashcompinit
fi
complete -F _complete_invoke -o default invoke inv

# ----------------------------
# Environment vars
# ----------------------------

VERSION_FILE=${PROJ_ROOT}/VERSION
export CPP_ROOT=${PROJ_ROOT}
export CPP_VERSION=$(cat ${VERSION_FILE})

export PS1="(cpp) $PS1"

# -----------------------------
# Splash
# -----------------------------

echo ""
echo "----------------------------------"
echo "CPP CLI"
echo "Version: ${CPP_VERSION}"
echo "Project root: ${CPP_ROOT}"
echo "Mode: ${MODE}"
echo "----------------------------------"
echo ""

popd >> /dev/null
