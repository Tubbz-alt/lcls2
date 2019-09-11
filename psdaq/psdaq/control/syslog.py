#!/usr/bin/env python
"""
syslog client
"""
import argparse
import logging
from logging.handlers import SysLogHandler

class SysLog:

    def __init__(self, *, instrument, level):

        root = logging.getLogger()
        root.setLevel(level)
        root.handlers = []

        # syslog handler relies on syslog server to add timestamp
        syslog_handler = SysLogHandler('/dev/log')
        syslog_handler.setLevel(level)
        syslog_formatter = logging.Formatter(instrument + '-%(module)s[%(process)d]: %(message)s')
        syslog_handler.setFormatter(syslog_formatter)
        root.addHandler(syslog_handler)

        # console handler applies local timestamp
        console_handler = logging.StreamHandler(None)
        console_handler.setLevel(level)
        console_formatter = logging.Formatter('%(levelname)s %(asctime)s ' + instrument +
                                              '-%(module)s[%(process)d]: %(message)s')
        console_handler.setFormatter(console_formatter)
        root.addHandler(console_handler)

        self.syslog_handler = syslog_handler
        self.console_handler = console_handler

        #for h in root.handlers : print(str(h))

# ------------------------ self-test ----------------------------

def main():

    # Process arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', action='store_true', help='be verbose')
    args = parser.parse_args()

    # choose logging level
    if args.v:
        level=logging.DEBUG
    else:
        level=logging.WARNING

    # configure logging handlers
    logger = SysLog(instrument='TST', level=level)

    # log test messages
    logging.critical('this is a test critical message')
    logging.error('this is a test error message')
    logging.warning('this is a test warning message')
    logging.info('this is a test info message')
    logging.debug('this is a test debug message')

    return

if __name__ == '__main__':
    main()