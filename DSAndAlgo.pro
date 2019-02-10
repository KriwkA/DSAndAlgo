TEMPLATE = subdirs

SUBDIRS += \
    DSAndAlgo \
    tests/tests

tests.depends += DSAndAlgo
