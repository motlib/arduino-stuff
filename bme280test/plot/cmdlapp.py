'''Framework to simplify implementation of command-line applications.'''

__revision__ = '$Id: cmdlapp.py 61515 2017-08-04 03:23:52Z schroe03 $'
__docformat__ = 'ReStructuredText'
__author__ = 'Andreas Schroeder <an.schroeder@kostal.com>'


from argparse import ArgumentParser
import logging
import sys
import yaml


class ConfigWrapper():
    '''Load and provide access to a configuration file.'''

    def __init__(self, filename):
        '''Initialize the ConfigWrapper object by loading the config file.

        :param string filename: The filename of the YAML config file to load.'''

        self._load_config(filename)


    def _load_config(self, filename):
        '''Load a YAML configuration file with the specified name.

        :param string filename: The filename of the YAML config file to load.'''

        with open(filename, 'r') as f:
            self.cfg = yaml.load(f)

        self.filename = filename


    def get(self, key, default=None, raise_ex=False):
        '''Retrieve a configuration value by path.

        If an element in a dictionary does not exist, the default value is
        returned.

        :param string key: The path to a config item, separated by '/' like
          ``section/subsection/key``.
        :param default: The default value returned if the key is not available
          in the config file.
        :param bool raise_ex: If set to true, an exception is raised if the
          specified key is not available.

        :returns: The config value read from the config file or the default.
        '''

        pathlist = key.split('/')

        d = self.cfg

        for p in pathlist:
            if p in d.keys():
                d = d[p]
            else:
                if raise_ex == True:
                    msg = "Config key '{key}' is not available in file '{file}'."
                    raise KeyError(msg.format(
                        key=key,
                        file=self.filename))
                else:
                    return default

        return d


class CmdlApp():
    '''Base class for implementing command-line applications.'''

    def __init__(self):
        self.use_cfgfile = False
        self.main_fct = None
        self.tool_name = 'unknown_tool'
        self.tool_version = '0.0-dev'

        self._cfg_keys = [
            'main_fct',
            'use_cfgfile',
            'tool_name',
            'tool_version',
            'loglevel']


    def configure(self, **args):
        '''Configure the behavior of the command-line application.

        :param function-pointer main_fct: The function to run when the tool is
          started.
        :param bool use_cfgfile: If set to True, a '--config' parameter is added
          to the command-line options and the given file (YAML format) expected
          is loaded and provided as the cfg member variable.\
        '''

        for key in args.keys():
            if key not in self._cfg_keys:
                msg = "Unknown config parameter '{0}'."
                raise ValueError(msg.format(key))

            setattr(self, key, args[key])


    def setup_arg_parser(self):
        '''Set up the command line argument parser.

        Override this function to add additional command-line parameters by
        calling add_argument() on the self.parser member variable.
        '''

        self.parser = ArgumentParser()

        self.parser.add_argument('-v', '--verbose',
            help='Verbose output.',
            action="store_true",
            default=False)

        if self.use_cfgfile:
            self.parser.add_argument('-c', '--config',
                type=str,
                help='Configuration file',
                default='cfg.yaml',
                dest='config_file')


    def _parse_cmdline(self, args):
        '''Configure command-line parser and parse command-line.'''

        self.args = self.parser.parse_args(args)


    def _setup_logging(self):
        '''Configure the logging.

        The logging is used for all output of the tool, so everything is written
        to stdout.'''

        if self.args.verbose:
            level = logging.DEBUG
        else:
            level = logging.INFO

        logfmt = '%(asctime)s %(levelname)s: {toolname}: %(message)s'.format(
            toolname=self.tool_name)

        logging.basicConfig(
            level=level,
            format=logfmt,
            stream=sys.stdout)

        logging.info('{0} version {1}'.format(
            self.tool_name,
            self.tool_version))


    def load_config(self):
        '''Load the configuration file.'''

        try:
            self.cfg = ConfigWrapper(self.args.config_file)
        except FileNotFoundError:
            msg = "Cannot open config file '{0}'."
            logging.error(msg.format(self.args.config_file))
            sys.exit(1)


    def run(self, args=None):
        '''Initializes the tool and then starts the automation to execute all
        TESSY tests of the given project.'''

        self.setup_arg_parser()
        self._parse_cmdline(args=sys.argv[1:])

        self._setup_logging()

        if self.use_cfgfile:
            self.load_config()

        # Start the main function
        if self.main_fct != None:
            self.main_fct()
        else:
            msg = "The 'main_fct' config is unset. Cannot run application."
            raise ValueError(msg)
