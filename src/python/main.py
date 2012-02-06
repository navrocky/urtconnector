from PyQt4.Qt import *
import sip

def toQt(object, type):
    ptr = liburtapi.unwrap(object)
    return sip.wrapinstance(ptr, type)

def toUt(object, type):
    ptr = sip.unwrapinstance(object)
    return type.wrap(ptr)

def bookmark_changed(old_bm, new_bm):
    hello.write('old_bm:')
    hello.write(old_bm.name.toStdString())
    hello.write('new_bm:')
    hello.write(new_bm.name.toStdString())




