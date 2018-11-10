import os


tendermint_home_directory = os.getenv('TM_HOME_DIR', '~/.tendermint')
tendermint_bin = os.getenv('TM_BIN', '/bin/tendermint')
band_home_directory = os.getenv('BAND_HOME_DIR', '~/.band')
band_bin = os.getenv('BAND_BIN', '/bin/band')


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


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


def colored_msg(msg, t=0):
    if t == 1:
        return bcolors.OKBLUE + msg + bcolors.ENDC
    elif t == 2:
        return bcolors.OKGREEN + msg + bcolors.ENDC
    else:
        return msg
