import subprocess
import util
import toml


def info(key):
    band_config = toml.load(util.band_home_directory + 'config/config.toml')
    info_bin = util.band_bin.split('/')
    info_bin[-1] = 'info'
    info_bin = '/'.join(info_bin)
    subprocess.check_call(
        [info_bin, '--db-path', util.band_home_directory + band_config['db_path'], '--key', key])
