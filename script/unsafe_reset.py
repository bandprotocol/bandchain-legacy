import subprocess
import toml
import shutil
from pathlib import Path
import util


def clear():
    subprocess.call([util.tendermint_bin, 'unsafe_reset_all',
                     '--home', util.tendermint_home_directory])

    band_home_dir = util.band_home_directory
    band_config = toml.load(band_home_dir + 'config/config.toml')
    if (Path(band_home_dir + band_config['db_path']).is_dir()):
        shutil.rmtree(band_home_dir + band_config['db_path'])
