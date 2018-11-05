import os


tendermint_home_directory = os.getenv('TM_HOME_DIR', '~/.tendermint')
tendermint_bin = os.getenv('TM_BIN', '/bin/tendermint')
band_home_directory = os.getenv('BAND_HOME_DIR', '~/.band')
band_bin = os.getenv('BAND_BIN', '/bin/band')


def input_default(prompt, default_value='', type=str):
    assert isinstance(default_value, type)
    val = input(f'{prompt} ({default_value}): ')
    if val == '':
        return default_value

    if type == str:
        return val
    elif type == int:
        return int(val)
    elif type == bool:
        if val.lower() in ['t', 'y', 'true', 'yes']:
            return True
        else:
            return False
    else:
        raise TypeError("Does not support this type")
