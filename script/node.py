import subprocess
import os
import toml
import time
import util


def node(debug):
    band_config = toml.load(
        util.band_home_directory + 'config/config.toml')

    if debug:
        band = subprocess.Popen(
            [util.band_bin, '-p', band_config['port'], '-v'])
    else:
        band = subprocess.Popen([util.band_bin, '--use-db', '--db-path',
                                 util.band_home_directory + band_config['db_path'], '-p', band_config['port']])

    tendermint = subprocess.Popen(
        [util.tendermint_bin, 'node', '--home', util.tendermint_home_directory])

    pid_band = f'band-pid-{band.pid}'
    with open(pid_band, 'w') as f:
        pass

    pid_tendermint = f'tm-pid-{tendermint.pid}'
    with open(pid_tendermint, 'w') as f:
        pass

    try:
        while True:
            time.sleep(0.1)
            if band.poll():
                tendermint.kill()
                os.remove(pid_band)
                os.remove(pid_tendermint)
                break
            if tendermint.poll():
                band.kill()
                os.remove(pid_band)
                os.remove(pid_tendermint)
                break

    except KeyboardInterrupt as e:
        band.kill()
        tendermint.kill()
        os.remove(pid_band)
        os.remove(pid_tendermint)
