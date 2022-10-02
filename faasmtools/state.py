from os.path import basename


# from faasmcli.util.upload_util import upload_file_to_s3, curl_file
# from faasmcli.util.upload_util import curl_file

import subprocess


def curl_file(url, file_path, headers=None, quiet=False):
    cmd = ["curl", "-X", "PUT", url, "-T", file_path]

    headers = headers if headers else {}
    for key, value in headers.items():
        cmd.append('-H "{}: {}"'.format(key, value))

    cmd = " ".join(cmd)

    if not quiet:
        print(cmd)

    res = subprocess.call(cmd, shell=True)

    if res == 0 and not quiet:
        print("Successfully PUT file {} to {}".format(file_path, url))
    elif res != 0:
        raise RuntimeError(
            "Failed PUTting file {} to {}".format(file_path, url)
        )


def upload_binary_state(user, key, binary_file, host=None, s3_bucket=None):
    assert host or s3_bucket, "Must provide either host or S3 bucket"

    print("Uploading binary file at {} for user {}".format(binary_file, user))

    if s3_bucket:
        #        s3_key = "{}/{}".format(user, key)
        #        print(
        #            "Uploading matrix binary
        #             to S3 {} -> {}/{}".format(key, s3_bucket, s3_key)
        #        )
        #        upload_file_to_s3(binary_file, s3_bucket, s3_key)
        print("Uploading to S3 bucket currently not supported")

    else:
        url = "http://{}:8002/s/{}/{}".format(host, user, key)
        curl_file(url, binary_file)


def upload_shared_file(host, local_path, shared_path, quiet=False):
    url = "http://{}:8002/file/".format(host)

    local_filename = basename(local_path)
    print("Uploading {} to {}".format(local_filename, shared_path))

    curl_file(
        url,
        local_path,
        headers={
            "FilePath": shared_path,
        },
        quiet=quiet,
    )
