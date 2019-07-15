#! /usr/bin/env python3

import tarfile
import shutil
import ssl

from os import environ as env
from platform import system
from subprocess import check_call, check_output
from argparse import ArgumentParser
from io import BytesIO
from urllib.request import Request, urlopen
from urllib.parse import quote, urlencode
from zipfile import ZipFile
from pathlib import Path


def define(key: str, value: str):
    return ['-D', f'{key}={value}']


def define_env(name: str):
    return define(name, env[name]) if name in env else []


def fetch_artifacts(project, reference, job):
    gitlab_host = 'https://git.imp.fu-berlin.de'
    project = quote(project, safe="")
    reference = quote(reference, safe="")
    params = urlencode([("job", job)], doseq=True)
    url = f'{gitlab_host}/api/v4/projects/{project}/jobs/artifacts/{reference}/download?{params}'
    headers = {'JOB-TOKEN': env['CI_JOB_TOKEN']}
    return ZipFile(
        BytesIO(
            urlopen(Request(url, headers=headers),
                    context=ssl._create_unverified_context()).read()))


def extract_cmake_package(artifacts, name):
    for filename in artifacts.namelist():
        if Path(filename).match(f'{name}-*.tar.xz'):
            with tarfile.open(fileobj=BytesIO(artifacts.read(filename))) as f:
                f.extractall('vendor')
            shutil.move(next(Path('vendor').glob(f'{name}-*/')),
                        f'vendor/{name}')


if system() == 'Windows':

    def setup_msvc():
        msvc_path = 'C:/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools/Common7/Tools'
        lines = check_output([
            'cmd', '/c', 'VsDevCmd.bat', '-arch=amd64',
            f'-vcvars_ver={env["VCVARS_VER"]}', '&', 'set'
        ],
                             cwd=msvc_path).decode('utf-8').splitlines()
        for line in lines:
            split = line.split('=')
            if len(split) != 2:
                continue
            key, value = split
            if key in env and env[key] == value:
                continue
            env[key] = value


def prepare(args):
    for name, project, reference, job in args.dependencies:
        with fetch_artifacts(project, reference, job) as artifacts:
            extract_cmake_package(artifacts, name)


def build(args):
    if system() == 'Windows':
        setup_msvc()

    command = ['cmake']
    command += ['-S', '.']
    command += ['-B', 'build']
    command += ['-G', 'Ninja']
    command += define('CMAKE_PREFIX_PATH', Path('vendor').resolve())
    command += define_env('CMAKE_BUILD_TYPE')
    command += define('CMAKE_SUPPRESS_REGENERATION', 'ON')
    command += define('CMAKE_SKIP_PACKAGE_ALL_DEPENDENCY', 'ON')

    if system() == 'Windows':
        command += define(
            'CMAKE_TOOLCHAIN_FILE',
            env['VCPKG_DIR'] + '/scripts/buildsystems/vcpkg.cmake')
        command += define('VCPKG_TARGET_TRIPLET', env['VCPKG_TRIPLET'])
        command += define('PACKAGE_WINDOWS_MSI', 'ON')
    elif system() == 'Linux':
        command += define('PACKAGE_TXZ', 'ON')
    check_call(command)

    command = ['ninja', '-C', 'build']
    check_call(command)


def package(args):
    command = ['ninja', '-C', 'build', 'package']
    check_call(command)


if __name__ == '__main__':
    parser = ArgumentParser()
    subparsers = parser.add_subparsers()

    prepare_parser = subparsers.add_parser('prepare')
    prepare_parser.set_defaults(task=prepare)
    prepare_parser.add_argument('--dependency',
                                dest='dependencies',
                                nargs=4,
                                action='append',
                                metavar=('PACKAGE', 'PROJECT', 'REFERENCE',
                                         'JOB'))

    build_parser = subparsers.add_parser('build')
    build_parser.set_defaults(task=build)

    package_parser = subparsers.add_parser('package')
    package_parser.set_defaults(task=package)

    args = parser.parse_args()
    args.task(args)
