import logging
import logging.handlers
import datetime
import time

def basic_test():
    # logging.basicConfig(level=logging.DEBUG)

    LOG_FORMAT = "%(pathname)s-%(lineno)s - %(asctime)s - %(levelname)s - %(message)s"
    DATE_FORMAT = "%m/%d/%Y %H:%M:%S %p"

    logging.basicConfig(filename='my.log', level=logging.DEBUG, format=LOG_FORMAT, datefmt=DATE_FORMAT)

    logging.debug("This is a debug log.")
    logging.info("This is a info log.")
    logging.warning("This is a warning log.")
    logging.error("This is a error log.")
    logging.critical("This is a critical log.")

def upgrade_test():
    logger = logging.getLogger('mylogger')
    logger.setLevel(logging.DEBUG)

    rf_handler = logging.handlers.TimedRotatingFileHandler('all.log', when='midnight', interval=1, backupCount=7, atTime=datetime.time(0, 0, 0, 0))
    rf_handler.setFormatter(logging.Formatter("%(asctime)s - %(levelname)s - %(message)s"))

    f_handler = logging.FileHandler('error.log')
    f_handler.setLevel(logging.ERROR)
    f_handler.setFormatter(logging.Formatter("%(asctime)s - %(levelname)s - %(filename)s[:%(lineno)d] - %(message)s"))

    logger.addHandler(rf_handler)
    logger.addHandler(f_handler)

    logger.debug('debug message')
    logger.info('info message')
    logger.warning('warning message')
    logger.error('error message')
    logger.critical('critical message')

def produce_test():
    LOG_FORMAT = "%(pathname)s-%(lineno)s - %(asctime)s - %(levelname)s - %(message)s"
    DATE_FORMAT = "%m/%d/%Y %H:%M:%S"    
    logging.basicConfig(level=logging.DEBUG, format=LOG_FORMAT, datefmt=DATE_FORMAT)

    logger = logging.getLogger('user_logger')
    logger.setLevel(logging.DEBUG)

    # detail_handler = logging.handlers.TimedRotatingFileHandler('log/all.log', when='midnight', interval=1, backupCount=7, atTime=datetime.time(0, 0, 0, 0))

    detail_handler = logging.handlers.RotatingFileHandler('log/all.log', maxBytes=8*1024*1024*500, backupCount=5)
    
    detail_handler.setFormatter(logging.Formatter("%(asctime)s - %(levelname)s - %(message)s"))

    # error_handler = logging.FileHandler('log/error.log')

    error_handler = logging.handlers.RotatingFileHandler('log/error.log', maxBytes=8*1024*1024*500, backupCount=5)

    error_handler.setLevel(logging.WARNING)
    error_handler.setFormatter(logging.Formatter("%(asctime)s - %(levelname)s - %(filename)s[:%(lineno)d] - %(message)s"))

    logger.addHandler(detail_handler)
    logger.addHandler(error_handler)

    logger.debug('debug message %s' % (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())))
    logger.info('info message %s' % (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())))
    logger.warning('warning message %s' % (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())))
    logger.error('error message %s' % (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())))
    logger.critical('critical message %s' % (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())))        

class Logger(object):
    def __init__(self):
        LOG_FORMAT = "%(pathname)s-%(lineno)s - %(asctime)s - %(levelname)s - %(message)s"
        DATE_FORMAT = "%Y/%m/%d %H:%M:%S"    
        logging.basicConfig(level=logging.DEBUG, format=LOG_FORMAT, datefmt=DATE_FORMAT)

        self._logger = logging.getLogger('user_logger')
        self._logger.setLevel(logging.DEBUG)

        # detail_handler = logging.handlers.TimedRotatingFileHandler('log/all.log', when='midnight', interval=1, backupCount=7, atTime=datetime.time(0, 0, 0, 0))

        detail_handler = logging.handlers.RotatingFileHandler('log/all.log', maxBytes=8*1024*1024*500, backupCount=3)
        
        detail_handler.setFormatter(logging.Formatter("%(asctime)s - %(levelname)s - %(message)s"))

        # error_handler = logging.FileHandler('log/error.log')

        error_handler = logging.handlers.RotatingFileHandler('log/error.log', maxBytes=8*1024*1024*500, backupCount=3)

        error_handler.setLevel(logging.WARNING)
        error_handler.setFormatter(logging.Formatter(fmt="%(asctime)s - %(levelname)s - %(filename)s[:%(lineno)d] - %(message)s"))

        self._logger.addHandler(detail_handler)
        self._logger.addHandler(error_handler)        

    def Debug(self, info):
        self._logger.debug(info)

    def Info(self, info):
        self._logger.info(info)

    def Warning(self, info):
        self._logger.warning(info)

    def Error(self, info):
        self._logger.error(info)

    def Critical(self, info):
        self._logger.critical(info)          

def test_logger():
    logger = Logger();  

    logger.Debug("test debug %s" % (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())))
    logger.Info("test info %s" % (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())))
    logger.Warning("test warning %s" % (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())))
    logger.Error("test error %s" % (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())))
    logger.Critical("test critical %s" % (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())))

def main():
    # basic_test()
    # upgrade_test()
    # produce_test()
    test_logger()

if __name__ == '__main__':
    main()