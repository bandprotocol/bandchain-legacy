import subprocess
import toml
from pathlib import Path

import util


def init():
    subprocess.check_call([util.tendermint_bin, 'init', '--home',
                           util.tendermint_home_directory])

    # Load data from config.toml
    tendermint_config = toml.load(
        util.tendermint_home_directory + 'config/config.toml')

    # Set variable need to config here
    tendermint_config['p2p']['persistent_peers'] = util.input_default(
        'Peer list', tendermint_config['p2p']['persistent_peers'])
    app_port = util.input_default(
        'Proxy App Port', tendermint_config['proxy_app'])
    tendermint_config['proxy_app'] = app_port
    tendermint_config['rpc']['laddr'] = util.input_default(
        'RPC Port', tendermint_config['rpc']['laddr'])
    tendermint_config['p2p']['laddr'] = util.input_default(
        'Tendermint Port', tendermint_config['p2p']['laddr'])
    tendermint_config['p2p']['addr_book_strict'] = util.input_default(
        'Strict address routability rules', tendermint_config['p2p']['addr_book_strict'], type=bool)

    # Save data back to config.toml
    with open(util.tendermint_home_directory + 'config/config.toml', "w") as f:
        toml.dump(tendermint_config, f)

    # Create/open config file for band
    band_home_dir = util.band_home_directory
    if Path(band_home_dir + 'config/config.toml').is_file():
        config_band = toml.load(band_home_dir + 'config/config.toml')
    else:
        Path(band_home_dir + 'config').mkdir(parents=True)
        config_band = {'port': app_port.split(':')[-1], 'db_path': 'data/'}

    config_band['port'] = app_port.split(':')[-1]
    config_band['db_path'] = util.input_default(
        'Band DB Path', config_band['db_path'])

    # Save data to band_home_dir/config/config.toml
    with open(band_home_dir + 'config/config.toml', "w") as f:
        toml.dump(config_band, f)
