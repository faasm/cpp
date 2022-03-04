from configparser import ConfigParser
from os.path import expanduser, join, exists

FAASM_INI_FILE = join(expanduser("~"), ".config", "faasm.ini")

DEFAULT_KNATIVE_HEADERS = {"Host": "faasm-worker.faasm.example.com"}

DEFAULT_INVOKE_HOST = "worker"
DEFAULT_UPLOAD_HOST = "upload"
DEFAULT_INVOKE_PORT = 8080
DEFAULT_UPLOAD_PORT = 8002


def faasm_config_exists():
    return exists(FAASM_INI_FILE)


def get_faasm_ini_value(section, key):
    if not exists(FAASM_INI_FILE):
        print("Expected to find faasm config at {}".format(FAASM_INI_FILE))
        raise RuntimeError("Did not find faasm config")

    config = ConfigParser()
    config.read(FAASM_INI_FILE)
    return config[section].get(key, "")


def get_faasm_upload_host_port():
    if not faasm_config_exists():
        return DEFAULT_UPLOAD_HOST, DEFAULT_UPLOAD_PORT

    host = get_faasm_ini_value("Faasm", "upload_host")
    port = get_faasm_ini_value("Faasm", "upload_port")

    return host, port


def get_faasm_invoke_host_port():
    if not faasm_config_exists():
        return DEFAULT_INVOKE_HOST, DEFAULT_INVOKE_PORT

    host = get_faasm_ini_value("Faasm", "invoke_host")
    port = get_faasm_ini_value("Faasm", "invoke_port")

    return host, port


def get_knative_headers():
    if not faasm_config_exists():
        return DEFAULT_KNATIVE_HEADERS

    knative_host = get_faasm_ini_value("Faasm", "knative_host")

    headers = {"Host": knative_host}

    return headers
